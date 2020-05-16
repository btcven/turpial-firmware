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
 * @file main.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#include <cstdio>
#include <cstring>
#include <memory>
#include <sstream>

#include "defaults.h"

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Battery.h>
#include <FuelGauge.h>
#include <Storage.h>
#include <Websocket.h>
#include <Slip.h>
#include <Network/WiFi.h>

#include "UserButton.h"
#include "UserButtonHandler.h"

#include "Credentials.h"
#include "RESTServer.h"
#include "Vaina.h"

static const char* TAG = "app_main";

esp_err_t getIsConfigured(bool& is_configured)
{
    esp_err_t err;

    storage::NVS app_nvs;
    err = app_nvs.open(NVS_APP_NAMESPACE, NVS_READWRITE);
    if (err != ESP_OK) {
        const char* err_str = esp_err_to_name(err);
        ESP_LOGE(TAG,
            "Couldn't open namespace \"%s\" (%s)",
            NVS_APP_NAMESPACE,
            err_str);
        return err;
    }

    err = app_nvs.get_bool(NVS_IS_CONFIGURED_KEY, is_configured);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // Set is_configured to true on flash so on next init the config is
        // read directly by the ESP-IDF Wi-Fi library component.
        err = app_nvs.set_bool(NVS_IS_CONFIGURED_KEY, true);
        if (err != ESP_OK) return err;
        err = app_nvs.commit();
        if (err != ESP_OK) return err;
        // Set the return variable to "false" to forcibly set the default
        // configuration
        is_configured = false;
    } else {
        return err;
    }

    return ESP_OK;
}

extern "C" void app_main()
{
    esp_err_t err;

    hmi::UserButton* usr_btn = new hmi::UserButton();
    UserButtonHandler::initRGBPort();
    usr_btn->init(DEFAULT_USER_BUTTON, true, UserButtonHandler::click, UserButtonHandler::doubleClick, UserButtonHandler::longClick);

    err = storage::init();

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't initialize NVS, error (%s)", esp_err_to_name(err));
        return;
    }

    bool is_configured = false;
    err = getIsConfigured(is_configured);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
            "Couldn't get \"is_configured\" value (%s)",
            esp_err_to_name(err));
    }

    credentials::setInitialCredentials();

    network::WiFi& wifi = network::WiFi::getInstance();

    err = wifi.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't initialize Wi-Fi interface (%s)", esp_err_to_name(err));
        return;
    }

    is_configured = false;
    if (!is_configured) {
        wifi.setMode(WIFI_MODE);

        // Default configuration for AP
        wifi_config_t ap_config;
        ap_config.ap.ssid_len = 0;
        std::memcpy(ap_config.ap.ssid, WAP_SSID, sizeof(WAP_SSID));
        std::memcpy(ap_config.ap.password, WAP_PASS, sizeof(WAP_PASS));
        ap_config.ap.authmode = WAP_AUTHMODE;
        ap_config.ap.max_connection = WAP_MAXCONN;
        ap_config.ap.channel = WAP_CHANNEL;
        wifi.setApConfig(ap_config);

        // Default configuration for STA
        wifi_config_t sta_config;
        std::memcpy(sta_config.sta.ssid, WST_SSID, sizeof(WST_SSID));
        std::memcpy(sta_config.sta.password, WST_PASS, sizeof(WST_PASS));
        wifi.setStaConfig(sta_config);
    }

    err = wifi.start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't start Wi-Fi, err = %s", esp_err_to_name(err));
        return;
    }

    rest_server::start_server();

#if CONFIG_RADIO_SLIP
    network::Slip radio_slip;
    err = radio_slip.start(CONFIG_RADIO_SLIP_UART, CONFIG_RADIO_SLIP_TX,
                           CONFIG_RADIO_SLIP_RX, CONFIG_RADIO_SLIP_BAUDRATE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't initialize radio SLIP netif = %s",
                 esp_err_to_name(err));
    }
#endif


#if CONFIG_ESC_ENABLED
    esc::FuelGauge& fuel_gauge = esc::FuelGauge::getInstance();

    esc::Battery& battery = esc::Battery::getInstance();

    gpio_num_t sysoff = static_cast<gpio_num_t>(CONFIG_ESC_SYSOFF_PIN);
    gpio_num_t gpout = static_cast<gpio_num_t>(CONFIG_ESC_GPOUT_PIN);

    err = battery.init(sysoff, gpout, CONFIG_ESC_SOC_DELTA,
        CONFIG_ESC_MAX_BATTERY_CAPACITY);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Can't setup ESC, err = %s",
            esp_err_to_name(err));
        return;
    }

    // Test interrupt
    fuel_gauge.pulseGPOUT();
#endif
}
