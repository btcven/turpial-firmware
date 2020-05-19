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

#include "Storage.h"
#include <cstring>
#include <iostream>
#include <unity.h>

TEST_CASE("Open namespace", "[nvs]")
{
    esp_err_t err;

    storage::NVS nvs;
    err = nvs.open("TEST", NVS_READONLY);
    if (err != ESP_OK) TEST_FAIL();
}

TEST_CASE("Save a bool in the NVS", "[nvs]")
{
    esp_err_t err;

    storage::NVS nvs;

    err = nvs.open("TEST", NVS_READWRITE);
    if (err != ESP_OK) TEST_FAIL();

    err = nvs.set_bool("test_bool", true);
    if (err != ESP_OK) TEST_FAIL();

    err = nvs.commit();
    if (err != ESP_OK) TEST_FAIL();
}

TEST_CASE("get bool in the storage", "[nvs]")
{
    esp_err_t err;
    bool value = true;
    storage::NVS nvs;

    err = nvs.open("TEST", NVS_READWRITE);
    if (err != ESP_OK) TEST_FAIL();

    err = nvs.get_bool("test_bool", value);
    if (err != ESP_OK) TEST_FAIL();

    TEST_ASSERT_TRUE(value);
}


TEST_CASE("Set a String in the NVS", "[nvs]")
{
    esp_err_t err;
    std::string value = "test";
    storage::NVS nvs;

    err = nvs.open("TEST", NVS_READWRITE);
    if (err != ESP_OK) TEST_FAIL();

    err = nvs.setString("test_string", value);
    if (err != ESP_OK) TEST_FAIL();

    err = nvs.commit();
    if (err != ESP_OK) TEST_FAIL();
}


TEST_CASE("get string in the storage", "[nvs]")
{
    esp_err_t err;
    char* compare = "test";
    storage::NVS nvs;
    err = nvs.open("TEST", NVS_READWRITE);
    if (err != ESP_OK) TEST_FAIL();

    char value[4];
    size_t len = strlen(compare) + 1;
    err = nvs.getString("test_string", value, &len);
    if (err != ESP_OK) TEST_FAIL();

    TEST_ASSERT_EQUAL_STRING(compare, value);
}