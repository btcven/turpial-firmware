
#ifndef WIFI_HANDLE_EVENT_H_
#define WIFI_HANDLE_EVENT_H_

#include <cstdio>
#include "esp_event.h"
#include "wifi_event_source.h"
#include "esp_log.h"

namespace wifi {
class WiFiHandleEvents {
    public: 
    WiFiHandleEvents();
    ~WiFiHandleEvents();
    char* get_id_string(esp_event_base_t base, int32_t id);
    void on(int32_t eventID, esp_event_handler_t handler);
    static void on();
    //static void sta_got_ip_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data);
    private:
        
};

}
#endif