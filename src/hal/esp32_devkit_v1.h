/**
 * @file featheresp32.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef ESP32_DEVKIT_V1
#define ESP32_DEVKIT_V1

#define ESC_ENABLED true
#define RAD_ENABLED false
#define BLE_ENABLED false
#define WST_ENABLED false
#define WAP_ENABLED false

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

/**
 * @brief WiFi Station default params.
 * 
 */
#define WST_SSID    ""
#define WST_PASS    ""

/**
 * @brief WiFi AP default params.
 * 
 */
#define WAP_SSID "io.locha.ap"
#define WAP_PASS ""
#define WAP_CHANNEL 6
#define WAP_MAXCONN 4

/**
 * @brief define where the Arduino loop is running.
 * 
 */
#if CONFIG_FREERTOS_UNICORE
    #define ARDUINO_RUNNING_CORE 0
#else
    #define ARDUINO_RUNNING_CORE 1
#endif // CONFIG_FREERTOS_UNICORE

#endif // ESP32_DEVKIT_V1