#ifndef WIFI_EVENT_SOURCE_H_
#define WIFI_EVENT_SOURCE_H_

#include "esp_timer.h"

#include "esp_err.h"
#include "esp_event.h"
#include "esp_wifi.h"


#ifdef __cplusplus
extern "C" {
#endif

ESP_EVENT_DECLARE_BASE(WIFI_EVENTS); // declaration of the task events family

enum {
    // raised during internal wifi events state machine, we need to rise up manually
    STA_AUTHMODE_CHANGE_EVENT,
    STA_CONNECTED_EVENT,
    STA_DISCONNECTED_EVENT,
    STA_GOT_IP_EVENT,
    STA_START_EVENT,
    STA_STOP_EVENT,
    WIFI_READY_EVENT,
};
#ifdef __cplusplus
}
#endif

#endif