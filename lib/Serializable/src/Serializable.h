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
    virtual std::size_t serialize_size() const = 0;
    virtual std::ostream& serialize(std::ostream& stream) const = 0;
    virtual std::istream& deserialize(std::istream& stream) = 0;

    friend std::ostream& operator<<(std::ostream& stream,
                                    const Serializable& ser) {
        return ser.serialize(stream);
    }

    friend std::istream& operator>>(std::istream& stream,
                                    Serializable& ser) {
        return ser.deserialize(stream);
    }
};

#endif