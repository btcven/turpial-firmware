/**
 * @file test_wifi_mode.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-17
 * @copyright Copyright (c) 2019
 * 
 */

#include "Namespace.h"
#include "WiFiMode.h"
#include "defaults.h"
#include "sdkconfig.h"


#include <cstdint>
#include <sstream>
#include <unity.h>

void setDefaultWiFiParams(wifi::DTOConfig& wifi_params)
{
    wifi_params.ap_channel = WAP_CHANNEL;
    wifi_params.ap_max_conn = WAP_MAXCONN;
    wifi_params.wifi_mode = WIFI_MODE;
    wifi_params.is_open = false;
    wifi_params.ap_ssid = WAP_SSID;
    wifi_params.ap_password = WAP_PASS;
    wifi_params.wst_ssid = WST_SSID;
    wifi_params.wst_password = WST_PASS;
}

void testWifiMode(void)
{
    const TickType_t xDelay = 10000 / portTICK_PERIOD_MS;

    // Initialize NVS
    nvs::begin();

    wifi::DTOConfig wifi_parameters;
    setDefaultWiFiParams(wifi_parameters);

    wifi::WiFiMode wifi_mode;
    esp_err_t err = wifi_mode.begin(wifi_parameters);
    if (err != ESP_OK) {
        ESP_LOGE(__func__, "Couldn't start WiFi interface (%s)", esp_err_to_name(err));
        return;
    }

    vTaskDelay(xDelay);
}

extern "C" void app_main()
{
    vTaskDelay(2000);

    UNITY_BEGIN();
    RUN_TEST(testWifiMode);
    UNITY_END();
}