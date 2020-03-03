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

#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <Battery.h>
#include <FuelGauge.h>
#include <Radio.h>
#include <Storage.h>
#include <WiFi.h>

#include "HttpServerHandler.h"
#include "defaults.h"
#include <HttpServer.h>
#include <WebSocket.h>
#include <WsHandlerEvents.h>

HttpServer httpServer;

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
        ESP_LOGI(TAG, "There arent any username in nvs, going to set it\n");
        err = app_nvs.setString(USER_NAME_KEY, DEFAUL_USERNAME);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "There are any errors , was not possible to save in NVS the username /n");
            return ESP_FAIL;
        }
        err = app_nvs.setString(USER_PASSWORD_KEY, DEFAULT_USER_PASSWORD);
        if (err != ESP_OK) {
            ESP_LOGI(TAG,"Error trying to save default password/n");
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
        ESP_LOGI(TAG, "\n");
        ESP_LOGI(TAG, "Username: %s, Password: %s --> inside eeprom", &user_name[0], &user_password[0]);
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


void webSocketHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse)
{
    WsHandlerEvents* myHandler = new WsHandlerEvents();
    if (pHttpRequest->isWebsocket()) {
        pHttpRequest->getWebSocket()->setHandler(myHandler);
        ESP_LOGI("available clients---->>", "%d", pHttpRequest->getWebSocket()->availableClients());
    }
}

extern "C" void app_main()
{
    esp_err_t err;

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
        ESP_LOGE(TAG, "Couldn't initalize Wi-Fi interface (%s)", esp_err_to_name(err));
        return;
    }

    if (!is_configured) {
        wifi.setMode(WIFI_MODE);

        network::APConfig ap_config = {
            .ssid = WAP_SSID,
            .password = WAP_PASS,
            .authmode = WAP_AUTHMODE,
            .max_conn = WAP_MAXCONN,
            .channel = WAP_CHANNEL,
        };
        wifi.setApConfig(ap_config);

        network::STAConfig sta_config = {
            .ssid = WST_SSID,
            .password = WST_PASS,
        };
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

    httpServer.addPathHandler(HttpRequest::HTTP_METHOD_GET, "/stream", webSocketHandler);
    httpServer.addPathHandler(HttpRequest::HTTP_METHOD_GET, "/get-device-info", HttpServerHandler::readDeviceInfoHandler);
    httpServer.addPathHandler(HttpRequest::HTTP_METHOD_POST, "/set-up-sta-ap", HttpServerHandler::setUpStaApHandler);
    httpServer.addPathHandler(HttpRequest::HTTP_METHOD_POST, "/set-up-credentials", HttpServerHandler::setUpCredentialHandler);
    httpServer.start(2565);

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
