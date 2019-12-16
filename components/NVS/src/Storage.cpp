/**
 * @file NVS.cpp
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-20
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "Storage.h"

#include <cstdint>

#include "esp_system.h"

#include "esp_log.h"
#include "nvs_flash.h"
#include "defaults.h"

namespace storage {

static const char* TAG = "NVS";

esp_err_t init()
{   
    esp_err_t err;

    ESP_LOGD(TAG, "Initializing NVS");
    err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
        err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGD(TAG, "NVS partition is truncated. Erasing partition...");

        err = nvs_flash_erase();
        if (err != ESP_OK) {
            ESP_LOGD(TAG, "Couldn't erase NVS partition");
            return err;
        }

        ESP_LOGD(TAG, "Retrying nvs_flash_init");
        return nvs_flash_init();
    }

    return err;
}

NVS::NVS() : m_is_opened(false), m_handle{0} {}

NVS::~NVS()
{
    close();
}

esp_err_t NVS::open(const char* name, nvs_open_mode open_mode)
{
    esp_err_t err;

    ESP_LOGD(TAG, "Opening namespace");

    if (m_is_opened) close();

    err = nvs_open(name, open_mode, &m_handle);
    if (err != ESP_OK) {
        const char* err_str = esp_err_to_name(err);
        ESP_LOGE(TAG, "Couldn't open namespace (%s)", err_str);
        return err;
    }

    m_is_opened = true;
    return ESP_OK;
}

void NVS::close()
{
    ESP_LOGD(TAG, "Closing namespace");

    if (m_is_opened) nvs_close(m_handle);
    m_is_opened = false;
}

esp_err_t NVS::set_bool(const char* key, bool value)
{
    return nvs_set_u8(m_handle, key, value ? 1 : 0);
}

esp_err_t NVS::get_bool(const char* key, bool& value)
{
    std::uint8_t v = 0;
    esp_err_t err = nvs_get_u8(m_handle, key, &v);
    if (err != ESP_OK) return err;

    if (v == 1) {
        value = true;
    } else {
        value = false;
    }

    return ESP_OK;
}

esp_err_t NVS::commit()
{
    return nvs_commit(m_handle);
}

esp_err_t NVS::getIsConfigured(bool& is_configured)
{
    esp_err_t err;

    storage::NVS app_nvs;
    err = app_nvs.open(NVS_APP_NAMESPACE, NVS_READWRITE);
    if (err != ESP_OK) {
        const char* err_str = esp_err_to_name(err);
        ESP_LOGE(TAG,
            "Couldn't open namespace \"%s\" (%s)",
            NVS_APP_NAMESPACE,
            err_str);
        return err;
    }

    err = app_nvs.get_bool(NVS_IS_CONFIGURED_KEY, is_configured);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // Set is_configured to true on flash so on next init the config is
        // readed directly by the ESP-IDF Wi-Fi library component.
        err = app_nvs.set_bool(NVS_IS_CONFIGURED_KEY, true);
        if (err != ESP_OK) return err;
        err = app_nvs.commit();
        if (err != ESP_OK) return err;
        // Set the return variable to "false" to forcibly set the default
        // configuration
        is_configured = false;
    } else {
        return err;
    }

    return ESP_OK;
}


} // namespace nvs
