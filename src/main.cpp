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

#include "esp_log.h"
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "NVS.h"
#include "WiFiMode.h"


#include "defaults.h"
#include "wifi_event_source.h"

#include "WiFiHandleEvents.h"

static const char* TAG = "app_main";
ESP_EVENT_DEFINE_BASE(WIFI_EVENTS);

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
        // readed directly by the ESP-IDF Wi-Fi library component.
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

static void sta_got_ip_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "*****************************!!!!!!!!!!!!!!!!!!!!!!!!!!!***********************************!!!!!!!!!!!!!!!!!!!!!!!!!");
    ESP_LOGI(TAG, "*************************sta_got_ip_handler*****************************");
}

extern "C" void app_main()
{
    esp_err_t err;
    wifi::WiFiMode wifi_mode;

    bool is_nvs_initialized = true;
    err = storage::init();
    if (err != ESP_OK) {
        const char* err_name = esp_err_to_name(err);
        ESP_LOGE(TAG, "Couldn't initialize NVS, error (%s)", err_name);
        is_nvs_initialized = false;
    }

    ESP_LOGD(TAG, "Init TCP/IP adapter");
    tcpip_adapter_init();

    bool is_configured = false;
    if (is_nvs_initialized) {
        err = getIsConfigured(is_configured);
        if (err != ESP_OK) {
            const char* err_str = esp_err_to_name(err);
            ESP_LOGE(TAG,
                "Couldn't get \"is_configured\" value (%s)",
                err_str);
        }
    }

    err = wifi_mode.init(is_nvs_initialized);
    if (err != ESP_OK) {
        const char* err_name = esp_err_to_name(err);
        ESP_LOGE(TAG, "Couldn't initalize Wi-Fi interface (%s)", err_name);
        // TODO: fallback to bluetooth mode to configure Wi-Fi?
        return;
    }

    if (!is_configured) {
        wifi_mode.set_mode(WIFI_MODE);

        wifi::APConfig ap_config = {
            .ssid = WAP_SSID,
            .password = WAP_PASS,
            .authmode = WAP_AUTHMODE,
            .max_conn = WAP_MAXCONN,
            .channel = WAP_CHANNEL,
        };
        wifi_mode.set_ap_config(ap_config);

        wifi::STAConfig sta_config = {
            .ssid = WST_SSID,
            .password = WST_PASS,
        };
        wifi_mode.set_sta_config(sta_config);
    }

    //register to listen STA_GOT_IP_EVENT
    //there is a file with all wifi events to listen
    wifi_mode.on(STA_GOT_IP_EVENT, sta_got_ip_handler);
    err = wifi_mode.start();
    // TODO: app loop
}