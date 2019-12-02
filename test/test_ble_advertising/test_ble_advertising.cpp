/**
 * @file test_ble_advertising.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-12-02
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include <cstdint>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>

#include "Advertising.h"

void testAdvertisementData()
{
    const std::size_t LENGTH = 1 + 1 + 7;
    const std::uint8_t DATA[LENGTH] = {1 + 7, ESP_BLE_AD_TYPE_NAME_CMPL, 0x54, 0x75, 0x72, 0x70, 0x69, 0x61, 0x6c};

    ble::AdvertisementData adv_data;
    adv_data.setName("Turpial");

    TEST_ASSERT_EQUAL_INT(LENGTH, adv_data.length());
    TEST_ASSERT_EQUAL_MEMORY(DATA, adv_data.getPayload(), LENGTH);
}

void testAdvertisementState()
{
    ble::AdvertisementState adv_state;

    adv_state.set_config_state();
    adv_state.set_scan_rsp_config_state();

    // TODO: these functions erase the state upon called, it's a good idea?
    // maybe unset_* methods should be added apart and these functions only
    // check the state
    TEST_ASSERT_TRUE(adv_state.is_config_state());
    TEST_ASSERT_TRUE(adv_state.is_scan_rsp_config_state());
}

extern "C" void app_main()
{
    vTaskDelay(2000);

    UNITY_BEGIN();
    RUN_TEST(testAdvertisementData);
    RUN_TEST(testAdvertisementState);
    UNITY_END();
}