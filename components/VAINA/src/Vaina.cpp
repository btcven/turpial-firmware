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

    ESP_LOGI(TAG, "hello:!!!!!!!!!!!!! %s", hex_mac_address.mac);
    if (!is_equal) {
        m_mac_array.push_back(hex_mac_address);
    }

    is_equal = false;


    return ESP_OK;
}

esp_err_t Vaina::staDisconected(wifi_event_ap_stadisconnected_t* info)
{
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
        std::cout << "address: ipv4 sin formato" << ipv4.addr << std::endl;
        ESP_LOGI(TAG, "Mac address %s", m_mac_array[i].mac);
        ipv4ToIpv6(&ipv4.addr);
    }
}


void Vaina::ipv4ToIpv6(uint32_t* ipv4)
{
    int first = ipv4[1];
    int second = ipv4[12];
    int third = ipv4[13];
    int fourth = ipv4[14];

    std::cout << first << std::endl;
    std::cout << second << std::endl;
    std::cout << third << std::endl;
    std::cout << fourth << std::endl;

    decToHexa(first);
}


void Vaina::decToHexa(int n)
{
    // char array to store hexadecimal number
    char hexaDeciNum[100];
    char result[2];

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

    // printing hexadecimal number array in reverse order
    for (int j = i - 1; j >= 0; j--) {
        result[j] = hexaDeciNum[j];
    }
    result[3] = '\0';
}
