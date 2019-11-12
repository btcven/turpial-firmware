/**
 * @file WiFiDTO.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-11-02
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "Serializable.h"
#include "TinyString.h"

#include <iostream>
#include <defaults.h>

#include <cstring>
#include <cstdlib>
#include <cstdint>

/**
 * @brief This class represent the data that need to travel throught:
 *          1. System storage
 *          2. Sockets
 *          3. Serial communication
 * 
 */
struct WiFiDTOConfig : public Serializable {
public:
    WiFiDTOConfig(int8_t channel,
                  int8_t maxconn,
                  bool is_ap,
                  bool is_st,
                  bool is_open,
                  const char* ssid,
                  const char* pass)
        : apChannel(channel)
        , apMaxConn(maxconn)
        , WAP_enabled(is_ap)
        , WST_enabled(is_st)
        , isOpen(is_open)
        , apSSID(ssid)
        , apPassword(pass) { }

    /**
     * @brief Returns the 
     * 
     * @return std::size_t 
     */
    virtual std::size_t serialize_size() const;

    /**
     * @brief Serializes this WiFiDTOConfig to the given buffer.
     * 
     * @param dataOut buffer where data is being serialized to.
     */
    virtual char* serialize(char* dataOut) const;

    /**
     * @brief Deserializes a WiFiDTOConfig from the given buffer.
     * 
     * @param dataIn the buffer from where the daa is being deserialized.
     */
    virtual const char* deserialize(const char* dataIn);

public:
    int8_t apChannel;
    int8_t apMaxConn;
    bool WAP_enabled;
    bool WST_enabled;
    bool isOpen;
    tinystring::String apSSID;
    tinystring::String apPassword;
};