#include "WiFiEventHandler.h"
#include "sdkconfig.h"
#include <esp_err.h>
#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#include <esp_wifi.h>

static const char* TAG = "WiFiEventHandler";


namespace network {

WiFiEventHandler::WiFiEventHandler()
    : m_next_handler(nullptr)
{
}

esp_err_t WiFiEventHandler::staScanDone(system_event_sta_scan_done_t info)
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

esp_err_t WiFiEventHandler::staConnected(system_event_sta_connected_t info)
{
    ESP_LOGD(TAG, "default staConnected");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::staDisconnected(system_event_sta_disconnected_t info)
{
    ESP_LOGD(TAG, "default staDisconnected");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::staAuthChange(system_event_sta_authmode_change_t info)
{
    ESP_LOGD(TAG, "default staAuthChange");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::staGotIp(system_event_sta_got_ip_t info)
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
esp_err_t WiFiEventHandler::apStaConnected(system_event_ap_staconnected_t info)
{
    ESP_LOGD(TAG, "default apStaConnected");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::apStaDisconnected(system_event_ap_stadisconnected_t info)
{
    ESP_LOGD(TAG, "default apStaDisconnected");
    return ESP_OK;
}

esp_err_t WiFiEventHandler::eventDispatcher(system_event_t* event)
{
    ESP_LOGD(TAG, "eventHandler called, event = 0x%x", event->event_id);

    switch (event->event_id) {
    case SYSTEM_EVENT_SCAN_DONE: {
        return staScanDone(event->event_info.scan_done);
        break;
    }
    case SYSTEM_EVENT_STA_START: {
        return staStart();
        break;
    }
    case SYSTEM_EVENT_STA_STOP: {
        return staStop();
        break;
    }
    case SYSTEM_EVENT_STA_CONNECTED: {
        return staConnected(event->event_info.connected);
        break;
    }
    case SYSTEM_EVENT_STA_DISCONNECTED: {
        return staDisconnected(event->event_info.disconnected);
        break;
    }
    case SYSTEM_EVENT_STA_AUTHMODE_CHANGE: {
        return staAuthChange(event->event_info.auth_change);
        break;
    }
    case SYSTEM_EVENT_STA_GOT_IP: {
        return staGotIp(event->event_info.got_ip);
        break;
    }
    case SYSTEM_EVENT_AP_START: {
        return apStart();
        break;
    }
    case SYSTEM_EVENT_AP_STOP: {
        return apStop();
        break;
    }
    case SYSTEM_EVENT_AP_STACONNECTED: {
        return apStaConnected(event->event_info.sta_connected);
        break;
    }
    case SYSTEM_EVENT_AP_STADISCONNECTED: {
        return apStaDisconnected(event->event_info.sta_disconnected);
        break;
    }
    default:
        ESP_LOGD(TAG, "unknown event = %d", event->event_id);
        break;
    }

    if (m_next_handler) {
        ESP_LOGD(TAG, "Dispatching event to the next_handler");
        return m_next_handler->eventDispatcher(event);
    }

    return ESP_OK;
}

} // namespace network