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

#include "Storage.h"
#include "WiFi.h"
#include "defaults.h"

#include <unity.h>

TEST_CASE("Wi-Fi initialize", "[network]")
{
    const TickType_t DELAY = 10000 / portTICK_PERIOD_MS;

    esp_err_t err;

    esp_netif_init();

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

TEST_CASE("Sanitize SSID", "[network]")
{
    std::uint8_t expected_buf[32] = {0};
    expected_buf[0] = 'a';
    expected_buf[1] = 'b';
    expected_buf[2] = '?';
    expected_buf[3] = 'c';

    std::uint8_t buf[32] = {0};
    buf[0] = 'a';
    buf[1] = 'b';
    buf[2] = 0x0E;
    buf[3] = 'c';

    esp_err_t err = network::sanitizeSsid(buf, 4);
    if (err != ESP_OK) TEST_FAIL();

    TEST_ASSERT_EQUAL_MEMORY(expected_buf, buf, 32);
}
