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


#include "WiFiMode.h"
#include "defaults.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>

void test_wifi_mode()
{
    const TickType_t xDelay = 10000 / portTICK_PERIOD_MS;

    esp_err_t err;

    tcpip_adapter_init();

    wifi::WiFiMode wifi_mode;
    err = wifi_mode.init(false);
    if (err != ESP_OK) TEST_FAIL();

    wifi_mode.set_mode(WIFI_MODE);

    wifi::APConfig ap_config = {
        .ssid = WAP_SSID,
        .password = WAP_PASS,
        .authmode = WAP_AUTHMODE,
        .max_conn = WAP_MAXCONN,
        .channel = WAP_CHANNEL,
    };

    err = wifi_mode.set_ap_config(ap_config);
    if (err != ESP_OK) TEST_FAIL();

    err = wifi_mode.start();
    if (err != ESP_OK) TEST_FAIL();

    vTaskDelay(xDelay);
}

extern "C" void app_main()
{
    vTaskDelay(2000);
    UNITY_BEGIN();
    RUN_TEST(test_wifi_mode);
    UNITY_END();
}