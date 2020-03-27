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

void Websocket::onReceive(httpd_ws_frame_t ws_pkt, httpd_req_t* req)
{
    int type = getTypeMessage(ws_pkt.payload);

    WebsocketType r = static_cast<WebsocketType>(type);
    client_data_t client;
    esp_err_t err;

    switch (r) {
    case WebsocketType::handshake:
        err = getClientData(ws_pkt.payload, &client, req);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "type must be numeric");
        };
        if (m_client.size() != 0) {
            for (size_t i = 0; i < m_client.size(); i++) {
                std::cout << m_client[i].shaUID << std::endl;
                if (m_client[i].shaUID == client.shaUID) {
                    m_client[i].req = client.req;
                } else {
                    m_client.push_back(client);
                }
            }
        } else {
            m_client.push_back(client);
        }

        break;
    case WebsocketType::msg:
        ESP_LOGI(TAG, "!!!!!msg");
        err = httpd_ws_send_frame(m_client[1].req, &ws_pkt);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", err);
        }
        break;
    case WebsocketType::status:
        ESP_LOGI(TAG, "!!!!!status");
        break;
    case WebsocketType::action:
        ESP_LOGI(TAG, "!!!!!action");
        break;
    default:
        break;
    }
}

int Websocket::getTypeMessage(uint8_t* payload)
{
    char* res = reinterpret_cast<char*>(payload);
    cJSON* req_root = cJSON_Parse(res);
    cJSON* type = cJSON_GetObjectItemCaseSensitive(req_root, "type");
    if (cJSON_IsNumber(type)) {
        return type->valueint;
    } else {
        ESP_LOGE(TAG, "type must be numeric");
        return -1;
    }
}


esp_err_t Websocket::getClientData(uint8_t* payload, client_data_t* client, httpd_req_t* req)
{
    char* res = reinterpret_cast<char*>(payload);
    cJSON* req_root = cJSON_Parse(res);
    cJSON* cjson_timestamp = cJSON_GetObjectItemCaseSensitive(req_root, "timestamp");
    cJSON* cjson_shaUID = cJSON_GetObjectItemCaseSensitive(req_root, "shaUID");

    char* sha_uid = cjson_shaUID->valuestring;
    int timestamp = cjson_timestamp->valueint;
    if (cJSON_IsNumber(cjson_timestamp) && cJSON_IsString(cjson_shaUID)) {
        client->shaUID = sha_uid;
        client->timestamp = timestamp;
        client->is_alive = true;
        client->req = req;
        return ESP_OK;
    };

    return ESP_FAIL;
}
