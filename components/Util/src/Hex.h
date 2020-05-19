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
 * @file Hex.h
 * @author Locha Mesh Developers (contact@locha.io)
 * 
 */

#ifndef UTIL_HEX_H
#define UTIL_HEX_L

#include <cstdint>

#include "esp_err.h"

namespace util {

/**
 * @brief Convert an hexadecimal digit character to an integer
 * 
 * @param      c: hexadecimal character
 * @param[out] r: return value
 * 
 * @return
 *      - ESP_OK: succeed
 *      - ESP_FAIL: invalid hex digit
 */
esp_err_t hexToInt(const char c, int& r);

/**
 * @brief Converts an hexadecimal string to a byte array
 * 
 * This function assumes src to be a zero terminated sanitized string with
 * an even number of [0-9a-f] characters, and target to be sufficiently large.
 *
 * @param      src: null terminated hexadecimal source string
 * @param[out] target: byte array
 * 
 * @return
 *      - ESP_OK: succeed
 *      - ESP_FAIL: an invalid character was found
 */
esp_err_t hexToBytes(const char* src, std::uint8_t* target);

/**
 * @brief Converts an array of bytes to a hexadecimal string
 * 
 *
 * @param      buf: null terminated hexadecimal source string
 * @param      len: byte size
 * @param[out] dst: hex array
 * 
 * @return
 *      - ESP_OK: succeed
 *      - ESP_FAIL: an invalid character was found
 */
esp_err_t bytesToHex(std::uint8_t* buf, char* dst, std::size_t len);

/**
 * 
 * @brief function to convert of decimal to hex 
 * @param[in] n decimal numbers
 *
 * @return char*
 */
void decToHexa(int n, uint8_t* hex_data);

} // namespace util


#endif // UTIL_HEX_L