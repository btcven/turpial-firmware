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

namespace wifi {

namespace wst {

esp_err_t begin(Config config)
{
    bool init_st = WiFi.begin(config.ssid, config.pass);

    if (init_st) {
        ESP_LOGD(__func__, "Connected to %s", config.ssid);
        return ESP_OK;
    } else {
        ESP_LOGE(__func__, "Can't connect to %s", config.ssid);
        return ESP_FAIL;
    }
}

} // namespace wst

} // namespace wifi