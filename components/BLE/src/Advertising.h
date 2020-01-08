/**
 * @file Advertising.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#ifndef BLE_ADVERTISING_H
#define BLE_ADVERTISING_H

#include <array>
#include <cstdint>

#include <esp_bt_defs.h>
#include <esp_gap_ble_api.h>
#include <esp_log.h>

namespace ble {

/**
 * @brief Serializes Adversitement packet data.
 * 
 * @attention 1. Adversitement data has a maximum of only 31 bytes, no more
 * fields can be added after that.
 * 
 * @attention 2. This serializer doesn't implement all fields of GAP
 * adversitement, only those that are really needed here are implemented.
 * 
 */
class AdvertisementData
{
public:
    /**
     * @brief Construct a new AdvertisementData
     * 
     */
    AdvertisementData() : m_payload{0}, m_pos(0) {}

    AdvertisementData(AdvertisementData const&) = delete;
    AdvertisementData& operator=(AdvertisementData const&) = delete;

    /**
     * @brief Set the adversitement flags
     * 
     * @param flags: BLE flags
     */
    void setFlags(std::uint8_t flags);

    /**
     * @brief Set the local device name
     * 
     * @param name: device name. Null terminated string.
     */
    void setName(const char* name);

    /**
     * @brief Get the payload length
     * 
     * @return std::size_t 
     */
    std::size_t length() const { return m_pos; }

    /**
     * @brief Get the payload data buffer
     * 
     * @attention 1. The length of this buffer is specified by calling the
     * AdversitementData::length() method.
     * 
     * @return Payload data buffer
     */
    std::uint8_t* getPayload() { return m_payload.data(); }

private:
    // Fixed size buffer containing the adversitement data payload
    std::array<std::uint8_t, 31> m_payload;
    // Position in m_payload
    std::size_t m_pos;
};

/// Advertising::setData was called
const std::uint8_t STATE_CONFIG = (1 << 0);
/// Advertising::setScanResponseData was called
const std::uint8_t STATE_SCAN_RSP_CONFIG = (1 << 1);

/**
 * @brief The state of the adversitement process
 * 
 */
class AdvertisementState
{
public:
    /**
     * @brief Construct a new Advertisement State
     * 
     */
    AdvertisementState() : m_state(0) {}

    /**
     * @brief Set the state to adversitement configuration
     * 
     */
    void set_config_state()
    {
        m_state |= STATE_CONFIG;
    }

    /**
     * @brief Set the state to adversitement scan response configuration
     * 
     */
    void set_scan_rsp_config_state()
    {
        m_state |= STATE_SCAN_RSP_CONFIG;
    }

    /**
     * @brief Is in adversitement configuration state?
     * 
     * @return true: it's in state
     * @return false: it's not state
     */
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

    /**
     * @brief Is in adversitement scan response configuration state?
     * 
     * @return true: it's in state
     * @return false: it's not in state
     */
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
    // State bit field value
    std::uint8_t m_state;
};

/**
 * @brief Advertising management
 * 
 */
class Advertising
{
public:
    /**
     * @brief Construct a new Advertising object
     * 
     */
    Advertising();

    Advertising(Advertising const&) = delete;
    Advertising& operator=(Advertising const&) = delete;

    /**
     * @brief Set the advertisement data
     * 
     * @param adv_data: advertisement data
     */
    void setData(AdvertisementData& adv_data);

    /**
     * @brief Set the scan response advertisement data
     * 
     * @param adv_data: advertisement data
     */
    void setScanResponseData(AdvertisementData& adv_data);

    /**
     * @brief Get a reference to BLE advertisement parameters to change them,
     * or to get them.
     * 
     * @return esp_ble_adv_params_t& reference to advertisement paramters
     */
    esp_ble_adv_params_t& parameters() { return m_params; }

    /**
     * @brief Get a reference to the AdvertisementState in order to change it
     * or to verify in which state the adversitement is.
     * 
     * @return AdvertisementState& 
     */
    AdvertisementState& state() { return m_adv_state; }

    /**
     * @brief Starts adversiting the device
     * 
     * @attention 1. You must have first configured adversitement data, and
     * scan response adversitement data.
     * 
     */
    void start();

    /**
     * @brief Stops adversiting the device
     * 
     */
    void stop();

private:
    esp_ble_adv_params_t m_params;
    AdvertisementState m_adv_state;
};

} // namespace ble

#endif // BLE_ADVERTISING_H
