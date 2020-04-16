/**
 * @file WiFi.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief
 * @version 0.1.1
 * @date 2019-08-15
 *
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "WiFi.h"

#include <cstdint>
#include <cstring>

#include <esp_log.h>
#include <esp_wifi.h>

namespace network {

static const char* TAG = "WiFi";

WiFiDefaultEventHandler::WiFiDefaultEventHandler()
{
}

esp_err_t WiFiDefaultEventHandler::staStart()
{
    ESP_LOGI(TAG, "STA Start");

    WiFi& wifi = WiFi::getInstance();

    esp_err_t err = wifi.connect();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't connect to AP (%s)", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

esp_err_t WiFiDefaultEventHandler::staStop()
{
    ESP_LOGI(TAG, "STA Stop");

    return ESP_OK;
}

WiFi::WiFi()
    : m_event_handler()
{
}
esp_err_t WiFi::init()
{
    esp_err_t err;

    ESP_LOGD(TAG, "Init TCP/IP adapter");
    err = esp_netif_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_netif_init failed, err = %s", esp_err_to_name(err));
        return err;
    }

    err = esp_event_loop_create_default();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't create event loop, err = %s", esp_err_to_name(err));
        return err;
    }
    esp_netif_create_default_wifi_ap();

    ESP_LOGD(TAG, "Initializing Wi-Fi");

    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    err = esp_wifi_init(&init_config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_init failed, err = %s", esp_err_to_name(err));
        return err;
    }

    err = esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
        &WiFi::eventHandler, this);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't register Wi-Fi event handler, err = %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

esp_err_t WiFi::setMode(wifi_mode_t mode)
{
    return esp_wifi_set_mode(mode);
}

esp_err_t WiFi::getMode(wifi_mode_t& mode)
{
    return esp_wifi_get_mode(&mode);
}

esp_err_t WiFi::setApConfig(wifi_config_t& ap_config)
{
    return esp_wifi_set_config(WIFI_IF_AP, &ap_config);
}

esp_err_t WiFi::getApConfig(wifi_config_t& ap_config)
{
    return esp_wifi_get_config(WIFI_IF_AP, &ap_config);
}

esp_err_t WiFi::setStaConfig(wifi_config_t& sta_config)
{
    return esp_wifi_set_config(WIFI_IF_STA, &sta_config);
}

esp_err_t WiFi::getStaConfig(wifi_config_t& sta_config)
{
    return esp_wifi_get_config(WIFI_IF_STA, &sta_config);
}

bool WiFi::isAp()
{
    wifi_mode_t mode;
    esp_err_t err = esp_wifi_get_mode(&mode);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_get_mode failed (%s)", esp_err_to_name(err));
        return false;
    }

    return (mode == WIFI_MODE_APSTA || mode == WIFI_MODE_AP);
}

bool WiFi::isSta()
{
    wifi_mode_t mode;
    esp_err_t err = esp_wifi_get_mode(&mode);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_get_mode failed (%s)", esp_err_to_name(err));
        return false;
    }

    return (mode == WIFI_MODE_APSTA || mode == WIFI_MODE_STA);
}

esp_err_t WiFi::start()
{
    ESP_LOGD(TAG, "Starting Wi-Fi");

    esp_err_t err = esp_wifi_start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_start failed");
        return err;
    }

    return ESP_OK;
}

esp_err_t WiFi::connect()
{
    esp_err_t err;

    wifi_mode_t wifimode;
    err = esp_wifi_get_mode(&wifimode);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_get_mode failed");
        return err;
    }

    if (!isSta()) {
        ESP_LOGE(TAG, "Wi-Fi mode is not STA");
        return ESP_FAIL;
    }

    return esp_wifi_connect();
}

void WiFi::setEventHandler(std::unique_ptr<WiFiEventHandler>&& event_handler)
{
    ESP_LOGD(TAG, "Setting Wi-Fi event handler");

    m_event_handler.setNextHandler(std::move(event_handler));
}

esp_err_t WiFi::stop()
{
    bool is_sta = isSta();

    esp_err_t err = esp_wifi_stop();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_stop failed, err = %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

void WiFi::eventHandler(void* event_handler_arg,
    esp_event_base_t event_base,
    std::int32_t event_id,
    void* event_data)
{
    ESP_LOGD(TAG, "Wi-Fi Event Handler Called");
    WiFi* wifi = reinterpret_cast<WiFi*>(event_handler_arg);

    esp_err_t err = wifi->m_event_handler.eventDispatcher(event_id, event_data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Event handler error, err = %s", esp_err_to_name(err));
    }
}

} // namespace network
