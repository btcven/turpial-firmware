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
 * @defgroup    vaina
 * @ingroup     vaina
 * @{
 *
 * @file
 * @author      Locha Mesh Developers (contact@locha.io)
 * @brief       VAINA - Versatile Address Interface | Network Administration
 */

#ifndef VAINA_VAINA_H
#define VAINA_VAINA_H

#include <iostream>

#include <esp_err.h>

#include "Network/WiFi.h"

namespace vaina {

/**
 * @brief   Connected client information
 */
struct sta_data_t {
    esp_ip4_addr_t sta_ipv4; /**< IPv4 address */
    char sta_mac[13];        /**< MAC address */
    esp_ip6_addr_t sta_ipv6; /**< IPv6 address */
};

/**
 * @brief   MAC address
 */
struct mac_length_t {
    char mac[13];
};

/**
 * @brief   Sequence number
 */
class SeqNo {
public:
    /**
     * @brief   Construct a new SeqNo
     */
    SeqNo() : m_seqno(1) {}

    /**
     * @brief    Generate a new sequence number
     */
    std::uint8_t generate() {
        std::uint8_t current = m_seqno;
        m_seqno += 1;
        return current;
    }

private:
    std::uint8_t m_seqno; /**< Current sequence number */
};

/**
 * @brief   VAINA protocol client
 */
class Vaina {
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
     * @brief   Function executed when connecting a new client sta
     *
     * @param[in] info information of new sta connected
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t staConnected(wifi_event_ap_staconnected_t* info);

    /**
     * @brief   Function executed when disconnect a client sta
     *
     * @param[in] info information of sta disconnect
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t staDisconected(wifi_event_ap_stadisconnected_t* info);

    /**
     * @brief   Function executed when is generated new ipv4 address
     *
     * @param[in] info information of sta disconnect
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    void setArrayIpv4(esp_ip4_addr_t ipv4);

    /**
     * @brief   Convert address ipv4 to ipv6
     *
     * @param[in] ipv4 IPv4 address
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_ip6_addr_t ipv4ToIpv6(uint32_t* ipv4);

    /**
     * @brief   Executed when a client connected
     *
     * @param[in] client client data
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t onConnected(sta_data_t client);

    /**
     * @brief   Executed when a client disconnects
     *
     * @param[in] client Client data
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t onDisconnected(sta_data_t client);

private:
    /**
     * @brief   Private constructor for Vaina object
     */
    Vaina();

    std::vector<sta_data_t> m_sta_connected;  /**< All data client array */
    std::vector<esp_ip4_addr_t> m_ipv4_array; /**< IPv4 array */
    std::vector<mac_length_t> m_mac_array;    /**< MAC array */
    SeqNo m_seqno;                            /**< Current sequence number */
};

} // namespace vaina

#endif // VAINA_VAINA_H
/** @} */
