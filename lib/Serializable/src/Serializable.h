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

// Interface class

/**
 * @brief This class is used as interface to be implemented by other classes type data transfer object or DTO
 * 
 */
class Serializable
{
public:
    virtual std::size_t serialize_size() const = 0;
    virtual char* serialize(char* dataOut) const = 0;
    virtual const char* deserialize(const char* dataIn) = 0;
};
#endif