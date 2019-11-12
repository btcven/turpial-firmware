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

#include "Arduino.h"
#include "WiFiDTO.h"
#include "WiFi.h"

class WiFiMode 
{
    public:
        /**
         * @brief Construct a new WiFiMode object
         * 
         */
        WiFiMode(WiFiDTOConfig wifi_params);
        /**
         * @brief Destroy the WiFiMode object
         * 
         */
        //~WiFiMode();

        /**
         * @brief 
         * 
         * @return esp_err_t
         */
        esp_err_t begin();

        /**
         * @brief Select Wi-Fi Mode, analyzing its parameters
         * 
         * @return wifi_mode_t
         */
        //void WiFiEvent(WiFiEvent_t evt);

    private:
        wifi_mode_t selectMode(bool AP, bool ST);

        WiFiDTOConfig _wifi_config;
};   

/**
 * @brief Initialize WiFi ifaces
 * 
 * @return esp_err_t 
 */
//esp_err_t WiFi_INIT();
void WiFiEvent(WiFiEvent_t evt);

#endif // WIFIMODE_H