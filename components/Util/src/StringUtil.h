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
 * @file StringUtil.h
 * @author Locha Mesh Developers (contact@locha.io)
 * 
 */


#ifndef UTIL_STRINGUTIL_H
#define UTIL_STRINGUTIL_H

#include <vector>
#include <string>

namespace util {

/**
 * @brief Split a string into parts based on a delimiter.
 *
 * @param[in] source: The source string to split.
 * @param[in] delimiter: The delimiter characters.
 *
 * @return A vector of strings that are the split of the input.
 */
std::vector<std::string> split(std::string source, char delimiter);

/**
 * @brief Convert a string to lower case.
 *
 * @param [in] value The string to convert to lower case.
 *
 * @return A lower case representation of the string.
 */
std::string toLower(std::string& value);

/**
 * @brief Remove white space from a string.
 *
 * @return String without white spaces.
 */
std::string trim(const std::string& str);

} // namespace util

#endif // UTIL_STRINGUTIL_H
