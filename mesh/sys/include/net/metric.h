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
 * @ingroup     net
 * @{
 *
 * @file
 * @brief       RFC 6551
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Gustavo Grisales <gustavosinbandera1@hotmail.com>
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef NET_METRIC_H
#define NET_METRIC_H

/*
 * @brief   Routing metric types.
 *
 * @see <a href="https://tools.ietf.org/html/rfc6551#section-6.1">
 *          RFC 6551, section 6.1 "Routing Metric/Constraint type"
 *      </a>
 *
 * @note Extend to include alternate metrics.
 */
typedef enum {
     METRIC_NODE_STATE_AND_ATTRIBUTE = 1, /**< Node State and Attribute */
     METRIC_NODE_ENERGY = 2,              /**< Node Energy */
     METRIC_HOP_COUNT = 3,                /**< Hop Count */
     METRIC_LINK_TROUGHPUT = 4,           /**< Link Throughput */
     METRIC_LINK_LATENCY = 5,             /**< Link Latency */
     METRIC_LINK_QUALITY_LEVEL = 6,       /**< Link Quality Level */
     METRIC_LINK_ETX = 7,                 /**< Link ETX*/
     METRIC_LINK_COLOR = 8,               /**< Link Color */
} routing_metric_t;

#endif /* NET_METRIC_H */
