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

#include "SerializablePOD.h"
#include "Serializable.h"

#include <algorithm>
#include <cstring>
#include <string>

namespace tinystring {

/**
 * @brief
 * A String that can be serialized and deserialized with the Serializable class
 * 
 */
class String : public Serializable {
public:
    /**
     * @brief Construct a new empty String object
     * 
     */
    String() : _inner() {}
    /**
     * @brief Construct a new String object copying the supplied str allocating
     * new memory.
     * 
     * @param str The string to be copied.
     */
    String(const char* str) : _inner(str) { }

    /**
     * @brief Get the length of the string
     * 
     * @return std::size_t the length
     */
    std::size_t length() const {
        return _inner.length();
    }

    /**
     * @brief Get the underlying string.
     * 
     * @return The string.
     */
    const std::string& str() const {
        return _inner;
    }

    /**
     * @brief Get the underlying const char*.
     * 
     * @return The string.
     */
    const char* c_str() const {
        return _inner.c_str();
    }

    virtual std::size_t serialize_size() const {
        return sizeof(std::size_t) + length();
    }

    virtual std::ostream& serialize(std::ostream& stream) const {
        auto const length_ = length();
        pod::serialize<std::size_t>(stream, length_);
        stream.write(_inner.c_str(), length_);

        return stream;
    }

    virtual std::istream& deserialize(std::istream& stream) {
        // If this String is already being used with some data and has allocated
        // memory just clear it.
        if (!_inner.empty()) {
            _inner.clear();
        }

        std::size_t length_;
        pod::deserialize(stream, length_);

        _inner.reserve(length_);
        std::copy_n(std::istreambuf_iterator<char>(stream), length_, std::back_inserter(_inner));

        return stream;
    }

private:
    std::string _inner;
};

}

#endif // TINYSTRING_H