/*
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
 * @brief       AODVv2 compile time configuration.
 *
 * @author      Locha Mesh developers <contact@locha.io>
 */

#ifndef AODVV2_CONF_H
#define AODVV2_CONF_H

/**
 * @brief   Active interval value in seconds
 */
#ifndef CONFIG_AODVV2_ACTIVE_INTERVAL
#define CONFIG_AODVV2_ACTIVE_INTERVAL (5)
#endif

#ifndef CONFIG_AODVV2_MAX_IDLETIME
#define CONFIG_AODVV2_MAX_IDLETIME (200)
#endif

#ifndef CONFIG_AODVV2_MAX_BLACKLIST_TIME
#define CONFIG_AODVV2_MAX_BLACKLIST_TIME (200)
#endif

/**
 * @brief   Maximum lifetime for a sequence number in seconds
 */
#ifndef CONFIG_AODVV2_MAX_SEQNUM_LIFETIME
#define CONFIG_AODVV2_MAX_SEQNUM_LIFETIME (300)
#endif

#ifndef CONFIG_AODVV2_RERR_TIMEOUT
#define CONFIG_AODVV2_RERR_TIMEOUT (3)
#endif

#ifndef CONFIG_AODVV2_RTEMSG_ENTRY_TIME
#define CONFIG_AODVV2_RTEMSG_ENTRY_TIME (12)
#endif

#ifndef CONFIG_AODVV2_RREQ_WAIT_TIME
#define CONFIG_AODVV2_RREQ_WAIT_TIME (2)
#endif

#ifndef CONFIG_AODVV2_RREP_ACK_SENT_TIMEOUT
#define CONFIG_AODVV2_RREP_ACK_SENT_TIMEOUT (1)
#endif

#ifndef CONFIG_AODVV2_RREQ_HOLDDOWN_TIME
#define CONFIG_AODVV2_RREQ_HOLDDOWN_TIME (10)
#endif

#endif /* AODVV2_CONF_H */
/** @} */
