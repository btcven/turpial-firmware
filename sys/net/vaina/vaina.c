/* Copyright 2019 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @ingroup     net_vaina
 *
 * @{
 * @file
 * @brief       VAINA - Versatile Address Interface | Network Administration
 *
 * @author      Locha Mesh Developers <developers@locha.io>
 * @}
 */

#include "net/vaina.h"

#include "net/sock/udp.h"

#define ENABLE_DEBUG (1)
#include "debug.h"

#if ENABLE_DEBUG == 1
static char _stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
#else /* ENABLE_DEBUG */
static char _stack[THREAD_STACKSIZE_DEFAULT];
#endif /* ENABLE_DEBUG */

static kernel_pid_t _pid;     /**< thread PID */
static gnrc_netif_t *_netif;  /**< netif where we send/receive packets */
static sock_udp_t _sock;      /**< UDP socket */
static sock_udp_ep_t _remote; /**< remote end point */
static int _pending_acks;     /**< number of pending ACKs */
static uint8_t _seqno;        /**< Current sequence number */

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
        case VAINA_MSG_ACK:
        case VAINA_MSG_NACK:
            vaina->msg = type;
            vaina->seqno = seqno;
            break;

        default:
            DEBUG("vaina: invalid message type\n");
            return -EINVAL;
    }

    return 0;
}

static int _process_msg(vaina_msg_t *msg)
{
    switch (msg->msg) {
        case VAINA_MSG_ACK:
        case VAINA_MSG_NACK:
            if (_pending_acks > 0) {
                _pending_acks -= 1;
            }
            else {
                DEBUG_PUTS("vaina: received [N]ACK, although, we didn't sent anything!\n");
            }
            DEBUG("vaina: received %s\n", msg->msg == VAINA_MSG_ACK ? "ACK" : "NACK");
            break;

        default:
            return -EINVAL;
    }

    return 0;
}

void *_event_loop(void *arg)
{
    uint8_t buf[UINT8_MAX];
    sock_udp_ep_t remote;
    vaina_msg_t msg;

    while (true) {
        int received = sock_udp_recv(&_sock, buf, sizeof(buf), SOCK_NO_TIMEOUT,
                                     &remote);
        if (remote.netif != _netif->pid && remote.netif != 0) {
            DEBUG("vaina: packet is not for out netif!\n");
            continue;
        }

        if (received < 0) {
            continue;
        }

        if (_parse_msg(&msg, buf, received) < 0) {
            DEBUG("vaina: couldn't parse message!\n");
            continue;
        }

        if (_process_msg(&msg) < 0) {
            DEBUG("vaina: couldn't process message!\n");
            continue;
        }
    }

    return NULL;
}

static int _serialize_msg(vaina_msg_t *msg, uint8_t *buf)
{
    int i = 0;

    buf[i] = msg->msg;
    buf[i + 1] = msg->seqno;
    i += 2;
    switch (msg->msg) {
        case VAINA_MSG_RCS_ADD:
            buf[i] = msg->payload.rcs_add.pfx_len;
            i += 1;
            memcpy(&buf[i], &msg->payload.rcs_add.ip, sizeof(ipv6_addr_t));
            i += sizeof(ipv6_addr_t);
            return i;

        case VAINA_MSG_RCS_DEL:
            buf[i] = msg->payload.rcs_del.pfx_len;
            i += 1;
            memcpy(&buf[i], &msg->payload.rcs_del.ip, sizeof(ipv6_addr_t));
            i += sizeof(ipv6_addr_t);
            return i;

        case VAINA_MSG_NIB_ADD:
            buf[i] = msg->payload.nib_add.pfx_len;
            i += 1;
            memcpy(&buf[i], &msg->payload.nib_del.ip, sizeof(ipv6_addr_t));
            i += sizeof(ipv6_addr_t);
            return i;

        case VAINA_MSG_NIB_DEL:
            buf[i] = msg->payload.nib_del.pfx_len;
            i += 1;
            memcpy(&buf[i], &msg->payload.nib_del.ip, sizeof(ipv6_addr_t));
            i += sizeof(ipv6_addr_t);
            return i;

        default:
            DEBUG("vaina: invalid message type!\n");
            return -1;
    }

    /* not reached */
    return -1;
}

uint8_t vaina_seqno(void)
{
    if (_seqno + 1 == 255) {
        _seqno = 0;
    }
    else {
        _seqno += 1;
    }

    return _seqno;
}

int vaina_client_send(vaina_msg_t *msg)
{
    uint8_t buf[UINT8_MAX];

    _pending_acks += 1;

    int len = _serialize_msg(msg, buf);
    if (len < 0) {
        DEBUG("vaina: couldn't serialize message!\n");
        return -1;
    }

    return sock_udp_send(&_sock, buf, len, &_remote);
}

kernel_pid_t vaina_client_init(gnrc_netif_t *netif)
{
    _netif = netif;
    _seqno = 0;

    ipv6_addr_t group;
    if (ipv6_addr_from_str(&group, CONFIG_VAINA_MCAST_ADDR) == NULL) {
        DEBUG_PUTS("vaina: invalid IPv6 group address");
        return -EINVAL;
    }

    /* Initialize remote end-point */
    _remote.family = AF_INET6;
    _remote.netif = _netif->pid;
    _remote.port = CONFIG_VAINA_PORT;
    memcpy(&_remote.addr, &group, sizeof(ipv6_addr_t));

    sock_udp_ep_t local = {
        .family = AF_INET6,
        .netif = netif->pid,
        .port = CONFIG_VAINA_PORT,
    };
    if (sock_udp_create(&_sock, &local, NULL, 0) < 0) {
        DEBUG_PUTS("vaina: couldn't create UDP socket");
        return -1;
    }

    _pid = thread_create(_stack, sizeof(_stack), THREAD_PRIORITY_MAIN - 1,
                         THREAD_CREATE_STACKTEST, _event_loop, NULL, "vaina");

    if (_pid < 0) {
        DEBUG("vaina: couldn't create thread.\n");
        return -1;
    }

    return _pid;
}
