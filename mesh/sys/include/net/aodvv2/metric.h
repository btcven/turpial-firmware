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
 * @brief       RFC 6551 metric calculation functions for AODVv2
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Gustavo Grisales <gustavosinbandera1@hotmail.com>
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef NET_AODVV2_METRIC_H
#define NET_AODVV2_METRIC_H

#include <stdint.h>
#include <stdbool.h>

#include "net/metric.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Default metric for AODVv2
 * @{
 */
#ifndef CONFIG_AODVV2_DEFAULT_METRIC
#define CONFIG_AODVV2_DEFAULT_METRIC (METRIC_HOP_COUNT)
#endif
/** @} */

/**
 * @name    Maximum value for "Hop Count" metric
 * @{
 */
#ifndef CONFIG_METRIC_HOP_COUNT_AODVV2_MAX
#define CONFIG_METRIC_HOP_COUNT_AODVV2_MAX (255)
#endif
/** @} */

#define AODVV2_METRIC_HOP_COUNT_COST (1) /**< Cost for "Hop Count" metric */

/**
 * @brief   Link cost for the given metric type. Cost(L)
 *
 * @param[in] metric_type Metric type.
 *
 * @return Cost associated with the metric.
 */
uint8_t aodvv2_metric_link_cost(routing_metric_t metric_type);

/**
 * @brief   Analyzes if a route is loop free given the metric. LoopFree(R1, R2)
 *
 * @see <a href="https://tools.ietf.org/html/draft-perkins-manet-aodvv2-03#section-5">
 *          draft-perkins-manet-aodvv2-03, section 5. "Metrics"
 *      </a>
 *
 * @param[in] metric_type Metric type.
 * @param[in] a           Cost(R1) to check.
 * @param[in] b           Cost(R2) to check.
 *
 * @return true  Free of loops.
 * @return false Possibly a loop.
 */
bool aodvv2_metric_loop_free(routing_metric_t metric_type, uint8_t a,
                             uint8_t b);

/**
 * @brief   Maximum value for the given metric type
 *
 * @param[in] metric_type Metric type.
 *
 * @return Maximum value associated with the metric.
 */
uint8_t aodvv2_metric_max(routing_metric_t metric_type);

/**
 * @brief   Update the value of th provided metric.
 *
 * @pre @p metric != NULL
 *
 * @param[in] metric_type The type of the metric to update.
 * @param[inout] metric   The current value of the metric which will be updated.
 */
void aodvv2_metric_update(routing_metric_t metric_type, uint8_t *metric);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_AODVV2_METRIC_H */
/** @} */
