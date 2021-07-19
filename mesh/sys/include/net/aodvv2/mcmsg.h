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
 *
 * @{
 * @file
 * @brief       AODVv2 Multicast Message Set
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Locha Mesh developers <contact@locha.io>
 */

#ifndef NET_AODVV2_MCMSG_H
#define NET_AODVV2_MCMSG_H

#include "net/metric.h"
#include "net/aodvv2/seqnum.h"
#include "net/aodvv2/rfc5444.h"

#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of entries on the set.
 */
#ifndef CONFIG_AODVV2_MCMSG_MAX_ENTRIES
#define CONFIG_AODVV2_MCMSG_MAX_ENTRIES (16)
#endif

/**
 * @brief   A Multicast Message
 */
typedef struct {
    ipv6_addr_t orig_prefix;      /**< OrigPrefix */
    uint8_t orig_pfx_len;         /**< OrigPrefix length */
    ipv6_addr_t targ_prefix;      /**< TargPrefix */
    aodvv2_seqnum_t orig_seqnum;  /**< SeqNum associated with OrigPrefix */
    aodvv2_seqnum_t targ_seqnum;  /**< SeqNum associated with TargPrefix */
    routing_metric_t metric_type; /**< Metric type of the RREQ */
    uint8_t metric;               /**< Metric of the RREQ */
    timex_t timestamp;            /**< Last time this entry was updated */
    timex_t removal_time;         /**< Time at which this entry should be removed */
    uint16_t netif;               /**< Interface where this McMsg was received */
    ipv6_addr_t seqnortr;         /**< SeqNoRtr */
} aodvv2_mcmsg_t;

enum {
    AODVV2_MCMSG_REDUNDANT = -1,  /**< Processed McMsg is redundant */
    AODVV2_MCMSG_OK = 0           /**< McMsg is new (ok) */
};

/**
 * @brief   Initialize RREQ table.
 */
void aodvv2_mcmsg_init(void);

/**
 * @brief   Process an RREQ
 *
 * @param[in] msg RREQ message
 *
 * @return AODVV2_MCMSG_OK processing went fine.
 * @return AODVV2_MCMSG_REDUNDANT message is redundant.
 */
int aodvv2_mcmsg_process(aodvv2_message_t *msg);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_AODVV2_MCMSG_H */
/** @} */
