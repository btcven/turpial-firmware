/**
 * @ingroup     net_vaina
 * @{
 *
 * @file
 * @brief       VAINA - Versatile Address Interface | Network Administration
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 * @}
 */

#ifndef NET_VAINA_H
#define NET_VAINA_H

#include "net/gnrc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  VAINA UDP port
 */
#ifndef CONFIG_VAINA_PORT
#define CONFIG_VAINA_PORT (1337)
#endif

/**
 * @brief   VAINA multicast address
 */
#ifndef CONFIG_VAINA_MCAST_ADDR
#define CONFIG_VAINA_MCAST_ADDR "ff15::42"
#endif

/**
 * @enum   VAINA message types
 */
enum {
    VAINA_MSG_ACK = 0,  /**< Message acknowledged */
    VAINA_MSG_NACK = 1, /**< Message not acknowledged */
    VAINA_MSG_RCS_ADD = 2,  /**< Add entry to Router Client Set */
    VAINA_MSG_RCS_DEL = 3,  /**< Delete entry from Router Client Set */
    VAINA_MSG_NIB_ADD = 4,  /**< Add entry to NIB */
    VAINA_MSG_NIB_DEL = 5,  /**< Delete entry from NIB */
};

/**
 * @brief   Router Client Set add message
 */
typedef struct {
    ipv6_addr_t ip; /**< IP address to add */
} vaina_msg_rcs_add_t;

/**
 * @brief   Router Client Set delete message
 */
typedef struct {
    ipv6_addr_t ip; /**< IP address to delete */
} vaina_msg_rcs_del_t;

/**
 * @brief   NIB add message
 */
typedef struct {
    uint8_t prefix; /**< IP address prefix */
    ipv6_addr_t ip; /**< IP address to add */
} vaina_msg_nib_add_t;

/**
 * @brief   NIB del message
 */
typedef struct {
    uint8_t prefix; /**< IP address prefix */
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
 * @brief   Get a new sequence number
 */
uint8_t vaina_seqno(void);

/**
 * @brief   Send a VAINA message.
 *
 * @param[in] msg The VAINA message.
 *
 * @return 0< on error.
 */
int vaina_client_send(vaina_msg_t *msg);

/**
 * @brief   Initialize VAINA client.
 *
 * @return 0< on error.
 * @return kernel_pid_t on success.
 */
kernel_pid_t vaina_client_init(gnrc_netif_t *netif);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_VAINA_H */
