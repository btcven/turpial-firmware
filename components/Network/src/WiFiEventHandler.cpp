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
 * @}
 */

#include "Network/WiFiEventHandler.h"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_wifi.h>

#include <Vaina/Vaina.h>

namespace network {

static const char* TAG = "WiFiEventHandler";

WiFiEventHandler::WiFiEventHandler()
    : m_next_handler(nullptr)
{
}

esp_err_t WiFiEventHandler::staScanDone(wifi_event_sta_scan_done_t* info)
{
    ESP_LOGD(TAG, "default staScanDone");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::staStart()
{
    ESP_LOGD(TAG, "default staStart");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::staStop()
{
    ESP_LOGD(TAG, "default staStop");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::staConnected(wifi_event_sta_connected_t* info)
{
    ESP_LOGI(TAG, "default staConnected");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::staDisconnected(wifi_event_sta_disconnected_t* info)
{
    ESP_LOGI(TAG, "default staDisconnected");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::staAuthChange(wifi_event_sta_authmode_change_t* info)
{
    ESP_LOGD(TAG, "default staAuthChange");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::staGotIp(ip_event_got_ip_t* info)
{
    ESP_LOGD(TAG, "default staGotIp");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::apStart()
{
    ESP_LOGD(TAG, "default apStart");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::apStop()
{
    ESP_LOGD(TAG, "default apStop");
    return ESP_OK;
}
esp_err_t WiFiEventHandler::apStaConnected(wifi_event_ap_staconnected_t* info)
{
    ESP_LOGI(TAG, "default apStaConnected");


    vaina::Vaina& vaina = vaina::Vaina::getInstance();

    vaina.staConnected(info);
    return ESP_OK;
}

esp_err_t WiFiEventHandler::apStaDisconnected(wifi_event_ap_stadisconnected_t* info)
{
    ESP_LOGI(TAG, "default apStaDisconnected");

    vaina::Vaina& vaina = vaina::Vaina::getInstance();

    vaina.staDisconected(info);
    return ESP_OK;
}

esp_err_t WiFiEventHandler::eventDispatcher(std::int32_t event_id,
    void* event_data)
{
    ESP_LOGI(TAG, "eventHandler called, event = 0x%x", event_id);

    // Call the necessary event handler
    switch (event_id) {
    case WIFI_EVENT_SCAN_DONE:
        return staScanDone(reinterpret_cast<wifi_event_sta_scan_done_t*>(event_data));

    case WIFI_EVENT_STA_START:
        return staStart();

    case WIFI_EVENT_STA_STOP:
        return staStop();

    case WIFI_EVENT_STA_CONNECTED:
        return staConnected(reinterpret_cast<wifi_event_sta_connected_t*>(event_data));

    case WIFI_EVENT_STA_DISCONNECTED:
        return staDisconnected(reinterpret_cast<wifi_event_sta_disconnected_t*>(event_data));

    case WIFI_EVENT_STA_AUTHMODE_CHANGE:
        return staAuthChange(reinterpret_cast<wifi_event_sta_authmode_change_t*>(event_data));

    case IP_EVENT_STA_GOT_IP:
        return staGotIp(reinterpret_cast<ip_event_got_ip_t*>(event_data));

    case WIFI_EVENT_AP_START:
        return apStart();

    case WIFI_EVENT_AP_STOP:
        return apStop();

    case WIFI_EVENT_AP_STACONNECTED:
        return apStaConnected(reinterpret_cast<wifi_event_ap_staconnected_t*>(event_data));

    case WIFI_EVENT_AP_STADISCONNECTED:
        return apStaDisconnected(reinterpret_cast<wifi_event_ap_stadisconnected_t*>(event_data));

    default:
        ESP_LOGD(TAG, "Unknown event = %d", event_id);
        break;
    }

    if (m_next_handler) {
        ESP_LOGD(TAG, "Dispatching event to the next_handler");
        return m_next_handler->eventDispatcher(event_id, event_data);
    }

    return ESP_OK;
}

} // namespace network
