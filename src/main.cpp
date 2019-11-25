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


static char* get_id_string(esp_event_base_t base, int32_t id)
{
    char* event = "";
    if (base == WIFI_EVENTS) {
        switch (id) {
        case STA_AUTHMODE_CHANGE_EVENT:
            event = "SYSTEM_EVENT_STA_AUTHMODE_CHANGE";
            break;
        case STA_CONNECTED_EVENT:
            event = "SYSTEM_EVENT_STA_CONNECTED";
            break;
        case STA_DISCONNECTED_EVENT:
            event = "SYSTEM_EVENT_STA_DISCONNECTED";
            break;
        case STA_GOT_IP_EVENT:
            event = "SYSTEM_EVENT_STA_GOT_IP";
            break;
        case STA_START_EVENT:
            event = "SYSTEM_EVENT_STA_START";
            break;
        case STA_STOP_EVENT:
            event = "SYSTEM_EVENT_STA_STOP";
            break;
        case WIFI_READY_EVENT:
            event = "SYSTEM_EVENT_WIFI_READY";
            break;
        }
    } else {
        event = "TASK_ITERATION_EVENT";
    }
    return event;
}


/* Handler for all events */
static void all_event_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "%s:%s: all_event_handler", base, get_id_string(base, id));
}

static void sta_got_ip_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    ESP_LOGI(TAG, "*****************************!!!!!!!!!!!!!!!!!!!!!!!!!!!***********************************!!!!!!!!!!!!!!!!!!!!!!!!!");
}

extern "C" void app_main()
{
    esp_err_t err;
    wifi::WiFiMode wifi_mode;

    wifi::WiFiHandleEvents wifi_event;


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


    // Create the default event loop
    //ESP_ERROR_CHECK(esp_event_loop_create_default());
    //ESP_ERROR_CHECK(esp_event_handler_register(ESP_EVENT_ANY_BASE, ESP_EVENT_ANY_ID, all_event_handler, NULL));
   // ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENTS, STA_GOT_IP_EVENT, sta_got_ip_handler, NULL));
    wifi_event.on(STA_GOT_IP_EVENT, sta_got_ip_handler);
    err = wifi_mode.start();
    // TODO: app loop
}