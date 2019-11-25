/**
 * @file main.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-25
 * 
 * @copyright Copyright (c) 2019
 * 
 */


#ifndef WIFI_HANDLE_EVENT_H_
#define WIFI_HANDLE_EVENT_H_

#include "esp_event.h"
#include "esp_log.h"
#include "wifi_event_source.h"
#include <cstdio>

namespace wifi {

/**
 * @brief 
 * this Class is intended to be inherited from main Wifi class,
 * also any oblect from this class is able to work as independient object,
 * listening custom  wifi events launched by the programmer
 */
class WiFiHandleEvents
{
public:
    /**
     * @brief Construct a new Wi Fi Handle Events object
     * 
     */
    WiFiHandleEvents();
    /**
     * @brief Destroy the Wi Fi Handle Events object
     * 
     */
    ~WiFiHandleEvents();
    /**
     * @brief Get the id string object representing the actual event
     * 
     * @param base 
     * @param id 
     * @return char* 
     */
    char* get_id_string(esp_event_base_t base, int32_t id);
    /**
     * @brief Subscribe to whatever wifi event
     * 
     * @param eventID 
     * @param handler 
     */
    void on(int32_t eventID, esp_event_handler_t handler);
};

} // namespace wifi
#endif