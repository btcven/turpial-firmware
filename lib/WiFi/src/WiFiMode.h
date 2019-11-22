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

#include "esp_err.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "Task.h"
#include "defaults.h"



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


class WiFiMode : public Task
{
public:
    /**
     * @brief Initialize Wi-Fi
     * 
     * @attention 1. If you set the parameter "use_nvs" to true you must make
     * sure that the NVS is initialized.
     * 
     * @param use_nvs: wether to use NVS for storage or not
     * 
     * 
     * 
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
     * @brief Start Wi-Fi operation mode
     * 
     * @attention 1. WiFiMode::init must have been called
     * 
     * @return
     *      - ESP_OK: succeed
     *      - (others): failed
     */
    esp_err_t init();

    

private:

    void run(void* data) override {
        int is_nvs_initialized= (int)data; //aqui deberiamos tener un objeto seriaalizado
        esp_err_t err;
            err = init((bool)is_nvs_initialized); //is_nvs_initialized
            if (err != ESP_OK) {
                const char* err_name = esp_err_to_name(err);
                ESP_LOGE("tESTfrertos", "Couldn't initalize Wi-Fi interface (%s)", err_name);
                // TODO: fallback to bluetooth mode to configure Wi-Fi?
                return;
            }
            if (is_nvs_initialized <= 1) { //nvs initialized but not configured
                set_mode(WIFI_MODE);

                APConfig ap_config = {
                .ssid = WAP_SSID,
                .password = WAP_PASS,
                .authmode = WAP_AUTHMODE,
                .max_conn = WAP_MAXCONN,
                .channel = WAP_CHANNEL,
                };
                set_ap_config(ap_config);

                STAConfig sta_config = {
                .ssid = WST_SSID,
                .password = WST_PASS,
                };
                set_sta_config(sta_config);
            }
            
            init();
            while(1) {
                vTaskDelay(10 / portTICK_PERIOD_MS); //don't remove this line
            }
    }
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