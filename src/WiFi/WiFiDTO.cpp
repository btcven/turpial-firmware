/**
 * @file WiFiDTO.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "WiFiDTO.h"

#include "SerializablePOD.h"
#include "Serializable.h"

// Implementation of all virtual methods from serializable interface.

namespace wifi {

std::size_t DTOConfig::serialize_size() const {
    std::size_t size;
    size = pod::serialize_size<std::int8_t>(apChannel) + 
           pod::serialize_size<std::int8_t>(apMaxConn) + 
           pod::serialize_size<bool>(WAP_enabled) +
           pod::serialize_size<bool>(WST_enabled) +
           pod::serialize_size<bool>(isOpen) +
           apSSID.serialize_size() +
           apPassword.serialize_size();  

    return size; 
}

std::ostream& DTOConfig::serialize(std::ostream& stream) const {
    pod::serialize<std::int8_t>(stream, apChannel);
    pod::serialize<std::int8_t>(stream, apMaxConn);
    pod::serialize<bool>(stream, WAP_enabled);
    pod::serialize<bool>(stream, WST_enabled);
    pod::serialize<bool>(stream, isOpen);
    apSSID.serialize(stream);
    apPassword.serialize(stream);
    return stream;
}

std::istream& DTOConfig::deserialize(std::istream& stream) {
    pod::deserialize<std::int8_t>(stream, apChannel);
    pod::deserialize<std::int8_t>(stream, apMaxConn);
    pod::deserialize<bool>(stream, WAP_enabled);
    pod::deserialize<bool>(stream, WST_enabled);
    pod::deserialize<bool>(stream, isOpen);
    apSSID.deserialize(stream);
    apPassword.deserialize(stream);
    return stream;
}

} // namespace wifi