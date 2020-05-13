/**
 * @file Vaina.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief VAINA - Versatile Address Interface | Network Administration
 * @version 0.1
 * @date 2020-04-07
 *
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 *
 */


#include "Vaina.h"

#include "Hex.h"
#include <cstring>
#include <esp_log.h>


static const char* TAG = "VAINA";

Vaina::Vaina()
{
    ESP_LOGI(TAG, "execute constructor");
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
            sendUartdisconnected(m_sta_connected[i]);
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
        sendUartConnected(m_sta_connected.back());
    }
}


esp_ip6_addr_t Vaina::ipv4ToIpv6(uint32_t* ipv4)
{
    int first = ipv4[1];
    int second = ipv4[12];
    int third = ipv4[13];
    int fourth = ipv4[14];
    char* result = (char*)malloc(9);


    memset(result, 0, 9);

    strcpy(result, decToHexa(first));
    strcat(result, decToHexa(second));
    strcat(result, decToHexa(third));
    strcat(result, decToHexa(fourth));


    esp_ip6_addr_t ipv6;

    ipv6.addr[0] = 0x20000000;
    ipv6.addr[1] = 0;
    ipv6.addr[3] = *(uint32_t*)&result;
    ipv6.addr[4] = 0;
    ipv6.zone = 64;

    free(result);
    return ipv6;
}


char* Vaina::decToHexa(int n)
{
    // char array to store hexadecimal number
    char hexaDeciNum[100];
    char* result = (char*)malloc(3);

    // counter for hexadecimal number array
    int i = 0;
    while (n != 0) {
        // temporary variable to store remainder
        int temp = 0;

        // storing remainder in temp variable.
        temp = n % 16;

        // check if temp < 10
        if (temp < 10) {
            hexaDeciNum[i] = temp + 48;
            i++;
        } else {
            hexaDeciNum[i] = temp + 55;
            i++;
        }

        n = n / 16;
    }
    result[0] = hexaDeciNum[1];
    result[1] = hexaDeciNum[0];
    result[2] = '\0';

    return result;
    free(result);
}


esp_err_t Vaina::sendUartConnected(sta_data_t client)
{
    std::uint8_t buf[3];
    buf[0] = VAINA_MSG_RCS_ADD;
    buf[1] = VAINA_MSG_RCS_ADD;

    memcpy(&buf[3], &client.sta_ipv6, sizeof(esp_ip6_addr_t));

    //  code to send here

    return ESP_OK;
}


esp_err_t Vaina::sendUartdisconnected(sta_data_t client)
{
    std::uint8_t buf[3];
    buf[0] = VAINA_MSG_RCS_DEL;
    buf[1] = VAINA_MSG_RCS_DEL;

    memcpy(&buf[3], &client.sta_ipv6, sizeof(esp_ip6_addr_t));

    //  code to send here

    return ESP_OK;
}
