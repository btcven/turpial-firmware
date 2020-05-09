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
#include <esp_log.h>
#include <iostream>

static const char* TAG = "VAINA";

Vaina::Vaina()
{
    ESP_LOGI(TAG, "execute constructor");
}


esp_err_t Vaina::staConnected(wifi_event_ap_staconnected_t* info)
{
    char hex_mac_address[13];
    util::bytesToHex(info->mac, hex_mac_address, 6);
    std::cout << "mac address: " << hex_mac_address << std::endl;

    network::WiFi& wifi_instance = network::WiFi::getInstance();

    vTaskDelay(10000 / portTICK_PERIOD_MS);
    wifi_instance.getApIpAddress();

    return ESP_OK;
}

esp_err_t Vaina::staDisconected(wifi_event_ap_stadisconnected_t* info)
{
    return ESP_OK;
}
