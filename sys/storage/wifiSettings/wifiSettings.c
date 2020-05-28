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
 * @file        wifiSettings.c
 * @brief       component created to manage the wifi configuration in memory
 *
 * @author      Locha Mesh Developers(conctact@locha.io)
 * @}
 */
#define ENABLE_DEBUG (1)
#include "debug.h"
#include "storage/wifiSettings.h"
#include "storage/nvs.h"

int set_ap_credentials(wifi_settings ap_value){

   int err = nvs_set_string("ap_ssid", ap_value.ssid);
   if(err != 0){
       printf("Error: could not save the ssid");
   }

   err = nvs_set_string("ap_password", ap_value.password);
   if(err != 0){
       printf("Error: could not save the password");
   }

   return 0;
}


int get_ap_credentials(wifi_settings *ap_value){
    
    size_t length = MAX_LENGTH;
    int err = nvs_get_string("ap_ssid", ap_value->ssid, &length);
    if(err != 0){
       printf("Error: could not get the ssid");
    }

    err = nvs_get_string("ap_password", ap_value->password, &length);
    if(err != 0){
       printf("Error: could not get the ap_password");
    }

    return err;
}


int set_global_ipv6(ipv6_addr_t ip6){

    int err = set_blob("global_ipv6", ip6.u8, sizeof(ipv6_addr_t));
    if(err != 0){
        printf("Error: could not set blob");
    }
    return err;
}

int get_global_ipv6(ipv6_addr_t* ip6){

    uint8_t value[16];
    memset(value, 0, sizeof(value));
    
    int err = get_blob("global_ipv6", value, sizeof(ipv6_addr_t));
    if(err != 0){
        printf("Error: could not get blob");
    }

    memcpy(ip6->u8, value, sizeof(ipv6_addr_t));

    return err;
}

