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
#include "hal/hardware.h"
#include "NVS/persistence.h"

void setup()
{
    //
    Persistence nvs;

    bool isOpen = nvs.begin("namespace", false);

    if (isOpen)
    {
        ESP_LOGD("nvsOpen", "nvs is open");
        // Save chars into theNVS
        size_t str_saved = nvs.setString("key", "value");
        if (str_saved > 0)
        {
            ESP_LOGD("nvs_set", "saved %d bytes", str_saved);
            // Read chars from the NVS
            const char * readString = nvs.getString("key", "ERROR");

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
    }
    else
    {
        ESP_LOGE("nvsOpen", "nvs is not open");
    }
}

void loop()
{
    //
}