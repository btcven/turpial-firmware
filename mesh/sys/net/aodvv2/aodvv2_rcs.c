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
 * @ingroup     aodvv2
 * @{
 *
 * @file
 * @brief       AODVv2 Router Client Set
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Gustavo Grisales <gustavosinbandera1@hotmail.com>
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "net/aodvv2/rcs.h"

#include "mutex.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/**
 * @brief   Memory for the Client Set entries.
 */
typedef struct {
    aodvv2_rcs_entry_t data; /**< Client data */
    bool used; /**< Is this entry used? */
} internal_entry_t;

static internal_entry_t _entries[CONFIG_AODVV2_RCS_ENTRIES];
static mutex_t _lock = MUTEX_INIT;

void aodvv2_rcs_init(void)
{
    mutex_lock(&_lock);
    memset(_entries, 0, sizeof(_entries));
    mutex_unlock(&_lock);
}

aodvv2_rcs_entry_t *aodvv2_rcs_add(const ipv6_addr_t *addr, uint8_t pfx_len,
                                   const uint8_t cost)
{
    if (pfx_len > 128) {
        pfx_len = 128;
    }

    const aodvv2_rcs_entry_t *entry = aodvv2_rcs_matches(addr, pfx_len);
    if (entry != NULL) {
        DEBUG_PUTS("aodvv2: client exists, not adding it");
        return NULL;
    }

    mutex_lock(&_lock);
    for (unsigned i = 0; i < ARRAY_SIZE(_entries); i++) {
        internal_entry_t *entry = &_entries[i];

        /* Find free spot to place the new entry. */
        if (!entry->used) {
            ipv6_addr_init_prefix(&entry->data.addr, addr, pfx_len);
            entry->data.pfx_len = pfx_len;
            entry->data.cost = cost;

            entry->used = true;

            mutex_unlock(&_lock);
            return &entry->data;
        }
    }

    DEBUG_PUTS("aodvv2: router client set is full");
    mutex_unlock(&_lock);
    return NULL;
}

void aodvv2_rcs_del(const ipv6_addr_t *addr, uint8_t pfx_len)
{
    if (pfx_len > 128) {
        pfx_len = 128;
    }

    aodvv2_rcs_entry_t *entry = aodvv2_rcs_matches(addr, pfx_len);

    if (!entry) {
        DEBUG_PUTS("aodvv2: client not found\n");
        return;
    }

    mutex_lock(&_lock);
    internal_entry_t *internal = container_of(entry, internal_entry_t, data);
    memset(&internal->data, 0, sizeof(aodvv2_rcs_entry_t));
    internal->used = false;
    mutex_unlock(&_lock);
}

aodvv2_rcs_entry_t *aodvv2_rcs_matches(const ipv6_addr_t *addr,
                                       uint8_t pfx_len)
{
    if (pfx_len > 128) {
        pfx_len = 128;
    }

    mutex_lock(&_lock);
    for (unsigned i = 0; i < ARRAY_SIZE(_entries); i++) {
        internal_entry_t *entry = &_entries[i];

        /* Skip unused entries */
        if (!entry->used) {
            continue;
        }

        /* Compare addresses by prefix */
        if ((entry->data.pfx_len == pfx_len) &&
            (ipv6_addr_match_prefix(&entry->data.addr,
                                   addr) >= entry->data.pfx_len)) {
            mutex_unlock(&_lock);
            return &entry->data;
        }
    }

    /* No entry matches */
    mutex_unlock(&_lock);
    return NULL;
}

aodvv2_rcs_entry_t *aodvv2_rcs_is_client(const ipv6_addr_t *addr)
{
    mutex_lock(&_lock);
    for (unsigned i = 0; i < ARRAY_SIZE(_entries); i++) {
        internal_entry_t *entry = &_entries[i];

        /* Skip unused entries */
        if (!entry->used) {
            continue;
        }

        /* Compare addresses by prefix */
        if (ipv6_addr_match_prefix(&entry->data.addr,
                                   addr) >= entry->data.pfx_len) {
            mutex_unlock(&_lock);
            return &entry->data;
        }
    }
    mutex_unlock(&_lock);
    return NULL;
}

void aodvv2_rcs_print_entries(void)
{
    char buf[IPV6_ADDR_MAX_STR_LEN];
    mutex_lock(&_lock);
    for (unsigned i = 0; i < ARRAY_SIZE(_entries); i++) {
        internal_entry_t *entry = &_entries[i];

        /* Skip unused entries */
        if (!entry->used) {
            continue;
        }

        /* prints ipv6/prefix | cost */
        printf("%s/%u | %u\n",
               ipv6_addr_to_str(buf, &entry->data.addr, sizeof(buf)),
               entry->data.pfx_len, entry->data.cost);
    }
    mutex_unlock(&_lock);
}
