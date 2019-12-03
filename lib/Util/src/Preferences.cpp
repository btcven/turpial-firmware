/**
 * @file Preferences.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-29
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "Preferences.h"

#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include <cstring>

namespace util {

static const char* TAG = "Preferences";

/**
 * @brief Copy a C string to a fixed size std::uint8_t buffer
 * 
 * @tparam MAX: number of bytes in dest
 * 
 * @param dest: destination
 * @param src: source
 */
template <unsigned int MAX>
void stringToFixedBytes(std::uint8_t* dest, const char* src)
{
    std::size_t len = std::strlen(src);
    if (len >= MAX - 1) {
        ESP_LOGW(TAG, "String is too big, trimming, max = %d", MAX);
        len = MAX - 1;
    }

    std::memcpy(dest, src, len);
    dest[len] = '\0';
}

/**
 * @brief Get the Fixed Blob object
 * 
 * @tparam MAX: maximum number of bytes
 * 
 * @param nvs: NVS instance
 * @param key: key name
 * @param data: blob data output
 * 
 * @return
 *      - ESP_OK: succeed
 *      - (others): failed
 */
template <unsigned int MAX>
esp_err_t getFixedBlob(storage::NVS& nvs, const char* key, std::uint8_t* data)
{
    std::vector<std::uint8_t> value;
    esp_err_t err = nvs.getBlob(key, value);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't get blob (%s)", esp_err_to_name(err));
        return err;
    }

    if (value.size() > MAX - 1) {
        ESP_LOGE(TAG, "Blob is too big, size = %d, max = %d", value.size(), MAX);
        return ESP_FAIL;
    }

    std::memcpy(data, &value[0], value.size());
    data[value.size()] = '\0';

    return ESP_OK;
}

/**
 * @brief Set the a fixed size blob
 * 
 * @tparam MAX: blob size
 * 
 * @param nvs: NVS instance
 * @param key: key name
 * @param data: blob data buffer
 * 
 * @return
 *      - ESP_OK: succeed
 *      - (others): failed
 */
template <unsigned int MAX>
esp_err_t setFixedBlob(storage::NVS& nvs, const char* key, std::uint8_t* data)
{
    std::size_t len = std::strlen(reinterpret_cast<char*>(data));
    if (len > MAX - 1) {
        ESP_LOGE(TAG, "Blob size is too big, size = %d", len);
        return ESP_FAIL;
    }

    esp_err_t err = nvs.setBlob(key, data, len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't set blob (%s)", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

/**
 * @brief Get the a type T from a std::uint8_t in the NVS
 * 
 * @tparam T: type to get
 * 
 * @param nvs 
 * @param key 
 * @param value
 * 
 * @return
 *      - ESP_OK: succeed
 *      - (others): failed
 */
template <typename T>
esp_err_t getFromU8(storage::NVS& nvs, const char* key, T& value)
{
    std::uint8_t& v = reinterpret_cast<std::uint8_t&>(value);
    return nvs.getU8(key, v);
}

/**
 * @brief Set the a type T to std::uint8_t in the NVS
 * 
 * @tparam T: type to set
 * 
 * @param nvs: NVS instance
 * @param key: key name
 * @param value: value to set
 *  
 * @return
 *      - ESP_OK: succeed
 *      - (others): failed
 */
template <typename T>
esp_err_t setToU8(storage::NVS& nvs, const char* key, T value)
{
    std::uint8_t v = static_cast<std::uint8_t>(value);
    return nvs.setU8(key, v);
}

void DefaultPreferences::setApSSID(const char* ssid)
{
    stringToFixedBytes<32>(ap_ssid, ssid);
}

void DefaultPreferences::setApPassword(const char* password)
{
    stringToFixedBytes<64>(ap_password, password);
}

void DefaultPreferences::setApMaxConn(std::uint8_t max_conn)
{
    ap_max_conn = max_conn;
}

void DefaultPreferences::setApChannel(std::uint8_t channel)
{
    ap_channel = channel;
}

void DefaultPreferences::setStaSsid(const char* ssid)
{
    stringToFixedBytes<32>(sta_ssid, ssid);
}

void DefaultPreferences::setStaPassword(const char* password)
{
    stringToFixedBytes<64>(sta_password, password);
}

void DefaultPreferences::setWiFiMode(wifi_mode_t mode)
{
    wifi_mode = mode;
}

Preferences::Preferences()
    : m_nvs()
{
}

esp_err_t Preferences::start(DefaultPreferences default_config)
{
    esp_err_t err;

    err = m_nvs.open("preferences", NVS_READWRITE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't open preferences namespace (%s)", esp_err_to_name(err));
        return err;
    }

    bool use_default;
    err = getUseDefault(use_default);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        err = setUseDefault(true);
        if (err != ESP_OK) return err;

        use_default = true;
    } else if (err != ESP_OK) {
        return err;
    }

    // Is use_default is true it means that the default preferences needs to be
    // written to the NVS so the program utilizes that configuration instead.
    if (use_default) {
        ESP_LOGI(TAG, "using configuration defaults");

        // Set default values on the NVS
        setApSSID(default_config.ap_ssid);
        setApPassword(default_config.ap_password);
        setApMaxConn(default_config.ap_max_conn);
        setApChannel(default_config.ap_channel);
        setStaSSID(default_config.sta_ssid);
        setStaPassword(default_config.sta_password);
        setWiFiMode(default_config.wifi_mode);

        setUseDefault(false);

        // Save changes
        commit();
    }

    return ESP_OK;
}

esp_err_t Preferences::setUseDefault(bool value)
{
    return setToU8<bool>(m_nvs, "pr.use_default", value);
}

esp_err_t Preferences::getUseDefault(bool& value)
{
    return getFromU8<bool>(m_nvs, "pr.use_default", value);
}

esp_err_t Preferences::setApSSID(std::uint8_t* data)
{
    return setFixedBlob<32>(m_nvs, "pr.ap.ssid", data);
}

esp_err_t Preferences::getApSSID(std::uint8_t* data)
{
    return getFixedBlob<32>(m_nvs, "pr.ap.ssid", data);
}

esp_err_t Preferences::setApPassword(std::uint8_t* data)
{
    return setFixedBlob<64>(m_nvs, "pr.ap.password", data);
}

esp_err_t Preferences::getApPassword(std::uint8_t* data)
{
    return getFixedBlob<64>(m_nvs, "pr.ap.password", data);
}

esp_err_t Preferences::setApMaxConn(std::uint8_t ap_max_conn)
{
    return m_nvs.setU8("pr.ap.maxconn", ap_max_conn);
}

esp_err_t Preferences::getApMaxConn(std::uint8_t& ap_max_conn)
{
    return m_nvs.getU8("pr.ap.maxconn", ap_max_conn);
}

esp_err_t Preferences::setApChannel(std::uint8_t ap_channel)
{
    return m_nvs.setU8("pr.ap.channel", ap_channel);
}

esp_err_t Preferences::getApChannel(std::uint8_t& ap_channel)
{
    return m_nvs.getU8("pr.ap.channel", ap_channel);
}

esp_err_t Preferences::setStaSSID(std::uint8_t* data)
{
    return setFixedBlob<32>(m_nvs, "pr.sta.ssid", data);
}

esp_err_t Preferences::getStaSSID(std::uint8_t* data)
{
    return getFixedBlob<32>(m_nvs, "pr.sta.ssid", data);
}

esp_err_t Preferences::setStaPassword(std::uint8_t* data)
{
    return setFixedBlob<64>(m_nvs, "pr.sta.password", data);
}

esp_err_t Preferences::getStaPassword(std::uint8_t* data)
{
    return getFixedBlob<64>(m_nvs, "pr.sta.password", data);
}

esp_err_t Preferences::setWiFiMode(wifi_mode_t wifi_mode)
{
    return setToU8<wifi_mode_t>(m_nvs, "pr.mode", wifi_mode);
}

esp_err_t Preferences::getWiFiMode(wifi_mode_t& wifi_mode)
{
    return getFromU8<wifi_mode_t>(m_nvs, "pr.mode", wifi_mode);
}

esp_err_t Preferences::commit()
{
    return m_nvs.commit();
}


} // namespace util