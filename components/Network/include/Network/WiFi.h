/**
 * Copyright 2020 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** 
 * @ingroup     network
 * @{
 *
 * @file
 * @author      Locha Mesh Developers (contact@locha.io)
 * @brief       Wi-Fi network interface
 */

#ifndef NETWORK_WIFI_H
#define NETWORK_WIFI_H

#include <cstdint>
#include <vector>

#include <esp_err.h>
#include <esp_event.h>
#include <esp_wifi.h>

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
     * @param ap_config: AP mode configuration
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setApConfig(wifi_config_t& ap_config);

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
     * @brief Set the STA configuration
     *
     * @param sta_config: ST mode configuration
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t setStaConfig(wifi_config_t& sta_config);

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

    /**
     * @brief get connected client
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t getConnectedList(wifi_sta_list_t& sta);


private:
    /**
     * @brief Constructor Wi-Fi
     *
     */
    WiFi();

    /**
     * @brief Wi-Fi event handler
     *
     * @param[in] event_handler_arg callback handler argument (in this case
     *                              our object).
     * @param[in] event_base        WIFI_EVENT
     * @param[in] event_id          Id of the event.
     * @param[in] event_data        Data of the event (event_id specifies the
     *                              type to use).
     */
    static void eventHandler(void* event_handler_arg,
        esp_event_base_t event_base,
        std::int32_t event_id,
        void* event_data);

    static void ipEventHandler(void* event_handler_arg,
        esp_event_base_t event_base,
        std::int32_t event_id,
        void* event_data);

    WiFiDefaultEventHandler m_event_handler; /*!< Pointer to desired handler event */


    esp_netif_t* m_ap_netif;
};

} // namespace network

#endif // NETWORK_WIFI_H
