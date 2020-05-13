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
 * 
 * @file Vaina.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief VAINA - Versatile Address Interface | Network Administration
 */


#ifndef VAINA_H
#define VAINA_H

#include "WiFi.h"
#include <esp_err.h>
#include <iostream>

/**
 * 
 * @brief client structure
 */
struct sta_data_t {
    esp_ip4_addr_t sta_ipv4; // ipv4 address
    char sta_mac[13];        // mac address
    esp_ip6_addr_t sta_ipv6; // ipv6 address
};


struct mac_length_t {
    char mac[13];
};

/**
                    
 * @enum   VAINA message types
 */
enum {
    VAINA_MSG_ACK = 0,     /**< Message acknowledged */
    VAINA_MSG_RCS_ADD = 2, /**< Add entry to Router Client Set */
    VAINA_MSG_RCS_DEL = 3, /**< Delete entry from Router Client Set */
    VAINA_MSG_NIB_ADD = 4, /**< Add entry to NIB */
    VAINA_MSG_NIB_DEL = 5, /**< Delete entry from NIB */
};


class Vaina
{
private:
    Vaina();

    std::vector<sta_data_t> m_sta_connected;  // all data client array
    std::vector<esp_ip4_addr_t> m_ipv4_array; // ipv4 array
    std::vector<mac_length_t> m_mac_array;    // mac array
    /* data */
public:
    friend network::WiFi

    Vaina(Vaina const&) = delete; // Copy construct
    Vaina(Vaina&&) = delete;      // Move construct

    Vaina& operator=(Vaina const&) = delete; // Copy assign
    Vaina& operator=(Vaina&&) = delete;      // Move assign

    /**
     * @brief Get the unique instance of the object
     *
     * @return Vaina&
     */
    static Vaina& getInstance()
    {
        static Vaina instance;
        return instance;
    }
    /**
     * 
     * @brief function executed when connecting a new client sta
     * 
     * @param[in] info information of new sta connected 
     * 
     * @return esp_err_t
     */
    esp_err_t staConnected(wifi_event_ap_staconnected_t* info);

    /**
     * 
     * @brief function executed when disconnect a client sta
     * @param[in] info information of sta disconnect
     * 
     * @return esp_err_t
     */
    esp_err_t staDisconected(wifi_event_ap_stadisconnected_t* info);
    /**
     * 
     * @brief function executed when is generated new ipv4 address
     * @param[in] info information of sta disconnect
     * 
     * @return esp_err_t
     */
    void setArrayIpv4(esp_ip4_addr_t ipv4);
    /**
     * 
     * @brief convert address ipv4 to ipv6
     * @param[in] ipv4 ipv4 address
     * 
     * @return esp_err_t
     */
    esp_ip6_addr_t ipv4ToIpv6(uint32_t* ipv4);
    /**
     * 
     * @brief function to convert of decimal to hex 
     * @param[in] n decimal numbers
     *
     * @return char*
     */
    char* decToHexa(int n);
    /**
     * @brief executed when a client connected 
     * @param[in] client client data
     * 
     * @return esp_err_t
     */
    esp_err_t sendUartConnected(sta_data_t client);
    /**
     * @brief executed when a client disconnect
     * @param[in] client client data
     * 
     * @return esp_err_t
     */
    esp_err_t sendUartdisconnected(sta_data_t client);
};


#endif // end vaina
