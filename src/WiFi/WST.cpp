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
#include "NVS/SingleNVS.h"

WST::WST(char* ssid, char* pass) {
    wst_ssid_ = ssid;
    wst_pass_ = pass;
}
esp_err_t WST::begin()
{
    bool initST = WiFi.begin(wst_ssid_, wst_pass_);

    if (initST)
    {
        ESP_LOGD(__func__, "Connected to %s", wst_ssid_);
        return ESP_OK;
    }
    else
    {
        ESP_LOGE(__func__, "Can't connect to %s", wst_ssid_);
        return ESP_FAIL;
    }
}