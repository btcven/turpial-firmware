/*
 * Copyright (C) 2021 btcven Locha Mesh developers
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
 * @brief       Demonstrating the usage of the AODVv2 API
 *
 * @author      Locha Mesh devs <contact@locha.io>
 *
 * @}
 */

#include "kernel_defines.h"

#if IS_USED(MODULE_AODVV2)

#include <stdio.h>

#include "net/aodvv2.h"

int find_route_cmd(int argc, char **argv)
{
    int res = 0;

    if (argc < 3) {
        puts("find_route <target> <orig>");
        puts("find a route using AODVv2 protocol to <target>");
        goto exit;
    }

    /* Parse <target> */
    ipv6_addr_t target_addr;
    if (ipv6_addr_from_str(&target_addr, argv[1]) == NULL) {
        res = -1;
        printf("%s: invalid <target>!\n", argv[0]);
        goto exit;
    }

    ipv6_addr_t orig_addr;
    if (ipv6_addr_from_str(&orig_addr, argv[2]) == NULL) {
        res = -1;
        printf("%s: invalid <orig>!\n", argv[0]);
        goto exit;
    }

    res = aodvv2_find_route(&orig_addr, &target_addr);
    if (res < 0) {
        printf("%s: failed!\n", argv[0]);
        goto exit;
    }

exit:
    return res;
}

#endif
