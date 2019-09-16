/**
 * @file WiFiMode.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#ifndef WIFIMODE_H
#define WIFIMODE_H
#include <Arduino.h>

/**
 * @brief Initialize WiFi ifaces
 * 
 * @return esp_err_t 
 */
esp_err_t WiFi_INIT();

#endif // WIFIMODE_H