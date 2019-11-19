/**
 * @file Serializable.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-11-02
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <cstddef>
#include <iostream>

// Interface class

/**
 * @brief This class is used as interface to be implemented by other classes
 * type data transfer object or DTO
 * 
 */
class Serializable
{
public:
    /**
     * @brief Get the size of an object when serialized, this is the totalsize in bytes
     * 
     * @return std::size_t the number of bytes
     */
    virtual std::size_t serialize_size() const = 0;

    /**
     * @brief Serialize this object to the given stream
     * 
     * @param[in] stream: the stream to write to
     * 
     * @return std::ostream& a reference to the same stream we wrote
     */
    virtual std::ostream& serialize(std::ostream& stream) const = 0;

    /**
     * @brief Deserialize this object from a stream
     * 
     * @param[in] stream: the stream to deserialize from 
     * 
     * @return std::istream& a reference to the same stream we readed
     */
    virtual std::istream& deserialize(std::istream& stream) = 0;
};

#endif