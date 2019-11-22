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

#include "NVS.h"

#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"

#include "esp_log.h"
#include "nvs_flash.h"

namespace nvs {

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

Namespace::Namespace() : m_is_opened(false), m_handle{0} {}

Namespace::~Namespace()
{
    close();
}

esp_err_t Namespace::open(const char* name, nvs_open_mode open_mode)
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

void Namespace::close()
{
    ESP_LOGD(TAG, "Closing namespace");

    if (m_is_opened) nvs_close(m_handle);
    m_is_opened = false;
}

esp_err_t Namespace::set_bool(const char* key, bool value)
{
    return nvs_set_u8(m_handle, key, value ? 1 : 0);
}

esp_err_t Namespace::get_bool(const char* key, bool& value)
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

esp_err_t Namespace::commit()
{
    return nvs_commit(m_handle);
}

} // namespace nvs