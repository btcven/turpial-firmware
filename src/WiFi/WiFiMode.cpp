/**
 * @file WiFiMode.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "WiFiMode.h"
#include "WAP.h"
#include "WST.h"

WiFiMode::WiFiMode(wifi_dto_config_t& wifi_params) {
    wifi_config_ = &wifi_params;
    WAP_enabled = wifi_config_->WAP_enabled; // Default value
    WST_enabled = wifi_config_->WST_enabled; // Default value
}

void WiFiEvent(WiFiEvent_t evt)
{
    switch (evt)
    {
    case SYSTEM_EVENT_WIFI_READY:
        ESP_LOGI(__func__, "Event -> WiFi ready #%d", evt);
        break;
    case SYSTEM_EVENT_SCAN_DONE:
        ESP_LOGI(__func__, "Event -> Scann done #%d", evt);
        break;
    case SYSTEM_EVENT_STA_START:
        ESP_LOGI(__func__, "Event -> WST start #%d", evt);
        break;
    case SYSTEM_EVENT_STA_STOP:
        ESP_LOGI(__func__, "Event -> WST stop #%d", evt);
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        ESP_LOGI(__func__, "Event -> WST connected #%d", evt);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGI(__func__, "Event -> WST disconnected #%d", evt);
        break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
        ESP_LOGI(__func__, "Event -> WST auth mode change #%d", evt);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGI(__func__, "Event -> WST got IP #%d", evt);
        break;
    case SYSTEM_EVENT_STA_LOST_IP:
        ESP_LOGI(__func__, "Event -> WST lost IP #%d", evt);
        break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
        ESP_LOGI(__func__, "Event -> WST WPS er success #%d", evt);
        break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
        ESP_LOGI(__func__, "Event -> WST WPS er failed #%d", evt);
        break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
        ESP_LOGI(__func__, "Event -> WST er timeout #%d", evt);
        break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
        ESP_LOGI(__func__, "Event -> WST er pin #%d", evt);
        break;
    case SYSTEM_EVENT_AP_START:
        ESP_LOGI(__func__, "Event -> AP start #%d", evt);
        break;
    case SYSTEM_EVENT_AP_STOP:
        ESP_LOGI(__func__, "Event -> AP stop #%d", evt);
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGI(__func__, "Event -> Client connected #%d", evt);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGI(__func__, "Event -> Client disconnected #%d", evt);
        break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
        ESP_LOGI(__func__, "Event -> IP assigned to client #%d", evt);
        break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
        ESP_LOGI(__func__, "Event -> PROBEREQRECVED #%d", evt);
        break;
    case SYSTEM_EVENT_GOT_IP6:
        ESP_LOGI(__func__, "Event -> got ipv6 #%d", evt);
        break;
    case SYSTEM_EVENT_ETH_START:
        ESP_LOGI(__func__, "Event -> ETH start #%d", evt);
        break;
    case SYSTEM_EVENT_ETH_STOP:
        ESP_LOGI(__func__, "Event -> ETH stop #%d", evt);
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        ESP_LOGI(__func__, "Event -> ETH connected #%d", evt);
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        ESP_LOGI(__func__, "Event -> ETH disconnected #%d", evt);
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        ESP_LOGI(__func__, "Event -> ETH got ip #%d", evt);
        break;
    default:
        ESP_LOGI(__func__, "Event -> Unknow event  #%d", evt);
        break;
    }
}

wifi_mode_t WiFiMode::selectMode(bool AP, bool ST)
{
    if (!AP && ST) {
        return WIFI_STA;
    } else if (AP && !ST) {
        return WIFI_AP;
    } else if (AP && ST) {
        return WIFI_AP_STA;
    } else if (!AP && !ST) {
        return WIFI_MODE_NULL;
    } else {
        return WIFI_STA;
    }    

}

esp_err_t WiFiMode::begin()
{
    ESP_LOGI(__func__,"initiating wlan**********************************");

    WAP* wap =  new WAP(wifi_config_->apSSID, 
                        wifi_config_->apPassword, 
                        wifi_config_->apChannel, 
                        wifi_config_->apMaxConn);

    WST* wst = new WST(wifi_config_->apSSID, wifi_config_->apPassword);

    wifi_mode_t WIFI_MODE = selectMode(WAP_enabled, WST_enabled);
    ESP_LOGI(__func__, "Starting WiFi mode: %d", WIFI_MODE);

    WiFi.onEvent(WiFiEvent);
    
    switch (WIFI_MODE)
    {
    case WIFI_STA:
        ESP_LOGI(__func__, "Starting WST iface only");
        return wst->begin();
        break;
    case WIFI_AP:
        ESP_LOGI(__func__, "Starting WAP iface only");
        return wap->begin();
        break;
    case WIFI_AP_STA:
        ESP_LOGI(__func__, "Starting WAP and WST ifaces");
        esp_err_t WAP_isInit; 
        esp_err_t WST_isInit;
        WAP_isInit = wap->begin();
        WST_isInit = wst->begin();
        if (WAP_isInit == ESP_OK && WST_isInit == ESP_OK)
        {
            return ESP_OK;
        }
        else
        {
            return ESP_FAIL;
        }
        break;
    case WIFI_MODE_NULL:
        ESP_LOGI(__func__, "WAP and WST ifaces are disabled");
        return ESP_OK;
        break;
    default:
        ESP_LOGI(__func__, "Unknow error starting WiFi");
        return ESP_FAIL;
        break;
    }
}