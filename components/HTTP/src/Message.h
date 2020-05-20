/**
 * Copyright 2020 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** 
 * @file Message.h
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */


#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H

#include "Hex.h"
#include "esp_err.h"
#include <cJSON.h>
#include <cbor.h>
#include <cstring>
#include <esp_log.h>
#include <iostream>
#include <stdint.h>
#include <string>

/**
 * @brief Chat ID
 */
typedef std::uint8_t chat_id_t[32];

/**
 * @brief Value for unspecified chat ID
 */
extern chat_id_t chat_id_unspecified;

/**
 * @brief Check if IDs are equal
 *
 * @param[in] a ID to compare.
 * @param[in] b ID to compare.
 *
 * @return true if equal.
 */
static inline bool chat_id_equal(chat_id_t a, chat_id_t b)
{
    return memcmp(a, b, sizeof(chat_id_t)) == 0;
}


/**
 * @brief Check if the IDs are different
 *
 * @param[in] a ID to compare.
 * @param[in] b ID to compare.
 *
 * @return true if equal.
 */

typedef struct {
    uint8_t buf[128]; /**< Buffer with contents */
    size_t len;       /**< Bytes used in buf */
} chat_msg_content_t;

struct chat_msg_t {
    chat_id_t from_uid;
    chat_id_t to_uid;
    chat_msg_content_t msg;
    chat_id_t msg_id;
    std::uint64_t timestamp;
    std::uint64_t type;
};


namespace message {
/**
 * @brief decoded string message
 *
 * @param[in]  CborValue cbor object
 * @param[in]  key name of the object to decode
 * @param[out] out  data decoded
 * @return esp_err_t
 * 
 */
esp_err_t parseMsgContent(CborValue* map_it, const char* key, chat_msg_content_t* content);
/**
 * @brief uid data decoder
 *
 * @param[in]  CborValue cbor object
 * @param[in]  key name of the object to decode
 * @param[out] out  data decoded
 * @return esp_err_t
 * 
 */
esp_err_t parseChatId(CborValue* map_it, const char* key, chat_id_t* id);

/**
 * @brief int data decoder
 *
 * @param[in]  CborValue cbor object
 * @param[in]  key name of the object to decode
 * @param[out] out  data decoded
 * @return esp_err_t
 * 
 */
esp_err_t parseUint64(CborValue* map_it, const char* key, uint64_t* out);


/**
 * @brief converts a string to bytes
 *
 * @param[out] msg  data converted to byte 
 * @param[in]  buffer encoded message 
 * @param[in]  length size of the message
 * @return esp_err_t
 * 
 */
esp_err_t parseMessage(std::uint8_t* buffer, chat_msg_t* msg, size_t length);


/**
 * @brief decodes the message in cbor format
 *
 * @param[out] buf  data converted to byte 
 * @param[in]  root cJSON object
 * @param[in]  max_size size of the buf
 * @return esp_err_t
 * 
 */
esp_err_t jsonParseHex(cJSON* root, std::uint8_t* buf, std::size_t max_size);

/**
 * @brief get all the data from the message
 *
 * @param[out] msg message data already parsed
 * @param[in]  payload cbor encoded data
 * @return esp_err_t
 * 
 */

esp_err_t getAllMessage(chat_msg_t* msg, std::uint8_t* payload);


/**
 * @brief get message type
 *
 * @param[in] payload data sent by the client
 *
 * @return
 * -  0: when the message type is handshakey
 * -  1: when the message type is message
 * -  2: when the message type is status
 * -  4: when the message type is action
 * - -1: when the message type is invalid
 */
int getTypeMessage(std::uint8_t* payload);


} // namespace message


#endif // HTTP_MESSAGE_H