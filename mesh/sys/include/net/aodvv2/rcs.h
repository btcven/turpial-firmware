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
 */

#ifndef AODVV2_RCS_H
#define AODVV2_RCS_H

#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Configurable number of maximum entries in the Router Client Set
 * @{
 */
#ifndef CONFIG_AODVV2_RCS_ENTRIES
#define CONFIG_AODVV2_RCS_ENTRIES (2)
#endif
/** @} */

/**
 * @brief   Router Client Set entry
 *
 * @see <a href="https://tools.ietf.org/html/draft-perkins-manet-aodvv2-03#section-4.2">
 *          draft-perkins-manet-aodvv2-03, Section 4.2 Router Client Set
 *      </a>
 */
typedef struct {
    /**
     * @brief An IP address or the start of an address range that requires route
     *        discovery services from the AODVv2 router.
     */
    ipv6_addr_t addr;
    /**
     * @brief The length, in bits, of the routing prefix associated with the
     * IP address.
     *
     * If the prefix length is not equal to the address length of IP address,
     * the AODVv2 router MUST participate in route discovery on behalf of all
     * addresses within that prefix.
     */
    uint8_t pfx_len;
    /**
     * @brief The cost associated with reaching the client's address or address
     *        range.
     */
    uint8_t cost;
} aodvv2_rcs_entry_t;

/**
 * @brief   Initialize Router Client Set
 */
void aodvv2_rcs_init(void);

/**
 * @brief   Add a client to the Router Client Set.
 *
 * @pre @p addr != NULL
 *
 * @param[in] addr          Client IP address.
 * @param[in] prefix_length Length of the routing prefix associated with the
 * address.
 * @param[in] cost          Cost associated with the client.
 *
 * @return NULL The Set is full.
 * @return aodvv2_rcs_entry_t * Pointer to the entry in the client set.
 */
aodvv2_rcs_entry_t *aodvv2_rcs_add(const ipv6_addr_t *addr,
                                   const uint8_t prefix_length,
                                   const uint8_t cost);

/**
 * @brief   Delete a client from the Router Client Set
 *
 * @pre @p addr != NULL
 *
 * @param[in] addr    IPv6 address of the client to remove.
 * @param[in] pfx_len `addr` prefix length.
 */
void aodvv2_rcs_del(const ipv6_addr_t *addr, uint8_t pfx_len);

/**
 * @brief   Find a client in the set.
 *
 * @pre @p addr != NULL
 *
 * @param[in] addr    The client address to be found.
 * @param[in] pfx_len `addr` prefix length.
 *
 * @return Pointer to entry if found.
 * @return NULL Not found.
 */
aodvv2_rcs_entry_t *aodvv2_rcs_matches(const ipv6_addr_t *addr,
                                       uint8_t pfx_len);

/**
 * @brief   Checks if the given IPv6 address matches an entry.
 *
 * @pre @p addr != NULL
 *
 * @param[in] addr The IPv6 address.
 *
 * @return NULL if not found, otherwise pointer to RCS entry.
 */
aodvv2_rcs_entry_t *aodvv2_rcs_is_client(const ipv6_addr_t *addr);

/**
 * @brief   Print RCS entries.
 *
 * @param[in] entry The entry to get printed.
 */
void aodvv2_rcs_print_entries(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* AODVV2_RCS_H */
/** @} */
