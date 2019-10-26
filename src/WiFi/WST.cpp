/**
 * @file WST.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#include "WST.h"
#include "NVS/SingletonNVS.h"
#include "hal/hardware.h"

esp_err_t WST::begin()
{
    SingletonNVS* nvs = SingletonNVS::getInstance();

    // Declare and define default values, before reading NVS
    char *wst_ssid = WST_SSID;
    char *wst_pass = WST_PASS;

    // Open nvs
    bool isOpen = nvs->open(NVS_WIFI_NAMESPACE, false);
    // Verifying if opened
    if (isOpen)
    {
        // This is just for testing purposes
        //=========================================================
        // Change the Wi-Fi mode permanently here...
        // nvs.setString(NVS_WST_SSID_KEY, "YOUR_SSID");
        // nvs.setString(NVS_WST_PASS_KEY, "YOUR_PASS");
        //=========================================================

        // get values from nvs or set default values from hal/hardware.h
        wst_ssid = nvs->getString(NVS_WST_SSID_KEY, WST_SSID);
        wst_pass = nvs->getString(NVS_WST_PASS_KEY, WST_PASS);
    }
    else
    {
        ESP_LOGE(__func__, "Error opening the NVS");
    }

    bool initST = WiFi.begin(wst_ssid, wst_pass);

    if (initST)
    {
        ESP_LOGD(__func__, "Connected to %s", wst_ssid);
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(__func__, "Can't connect to %s", wst_ssid);
        return ESP_FAIL;
    }
}