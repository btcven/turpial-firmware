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
 * @file StringUtil.cpp
 * @author Locha Mesh Developers (contact@locha.io)
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
