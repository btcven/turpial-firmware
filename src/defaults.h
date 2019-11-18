/**
 * @file defaults.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-07
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef DEFAULTS_H_
#define DEFAULTS_H_

// NVS related defines
#define NVS_WIFI_NAMESPACE "wifi_config"
#define NVS_WAP_ENABLED_KEY "WAP_enabled"
#define NVS_WST_ENABLED_KEY "WST_enabled"
#define NVS_WAP_SSID_KEY "WAP_ssid"
#define NVS_WAP_PASS_KEY "WAP_pass"
#define NVS_WAP_CHANNEL_KEY "WAP_channel"
#define NVS_WAP_MAXCONN_KEY "WAP_maxconn"
#define NVS_WST_SSID_KEY "WST_ssid"
#define NVS_WST_PASS_KEY "WST_pass"


// ESC enabled?
#define ESC_ENABLED true
#define ESC_PERCENTAGE_CHANGE_INTERVAL 1 // in % unit
#define ESC_MAX_BATTERY_CAPACITY 2200    // in mAh unit
#define ESC_LOW_BATTTERY_THRESHOLD 20    // in % unit
#define ESC_CRITICAL_BAT_THRESHOLD 7     // in % unit
#define ESC_I2C_CLOCK_FREQ 100000        // in Hz, default is 100 kHz, TODO: sdkconfig.h?
#define ESC_I2C_PORT I2C_NUM_0
#define ESC_I2C_SDA GPIO_NUM_23
#define ESC_I2C_SCL GPIO_NUM_22
#define ESC_GPIO_GPOUT GPIO_NUM_14

// BLE enabled?
#define BLE_ENABLED false
// Radio's subsystem enabled?
#define RAD_ENABLED false

/**
 * @brief WiFi AP/ST default params.
 * 
 */
#define WST_ENABLED true
#define WST_SSID "HOME-EB05"
#define WST_PASS "D8ED78ECC6372942"

#define WAP_ENABLED false
#define WAP_SSID "io.locha.ap"
#define WAP_PASS ""
#define WAP_CHANNEL 6
#define WAP_MAXCONN 4

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