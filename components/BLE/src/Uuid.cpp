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

#include "Uuid.h"

#include <esp_log.h>

#include <cstring>

#include "Hex.h"

namespace ble {

static const char* TAG = "BLE_Uuid";

Uuid::Uuid() : m_inner() {}

Uuid::Uuid(std::uint16_t uuid) : m_inner()
{
    m_inner.len = ESP_UUID_LEN_16;
    m_inner.uuid.uuid16 = uuid;
}

esp_err_t Uuid::fromHex(const char* hex, Uuid& uuid)
{
    std::size_t hex_len = std::strlen(hex);
    std::size_t uuid_len = hex_len / 2;

    switch (uuid_len) {
    case ESP_UUID_LEN_16: {
        std::uint8_t uuid_16[ESP_UUID_LEN_16];

        esp_err_t err = util::hexToBytes(hex, uuid_16);
        if (err != ESP_OK) return err;

        uuid.m_inner.len = ESP_UUID_LEN_16;
        std::memcpy(&uuid.m_inner.uuid.uuid16, uuid_16, ESP_UUID_LEN_16);

        return ESP_OK;
        break;
    }
    case ESP_UUID_LEN_32: {
        std::uint8_t uuid_32[ESP_UUID_LEN_32];

        esp_err_t err = util::hexToBytes(hex, uuid_32);
        if (err != ESP_OK) return err;

        uuid.m_inner.len = ESP_UUID_LEN_32;
        std::memcpy(&uuid.m_inner.uuid.uuid32, uuid_32, ESP_UUID_LEN_32);

        return ESP_OK;
        break;
    }
    case ESP_UUID_LEN_128: {
        std::uint8_t uuid_128[ESP_UUID_LEN_128];

        esp_err_t err = util::hexToBytes(hex, uuid_128);
        if (err != ESP_OK) return err;

        uuid.m_inner.len = ESP_UUID_LEN_128;
        std::memcpy(uuid.m_inner.uuid.uuid128, uuid_128, ESP_UUID_LEN_128);

        return ESP_OK;
    }
    default: {
        ESP_LOGE(TAG, "invalid UUID len");
        return ESP_FAIL;
        break;
    }
    }

    return ESP_FAIL;
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
                   ESP_UUID_LEN_128) == 0;
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
