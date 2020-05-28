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
 * @defgroup     storage_wifiSettings
 * @ingroup      storage_wifiSettings
 * @{
 *
 * @file        wifiSettings.h
 * @brief       component created to manage the wifi configuration in memory
 *
 * @author      Locha Mesh Developers(conctact@locha.io)
 * @}
 */



#ifndef STORAGE_NVS_WIFISETTINGS_H
#define STORAGE_NVS_WIFISETTINGS_H

#include "net/gnrc.h"


#define MAX_LENGTH 20

typedef  struct {
    char ssid[MAX_LENGTH];
    char password[MAX_LENGTH];
} wifi_settings;


int set_ap_credentials(wifi_settings ap_value);

int get_ap_credentials(wifi_settings* ap_value);

int set_global_ipv6(ipv6_addr_t ip6);

int get_global_ipv6(ipv6_addr_t* ip6);


#endif /* STORAGE_NVS_WIFISETTINGS_H */



