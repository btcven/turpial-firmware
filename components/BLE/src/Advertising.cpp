/**
 * @file Advertising.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#include "Advertising.h"

#include <cstring>

#include <esp_log.h>

namespace ble {

/// Logging tag
const char* TAG = "BLE_Advertising";

void AdvertisementData::setFlags(std::uint8_t flags)
{
    m_payload[m_pos + 0] = 2;
    m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_FLAG;
    m_payload[m_pos + 2] = flags;
    m_pos += 3;
    ESP_LOGD(TAG, "%s end: m_pos = %x", __func__, m_pos);
}

void AdvertisementData::setName(const char* name)
{
    std::size_t len = std::strlen(name);
    m_payload[m_pos + 0] = 1 + len;
    m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_NAME_CMPL;
    m_pos += 2;
    std::memcpy(&m_payload[m_pos], name, len);
    m_pos += len;
    ESP_LOGD(TAG, "%s end: m_pos = %x", __func__, m_pos);
}

Advertising::Advertising() : m_params(), m_adv_state()
{
    m_params.adv_int_min = 0x20;
    m_params.adv_int_max = 0x40;
    m_params.adv_type = ADV_TYPE_IND;
    m_params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
    m_params.peer_addr_type = BLE_ADDR_TYPE_PUBLIC;
    m_params.channel_map = ADV_CHNL_ALL;
    m_params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;
}

void Advertising::setData(AdvertisementData& adv_data)
{
    ESP_LOGD(TAG, "set ADV data");
    std::uint8_t* raw_data = adv_data.getPayload();
    std::uint32_t len = adv_data.length();
    esp_err_t err = esp_ble_gap_config_adv_data_raw(raw_data, len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
            "esp_ble_gap_config_adv_data_raw: %s",
            esp_err_to_name(err));
        return;
    }
    m_adv_state.set_config_state();
}


void Advertising::setScanResponseData(AdvertisementData& adv_data)
{
    ESP_LOGD(TAG, "set scan rsp ADV data");
    std::uint8_t* raw_data = adv_data.getPayload();
    std::uint32_t len = adv_data.length();
    esp_err_t err = esp_ble_gap_config_scan_rsp_data_raw(raw_data, len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
            "esp_ble_gap_config_scan_rsp_data_raw: %s",
            esp_err_to_name(err));
    }
    m_adv_state.set_scan_rsp_config_state();
}

void Advertising::start()
{
    ESP_LOGI(TAG, "ADV start");
    esp_err_t err = esp_ble_gap_start_advertising(&m_params);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
            "esp_ble_gap_start_advertising: %s",
            esp_err_to_name(err));
    }
}

void Advertising::stop()
{
    ESP_LOGI(TAG, "ADV stop");
    esp_err_t err = esp_ble_gap_stop_advertising();
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
            "esp_ble_gap_stop_advertising: %s",
            esp_err_to_name(err));
    }
}

} // namespace ble
