/**
 * @file test_ble_uuid.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-12-02
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "Uuid.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>

void testFromHex()
{
    const char* UUID_STR = "4e9a921beba54c09b4fd34886ccf46ef";
    const std::uint8_t EXPECTED[ESP_UUID_LEN_128] = {0x4e, 0x9a, 0x92, 0x1b, 0xeb, 0xa5, 0x4c, 0x09, 0xb4, 0xfd, 0x34, 0x88, 0x6c, 0xcf, 0x46, 0xef};

    ble::Uuid uuid;
    esp_err_t err = ble::Uuid::fromHex(UUID_STR, uuid);
    if (err != ESP_OK) TEST_FAIL();

    TEST_ASSERT_EQUAL_UINT16(ESP_UUID_LEN_128, uuid.getRaw().len);
    TEST_ASSERT_EQUAL_MEMORY(
        EXPECTED,
        uuid.getRaw().uuid.uuid128,
        ESP_UUID_LEN_128);
}

void testEquals()
{
    const char* UUID_1_STR = "4e9a921beba54c09b4fd34886ccf46ef";
    const char* UUID_2_STR = "4e9a921beba54c09b4fd34886ccf46ef";
    const char* UUID_3_STR = "5e9a921beba54c09b4fd34886cbf46bf";

    esp_err_t err = ESP_OK;

    ble::Uuid uuid_1;
    err = ble::Uuid::fromHex(UUID_1_STR, uuid_1);
    if (err != ESP_OK) TEST_FAIL();

    ble::Uuid uuid_2;
    err = ble::Uuid::fromHex(UUID_2_STR, uuid_2);
    if (err != ESP_OK) TEST_FAIL();

    ble::Uuid uuid_3;
    err = ble::Uuid::fromHex(UUID_3_STR, uuid_3);
    if (err != ESP_OK) TEST_FAIL();

    TEST_ASSERT_TRUE(uuid_1.equals(uuid_2));
    TEST_ASSERT_TRUE(uuid_2.equals(uuid_1));

    // Must not be equals
    TEST_ASSERT_FALSE(uuid_1.equals(uuid_3));
    TEST_ASSERT_FALSE(uuid_2.equals(uuid_3));
}

extern "C" void app_main()
{
    vTaskDelay(2000);
    UNITY_BEGIN();
    RUN_TEST(testFromHex);
    RUN_TEST(testEquals);
    UNITY_END();
}