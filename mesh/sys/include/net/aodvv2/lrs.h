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
 * @brief       AODVv2 Local Route Set
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Locha Mesh developers <contact@locha.io> 
 */

#ifndef AODVV2_LRS_H
#define AODVV2_LRS_H

#include <string.h>

#include "net/aodvv2/rfc5444.h"
#include "net/aodvv2/seqnum.h"
#include "net/metric.h"

#include "timex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum number of routing entries
 * @{
 */
#ifndef CONFIG_AODVV2_MAX_ROUTING_ENTRIES
#define CONFIG_AODVV2_MAX_ROUTING_ENTRIES (16)
#endif
/** @} */

/**
 * A route table entry (i.e., a route) may be in one of the following states:
 */
enum aodvv2_routing_state {
    ROUTE_STATE_ACTIVE,
    ROUTE_STATE_IDLE,
    ROUTE_STATE_EXPIRED,
    ROUTE_STATE_BROKEN,
    ROUTE_STATE_TIMED
};

/**
 * @brief   A Local Route
 */
typedef struct {
    ipv6_addr_t addr;             /**< Destination IPv6 address */
    uint8_t pfx_len;              /**< Prefix length */
    aodvv2_seqnum_t seqnum;       /**< SeqNum associated with the IPv6 address */
    ipv6_addr_t next_hop;         /**< Next hop IP address towards the destination */
    timex_t last_used;            /**< Last time this route was used */
    timex_t expiration_time;      /**< Time at which this route expires */
    routing_metric_t metric_type; /**< Metric type of this route */
    uint8_t metric;               /**< Metric value of this route*/
    uint8_t state;                /**< State of this route */
} aodvv2_local_route_t;

/**
 * @brief     Initialize Local Route Set.
 */
void aodvv2_lrs_init(void);

/**
 * @brief     Get next hop towards dest.
 *
 * @param[in] dest        Destination of the packet
 * @param[in] metric_type  Metric Type of the desired route
 *
 * @return Next hop towards dest if it exists, NULL otherwise.
 */
ipv6_addr_t *aodvv2_lrs_get_next_hop(ipv6_addr_t *dest,
                                     routing_metric_t metric_type);

/**
 * @brief     Add new entry to Local Route, if there is no other entry
 *            to the same destination.
 *
 * @param[in] entry The Local Route to add.
 */
void aodvv2_lrs_add_entry(aodvv2_local_route_t *entry);

/**
 * @brief     Retrieve pointer to a Local Route entry.
 *
 * @param[in] addr       The address towards which the route should point
 * @param[in] metric_type Metric Type of the desired route
 *
 * @return Local Route if it exists, NULL otherwise
 */
aodvv2_local_route_t *aodvv2_lrs_get_entry(ipv6_addr_t *addr,
                                           routing_metric_t metric_type);

/**
 * @brief     Delete Local Route entry towards addr with metric type MetricType,
 *            if it exists.
 *
 * @param[in] addr       The address towards which the route should point
 * @param[in] metric_type Metric Type of the desired route
 */
void aodvv2_lrs_delete_entry(ipv6_addr_t *addr, routing_metric_t metric_type);

/**
 * @brief   Check if the data of a RREQ or RREP offers improvement for an
 *          existing Local Route entry.
 *
 * @param[in] rt_entry  The Local Route to check.
 * @param[in] node_data The data to check against.
 *
 * @return true if offers improvement, false otherwise.
 */
bool aodvv2_lrs_offers_improvement(aodvv2_local_route_t *rt_entry,
                                   node_data_t *node_data);

/**
 * @brief   Fills a Local Route entry with the data of a RREQ.
 *
 * @param[in]  msg       The RREQ's data
 * @param[out] rt_entry  The Local Route entry to fill
 * @param[in]  link_cost The link cost for this RREQ
 */
void aodvv2_lrs_fill_routing_entry_rreq(aodvv2_message_t *msg,
                                        aodvv2_local_route_t *rt_entry,
                                        uint8_t link_cost);

/**
 * @brief   Fills a Local Route entry with the data of a RREP.
 *
 * @param[in]  msg       The RREP's data
 * @param[out] rt_entry  The Local Route entry to fill
 * @param[in]  link_cost The link cost for this RREP
 */
void aodvv2_lrs_fill_routing_entry_rrep(aodvv2_message_t *msg,
                                        aodvv2_local_route_t *rt_entry,
                                        uint8_t link_cost);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* AODVV2_LRS_H  */
