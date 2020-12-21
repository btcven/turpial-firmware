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
 * @brief       AODVv2 Node Sequence Number maintenance
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Gustavo Grisales <gustavosinbandera1@hotmail.com>
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef NET_AODVV2_SEQNUM_H
#define NET_AODVV2_SEQNUM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Sequence number type
 */
typedef uint16_t aodvv2_seqnum_t;

/**
 * @brief   Initializ SeqNum.
 */
void aodvv2_seqnum_init(void);

/**
 * @brief   Increment the SeqNum.
 */
void aodvv2_seqnum_inc(void);

/**
 * @brief   Get the SeqNum.
 */
aodvv2_seqnum_t aodvv2_seqnum_get(void);

/**
 * @brief   Compare sequence numbers
 *
 * @see <a href="https://tools.ietf.org/html/draft-perkins-manet-aodvv2-03#section-4.4">
 *          draft-perkins-manet-aodvv2-03, Section 4.4. Sequence Numbers
 *      </a>
 *
 * @param[in] existing Known sequence number.
 * @param[in] received Newly received sequence number.
 *
 * @return <0 Received sequence number is older.
 * @return >0 Received sequence number is newer.
 * @return 0  Existing sequence number is equal to the received.
 */
static inline int16_t aodvv2_seqnum_cmp(aodvv2_seqnum_t existing,
                                        aodvv2_seqnum_t received)
{
    return (int16_t)((int32_t)received - (int32_t)existing);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* AODVV2_SEQNUM_H */
/** @} */
