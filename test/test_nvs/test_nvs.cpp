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

#include <cstring>

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

    storage::NVS nvs;
    err = nvs.open("TEST", NVS_READONLY);
    if (err != ESP_OK) TEST_FAIL();
}

void test_set_get_u8()
{
    esp_err_t err;
    storage::NVS nvs;

    err = nvs.open("TEST", NVS_READWRITE);
    if (err != ESP_OK) TEST_FAIL();

    err = nvs.setU8("test_u8", 254);
    if (err != ESP_OK) TEST_FAIL();

    err = nvs.commit();
    if (err != ESP_OK) TEST_FAIL();

    std::uint8_t v = 0;
    err = nvs.getU8("test_u8", v);
    if (err != ESP_OK) TEST_FAIL();

    TEST_ASSERT_EQUAL_UINT8(254, v);
}

void test_set_get_blob()
{
    esp_err_t err;
    storage::NVS nvs;

    const std::size_t blob_size = 9;
    const char* blob = "test data";

    err = nvs.open("TEST", NVS_READWRITE);
    if (err != ESP_OK) TEST_FAIL();

    err = nvs.setBlob("test_blob",
        reinterpret_cast<const std::uint8_t*>(blob),
        blob_size);
    if (err != ESP_OK) TEST_FAIL();

    err = nvs.commit();
    if (err != ESP_OK) TEST_FAIL();

    std::vector<std::uint8_t> value;
    err = nvs.getBlob("test_blob", value);
    if (err != ESP_OK) TEST_FAIL();

    int equals = std::memcmp(&value[0], blob, blob_size);
    if (equals != 0) TEST_FAIL();
}

extern "C" void app_main()
{
    vTaskDelay(2000);

    esp_err_t err = storage::NVS::start();
    if (err != ESP_OK) return;

    UNITY_BEGIN();
    RUN_TEST(test_open_namespace);
    RUN_TEST(test_set_get_u8);
    RUN_TEST(test_set_get_blob);
    UNITY_END();
}