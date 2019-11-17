/**
 * @file WAP.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#ifndef WAP_H
#define WAP_H

#include "WiFi.h"

namespace wifi {

namespace wap {

/**
 * @brief WAP interface configuration
 * 
 */
struct Config {
    /// The AP SSID
    const char* ap_ssid;
    /// The AP passowrd
    const char* ap_pass;
    /// AP channel
    int8_t ap_channel;
    /// AP max conn
    int8_t ap_max_conn;
};

/**
 * @brief Initialize WAP interface
 * 
 * @param config WAP configuration
 * @return esp_err_t ESP_OK on success
 */
esp_err_t begin(Config config);

} // namespace wap

} // namespace wifi

#endif // WAP_H