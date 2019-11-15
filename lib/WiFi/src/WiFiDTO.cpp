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

#include "Serializable.h"
#include "SerializablePOD.h"


// Implementation of all virtual methods from serializable interface.

namespace wifi {

std::size_t DTOConfig::serialize_size() const
{
    std::size_t size;
    size = pod::serialize_size<std::int8_t>(ap_channel) +
           pod::serialize_size<std::int8_t>(ap_max_conn) +
           pod::serialize_size<bool>(wap_enabled) +
           pod::serialize_size<bool>(wst_enabled) +
           pod::serialize_size<bool>(is_open) +
           ap_ssid.serialize_size() +
           ap_password.serialize_size();

    return size;
}

std::ostream& DTOConfig::serialize(std::ostream& stream) const
{
    pod::serialize<std::int8_t>(stream, ap_channel);
    pod::serialize<std::int8_t>(stream, ap_max_conn);
    pod::serialize<bool>(stream, wap_enabled);
    pod::serialize<bool>(stream, wst_enabled);
    pod::serialize<bool>(stream, is_open);
    ap_ssid.serialize(stream);
    ap_password.serialize(stream);
    return stream;
}

std::istream& DTOConfig::deserialize(std::istream& stream)
{
    pod::deserialize<std::int8_t>(stream, ap_channel);
    pod::deserialize<std::int8_t>(stream, ap_max_conn);
    pod::deserialize<bool>(stream, wap_enabled);
    pod::deserialize<bool>(stream, wst_enabled);
    pod::deserialize<bool>(stream, is_open);
    ap_ssid.deserialize(stream);
    ap_password.deserialize(stream);
    return stream;
}

} // namespace wifi