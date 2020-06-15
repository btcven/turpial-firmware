/**
 * Copyright 2020 btcven and Locha Mesh developers
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
 * @defgroup     storage_tfsettings
 * @ingroup      storage_tfsettings
 * @{
 *
 * @file
 * @brief       Turpial firmware persistent settings.
 *
 * @author      Locha Mesh Developers(conctact@locha.io)
 */

#ifndef STORAGE_TFSETTINGS_H
#define STORAGE_TFSETTINGS_H

#include <stdint.h>

#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_PASSWORD_LENGTH (64) /**< WiFi Password length  */
#define WIFI_SSID_LENGTH     (32) /**< WiFi SSID length */

/**
 * @brief   WiFi configuration
 */
typedef struct {
    uint8_t ssid[WIFI_SSID_LENGTH];         /**< WiFi AP SSID */
    uint8_t password[WIFI_PASSWORD_LENGTH]; /**< WiFi AP Password */
    ipv6_addr_t glb;                        /**< Global Unique Local Address */
    uint8_t glb_pfx_len;                    /**< Prefix length for glb */
} tfsettings_ap_t;

/**
 *
 * @brief   Store Turpial AP configuration.
 *
 * @param[in] settings AP settings.
 *
 * @return 0 on succeess
 * @return 0< on failure
 */
int tfsettings_set_ap(tfsettings_ap_t *settings);

/**
 * @brief   Retrieve Turpial AP configuration.
 *
 * @param[out] settings AP settings.
 *
 * @return 0 on succeess
 * @return 0< on failure
 */
int tfsettings_get_ap(tfsettings_ap_t *settings);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* STORAGE_TFSETTINGS_H */
/** @} */
