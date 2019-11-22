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

#include "Uuid.h"

#include "esp_log.h"

#include <cstring>

namespace ble {

static const char* TAG = "BLE_Uuid";

Uuid::Uuid() : m_inner() {}

Uuid::Uuid(std::uint16_t uuid) : m_inner()
{
    m_inner.len = ESP_UUID_LEN_16;
    m_inner.uuid.uuid16 = uuid;
}

bool Uuid::equals(const Uuid& other) const
{
    switch (other.m_inner.len) {
    case ESP_UUID_LEN_16: {
        if (m_inner.len != ESP_UUID_LEN_16) return false;
        return (m_inner.uuid.uuid16 == other.m_inner.uuid.uuid16);
        break;
    }
    case ESP_UUID_LEN_32: {
        if (m_inner.len != ESP_UUID_LEN_32) return false;
        return (m_inner.uuid.uuid32 == other.m_inner.uuid.uuid32);
        break;
    }
    case ESP_UUID_LEN_128: {
        if (m_inner.len != ESP_UUID_LEN_128) return false;
        return std::memcmp(m_inner.uuid.uuid128,
            other.m_inner.uuid.uuid128,
            ESP_UUID_LEN_128);
        break;
    }
    default: {
        ESP_LOGE(TAG, "invalid UUID len");
        return false;
        break;
    }
    }

    return false;
}

void Uuid::copyTo(esp_bt_uuid_t& other) const
{
    other.len = m_inner.len;
    switch (m_inner.len) {
    case ESP_UUID_LEN_16: {
        other.uuid.uuid16 = m_inner.uuid.uuid16;
        break;
    }
    case ESP_UUID_LEN_32: {
        other.uuid.uuid32 = m_inner.uuid.uuid32;
        break;
    }
    case ESP_UUID_LEN_128: {
        std::memcpy(other.uuid.uuid128, m_inner.uuid.uuid128, ESP_UUID_LEN_128);
        break;
    }
    default: {
        ESP_LOGE(TAG, "invalid UUID length, len = 0x%x", other.len);
        break;
    }
    }
}

esp_bt_uuid_t& Uuid::getRaw()
{
    return m_inner;
}

} // namespace ble