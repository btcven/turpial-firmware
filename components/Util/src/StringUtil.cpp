/**
 * @file StringUtil.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief
 * @version 0.1
 * @date 2019-11-22
 *
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 *
 */

#include "StringUtil.h"

#include <cstdint>
#include <cctype>
#include <algorithm>
#include <sstream>

namespace util {

std::vector<std::string> split(std::string source, char delimiter)
{
    // See also: https://stackoverflow.com/questions/5167625/splitting-a-c-stdstring-using-tokens-e-g
    std::vector<std::string> strings;

    std::istringstream iss(source);
    std::string s;

    while (std::getline(iss, s, delimiter)) {
        strings.push_back(trim(s));
    }

    return strings;
}

std::string toLower(std::string& value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) -> unsigned char {
                        return std::tolower(c);
                   });
    return value;
}

std::string trim(const std::string& str)
{
    std::size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) return str;
    std::size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

} // namespace util
