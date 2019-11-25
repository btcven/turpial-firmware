
#ifndef WIFI_HANDLE_EVENT_H_
#define WIFI_HANDLE_EVENT_H_

#include "esp_event.h"
#include "esp_log.h"
#include "wifi_event_source.h"
#include <cstdio>

namespace wifi {
class WiFiHandleEvents
{
public:
    WiFiHandleEvents();
    ~WiFiHandleEvents();
    char* get_id_string(esp_event_base_t base, int32_t id);
    void on(int32_t eventID, esp_event_handler_t handler);

private:
};

} // namespace wifi
#endif