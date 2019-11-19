/**
 * @file TinyString.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef TINYSTRING_H
#define TINYSTRING_H

#include "Serializable.h"
#include "SerializablePOD.h"


#include <algorithm>
#include <cstring>
#include <string>

namespace tinystring {

/**
 * @brief
 * A String that can be serialized and deserialized with the Serializable class
 * 
 */
class String : public Serializable
{
public:
    /**
     * @brief Construct a new empty String object
     * 
     */
    String() : m_inner() {}
    /**
     * @brief Construct a new String object copying the supplied str allocating
     * new memory.
     * 
     * @param str The string to be copied.
     */
    String(const char* str) : m_inner(str) {}

    /**
     * @brief Get the length of the string
     * 
     * @return std::size_t the length
     */
    std::size_t length() const
    {
        return m_inner.length();
    }

    /**
     * @brief Get the underlying string.
     * 
     * @return The string.
     */
    const std::string& str() const
    {
        return m_inner;
    }

    /**
     * @brief Get the underlying const char*.
     * 
     * @return The string.
     */
    const char* c_str() const
    {
        return m_inner.c_str();
    }

    /**
     * @brief Copy this string to the given buffer
     * 
     * @param[out] buf: the output buffer, it should be at least
     * "String::length() + 1" 
     */
    void copy_to(char* buf) const
    {
        std::size_t length_ = length();
        std::memcpy(buf, m_inner.c_str(), length_);
        buf[length_] = '\0';
    }

    virtual std::size_t serialize_size() const
    {
        return sizeof(std::size_t) + length();
    }

    virtual std::ostream& serialize(std::ostream& stream) const
    {
        auto const length_ = length();
        pod::serialize<std::size_t>(stream, length_);
        stream.write(m_inner.c_str(), length_);

        return stream;
    }

    virtual std::istream& deserialize(std::istream& stream)
    {
        // If this String is already being used with some data and has allocated
        // memory just clear it.
        if (!m_inner.empty()) m_inner.clear();

        std::size_t length_;
        pod::deserialize(stream, length_);

        m_inner.reserve(length_);
        //std::copy_n(std::istreambuf_iterator<char>(stream), length_, std::back_inserter(m_inner));
        stream.read(&m_inner[0], length_);
        m_inner[length_] = '\0';
        return stream;
    }

private:
    std::string m_inner;
};

} // namespace tinystring

#endif // TINYSTRING_H