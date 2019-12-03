/**
 * @file test_nvs.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-20
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>

#include "NVS.h"

/**
 * @brief 
 * Test NVS
 * it try to show how to used class to store and retrieve a blob object
 * to run test inside platform Core-cli the next command
 * platformio test -e featheresp32 -f test_nvs
 *  
 */
void test_open_namespace(void)
{
    esp_err_t err;

    storage::NVS wifi_nvs;
    err = wifi_nvs.open("TEST", NVS_READONLY);
    if (err != ESP_OK) TEST_FAIL();
}

void test_create_and_save_bool()
{
    esp_err_t err;

    storage::NVS wifi_nvs;

    err = wifi_nvs.open("TEST", NVS_READWRITE);
    if (err != ESP_OK) TEST_FAIL();

    err = wifi_nvs.set_bool("test_bool", true);
    if (err != ESP_OK) TEST_FAIL();

    err = wifi_nvs.commit();
    if (err != ESP_OK) TEST_FAIL();
}

extern "C" void app_main()
{
    vTaskDelay(2000);

    esp_err_t err = storage::init();
    if (err != ESP_OK) return;

    UNITY_BEGIN();
    RUN_TEST(test_open_namespace);
    RUN_TEST(test_create_and_save_bool);
    UNITY_END();
}