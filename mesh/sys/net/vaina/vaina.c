/*
 * Copyright (C) 2021 btcven and Locha Mesh developers
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_vaina
 * @{
 *
 * @file
 * @brief       VAINA - Versatile Address Interface | Network Administration
 *
 * @author      Locha Mesh developers <developers@locha.io>
 * @}
 */

#include "net/vaina.h"

#include "net/sock/udp.h"
#include "net/gnrc/ipv6/nib/ft.h"

#if IS_USED(MODULE_AODVV2)
#include "net/aodvv2/rcs.h"
#endif

#define ENABLE_DEBUG (0)
#include "debug.h"

#if ENABLE_DEBUG == 1
static char _stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[THREAD_STACKSIZE_DEFAULT];
#endif

/**
 * @brief   UDP socket
 */
static sock_udp_t _sock;

/**
 * @brief   GNRC netif
 */
static gnrc_netif_t *_netif;

/**
 * @brief   UDP local endpoint
 */
static sock_udp_ep_t _local;

static int _parse_msg(vaina_msg_t *vaina, uint8_t *buf, size_t len)
{
    memset(vaina, 0, sizeof(vaina_msg_t));

    if (len < 2) {
        DEBUG_PUTS("vaina: invalid message size!");
        return -EINVAL;
    }

    uint8_t type = buf[0];
    uint8_t seqno = buf[1];

    switch (type) {
#if IS_USED(MODULE_AODVV2)
        case VAINA_MSG_RCS_ADD:
        case VAINA_MSG_RCS_DEL:
            if (len < (2 + 1 + sizeof(ipv6_addr_t))) {
                return -EINVAL;
            }
            vaina->msg = type;
            vaina->seqno = seqno;
            if (vaina->msg == VAINA_MSG_RCS_ADD) {
                vaina->payload.rcs_add.pfx_len = buf[2];
                memcpy(&vaina->payload.rcs_add.ip, &buf[3], sizeof(ipv6_addr_t));
            }
            else {
                vaina->payload.rcs_del.pfx_len = buf[2];
                memcpy(&vaina->payload.rcs_del.ip, &buf[3], sizeof(ipv6_addr_t));
            }
            break;
#endif

        case VAINA_MSG_NIB_ADD:
        case VAINA_MSG_NIB_DEL:
            if (len < (2 + 1 + sizeof(ipv6_addr_t))) {
                return -EINVAL;
            }
            vaina->msg = type;
            vaina->seqno = seqno;
            if (vaina->msg == VAINA_MSG_RCS_ADD) {
                vaina->payload.nib_add.pfx_len = buf[2];
                memcpy(&vaina->payload.nib_add.ip, &buf[3], sizeof(ipv6_addr_t));
            }
            else {
                vaina->payload.nib_del.pfx_len = buf[2];
                memcpy(&vaina->payload.nib_del.ip, &buf[3], sizeof(ipv6_addr_t));
            }
            break;

        default:
            DEBUG_PUTS("vaina: invalid message type");
            return -EINVAL;
    }

    return 0;
}

static int _process_msg(vaina_msg_t *msg)
{
    switch (msg->msg) {
#if IS_USED(MODULE_AODVV2)
        case VAINA_MSG_RCS_ADD:
            DEBUG_PUTS("vaina: adding new client");
            if (aodvv2_rcs_add(&msg->payload.rcs_add.ip,
                               msg->payload.rcs_add.pfx_len, 1) == NULL) {
                DEBUG_PUTS("vaina: client set is full");
                return -ENOSPC;
            }
            break;

        case VAINA_MSG_RCS_DEL:
            aodvv2_rcs_del(&msg->payload.rcs_del.ip, msg->payload.rcs_del.pfx_len);
            break;
#endif

        case VAINA_MSG_NIB_ADD: {
            DEBUG_PUTS("vaina: adding NIB entry");
            ipv6_addr_t next_hop = {
                .u8 = { 0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 }
            };
            if (gnrc_ipv6_nib_ft_add(&msg->payload.nib_add.ip,
                                     msg->payload.nib_add.pfx_len,
                                     &next_hop, _netif->pid, 0) < 0) {
                return -ENOMEM;
            }
            break;
        }

        case VAINA_MSG_NIB_DEL:
            gnrc_ipv6_nib_ft_del(&msg->payload.nib_del.ip,
                                 msg->payload.nib_del.pfx_len);
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

static int _send_ack(vaina_msg_t *msg, sock_udp_ep_t *remote, bool good_ack)
{
    DEBUG("vaina: sending %s\n", good_ack ? "ack" : "nack");
    uint8_t buf[2];
    if (good_ack) {
        buf[0] = VAINA_MSG_ACK;
    }
    else {
        buf[0] = VAINA_MSG_NACK;
    }
    buf[1] = msg->seqno;

    return sock_udp_send(&_sock, buf, sizeof(buf), remote);
}

static void *_vaina_thread(void *arg)
{
    (void) arg;
    uint8_t buf[UINT8_MAX];
    sock_udp_ep_t remote;
    vaina_msg_t msg;

    while (true) {
        int received = sock_udp_recv(&_sock, buf, sizeof(buf), SOCK_NO_TIMEOUT,
                                     &remote);

        DEBUG("vaina: received new packet\n");

        /* TODO: why remote.netif is equal to 0 when sending a receiving a
         * packet from SLIP
         */
        if (remote.netif != _netif->pid && remote.netif != 0) {
            DEBUG("vaina: not from our netif: %d\n", remote.netif);
            continue;
        }

        if (received < 0) {
            DEBUG_PUTS("vaina: couldn't receive packet");
            continue;
        }

        if (received == 0) {
            DEBUG_PUTS("vaina: packet doesn't have a payload, dropping");
            continue;
        }

        if (_parse_msg(&msg, buf, received) < 0) {
            DEBUG_PUTS("vaina: couldn't parse received message.");
            continue;
        }

        bool good_ack = true;
        if (_process_msg(&msg) < 0) {
            DEBUG_PUTS("vaina: couldn't process message.");
            good_ack = false;
        }

        if (_send_ack(&msg, &remote, good_ack) < 0) {
            DEBUG_PUTS("vaina: couldn't send the ACK!");
        }
    }

    /* Never reached */
    return NULL;
}

int vaina_init(gnrc_netif_t *netif)
{
    assert(netif != NULL);

    ipv6_addr_t group;
    if (ipv6_addr_from_str(&group, CONFIG_VAINA_MCAST_ADDR) == NULL) {
        DEBUG_PUTS("vaina: invalid IPv6 group address");
        return -EINVAL;
    }

    if (gnrc_netif_ipv6_group_join(netif, &group) < 0) {
        DEBUG_PUTS("vaina: coudln't joint VAINA IPv6 group");
        return -1;
    }

    _local.family = AF_INET6;
    memcpy(&_local.addr, &group, sizeof(ipv6_addr_t));
    _local.netif = netif->pid;
    _local.port = CONFIG_VAINA_PORT;
    if (sock_udp_create(&_sock, &_local, NULL, 0) < 0) {
        DEBUG_PUTS("vaina: couldn't create UDP socket");
        return -1;
    }

    _netif = netif;

    return thread_create(_stack, sizeof(_stack), THREAD_PRIORITY_MAIN + 2,
                         THREAD_CREATE_STACKTEST, _vaina_thread, NULL, "vaina");
}
