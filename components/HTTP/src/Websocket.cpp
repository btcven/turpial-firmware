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
#include <cstdlib>
#include <cstring>

#include <cbor.h>

static const char* TAG = "Websocket";

chat_id_t chat_id_unspecified = { 0xff, 0xff, 0xff, 0xff,
                                  0xff, 0xff, 0xff, 0xff,
                                  0xff, 0xff, 0xff, 0xff,
                                  0xff, 0xff, 0xff, 0xff,
                                  0xff, 0xff, 0xff, 0xff,
                                  0xff, 0xff, 0xff, 0xff,
                                  0xff, 0xff, 0xff, 0xff,
                                  0xff, 0xff, 0xff, 0xff };

static esp_err_t _json_parse_hex(cJSON* root, std::uint8_t *buf,
                                 std::size_t max_size)
{
    if (!cJSON_IsString(root)) {
        ESP_LOGE(TAG, "Field is not a string\n");
        return ESP_FAIL;
    } else {
        char *from_uid_str = cJSON_GetStringValue(root);
        if (strlen(from_uid_str) > (max_size * 2)) {
            ESP_LOGE(TAG, "Invalid fromUID size\n");
            return ESP_FAIL;
        }
        util::hexToBytes(cJSON_GetStringValue(root), buf);
    }

    return ESP_OK;
}

void websocketRadioRx(const std::uint8_t* buffer, std::size_t length)
{
    /*
    char payload[500];
    esp_err_t err;
    err = util::decode((std::uint8_t*)buffer, payload);
    if (err) {
        ESP_LOGE("TEST", "error decoding the value");
    }

    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (std::uint8_t*)payload;
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    ws_pkt.len = strlen(payload);
    ws_pkt.final = 1;
    */
    // send message to customers
}

Websocket::Websocket()
{
    static CheckConnections g_check_connections;
    g_check_connections.start(NULL);
}

void Websocket::onReceive(httpd_ws_frame_t ws_pkt, httpd_req_t* req)
{
    req_handler = req;
    Websocket::checkMessageType(ws_pkt, false);
}


void Websocket::checkMessageType(httpd_ws_frame_t ws_pkt, bool uart)
{
    int type = getTypeMessage(ws_pkt.payload);
    WsMsgType r = static_cast<WsMsgType>(type);
    client_data_t client;
    esp_err_t err;
    uid_message_t client_uid;
    bool update = false;

    switch (r) {
    case WsMsgType::Handshake:
        ESP_LOGI(TAG, "!!!!!Handshake");
        err = getClientData(ws_pkt.payload, &client);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "type must be numeric 0 ");
        };

        if (m_client.size() > 0) {
            int size = m_client.size();
            for (size_t i = 0; i < size; i++) {
                if (chat_id_equal(m_client[i].shaUID, client.shaUID)) {
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

        break;
    case WsMsgType::Message:
        ESP_LOGI(TAG, "!!!!!msg");

        err = messageRecipient(ws_pkt.payload, &client_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting message uids %s", esp_err_to_name(err));
            return;
        };

        err = sendWsData(client_uid, ws_pkt, uart);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error sending message %s", esp_err_to_name(err));
            return;
        };

        break;
    case WsMsgType::Status:
        ESP_LOGI(TAG, "!!!!!status");

        err = messageRecipient(ws_pkt.payload, &client_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting message uids %s", esp_err_to_name(err));
            return;
        };

        err = sendWsData(client_uid, ws_pkt, uart);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error sending message %s", esp_err_to_name(err));
            return;
        };

        break;
    case WsMsgType::Action:
        ESP_LOGI(TAG, "!!!!!action");

        err = messageRecipient(ws_pkt.payload, &client_uid);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error getting message uids %s", esp_err_to_name(err));
            return;
        };

        err = sendWsData(client_uid, ws_pkt, uart);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Sending message: %s", esp_err_to_name(err));
            return;
        };

        break;
    default:
        ESP_LOGE(TAG, "Unknown message type");

        break;
    }
}


int Websocket::getTypeMessage(std::uint8_t* payload)
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
    ws_pkt.payload = (std::uint8_t*)data;
    ws_pkt.len = strlen(data);
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;

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

esp_err_t Websocket::messageRecipient(std::uint8_t* payload, uid_message_t* uid_receiving)
{
    char* res = reinterpret_cast<char*>(payload);
    cJSON* req_root = cJSON_Parse(res);
    cJSON* cjson_fromUID = cJSON_GetObjectItemCaseSensitive(req_root, "fromUID");
    cJSON* cjson_toUID = cJSON_GetObjectItemCaseSensitive(req_root, "toUID");

    const char* from_uid = cjson_fromUID->valuestring;
    const char* to_uid = cjson_toUID->valuestring;

    if (cJSON_IsNull(cjson_toUID)) {
        /* Set ID to unspeficied */
        std::memcpy(uid_receiving->to_uid, chat_id_unspecified, sizeof(chat_id_t));
    } else if (cJSON_IsString(cjson_toUID)) {
        if (strlen(to_uid) != 64) {
            cJSON_Delete(req_root);
            ESP_LOGI(TAG, "toUID length  is less than 64 ");
            return ESP_ERR_INVALID_SIZE;
        };

        // Parse hex UID
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


esp_err_t Websocket::sendWsData(uid_message_t client_uid, httpd_ws_frame_t ws_pkt, bool uart)
{
    esp_err_t err;

    if (m_client.size() == 0) {
        ESP_LOGE(TAG, "No clients connected");
        return ESP_FAIL;
    }

    if (uart == false && chat_id_equal(client_uid.to_uid, chat_id_unspecified)) {
        err = sendUart(ws_pkt);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Error sending data by uart");
            return err;
        }
    }

    for (size_t i = 0; i < m_client.size(); i++) {
        if (chat_id_equal(client_uid.to_uid, chat_id_unspecified)) {
            if (chat_id_equal(client_uid.from_uid, m_client[i].shaUID)) {
                err = httpd_ws_send_frame_async(req_handler->handle, m_client[i].fd, &ws_pkt);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "httpd_ws_send_frame_async failed with: %s",
                        esp_err_to_name(err));
                    return ESP_FAIL;
                }
            }
        } else if (chat_id_equal(client_uid.to_uid, m_client[i].shaUID)) {
            err = httpd_ws_send_frame_async(req_handler->handle, m_client[i].fd, &ws_pkt);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "httpd_ws_send_frame_async failed with: %s",
                    esp_err_to_name(err));
                return ESP_FAIL;
            }
        } else {
            err = sendUart(ws_pkt);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Error sending data by uart");
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "Error when sending data by the uart");
                    return ESP_FAIL;
                }
            }
        }
    }

    return ESP_OK;
}

void Websocket::pong(httpd_req_t * req)
{
    if (m_client.size() != 0) {
        return;
    }

    static const char* data = "Async data";
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (std::uint8_t*)data;
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

esp_err_t Websocket::sendUart(httpd_ws_frame_t ws_pkt)
{
    chat_msg_t msg;

    ESP_LOG_BUFFER_CHAR(TAG, ws_pkt.payload, ws_pkt.len);

    /* Parse JSON */
    cJSON *msg_root = cJSON_Parse((const char*)ws_pkt.payload);

    cJSON* type_root = cJSON_GetObjectItemCaseSensitive(msg_root, "type");

    if (cJSON_IsNumber(type_root)) {
        msg.type = (uint64_t)type_root->valuedouble;
    } else {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "%lld", msg.type);

    if (msg.type != 2) {
        /* Gather message fields */
        cJSON* from_uid_root = cJSON_GetObjectItemCaseSensitive(msg_root, "fromUID");
        cJSON* to_uid_root = cJSON_GetObjectItemCaseSensitive(msg_root, "toUID");
        cJSON* msg_msg_root = cJSON_GetObjectItemCaseSensitive(msg_root, "msg");
        cJSON* msg_msg_text_root = cJSON_GetObjectItemCaseSensitive(msg_msg_root, "text");
        cJSON* msg_id_root = cJSON_GetObjectItemCaseSensitive(msg_root, "msgID");
        cJSON* timestamp_root = cJSON_GetObjectItemCaseSensitive(msg_root, "timestamp");

        if (_json_parse_hex(from_uid_root, msg.from_uid, sizeof(chat_id_t)) != ESP_OK) {
            return ESP_FAIL;
        }

        if (_json_parse_hex(to_uid_root, msg.to_uid, sizeof(chat_id_t)) != ESP_OK) {
            ESP_LOGI(TAG, "multicast message!");
        } else {
            std::memcpy(msg.to_uid, chat_id_unspecified, sizeof(chat_id_t));
        }

        if (cJSON_IsObject(msg_msg_root) && cJSON_IsString(msg_msg_text_root)) {
            char *msg_text = cJSON_GetStringValue(msg_msg_text_root);
            std::size_t len = strlen(msg_text);
            if (len > sizeof(msg.msg)) {
                ESP_LOGE(TAG, "sorry mate, we don't do that here (yet).");
                return ESP_FAIL;
            } else {
                std::memcpy(msg.msg, msg_text, len);
                msg.msg_len = len;
            }
        } else {
            return ESP_FAIL;
        }

        if (_json_parse_hex(msg_id_root, msg.msg_id, sizeof(chat_id_t)) != ESP_OK) {
            return ESP_FAIL;
        }

        if (cJSON_IsNumber(timestamp_root)) {
            msg.timestamp = (uint64_t)timestamp_root->valuedouble;
        } else {
            return ESP_FAIL;
        }
    } else {
        /* TODO: handle these messages */
        return ESP_OK;
    }

    cJSON_Delete(msg_root);

    std::uint8_t buffer[256];

    CborEncoder encoder;
    cbor_encoder_init(&encoder, buffer, sizeof(buffer), 0);

    CborEncoder map_encoder;
    cbor_encoder_create_map(&encoder, &map_encoder, 6);

    cbor_encode_text_stringz(&map_encoder, "fromUID");
    cbor_encode_byte_string(&map_encoder, msg.from_uid, sizeof(chat_id_t));

    cbor_encode_text_stringz(&map_encoder, "toUID");
    cbor_encode_byte_string(&map_encoder, msg.to_uid, sizeof(chat_id_t));

    cbor_encode_text_stringz(&map_encoder, "msgID");
    cbor_encode_byte_string(&map_encoder, msg.to_uid, sizeof(chat_id_t));

    cbor_encode_text_stringz(&map_encoder, "msg");
    cbor_encode_byte_string(&map_encoder, msg.msg, msg.msg_len);

    cbor_encode_text_stringz(&map_encoder, "timestamp");
    cbor_encode_uint(&map_encoder, msg.timestamp);

    cbor_encode_text_stringz(&map_encoder, "type");
    cbor_encode_uint(&map_encoder, msg.type);

    cbor_encoder_close_container(&encoder, &map_encoder);
    std::size_t length = cbor_encoder_get_buffer_size(&encoder, buffer);

    int cnt = radio::write(buffer, length);
    if (cnt < 0) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "mate this just worked!\n");

    return ESP_OK;
}
