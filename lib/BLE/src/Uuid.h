/**
 * @file Uuid.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BLE_UUID_H
#define BLE_UUID_H

#include <cstdint>

#include "esp_bt_defs.h"
#include "esp_err.h"

namespace ble {

class Uuid
{
public:
    Uuid();
    Uuid(std::uint16_t uuid);

    static esp_err_t fromHex(const char* hex, Uuid& uuid);

    bool equals(const Uuid& other) const;
    void copyTo(esp_bt_uuid_t& ohter) const;
    esp_bt_uuid_t& getRaw();

private:
    esp_bt_uuid_t m_inner;
};

} // namespace ble

#endif // BLE_UUID_H