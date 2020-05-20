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
 * @ingroup     vaina
 * @{
 *
 * @file
 * @author      Locha Mesh Developers (contact@locha.io)
 * @brief       VAINA - Versatile Address Interface | Network Administration
 * @}
 */

#include <Vaina/Vaina.h>
#include <Vaina/Message.h>

#include <cstring>

#include <esp_log.h>

#include "Hex.h"

namespace vaina {

static const char* TAG = "VAINA";

Vaina::Vaina()
    : m_sta_connected()
    , m_ipv4_array()
    , m_mac_array()
    , m_seqno()
{
}

esp_err_t Vaina::staConnected(wifi_event_ap_staconnected_t* info)
{
    mac_length_t hex_mac_address;
    util::bytesToHex(info->mac, hex_mac_address.mac, 6);

    bool is_equal = false;
    if (m_mac_array.size() != 0) {
        for (size_t i = 0; i < m_mac_array.size(); i++) {
            if (strcmp(m_mac_array[i].mac, hex_mac_address.mac) == 0) {
                is_equal = true;
            }
        }
    }

    ESP_LOGI(TAG, "Mac address %s", hex_mac_address.mac);
    if (!is_equal) {
        m_mac_array.push_back(hex_mac_address);
    }

    is_equal = false;


    return ESP_OK;
}

esp_err_t Vaina::staDisconected(wifi_event_ap_stadisconnected_t* info)
{
    mac_length_t hex_mac_address;
    util::bytesToHex(info->mac, hex_mac_address.mac, 6);

    for (size_t i = 0; i < m_sta_connected.size(); i++) {
        if (strcmp(m_sta_connected[i].sta_mac, hex_mac_address.mac) == 0) {
            onDisconnected(m_sta_connected[i]);
            m_sta_connected.erase(m_sta_connected.begin() + i);
        }
    }


    return ESP_OK;
}


void Vaina::setArrayIpv4(esp_ip4_addr_t ipv4)
{
    bool is_equal = false;
    if (m_ipv4_array.size() != 0) {
        for (size_t i = 0; i < m_ipv4_array.size(); i++) {
            ESP_LOGI(TAG, "aquiiii: %d , %d", m_ipv4_array[i].addr, ipv4.addr);
            if (m_ipv4_array[i].addr == ipv4.addr) {
                is_equal = true;
            }
        }
    }
    if (!is_equal) {
        m_ipv4_array.push_back(ipv4);
    }

    is_equal = false;

    for (size_t i = 0; i < m_ipv4_array.size(); i++) {
        sta_data_t sta_client;

        memcpy(sta_client.sta_mac, m_mac_array[i].mac, 13);
        sta_client.sta_ipv4 = ipv4;
        sta_client.sta_ipv6 = ipv4ToIpv6(&ipv4.addr);

        m_sta_connected.push_back(sta_client);
    }

    m_mac_array.clear();
    m_ipv4_array.clear();

    if (!m_sta_connected.empty()) {
        onConnected(m_sta_connected.back());
    }
}


esp_ip6_addr_t Vaina::ipv4ToIpv6(std::uint32_t* ipv4)
{
    esp_ip6_addr_t ipv6;

    ipv6.addr[0] = 0x20000000;
    ipv6.addr[1] = 0;
    ipv6.addr[3] = (std::uint32_t)ipv4;
    ipv6.addr[4] = 0;
    ipv6.zone = 64;
    return ipv6;
}


esp_err_t Vaina::onConnected(sta_data_t client)
{
    MsgBuf buf;

    RcsAdd rcs_add {
        .ip = client.sta_ipv6,
    };
    Message msg(m_seqno.generate(), rcs_add);
    int count = msg.serialize(buf);

    return ESP_OK;
}


esp_err_t Vaina::onDisconnected(sta_data_t client)
{
    MsgBuf buf;

    RcsDel rcs_del {
        .ip = client.sta_ipv6,
    };
    Message msg(m_seqno.generate(), rcs_del);
    int count = msg.serialize(buf);

    return ESP_OK;
}

} // namespace vaina
