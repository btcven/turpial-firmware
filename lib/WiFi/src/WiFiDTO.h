/**
 * @file WiFiDTO.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-11-02
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "esp_wifi.h"

#include "Serializable.h"
#include "TinyString.h"

namespace wifi {

/**
 * @brief This class represent the data that need to travel throught:
 *          1. System storage
 *          2. Sockets
 *          3. Serial communication
 * 
 */
struct DTOConfig : public Serializable {
public:
    /**
     * @brief Construct a new empty DTOConfig object
     * 
     */
    DTOConfig()
        : ap_channel(0), ap_max_conn(0), wifi_mode(WIFI_MODE_NULL), is_open(false), ap_ssid(tinystring::String()), ap_password(tinystring::String()), wst_ssid(tinystring::String()), wst_password(tinystring::String()) {}

    wifi_auth_mode_t auth_mode() const;

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
    virtual std::ostream& serialize(std::ostream& stream) const;

    /**
     * @brief Deserializes a WiFiDTOConfig from the given buffer.
     * 
     * @param dataIn the buffer from where the daa is being deserialized.
     */
    virtual std::istream& deserialize(std::istream& stream);

public:
    int8_t ap_channel;
    int8_t ap_max_conn;
    wifi_mode_t wifi_mode;
    bool is_open;
    tinystring::String ap_ssid;
    tinystring::String ap_password;
    tinystring::String wst_ssid;
    tinystring::String wst_password;
};

} // namespace wifi