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


#include <sstream>
#include <cstdio>


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include "WiFiMode.h"
#include "testRTOSCPP/Hello.hpp"

#include "Namespace.h"
#include "BQ27441.h"

#include "defaults.h"


esp_err_t readWiFiParams(wifi::DTOConfig& wifi_params) {
    ESP_LOGD(__func__, "Reading WiFi configuration from NVS");
    nvs::Namespace wifi_nvs;
    auto err = wifi_nvs.open("wifi", NVS_READWRITE);
    if (err != ESP_OK) {
        return err;
    }

    std::stringstream blob;
    err = wifi_nvs.get_blob("wifi_dto_config", blob);
    if (err != ESP_OK) {
        return err;
    }

    wifi_params.deserialize(blob);
    return ESP_OK;
}

void setDefaultWiFiParams(wifi::DTOConfig& wifi_params) {
    wifi_params.apChannel = WAP_CHANNEL;
    wifi_params.apMaxConn = WAP_MAXCONN;
    wifi_params.WAP_enabled = WAP_ENABLED;
    wifi_params.WST_enabled = WST_ENABLED;
    wifi_params.isOpen = false;
    wifi_params.apSSID = tinystring::String(WAP_SSID);
    wifi_params.apPassword = tinystring::String(WAP_PASS);
} 

extern "C" void app_main()
{
    // Initialize arduino as a component
    initArduino();
    // Initialize NVS.
    auto nvs_err = nvs::begin();
    // TODO: app loop

    bq27441::bq27441.begin();
}

 