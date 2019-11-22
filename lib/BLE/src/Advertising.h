/**
 * @file Advertising.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BLE_ADVERTISING_H
#define BLE_ADVERTISING_H

#include <array>
#include <cstdint>

#include "esp_bt_defs.h"
#include "esp_gap_ble_api.h"
#include "esp_log.h"

namespace ble {

class AdvertisementData
{
public:
    AdvertisementData() : m_payload{0}, m_pos(0) {}

    void setAppearance(std::uint16_t appearance);
    void setCompleteServices(esp_bt_uuid_t uuid);
    void setFlags(std::uint8_t flags);
    void setManufacturerData(std::uint8_t* data, std::size_t len);
    void setName(const char* name);
    void setPartialServices(esp_bt_uuid_t uuid);
    void setServiceData(esp_bt_uuid_t uuid, std::uint8_t* data, std::size_t len);
    void setShortName(const char* name);

    std::size_t length() const { return m_pos; }

    std::uint8_t* getPayload() { return m_payload.data(); }

private:
    std::array<std::uint8_t, 31> m_payload;
    std::size_t m_pos;
};

/// Advertising::setData was called
#define STATE_CONFIG (1 << 0)
/// Advertising::setScanResponseData was called
#define STATE_SCAN_RSP_CONFIG (1 << 1)

class AdvertisementState
{
public:
    AdvertisementState() : m_state(0) {}

    void set_config_state()
    {
        m_state |= STATE_CONFIG;
    }

    void set_scan_rsp_config_state()
    {
        m_state |= STATE_SCAN_RSP_CONFIG;
    }

    bool is_config_state()
    {
        ESP_LOGD("AdvState", "bfr %s m_state = %x", __func__, m_state);
        bool ret = false;
        if ((m_state & STATE_CONFIG) == STATE_CONFIG) {
            ESP_LOGD("AdvState", "%s true", __func__);
            ret = true;
        }
        m_state = m_state & (~STATE_CONFIG);
        ESP_LOGD("AdvState", "aft %s m_state = %x", __func__, m_state);
        return ret;
    }

    bool is_scan_rsp_config_state()
    {
        ESP_LOGD("AdvState", "bfr %s m_state = %x", __func__, m_state);
        bool ret = false;
        if ((m_state & STATE_SCAN_RSP_CONFIG) == STATE_SCAN_RSP_CONFIG) {
            ESP_LOGD("AdvState", "%s true", __func__);
            ret = true;
        }
        m_state = m_state & (~STATE_SCAN_RSP_CONFIG);
        ESP_LOGD("AdvState", "aft %s m_state = %x", __func__, m_state);
        return ret;
    }

private:
    std::uint8_t m_state;
};

class Advertising
{
public:
    Advertising();

    void setData(AdvertisementData& adv_data);
    void setScanResponseData(AdvertisementData& adv_data);

    esp_ble_adv_params_t& parameters() { return m_params; }
    AdvertisementState& state() { return m_adv_state; }

    void start();
    void stop();

private:
    esp_ble_adv_params_t m_params;
    AdvertisementState m_adv_state;
};

} // namespace ble

#endif // BLE_ADVERTISING_H