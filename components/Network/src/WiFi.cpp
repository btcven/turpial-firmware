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
 * @ingroup     network
 * @{
 *
 * @file
 * @author      Locha Mesh Developers (contact@locha.io)
 * @brief       Wi-Fi network interface
 * @}
 */

#include "Network/WiFi.h"
#include "Network/Network.h"

#include <cstdint>
#include <cstring>

#include <esp_log.h>
#include <esp_wifi.h>
#include <iostream>

#include "Vaina/Vaina.h"

namespace network {

static const char* TAG = "WiFi";

WiFiDefaultEventHandler::WiFiDefaultEventHandler()
{
}

esp_err_t WiFiDefaultEventHandler::staStart()
{
    ESP_LOGI(TAG, "STA Start");

    esp_err_t err = netif_wifi.connect();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't connect to AP (%s)", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

esp_err_t WiFiDefaultEventHandler::staStop()
{
    ESP_LOGI(TAG, "STA Stop");

    return ESP_OK;
}

WiFi::WiFi()
    : m_event_handler()
{
    m_ap_netif = nullptr;
}

esp_err_t WiFi::init()
{
    esp_err_t err;

    m_ap_netif = esp_netif_create_default_wifi_ap();

    ESP_LOGD(TAG, "Initializing Wi-Fi");

    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&init_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init failed, err = %s", esp_err_to_name(err));
        return err;
    }

    err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
        &WiFi::eventHandler, this);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't register Wi-Fi event handler, err = %s", esp_err_to_name(err));
        return err;
    }

    err = esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED,
        &WiFi::ipEventHandler, NULL);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't register Wi-Fi event handler, err = %s", esp_err_to_name(err));
        return err;
    }

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

esp_err_t WiFi::setMode(wifi_mode_t mode)
{
    return esp_wifi_set_mode(mode);
}

esp_err_t WiFi::getMode(wifi_mode_t& mode)
{
    return esp_wifi_get_mode(&mode);
}

esp_err_t WiFi::setApConfig(wifi_config_t& ap_config)
{
    return esp_wifi_set_config(WIFI_IF_AP, &ap_config);
}

esp_err_t WiFi::getApConfig(wifi_config_t& ap_config)
{
    return esp_wifi_get_config(WIFI_IF_AP, &ap_config);
}

esp_err_t WiFi::setStaConfig(wifi_config_t& sta_config)
{
    return esp_wifi_set_config(WIFI_IF_STA, &sta_config);
}

esp_err_t WiFi::getStaConfig(wifi_config_t& sta_config)
{
    return esp_wifi_get_config(WIFI_IF_STA, &sta_config);
}

bool WiFi::isAp()
{
    wifi_mode_t mode;
    esp_err_t err = esp_wifi_get_mode(&mode);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_get_mode failed (%s)", esp_err_to_name(err));
        return false;
    }

    return (mode == WIFI_MODE_APSTA || mode == WIFI_MODE_AP);
}

bool WiFi::isSta()
{
    wifi_mode_t mode;
    esp_err_t err = esp_wifi_get_mode(&mode);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_get_mode failed (%s)", esp_err_to_name(err));
        return false;
    }

    return (mode == WIFI_MODE_APSTA || mode == WIFI_MODE_STA);
}

esp_err_t WiFi::connect()
{
    esp_err_t err;

    wifi_mode_t wifimode;
    err = esp_wifi_get_mode(&wifimode);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_get_mode failed");
        return err;
    }

    if (!isSta()) {
        ESP_LOGE(TAG, "Wi-Fi mode is not STA");
        return ESP_FAIL;
    }

    return esp_wifi_connect();
}

void WiFi::setEventHandler(std::unique_ptr<WiFiEventHandler>&& event_handler)
{
    ESP_LOGD(TAG, "Setting Wi-Fi event handler");

    m_event_handler.setNextHandler(std::move(event_handler));
}

esp_err_t WiFi::stop()
{
    esp_err_t err = esp_wifi_stop();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_stop failed, err = %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

void WiFi::eventHandler(void* event_handler_arg,
    esp_event_base_t event_base,
    std::int32_t event_id,
    void* event_data)
{
    ESP_LOGI(TAG, "Wi-Fi Event Handler Called");

    WiFi* wifi = reinterpret_cast<WiFi*>(event_handler_arg);

    esp_err_t err = wifi->m_event_handler.eventDispatcher(event_id, event_data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Event handler error, err = %s", esp_err_to_name(err));
    }
}

esp_err_t WiFi::getConnectedList(wifi_sta_list_t& sta)
{
    return esp_wifi_ap_get_sta_list(&sta);
}

void WiFi::ipEventHandler(void* event_handler_arg,
    esp_event_base_t event_base,
    std::int32_t event_id,
    void* event_data)
{
    ip_event_got_ip_t* event = (ip_event_got_ip_t*)event_data;

    vaina::Vaina& vaina = vaina::Vaina::getInstance();

    ESP_LOGI(TAG, "got ip the  event:" IPSTR, IP2STR(&event->ip_info.ip));
    vaina.setArrayIpv4(event->ip_info.ip);
}


} // namespace network
