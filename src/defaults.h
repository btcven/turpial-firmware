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

#ifndef DEFAULTS_H
#define DEFAULTS_H

#define NVS_APP_NAMESPACE "turpial"           /*!< NVS application namespace */
#define NVS_IS_CONFIGURED_KEY "is_configured" /*!< Is turpial configured? */

#define ESC_ENABLED true              /*!< ESC feature enabled? */
#define ESC_MAX_BATTERY_CAPACITY 2200 /*!< in mAh unit */
#define ESC_LOW_BATTTERY_THRESHOLD 20 /*!< in % unit */
#define ESC_CRITICAL_BAT_THRESHOLD 7  /*!< in % unit */

#define BLE_ENABLED false /*!< BLE feature enabled? */
#define RAD_ENABLED false /*!< Radio subsystem enabled? */

#define WIFI_MODE WIFI_MODE_AP              /*!< Wi-Fi Operation mode */
#define WST_SSID ""                         /*!< STA SSID */
#define WST_PASS ""                         /*!< STA Password */
#define WAP_SSID "io.locha.ap"              /*!< AP SSID */
#define WAP_PASS "default_password"         /*!< AP Password */
#define WAP_CHANNEL 6                       /*!< AP Channel */
#define WAP_MAXCONN 4                       /*!< AP Maximum connections */
#define WAP_AUTHMODE WIFI_AUTH_WPA_WPA2_PSK /*!< AP Authentication mode */

#endif // DEFAULTS_H