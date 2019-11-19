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
#include "WiFi.h"
#include "WiFiDTO.h"


namespace wifi {

namespace mode {

/**
 * @brief WAP/WST operation modes
 * 
 */
enum class OperationMode {
    /// No operation mode selected
    None = WIFI_MODE_NULL,
    /// WST only
    St = WIFI_STA,
    /// WAP onyly
    Ap = WIFI_AP,
    // Both WAP/WST
    ApSt = WIFI_AP_STA,
};

/**
 * @brief Select the WiFi operation mode
 * 
 * @param[in] ap: AP mode.
 * @param[in] st: ST mode.
 *  
 * @return OperationMode the selected operation mode 
 */
OperationMode selectOperationMode(bool ap, bool st);

/**
 * @brief Handle WiFi interface events
 * 
 * @param[in] evt: a WiFi event 
 */
void handleWiFiEvent(WiFiEvent_t evt);

/**
 * @brief Initialize WiFi with the given modes specified in "DTOConfig"
 * 
 * @param[in] wifi_params: WiFi DTO configuration parameters
 * 
 * @return
 *      - ESP_OK: on success
 */
esp_err_t begin(DTOConfig wifi_params);

} // namespace mode

} // namespace wifi

#endif // WIFIMODE_H