/**
 * @file SerializablePOD.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-11-02
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#ifndef SERIALIZABLEPOD_H
#define SERIALIZABLEPOD_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <type_traits>

namespace pod {

template <typename T>
std::size_t serialize_size(const T str)
{
    static_assert(std::is_standard_layout<T>::value, "T needs to be POD");

    return sizeof(str);
}

template <typename T>
std::ostream& serialize(std::ostream& stream, const T value)
{
    static_assert(std::is_standard_layout<T>::value, "T needs to be POD");

    const auto length = serialize_size(value);
    const char* value_str = reinterpret_cast<const char*>(&value);
    return stream.write(value_str, length);
}

template <typename T>
std::istream& deserialize(std::istream& stream, T& target)
{
    static_assert(std::is_standard_layout<T>::value, "T needs to be POD");

    const auto length = serialize_size(target);
    T d;
    stream.read(reinterpret_cast<char*>(&d), length);
    target = d;
    return stream;
}

} // namespace pod

#endif // SERIALIZABLEPOD_H
