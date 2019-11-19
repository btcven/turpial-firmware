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

/**
 * @brief Get the length of a serialized POD type in bytes
 * 
 * @tparam T the POD type
 * 
 * @return std::size_t the POD size in bytes
 */
template <typename T>
std::size_t serialize_size()
{
    static_assert(std::is_standard_layout<T>::value, "T needs to be POD");

    return sizeof(T);
}

/**
 * @brief Serialize a POD type
 * 
 * @tparam T: the POD type
 * 
 * @param[in] stream: the stream to serialize to
 * @param[in] value: the POD value
 * 
 * @return std::ostream& a reference to the same stream we wrote
 */
template <typename T>
std::ostream& serialize(std::ostream& stream, const T value)
{
    static_assert(std::is_standard_layout<T>::value, "T needs to be POD");

    const auto length = serialize_size<T>();
    const char* value_str = reinterpret_cast<const char*>(&value);
    return stream.write(value_str, length);
}

/**
 * @brief Deserialize a POD type
 * 
 * @tparam T: the POD type
 * 
 * @param[in]  stream: the stream to deserialize from
 * @param[out] target: the result variable where the deserialized value is
 * stored
 * 
 * @return std::istream& a reference to the stream we readed from
 */
template <typename T>
std::istream& deserialize(std::istream& stream, T& target)
{
    static_assert(std::is_standard_layout<T>::value, "T needs to be POD");

    const auto length = serialize_size<T>();
    T d;
    stream.read(reinterpret_cast<char*>(&d), length);
    target = d;
    return stream;
}

} // namespace pod

#endif // SERIALIZABLEPOD_H
