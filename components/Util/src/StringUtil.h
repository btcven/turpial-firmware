/**
 * @file StringUtil.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief
 * @version 0.1
 * @date 2019-11-22
 *
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
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
