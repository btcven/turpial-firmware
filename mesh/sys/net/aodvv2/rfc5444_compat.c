/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * Copyright (C) 2021 Locha Mesh developers <contact@locha.io>
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
 * @brief       RFC5444 Utils
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Locha Mesh devs <contact@locha.io>
 * @}
 */

#include "rfc5444_compat.h"

#include <assert.h>

void ipv6_addr_to_netaddr(const ipv6_addr_t *src, uint8_t pfx_len,
                          struct netaddr *dst)
{
    assert(src != NULL && dst != NULL);

    /* Guard against invalid prefix lengths */
    if (pfx_len > 128) {
        pfx_len = 128;
    }

    dst->_type = AF_INET6;
    dst->_prefix_len = pfx_len;

    memcpy(dst->_addr, src, sizeof(dst->_addr));
}

void netaddr_to_ipv6_addr(struct netaddr *src, ipv6_addr_t *dst,
                          uint8_t *pfx_len)
{
    assert(src != NULL && dst != NULL);

    /* Address is unspecified */
    if (src->_type == AF_UNSPEC) {
        memset(dst, 0, sizeof(ipv6_addr_t));
        *pfx_len = 0;
        return;
    }

    /* Guard against invalid prefix lengths */
    if (src->_prefix_len > 128) {
        *pfx_len = 128;
    }
    else {
        *pfx_len = src->_prefix_len;
    }

    /* Initialize dst with only the needed bits found in the prefix length */
    ipv6_addr_t pfx;
    memcpy(&pfx, src->_addr, sizeof(ipv6_addr_t));

    ipv6_addr_init_prefix(dst, &pfx, *pfx_len);
}
