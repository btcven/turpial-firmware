/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     shell_extended
 * @{
 *
 * @file
 * @brief       Extended shell commands
 *
 * @author      Gustavo Grisales <gustavosinbandera1@hotmail.com>
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 *
 * @}
 */

#include "shell_extended.h"

#include "kernel_defines.h"

#if IS_USED(MODULE_AODVV2)
int find_route_cmd(int argc, char **argv);
int seqnum_get_cmd(int argc, char **argv);
#endif

#if IS_USED(MODULE_GNRC_UDP)
int udp_cmd(int argc, char **argv);
#endif

#if IS_USED(MODULE_AODVV2)
int sc_aodvv2_cmd(int argc, char **argv);
#endif

const shell_command_t shell_extended_commands[] = {
#if IS_USED(MODULE_AODVV2)
    { "find_route", "find a route to a node using IPv6 address", find_route_cmd },
    { "seqnum_get", "get (and increment) a SeqNum", seqnum_get_cmd },
#endif
#if IS_USED(MODULE_GNRC_UDP)
    { "udp", "send data over UDP and listen on UDP ports", udp_cmd },
#endif
#if IS_USED(MODULE_AODVV2)
    { "aodvv2", "AODVv2 routing protocol command", sc_aodvv2_cmd },
#endif
    { NULL, NULL, NULL }
};
