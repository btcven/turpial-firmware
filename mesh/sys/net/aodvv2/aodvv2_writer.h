/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * Copyright (C) 2021 btcven Locha Mesh developers <contact@locha.io>
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
 * @brief       AODVVv2 Message Writer
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Locha Mesh devs <contact@locha.io>
 */

#ifndef AODVV2_WRITER_H
#define AODVV2_WRITER_H

#include "net/aodvv2/rfc5444.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Register AODVv2 message writer
 *
 * @param[in] wr      The RFC 5444 writer.
 */
void aodvv2_writer_init(struct rfc5444_writer *wr);

/**
 * @brief   Write a RREQ
 *
 * @pre (@p wr != NULL) && (@p message != NULL)
 *
 * @param[in] wr      The RFC 5444 writer.
 * @param[in] message The RREQ message data.
 *
 * @return 0 on success, otherwise 0< on failure.
 */
int aodvv2_writer_send_rreq(struct rfc5444_writer *wr, aodvv2_message_t *message);

/**
 * @brief   Write a RREP
 *
 * @pre (@p wr != NULL) && (@p message != NULL)
 *
 * @param[in] wr      The RFC 5444 writer.
 * @param[in] message The RREP message data.
 *
 * @return 0 on success, otherwise 0< on failure.
 */
int aodvv2_writer_send_rrep(struct rfc5444_writer *wr, aodvv2_message_t *message);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* AODVV2_WRITER_H */
/** @} */
