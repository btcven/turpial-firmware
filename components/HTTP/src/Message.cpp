/**
 * @file Message.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief
 * @version 0.1
 * @date 2020-04-23
 *
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 *
 */

#include "Message.h"

static const char* TAG = "Message";


namespace message {

esp_err_t _json_parse_hex(cJSON* root, std::uint8_t* buf, std::size_t max_size)
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
int getTypeMessage(std::uint8_t* payload)
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


esp_err_t getAllMessage(chat_msg_t* msg, std::uint8_t* payload)
{
    /* Parse JSON */
    cJSON* msg_root = cJSON_Parse((const char*)payload);

    cJSON* type_root = cJSON_GetObjectItemCaseSensitive(msg_root, "type");

    if (cJSON_IsNumber(type_root)) {
        msg->type = (uint64_t)type_root->valuedouble;
    } else {
        return ESP_FAIL;
    }

    if (msg->type != 1) {
        return ESP_OK;
    }
    /* Gather message fields */
    cJSON* from_uid_root = cJSON_GetObjectItemCaseSensitive(msg_root, "fromUID");
    cJSON* to_uid_root = cJSON_GetObjectItemCaseSensitive(msg_root, "toUID");
    cJSON* msg_msg_root = cJSON_GetObjectItemCaseSensitive(msg_root, "msg");
    cJSON* msg_msg_text_root = cJSON_GetObjectItemCaseSensitive(msg_msg_root, "text");
    cJSON* msg_id_root = cJSON_GetObjectItemCaseSensitive(msg_root, "msgID");
    cJSON* timestamp_root = cJSON_GetObjectItemCaseSensitive(msg_root, "timestamp");

    if (message::_json_parse_hex(from_uid_root, msg->from_uid, sizeof(chat_id_t)) != ESP_OK) {
        return ESP_FAIL;
    }

    if (message::_json_parse_hex(to_uid_root, msg->to_uid, sizeof(chat_id_t)) == ESP_OK) {
        ESP_LOGI(TAG, "toUID provided!");
    } else {
        ESP_LOGI(TAG, "toUID not provided, defaulting to broadcast!");
        std::memcpy(msg->to_uid, chat_id_unspecified, sizeof(chat_id_t));
    }

    if (cJSON_IsObject(msg_msg_root) && cJSON_IsString(msg_msg_text_root)) {
        char* msg_text = cJSON_GetStringValue(msg_msg_text_root);
        std::size_t len = strlen(msg_text);
        if (len > sizeof(msg->msg)) {
            ESP_LOGE(TAG, "sorry mate, we don't do that here (yet).");
            return ESP_FAIL;
        } else {
            std::memcpy(msg->msg.buf, msg_text, len);
            msg->msg.len = len;
        }
    } else {
        return ESP_FAIL;
    }

    if (message::_json_parse_hex(msg_id_root, msg->msg_id, sizeof(chat_id_t)) != ESP_OK) {
        return ESP_FAIL;
    }

    if (cJSON_IsNumber(timestamp_root)) {
        msg->timestamp = (uint64_t)timestamp_root->valuedouble;
    } else {
        return ESP_FAIL;
    }
    
    cJSON_Delete(msg_root);
    return ESP_OK;
}
} // namespace message