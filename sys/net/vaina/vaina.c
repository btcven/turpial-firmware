/**
 * @ingroup     net_vaina
 * @{
 *
 * @file
 * @brief       VAINA - Versatile Address Interface | Network Administration
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "net/vaina.h"

#include "net/sock/udp.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if ENABLE_DEBUG == 1
static char _stack[THREAD_STACKSIZE_DEFAULT + THREAD_EXTRA_STACKSIZE_PRINTF];
#else /* ENABLE_DEBUG */
static char _stack[THREAD_STACKSIZE_DEFAULT];
#endif /* ENABLE_DEBUG */

static kernel_pid_t _pid;    /**< thread PID */
static gnrc_netif_t *_netif; /**< netif where we send/receive packets */
static sock_udp_t _sock;     /**< UDP socket */
static int _pending_acks;    /**< number of pending ACKs */

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
                DEBUG("vaina: received [N]ACK, although, we didn't sent anything!\n");
            }
            DEBUG("vaina: received %s", msg->msg == VAINA_MSG_ACK ? "ACK" : "NACK");
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

kernel_pid_t vaina_client_init(gnrc_netif_t *netif)
{
    _netif = netif;

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
