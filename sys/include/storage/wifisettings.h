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


#define PASSWORD_LENGTH 64 /**  password length  **/
#define SSID_LENGTH 32 /**  ssid length  **/

/** ap credentials */
typedef struct {
    char ssid[SSID_LENGTH]; /**< ap ssid */
    char password[PASSWORD_LENGTH]; /**< ap password */
} wifi_settings;

/**
 * 
 * @brief    save in the storage the password and ssid of the AP wifi
 * 
 * @param ap_value[in]  ssid nad password  
 * 
 * @return    0:succeed
 * @return   -1:fail
 */ 
int set_ap_credentials(wifi_settings ap_value);

/**
 * 
 * @brief   get ssid and password saved in the storage 
 * 
 * @param ap_value[out]  ssid nad password  
 * 
 * @return    0:succeed
 * @return   -1:fail
 */ 
int get_ap_credentials(wifi_settings* ap_value);

/**
 * 
 * @brief   set global ipv6 in the storage 
 * 
 * @param ip6[in]  ssid nad password  
 * 
 * @return    0:succeed
 * @return   -1:fail
 */ 
int set_global_ipv6(ipv6_addr_t ip6);

/**
 * 
 * @brief   set global ipv6 in the storage 
 * 
 * @param ip6[in]  ssid nad password  
 * 
 * @return    0:succeed
 * @return   -1:fail
 */ 
int get_global_ipv6(ipv6_addr_t* ip6);

#endif /* STORAGE_NVS_WIFISETTINGS_H */



