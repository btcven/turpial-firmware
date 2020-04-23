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
#include <cstdlib>
#include <cstring>
#include <esp_log.h>
#include <iostream>

#include <cbor.h>

static const char* TAG = "Websocket";

chat_id_t chat_id_unspecified = {0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff};

static esp_err_t _json_parse_hex(cJSON* root, std::uint8_t* buf, std::size_t max_size)
{
    if (!cJSON_IsString(root)) {
        ESP_LOGE(TAG, "Field is not a string");
        return ESP_FAIL;
    } else {
        char* from_uid_str = cJSON_GetStringValue(root);
        if (strlen(from_uid_str) > (max_size * 2)) {
            ESP_LOGE(TAG, "Invalid fromUID size");
            return ESP_FAIL;
        }
        util::hexToBytes(cJSON_GetStringValue(root), buf);
    }

    return ESP_OK;
}

esp_err_t _parse_chat_id(CborValue* map_it, const char* key, chat_id_t* id)
{
    /* Find value in map */
    CborValue id_it;
    cbor_value_map_find_value(map_it, key, &id_it);

    /* Verify we've got a valid byte string */
    if (!cbor_value_is_valid(&id_it) &&
        !cbor_value_is_byte_string(&id_it)) {
        return ESP_FAIL;
    }

    /* Verify length is okay */
    size_t len;
    cbor_value_calculate_string_length(&id_it, &len);
    if (len != sizeof(chat_id_t)) {
        return ESP_ERR_INVALID_SIZE;
    }

    /* Copy byte string */

    cbor_value_copy_byte_string(&id_it, (std::uint8_t*)id, &len, NULL);


    return ESP_OK;
}

esp_err_t _parse_msg_content(CborValue* map_it, const char* key, chat_msg_content_t* content)
{
    CborValue content_it;
    cbor_value_map_find_value(map_it, key, &content_it);

    /* Verify we've got a byte string */
    if (!cbor_value_is_valid(&content_it) &&
        !cbor_value_is_byte_string(&content_it)) {
        ESP_LOGE(TAG, "it's not valid");
        return ESP_FAIL;
    }

    /* Find value in the map */
    std::size_t len;
    cbor_value_calculate_string_length(&content_it, &len);

    /* Copy byte string */
    content->len = (std::size_t)len;
    cbor_value_copy_byte_string(&content_it, content->buf, &len, NULL);

    return ESP_OK;
}

esp_err_t _parse_uint64(CborValue* map_it, const char* key, uint64_t* out)
{
    /* Find value in the map */
    CborValue int_it;
    cbor_value_map_find_value(map_it, key, &int_it);

    /* Verify we've got a valid integer */
    if (!cbor_value_is_valid(&int_it) &&
        !cbor_value_is_integer(&int_it)) {
        return ESP_FAIL;
    }

    /* Copy byte string */
    cbor_value_get_uint64(&int_it, out);
    return ESP_OK;
}


esp_err_t parseMessage(std::uint8_t* buffer, chat_msg_t* msg, size_t length)
{
    CborParser parser;
    CborValue it;

    if (cbor_parser_init(buffer, length, 0, &parser, &it) != CborNoError) {
        ESP_LOGE(TAG, "chat: couldn't parse chat cbor input");
        return ESP_FAIL;
    }

    if (!cbor_value_is_map(&it)) {
        ESP_LOGE(TAG, "chat: not a map");
        return ESP_FAIL;
    }

    /* Parse fromUID */
    if (_parse_chat_id(&it, "fromUID", &msg->from_uid) != ESP_OK) {
        ESP_LOGE(TAG, "chat: fromUID is invalid!");
        return ESP_FAIL;
    }

    /* Parse toUID */
    if (_parse_chat_id(&it, "toUID", &msg->to_uid) != ESP_OK) {
        ESP_LOGE(TAG, "chat: toUID is invalid!");
        return ESP_FAIL;
    }

    /* Parse msgID */
    if (_parse_chat_id(&it, "msgID", &msg->msg_id) != ESP_OK) {
        ESP_LOGE(TAG, "chat: msgID is invalid!");
        return ESP_FAIL;
    }

    /* Parse message content */
    if (_parse_msg_content(&it, "msg", &msg->msg) != ESP_OK) {
        ESP_LOGE(TAG, "chat: msg is invalid!");
        return ESP_FAIL;
    }

    /* Parse timestamp */
    if (_parse_uint64(&it, "timestamp", &msg->timestamp) != ESP_OK) {
        ESP_LOGE(TAG, "chat: invalid timestamp!");
        return ESP_FAIL;
    }

    /* Parse type */
    if (_parse_uint64(&it, "type", &msg->type) != ESP_OK) {
        ESP_LOGE(TAG, "chat: invalid type!");
        return ESP_FAIL;
    }


    return ESP_OK;
}


Websocket::Websocket()
{
    static CheckConnections g_check_connections;
    g_check_connections.start(NULL);
}

void Websocket::onReceive(httpd_ws_frame_t ws_pkt, httpd_req_t* req)
{
    ESP_LOGI(TAG, "STRING %s", ws_pkt.payload);
    m_server = req->handle;
    req_handler = req;
    ESP_LOGI(TAG, "onReceive(%p): %d", req, httpd_req_to_sockfd(req));
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
        }


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
    ESP_LOGI(TAG, "ws_async_send(%p)", arg);
    async_resp_arg_t* resp_arg = reinterpret_cast<async_resp_arg_t*>(arg);

    ESP_LOGI(TAG, "ws_async_send: %p, %d", resp_arg->hd, resp_arg->fd);
    esp_err_t err =
        httpd_ws_send_frame_async(resp_arg->hd, resp_arg->fd, &resp_arg->ws_pkt);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "httpd_ws_send_frame_async %s", esp_err_to_name(err));
    }

    free(resp_arg);
}

esp_err_t Websocket::async_send(httpd_handle_t handle, int fd,
                                httpd_ws_frame_t &ws_pkt)
{
    ESP_LOGI(TAG, "async_send(%p, %d)", handle, fd);
    async_resp_arg_t* resp_arg = reinterpret_cast<async_resp_arg_t*>(malloc(sizeof(async_resp_arg_t)));
    resp_arg->hd = handle;
    resp_arg->fd = fd;
    resp_arg->ws_pkt = ws_pkt;

    return httpd_queue_work(handle, ws_async_send, resp_arg);
}

esp_err_t Websocket::messageRecipient(std::uint8_t* payload, uid_message_t* uid_receiving)
{
    char* res = reinterpret_cast<char*>(payload);
    cJSON* req_root = cJSON_Parse(res);
    cJSON* cjson_fromUID = cJSON_GetObjectItemCaseSensitive(req_root, "fromUID");
    cJSON* cjson_toUID = cJSON_GetObjectItemCaseSensitive(req_root, "toUID");


    const char* from_uid = cjson_fromUID->valuestring;
    ESP_LOGI(TAG, "paso messageRecipient");

    if (cJSON_IsNull(cjson_toUID)) {
        /* Set ID to unspeficied */
        std::memcpy(uid_receiving->to_uid, chat_id_unspecified, sizeof(chat_id_t));
    } else if (cJSON_IsString(cjson_toUID)) {
        const char* to_uid = cjson_toUID->valuestring;
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
        }
    }

    for (size_t i = 0; i < m_client.size(); i++) {
        ESP_LOG_BUFFER_HEXDUMP(TAG, client_uid.to_uid, sizeof(chat_id_t), ESP_LOG_INFO);
        ESP_LOG_BUFFER_HEXDUMP(TAG, client_uid.from_uid, sizeof(chat_id_t), ESP_LOG_INFO);
        if (chat_id_equal(client_uid.to_uid, chat_id_unspecified)) {
            ESP_LOGI(TAG, "sending to broadcast");
            if (!chat_id_equal(client_uid.from_uid, m_client[i].shaUID)) {
                ESP_LOGI(TAG, "ptr %p", m_server);
                err = async_send(m_server, m_client[i].fd, ws_pkt);
                if (err != ESP_OK) {
                    ESP_LOGE(TAG, "async_send failed with: %s",
                        esp_err_to_name(err));
                    return ESP_FAIL;
                }
            }
        } else if (!chat_id_equal(client_uid.to_uid, m_client[i].shaUID)) {
            ESP_LOGI(TAG, "sending direct message");
            err = async_send(m_server, m_client[i].fd, ws_pkt);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "async_send failed with: %s",
                    esp_err_to_name(err));
                return ESP_FAIL;
            }
        } else {
            ESP_LOGI(TAG, "sending by uart");
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

void Websocket::pong(httpd_req_t* req)
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

        esp_err_t err = async_send(req->handle, m_client[i].fd, ws_pkt);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "async_send failed with %d", err);
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
    cJSON* msg_root = cJSON_Parse((const char*)ws_pkt.payload);

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

        if (_json_parse_hex(to_uid_root, msg.to_uid, sizeof(chat_id_t)) == ESP_OK) {
            ESP_LOGI(TAG, "toUID provided!");
        } else {
            ESP_LOGI(TAG, "toUID not provided, defaulting to broadcast!");
            std::memcpy(msg.to_uid, chat_id_unspecified, sizeof(chat_id_t));
        }

        if (cJSON_IsObject(msg_msg_root) && cJSON_IsString(msg_msg_text_root)) {
            char* msg_text = cJSON_GetStringValue(msg_msg_text_root);
            std::size_t len = strlen(msg_text);
            if (len > sizeof(msg.msg)) {
                ESP_LOGE(TAG, "sorry mate, we don't do that here (yet).");
                return ESP_FAIL;
            } else {
                std::memcpy(msg.msg.buf, msg_text, len);
                msg.msg.len = len;
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
    cbor_encode_byte_string(&map_encoder, msg.msg_id, sizeof(chat_id_t));

    cbor_encode_text_stringz(&map_encoder, "msg");
    cbor_encode_byte_string(&map_encoder, msg.msg.buf, msg.msg.len);

    cbor_encode_text_stringz(&map_encoder, "timestamp");
    cbor_encode_uint(&map_encoder, msg.timestamp);

    cbor_encode_text_stringz(&map_encoder, "type");
    cbor_encode_uint(&map_encoder, msg.type);

    cbor_encoder_close_container(&encoder, &map_encoder);
    std::size_t length = cbor_encoder_get_buffer_size(&encoder, buffer);

    ESP_LOG_BUFFER_HEXDUMP(TAG, buffer, length, ESP_LOG_INFO);

    int cnt = radio::write(buffer, length);
    if (cnt < 0) {
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "mate this just worked!\n");
    return ESP_OK;
}


void Websocket::websocketRadioRx(const std::uint8_t* buffer, std::size_t length)
{
    chat_msg_t msg;
    esp_err_t err;
    size_t uidlen = 32;
    err = parseMessage((std::uint8_t*)buffer, &msg, length);
    if (err) {
        ESP_LOGE("TEST", "error decoding the value");
    }

    cJSON* root = cJSON_CreateObject();

    char fromUID[65];
    util::bytesToHex(msg.from_uid, fromUID, uidlen);
    cJSON_AddStringToObject(root, "fromUID", fromUID);

    if (chat_id_equal(msg.to_uid, chat_id_unspecified)) {
        cJSON_AddNullToObject(root, "toUID");
    } else {
        char toUID[65];
        util::bytesToHex(msg.to_uid, toUID, uidlen);
        cJSON_AddStringToObject(root, "toUID", toUID);
    }

    char msgID[65];
    util::bytesToHex(msg.msg_id, msgID, uidlen);
    cJSON_AddStringToObject(root, "msgID", msgID);

    cJSON_AddNumberToObject(root, "timestamp", msg.timestamp);
    cJSON_AddNumberToObject(root, "type", msg.type);
    cJSON* ap_root = cJSON_AddObjectToObject(root, "msg");
    cJSON_AddStringToObject(ap_root, "text", (char*)msg.msg.buf);

    char* payload = cJSON_Print(root);
    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    ws_pkt.payload = (std::uint8_t*)payload;
    ws_pkt.type = HTTPD_WS_TYPE_TEXT;
    ws_pkt.len = strlen(payload);
    ws_pkt.final = 1;

    ESP_LOGI(TAG, "payload %s", payload);

    checkMessageType(ws_pkt, true);
}
