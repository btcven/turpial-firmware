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
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>


#include "WiFiMode.h"
#include "testRTOSCPP/Hello.h"

#include "BQ27441.h"
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
    wifi_params.wap_enabled = WAP_ENABLED;
    wifi_params.wst_enabled = WST_ENABLED;
    wifi_params.is_open = false;
    wifi_params.ap_ssid = tinystring::String(WAP_SSID);
    wifi_params.ap_password = tinystring::String(WAP_PASS);
}

extern "C" void app_main()
{
    esp_err_t err;

    // Initialize arduino as a component
    initArduino();

    // Set logging level for all tags.
    esp_log_level_set("*", ESP_LOG_VERBOSE);

    // Initialize NVS.
    auto nvs_err = nvs::begin();
    wifi::DTOConfig wifi_params;
    nvs::Namespace wifi_nvs;
    wifi_nvs.open("wifi", NVS_READWRITE);

    wifi::DTOConfig wifi_params2;
    setDefaultWiFiParams(wifi_params);
    std::stringstream blob;
    std::stringstream blob2;


    auto err = wifi_nvs.get_blob("wifi", blob2);
    wifi_params2.deserialize(blob2);


    /*wifi::mode::begin(wifi_params); */
    // TODO: app loop
}
