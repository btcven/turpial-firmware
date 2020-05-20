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
 * @brief       Wi-Fi Event Handler
 */

#ifndef NETWORK_WIFI_EVENT_HANDLER_H
#define NETWORK_WIFI_EVENT_HANDLER_H

#include <memory>

#include <esp_event.h>
#include <esp_wifi.h>
#include <esp_netif.h>

namespace network {

/**
 * @brief Methods that will be called back when a Wi-Fi event happens
 *
 * Typically this class is subclassed to provide implementations for the
 * callbacks we want to handle:
 *
 * @code{cpp}
 * class MyHandler : public WiFiEventHandler {
 *     esp_err_t apStart() {
 *         ESP_LOGD(tag, "MyHandler(Class): apStart");
 *         return ESP_OK;
 *     }
 * }
 * @endcode
 */
class WiFiEventHandler
{
public:
    /**
     * @brief Construct a new Wi-Fi event handler object
     *
     */
    WiFiEventHandler();

    /**
     * @brief "Scan Done" handler
     *
     * @param info: event information
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t staScanDone(wifi_event_sta_scan_done_t* info);

    /**
     * @brief "Station Start" handler
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t staStart();

    /**
     * @brief "Station Stop" handler
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t staStop();

    /**
     * @brief "Station Connected" handler
     *
     * @param info: event information
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t staConnected(wifi_event_sta_connected_t* info);

    /**
     * @brief "Station Disconnected" event
     *
     * @param info: event information
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t staDisconnected(wifi_event_sta_disconnected_t* info);

    /**
     * @brief "Authentication Mode Change" handler
     *
     * @param info: event information
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t staAuthChange(wifi_event_sta_authmode_change_t* info);

    /**
     * @brief "Station Got IP" handler
     *
     * @param info: event information
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t staGotIp(ip_event_got_ip_t* info);

    /**
     * @brief "AP Start" handler
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t apStart();

    /**
     * @brief "AP Stop" handler
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t apStop();

    /**
     * @brief "AP Station Connected" handler
     *
     * @param info: event information
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t apStaConnected(wifi_event_ap_staconnected_t* info);

    /**
     * @brief "Station Disconnected" from AP event
     *
     * @param info: event information
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    virtual esp_err_t apStaDisconnected(wifi_event_ap_stadisconnected_t* info);

    /**
     * @brief Set the next WiFi event handler in the chain
     *
     * @param next_handler: the next Wi-Fi event handler in the chain
     */
    void setNextHandler(std::unique_ptr<WiFiEventHandler>&& next_handler)
    {
        m_next_handler = std::move(next_handler);
    }

    /**
     * @brief Dispatch events by calling the respective WiFiEventHander methods
     *
     * @param[in] event_id   The Wi-Fi event ID.
     * @param[in] event_data Data about the event
     *
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t eventDispatcher(std::int32_t event_id, void* event_data);

private:
    std::unique_ptr<WiFiEventHandler> m_next_handler;
};

} // namespace network

#endif // NETWORK_WIFI_EVENT_HANDLER_H
/** @} */
