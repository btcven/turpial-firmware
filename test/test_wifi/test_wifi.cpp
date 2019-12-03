/**
 * @file test_wifi_mode.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-17
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */


#include "NVS.h"
#include "WiFi.h"
#include "defaults.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>

void testWiFi()
{
    const TickType_t DELAY = 10000 / portTICK_PERIOD_MS;

    esp_err_t err;

    tcpip_adapter_init();

    network::WiFi& wifi = network::WiFi::getInstance();
    err = wifi.init();
    if (err != ESP_OK) TEST_FAIL();

    wifi.setMode(WIFI_MODE);

    network::APConfig ap_config = {
        .ssid = WAP_SSID,
        .password = WAP_PASS,
        .authmode = WAP_AUTHMODE,
        .max_conn = WAP_MAXCONN,
        .channel = WAP_CHANNEL,
    };

    err = wifi.setApConfig(ap_config);
    if (err != ESP_OK) TEST_FAIL();

    err = wifi.start();
    if (err != ESP_OK) TEST_FAIL();

    vTaskDelay(DELAY);
}

extern "C" void app_main()
{
    storage::init();

    vTaskDelay(2000);
    UNITY_BEGIN();
    RUN_TEST(testWiFi);
    UNITY_END();
}