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
 * @brief       RFC 5444 compatibility functions
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Gustavo Grisales <gustavosinbandera1@hotmail.com>
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef RFC5444_COMPAT_H
#define RFC5444_COMPAT_H

#include "common/netaddr.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   `ipv6_addr_t` to `struct netaddr`.
 *
 * @pre (@p src != NULL) && (@p dst != NULL)
 *
 * @param[in]  src     Source.
 * @param[in]  pfx_len Prefix length.
 * @param[out] dst     Destination.
 */
void ipv6_addr_to_netaddr(const ipv6_addr_t *src, uint8_t pfx_len, struct netaddr *dst);

/**
 * @brief   `struct netaddr` to `ipv6_addr_t`.
 *
 * @pre (@p src != NULL) && (@p dst != NULL)
 *
 * @param[in]  src     Source.
 * @param[out] dst     Destination.
 * @param[out] pfx_len Prefix length.
 */
void netaddr_to_ipv6_addr(struct netaddr *src, ipv6_addr_t *dst, uint8_t *pfx_len);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* RFC5444_COMPAT_H */
/** @} */
