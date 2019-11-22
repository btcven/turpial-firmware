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

#include "esp_bt_defs.h"

#include <cstdint>

namespace ble {

class Uuid
{
public:
    Uuid();
    Uuid(std::uint16_t uuid);

    bool equals(const Uuid& other) const;
    void copyTo(esp_bt_uuid_t& ohter) const;
    esp_bt_uuid_t& getRaw();

private:
    esp_bt_uuid_t m_inner;
};

} // namespace ble

#endif // BLE_UUID_H