/**
 * @file test_util_hex.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-12-02
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "Hex.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <unity.h>

void testDigit(const char actual, int expected)
{
    int r = 0;

    esp_err_t err = util::hexToInt(actual, r);
    if (err != ESP_OK) TEST_FAIL();

    TEST_ASSERT_EQUAL_INT(expected, r);
}

void testHexToInt()
{
    testDigit('0', 0);
    testDigit('1', 1);
    testDigit('2', 2);
    testDigit('3', 3);
    testDigit('4', 4);
    testDigit('5', 5);
    testDigit('6', 6);
    testDigit('7', 7);
    testDigit('8', 8);
    testDigit('9', 9);
    testDigit('A', 10);
    testDigit('B', 11);
    testDigit('C', 12);
    testDigit('D', 13);
    testDigit('E', 14);
    testDigit('F', 15);
}

void testHexToBytes()
{
    const char* TEST_STRING = "aabbccddeeff";
    const std::uint8_t EXPECTED[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};

    std::uint8_t result[6] = {0};

    esp_err_t err = util::hexToBytes(TEST_STRING, result);
    if (err != ESP_OK) TEST_FAIL();

    TEST_ASSERT_EQUAL_MEMORY(EXPECTED, result, 6);
}

extern "C" void app_main()
{
    vTaskDelay(2000);
    UNITY_BEGIN();
    RUN_TEST(testHexToInt);
    RUN_TEST(testHexToBytes);
    UNITY_END();
}