/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * Copyright (C) 2021 btcven and Locha Mesh developers <contact@locha.io>
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
 * @brief       RFC5444 RREP
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Locha Mesh devs <contact@locha.io>
 * @}
 */

#include "aodvv2_reader.h"
#include "net/aodvv2.h"
#include "net/aodvv2/lrs.h"
#include "net/aodvv2/mcmsg.h"
#include "net/aodvv2/metric.h"
#include "net/aodvv2/rcs.h"
#include "net/aodvv2/rfc5444.h"
#include "net/manet.h"

#include "net/gnrc/ipv6/nib/ft.h"

#include "xtimer.h"

#include "rfc5444_compat.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define AODVV2_ROUTE_LIFETIME \
    (CONFIG_AODVV2_ACTIVE_INTERVAL + CONFIG_AODVV2_MAX_IDLETIME)

static enum rfc5444_result _cb_rrep_blocktlv_addresstlvs_okay(
    struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _cb_rrep_blocktlv_messagetlvs_okay(
    struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _cb_rrep_end_callback(
    struct rfc5444_reader_tlvblock_context *cont, bool dropped);

static enum rfc5444_result _cb_rreq_blocktlv_addresstlvs_okay(
    struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _cb_rreq_blocktlv_messagetlvs_okay(
    struct rfc5444_reader_tlvblock_context *cont);
static enum rfc5444_result _cb_rreq_end_callback(
    struct rfc5444_reader_tlvblock_context *cont, bool dropped);

/*
 * Message consumer, will be called once for every message of
 * type RFC5444_MSGTYPE_RREP that contains all the mandatory message TLVs
 */
static struct rfc5444_reader_tlvblock_consumer _rrep_consumer =
{
    .msg_id = RFC5444_MSGTYPE_RREP,
    .block_callback = _cb_rrep_blocktlv_messagetlvs_okay,
    .end_callback = _cb_rrep_end_callback,
};

/*
 * Address consumer. Will be called once for every address in a message of
 * type RFC5444_MSGTYPE_RREP.
 */
static struct rfc5444_reader_tlvblock_consumer _rrep_address_consumer =
{
    .msg_id = RFC5444_MSGTYPE_RREP,
    .addrblock_consumer = true,
    .block_callback = _cb_rrep_blocktlv_addresstlvs_okay,
};

/*
 * Message consumer, will be called once for every message of
 * type RFC5444_MSGTYPE_RREQ that contains all the mandatory message TLVs
 */
static struct rfc5444_reader_tlvblock_consumer _rreq_consumer =
{
    .msg_id = RFC5444_MSGTYPE_RREQ,
    .block_callback = _cb_rreq_blocktlv_messagetlvs_okay,
    .end_callback = _cb_rreq_end_callback,
};

/*
 * Address consumer. Will be called once for every address in a message of
 * type RFC5444_MSGTYPE_RREQ.
 */
static struct rfc5444_reader_tlvblock_consumer _rreq_address_consumer =
{
    .msg_id = RFC5444_MSGTYPE_RREQ,
    .addrblock_consumer = true,
    .block_callback = _cb_rreq_blocktlv_addresstlvs_okay,
};

/*
 * Address consumer entries definition
 * TLV types RFC5444_MSGTLV__SEQNUM and RFC5444_MSGTLV_METRIC
 */
static struct rfc5444_reader_tlvblock_consumer_entry _address_consumer_entries[] =
{
    [RFC5444_MSGTLV_ORIGSEQNUM] = { .type = RFC5444_MSGTLV_ORIGSEQNUM },
    [RFC5444_MSGTLV_TARGSEQNUM] = { .type = RFC5444_MSGTLV_TARGSEQNUM },
    [RFC5444_MSGTLV_METRIC] = { .type = RFC5444_MSGTLV_METRIC }
};

static struct netaddr_str nbuf;
static aodvv2_message_t _msg_data;

static kernel_pid_t _netif_pid = KERNEL_PID_UNDEF;

static enum rfc5444_result _cb_rrep_blocktlv_messagetlvs_okay(
        struct rfc5444_reader_tlvblock_context *cont)
{
    if (!cont->has_hoplimit) {
        DEBUG_PUTS("aodvv2: missing hop limit");
        return RFC5444_DROP_PACKET;
    }

    _msg_data.msg_hop_limit = cont->hoplimit;
    if (_msg_data.msg_hop_limit == 0) {
        DEBUG_PUTS("aodvv2: hop limit is 0");
        return RFC5444_DROP_PACKET;
    }

    _msg_data.msg_hop_limit--;
    return RFC5444_OKAY;
}

static enum rfc5444_result _cb_rrep_blocktlv_addresstlvs_okay(
        struct rfc5444_reader_tlvblock_context *cont)
{
    struct rfc5444_reader_tlvblock_entry *tlv;
    bool is_targ_node_addr = false;

    DEBUG("aodvv2: %s\n", netaddr_to_string(&nbuf, &cont->addr));

    /* handle TargNode SeqNum TLV */
    tlv = _address_consumer_entries[RFC5444_MSGTLV_TARGSEQNUM].tlv;
    if (tlv) {
        DEBUG("aodvv2: RFC5444_MSGTLV_TARGSEQNUM: %d\n", *tlv->single_value);
        is_targ_node_addr = true;
        netaddr_to_ipv6_addr(&cont->addr, &_msg_data.targ_node.addr,
                             &_msg_data.targ_node.pfx_len);
        _msg_data.targ_node.seqnum = *tlv->single_value;
    }

    /* handle OrigNode SeqNum TLV */
    tlv = _address_consumer_entries[RFC5444_MSGTLV_ORIGSEQNUM].tlv;
    if (tlv) {
        DEBUG("aodvv2: RFC5444_MSGTLV_ORIGSEQNUM: %d\n", *tlv->single_value);
        is_targ_node_addr = false;
        netaddr_to_ipv6_addr(&cont->addr, &_msg_data.orig_node.addr,
                             &_msg_data.targ_node.pfx_len);
        _msg_data.orig_node.seqnum = *tlv->single_value;
    }

    if (!tlv && !is_targ_node_addr) {
        DEBUG_PUTS("aodvv2: mandatory SeqNum TLV missing!");
        return RFC5444_DROP_PACKET;
    }

    tlv = _address_consumer_entries[RFC5444_MSGTLV_METRIC].tlv;
    if (!tlv && is_targ_node_addr) {
        DEBUG_PUTS("aodvv2: missing or unknown metric TLV!");
        return RFC5444_DROP_PACKET;
    }

    if (tlv) {
        if (!is_targ_node_addr) {
            DEBUG_PUTS("aodvv2: metric TLV belongs to wrong address!");
            return RFC5444_DROP_PACKET;
        }

        DEBUG("aodvv2: RFC5444_MSGTLV_METRIC val: %d, exttype: %d\n",
              *tlv->single_value, tlv->type_ext);

        _msg_data.metric_type = tlv->type_ext;
        _msg_data.orig_node.metric = *tlv->single_value;
    }

    return RFC5444_OKAY;
}

static enum rfc5444_result _cb_rrep_end_callback(
        struct rfc5444_reader_tlvblock_context *cont, bool dropped)
{
    (void)cont;

    /* Check if packet contains the required information */
    if (dropped) {
        DEBUG_PUTS("aodvv2: dropping packet");
        return RFC5444_DROP_PACKET;
    }

    if (ipv6_addr_is_unspecified(&_msg_data.orig_node.addr) ||
        _msg_data.orig_node.seqnum == 0) {
        DEBUG_PUTS("aodvv2: missing OrigNode Address or SeqNum");
        return RFC5444_DROP_PACKET;
    }

    if (ipv6_addr_is_unspecified(&_msg_data.targ_node.addr) ||
        _msg_data.targ_node.seqnum == 0) {
        DEBUG_PUTS("aodvv2: missing TargNode Address or SeqNum");
        return RFC5444_DROP_PACKET;
    }

    uint8_t link_cost = aodvv2_metric_link_cost(_msg_data.metric_type);

    if ((aodvv2_metric_max(_msg_data.metric_type) - link_cost) <=
        _msg_data.targ_node.metric) {
        DEBUG_PUTS("aodvv2: metric limit reached");
        return RFC5444_DROP_PACKET;
    }

    aodvv2_metric_update(_msg_data.metric_type, &_msg_data.targ_node.metric);

    /* Update packet timestamp */
    timex_t now;
    xtimer_now_timex(&now);
    _msg_data.timestamp = now;

    /* for every relevant address (RteMsg.Addr) in the RteMsg, HandlingRtr
    searches its route table to see if there is a route table entry with the
    same MetricType of the RteMsg, matching RteMsg.Addr. */

    aodvv2_local_route_t *rt_entry =
        aodvv2_lrs_get_entry(&_msg_data.targ_node.addr, _msg_data.metric_type);

    if (!rt_entry || (rt_entry->metric_type != _msg_data.metric_type)) {
        DEBUG_PUTS("aodvv2: creating new Local Route");

        aodvv2_local_route_t tmp = {0};
        aodvv2_lrs_fill_routing_entry_rrep(&_msg_data, &tmp, link_cost);
        aodvv2_lrs_add_entry(&tmp);

        /* Add entry to NIB forwarding table */
        DEBUG_PUTS("aodvv2: adding Local Route to NIB FT");
        if (gnrc_ipv6_nib_ft_add(&_msg_data.targ_node.addr,
                                 _msg_data.targ_node.pfx_len, &_msg_data.sender,
                                 _netif_pid, AODVV2_ROUTE_LIFETIME) < 0) {
            DEBUG_PUTS("aodvv2: couldn't add route");
        }
    }
    else {
        if (!aodvv2_lrs_offers_improvement(rt_entry, &_msg_data.targ_node)) {
            DEBUG_PUTS("aodvv2: RREP offers no improvement over known route");
            return RFC5444_DROP_PACKET;
        }

        /* The incoming routing information is better than existing routing
         * table information and SHOULD be used to improve the route table. */
        DEBUG_PUTS("aodvv2: updating Routing Table entry");
        aodvv2_lrs_fill_routing_entry_rrep(&_msg_data, rt_entry, link_cost);

        /* Add entry to nib forwarding table */
        gnrc_ipv6_nib_ft_del(&rt_entry->addr, rt_entry->pfx_len);

        DEBUG_PUTS("aodvv2: adding route to NIB FT");
        if (gnrc_ipv6_nib_ft_add(&rt_entry->addr, rt_entry->pfx_len,
                                 &rt_entry->next_hop, _netif_pid,
                                 AODVV2_ROUTE_LIFETIME) < 0) {
            DEBUG_PUTS("aodvv2: couldn't add route");
        }
    }

    if (aodvv2_rcs_is_client(&_msg_data.orig_node.addr) != NULL) {
        DEBUG("aodvv2: {%" PRIu32 ":%" PRIu32 "}\n",
              now.seconds, now.microseconds);
        DEBUG("aodvv2: this is my RREP (SeqNum: %d)\n",
              _msg_data.orig_node.seqnum);
        DEBUG_PUTS("aodvv2: We are done here, thanks!");

        /* Send buffered packets for this address */
        aodvv2_buffer_dispatch(&_msg_data.targ_node.addr);
    }
    else {
        DEBUG_PUTS("aodvv2: not my RREP, passing it on to the next hop.");

        ipv6_addr_t *next_hop =
            aodvv2_lrs_get_next_hop(&_msg_data.orig_node.addr,
                                    _msg_data.metric_type);
        aodvv2_send_rrep(&_msg_data, next_hop);
    }
    return RFC5444_OKAY;
}

static enum rfc5444_result _cb_rreq_blocktlv_messagetlvs_okay(
        struct rfc5444_reader_tlvblock_context *cont)
{
    if (!cont->has_hoplimit) {
        DEBUG("aodvv2: missing hop limit\n");
        return RFC5444_DROP_PACKET;
    }

    _msg_data.msg_hop_limit = cont->hoplimit;
    if (_msg_data.msg_hop_limit == 0) {
        DEBUG("aodvv2: Hoplimit is 0.\n");
        return RFC5444_DROP_PACKET;
    }
    _msg_data.msg_hop_limit--;

    return RFC5444_OKAY;
}

static enum rfc5444_result _cb_rreq_blocktlv_addresstlvs_okay(
        struct rfc5444_reader_tlvblock_context *cont)
{
    struct rfc5444_reader_tlvblock_entry *tlv;
    bool is_orig_node_addr = false;
    bool is_targ_node = false;

    DEBUG("aodvv2: %s\n", netaddr_to_string(&nbuf, &cont->addr));

    /* handle OrigNode SeqNum TLV */
    tlv = _address_consumer_entries[RFC5444_MSGTLV_ORIGSEQNUM].tlv;
    if (tlv) {
        DEBUG("aodvv2: RFC5444_MSGTLV_ORIGSEQNUM: %d\n", *tlv->single_value);
        is_orig_node_addr = true;
        netaddr_to_ipv6_addr(&cont->addr, &_msg_data.orig_node.addr,
                             &_msg_data.orig_node.pfx_len);
        _msg_data.orig_node.seqnum = *tlv->single_value;
    }

    /* handle TargNode SeqNum TLV */
    tlv = _address_consumer_entries[RFC5444_MSGTLV_TARGSEQNUM].tlv;
    if (tlv) {
        DEBUG("aodvv2: RFC5444_MSGTLV_TARGSEQNUM: %d\n", *tlv->single_value);

        is_targ_node = true;
        netaddr_to_ipv6_addr(&cont->addr, &_msg_data.targ_node.addr,
                             &_msg_data.targ_node.pfx_len);
        _msg_data.targ_node.seqnum = *tlv->single_value;
    }

    if (!tlv && !is_orig_node_addr) {
        /* assume that tlv missing => targ_node Address */
        is_targ_node = true;
        netaddr_to_ipv6_addr(&cont->addr, &_msg_data.targ_node.addr,
                             &_msg_data.targ_node.pfx_len);
    }

    if (!is_orig_node_addr && !is_targ_node) {
        DEBUG_PUTS("aodvv2: mandatory RFC5444_MSGTLV_ORIGSEQNUM TLV missing");
        return RFC5444_DROP_PACKET;
    }

    /* handle Metric TLV */
    /* cppcheck: suppress false positive on non-trivially initialized arrays.
     *           this is a known bug: http://trac.cppcheck.net/ticket/5497 */
    /* cppcheck-suppress arrayIndexOutOfBounds */
    tlv = _address_consumer_entries[RFC5444_MSGTLV_METRIC].tlv;
    if (!tlv && is_orig_node_addr) {
        DEBUG_PUTS("aodvv2: missing or unknown metric TLV");
        return RFC5444_DROP_PACKET;
    }

    if (tlv) {
        if (!is_orig_node_addr) {
            DEBUG_PUTS("aodvv2: metric TLV belongs to wrong address");
            return RFC5444_DROP_PACKET;
        }
        DEBUG("aodvv2: RFC5444_MSGTLV_METRIC val: %d, exttype: %d\n",
               *tlv->single_value, tlv->type_ext);

        _msg_data.metric_type = tlv->type_ext;
        _msg_data.orig_node.metric = *tlv->single_value;
    }
    return RFC5444_OKAY;
}

static enum rfc5444_result _cb_rreq_end_callback(
    struct rfc5444_reader_tlvblock_context *cont, bool dropped)
{
    (void) cont;


    /* Check if packet contains the required information */
    if (dropped) {
        DEBUG_PUTS("aodvv2: dropping packet");
        return RFC5444_DROP_PACKET;
    }

    if (ipv6_addr_is_unspecified(&_msg_data.orig_node.addr) ||
        _msg_data.orig_node.seqnum == 0) {
        DEBUG_PUTS("aodvv2: missing OrigNode Address or SeqNum");
        return RFC5444_DROP_PACKET;
    }

    if (ipv6_addr_is_unspecified(&_msg_data.targ_node.addr)) {
        DEBUG_PUTS("aodvv2: missing TargNode Address");
        return RFC5444_DROP_PACKET;
    }

    if (_msg_data.msg_hop_limit == 0) {
        DEBUG_PUTS("aodvv2: hop limit is 0");
        return RFC5444_DROP_PACKET;
    }

    uint8_t link_cost = aodvv2_metric_link_cost(_msg_data.metric_type);
    if ((aodvv2_metric_max(_msg_data.metric_type) - link_cost) <=
        _msg_data.orig_node.metric) {
        DEBUG_PUTS("aodvv2: metric limit reached");
        return RFC5444_DROP_PACKET;
    }

    /* The incoming RREQ MUST be checked against previously received information */
    if (aodvv2_mcmsg_process(&_msg_data) == AODVV2_MCMSG_REDUNDANT) {
        DEBUG_PUTS("aodvv2: packet is redundant");
        return RFC5444_DROP_PACKET;
    }

    aodvv2_metric_update(_msg_data.metric_type, &_msg_data.orig_node.metric);

    /* Update packet timestamp */
    timex_t now;
    xtimer_now_timex(&now);
    _msg_data.timestamp = now;

    /* For every relevant address (RteMsg.Addr) in the RteMsg, HandlingRtr
     * searches its route table to see if there is a route table entry with the
     * same MetricType of the RteMsg, matching RteMsg.Addr.
     */
    aodvv2_local_route_t *rt_entry =
        aodvv2_lrs_get_entry(&_msg_data.orig_node.addr,
                             _msg_data.metric_type);

    if (!rt_entry || (rt_entry->metric_type != _msg_data.metric_type)) {
        DEBUG_PUTS("aodvv2: creating new Local Route");

        aodvv2_local_route_t tmp = {0};

        /* Add this RREQ to LRS */
        aodvv2_lrs_fill_routing_entry_rreq(&_msg_data, &tmp, link_cost);
        aodvv2_lrs_add_entry(&tmp);

        /* Add entry to NIB forwarding table */
        DEBUG_PUTS("aodvv2: adding route to NIB FT");
        if (gnrc_ipv6_nib_ft_add(&_msg_data.orig_node.addr,
                                 _msg_data.orig_node.pfx_len, &_msg_data.sender,
                                 _netif_pid, AODVV2_ROUTE_LIFETIME) < 0) {
            DEBUG_PUTS("aodvv2: couldn't add route");
        }
    }
    else {
        /* If the route is already stored verify if this route offers an
         * improvement in path*/
        if (!aodvv2_lrs_offers_improvement(rt_entry, &_msg_data.orig_node)) {
            DEBUG_PUTS("aodvv2: packet offers no improvement over known route");
            return RFC5444_DROP_PACKET;
        }

        /* The incoming routing information is better than existing routing
         * table information and SHOULD be used to improve the route table. */
        DEBUG_PUTS("aodvv2: updating Local Route");
        aodvv2_lrs_fill_routing_entry_rreq(&_msg_data, rt_entry, link_cost);

        /* Add entry to nib forwarding table */
        gnrc_ipv6_nib_ft_del(&rt_entry->addr, rt_entry->pfx_len);

        DEBUG_PUTS("aodvv2: adding route to NIB FT");
        if (gnrc_ipv6_nib_ft_add(&rt_entry->addr, rt_entry->pfx_len,
                                 &rt_entry->next_hop, _netif_pid,
                                 AODVV2_ROUTE_LIFETIME) < 0) {
            DEBUG_PUTS("aodvv2: couldn't add route");
        }
    }

    /* If TargNode is a client of the router receiving the RREQ, then the
     * router generates a RREP message as specified in Section 7.4, and
     * subsequently processing for the RREQ is complete.  Otherwise,
     * processing continues as follows.
     */
    if (aodvv2_rcs_is_client(&_msg_data.targ_node.addr) != NULL) {
        DEBUG_PUTS("aodvv2: TargNode is on client list, sending RREP");

        /* Make sure to start with a clean metric value */
        _msg_data.targ_node.metric = 0;

        aodvv2_send_rrep(&_msg_data, &_msg_data.sender);
    }
    else {
        DEBUG_PUTS("aodvv2: I'm not TargNode, forwarding RREQ");
        aodvv2_send_rreq(&_msg_data, &ipv6_addr_all_manet_routers_link_local);
    }

    return RFC5444_OKAY;
}

void aodvv2_reader_init(struct rfc5444_reader *reader, kernel_pid_t netif_pid)
{
    assert(reader != NULL && netif_pid != KERNEL_PID_UNDEF);

    if (_netif_pid == KERNEL_PID_UNDEF) {
        _netif_pid = netif_pid;
    }

    rfc5444_reader_add_message_consumer(reader, &_rrep_consumer,
                                        NULL, 0);

    rfc5444_reader_add_message_consumer(reader, &_rrep_address_consumer,
                                        _address_consumer_entries,
                                        ARRAY_SIZE(_address_consumer_entries));

    rfc5444_reader_add_message_consumer(reader, &_rreq_consumer,
                                        NULL, 0);

    rfc5444_reader_add_message_consumer(reader, &_rreq_address_consumer,
                                        _address_consumer_entries,
                                        ARRAY_SIZE(_address_consumer_entries));
}

void aodvv2_rfc5444_handle_packet_prepare(ipv6_addr_t *sender)
{
    assert(sender != NULL);

    _msg_data.sender = *sender;
}
