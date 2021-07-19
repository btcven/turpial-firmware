/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * Copyright (C) 2021 btcven and Locha Mesh developers
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_aodvv2
 * @{
 *
 * @file
 * @brief       RFC5444 server implementation
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Locha Mesh developers <contact@locha.io>
 * @}
 */

#include "net/aodvv2.h"
#include "net/aodvv2/rfc5444.h"
#include "net/aodvv2/lrs.h"
#include "net/aodvv2/mcmsg.h"
#include "net/aodvv2/metric.h"
#include "net/aodvv2/rcs.h"
#include "net/aodvv2/seqnum.h"

#include "net/gnrc/ipv6.h"
#include "net/gnrc/udp.h"
#include "net/gnrc/netif/hdr.h"

#include "mutex.h"

#include "aodvv2_reader.h"
#include "aodvv2_writer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#if ENABLE_DEBUG == 1
#include "rfc5444/rfc5444_print.h"
#endif

/**
 * @brief   Save the kernel PID for later reference
 */
static kernel_pid_t _pid = KERNEL_PID_UNDEF;

#if ENABLE_DEBUG == 1
static char _stack[CONFIG_AODVV2_RFC5444_STACK_SIZE + THREAD_EXTRA_STACKSIZE_PRINTF];
#else
static char _stack[CONFIG_AODVV2_RFC5444_STACK_SIZE];
#endif

/**
 * @brief   Network interface to use
 */
static gnrc_netif_t *_netif;

/**
 * @brief   Netreg
 */
static gnrc_netreg_entry_t netreg = GNRC_NETREG_ENTRY_INIT_PID(GNRC_NETREG_DEMUX_CTX_ALL,
                                                               KERNEL_PID_UNDEF);

/**
 * @brief   The RFC5444 packet reader context
 */
static struct rfc5444_reader _reader;
static mutex_t _reader_lock;

/**
 * @brief   The RFC5444 packet writer context
 */
static struct rfc5444_writer _writer;
static aodvv2_writer_target_t _writer_context;
static uint8_t _writer_msg_buffer[CONFIG_AODVV2_RFC5444_PACKET_SIZE];
static uint8_t _writer_msg_addrtlvs[CONFIG_AODVV2_RFC5444_ADDR_TLVS_SIZE];
static uint8_t _writer_pkt_buffer[CONFIG_AODVV2_RFC5444_PACKET_SIZE];
static mutex_t _writer_lock;

static void _route_info(unsigned type, const ipv6_addr_t *ctx_addr,
                        const void *ctx)
{
    switch (type) {
        case GNRC_IPV6_NIB_ROUTE_INFO_TYPE_UNDEF:
            DEBUG("aodvv2: GNRC_IPV6_NIB_ROUTE_INFO_TYPE_UNDEF\n");
            break;

        case GNRC_IPV6_NIB_ROUTE_INFO_TYPE_RRQ:
            DEBUG("aodvv2: GNRC_IPV6_NIB_ROUTE_INFO_TYPE_RRQ\n");
            {
                gnrc_pktsnip_t *pkt = (gnrc_pktsnip_t *)ctx;
                ipv6_hdr_t *ipv6_hdr = gnrc_ipv6_get_header(pkt);

                if (aodvv2_rcs_is_client(&ipv6_hdr->src) != NULL) {
                    if (aodvv2_buffer_pkt_add(ctx_addr, pkt) == 0) {
                        DEBUG("aodvv2: finding route\n");
                        aodvv2_find_route(&ipv6_hdr->src, ctx_addr);
                    }
                    else {
                        DEBUG("aodvv2: couldn't buffer packet!\n");
                    }
                }
                else {
                    DEBUG("aodvv2: src is not our client!\n");
                }
            }
            break;

        case GNRC_IPV6_NIB_ROUTE_INFO_TYPE_RN:
            DEBUG("aodvv2: GNRC_IPV6_NIB_ROUTE_INFO_TYPE_RN\n");
            break;

        case GNRC_IPV6_NIB_ROUTE_INFO_TYPE_NSC:
            DEBUG("aodvv2: GNRC_IPV6_NIB_ROUTE_INFO_TYPE_NSC\n");
            break;

        default:
            DEBUG("aodvv2: unknown route info!\n");
            break;
    }
}

static void _send_rreq(aodvv2_message_t *message, ipv6_addr_t *next_hop)
{
    assert(message != NULL);
    assert(next_hop != NULL);

    /* Make sure no other thread is using the writer right now */
    mutex_lock(&_writer_lock);
    _writer_context.target_addr = *next_hop;

    aodvv2_writer_send_rreq(&_writer, message);

    rfc5444_writer_flush(&_writer, &_writer_context.target, false);
    mutex_unlock(&_writer_lock);
}

static void _send_rrep(aodvv2_message_t *message, ipv6_addr_t *next_hop)
{
    assert(message != NULL);
    assert(next_hop != NULL);

    /* Make sure no other thread is using the writer right now */
    mutex_lock(&_writer_lock);
    _writer_context.target_addr = *next_hop;

    aodvv2_writer_send_rrep(&_writer, message);

    rfc5444_writer_flush(&_writer, &_writer_context.target, false);
    mutex_unlock(&_writer_lock);
}

static void _send_packet(struct rfc5444_writer *writer,
                         struct rfc5444_writer_target *iface, void *buffer,
                         size_t length)
{
#ifdef DEVELHELP
    assert(writer != NULL && iface != NULL && buffer != NULL && length != 0);
#else
    (void)writer;
#endif

#if ENABLE_DEBUG == 1
    static struct autobuf hexbuf;

    /* Generate hexdump of packet */
    abuf_hexdump(&hexbuf, "\t", buffer, length);
    rfc5444_print_direct(&hexbuf, buffer, length);

    /* Print hexdump to console */
    DEBUG("%s", abuf_getptr(&hexbuf));

    abuf_free(&hexbuf);
#endif

    aodvv2_writer_target_t *ctx = container_of(iface, aodvv2_writer_target_t,
                                               target);

    gnrc_pktsnip_t *payload;
    gnrc_pktsnip_t *udp;
    gnrc_pktsnip_t *ip;

    /* Generate our pktsnip with our RFC5444 message */
    payload = gnrc_pktbuf_add(NULL, buffer, length, GNRC_NETTYPE_UNDEF);
    if (payload == NULL) {
        DEBUG("aodvv2: couldn't allocate payload\n");
        return;
    }

    /* Build UDP packet */
    uint16_t port = UDP_MANET_PORT;
    udp = gnrc_udp_hdr_build(payload, port, port);
    if (udp == NULL) {
        DEBUG("aodvv2: unable to allocate UDP header\n");
        gnrc_pktbuf_release(payload);
        return;
    }

    /* Build IPv6 header */
    ip = gnrc_ipv6_hdr_build(udp, NULL, &ctx->target_addr);
    if (ip == NULL) {
        DEBUG("aodvv2: unable to allocate IPv6 header\n");
        gnrc_pktbuf_release(udp);
        return;
    }

    /* Build netif header */
    gnrc_pktsnip_t *netif_hdr = gnrc_netif_hdr_build(NULL, 0, NULL, 0);
    gnrc_netif_hdr_set_netif(netif_hdr->data, _netif);
    LL_PREPEND(ip, netif_hdr);

    /* Send packet */
    int res = gnrc_netapi_dispatch_send(GNRC_NETTYPE_UDP,
                                        GNRC_NETREG_DEMUX_CTX_ALL, ip);
    if (res < 1) {
        DEBUG("aodvv2: unable to locate UDP thread\n");
        gnrc_pktbuf_release(ip);
        return;
    }
}

static void _receive(gnrc_pktsnip_t *pkt)
{
    assert(pkt != NULL && pkt->data != NULL && pkt->size > 0);

#if ENABLE_DEBUG == 1
    static struct autobuf hexbuf;

    /* Generate hexdump of packet */
    abuf_hexdump(&hexbuf, "\t", pkt->data, pkt->size);
    rfc5444_print_direct(&hexbuf, pkt->data, pkt->size);

    /* Print hexdump to console */
    DEBUG("%s", abuf_getptr(&hexbuf));

    abuf_free(&hexbuf);
#endif

    /* Find sender address on IPv6 header */
    ipv6_addr_t sender;
    ipv6_hdr_t *ipv6_hdr = gnrc_ipv6_get_header(pkt);
    assert(ipv6_hdr != NULL);
    memcpy(&sender, &ipv6_hdr->src, sizeof(ipv6_addr_t));

    mutex_lock(&_reader_lock);
    aodvv2_rfc5444_handle_packet_prepare(&sender);
    if (rfc5444_reader_handle_packet(&_reader, pkt->data, pkt->size) != RFC5444_OKAY) {
        DEBUG("aodvv2: couldn't handle packet!\n");
    }
    mutex_unlock(&_reader_lock);

    gnrc_pktbuf_release(pkt);
}

static void *_event_loop(void *arg)
{
    (void)arg;
    msg_t msg;
    msg_t reply;
    msg_t msg_queue[CONFIG_AODVV2_RFC5444_MSG_QUEUE_SIZE];

    /* Initialize message queue */
    msg_init_queue(msg_queue, CONFIG_AODVV2_RFC5444_MSG_QUEUE_SIZE);

    reply.content.value = (uint32_t)(-ENOTSUP);
    reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

    while (1) {
        msg_receive(&msg);

        switch (msg.type) {
            case AODVV2_MSG_TYPE_SEND_RREQ:
                DEBUG("AODVV2_MSG_TYPE_SEND_RREQ\n");
                {
                    aodvv2_msg_t m;
                    memcpy(&m, (aodvv2_msg_t *)msg.content.ptr, sizeof(m));
                    free(msg.content.ptr);

                    _send_rreq(&m.pkt, &m.next_hop);
                }
                break;

            case AODVV2_MSG_TYPE_SEND_RREP:
                DEBUG("AODVV2_MSG_TYPE_SEND_RREP\n");
                {
                    aodvv2_msg_t m;
                    memcpy(&m, (aodvv2_msg_t *)msg.content.ptr, sizeof(m));
                    free(msg.content.ptr);

                    _send_rrep(&m.pkt, &m.next_hop);
                }
                break;

            case GNRC_NETAPI_MSG_TYPE_RCV:
                DEBUG("GNRC_NETAPI_MSG_TYPE_RCV\n");
                _receive((gnrc_pktsnip_t *)msg.content.ptr);
                break;

            case GNRC_NETAPI_MSG_TYPE_GET:
            case GNRC_NETAPI_MSG_TYPE_SET:
                msg_reply(&msg, &reply);
                break;

            default:
                DEBUG("aodvv2: received unidentified message\n");
                break;
        }
    }

    /* Never reached */
    return NULL;
}

int aodvv2_init(gnrc_netif_t *netif)
{
    assert(netif != NULL);

    /* Check if thread is already running */
    if (_pid != KERNEL_PID_UNDEF) {
        return _pid;
    }

    mutex_init(&_writer_lock);
    mutex_init(&_reader_lock);

    /* Start RFC5444 thread */
    _pid = thread_create(_stack, sizeof(_stack), CONFIG_AODVV2_RFC5444_PRIO,
                         THREAD_CREATE_STACKTEST, _event_loop, NULL,
                         "aodvv2");

    /* Verify thread is created */
    if (_pid < 0) {
        return _pid;
    }

    /* Save netif for later reference */
    _netif = netif;

    /* Initialize AODVv2 internal structures */
    aodvv2_seqnum_init();
    aodvv2_lrs_init();
    aodvv2_rcs_init();
    aodvv2_mcmsg_init();
    aodvv2_buffer_init();

    /* Register netreg */
    gnrc_netreg_entry_init_pid(&netreg, UDP_MANET_PORT, _pid);
    gnrc_netreg_register(GNRC_NETTYPE_UDP, &netreg);

    /* Initialize RFC5444 reader */
    mutex_lock(&_reader_lock);

    /* Initialize reader */
    rfc5444_reader_init(&_reader);

    /* Register AODVv2 messages reader */
    aodvv2_reader_init(&_reader, _netif->pid);

    mutex_unlock(&_reader_lock);

    /* Initialize RFC5444 writer */
    mutex_lock(&_writer_lock);

    _writer.msg_buffer = _writer_msg_buffer;
    _writer.msg_size = sizeof(_writer_msg_buffer);
    _writer.addrtlv_buffer = _writer_msg_addrtlvs;
    _writer.addrtlv_size = sizeof(_writer_msg_addrtlvs);

    /* Define target for generating rfc5444 packets */
    _writer_context.target.packet_buffer = _writer_pkt_buffer;
    _writer_context.target.packet_size = sizeof(_writer_pkt_buffer);

    /* Set function to send binary packet content */
    _writer_context.target.sendPacket = _send_packet;

    /* Initialize writer */
    rfc5444_writer_init(&_writer);

    /* Register a target (for sending messages to) in writer */
    rfc5444_writer_register_target(&_writer, &_writer_context.target);

    aodvv2_writer_init(&_writer);

    mutex_unlock(&_writer_lock);

    /* Install route info callback, this is called from the NIB when a route is
     * needed, this is what needs to be used for reactive protocols like AODVv2
     */
    _netif->ipv6.route_info_cb = _route_info;

    return _pid;
}

int aodvv2_send_rreq(aodvv2_message_t *pkt,
                     ipv6_addr_t *next_hop)
{
    aodvv2_msg_t *msg = malloc(sizeof(aodvv2_msg_t));
    if (msg == NULL) {
        DEBUG("aodvv2: out of memory!\n");
        return -1;
    }

    /* Set destination address */
    memcpy(&msg->next_hop, next_hop, sizeof(ipv6_addr_t));

    /* Copy RREQ packet */
    memcpy(&msg->pkt, pkt, sizeof(aodvv2_message_t));

    /* Prepare and send IPC message */
    msg_t ipc_msg;
    ipc_msg.content.ptr = msg;
    ipc_msg.type = AODVV2_MSG_TYPE_SEND_RREQ;

    if (msg_send(&ipc_msg, _pid) < 1) {
        DEBUG("aodvv2: couldn't send RREQ.\n");
        return -1;
    }

    return 0;
}

int aodvv2_send_rrep(aodvv2_message_t *pkt,
                     ipv6_addr_t *next_hop)
{
    aodvv2_msg_t *msg = malloc(sizeof(aodvv2_msg_t));
    if (msg == NULL) {
        DEBUG("aodvv2: out of memory!\n");
        return -1;
    }

    /* Set destination address */
    memcpy(&msg->next_hop, next_hop, sizeof(ipv6_addr_t));

    /* Copy RREQ packet */
    memcpy(&msg->pkt, pkt, sizeof(aodvv2_message_t));

    /* Prepare and send IPC message */
    msg_t ipc_msg;
    ipc_msg.content.ptr = msg;
    ipc_msg.type = AODVV2_MSG_TYPE_SEND_RREP;

    if (msg_send(&ipc_msg, _pid) < 1) {
        DEBUG("aodvv2: couldn't send RREQ.\n");
        return -1;
    }

    return 0;
}

int aodvv2_find_route(const ipv6_addr_t *orig_addr,
                      const ipv6_addr_t *target_addr)
{
    assert(orig_addr != NULL && target_addr != NULL);

    aodvv2_message_t pkt;

    /* Set metric information */
    pkt.msg_hop_limit = aodvv2_metric_max(METRIC_HOP_COUNT);
    pkt.metric_type = CONFIG_AODVV2_DEFAULT_METRIC;

    /* Set OrigNode information */
    aodvv2_rcs_entry_t *client;
    if ((client = aodvv2_rcs_is_client(orig_addr)) != NULL) {
        pkt.orig_node.addr = client->addr;
        pkt.orig_node.pfx_len = client->pfx_len;
    }
    else {
        DEBUG_PUTS("aodvv2: not a client");
        return -1;
    }

    pkt.orig_node.metric = 0;
    pkt.orig_node.seqnum = aodvv2_seqnum_get();
    aodvv2_seqnum_inc();

    /* Set TargNode information */
    pkt.targ_node.addr = *target_addr;
    pkt.targ_node.pfx_len = 128;
    pkt.targ_node.metric = 0;
    pkt.targ_node.seqnum = 0;

    /* Add RREQ to mcmsg */
    aodvv2_mcmsg_process(&pkt);

    return aodvv2_send_rreq(&pkt, &ipv6_addr_all_manet_routers_link_local);
}
