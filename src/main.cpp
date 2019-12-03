/**
 * @file main.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <cstdio>
#include <memory>
#include <sstream>

#include "esp_log.h"
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "BLEPreferences.h"
#include "NVS.h"
#include "Preferences.h"
#include "WiFi.h"


#include "defaults.h"

static const char* TAG = "app_main";

extern "C" void app_main()
{
    esp_err_t err;

    err = storage::NVS::start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't initialize NVS, error (%s)", esp_err_to_name(err));
        return;
    }

    util::DefaultPreferences default_prefs;
    default_prefs.setApSSID(WAP_SSID);
    default_prefs.setApPassword(WAP_PASS);
    default_prefs.setApMaxConn(WAP_MAXCONN);
    default_prefs.setApChannel(WAP_CHANNEL);
    default_prefs.setStaSsid(WST_SSID);
    default_prefs.setStaPassword(WST_PASS);
    default_prefs.setWiFiMode(WIFI_MODE);

    util::Preferences& prefs = util::Preferences::getInstance();
    err = prefs.start(default_prefs);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Can't start preferences (%s)", esp_err_to_name(err));
        return;
    }

    ESP_LOGD(TAG, "Init TCP/IP adapter");
    tcpip_adapter_init();

    network::WiFi& wifi = network::WiFi::getInstance();
    err = wifi.init();
    if (err != ESP_OK) {
        const char* err_name = esp_err_to_name(err);
        ESP_LOGE(TAG, "Couldn't initalize Wi-Fi interface (%s)", err_name);
        return;
    }

    wifi_mode_t mode;
    prefs.getWiFiMode(mode);
    wifi.setMode(mode);

    wifi_config_t ap_config = {};
    prefs.getApSSID(ap_config.ap.ssid);
    prefs.getApPassword(ap_config.ap.password);
    prefs.getApMaxConn(ap_config.ap.max_connection);
    prefs.getApChannel(ap_config.ap.channel);
    ap_config.ap.authmode = WAP_AUTHMODE;
    wifi.setApConfig(ap_config);

    wifi_config_t sta_config = {};
    prefs.getStaSSID(sta_config.sta.ssid);
    prefs.getStaPassword(sta_config.sta.password);
    wifi.setStaConfig(sta_config);

    err = wifi.start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't start Wi-Fi, err = %s", esp_err_to_name(err));
        return;
    }

    ble::ServerParams server_params;
    server_params.device_name = "Turpial-1234";
    server_params.static_passkey = 123456;
    server_params.app_id = 0;
    ble_preferences::start(server_params);
}