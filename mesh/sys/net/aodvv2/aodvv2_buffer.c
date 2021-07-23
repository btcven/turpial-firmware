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
 * @brief       AODVv2 packet buffering
 *
 * @author      Locha Mesh developers <contact@locha.io>
 * @}
 */

#include <stdbool.h>

#include "net/aodvv2.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/ipv6.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define CONFIG_AODVV2_MAX_BUFFERED_PACKETS (10)

typedef struct {
    bool used;
    gnrc_pktsnip_t *pkt;
    ipv6_addr_t dst;
} buffered_pkt_t;

static buffered_pkt_t _buffered_pkts[CONFIG_AODVV2_MAX_BUFFERED_PACKETS];

static void _pkt_del(unsigned i)
{
    buffered_pkt_t *entry = &_buffered_pkts[i];
    if (entry->used) {
        entry->used = false;
        entry->pkt = NULL;
        entry->dst = ipv6_addr_unspecified;
    }
}

void aodvv2_buffer_init(void)
{
    memset(_buffered_pkts, 0, sizeof(_buffered_pkts));
}

int aodvv2_buffer_pkt_add(const ipv6_addr_t *dst, gnrc_pktsnip_t *pkt)
{
    for (unsigned i = 0; i < ARRAY_SIZE(_buffered_pkts); i++) {
        buffered_pkt_t *entry = &_buffered_pkts[i];
        /* Find free spot */
        if (!entry->used) {
            entry->used = true;
            entry->pkt = pkt;
            memcpy(&entry->dst, dst, sizeof(ipv6_addr_t));

            /* Increase reference count for this packet as we'll l store it
             * until we find a route to send it (or not, and release the
             * packet) */
            gnrc_pktbuf_hold(entry->pkt, 1);

            return 0;
        }
    }

    /* List of buffered packets is _full_ :/ */
    return -1;
}

void aodvv2_buffer_dispatch(const ipv6_addr_t *targ_addr)
{
    assert(targ_addr != NULL);

    for (unsigned i = 0; i < ARRAY_SIZE(_buffered_pkts); i++) {
        buffered_pkt_t *entry = &_buffered_pkts[i];

        if (ipv6_addr_equal(&entry->dst, targ_addr)) {
            int res = gnrc_netapi_dispatch_send(GNRC_NETTYPE_IPV6,
                                                GNRC_NETREG_DEMUX_CTX_ALL,
                                                entry->pkt);

            if (res < 1) {
                DEBUG("aodvv2: couldn't dispatch packet!\n");
            }
            _pkt_del(i);
        }
    }
}

