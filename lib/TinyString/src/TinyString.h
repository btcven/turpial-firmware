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

#ifndef STRING_H
#define STRING_H

#include "SerializablePOD.h"
#include "Serializable.h"

#include <cstring>

namespace tinystring {

class String : public Serializable {
public:
    /**
     * @brief Construct a new String object without anything inside.
     * 
     */
    String() : _buf(nullptr) {}

    /**
     * @brief Construct a new String object copying the supplied str allocating
     * new memory.
     * 
     * @param str The string to be copied.
     */
    String(const char* str) : _buf(nullptr) {
        const auto length = std::strlen(str);
        _buf = new char[length + 1];
        std::memcpy(_buf, str, length + 1);
    }

    /**
     * @brief Destroy the String object, frees the internal buffer.
     * 
     */
    ~String() {
        if (_buf != nullptr) {
            delete[] _buf;
        }
    }

    /**
     * @brief Get the length of the string
     * 
     * @return std::size_t the length
     */
    std::size_t length() const {
        return std::strlen(_buf);
    }

    /**
     * @brief Returns a pointer to the first character of the string.
     * 
     * @return const char* the pointer.
     */
    const char* c_str() const {
        return _buf;
    }

    virtual std::size_t serialize_size() const {
        return sizeof(std::size_t) + length();
    }

    virtual char* serialize(char* dataOut) const {
        const auto length_ = length();
        dataOut = pod::serialize<std::size_t>(dataOut, length_);
        std::memcpy(dataOut, _buf, length_);

        return dataOut + length_; 
    }

    virtual const char* deserialize(const char* dataIn) {
        // If this String is already being used with some data and has allocated
        // memory just free the memory, the buf_ isn't reused because it could
        // be bigger than the string to deserialize thus wasting memory. Here
        // we exhange the speed trade-off with the memory trade-off. The
        // performance penalty should be minimal because the go to use case for
        // an String is to just create an empty one and then deserialize.
        if (_buf != nullptr) {
            delete[] _buf;
        }

        std::size_t length;
        dataIn = pod::deserialize(dataIn, length);

        _buf = new char[length + 1];
        std::memcpy(_buf, dataIn + sizeof(std::size_t), length);
        _buf[length] = '\0';

        return dataIn + length;
    }

private:
    char* _buf;
};

}

#endif // STRING_H