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

    std::cout << "############### EL TYPE: " << type << std::endl;

    WebsocketType r = static_cast<WebsocketType>(type);
    client_data_t client;
    esp_err_t err;
    uid_message_t client_uid;

    const char* null_to_uid = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
    std::uint8_t compare_uid[32];
    bool update = false;

    switch (r) {
    case WebsocketType::handshake:
        ESP_LOGI(TAG, "!!!!!hanshake");
        err = getClientData(ws_pkt.payload, &client, req);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "type must be numeric 0 ");
        };


        if (m_client.size() > 0) {
            int size = m_client.size();
            for (size_t i = 0; i < size; i++) {
                if (memcmp(m_client[i].shaUID, client.shaUID, sizeof(client.shaUID)) == 0) {
                    m_client[i].fd = client.fd;
                    update = true;
                }
            }
            if (update == false) {
                m_client.push_back(client);
            }

            update = false;
        } else {
            m_client.push_back(client);
        }

        std::cout << "vector: " << m_client.size() << std::endl;
        break;
    case WebsocketType::msg:
        ESP_LOGI(TAG, "!!!!!msg");
        err = messageRecipient(ws_pkt.payload, &client_uid, null_to_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting message uids %d", err);
        };


        err = util::hexToBytes(null_to_uid, compare_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting uid %d", err);
        };

        for (size_t i = 0; i < m_client.size(); i++) {
            if (memcmp(client_uid.to_uid, compare_uid, sizeof(compare_uid)) == 0) {
                if (memcmp(client_uid.from_uid, m_client[i].shaUID, sizeof(m_client[i].shaUID)) != 0) {
                    err = httpd_ws_send_frame_async(req->handle, m_client[i].fd, &ws_pkt);
                    if (err != ESP_OK) {
                        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", err);
                    };
                }
            } else if (memcmp(client_uid.to_uid, m_client[i].shaUID, sizeof(m_client[i].shaUID)) == 0) {
                err = httpd_ws_send_frame_async(req->handle, m_client[i].fd, &ws_pkt);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", err);
                };
            }
        }
        break;
    case WebsocketType::status:
        ESP_LOGI(TAG, "!!!!!status");
        err = messageRecipient(ws_pkt.payload, &client_uid, null_to_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting message uids %d", err);
        };


        err = util::hexToBytes(null_to_uid, compare_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting uid %d", err);
        };

        for (size_t i = 0; i < m_client.size(); i++) {
            if (memcmp(client_uid.to_uid, compare_uid, sizeof(compare_uid)) == 0) {
                if (memcmp(client_uid.from_uid, m_client[i].shaUID, sizeof(m_client[i].shaUID)) != 0) {
                    err = httpd_ws_send_frame_async(req->handle, m_client[i].fd, &ws_pkt);
                    if (err != ESP_OK) {
                        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", err);
                    };
                }
            } else if (memcmp(client_uid.to_uid, m_client[i].shaUID, sizeof(m_client[i].shaUID)) == 0) {
                err = httpd_ws_send_frame_async(req->handle, m_client[i].fd, &ws_pkt);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", err);
                };
            }
        }
        break;
    case WebsocketType::action:
        ESP_LOGI(TAG, "!!!!!action");
        err = messageRecipient(ws_pkt.payload, &client_uid, null_to_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting message uids %d", err);
        };


        err = util::hexToBytes(null_to_uid, compare_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting uid %d", err);
        };

        for (size_t i = 0; i < m_client.size(); i++) {
            if (memcmp(client_uid.to_uid, compare_uid, sizeof(compare_uid)) == 0) {
                if (memcmp(client_uid.from_uid, m_client[i].shaUID, sizeof(m_client[i].shaUID)) != 0) {
                    err = httpd_ws_send_frame_async(req->handle, m_client[i].fd, &ws_pkt);
                    if (err != ESP_OK) {
                        ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", err);
                    };
                }
            } else if (memcmp(client_uid.to_uid, m_client[i].shaUID, sizeof(m_client[i].shaUID)) == 0) {
                err = httpd_ws_send_frame_async(req->handle, m_client[i].fd, &ws_pkt);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "httpd_ws_send_frame failed with %d", err);
                };
            }
        }
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

    const char* sha_uid = cjson_shaUID->valuestring;
    int timestamp = cjson_timestamp->valueint;
    if (cJSON_IsNumber(cjson_timestamp) && cJSON_IsString(cjson_shaUID)) {
        if (strlen(sha_uid) != 64) {
            return ESP_FAIL;
        }

        util::hexToBytes(sha_uid, client->shaUID);

        client->timestamp = timestamp;
        client->is_alive = true;
        client->fd = httpd_req_to_sockfd(req);
        return ESP_OK;
    };

    return ESP_FAIL;
}


void ws_async_send(void* arg)
{
    static const char* data = "Async data";
    struct async_resp_arg_t* resp_arg = (struct async_resp_arg_t*)arg;
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
    struct async_resp_arg_t* resp_arg = (struct async_resp_arg_t*)malloc(sizeof(struct async_resp_arg_t));
    resp_arg->hd = handle;
    resp_arg->fd = fd;
    return httpd_queue_work(handle, ws_async_send, resp_arg);
}

esp_err_t Websocket::messageRecipient(uint8_t* payload, uid_message_t* uid_receiving, const char* null_to_uid)
{
    char* res = reinterpret_cast<char*>(payload);
    cJSON* req_root = cJSON_Parse(res);
    cJSON* cjson_fromUID = cJSON_GetObjectItemCaseSensitive(req_root, "fromUID");
    cJSON* cjson_toUID = cJSON_GetObjectItemCaseSensitive(req_root, "toUID");

    const char* from_uid = cjson_fromUID->valuestring;
    const char* to_uid = cjson_toUID->valuestring;

    if (cJSON_IsNull(cjson_toUID)) {
        util::hexToBytes(null_to_uid, uid_receiving->to_uid);

    } else if (cJSON_IsString(cjson_toUID)) {
        if (strlen(to_uid) != 64) {
            ESP_LOGI(TAG, "toUID length  is less than 64 ");
            return ESP_FAIL;
        };

        ESP_LOGI(TAG, "ANTES DE SETEAR EL DATO");
        std::cout << "entro aqui" << to_uid << std::endl;

        util::hexToBytes(to_uid, uid_receiving->to_uid);
    }

    if (cJSON_IsInvalid(cjson_fromUID) || cJSON_IsNull(cjson_fromUID)) {
        return ESP_FAIL;
    } else {
        if (strlen(from_uid) != 64) {
            ESP_LOGI(TAG, "  FromUID length  is less than 64 ");
            return ESP_FAIL;
        };
        util::hexToBytes(from_uid, uid_receiving->from_uid);
    }

    return ESP_OK;
}
