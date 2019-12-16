/**
 * @file Uuid.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#ifndef BLE_UUID_H
#define BLE_UUID_H

#include <cstdint>

#include <esp_bt_defs.h>
#include <esp_err.h>

namespace ble {

/**
 * @brief A BLE Universal-Unique-Identifier
 * 
 */
class Uuid
{
public:
    /**
     * @brief Construct a new Uuid
     * 
     */
    Uuid();

    /**
     * @brief Construct a new 16-bit Uuid
     * 
     * @param uuid 
     */
    Uuid(std::uint16_t uuid);

    /**
     * @brief Create a UUID from an hexadecimal string
     * 
     * @param hex: hexadecimal string
     * @param[out] uuid: the UUID
     * 
     * @return
     *      - ESP_OK: succeed.
     *      - ESP_FAIL: invalid hex string, invalid UUID
     */
    static esp_err_t fromHex(const char* hex, Uuid& uuid);

    /**
     * @brief Compare with other UUID
     * 
     * @param other: the other UUID
     * 
     * @return true: equals
     * @return false: not equals
     */
    bool equals(const Uuid& other) const;

    /**
     * @brief Copy this UUID to a raw UUID.
     * 
     * @param other: where to copy this UUID
     */
    void copyTo(esp_bt_uuid_t& other) const;

    /**
     * @brief Get the raw value of this UUID
     * 
     * @return esp_bt_uuid_t& the raw UUID
     */
    esp_bt_uuid_t& getRaw();

private:
    esp_bt_uuid_t m_inner; /**< Inner value of the UUID */
};

} // namespace ble

#endif // BLE_UUID_H
