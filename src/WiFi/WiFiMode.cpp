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
#include "defaults.h"
#include "NVS/SingletonNVS.h"
//#include "NVS/NVStorage.h"

void WiFiEvent(WiFiEvent_t evt)
{
    switch (evt)
    {
    case SYSTEM_EVENT_WIFI_READY:
        ESP_LOGD(__func__, "Event -> WiFi ready #%d", evt);
        break;
    case SYSTEM_EVENT_SCAN_DONE:
        ESP_LOGD(__func__, "Event -> Scann done #%d", evt);
        break;
    case SYSTEM_EVENT_STA_START:
        ESP_LOGD(__func__, "Event -> WST start #%d", evt);
        break;
    case SYSTEM_EVENT_STA_STOP:
        ESP_LOGD(__func__, "Event -> WST stop #%d", evt);
        break;
    case SYSTEM_EVENT_STA_CONNECTED:
        ESP_LOGD(__func__, "Event -> WST connected #%d", evt);
        break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
        ESP_LOGD(__func__, "Event -> WST disconnected #%d", evt);
        break;
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
        ESP_LOGD(__func__, "Event -> WST auth mode change #%d", evt);
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
        ESP_LOGD(__func__, "Event -> WST got IP #%d", evt);
        break;
    case SYSTEM_EVENT_STA_LOST_IP:
        ESP_LOGD(__func__, "Event -> WST lost IP #%d", evt);
        break;
    case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
        ESP_LOGD(__func__, "Event -> WST WPS er success #%d", evt);
        break;
    case SYSTEM_EVENT_STA_WPS_ER_FAILED:
        ESP_LOGD(__func__, "Event -> WST WPS er failed #%d", evt);
        break;
    case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
        ESP_LOGD(__func__, "Event -> WST er timeout #%d", evt);
        break;
    case SYSTEM_EVENT_STA_WPS_ER_PIN:
        ESP_LOGD(__func__, "Event -> WST er pin #%d", evt);
        break;
    case SYSTEM_EVENT_AP_START:
        ESP_LOGD(__func__, "Event -> AP start #%d", evt);
        break;
    case SYSTEM_EVENT_AP_STOP:
        ESP_LOGD(__func__, "Event -> AP stop #%d", evt);
        break;
    case SYSTEM_EVENT_AP_STACONNECTED:
        ESP_LOGD(__func__, "Event -> Client connected #%d", evt);
        break;
    case SYSTEM_EVENT_AP_STADISCONNECTED:
        ESP_LOGD(__func__, "Event -> Client disconnected #%d", evt);
        break;
    case SYSTEM_EVENT_AP_STAIPASSIGNED:
        ESP_LOGD(__func__, "Event -> IP assigned to client #%d", evt);
        break;
    case SYSTEM_EVENT_AP_PROBEREQRECVED:
        ESP_LOGD(__func__, "Event -> PROBEREQRECVED #%d", evt);
        break;
    case SYSTEM_EVENT_GOT_IP6:
        ESP_LOGD(__func__, "Event -> got ipv6 #%d", evt);
        break;
    case SYSTEM_EVENT_ETH_START:
        ESP_LOGD(__func__, "Event -> ETH start #%d", evt);
        break;
    case SYSTEM_EVENT_ETH_STOP:
        ESP_LOGD(__func__, "Event -> ETH stop #%d", evt);
        break;
    case SYSTEM_EVENT_ETH_CONNECTED:
        ESP_LOGD(__func__, "Event -> ETH connected #%d", evt);
        break;
    case SYSTEM_EVENT_ETH_DISCONNECTED:
        ESP_LOGD(__func__, "Event -> ETH disconnected #%d", evt);
        break;
    case SYSTEM_EVENT_ETH_GOT_IP:
        ESP_LOGD(__func__, "Event -> ETH got ip #%d", evt);
        break;
    default:
        ESP_LOGD(__func__, "Event -> Unknow event  #%d", evt);
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
    //
    // only in dev stage:
    // clear nvs before start.
    //
    // nvs_clear("WAP");
    // nvs_clear("WST");

    // Declare and define default values, before reading NVS
    bool WAP_enabled = WAP_ENABLED; // Default value
    bool WST_enabled = WST_ENABLED; // Default value

    // Creating instances of NVStorage, WAP and WST classes
    SingletonNVS* nvs = SingletonNVS::getInstance();
    WAP wap;
    WST wst;
    // Initialize the NVS flash storage
    //nvs.begin();

    // Open nvs
    bool isOpen = nvs->open(NVS_WIFI_NAMESPACE, false);
    // Verifying if opened
    if (isOpen)
    {
        // This is just for testing purposes
        //=========================================================
        // Change the Wi-Fi mode permanently here...
        //nvs.setBool(NVS_WAP_ENABLED_KEY, 1);
        //nvs.setBool(NVS_WST_ENABLED_KEY, 1);
        //=========================================================

        WAP_enabled = nvs->getBool(NVS_WAP_ENABLED_KEY, WAP_ENABLED);
        ESP_LOGD(__func__, "WAP iface enabled: %d", WAP_enabled);

        WST_enabled = nvs->getBool(NVS_WST_ENABLED_KEY, WST_ENABLED);
        ESP_LOGD(__func__, "WST iface enabled: %d", WST_enabled);
    }
    else
    {
        ESP_LOGE(__func__, "Error opening the NVS");
    }

    wifi_mode_t WIFI_MODE = selectMode(WAP_enabled, WST_enabled);
    ESP_LOGD(__func__, "Starting WiFi mode: %d", WIFI_MODE);

    WiFi.onEvent(WiFiEvent);
    
    switch (WIFI_MODE)
    {
    case WIFI_STA:
        ESP_LOGD(__func__, "Starting WST iface only");
        return wst.begin();
        break;
    case WIFI_AP:
        ESP_LOGD(__func__, "Starting WAP iface only");
        return wap.begin();
        break;
    case WIFI_AP_STA:
        ESP_LOGD(__func__, "Starting WAP and WST ifaces");
        esp_err_t WAP_isInit, WST_isInit;
        WAP_isInit = wap.begin();
        WST_isInit = wst.begin();
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
        ESP_LOGD(__func__, "WAP and WST ifaces are disabled");
        return ESP_OK;
        break;
    default:
        ESP_LOGE(__func__, "Unknow error starting WiFi");
        return ESP_FAIL;
        break;
    }
}