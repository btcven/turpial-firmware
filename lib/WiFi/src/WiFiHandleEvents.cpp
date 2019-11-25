#include "WiFiHandleEvents.h"
namespace wifi {

static const char* TAG = "WiFiMHandleEvent";
WiFiHandleEvents::WiFiHandleEvents()
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
}

WiFiHandleEvents::~WiFiHandleEvents()
{
}

void WiFiHandleEvents::on(int32_t eventID, esp_event_handler_t handler)
{
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENTS, eventID, handler, NULL));
}

char* WiFiHandleEvents::get_id_string(esp_event_base_t base, int32_t id)
{
    char* event = "";
    if (base == WIFI_EVENTS) {
        switch (id) {
        case STA_AUTHMODE_CHANGE_EVENT:
            event = "SYSTEM_EVENT_STA_AUTHMODE_CHANGE";
            break;
        case STA_CONNECTED_EVENT:
            event = "SYSTEM_EVENT_STA_CONNECTED";
            break;
        case STA_DISCONNECTED_EVENT:
            event = "SYSTEM_EVENT_STA_DISCONNECTED";
            break;
        case STA_GOT_IP_EVENT:
            event = "SYSTEM_EVENT_STA_GOT_IP";
            break;
        case STA_START_EVENT:
            event = "SYSTEM_EVENT_STA_START";
            break;
        case STA_STOP_EVENT:
            event = "SYSTEM_EVENT_STA_STOP";
            break;
        case WIFI_READY_EVENT:
            event = "SYSTEM_EVENT_WIFI_READY";
            break;
        }
    } else {
        event = "TASK_ITERATION_EVENT";
    }
    return event;
}


} // namespace wifi