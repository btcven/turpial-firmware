/**
 * @file WST.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#ifndef WST_H
#define WST_H

#include "Arduino.h"
#include "WiFi.h"

namespace wifi {

namespace wst {

/**
 * @brief Wireless ST configuration
 * 
 */
struct Config {
    /// The SSID to connect to
    const char* ssid;
    /// The password
    const char* pass;
};

/**
 * @brief Initializes WST wifi mode
 * 
 * @param config The WST configuration
 * @return esp_err_t ESP_OK on success
 */
esp_err_t begin(Config config);

} // namespace wst

} // namespace wifi

#endif // WST_H