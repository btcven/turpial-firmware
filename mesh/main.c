/* Copyright 2021 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @{
 * @file
 * @brief       Main radio-firmware file
 *
 * @author      Locha Mesh Developers <developers@locha.io>
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "msg.h"

#include "net/aodvv2.h"
#include "net/manet.h"
#include "net/gnrc/ipv6/nib.h"
#include "net/vaina.h"

#include "shell_extended.h"

static int _init_ieee802154(void);
static int _init_slipdev(void);

/**
 * @brief   Network interfaces PID.
 *
 * These are the available network interfaces on the Turpial device, please
 * note that these values _aren't_ fixed, they _might_ and _will_ change in
 * the future, so don't worry if on the console you get various errors, just
 * update accordingly, this happens because there's still an ongoing process
 * on RIOT to add identifiers to network interfaces so this doesn't has to be
 * done manually in the future.
 *
 * If something has it's own thread and is created before these network
 * interfaces you might want to update them with a fast `ifconfig` and replacing
 * the values yourself here ;).
 *
 * @{
 */
#define IEEE802154_IF (8)
#define SLIPDEV_IF    (7)
/** @} */

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int main(void)
{
    if (_init_ieee802154() < 0) {
        printf("Error: Couldn't initialize IEEE 802.15.4g device correctly.\n");
    }

    if (_init_slipdev() < 0) {
        printf("Error: Couldn't initialize SLIP correctly.\n");
        return -1;
    }

    puts("Welcome to Turpial Radio!");

    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    /* Start shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_extended_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* Should be never reached */
    return 0;
}

static int _init_ieee802154(void)
{
    gnrc_netif_t *ieee802154_netif = gnrc_netif_get_by_pid(IEEE802154_IF);
    if (ieee802154_netif == NULL) {
        printf("Error: Couldn't find IEEE 802.154.g device.\n");
        return -1;
    }

    /* Disable router advertisements on this interface, this interface acts as
     * a router, but only does route IPv6 packets to other nodes, it isn't some
     * sort of generic router which peers should connect to. */
    gnrc_ipv6_nib_change_rtr_adv_iface(ieee802154_netif, false);

    /* Join LL-MANET-Routers multicast group, this is the IPv6 group where we'll
     * be receiving RFC 5444 UDP packets */
    if (manet_netif_ipv6_group_join(ieee802154_netif) < 0) {
        printf("Error: Couldn't join LL-MANET-Routers group\n");
        return -1;
    }

    /* Initialize AODVv2 */
    if (aodvv2_init(ieee802154_netif) < 0) {
        printf("Error: Couldn't initialize AODVv2\n");
        return -1;
    }

    return 0;
}

static int _init_slipdev(void)
{
    gnrc_netif_t *slipdev_netif = gnrc_netif_get_by_pid(SLIPDEV_IF);
    if (slipdev_netif == NULL) {
        printf("Error: We need a wired interface to send packets to.\n");
        return -1;
    }

    /* Disable router advertisements on the SLIP interface to not confuse
     * connected nodes */
    gnrc_ipv6_nib_change_rtr_adv_iface(slipdev_netif, false);

    /* Add known fe80::2 local address so a computer/esp32 can know how to
     * configure the routing table. */
    ipv6_addr_t addr = {
        .u8 = { 0xfe, 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2 }
    };

    if (gnrc_netif_ipv6_addr_add(slipdev_netif, &addr, 128, 0) != sizeof(ipv6_addr_t)) {
        printf("Error: Couldn't setup SLIP local address.\n");
        return -1;
    }

    /* Initialize VAINA config interface */
    if (vaina_init(slipdev_netif) < 0) {
        printf("Error: Couldn't initialize VAINA\n");
        return -1;
    }

    return 0;
}
