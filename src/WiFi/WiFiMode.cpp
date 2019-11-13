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

namespace wifi {

namespace mode {

OperationMode selectOperationMode(bool ap, bool st) {
    if (!ap && st) {
        return OperationMode::Ap;
    } else if (ap && !st) {
        return OperationMode::St;
    } else if (ap && st) {
        return OperationMode::ApSt;
    } else if (!ap && !st) {
        return OperationMode::None;
    } else {
        // unreachable condition
        return OperationMode::St;
    }
}

void handleWiFiEvent(WiFiEvent_t evt)
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

esp_err_t begin(DTOConfig config)
{
    ESP_LOGI(__func__,"WiFi Mode begin");

    wap::Config wap_config = {
        .apSSID = config.apSSID.c_str(),
        .apPass = config.apPassword.c_str(), 
        .apChannel = config.apChannel,
        .apMaxConn = config.apMaxConn,
    };

    wst::Config wst_config = {
        .ssid = config.apSSID.c_str(),
        .pass = config.apPassword.c_str(),
    };

    auto isAp = config.WAP_enabled;
    auto isSt = config.WST_enabled;
    auto op_mode = selectOperationMode(isAp, isSt);
    
    ESP_LOGI(__func__, "Starting WiFi mode: %d", static_cast<int>(op_mode));

    WiFi.onEvent(handleWiFiEvent);
    
    switch (op_mode) {
    case OperationMode::St:
        ESP_LOGI(__func__, "Starting WST iface only");
        return wst::begin(wst_config);
        break;
    case OperationMode::Ap:
        ESP_LOGI(__func__, "Starting WAP iface only");
        return wap::begin(wap_config);
        break;
    case OperationMode::ApSt:
        {
            ESP_LOGI(__func__, "Starting WAP and WST ifaces");
            auto WAP_isInit = wap::begin(wap_config);
            auto WST_isInit = wst::begin(wst_config);
            if (WAP_isInit == ESP_OK && WST_isInit == ESP_OK)
            {
                return ESP_OK;
            }
            else
            {
                return ESP_FAIL;
            }
        }
        break;
    case OperationMode::None:
        ESP_LOGI(__func__, "WAP and WST ifaces are disabled");
        return ESP_OK;
        break;
    default:
        ESP_LOGI(__func__, "Unknow error starting WiFi");
        return ESP_FAIL;
        break;
    }
}

} // namespace mode

} // namespace wifi