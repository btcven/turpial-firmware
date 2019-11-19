/**
 * @file SerializableBytes.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-19
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef SERIALIZABLE_BYTES_H
#define SERIALIZABLE_BYTES_H

#include <array>
#include <cstring>
#include <initializer_list>

#include "Serializable.h"

/**
 * @brief Thin wrapper over an std::array<std::uint8_t, N> in order to
 * serialize/deserialize it
 * 
 * @tparam N: byte count
 */
template <unsigned int N>
class SerializableBytes
{
public:
    SerializableBytes() : m_array{0} {}

    /**
     * @brief Assigns the contents of a "const char*" string to this bytes
     * 
     * @param[in] str: the string to copy from
     * 
     * @return SerializableBytes<N>& "SerializableBytes" with the content
     */
    SerializableBytes<N>& operator=(const char* str)
    {
        std::size_t length = std::strlen(str);
        if (length > (N - 1)) {
            // If higher than expected only use the bytes we need to create a
            // C string.
            length = N - 1;
        }

        std::memcpy(m_array.data(), str, length);
        m_array[length] = '\0';

        return *this;
    }

    /**
     * @brief Get bytes data pointer
     * 
     * @return std::uint8_t* the pointer
     */
    std::uint8_t* data()
    {
        return m_array.data();
    }

    /**
     * @brief Get bytes data pointer
     * 
     * @return std::uint8_t* the pointer
     */
    const std::uint8_t* data() const
    {
        return m_array.data();
    }

    /**
     * @brief Get C string pointer
     * 
     * @return char* C string pointer
     */
    char* c_str()
    {
        return reinterpret_cast<char*>(m_array.data());
    }

    /**
     * @brief Index a byte
     * 
     * @param[in] pos: position
     * 
     * @return std::uint8_t& reference to byte
     */
    std::uint8_t& operator[](std::size_t pos)
    {
        return m_array[pos];
    }

    /**
     * @brief Index a byte
     * 
     * @param[in] pos: position
     * 
     * @return const std::uint8_t& reference to byte
     */
    const std::uint8_t& operator[](std::size_t pos) const
    {
        return m_array[pos];
    }

    /**
     * @brief Get serialize size (for now it's N)
     * 
     * @return std::size_t the size in bytes
     */
    virtual std::size_t serialize_size() const
    {
        return N;
    }

    /**
     * @brief Serialize this bytes
     * 
     * @param[in] stream: where we are going to serialize to
     * 
     * @return std::ostream& the same "stream"
     */
    virtual std::ostream& serialize(std::ostream& stream) const
    {
        const char* ptr = reinterpret_cast<const char*>(m_array.data());
        stream.write(ptr, N);
        return stream;
    }

    /**
     * @brief Deserialize bytes
     * 
     * @param[in] stream: where we are going to deserialize from 
     * 
     * @return std::istream& the same "stream"
     */
    virtual std::istream& deserialize(std::istream& stream)
    {
        char* ptr = reinterpret_cast<char*>(m_array.data());
        stream.read(ptr, N);
        return stream;
    }

private:
    std::array<std::uint8_t, N> m_array;
};


#endif // SERIALIZABLE_BYTES_H