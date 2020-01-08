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

#include <unity.h>

#include "Storage.h"

TEST_CASE("Open namespace", ["nvs"])
{
    esp_err_t err;

    storage::NVS nvs;
    err = nvs.open("TEST", NVS_READONLY);
    if (err != ESP_OK) TEST_FAIL();
}

TEST_CASE("Save a bool in the NVS", ["nvs"])
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
