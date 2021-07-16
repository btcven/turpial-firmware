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
 *
 * @{
 * @file
 * @brief       AODVv2 Multicast Message Set
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Locha Mesh devs <contact@locha.io>
 * @}
 */

#include "net/aodvv2/conf.h"
#include "net/aodvv2/mcmsg.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

typedef struct {
    aodvv2_mcmsg_t data; /**< McMsg data */
    bool used;           /**< Is this entry used? */
} internal_entry_t;

static internal_entry_t _entries[CONFIG_AODVV2_MCMSG_MAX_ENTRIES];
static mutex_t _lock = MUTEX_INIT;

static timex_t _max_seqnum_lifetime;

static void _reset_entry_if_stale(internal_entry_t *entry)
{
    if (!entry->used) {
        return;
    }

    timex_t current_time;
    xtimer_now_timex(&current_time);

    if (timex_cmp(current_time, entry->data.removal_time) == 1) {
        DEBUG_PUTS("aodvv2: McMsg is stale");
        memset(&entry->data, 0, sizeof(entry->data));
        entry->used = false;
    }
}

static inline bool _is_compatible_mcmsg(aodvv2_mcmsg_t *lhs, aodvv2_mcmsg_t *rhs)
{
    /* A RREQ is considered compatible if they both contain the same OrigPrefix,
     * OrigPrefixLength, TargPrefix and MetricType */
    if (ipv6_addr_equal(&lhs->orig_prefix, &rhs->orig_prefix) &&
        (lhs->orig_pfx_len == rhs->orig_pfx_len) &&
        ipv6_addr_equal(&lhs->targ_prefix, &rhs->targ_prefix) &&
        (lhs->metric_type == rhs->metric_type)) {
        return true;
    }

    return false;
}

static inline bool _is_compatible(aodvv2_mcmsg_t *entry, aodvv2_message_t *msg)
{
    /* A RREQ is considered compatible if they both contain the same OrigPrefix,
     * OrigPrefixLength, TargPrefix and MetricType */
    if (ipv6_addr_equal(&entry->orig_prefix, &msg->orig_node.addr) &&
        (entry->orig_pfx_len == msg->orig_node.pfx_len) &&
        ipv6_addr_equal(&entry->targ_prefix, &msg->targ_node.addr) &&
        (entry->metric_type == msg->metric_type)) {
        return true;
    }

    return false;
}

static inline bool _is_comparable(aodvv2_mcmsg_t *entry, aodvv2_message_t *msg)
{
    /* If both McMsg don't provide a SeqNoRtr address (is unspcified), they only
     * need to be compatible to be comparable */
    if (ipv6_addr_is_unspecified(&entry->seqnortr) &&
        ipv6_addr_is_unspecified(&msg->seqnortr)) {
        return _is_compatible(entry, msg);
    }

    /* At least one of the McMsg provided a SeqNoRtr address, so it needs to be
     * checked if they're the same in order for the McMsgs to be comparable */
    if (_is_compatible(entry, msg) &&
        ipv6_addr_equal(&entry->seqnortr, &msg->seqnortr)) {
        return true;
    }

    return false;
}

static internal_entry_t *_find_comparable_entry(aodvv2_message_t *msg)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_entries); i++) {
        internal_entry_t *entry = &_entries[i];
        _reset_entry_if_stale(entry);

        if (entry->used) {
            if (_is_comparable(&entry->data, msg)) {
                return entry;
            }
        }
    }

    return NULL;
}

static internal_entry_t *_add(aodvv2_message_t *msg)
{
    /* Find empty McMsg and fill it */
    for (unsigned i = 0; i < ARRAY_SIZE(_entries); i++) {
        internal_entry_t *entry = &_entries[i];

        if (!entry->used) {
            timex_t current_time;
            xtimer_now_timex(&current_time);

            entry->used = true;
            entry->data.orig_prefix = msg->orig_node.addr;
            entry->data.orig_pfx_len = msg->orig_node.pfx_len;
            entry->data.targ_prefix = msg->targ_node.addr;
            entry->data.metric_type = msg->metric_type;
            entry->data.metric = msg->orig_node.metric;
            entry->data.orig_seqnum = msg->orig_node.seqnum;

            entry->data.timestamp = current_time;
            entry->data.removal_time = timex_add(current_time, _max_seqnum_lifetime);
            return entry;
        }
    }

    return NULL;
}

void aodvv2_mcmsg_init(void)
{
    DEBUG_PUTS("aodvv2: init McMset set");
    mutex_lock(&_lock);

    _max_seqnum_lifetime = timex_set(CONFIG_AODVV2_MAX_SEQNUM_LIFETIME, 0);

    memset(&_entries, 0, sizeof(_entries));
    mutex_unlock(&_lock);
}

int aodvv2_mcmsg_process(aodvv2_message_t *msg)
{
    mutex_lock(&_lock);

    internal_entry_t *comparable = _find_comparable_entry(msg);
    if (comparable == NULL) {
        DEBUG_PUTS("aodvv2: adding new McMsg");
        if (_add(msg) == NULL) {
            DEBUG_PUTS("aodvv2: McMsg set is full");
        }
        mutex_unlock(&_lock);
        return AODVV2_MCMSG_OK;
    }

    DEBUG_PUTS("aodvv2: comparable McMsg found");

    /* There's a comparable entry, update it's timing information */
    timex_t current_time;
    xtimer_now_timex(&current_time);

    comparable->data.timestamp = current_time;
    comparable->data.removal_time = timex_add(current_time, _max_seqnum_lifetime);

    int seqcmp = aodvv2_seqnum_cmp(comparable->data.orig_seqnum, msg->orig_node.seqnum);
    if (seqcmp < 0) {
        DEBUG_PUTS("aodvv2: stored McMsg is newer");
        mutex_unlock(&_lock);
        return AODVV2_MCMSG_REDUNDANT;
    }

    if (seqcmp == 0) {
        if (comparable->data.metric == msg->orig_node.metric) {
            DEBUG_PUTS("aodvv2: stored McMsg is no worse than received");
            mutex_unlock(&_lock);
            return AODVV2_MCMSG_REDUNDANT;
        }
    }

    if (seqcmp > 0) {
        DEBUG_PUTS("aodvv2: received McMsg is newer than stored");
    }

    comparable->data.orig_seqnum = msg->orig_node.seqnum;
    comparable->data.metric = msg->orig_node.metric;

    /* Search for compatible entries and compare their metrics */
    for (unsigned i = 0; i < ARRAY_SIZE(_entries); i++) {
        internal_entry_t *entry = &_entries[i];
        if (entry == comparable) {
            continue;
        }

        _reset_entry_if_stale(entry);

        if (entry->used && entry != comparable) {
            if (_is_compatible_mcmsg(&comparable->data, &entry->data)) {
                if (entry->data.metric <= comparable->data.metric) {
                    DEBUG_PUTS("aodvv2: received McMsg is worse than stored");
                    mutex_unlock(&_lock);
                    return AODVV2_MCMSG_REDUNDANT;
                }
            }
        }
    }

    mutex_unlock(&_lock);
    return AODVV2_MCMSG_OK;
}
