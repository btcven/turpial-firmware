/**
 * @file WiFiDTO.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-11-02
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include <array>
#include <cstdint>
#include <cstdlib>

#include "esp_wifi.h"

#include "Serializable.h"
#include "SerializableBytes.h"

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
        : ap_channel(0), ap_max_conn(0), wifi_mode(WIFI_MODE_NULL), is_open(false), ap_ssid(), ap_password(), wst_ssid(), wst_password() {}

    /**
     * @brief Returns the serialize size
     * 
     * @return std::size_t size in bytes
     */
    virtual std::size_t serialize_size() const;

    /**
     * @brief Serializes this WiFiDTOConfig to the given buffer.
     * 
     * @param[in] stream: buffer where data is being serialized to.
     * 
     * @return std::ostream& a reference to the stream we wrote to
     */
    virtual std::ostream& serialize(std::ostream& stream) const;

    /**
     * @brief Deserializes a WiFiDTOConfig from the given buffer.
     * 
     * @param[in] stream: the stream from where the data is being deserialized.
     * 
     * @return std::istream& a reference to the stream we readed from
     */
    virtual std::istream& deserialize(std::istream& stream);

public:
    std::int8_t ap_channel;
    std::int8_t ap_max_conn;
    wifi_mode_t wifi_mode;
    bool is_open;
    SerializableBytes<32> ap_ssid;
    SerializableBytes<64> ap_password;
    SerializableBytes<32> wst_ssid;
    SerializableBytes<64> wst_password;
};

} // namespace wifi