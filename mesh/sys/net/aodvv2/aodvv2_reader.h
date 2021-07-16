/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2014 Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * Copyright (C) 2021 Locha Mesh developers
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

#ifndef AODVV2_READER_H
#define AODVV2_READER_H

#include "net/aodvv2/rfc5444.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Register AODVv2 message reader
 *
 * @param[in] reader Pointer to the reader context.
 */
void aodvv2_reader_init(struct rfc5444_reader *reader, kernel_pid_t netif_pid);

/**
 * @brief   Sets the sender address
 *
 * @notes MUST be called before starting to parse the packet.
 *
 * @param[in] sender The address of the sender.
 */
void aodvv2_rfc5444_handle_packet_prepare(ipv6_addr_t *sender);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* AODVV2_READER_H */
/** @} */
