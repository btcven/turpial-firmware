/* Copyright 2020 Locha Inc
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

#include "kernel_defines.h"

#if IS_USED(MODULE_AODVV2)

#include <stdio.h>

#include "net/aodvv2/rcs.h"

/** Default prefix length if not specified */
#define _IPV6_DEFAULT_PREFIX_LEN (64U)

static uint8_t _get_pfx_len(char *addr)
{
    int pfx_len = ipv6_addr_split_int(addr, '/', _IPV6_DEFAULT_PREFIX_LEN);

    if (pfx_len < 1) {
        pfx_len = _IPV6_DEFAULT_PREFIX_LEN;
    }

    return pfx_len;
}

static void _rcs_add_usage(char *cmd_name)
{
    printf("usage: %s rcs add <address>[/prefix]\n", cmd_name);
}

static int _rcs_add(int argc, char **argv)
{
    (void)argc;

    char *addr_str = argv[0];

    uint8_t pfx_len = _get_pfx_len(addr_str);

    ipv6_addr_t addr;
    if (ipv6_addr_from_str(&addr, addr_str) == NULL) {
        printf("error: unable to parse IPv6 address\n");
        return 1;
    }

    if (aodvv2_rcs_add(&addr, pfx_len, 1) == NULL) {
        printf("error: unable to add client to RCS\n");
        return 1;
    }
    else {
        printf("success: added client to the RCS\n");
    }

    return 0;
}

int sc_aodvv2_cmd(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: %s [rcs]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "rcs") == 0) {
        if (argc == 2) {
            aodvv2_rcs_print_entries();
        }
        else if (strcmp(argv[2], "add") == 0) {
            if (argc < 4) {
                _rcs_add_usage(argv[0]);
                return 1;
            }

            return _rcs_add(argc - 3, argv + 3);
        }
        else {
            puts("error: invalid command");
        }
    }
    else {
        puts("error: invalid command");
    }
    return 0;
}

#endif /* IS_USED(MODULE_AODVV2) */
