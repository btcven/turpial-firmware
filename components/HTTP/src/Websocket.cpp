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

        std::cout << client.fd << std::endl;
        if (m_client.size() != 0) {
            for (size_t i = 0; i < m_client.size(); i++) {
                if (m_client[i].shaUID == client.shaUID) {
                    m_client[i].req = client.req;
                    m_client[i].fd = client.fd;
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
        for (size_t i = 0; i < m_client.size(); i++) {
            // err = trigger_async_send(req->handle, m_client[i].fd);
            // if (err != ESP_OK) {
            //     ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", err);
            // };

            // normal response method
            err = httpd_ws_send_frame(m_client[i].req, &ws_pkt);
            std::cout <<httpd_req_to_sockfd(m_client[i].req)   << std::endl;
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", err);
            };
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
        client->fd = httpd_req_to_sockfd(req);
        return ESP_OK;
    };

    return ESP_FAIL;
}


void ws_async_send(void* arg)
{
    static const char* data = "Async data";
    struct async_resp_arg* resp_arg = (struct async_resp_arg*)arg;
    httpd_handle_t hd = resp_arg->hd;
    int fd = resp_arg->fd;
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t*)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

    std::cout << ws_pkt.payload << std::endl;

    httpd_ws_send_frame_async(hd, fd, &ws_pkt);
    free(resp_arg);
}

esp_err_t Websocket::trigger_async_send(httpd_handle_t handle, int fd)
{
    struct async_resp_arg* resp_arg = (struct async_resp_arg*)malloc(sizeof(struct async_resp_arg));
    resp_arg->hd = handle;
    resp_arg->fd = fd;
    return httpd_queue_work(handle, ws_async_send, resp_arg);
}
