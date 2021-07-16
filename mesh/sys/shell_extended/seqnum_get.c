/*
 * Copyright (C) 2021 btcven and Locha Mesh developers
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
 * @brief       Demonstrating management of AODVv2 sequence numbers
 *
 * @author      Locha Mesh devs <contact@locha.io>
 *
 * @}
 */

#include "kernel_defines.h"

#if IS_USED(MODULE_AODVV2)

#include <stdio.h>

#include "net/aodvv2/seqnum.h"

int seqnum_get_cmd(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    aodvv2_seqnum_t old = aodvv2_seqnum_get();
    aodvv2_seqnum_inc();
    aodvv2_seqnum_t new = aodvv2_seqnum_get();

    printf("old = %d, new = %d\n", old, new);

    return 0;
}

#endif
