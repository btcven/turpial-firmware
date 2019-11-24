/**
 * @file WiFiMode.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "WiFiMode.h"

#include <cstdint>
#include <cstring>

#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_wifi.h"


namespace wifi {

static const char* TAG = "WiFiMode";

WiFiMode::WiFiMode() : m_p_wifi_event_handler(nullptr)
{
}

WiFiMode::~WiFiMode()
{
    if (m_p_wifi_event_handler != nullptr) {
        delete m_p_wifi_event_handler;
        m_p_wifi_event_handler = nullptr;
    }
}

void WiFiMode::setWiFiEventHandler(WiFiEventHandler* wifiEventHandler)
{
    ESP_LOGD(TAG, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> setWifiEventHandler: 0x%d>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>", (uint32_t)wifiEventHandler);
    this->m_p_wifi_event_handler = wifiEventHandler;
    ESP_LOGD(TAG, "<< ***************************************************setWifiEventHandler*************************************");
}

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

esp_err_t WiFiMode::init(bool use_nvs)
{
    esp_err_t err;

    esp_event_loop_init(&WiFiMode::eventHandler, this);

    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    if (!use_nvs) {
        ESP_LOGD(TAG, "Disabling NVS in Wi-Fi");
        init_config.nvs_enable = false;
    }

    ESP_LOGD(TAG, "Initializing Wi-Fi library");
    err = esp_wifi_init(&init_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init failed!");
        return err;
    }

    if (!use_nvs) {
        ESP_LOGD(TAG, "Using RAM as storage mode for Wi-Fi");
        err = esp_wifi_set_storage(WIFI_STORAGE_RAM);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Couldn't set RAM as Wi-Fi storage");
            return err;
        }
    }

    return ESP_OK;
}

esp_err_t WiFiMode::set_mode(wifi_mode_t mode)
{
    return esp_wifi_set_mode(mode);
}

esp_err_t WiFiMode::set_ap_config(APConfig& ap_config)
{
    const std::uint8_t BROADCAST_SSID = 0;

    wifi_config_t wifi_config;
    std::memset(&wifi_config, 0, sizeof(wifi_config_t));

    copy_bytes(wifi_config.ap.ssid, ap_config.ssid, 32);
    copy_bytes(wifi_config.ap.password, ap_config.password, 64);
    wifi_config.ap.channel = ap_config.channel;
    wifi_config.ap.authmode = ap_config.authmode;
    wifi_config.ap.ssid_hidden = BROADCAST_SSID;
    wifi_config.ap.max_connection = ap_config.max_conn;

    return esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
}

esp_err_t WiFiMode::set_sta_config(STAConfig& sta_config)
{
    const std::uint8_t UNKNOWN_CHANNEL = 0;
    const std::uint8_t DEFAULT_LISTEN_INTERVAL = 0;

    wifi_config_t wifi_config;
    std::memset(&wifi_config, 0, sizeof(wifi_config_t));

    copy_bytes(wifi_config.sta.ssid, sta_config.ssid, 32);
    copy_bytes(wifi_config.sta.password, sta_config.password, 64);
    wifi_config.sta.scan_method = WIFI_FAST_SCAN;
    wifi_config.sta.bssid_set = false;
    wifi_config.sta.channel = UNKNOWN_CHANNEL;
    wifi_config.sta.listen_interval = DEFAULT_LISTEN_INTERVAL;
    wifi_config.sta.sort_method = WIFI_CONNECT_AP_BY_SECURITY;

    return esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
}

esp_err_t WiFiMode::init()
{
    esp_err_t err;

    ESP_LOGD(TAG, "Starting Wi-Fi");

    err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed");
        return err;
    }

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
    }

    return ESP_OK;
}

esp_err_t WiFiMode::eventHandler(void* ctx, system_event_t* event)
{
    ESP_LOGD(TAG, ">> *****************EVENT HANDLER CALLED*******************************************");
    ESP_LOGD(TAG, ">> ********************************************************************************");
    WiFiMode* pWiFiMode = reinterpret_cast<WiFiMode*>(ctx);
    //WiFiMode* pWiFiMode = (WiFiMode*) ctx;   // retrieve the WiFi object from the passed in context.

    // Invoke the event handler.
    esp_err_t rc;
    if (pWiFiMode->m_p_wifi_event_handler != nullptr) {
        rc = pWiFiMode->m_p_wifi_event_handler->getEventHandler()(pWiFiMode->m_p_wifi_event_handler, event);
    } else {
        rc = ESP_OK;
    }

    return rc;
}
} // namespace wifi