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

#define ENABLE_DEBUG (0)
#include "debug.h"

#define VAINA_ACK_TIMEOUT (1000 * US_PER_MS) /**< Wait time for an ACK */

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
        return VAINA_ERROR_INVALID_MSG;
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
            return VAINA_ERROR_INVALID_MSG;
    }

    return VAINA_OK;
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
            return VAINA_ERROR_INVALID_MSG;
    }

    return VAINA_OK;
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
            return VAINA_ERROR_INVALID_MSG;
    }

    /* not reached */
    return VAINA_ERROR_INVALID_MSG;
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
        DEBUG_PUTS("vaina: couldn't serialize message");
        return VAINA_ERROR_INVALID_MSG;
    }

    if (sock_udp_send(&_sock, buf, len, &_remote) < 0) {
        DEBUG_PUTS("vaina: can't send UDP datagram");
        return VAINA_ERROR_UDP;
    }

    memset(buf, 0, sizeof(buf));
    vaina_msg_t ack_msg;
    sock_udp_ep_t remote;
    int received = sock_udp_recv(&_sock, buf, sizeof(buf), VAINA_ACK_TIMEOUT,
                                 &remote);
    if (received == -ETIMEDOUT) {
        DEBUG_PUTS("vaina: expected ACK, timed out");
        return VAINA_ERROR_ACK_TIMEOUT;
    }

    if (received < 0) {
        DEBUG_PUTS("vaina: unexpected UDP error");
        return VAINA_ERROR_UDP;
    }

    if (remote.netif != _netif->pid && remote.netif != 0) {
        DEBUG_PUTS("vaina: packet is not for our netif");
        return VAINA_ERROR_FORBIDDEN;
    }

    if (_parse_msg(&ack_msg, buf, received) != VAINA_OK) {
        DEBUG_PUTS("vaina: couldn't parse message");
        return VAINA_ERROR_INVALID_MSG;
    }

    if (_process_msg(&ack_msg) != VAINA_OK) {
        DEBUG_PUTS("vaina: couldn't process message");
        return VAINA_ERROR_INVALID_MSG;
    }

    if (ack_msg.msg == VAINA_MSG_NACK) {
        return VAINA_ERROR_NACK;
    }

    return VAINA_OK;
}

int vaina_client_init(gnrc_netif_t *netif)
{
    assert(netif != NULL);

    _netif = netif;
    _seqno = 0;

    ipv6_addr_t group;
    assert(ipv6_addr_from_str(&group, CONFIG_VAINA_MCAST_ADDR) != NULL);

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
        return VAINA_ERROR_UDP;
    }

    return VAINA_OK;
}
