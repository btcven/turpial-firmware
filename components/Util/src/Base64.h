/**
 * @file Base64.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-22
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#ifndef UTIL_BASE64_H
#define UTIL_BASE64_H

#include <cstdint>
#include <string>

namespace util {

/**
 * @brief Get Base64 encoded length from buffer length.
 *
 * @param[in] length: Length in bytes.
 *
 * @return std::size_t: The encoded length.
 */
static std::size_t base64EncodedLength(std::size_t length)
{
    return (length + 2 - ((length + 2) % 3)) / 3 * 4;
}

/**
 * @brief Get Base64 encoded length from std::string length.
 *
 * @param[in] in: Input buffer.
 *
 * @return std::size_t: The encoded length.
 */
static std::size_t base64EncodedLength(const std::string& in)
{
    return base64EncodedLength(in.length());
}

/**
 * @brief Encode a chunk of data into base 64.
 *
 * @param[in]   in: Input data.
 * @param[out] out: Output data.
 *
 * @return
 *      - true: succeed.
 *      - false: failed.
 */
bool base64Encode(const std::string& in, std::string* out);

} // namespace util

#endif // UTIL_BASE64_H
