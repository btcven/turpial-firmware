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
 * @brief       AODVv2 metrics
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Gustavo Grisales <gustavosinbandera1@hotmail.com>
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#include "net/aodvv2/metric.h"

#include <assert.h>

uint8_t aodvv2_metric_link_cost(routing_metric_t metric_type)
{
    switch (metric_type) {
        case METRIC_HOP_COUNT:
            return AODVV2_METRIC_HOP_COUNT_COST;

        default:
            return 0;
    }
}

bool aodvv2_metric_loop_free(routing_metric_t metric_type, uint8_t a,
                             uint8_t b)
{
    switch (metric_type) {
        case METRIC_HOP_COUNT:
            return a <= b;

        /* Undefined for other metric types */
        default:
            return false;
    }
}

uint8_t aodvv2_metric_max(routing_metric_t metric_type)
{
    switch (metric_type) {
        case METRIC_HOP_COUNT:
            return CONFIG_METRIC_HOP_COUNT_AODVV2_MAX;

        default:
            return 0;
    }

    return 0;
}

void aodvv2_metric_update(routing_metric_t metric_type, uint8_t *metric)
{
    assert(metric != NULL);

    switch (metric_type) {
        case METRIC_HOP_COUNT:
            *metric = (*metric) + 1;
            break;

        default:
            break;
    }
}
