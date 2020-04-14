
/**
 * @file Serialization.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-04-11
 * 
 * @copyright Copyright (c) 2020 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */


#include "esp_err.h"
#include <cbor.h>
#include <stdint.h>
#include <iostream>
#include <esp_log.h>

#ifndef UTIL_SERILIZATION_H
#define UTIL_SERILIZATION_H


namespace util {

esp_err_t encode(uint8_t* payload, std::uint8_t* encode, size_t buf_size);
esp_err_t decode(uint8_t* encode, char* decodeData);

} // namespace util


#endif // UTIL_SERILIZATION_L