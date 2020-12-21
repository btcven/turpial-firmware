/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * Copyright (C) 2020 Locha Inc
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
 * @brief       RFC5444 RREQ
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Gustavo Grisales <gustavosinbandera1@hotmail.com>
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "aodvv2_writer.h"
#include "net/aodvv2/metric.h"

#include "rfc5444_compat.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static int _cb_add_message_header(struct rfc5444_writer *wr, struct rfc5444_writer_message *message);
static void _cb_rreq_add_addresses(struct rfc5444_writer *wr);
static void _cb_rrep_add_addresses(struct rfc5444_writer *wr);

/*
 * message content provider that will add message TLVs,
 * addresses and address block TLVs to all messages of type RREQ.
 */
static struct rfc5444_writer_content_provider _rreq_message_content_provider =
{
    .msg_type = RFC5444_MSGTYPE_RREQ,
    .addAddresses = _cb_rreq_add_addresses,
};

/* declaration of all address TLVs added to the RREQ message */
static struct rfc5444_writer_tlvtype _rreq_addrtlvs[] =
{
    [RFC5444_MSGTLV_ORIGSEQNUM] = { .type = RFC5444_MSGTLV_ORIGSEQNUM },
    [RFC5444_MSGTLV_METRIC] = {
        .type = RFC5444_MSGTLV_METRIC,
        .exttype = CONFIG_AODVV2_DEFAULT_METRIC
    },
};

/*
 * message content provider that will add message TLVs,
 * addresses and address block TLVs to all messages of type RREQ.
 */
static struct rfc5444_writer_content_provider _rrep_message_content_provider =
{
    .msg_type = RFC5444_MSGTYPE_RREP,
    .addAddresses = _cb_rrep_add_addresses,
};

/* declaration of all address TLVs added to the RREP message */
static struct rfc5444_writer_tlvtype _rrep_addrtlvs[] =
{
    [RFC5444_MSGTLV_ORIGSEQNUM] = { .type = RFC5444_MSGTLV_ORIGSEQNUM},
    [RFC5444_MSGTLV_TARGSEQNUM] = { .type = RFC5444_MSGTLV_TARGSEQNUM},
    [RFC5444_MSGTLV_METRIC] = {
        .type = RFC5444_MSGTLV_METRIC,
        .exttype = CONFIG_AODVV2_DEFAULT_METRIC
    },
};

static struct rfc5444_writer_message *_rreq_msg;
static struct rfc5444_writer_message *_rrep_msg;

static aodvv2_message_t _msg;

static int _cb_add_message_header(struct rfc5444_writer *wr, struct rfc5444_writer_message *message)
{
    /* no originator, no hopcount, has msg_hop_limit, no seqno */
    rfc5444_writer_set_msg_header(wr, message, false, false, true, false);
    rfc5444_writer_set_msg_hoplimit(wr, message, _msg.msg_hop_limit);

    return 0;
}

static void _cb_rreq_add_addresses(struct rfc5444_writer *wr)
{
    struct rfc5444_writer_address *orig_prefix;
    struct netaddr tmp;
    uint8_t pfx_len;

    /* Add OrigPrefix address */
    pfx_len = _msg.orig_node.pfx_len;
    if (pfx_len == 0 || pfx_len > 128) {
        pfx_len = 128;
    }
    ipv6_addr_to_netaddr(&_msg.orig_node.addr, pfx_len, &tmp);
    orig_prefix = rfc5444_writer_add_address(wr, _rreq_message_content_provider.creator, &tmp, true);
    assert(orig_prefix != NULL);

    /* Add TargPrefix address */
    pfx_len = _msg.targ_node.pfx_len;
    if (pfx_len == 0 || pfx_len > 128) {
        pfx_len = 128;
    }
    ipv6_addr_to_netaddr(&_msg.targ_node.addr, pfx_len, &tmp);
    rfc5444_writer_add_address(wr, _rreq_message_content_provider.creator, &tmp, true);

    /* Add SeqNum TLV and metric TLV to OrigPrefix */
    rfc5444_writer_add_addrtlv(wr, orig_prefix, &_rreq_addrtlvs[RFC5444_MSGTLV_ORIGSEQNUM],
                               &_msg.orig_node.seqnum, sizeof(_msg.orig_node.seqnum),
                               false);

    rfc5444_writer_add_addrtlv(wr, orig_prefix, &_rreq_addrtlvs[RFC5444_MSGTLV_METRIC],
                               &_msg.orig_node.metric, sizeof(_msg.orig_node.metric),
                               false);
}

static void _cb_rrep_add_addresses(struct rfc5444_writer *wr)
{
    struct rfc5444_writer_address *orig_prefix;
    struct rfc5444_writer_address *targ_prefix;
    struct netaddr tmp;
    uint8_t pfx_len;

    uint16_t orig_node_seqnum = _msg.orig_node.seqnum;
    uint16_t targ_node_seqnum = aodvv2_seqnum_get();
    aodvv2_seqnum_inc();
    uint8_t targ_node_hopct = _msg.targ_node.metric;

    /* Add OrigPrefix address */
    pfx_len = _msg.orig_node.pfx_len;
    if (pfx_len == 0 || pfx_len > 128) {
        pfx_len = 128;
    }
    ipv6_addr_to_netaddr(&_msg.orig_node.addr, pfx_len, &tmp);
    orig_prefix = rfc5444_writer_add_address(wr, _rrep_message_content_provider.creator, &tmp, true);
    assert(orig_prefix != NULL);

    /* Add TargPrefix address */
    pfx_len = _msg.targ_node.pfx_len;
    if (pfx_len == 0 || pfx_len > 128) {
        pfx_len = 128;
    }
    ipv6_addr_to_netaddr(&_msg.targ_node.addr, pfx_len, &tmp);
    targ_prefix = rfc5444_writer_add_address(wr, _rrep_message_content_provider.creator, &tmp, true);
    assert(orig_prefix != NULL);

    /* Add ORIGSEQNUM TLV to OrigPrefix */
    rfc5444_writer_add_addrtlv(wr, orig_prefix, &_rrep_addrtlvs[RFC5444_MSGTLV_ORIGSEQNUM], &orig_node_seqnum,
                               sizeof(orig_node_seqnum), false);

    /* Add ORIGSEQNUM and METRIC TLV to TargPrefix */
    rfc5444_writer_add_addrtlv(wr, targ_prefix, &_rrep_addrtlvs[RFC5444_MSGTLV_TARGSEQNUM], &targ_node_seqnum,
                               sizeof(targ_node_seqnum), false);

    rfc5444_writer_add_addrtlv(wr, targ_prefix, &_rrep_addrtlvs[RFC5444_MSGTLV_METRIC], &targ_node_hopct,
                               sizeof(targ_node_hopct), false);
}

void aodvv2_writer_init(struct rfc5444_writer *wr)
{
    assert(wr != NULL);
    int res;

    res = rfc5444_writer_register_msgcontentprovider(wr, &_rreq_message_content_provider, _rreq_addrtlvs,
                                                     ARRAY_SIZE(_rreq_addrtlvs));
    if (res < 0) {
        DEBUG("rfc5444_writer: couldn't register RREQ message provider\n");
        return;
    }

    res = rfc5444_writer_register_msgcontentprovider(wr, &_rrep_message_content_provider, _rrep_addrtlvs,
                                                     ARRAY_SIZE(_rrep_addrtlvs));
    if (res < 0) {
        DEBUG("rfc5444_writer: couldn't register RREQ message provider\n");
        return;
    }

    _rreq_msg = rfc5444_writer_register_message(wr, RFC5444_MSGTYPE_RREQ, false);
    if (_rreq_msg == NULL) {
        DEBUG("rfc5444_writer: couldn't register RREQ message\n");
        return;
    }

    _rrep_msg = rfc5444_writer_register_message(wr, RFC5444_MSGTYPE_RREP, false);
    if (_rrep_msg == NULL) {
        DEBUG("rfc5444_writer: couldn't register RREP message\n");
        return;
    }

    _rreq_msg->addMessageHeader = _cb_add_message_header;
    _rrep_msg->addMessageHeader = _cb_add_message_header;
}

int aodvv2_writer_send_rreq(struct rfc5444_writer *wr, aodvv2_message_t *message)
{
    memcpy(&_msg, message, sizeof(aodvv2_message_t));

    if (rfc5444_writer_create_message_alltarget(wr, RFC5444_MSGTYPE_RREQ,
                                                RFC5444_MAX_ADDRLEN) != RFC5444_OKAY) {
        DEBUG_PUTS("aodvv2: RREQ message not created");
        return -EIO;
    }

    return 0;
}

int aodvv2_writer_send_rrep(struct rfc5444_writer *wr, aodvv2_message_t *message)
{
    memcpy(&_msg, message, sizeof(aodvv2_message_t));

    /* TODO(jeandudey): should we use alltarget for RREP? AFAIK we should have
     * multiple targets to specific destinations (with the specified network
     * interface), not to _all targets_ (all nodes we know of) */
    if (rfc5444_writer_create_message_alltarget(wr, RFC5444_MSGTYPE_RREP,
                                                RFC5444_MAX_ADDRLEN) != RFC5444_OKAY) {
        DEBUG_PUTS("aodvv2: RREP message not created");
        return -EIO;
    }

    return 0;
}
