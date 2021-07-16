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
 * @ingroup     net_aodvv2
 * @{
 *
 * @file
 * @brief       AODVV2
 *
 * @author      Lotte Steenbrink <lotte.steenbrink@fu-berlin.de>
 * @author      Locha Mesh devs <contact@locha.io>
 */

#ifndef NET_AODVV2_AODVV2_H
#define NET_AODVV2_AODVV2_H

#include "net/aodvv2/conf.h"
#include "net/aodvv2/rfc5444.h"
#include "net/ipv6/addr.h"
#include "net/gnrc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   IPC message to send a RREQ
 */
#define AODVV2_MSG_TYPE_SEND_RREQ (0x9000)

/**
 * @brief   IPC message to send a RREP
 */
#define AODVV2_MSG_TYPE_SEND_RREP (0x9001)

typedef struct {
    aodvv2_message_t pkt; /**< Packet to send */
    ipv6_addr_t next_hop; /**< Next hop */
} aodvv2_msg_t;

/**
 * @brief   Initialize and start RFC5444
 *
 * @pre @p netif != NULL
 *
 * @return PID of the RFC5444 thread
 * @reutnr negative value on error
 */
int aodvv2_init(gnrc_netif_t *netif);

/**
 * @brief   Send a RREQ
 *
 * @pre (@p pkt != NULL) && (@p next_hop != NULL)
 *
 * @param[in] pkt      The RREQ packet.
 * @param[in] next_hop Where to send the packet.
 *
 * @return Negative number on failure, otherwise succeed.
 */
int aodvv2_send_rreq(aodvv2_message_t *pkt, ipv6_addr_t *next_hop);

/**
 * @brief   Send a RREP
 *
 * @pre @p netif != NULL
 *
 * @param[in] pkt      The RREQ packet.
 * @param[in] next_hop Where to send the packet.
 *
 * @return Negative number on failure, otherwise succeed.
 */
int aodvv2_send_rrep(aodvv2_message_t *pkt, ipv6_addr_t *next_hop);

/**
 * @brief   Initiate a route discovery process to find the given address.
 *
 * @pre @p target_addr != NULL && @p orig_addr != NULL
 *
 * @param[in] target_addr The IP address where we want a route to.
 *
 * @return Negative number on failure, otherwise succeed.
 */
int aodvv2_find_route(const ipv6_addr_t *orig_addr,
                      const ipv6_addr_t *target_addr);

/**
 * @brief   Initialize the AODVv2 packer buffering code.
 */
void aodvv2_buffer_init(void);

/**
 * @brief   Add a packet to the packet buffer
 *
 * @pre @p dst != NULL && @p pkt != NULL
 *
 * @brief[in] dst Packet destination address.
 * @brief[in] pkt Packet.
 */
int aodvv2_buffer_pkt_add(const ipv6_addr_t *dst, gnrc_pktsnip_t *pkt);

/**
 * @brief   Dispatch buffered packets to `targ_addr`
 *
 * @notes Only call this when a route to `targ_addr` is on the NIB
 *
 * @param[in] targ_addr Target address to dispatch packets.
 */
void aodvv2_buffer_dispatch(const ipv6_addr_t *targ_addr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_AODVV2_AODVV2_H */
/** @} */
