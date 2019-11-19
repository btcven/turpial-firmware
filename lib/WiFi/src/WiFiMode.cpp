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

#include <cstring>

#include "sdkconfig.h"

#include "esp_event_loop.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "esp_wifi.h"

namespace wifi {

template <unsigned int N>
void copy_bytes(std::uint8_t* dest, const SerializableBytes<N>& src)
{
    // Copy bytes
    std::memcpy(dest, src.data(), N);
}

WiFiMode::WiFiMode()
{
}

esp_err_t WiFiMode::begin(const DTOConfig& dto_config)
{
    ESP_LOGI(__func__, "WiFiMode begin");

    esp_err_t err;

    wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();

    ESP_LOGD(__func__, "init WiFi");
    err = esp_wifi_init(&wifi_init_config);
    if (err != ESP_OK) return err;

    ESP_LOGD(__func__, "set WiFi mode %d", dto_config.wifi_mode);
    err = esp_wifi_set_mode(dto_config.wifi_mode);
    if (err != ESP_OK) return err;

    ESP_LOGD(__func__, "init WiFi event loop");
    err = esp_event_loop_init(
        &WiFiMode::eventHandler,
        reinterpret_cast<void*>(this));
    if (err != ESP_OK) return err;


    bool should_init_sta = (dto_config.wifi_mode == WIFI_MODE_APSTA) ||
                           (dto_config.wifi_mode == WIFI_MODE_STA);
    bool should_init_ap = (dto_config.wifi_mode == WIFI_MODE_APSTA) ||
                          (dto_config.wifi_mode == WIFI_MODE_AP);

    if (should_init_sta) {
        err = setSTAConfig(dto_config);
        if (err != ESP_OK) return err;
    }

    if (should_init_ap) {
        err = setAPConfig(dto_config);
        if (err != ESP_OK) return err;
    }

    ESP_LOGD(__func__, "Start WiFi (and AP if enabled)");

    err = esp_wifi_start();
    if (err != ESP_OK) return err;

    if (should_init_sta) {
        ESP_LOGD(__func__, "connecting WiFi STA");

        err = esp_wifi_connect();
        if (err != ESP_OK) return err;
    }

    return ESP_OK;
}

esp_err_t WiFiMode::setSTAConfig(const DTOConfig& dto_config)
{
    // Zero-out the wifi_config_t structure before using it
    wifi_config_t sta_config;
    std::memset(reinterpret_cast<void*>(&sta_config), 0, sizeof(wifi_config_t));

    copy_bytes<32>(sta_config.sta.ssid, dto_config.wst_ssid);
    copy_bytes<64>(sta_config.sta.password, dto_config.wst_password);

    sta_config.sta.scan_method = WIFI_ALL_CHANNEL_SCAN;
    sta_config.sta.bssid_set = false;
    sta_config.sta.channel = 0;
    sta_config.sta.sort_method = WIFI_CONNECT_AP_BY_SIGNAL;

    return esp_wifi_set_config(WIFI_IF_STA, &sta_config);
}

esp_err_t WiFiMode::setAPConfig(const DTOConfig& dto_config)
{
    if (dto_config.ap_max_conn > 4) return ESP_FAIL;

    // Zero-out the wifi_config_t structure before using it
    wifi_config_t ap_config;
    std::memset(reinterpret_cast<void*>(&ap_config), 0, sizeof(wifi_config_t));

    copy_bytes<32>(ap_config.ap.ssid, dto_config.ap_ssid);
    copy_bytes<64>(ap_config.ap.password, dto_config.ap_password);

    ap_config.ap.ssid_len = 0;
    ap_config.ap.channel = dto_config.ap_channel;
    ap_config.ap.authmode = WIFI_AUTH_WPA_WPA2_PSK;
    ap_config.ap.ssid_hidden = 0;
    ap_config.ap.max_connection = dto_config.ap_max_conn;

    return esp_wifi_set_config(WIFI_IF_AP, &ap_config);
}

esp_err_t WiFiMode::eventHandler(void* ctx, system_event_t* event)
{
    ESP_LOGD(__func__, "eventHandler called");
    //WiFiMode* wifi_mode = reinterpret_cast<WiFiMode*>(ctx);
    return ESP_OK;
}

} // namespace wifi