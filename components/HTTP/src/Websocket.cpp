/**
 * @file Websocket.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-03-25
 * 
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 * 
 */

#include "Websocket.h"
#include "CheckConnections.h"
#include <esp_log.h>
#include <iostream>
static const char* TAG = "Websocket";


Websocket::Websocket()
{
    static CheckConnections g_check_connections;
    g_check_connections.start(NULL);
}

void Websocket::onReceive(httpd_ws_frame_t ws_pkt, httpd_req_t* req)
{
    req_handler = req;
    int type = getTypeMessage(ws_pkt.payload);
    WsMsgType r = static_cast<WsMsgType>(type);
    client_data_t client;
    esp_err_t err;
    uid_message_t client_uid;
    const char* null_to_uid = "ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";
    bool update = false;

    switch (r) {
    case WsMsgType::Handshake:
        ESP_LOGI(TAG, "!!!!!hanshake");
        err = getClientData(ws_pkt.payload, &client);
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
    case WsMsgType::Message:
        ESP_LOGI(TAG, "!!!!!msg");

        err = messageRecipient(ws_pkt.payload, &client_uid, null_to_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting message uids %d", err);
            return;
        };

        err = sendWsData(client_uid, ws_pkt, null_to_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error sending message %d", err);
            return;
        };

        break;
    case WsMsgType::Status:
        ESP_LOGI(TAG, "!!!!!status");

        err = messageRecipient(ws_pkt.payload, &client_uid, null_to_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting message uids %d", err);
            return;
        };

        err = sendWsData(client_uid, ws_pkt, null_to_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error sending message %d", err);
            return;
        };

        break;
    case WsMsgType::Action:
        ESP_LOGI(TAG, "!!!!!action");

        err = messageRecipient(ws_pkt.payload, &client_uid, null_to_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting message uids %d", err);
            return;
        };

        err = sendWsData(client_uid, ws_pkt, null_to_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Sending message: %s", esp_err_to_name(err));
            return;
        };

        break;
    default:
        ESP_LOGE(TAG, "Unknow message type");
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


esp_err_t Websocket::getClientData(uint8_t* payload, client_data_t* client)
{
    char* res = reinterpret_cast<char*>(payload);
    cJSON* req_root = cJSON_Parse(res);
    cJSON* cjson_timestamp = cJSON_GetObjectItemCaseSensitive(req_root, "timestamp");
    cJSON* cjson_shaUID = cJSON_GetObjectItemCaseSensitive(req_root, "shaUID");

    const char* sha_uid = cjson_shaUID->valuestring;
    int timestamp = cjson_timestamp->valueint;
    if (cJSON_IsNumber(cjson_timestamp) && cJSON_IsString(cjson_shaUID)) {
        if (strlen(sha_uid) != 64) {
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }

        util::hexToBytes(sha_uid, client->shaUID);

        client->timestamp = timestamp;
        client->is_alive = true;
        client->fd = httpd_req_to_sockfd(req_handler);
        cJSON_Delete(req_root);
        return ESP_OK;
    };
     cJSON_Delete(req_root);
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
            cJSON_Delete(req_root);
            ESP_LOGI(TAG, "toUID length  is less than 64 ");
            return ESP_ERR_INVALID_SIZE;
        };

        util::hexToBytes(to_uid, uid_receiving->to_uid);
    }

    if (cJSON_IsInvalid(cjson_fromUID) || cJSON_IsNull(cjson_fromUID)) {
         cJSON_Delete(req_root);
        return ESP_FAIL;
    } else {
        if (strlen(from_uid) != 64) {
            cJSON_Delete(req_root);
            ESP_LOGI(TAG, "  FromUID length  is less than 64 ");
            return ESP_ERR_INVALID_SIZE;
        };
        util::hexToBytes(from_uid, uid_receiving->from_uid);
    }
     cJSON_Delete(req_root);
    return ESP_OK;
}


esp_err_t Websocket::sendWsData(uid_message_t client_uid, httpd_ws_frame_t ws_pkt, const char* null_to_uid)
{
    esp_err_t err;
    std::uint8_t compare_uid[32];

    if (m_client.size() == 0) {
        ESP_LOGE(TAG, "No clients connected");
        return ESP_FAIL;
    }

    err = util::hexToBytes(null_to_uid, compare_uid);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error getting uid %d", err);
        return ESP_FAIL;
    };

    if (sizeof(client_uid.to_uid) != 32) {
        ESP_LOGE(TAG, "toUID does not have the correct length");
        return ESP_FAIL;
    }

    if (sizeof(client_uid.from_uid) != 32) {
        ESP_LOGE(TAG, "fromUID does not have the correct length");
        return ESP_FAIL;
    }

    for (size_t i = 0; i < m_client.size(); i++) {
        if (memcmp(client_uid.to_uid, compare_uid, sizeof(compare_uid)) == 0) {
            if (memcmp(client_uid.from_uid, m_client[i].shaUID, sizeof(m_client[i].shaUID)) != 0) {
                err = httpd_ws_send_frame_async(req_handler->handle, m_client[i].fd, &ws_pkt);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "httpd_ws_send_frame_async failed with %d", err);
                    return ESP_FAIL;
                };
            }
        } else if (memcmp(client_uid.to_uid, m_client[i].shaUID, sizeof(m_client[i].shaUID)) == 0) {
            err = httpd_ws_send_frame_async(req_handler->handle, m_client[i].fd, &ws_pkt);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "httpd_ws_send_frame_async failed with %d", err);
                return ESP_FAIL;
            };
        }
    }

    return ESP_OK;
}


void Websocket::pong(httpd_req_t* req)
{
    ESP_LOGI(TAG, "SENDING PONG ");

    if (m_client.size() != 0) {
        return;
    }

    static const char* data = "Async data";
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (uint8_t*)data;
    ws_pkt.type = HTTPD_WS_TYPE_PONG;
    ws_pkt.len = strlen(data);
    ws_pkt.final = 1;

    for (size_t i = 0; i < m_client.size(); i++) {
        ESP_LOGI(TAG, "Packet type: %d", m_client[i].fd);

        esp_err_t err = httpd_ws_send_frame_async(req->handle, m_client[i].fd, &ws_pkt);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "httpd_ws_send_frame_async failed with %d", err);
            // remove client not connected

            m_client.erase(m_client.begin() + i);
        }
    }
}

void Websocket::checkConnection()
{
    if (m_client.size() != 0) {
        Websocket::pong(req_handler);
    }
}