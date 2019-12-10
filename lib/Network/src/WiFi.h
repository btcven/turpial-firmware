/**
 * @file WiFi.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#ifndef NETWORK_WIFI_H
#define NETWORK_WIFI_H

#include <cstdint>

#include <esp_err.h>
#include <esp_event.h>
#include <esp_wifi.h>

#include "Sema.h"
#include "WiFiEventHandler.h"

namespace network {

/**
 * @brief Wi-Fi default event handler
 * 
 */
class WiFiDefaultEventHandler : public WiFiEventHandler
{
public:
    /**
     * @brief Construct a new Wi-Fi default event handler
     * 
     */
    WiFiDefaultEventHandler();

    /**
     * @brief Handles the "STA Start" event, starts connection to AP.
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t staStart();

    /**
     * @brief Handles the "STA Stop" event
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t staStop();

private:
    friend class WiFi;

    util::Semaphore m_sta_start_sema; /*!< "STA Start" event semaphore */
    util::Semaphore m_sta_stop_sema;  /*!< "STA Stop" event semaphore */
};

/**
 * @brief AP mode configuration
 * 
 */
struct APConfig {
    const char* ssid;          /*!< AP SSID */
    const char* password;      /*!< AP Password */
    wifi_auth_mode_t authmode; /*!< AP Authentication mode*/
    std::uint8_t max_conn;     /*!< AP Maximum connections */
    std::uint8_t channel;      /*!< AP Channel */
};

/**
 * @brief STA mode configuration
 * 
 */
struct STAConfig {
    const char* ssid;     /*!< Station SSID */
    const char* password; /*!< Station Password */
};

/**
 * @brief Wi-Fi manager class
 * 
 */
class WiFi
{
public:
    /**
     * @brief Get the unique instance of the object
     * 
     * @return WiFi& 
     */
    static WiFi& getInstance()
    {
        static WiFi instance;
        return instance;
    }

    WiFi(WiFi const&) = delete; // Copy construct
    WiFi(WiFi&&) = delete;      // Move construct

    WiFi& operator=(WiFi const&) = delete; // Copy assign
    WiFi& operator=(WiFi&&) = delete;      // Move assign

    /**
     * @brief Initialize Wi-Fi
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t init();

    /**
     * @brief Set Wi-Fi operation mode
     * 
     * @param mode: operation mode
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setMode(wifi_mode_t mode);

    /**
     * @brief Get the Wi-Fi operation mode
     * 
     * @param mode: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getMode(wifi_mode_t& mode);

    /**
     * @brief Set the AP configuration
     * 
     * @attention 1. If "use_nvs" was set to true when WiFiMode was initialized
     * this configuration is going to be saved to the NVS.
     * 
     * @param ap_config: AP mode configuration
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setApConfig(APConfig& ap_config);

    /**
     * @brief Set the AP configuration
     * 
     * @attention 1. If "use_nvs" was set to true when WiFiMode was initialized
     * this configuration is going to be saved to the NVS.
     * 
     * @param ap_config: AP mode configuration
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setApConfig(wifi_config_t& ap_config);

    /**
     * @brief Set the STA configuration
     * 
     * @attention 1. If "use_nvs" was set to true when WiFiMode was initialized
     * this configuration is going to be saved to the NVS.
     * 
     * @param sta_config: ST mode configuration
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setStaConfig(STAConfig& sta_config);

    /**
     * @brief Set the STA configuration
     * 
     * @attention 1. If "use_nvs" was set to true when WiFiMode was initialized
     * this configuration is going to be saved to the NVS.
     * 
     * @param sta_config: ST mode configuration
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setStaConfig(wifi_config_t& sta_config);

    /**
     * @brief Get the AP configuration
     * 
     * @param[out] ap_config: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getApConfig(wifi_config_t& ap_config);

    /**
     * @brief Get the STA configuration
     * 
     * @param[out] sta_config: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getStaConfig(wifi_config_t& sta_config);

    /**
     * @brief Checks if the Wi-Fi mode is AP
     * 
     * @return true: is AP
     * @return false: isn't AP
     */
    bool isAp();

    /**
     * @brief Checks if the Wi-Fi mode is STA
     * 
     * @return true: is AP
     * @return false: is STA
     */
    bool isSta();

    /**
     * @brief Start Wi-Fi operation mode
     * 
     * @attention 1. WiFi::init must have been called
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t start();

    /**

     * @brief Stop WiFi
     * 
     * @return esp_err_t 
     */
    esp_err_t stop();

    /**
     * @brief
     * 
     * @attention 1. WiF::init must have been called
     * 
     * @return
     *      - ESP_OK: succeed
     *      -(others) : failed
     */
    esp_err_t connect();

    /**
     * @brief Set callback handler to catch WiFi events outside of class itself
     * 
     */
    void setEventHandler(std::unique_ptr<WiFiEventHandler>&& event_handler);

private:
    /**
     * @brief Constructor Wi-Fi
     *
     */
    WiFi();

    /**
     * @brief Wi-Fi event handler
     * 
     * @param ctx: application specific data (currently a pointer to the class
     * instance)
     * @param event: Wi-Fi event
     */
    static esp_err_t eventHandler(void* ctx, system_event_t* event);

    WiFiDefaultEventHandler m_event_handler; /*!< Pointer to desired hanlder events */
};

/**
 * @brief Sanitize SSIDs
 * 
 * @param[inout] ssid: SSID characters (bytes, not null terminated)
 * @param        len: length of the ssid
 * 
 * @return esp_err_t 
 */
esp_err_t sanitizeSsid(std::uint8_t (&ssid)[32], std::size_t len);

} // namespace network

#endif // NETWORK_WIFI_H