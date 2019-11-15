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

namespace wifi {

namespace mode {

enum class OperationMode {
    None = WIFI_MODE_NULL,
    St = WIFI_STA,
    Ap = WIFI_AP,
    ApSt = WIFI_AP_STA,
};

OperationMode selectOperationMode(bool ap, bool st);
void handleWiFiEvent(WiFiEvent_t evt);

esp_err_t begin(DTOConfig wifi_params);

} // namespace mode

} // namespace wifi

#endif // WIFIMODE_H