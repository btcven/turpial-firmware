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

#include "esp_event.h"

#include "WiFiDTO.h"

namespace wifi {

/**
 * @brief Manages WiFi connections
 * 
 */
class WiFiMode
{
public:
    WiFiMode();
    /**
    * @brief Initialize WiFiMode interface
    * 
    * @attention 1. Before using this method make sure that the NVS is already
    * initialized, see nvs::begin()
    * 
    * @param[in] dto_config: WiFi interface mode configuration
    * 
    * @return
    *       - ESP_OK: succeed
    */
    esp_err_t begin(const DTOConfig& dto_config);

private:
    /**
     * @brief Set the STA interface configuration
     * 
     * @param[out] dto_config: WiFi STA configuration parameters
     * 
     * @return
     *      - ESP_OK: succeed 
     */
    esp_err_t setSTAConfig(const DTOConfig& dto_config);

    /**
     * @brief Set the AP interface configuration
     * 
     * @param[in] dto_config: WiFi AP configuration parameters
     * 
     * @return
     *      - ESP_OK: succeed
     *      - ESP_FAIL: when dto_conifg.ap_ssid length is >= 32, or when
     * dto_config.ap_password length is >= 64, or when dto_config.ap_max_conn
     * is >= 4.
     */
    esp_err_t setAPConfig(const DTOConfig& dto_config);

    /**
     * @brief Handle WiFi events
     * 
     * @param[in] ctx: custom argument passed to event handler, currently this
     * is a poiner to WiFiMode 
     * @param[in] event: the event
     * 
     * @return
     *      - ESP_OK: succeed
     */
    static esp_err_t eventHandler(void* ctx, system_event_t* event);
};


} // namespace wifi

#endif // WIFIMODE_H