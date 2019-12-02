/**
 * @file Hex.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-26
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
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

} // namespace util


#endif // UTIL_HEX_L