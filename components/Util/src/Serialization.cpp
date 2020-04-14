
/**
 * @file Serialization.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-04-11
 * 
 * @copyright Copyright (c) 2020 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#include "Serialization.h"


static const char* TAG = "SERIALIZATION";
namespace util {


esp_err_t encode(uint8_t* payload, std::uint8_t* encode, size_t buf_size)
{
    char* res = reinterpret_cast<char*>(payload);
    CborEncoder root_encoder;
    CborParser root_parser;
    CborValue it;
    CborError err = CborNoError;


    // Initialize the outermost cbor encoder
    cbor_encoder_init(&root_encoder, encode, buf_size, 0);

    err = cbor_encode_text_string(&root_encoder, res, strlen(res) + 1);
    if (err != CborNoError) {
        // ESP_LOGE(TAG, "error encoding text: %d ", err);
        return ESP_FAIL;
    }
    return ESP_OK;
}


esp_err_t decode(uint8_t* encode, char* decodeData)
{
    CborParser root_parser;
    CborValue it;
    size_t size;
    char* buf;
    CborError err = CborNoError;

    err = cbor_parser_init(encode, 500, 0, &root_parser, &it);
    if (err != CborNoError) {
        ESP_LOGE(TAG, "error parsing buffer: %d ", err);
        return ESP_FAIL;
    }

    err = cbor_value_to_json(stdout, &it, 0);
    if (err != CborNoError) {
        ESP_LOGE(TAG, "error converting value to json: %d ", err);
        return ESP_FAIL;
    }

    puts("");

    CborType type = cbor_value_get_type(&it);

    if (type == CborTextStringType) {
        err = cbor_value_dup_text_string(&it, &buf, &size, &it);
        if (err != CborNoError) {
            ESP_LOGE(TAG, "error decoding buffer: %d ", err);
            return ESP_FAIL;
        }
    }
    
    memcpy(decodeData, buf, strlen(buf) + 1);

    return ESP_OK;
}

} // namespace util