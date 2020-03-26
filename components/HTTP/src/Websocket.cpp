/**
 * @file Socket.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-03-25
 * 
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 * 
 */

#include "Websocket.h"
#include <esp_log.h>
#include <iostream>

static const char* TAG = "Websocket";

Websocket::Websocket()
{
    ESP_LOGI(TAG, "Execute contructor websocket");
}

void Websocket::onReceive(httpd_ws_frame_t ws_pkt)
{
    char* type = parseJson(ws_pkt.payload);

    switch (type) {
    case 'handshake':
        /* code  here*/
        break;
    case 'status':
        /* code  here*/
    case 'action':
        /* code  here*/
    default:
        break;
    }
}


char* Websocket::parseJson(uint8_t* payload)
{
    char* res = reinterpret_cast<char*>(payload);
    cJSON* req_root = cJSON_Parse(res);
    cJSON* type = cJSON_GetObjectItemCaseSensitive(req_root, "type");

    return cJSON_GetStringValue(type);
}
