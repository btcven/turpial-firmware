/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_manet
 * @{
 *
 * @file
 * @brief       MANET standards helper code
 *
 * @author      Gustavo Grisales <gustavosinbandera1@hotmail.com>
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef NET_MANET_H
#define NET_MANET_H

#include "net/gnrc/netif.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Static initializer for the link-local all MANET-Routers IPv6
 *          address (`ff02:0:0:0:0:0:0:6d`)
 *
 * @see <a href="https://tools.ietf.org/html/rfc5498#section-6">
 *          RFC 5498, section 6
 *      </a>
 */
#define IPV6_ADDR_ALL_MANET_ROUTERS_LINK_LOCAL {{ 0xff, 0x02, 0x00, 0x00, \
                                                  0x00, 0x00, 0x00, 0x00, \
                                                  0x00, 0x00, 0x00, 0x00, \
                                                  0x00, 0x00, 0x00, 0x6d }}

/**
 * @brief   UDP Port for MANET Protocols (udp/269).
 *
 * @see <a href="https://tools.ietf.org/html/rfc5498#section-6">
 *          RFC 5498, section 6
 *      </a>
 */
#define UDP_MANET_PORT     (269)

/**
 * @brief   IP Port for MANET Protocols (138).
 *
 * @see <a href="https://tools.ietf.org/html/rfc5498#section-6">
 *          RFC 5498, section 6
 *      </a>
 */
#define IP_MANET_PROTOCOLS (138)

/**
 * @see @ref IPV6_ADDR_ALL_MANET_ROUTERS_LINK_LOCAL
 */
extern ipv6_addr_t ipv6_addr_all_manet_routers_link_local;

/**
 * @brief   Join a network interface to the LL-MANET-Routers multicast group.
 *
 * @pre @p netif != NULL
 *
 * @param[in] netif The interface.
 *
 * @return 0 on success
 * @return -1 otherwise
 */
int manet_netif_ipv6_group_join(gnrc_netif_t *netif);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_MANET_H */
/** @} */
