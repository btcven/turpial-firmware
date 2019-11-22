/**
 * @file Advertising.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-21
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "Advertising.h"

#include <cstring>

#include "esp_log.h"

namespace ble {

const char* TAG = "BLE_Advertising";

void AdvertisementData::setAppearance(std::uint16_t appearance)
{
    std::uint8_t* bytes = reinterpret_cast<std::uint8_t*>(&appearance);
    m_payload[m_pos + 0] = 3;
    m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_APPEARANCE;
    m_payload[m_pos + 2] = bytes[0];
    m_payload[m_pos + 3] = bytes[1];
    m_pos += 4;

    ESP_LOGD(TAG, "%s end: m_pos = %x", __func__, m_pos);
}

void AdvertisementData::setCompleteServices(esp_bt_uuid_t uuid)
{
    if (uuid.len == ESP_UUID_LEN_16) {
        m_payload[m_pos + 0] = 3;
        m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_16SRV_CMPL;
        m_pos += 2;
        std::memcpy(&m_payload[m_pos], &uuid.uuid.uuid16, 2);
        m_pos += 2;
        ESP_LOGD(TAG, "%s end: m_pos = %x", __func__, m_pos);
        return;
    } else if (uuid.len == ESP_UUID_LEN_32) {
        m_payload[m_pos + 0] = 5;
        m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_32SRV_CMPL;
        m_pos += 2;
        std::memcpy(&m_payload[m_pos], &uuid.uuid.uuid32, 4);
        m_pos += 4;
        ESP_LOGD(TAG, "%s end: m_pos = %x", __func__, m_pos);
        return;
    } else if (uuid.len == ESP_UUID_LEN_128) {
        m_payload[m_pos + 0] = 17;
        m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_128SRV_CMPL;
        m_pos += 2;
        std::memcpy(&m_payload[m_pos], uuid.uuid.uuid128, 16);
        m_pos += 16;
        ESP_LOGD(TAG, "%s end: m_pos = %x", __func__, m_pos);
        return;
    } else {
        ESP_LOGE(TAG, "Invalid UUID length");
        return;
    }
}

void AdvertisementData::setFlags(std::uint8_t flags)
{
    m_payload[m_pos + 0] = 2;
    m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_FLAG;
    m_payload[m_pos + 2] = flags;
    m_pos += 3;
    ESP_LOGD(TAG, "%s end: m_pos = %x", __func__, m_pos);
}

void AdvertisementData::setManufacturerData(std::uint8_t* data,
    std::size_t len)
{
    m_payload[m_pos + 0] = 1 + len;
    m_payload[m_pos + 1] = ESP_BLE_AD_MANUFACTURER_SPECIFIC_TYPE;
    m_pos += 2;
    std::memcpy(&m_payload[m_pos], data, len);
    m_pos += len;
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

void AdvertisementData::setPartialServices(esp_bt_uuid_t uuid)
{
    if (uuid.len == ESP_UUID_LEN_16) {
        m_payload[m_pos + 0] = 3;
        m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_16SRV_PART;
        m_pos += 2;
        std::memcpy(&m_payload[m_pos], &uuid.uuid.uuid16, 2);
        m_pos += 2;
        ESP_LOGD(TAG, "%s end: m_pos = %x", __func__, m_pos);
        return;
    } else if (uuid.len == ESP_UUID_LEN_32) {
        m_payload[m_pos + 0] = 5;
        m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_32SRV_PART;
        m_pos += 2;
        std::memcpy(&m_payload[m_pos], &uuid.uuid.uuid32, 4);
        m_pos += 4;
        ESP_LOGD(TAG, "%s end: m_pos = %x", __func__, m_pos);
        return;
    } else if (uuid.len == ESP_UUID_LEN_128) {
        m_payload[m_pos + 0] = 17;
        m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_128SRV_PART;
        m_pos += 2;
        std::memcpy(&m_payload[m_pos], uuid.uuid.uuid128, 16);
        m_pos += 16;
        ESP_LOGD(TAG, "%s end: m_pos = %x", __func__, m_pos);
        return;
    } else {
        ESP_LOGE(TAG, "Invalid UUID length");
        return;
    }
}

void AdvertisementData::setServiceData(esp_bt_uuid_t uuid,
    std::uint8_t* data,
    std::size_t len)
{
    if (uuid.len == ESP_UUID_LEN_16) {
        m_payload[m_pos + 0] = len + 3;
        m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_16SRV_PART;
        m_pos += 2;
        std::memcpy(&m_payload[m_pos], &uuid.uuid.uuid16, 2);
        m_pos += 2;
    } else if (uuid.len == ESP_UUID_LEN_32) {
        m_payload[m_pos + 0] = len + 5;
        m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_32SRV_PART;
        m_pos += 2;
        std::memcpy(&m_payload[m_pos], &uuid.uuid.uuid32, 4);
        m_pos += 4;
    } else if (uuid.len == ESP_UUID_LEN_128) {
        m_payload[m_pos + 0] = len + 17;
        m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_128SRV_PART;
        m_pos += 2;
        std::memcpy(&m_payload[m_pos], uuid.uuid.uuid128, 16);
        m_pos += 16;
    } else {
        ESP_LOGE(TAG, "Invalid UUID length");
        return;
    }

    std::memcpy(&m_payload[m_pos], data, len);
    m_pos += len;

    ESP_LOGD(TAG, "%s end: m_pos = %x", __func__, m_pos);
}

void AdvertisementData::setShortName(const char* name)
{
    std::size_t len = std::strlen(name);
    m_payload[m_pos + 0] = 1 + len;
    m_payload[m_pos + 1] = ESP_BLE_AD_TYPE_NAME_SHORT; // 0x08
    std::memcpy(&m_payload[m_pos], name, len);
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