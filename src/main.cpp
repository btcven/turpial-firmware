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
#include <sstream>

#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


#include "WiFiMode.h"
#include "testRTOSCPP/Hello.h"

#include "Namespace.h"

#include "defaults.h"

esp_err_t readWiFiParams(wifi::DTOConfig& wifi_params)
{
    ESP_LOGD(__func__, "Reading WiFi configuration from NVS");

    nvs::Namespace wifi_nvs;
    auto err = wifi_nvs.open("wifi", NVS_READWRITE);
    if (err != ESP_OK) return err;

    std::stringstream blob;
    err = wifi_nvs.get_blob("wifi_dto_config", blob);
    if (err != ESP_OK) return err;

    wifi_params.deserialize(blob);
    return ESP_OK;
}

void setDefaultWiFiParams(wifi::DTOConfig& wifi_params)
{
    wifi_params.ap_channel = WAP_CHANNEL;
    wifi_params.ap_max_conn = WAP_MAXCONN;
    wifi_params.wifi_mode = WIFI_MODE;
    wifi_params.is_open = false;
    wifi_params.ap_ssid = tinystring::String(WAP_SSID);
    wifi_params.ap_password = tinystring::String(WAP_PASS);
    wifi_params.wst_ssid = tinystring::String(WST_SSID);
    wifi_params.wst_password = tinystring::String(WST_PASS);
}

extern "C" void app_main()
{
    esp_err_t err;

    // Initialize NVS.
    err = nvs::begin();
    wifi::DTOConfig wifi_params;

    if (err != ESP_OK) {
        auto estr = esp_err_to_name(err);
        ESP_LOGE(__func__, "Couldn't initialize NVS, error %s", estr);
        ESP_LOGI(__func__, "Using default WiFi parameters");

        setDefaultWiFiParams(wifi_params);
    } else {
        err = readWiFiParams(wifi_params);
        if (err != ESP_OK) {
            auto estr = esp_err_to_name(err);
            ESP_LOGE(__func__, "Couldn't read WiFi parameters (%s)", estr);
            ESP_LOGI(__func__, "Using default WiFi parameters");

            setDefaultWiFiParams(wifi_params);
        }
    }

    /// Create default event loop (needed by WiFiMode)
    err = esp_event_loop_create_default();
    if (err != ESP_OK) {
        auto estr = esp_err_to_name(err);
        ESP_LOGE(__func__, "couldn't create event loop (%s)", estr);
        return;
    }

    wifi::WiFiMode wifi_mode;

    err = wifi_mode.begin(wifi_params);
    if (err != ESP_OK) {
        auto estr = esp_err_to_name(err);
        ESP_LOGE(__func__, "Couldn't start WiFi interface (%s)", estr);
        return;
    }

    // TODO: app loop
}
