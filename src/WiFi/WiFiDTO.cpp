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

size_t WiFiDTOConfig::serialize_size() const {
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

char* WiFiDTOConfig::serialize(char* dataOut) const {
    dataOut = pod::serialize<std::int8_t>(dataOut, apChannel);
    dataOut = pod::serialize<std::int8_t>(dataOut, apMaxConn);
    dataOut = pod::serialize<bool>(dataOut, WAP_enabled);
    dataOut = pod::serialize<bool>(dataOut, WST_enabled);
    dataOut = pod::serialize<bool>(dataOut, isOpen);
    dataOut = apSSID.serialize(dataOut);
    dataOut = apPassword.serialize(dataOut);
    return dataOut;
}

const char* WiFiDTOConfig::deserialize(const char* dataIn) {
    dataIn = pod::deserialize<std::int8_t>(dataIn, apChannel);
    dataIn = pod::deserialize<std::int8_t>(dataIn, apMaxConn);
    dataIn = pod::deserialize<bool>(dataIn, WAP_enabled);
    dataIn = pod::deserialize<bool>(dataIn, WST_enabled);
    dataIn = pod::deserialize<bool>(dataIn, isOpen);
    dataIn = apSSID.deserialize(dataIn);
    dataIn = apPassword.deserialize(dataIn);

    return dataIn;
}