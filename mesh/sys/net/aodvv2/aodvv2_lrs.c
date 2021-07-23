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
 * @brief       Cobbled-together routing table.
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Locha Mesh developers <contact@locha.io>
 */

#include "net/aodvv2/conf.h"
#include "net/aodvv2/lrs.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void _reset_entry_if_stale(uint8_t i);

/**
 * @brief   Container for @ref aodvv2_local_route_t
 *
 * This wraps the Local Route and adds an `used` field to check if the entry is
 * in use on the storage array.
 */
typedef struct {
    aodvv2_local_route_t route; /**< Local Route */
    bool used; /**< Is this entry used? */
} lrs_entry_t;

/**
 * @brief   Memory for the Routing Entries Set
 */
static lrs_entry_t routing_table[CONFIG_AODVV2_MAX_ROUTING_ENTRIES];

static timex_t null_time;
static timex_t max_seqnum_lifetime;
static timex_t active_interval;
static timex_t max_idletime;
static timex_t validity_t;
static timex_t now;

void aodvv2_lrs_init(void)
{
    DEBUG("aodvv2_lrs_init()\n");

    null_time = timex_set(0, 0);
    max_seqnum_lifetime = timex_set(CONFIG_AODVV2_MAX_SEQNUM_LIFETIME, 0);
    active_interval = timex_set(CONFIG_AODVV2_ACTIVE_INTERVAL, 0);
    max_idletime = timex_set(CONFIG_AODVV2_MAX_IDLETIME, 0);
    validity_t = timex_set(CONFIG_AODVV2_ACTIVE_INTERVAL +
                           CONFIG_AODVV2_MAX_IDLETIME, 0);

    memset(&routing_table, 0, sizeof(routing_table));
}

ipv6_addr_t *aodvv2_lrs_get_next_hop(ipv6_addr_t *dest,
                                     routing_metric_t metric_type)
{
    aodvv2_local_route_t *entry = aodvv2_lrs_get_entry(dest, metric_type);
    if (!entry) {
        return NULL;
    }
    return (&entry->next_hop);
}

void aodvv2_lrs_add_entry(aodvv2_local_route_t *entry)
{
    /* only add if we don't already know the address */
    if (aodvv2_lrs_get_entry(&entry->addr, entry->metric_type)) {
        return;
    }
    /*find free spot in RT and place rt_entry there */
    for (unsigned i = 0; i < ARRAY_SIZE(routing_table); i++) {
        if (!routing_table[i].used) {
            memcpy(&routing_table[i].route, entry, sizeof(aodvv2_local_route_t));
            routing_table[i].used = true;
            return;
        }
    }
}

aodvv2_local_route_t *aodvv2_lrs_get_entry(ipv6_addr_t *addr,
                                           routing_metric_t metric_type)
{
    for (unsigned i = 0; i < ARRAY_SIZE(routing_table); i++) {
        _reset_entry_if_stale(i);

        if (ipv6_addr_equal(&routing_table[i].route.addr, addr) &&
            routing_table[i].route.metric_type == metric_type) {
            return &routing_table[i].route;
        }
    }
    return NULL;
}

void aodvv2_lrs_delete_entry(ipv6_addr_t *addr, routing_metric_t metric_type)
{
    for (unsigned i = 0; i < ARRAY_SIZE(routing_table); i++) {
        _reset_entry_if_stale(i);

        if (routing_table[i].used) {
            if (ipv6_addr_equal(&routing_table[i].route.addr, addr) &&
                routing_table[i].route.metric_type == metric_type) {
                memset(&routing_table[i].route, 0, sizeof(aodvv2_local_route_t));
                routing_table[i].used = false;
                return;
            }
        }
    }
}


/*
 * Check if entry at index i is stale as described in Section 6.3.
 * and clear the struct it fills if it is
 */
static void _reset_entry_if_stale(uint8_t i)
{
    xtimer_now_timex(&now);
    timex_t last_used, expiration_time;

    if (timex_cmp(routing_table[i].route.expiration_time, null_time) == 0) {
        return;
    }

    int state = routing_table[i].route.state;
    last_used = routing_table[i].route.last_used;
    expiration_time = routing_table[i].route.expiration_time;

    /* an Active route is considered to remain Active as long as it is used at least once
     * during every ACTIVE_INTERVAL. When a route is no longer Active, it becomes an Idle route. */

    /* if the node is younger than the active interval, don't bother */
    if (timex_cmp(now, active_interval) < 0) {
        return;
    }

    if ((state == ROUTE_STATE_ACTIVE) &&
        (timex_cmp(timex_sub(now, active_interval), last_used) == 1)) {
        routing_table[i].route.state = ROUTE_STATE_IDLE;
        routing_table[i].route.last_used = now; /* mark the time entry was set to Idle */
    }

    /* After an idle route remains Idle for MAX_IDLETIME, it becomes an Expired route.
       A route MUST be considered Expired if Current_Time >= Route.ExpirationTime
    */

    /* if the node is younger than the expiration time, don't bother */
    if (timex_cmp(now, expiration_time) < 0) {
        return;
    }

    if ((state == ROUTE_STATE_IDLE) &&
        (timex_cmp(expiration_time, now) < 1)) {
        DEBUG("\t expiration_time: %"PRIu32":%"PRIu32" , now: %"PRIu32":%"PRIu32"\n",
              expiration_time.seconds, expiration_time.microseconds,
              now.seconds, now.microseconds);
        routing_table[i].route.state = ROUTE_STATE_EXPIRED;
        routing_table[i].route.last_used = now; /* mark the time entry was set to Expired */
    }

    /* After that time, old sequence number information is considered no longer
     * valuable and the Expired route MUST BE expunged */
    if (timex_cmp(timex_sub(now, last_used), max_seqnum_lifetime) >= 0) {
        memset(&routing_table[i].route, 0, sizeof(aodvv2_local_route_t));
    }
}

bool aodvv2_lrs_offers_improvement(aodvv2_local_route_t *rt_entry,
                                   node_data_t *node_data)
{
    /* Check if new info is stale */
    if (aodvv2_seqnum_cmp(node_data->seqnum, rt_entry->seqnum) < 0) {
        return false;
    }
    /* Check if new info is more costly */
    if ((node_data->metric >= rt_entry->metric)
        && !(rt_entry->state != ROUTE_STATE_BROKEN)) {
        return false;
    }
    /* Check if new info repairs a broken route */
    if (!(rt_entry->state != ROUTE_STATE_BROKEN)) {
        return false;
    }
    return true;
}

void aodvv2_lrs_fill_routing_entry_rreq(aodvv2_message_t *msg,
                                        aodvv2_local_route_t *rt_entry,
                                        uint8_t link_cost)
{
    rt_entry->addr = msg->orig_node.addr;
    rt_entry->pfx_len = msg->orig_node.pfx_len;
    rt_entry->seqnum = msg->orig_node.seqnum;
    rt_entry->next_hop = msg->sender;
    rt_entry->last_used = msg->timestamp;
    rt_entry->expiration_time = timex_add(msg->timestamp, validity_t);
    rt_entry->metric_type = msg->metric_type;
    rt_entry->metric = msg->orig_node.metric + link_cost;
    rt_entry->state = ROUTE_STATE_ACTIVE;
}

void aodvv2_lrs_fill_routing_entry_rrep(aodvv2_message_t *msg,
                                        aodvv2_local_route_t *rt_entry,
                                        uint8_t link_cost)
{
    rt_entry->addr = msg->targ_node.addr;
    rt_entry->pfx_len = msg->targ_node.pfx_len;
    rt_entry->seqnum = msg->targ_node.seqnum;
    rt_entry->next_hop = msg->sender;
    rt_entry->last_used = msg->timestamp;
    rt_entry->expiration_time = timex_add(msg->timestamp, validity_t);
    rt_entry->metric_type = msg->metric_type;
    rt_entry->metric = msg->targ_node.metric + link_cost;
    rt_entry->state = ROUTE_STATE_ACTIVE;
}
