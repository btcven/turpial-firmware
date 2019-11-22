/**
 * @file BLEPreferences.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-25
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BLE_PREFERENCES_H
#define BLE_PREFERENCES_H

#include "esp_err.h"

#include "Server.h"

namespace ble_preferences {

void start(ble::ServerParams server_params);

} // namespace ble_preferences

#endif // BLE_PREFERENCES_H