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
#include "NVS/NVStorage.h"

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
    NVStorage nvs;

    // Initialize the NVS flash storage
    nvs.begin();

    // open nvs
    bool isOpen = nvs.open("namespace", false);

    if (isOpen)
    {
        ESP_LOGD("nvsOpen", "nvs is open");

        // Save chars into the NVS
        nvs.setString(NVS_STR_KEY, "ESTA ES UNA PRUEBA CON UN STRING LARGO... SALUD!");
        //size_t str_saved = nvs.setString(NVS_STR_KEY, "ESTA ES UNA PRUEBA CON UN STRING LARGO... SALUD!");
        //ESP_LOGD("nvs_set", "saved %d bytes", str_saved);

        // Read chars from the NVS
        char *readString = nvs.getString(NVS_STR_KEY, "ERROR");

        if (readString != "ERROR" && readString)
        {
            ESP_LOGD("nvs_get", "have a key w/value %s", readString);
            free(readString);
        }
        else
        {
            ESP_LOGE("nvs_get", "error reading value w/key");
        }
        /*
        if (str_saved > 0)
        {
            ESP_LOGD("nvs_set", "saved %d bytes", str_saved);
            // Read chars from the NVS
            char *readString = nvs.getString(NVS_STR_KEY, "ERROR");

            if(readString != "ERROR" && readString) {
                ESP_LOGD("nvs_get", "have a key w/value %s", readString);
                free(readString);
            } else {
                ESP_LOGE("nvs_get", "error reading value w/key");
            }
        }
        else
        {
            ESP_LOGE("nvs_set", "Error saving into the nvs");
        }
        */

        // Save int value into the NVS
        nvs.setInt(NVS_INT_KEY, 23987);
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