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
 * @file Base64.h
 * @author Locha Mesh Developers (contact@locha.io)
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
