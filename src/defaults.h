/**
 * @file defaults.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-07
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#ifndef DEFAULTS_H_
#define DEFAULTS_H_

// NVS related defines
#define NVS_APP_NAMESPACE "turpial"
#define NVS_IS_CONFIGURED_KEY "is_configured"

// ESC enabled?
#define ESC_ENABLED true
#define ESC_MAX_BATTERY_CAPACITY 2200 // in mAh unit
#define ESC_LOW_BATTTERY_THRESHOLD 20 // in % unit
#define ESC_CRITICAL_BAT_THRESHOLD 7  // in % unit

// BLE enabled?
#define BLE_ENABLED false
// Radio's subsystem enabled?
#define RAD_ENABLED false

/**
 * @brief WiFi AP/ST default params.
 * 
 */
#define WIFI_MODE WIFI_MODE_AP
#define WST_SSID "HOME-EB05"
#define WST_PASS "D8ED78ECC6372942"
#define WAP_SSID "io.locha.ap"
#define WAP_PASS "default_password"
#define WAP_CHANNEL 6
#define WAP_MAXCONN 4
#define WAP_AUTHMODE WIFI_AUTH_WPA_WPA2_PSK

/**
 * @brief When available, define where the Arduino loop is running.
 * 
 */
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif // CONFIG_FREERTOS_UNICORE

#endif // DEFAULTS_H_