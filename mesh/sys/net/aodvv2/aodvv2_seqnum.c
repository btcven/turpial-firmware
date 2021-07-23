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
 * @ingroup     aodvv2
 * @{
 *
 * @file
 * @brief       AODVv2 routing protocol
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Locha Mesh developers <contact@locha.io>
 * @}
 */

#include "net/aodvv2/seqnum.h"

#include <stdatomic.h>

static atomic_uint_fast32_t seqnum;

void aodvv2_seqnum_init(void)
{
    /* Initialize to 1 */
    atomic_init(&seqnum, 1);
}

void aodvv2_seqnum_inc(void)
{
    if (atomic_fetch_add(&seqnum, 1) >= 65535) {
        atomic_store(&seqnum, 1);
    }
}

aodvv2_seqnum_t aodvv2_seqnum_get(void)
{
    return atomic_load(&seqnum);
}
