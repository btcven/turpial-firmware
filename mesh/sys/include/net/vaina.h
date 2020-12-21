/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_vaina
 * @{
 *
 * @file
 * @brief       VAINA - Versatile Address Interface | Network Administration
 *
 * @author      Locha Mesh developers <developers@locha.io>
 * @}
 */

#ifndef NET_VAINA_H
#define NET_VAINA_H

#include "net/gnrc.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CONFIG_VAINA_PORT
/**
 * @brief  VAINA UDP port
 */
#define CONFIG_VAINA_PORT (1337)
#endif

#ifndef CONFIG_VAINA_MCAST_ADDR
/**
 * @brief   VAINA multicast address
 */
#define CONFIG_VAINA_MCAST_ADDR "ff15::42"
#endif

/**
 * @enum   VAINA message types
 */
enum {
    VAINA_MSG_ACK = 0,  /**< Message acknowledged */
    VAINA_MSG_NACK = 1, /**< Message not acknowledged */
#if IS_USED(MODULE_AODVV2)
    VAINA_MSG_RCS_ADD = 2,  /**< Add entry to Router Client Set */
    VAINA_MSG_RCS_DEL = 3,  /**< Delete entry from Router Client Set */
#endif
    VAINA_MSG_NIB_ADD = 4,  /**< Add entry to NIB */
    VAINA_MSG_NIB_DEL = 5,  /**< Delete entry from NIB */
};

/**
 * @brief   Router Client Set add message
 */
typedef struct {
    uint8_t pfx_len; /**< IP address pfx_len */
    ipv6_addr_t ip; /**< IP address to add */
} vaina_msg_rcs_add_t;

/**
 * @brief   Router Client Set delete message
 */
typedef struct {
    uint8_t pfx_len; /**< IP address pfx_len */
    ipv6_addr_t ip; /**< IP address to delete */
} vaina_msg_rcs_del_t;

/**
 * @brief   NIB add message
 */
typedef struct {
    uint8_t pfx_len; /**< IP address pfx_len */
    ipv6_addr_t ip; /**< IP address to add */
} vaina_msg_nib_add_t;

/**
 * @brief   NIB del message
 */
typedef struct {
    uint8_t pfx_len; /**< IP address pfx_len */
    ipv6_addr_t ip; /**< IP address to delete */
} vaina_msg_nib_del_t;

/**
 * @brief   VAINA message
 */
typedef struct {
    uint8_t msg; /**< Message type */
    uint8_t seqno; /**< Sequence number */
    union {
        vaina_msg_rcs_add_t rcs_add; /**< VAINA_MSG_RCS_ADD */
        vaina_msg_rcs_del_t rcs_del; /**< VAINA_MSG_RCS_DEL */
        vaina_msg_nib_add_t nib_add; /**< VAINA_MSG_NIB_ADD */
        vaina_msg_nib_del_t nib_del; /**< VAINA_MSG_NIB_DEL */
    } payload; /** Payload of the message */
} vaina_msg_t;

/**
 * @brief   Initialize VAINA server.
 */
int vaina_init(gnrc_netif_t *netif);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_VAINA_H */
