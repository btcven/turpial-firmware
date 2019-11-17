/**
 * @file WAP.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#include "WAP.h"

namespace wifi {

namespace wap {

esp_err_t begin(Config config)
{
    bool init_ap = WiFi.softAP(config.ap_ssid, config.ap_pass, config.ap_channel, 0, config.ap_max_conn);

    if (init_ap) {
        ESP_LOGI(__func__, "AP init OK");
        return ESP_OK;
    } else {
        ESP_LOGE(__func__, "AP init ERROR");
        return ESP_FAIL;
    }
}

} // namespace wap

} // namespace wifi