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

#include <cstdint>

#include "WiFiEventHandler.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_wifi.h"

namespace wifi {

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

class WiFiMode
{
public:
    /**
     * @brief Destructor WiFiMode
     */
    ~WiFiMode();

    WiFiMode(WiFiMode const&) = delete;
    WiFiMode(WiFiMode&&) = delete;

    WiFiMode& operator=(WiFiMode const&) = delete;
    WiFiMode& operator=(WiFiMode&&) = delete;

    static WiFiMode& getInstance()
    {
        // Instatiated only once, won't be created again
        static WiFiMode g_instance;
        return g_instance;
    }

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
    esp_err_t init(bool use_nvs);

    /**
     * @brief Set Wi-Fi operation mode
     * 
     * @param mode: operation mode
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t set_mode(wifi_mode_t mode);

    /**
     * @brief Get the Wi-Fi operation mode
     * 
     * @param mode: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t get_mode(wifi_mode_t& mode);

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
    esp_err_t set_ap_config(APConfig& ap_config);

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
    esp_err_t set_ap_config(wifi_config_t& ap_config);

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
    esp_err_t set_sta_config(STAConfig& sta_config);

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
    esp_err_t set_sta_config(wifi_config_t& sta_config);

    /**
     * @brief Get the AP configuration
     * 
     * @param[out] ap_config: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t get_ap_config(wifi_config_t& ap_config);

    /**
     * @brief Get the STA configuration
     * 
     * @param[out] sta_config: return value
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t get_sta_config(wifi_config_t& sta_config);

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
     * @brief Stop WiFi
     * 
     * @return esp_err_t 
     */
    esp_err_t stop();

    /**
     * @brief Set callback handler to catch WiFi events outside of class itself
     *      
     */
    void setWiFiEventHandler(WiFiEventHandler* WiFiEventHandler);

private:
    /**
     * @brief Constructor WiFiMode
     */
    WiFiMode();

    /**
     * @brief pointer to desired hanlder events
     *
     */
    WiFiEventHandler* m_p_wifi_event_handler;

    /**
     * @brief Wi-Fi event handler
     * 
     * @param ctx: application specific data (currently a pointer to the class
     * instance)
     * @param event: Wi-Fi event
     */
    static esp_err_t eventHandler(void* ctx, system_event_t* event);
};

} // namespace wifi

#endif // WIFIMODE_H