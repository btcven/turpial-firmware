/**
 * @file Preferences.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-29
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#ifndef UTIL_PREFERENCES_H
#define UTIL_PREFERENCES_H

#include <cstddef>
#include <cstdint>

#include "esp_wifi.h"

#include "NVS.h"

namespace util {

/**
 * @brief Turpial default preferences
 * 
 */
class DefaultPreferences
{
public:
    std::uint8_t ap_ssid[32];      /*!< AP SSID */
    std::uint8_t ap_password[64];  /*!< AP Password */
    std::uint8_t ap_max_conn;      /*!< AP Maximum connection */
    std::uint8_t ap_channel;       /*!< AP Channel */
    std::uint8_t sta_ssid[32];     /*!< STA SSID */
    std::uint8_t sta_password[64]; /*!< STA Password */
    wifi_mode_t wifi_mode;         /*!< Wi-Fi Operation mode */

    /**
     * @brief Set the default AP SSID
     * 
     * @param ssid: Null-terminated C string
     */
    void setApSSID(const char* ssid);

    /**
     * @brief Set the default AP Password
     * 
     * @param password: Null-terminated C string
     */
    void setApPassword(const char* password);

    /**
     * @brief Set the default AP Maximum connections
     * 
     * @param max_conn: maximum connections
     */
    void setApMaxConn(std::uint8_t max_conn);

    /**
     * @brief Set the default AP channel
     * 
     * @param channel: channel
     */
    void setApChannel(std::uint8_t channel);

    /**
     * @brief Set the Sta Ssid object
     * 
     * @param ssid: Null-terminated C string
     */
    void setStaSsid(const char* ssid);

    /**
     * @brief Set the Sta Password object
     * 
     * @param password: Null-terminated C string
     */
    void setStaPassword(const char* password);

    /**
     * @brief Set the Wi Fi Mode object
     * 
     * @param mode: Null-terminated C string
     */
    void setWiFiMode(wifi_mode_t mode);
};

/**
 * @brief Representation of the user preferences
 * 
 */
class Preferences
{
public:
    Preferences(Preferences const&) = delete;
    Preferences(Preferences&&) = delete;

    Preferences& operator=(Preferences const&) = delete;
    Preferences& operator=(Preferences&&) = delete;

    /**
     * @brief Returns the instance of Preferences
     * 
     * @return Preferences& the unique instance
     */
    static Preferences& getInstance()
    {
        // Instatiated only once, won't be created again
        static Preferences g_instance;
        return g_instance;
    }

    /**
     * @brief Start the Preferences
     * 
     * @attention 1. Call this function only once when the program starts.
     * 
     * @param default_config: default user configuration
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t start(DefaultPreferences default_conifg);

    /**
     * @brief Set the the "use default" preference
     * 
     * @attention 1. If you set this to true the next time that
     * Preferences::start() is called all the preferences are going to be
     * resetted.
     * 
     * @param value: use default value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setUseDefault(bool value);

    /**
     * @brief Get the "use default" preference
     * 
     * @param[out] value: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getUseDefault(bool& value);

    /**
     * @brief Set the AP SSID
     * 
     * @attention 1. The byte-string must be null-terminated and the maximum 
     * character size is of 31, so in total 32 bytes maximum including
     * null-terminator.
     * 
     * @param data: SSID
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */

    esp_err_t setApSSID(std::uint8_t* data);

    /**
     * @brief Get the AP SSID
     * 
     * @attention 1. data must be at least 32 bytes to store the SSID, the
     * returned string is null-terminated and has maximum 31 characters on it.
     * 
     * @param[out] data: result value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getApSSID(std::uint8_t* data);

    /**
     * @brief Set the AP Password
     * 
     * @attention 1. The byte-string must be null-terminated and the maximum 
     * character size is of 63, so in total 64 bytes maximum including
     * null-terminator.
     * 
     * @param data: password
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setApPassword(std::uint8_t* data);

    /**
     * @brief Get the AP Password object
     * 
     * @attention 1. data must be at least 64 bytes to store the password, the
     * returned string is null-terminated and has maximum 63 characters on it.
     * 
     * @param[out] data: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getApPassword(std::uint8_t* data);

    /**
     * @brief Set the AP maximum connections
     * 
     * @param ap_max_conn: AP Maximum connections
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setApMaxConn(std::uint8_t ap_max_conn);

    /**
     * @brief Get the AP maximum connections
     * 
     * @param[out] ap_max_conn: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getApMaxConn(std::uint8_t& ap_max_conn);

    /**
     * @brief Set the AP channel
     * 
     * @param ap_channel: AP channel
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setApChannel(std::uint8_t ap_channel);

    /**
     * @brief Get the Ap Channel object
     * 
     * @param[out] ap_channel: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getApChannel(std::uint8_t& ap_channel);

    /**
     * @brief Set the STA SSID
     * 
     * @attention 1. The byte-string must be null-terminated and the maximum 
     * character size is of 31, so in total 32 bytes maximum including
     * null-terminator.
     * 
     * @param data: SSID
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setStaSSID(std::uint8_t* data);

    /**
     * @brief Get the STA SSID
     * 
     * @attention 1. data must be at least 32 bytes to store the SSID, the
     * returned string is null-terminated and has maximum 31 characters on it.
     * 
     * @param[out] data: return value 
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getStaSSID(std::uint8_t* data);

    /**
     * @brief Set the STA Password
     * 
     * @attention 1. The byte-string must be null-terminated and the maximum 
     * character size is of 63, so in total 64 bytes maximum including
     * null-terminator.
     * 
     * @param data 
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setStaPassword(std::uint8_t* data);

    /**
     * @brief Get the STA Password
     * 
     * @attention 1. data must be at least 64 bytes to store the password, the
     * returned string is null-terminated and has maximum 63 characters on it.
     * 
     * @param[out] data: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getStaPassword(std::uint8_t* data);

    /**
     * @brief Set the Wi-Fi Mode
     * 
     * @param wifi_mode: operation mode
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setWiFiMode(wifi_mode_t wifi_mode);

    /**
     * @brief Get the Wi-Fi Mode
     * 
     * @param[out] wifi_mode: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getWiFiMode(wifi_mode_t& wifi_mode);

    /**
     * @brief Commit changes to flash
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t commit();

private:
    /**
     * @brief Construct a new Namespace
     * 
     */
    Preferences();

    storage::NVS m_nvs; /*!< NVS instanc */
};

} // namespace util

#endif // UTIL_PREFERENCES_H