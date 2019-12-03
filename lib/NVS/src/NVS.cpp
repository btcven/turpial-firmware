/**
 * @file NVS.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-20
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "NVS.h"

#include "esp_log.h"

namespace storage {

static const char* TAG = "NVS";

NVS::NVS()
    : m_handle(0),
      m_is_opened(false)
{
}

NVS::~NVS()
{
    if (m_is_opened) nvs_close(m_handle);
}

esp_err_t NVS::start()
{
    esp_err_t err = 0;

    ESP_LOGI(TAG, "Initializing NVS");

    nvs_flash_erase();

    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition is truncated. Erasing partition...");

        err = nvs_flash_erase();
        if (err != ESP_OK) {
            ESP_LOGW(TAG, "Couldn't erase NVS partition (%s)", esp_err_to_name(err));
            return err;
        }

        ESP_LOGD(TAG, "Retrying nvs_flash_init");
        err = nvs_flash_init();
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "Retry failed (%s)", esp_err_to_name(err));
            return err;
        }
    }

    return ESP_OK;
}

esp_err_t NVS::open(const char* name, nvs_open_mode mode)
{
    if (m_is_opened) return ESP_FAIL;

    ESP_LOGD(TAG, "Opening namespace");

    esp_err_t err = nvs_open("preferences", NVS_READWRITE, &m_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't open namespace (%s)", esp_err_to_name(err));
        return err;
    }

    m_is_opened = true;

    return ESP_OK;
}

esp_err_t NVS::setBlob(const char* key, const std::uint8_t* data, std::size_t len)
{
    if (!m_is_opened) return ESP_FAIL;

    return nvs_set_blob(m_handle, key, data, len);
}

esp_err_t NVS::getBlob(const char* key, std::vector<uint8_t>& data)
{
    esp_err_t err = 0;
    std::size_t len;

    if (!m_is_opened) return ESP_FAIL;

    err = nvs_get_blob(m_handle, key, nullptr, &len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't get blob length (%s)", esp_err_to_name(err));
        return err;
    }

    data.resize(len, 0);

    err = nvs_get_blob(m_handle, key, &data[0], &len);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't get blob (%s)", esp_err_to_name(err));
        return err;
    }

    ESP_LOGD(TAG, "len = %d, size = %d", len, data.size());

    return ESP_OK;
}

esp_err_t NVS::setU8(const char* key, std::uint8_t value)
{
    if (!m_is_opened) return ESP_FAIL;

    return nvs_set_u8(m_handle, key, value);
}

esp_err_t NVS::getU8(const char* key, std::uint8_t& value)
{
    if (!m_is_opened) return ESP_FAIL;

    return nvs_get_u8(m_handle, key, &value);
}

esp_err_t NVS::commit()
{
    if (!m_is_opened) return ESP_FAIL;

    return nvs_commit(m_handle);
}

} // namespace storage