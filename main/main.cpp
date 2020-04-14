/**
 * @file main.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief
 * @version 0.1
 * @date 2019-09-11
 *
 * @copyright Copyright (c) 2020 Locha Mesh Developers
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
#include <Radio.h>
#include <Storage.h>
#include <WiFi.h>

#include "UserButton.h"
#include "UserButtonHandler.h"

#include "RESTServer.h"

static const char* TAG = "app_main";

esp_err_t getSetCredentials(void)
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

    char user_name[MAX_USER_NAME_LENGTH];
    char user_password[MAX_USER_PASSWORD_LENGTH];
    size_t ul = MAX_USER_NAME_LENGTH;
    size_t pl = MAX_USER_PASSWORD_LENGTH;
    err = app_nvs.getString(USER_NAME_KEY, user_name, &ul);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "There is no username in nvs, going to set it");
        err = app_nvs.setString(USER_NAME_KEY, DEFAUL_USERNAME);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "There are errors , was not possible to save in NVS the username");
            return ESP_FAIL;
        }
        err = app_nvs.setString(USER_PASSWORD_KEY, DEFAULT_USER_PASSWORD);
        if (err != ESP_OK) {
            ESP_LOGI(TAG,"Error trying to save default password");
            return ESP_FAIL;
        }
    } else {
        err = app_nvs.getString(USER_PASSWORD_KEY, user_password, &pl);
        if (err != ESP_OK) {
            err = app_nvs.setString(USER_PASSWORD_KEY, DEFAULT_USER_PASSWORD);
            if (err != ESP_OK) {
                return ESP_FAIL;
            }
        }
        ESP_LOGI(TAG, "\nUsername: %s, Password: %s --> inside eeprom", &user_name[0], &user_password[0]);
    }

    return ESP_OK;
}

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

    getSetCredentials();

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

#if RAD_ENABLED == true
    radio::Radio* radio_task = new radio::Radio();
    radio_task->start();
#endif

    rest_server::start_server();

#if ESC_ENABLED == true
    esc::FuelGauge& fuel_gauge = esc::FuelGauge::getInstance();

    esc::Battery& battery = esc::Battery::getInstance();

    err = battery.init(ESC_SYSOFF_PIN, ESC_GPOUT_PIN, ESC_SOC_DELTA, ESC_MAX_BATTERY_CAPACITY);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Can't setup ESC, err = %s",
            esp_err_to_name(err));
        return;
    }

    // Test interrupt
    fuel_gauge.pulseGPOUT();
#endif
}
