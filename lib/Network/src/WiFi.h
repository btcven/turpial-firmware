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

#include "WiFiEventHandler.h"

namespace network {

/**
 * @brief AP mode configuration
 * 
 */
struct APConfig {
    const char* ssid;
    const char* password;
    wifi_auth_mode_t authmode;
    std::uint8_t max_conn;
    std::uint8_t channel;
};

/**
 * @brief STA mode configuration
 * 
 */
struct STAConfig {
    const char* ssid;
    const char* password;
};

class WiFi
{
public:
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
     * @attention 1. If you set the parameter "use_nvs" to true you must make
     * sure that the NVS is initialized.
     * 
     * @param use_nvs: wether to use NVS for storage or not
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
     * @brief Start Wi-Fi operation mode
     * 
     * @attention 1. WiFiMode::init must have been called
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t start();

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
     * @brief pointer to desired hanlder events
     *
     */
    std::unique_ptr<WiFiEventHandler> m_event_handler;

    /**
     * @brief Wi-Fi event handler
     * 
     * @param ctx: application specific data (currently a pointer to the class
     * instance)
     * @param event: Wi-Fi event
     */
    static esp_err_t eventHandler(void* ctx, system_event_t* event);
};

} // namespace network

#endif // NETWORK_WIFI_H