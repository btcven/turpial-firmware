/**
 * @file main.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <Arduino.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "hal/hardware.h"
#include "NVS/persistence.h"

/*
esp_err_t batteryTest() {
    // Put the code here for battery test
    esp_err_t err;
}

esp_err_t bleTest() {
    // Put the code here for BLE test
    esp_err_t err;
}

esp_err_t wifiTest() {
    // Put the code here for Wi-Fi test
    esp_err_t err;
}
*/

void nvsTest() {
    Persistence nvs;

    // open nvs
    bool isOpen = nvs.begin("namespace", false);

    if (isOpen)
    {
        ESP_LOGD("nvsOpen", "nvs is open");

        // Save chars into the NVS
        size_t str_saved = nvs.setString(NVS_STR_KEY, "ANTONIO");
        if (str_saved > 0)
        {
            ESP_LOGD("nvs_set", "saved %d bytes", str_saved);
            // Read chars from the NVS
            const char *readString = nvs.getString(NVS_STR_KEY, "ERROR");

            if(readString != "ERROR") {
                ESP_LOGD("nvs_get", "have a key w/value %s", readString);
            } else {
                ESP_LOGE("nvs_get", "error reading value w/key");
            }
        }
        else
        {
            ESP_LOGE("nvs_set", "Error saving into the nvs");
        }

        // Save int value into the NVS
        nvs.setInt(NVS_INT_KEY, 16782);
        // Read int from the NVS
        int32_t readInt = nvs.getInt(NVS_INT_KEY, 0);
        ESP_LOGD(__func__, "Int value from NVS: %i", readInt);

        // Save bool value into the NVS
        nvs.setBool(NVS_BOOL_KEY, 1);
        // Read int from the NVS
        bool readBool = nvs.getBool(NVS_BOOL_KEY, 0);
        ESP_LOGD(__func__, "Bool value from NVS: %i", readBool);
    }
    else
    {
        ESP_LOGE("nvsOpen", "Error opening the NVS");
    }
}

void setup()
{
    nvsTest();
}

void loop()
{
    //
}