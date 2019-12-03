/**
 * @file WiFi.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "WiFi.h"

#include <cstdint>
#include <cstring>

#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_wifi.h>

namespace network {

static const char* TAG = "WiFi";

void copy_bytes(std::uint8_t* dest, const char* src, std::size_t max)
{
    std::size_t len = std::strlen(src);
    if (len > (max - 1)) {
        ESP_LOGW(TAG, "Field too lage (max is %d), trimming...", max);
        len = (max - 1);
    }
    std::memcpy(dest, src, len);
    dest[len] = '\0';
}

WiFiDefaultEventHandler::WiFiDefaultEventHandler()
{
}

esp_err_t WiFiDefaultEventHandler::staStart()
{
    ESP_LOGI(TAG, "STA Start");

    WiFi& wifi = WiFi::getInstance();
    return wifi.connect();
}

WiFi::WiFi()
    : m_event_handler(nullptr)
{
}

esp_err_t WiFi::init()
{
    esp_err_t err;

    ESP_LOGD(TAG, "Init TCP/IP adapter");
    tcpip_adapter_init();

    ESP_LOGD(TAG, "Initializing Wi-Fi");

    esp_event_loop_init(&WiFi::eventHandler, this);

    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&init_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init failed, err = %s", esp_err_to_name(err));
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

esp_err_t WiFi::setApConfig(APConfig& ap_config)
{
    const std::uint8_t BROADCAST_SSID = 0;

    wifi_config_t wifi_config = {};

    copy_bytes(wifi_config.ap.ssid, ap_config.ssid, 32);
    copy_bytes(wifi_config.ap.password, ap_config.password, 64);
    wifi_config.ap.channel = ap_config.channel;
    wifi_config.ap.authmode = ap_config.authmode;
    wifi_config.ap.ssid_hidden = BROADCAST_SSID;
    wifi_config.ap.max_connection = ap_config.max_conn;

    return esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
}

esp_err_t WiFi::setApConfig(wifi_config_t& ap_config)
{
    return esp_wifi_set_config(WIFI_IF_AP, &ap_config);
}

esp_err_t WiFi::setStaConfig(STAConfig& sta_config)
{
    const std::uint8_t UNKNOWN_CHANNEL = 0;
    const std::uint8_t DEFAULT_LISTEN_INTERVAL = 0;

    wifi_config_t wifi_config = {};

    copy_bytes(wifi_config.sta.ssid, sta_config.ssid, 32);
    copy_bytes(wifi_config.sta.password, sta_config.password, 64);
    wifi_config.sta.scan_method = WIFI_FAST_SCAN;
    wifi_config.sta.bssid_set = false;
    wifi_config.sta.channel = UNKNOWN_CHANNEL;
    wifi_config.sta.listen_interval = DEFAULT_LISTEN_INTERVAL;
    wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SECURITY;

    return esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
}

esp_err_t WiFi::setStaConfig(wifi_config_t& sta_config)
{
    return esp_wifi_set_config(WIFI_IF_STA, &sta_config);
}

esp_err_t WiFi::getApConfig(wifi_config_t& ap_config)
{
    return esp_wifi_get_config(WIFI_IF_AP, &ap_config);
}

esp_err_t WiFi::getStaConfig(wifi_config_t& sta_config)
{
    return esp_wifi_get_config(WIFI_IF_STA, &sta_config);
}

esp_err_t WiFi::start()
{
    esp_err_t err;

    ESP_LOGD(TAG, "Starting Wi-Fi");

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed");
        return err;
    }

    return ESP_OK;
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

    if (wifimode == WIFI_MODE_APSTA ||
        wifimode == WIFI_MODE_STA) {
        err = esp_wifi_connect();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_wifi_connect failed");
            return err;
        }

        return ESP_OK;
    } else {
        ESP_LOGE(TAG, "Wi-Fi mode is not STA");
        return ESP_FAIL;
    }
}

void WiFi::setEventHandler(std::unique_ptr<WiFiEventHandler>&& event_handler)
{
    ESP_LOGD(TAG, "Setting Wi-Fi event handler");

    m_event_handler = std::move(event_handler);
}

esp_err_t WiFi::stop()
{
    return esp_wifi_stop();
}

esp_err_t WiFi::eventHandler(void* ctx, system_event_t* event)
{
    ESP_LOGD(TAG, "Wi-Fi Event Handler Called");
    WiFi* wifi = reinterpret_cast<WiFi*>(ctx);

    if (wifi->m_event_handler) {
        esp_err_t err = wifi->m_event_handler->eventDispatcher(event);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Event handler error, err = %s", esp_err_to_name(err));
            return err;
        }
    }

    return ESP_OK;
}

} // namespace network