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
 * @brief       RFC5444
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Locha Mesh developers <contact@locha.io>
 */

#ifndef NET_AODVV2_RFC5444_H
#define NET_AODVV2_RFC5444_H

#include "net/aodvv2/seqnum.h"
#include "net/manet.h"
#include "net/metric.h"

#include "timex.h"

#include "common/netaddr.h"
#include "rfc5444/rfc5444_reader.h"
#include "rfc5444/rfc5444_writer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    RFC5444 thread stack size
 */
#ifndef CONFIG_AODVV2_RFC5444_STACK_SIZE
#define CONFIG_AODVV2_RFC5444_STACK_SIZE     (2048)
#endif

/**
 * @name    RFC5444 thread priority
 */
#ifndef CONFIG_AODVV2_RFC5444_PRIO
#define CONFIG_AODVV2_RFC5444_PRIO           (6)
#endif

/**
 * @name    RFC5444 message queue size
 */
#ifndef CONFIG_AODVV2_RFC5444_MSG_QUEUE_SIZE
#define CONFIG_AODVV2_RFC5444_MSG_QUEUE_SIZE (32)
#endif

/**
 * @name    RFC5444 maximum packet size
 */
#ifndef CONFIG_AODVV2_RFC5444_PACKET_SIZE
#define CONFIG_AODVV2_RFC5444_PACKET_SIZE    (128)
#endif

/**
 * @name    RFC5444 address TLVs buffer size
 */
#ifndef CONFIG_AODVV2_RFC5444_ADDR_TLVS_SIZE
#define CONFIG_AODVV2_RFC5444_ADDR_TLVS_SIZE (1000)
#endif

/**
 * @brief   AODVv2 message types
 */
typedef enum {
    RFC5444_MSGTYPE_RREQ = 10,    /**< RREQ message type */
    RFC5444_MSGTYPE_RREP = 11,    /**< RREP message type */
    RFC5444_MSGTYPE_RERR = 12,    /**< RERR message type */
    RFC5444_MSGTYPE_RREP_ACK = 13 /**< RREP_Ack message type */
} rfc5444_msg_type_t;

/**
 * @brief   AODVv2 TLV types
 */
typedef enum {
    RFC5444_MSGTLV_ORIGSEQNUM,
    RFC5444_MSGTLV_TARGSEQNUM,
    RFC5444_MSGTLV_UNREACHABLE_NODE_SEQNUM,
    RFC5444_MSGTLV_METRIC,
} rfc5444_tlv_type_t;

/**
 * @brief   Data about an OrigNode or TargNode.
 */
typedef struct {
    ipv6_addr_t addr;             /**< IPv6 address of the node */
    uint8_t pfx_len;              /**< IPv6 address length */
    uint8_t metric;               /**< Metric value */
    aodvv2_seqnum_t seqnum;       /**< Sequence Number */
} node_data_t;

/**
 * @brief   All data contained in a RREQ or RREP.
 */
typedef struct {
    uint8_t msg_hop_limit;        /**< Hop limit */
    ipv6_addr_t sender;           /**< IP address of the neighboring router */
    routing_metric_t metric_type; /**< Metric type */
    node_data_t orig_node;        /**< OrigNode data */
    node_data_t targ_node;        /**< TargNode data */
    ipv6_addr_t seqnortr;         /**< SeqNoRtr */
    timex_t timestamp;            /**< Time at which the message was received */
} aodvv2_message_t;

typedef struct {
    struct rfc5444_writer_target target; /**< RFC5444 writer target */
    ipv6_addr_t target_addr;             /**< Address where the packet will be sent */
} aodvv2_writer_target_t;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_AODVV2_RFC5444_H */
/** @} */
