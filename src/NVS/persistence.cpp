/**
 * @file persistence.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "persistence.h"
#include "nvs.h"

/**
 * @brief Construct a new Persistence:: Persistence object
 * 
 */
Persistence::Persistence()
    : _handle(0), _started(false), _readOnly(false)
{
}

/**
 * @brief Destroy the Persistence:: Persistence object
 * 
 */
Persistence::~Persistence()
{
    end();
}

/**
 * @brief 
 * 
 * @param name 
 * @param readOnly 
 * @return true 
 * @return false 
 */
bool Persistence::begin(const char *name, bool readOnly)
{
    if (_started)
    {
        return false;
    }
    _readOnly = readOnly;

    esp_err_t err = nvs_open(name, readOnly ? NVS_READONLY : NVS_READWRITE, &_handle);
    if (err)
    {
        ESP_LOGE("tag", "nvs_open failed: %s", esp_err_to_name(err));
        return false;
    }
    else
    {
        _started = true;
        return true;
    }
}

/**
 * @brief 
 * 
 */
void Persistence::end()
{
    if (!_started)
    {
        return;
    }
    nvs_close(_handle);
    _started = false;
}

/**
 * @brief Clear the entire NVS
 * 
 * @return true 
 * @return false 
 */
bool Persistence::clear()
{
    if (!_started || _readOnly)
    {
        return false;
    }

    esp_err_t err = nvs_erase_all(_handle);

    if (err)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * @brief 
 * 
 * @param key 
 * @return true 
 * @return false 
 */
bool Persistence::remove(const char *key)
{
    if (!_started || !key || _readOnly)
    {
        return false;
    }

    esp_err_t err = nvs_erase_key(_handle, key);

    if (err)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/**
 * @brief Set key: value
 * 
 * @param key 
 * @param value 
 * @return size_t 
 */
size_t Persistence::setString(const char *key, const char *value)
{
    const char *TAG = "nvs_setString";

    if (!_started || !key || !value || _readOnly)
    {
        return 0;
    }

    ESP_LOGD(TAG, "Updating string value in NVS...\n");
    esp_err_t err = nvs_set_str(_handle, key, value);

    if (err)
    {
        ESP_LOGE(TAG, "nvs_set_str fail: %s %s", key, esp_err_to_name(err));
        return 0;
    }
    else 
    {
        ESP_LOGD(TAG, "nvs_set_str write done\n");
    } 

    err = nvs_commit(_handle);

    if (err)
    {
        ESP_LOGE(TAG, "nvs_commit fail: %s %s", key, esp_err_to_name(err));
        return 0;
    }
    else
    {
        ESP_LOGD(TAG, "nvs_commit done\n");
        return strlen(value);
    }
}

void Persistence::setInt(const char *key, int32_t value)
{
    const char *TAG = "nvs_setInt";

    if (!_started || !key || !value || _readOnly)
    {
        return;
    }

    ESP_LOGD(TAG, "Updating int value in NVS...\n");
    esp_err_t err = nvs_set_i32(_handle, key, value);
    if (err)
    {
        ESP_LOGE(TAG, "nvs_set_int fail: %s %s", key, esp_err_to_name(err));
        return;
    }
    else
    {
        ESP_LOGD(TAG, "nvs_set_int write done\n");
    }

    err = nvs_commit(_handle);
    if (err)
    {
        ESP_LOGE(TAG, "nvs_commit fail: %s %s", key, esp_err_to_name(err));
        return;
    }
    else
    {
        ESP_LOGD(TAG, "nvs_commit done\n");
    }
}

void Persistence::setBool(const char *key, uint8_t value)
{
    const char *TAG = "nvs_setBool";

    if (!_started || !key || !value || _readOnly)
    {
        return;
    }

    ESP_LOGD(TAG, "Updating bool value in NVS...\n");
    esp_err_t err = nvs_set_u8(_handle, key, value);
    if (err)
    {
        ESP_LOGE(TAG, "nvs_set_int fail: %s %s", key, esp_err_to_name(err));
        return;
    }
    else
    {
        ESP_LOGD(TAG, "nvs_set_int write done\n");
    }

    err = nvs_commit(_handle);
    if (err)
    {
        ESP_LOGE(TAG, "nvs_commit fail: %s %s", key, esp_err_to_name(err));
        return;
    }
    else
    {
        ESP_LOGD(TAG, "nvs_commit done\n");
    }
}

/**
 * @brief Get string  
 * 
 * @param key 
 * @param defaultValue 
 * @return const char* 
 */
char *Persistence::getString(const char *key, char *defaultValue)
{
    size_t required_size;
    const char* TAG = "nvs_getString";

    if (!_started || !key)
    {
        return defaultValue;
    }

    esp_err_t err = nvs_get_str(_handle, key, NULL, &required_size);

    if (err)
    {
        ESP_LOGE(TAG, "nvs_get_str required_size fail: %s %s", key, esp_err_to_name(err));
        return defaultValue;
    }
    else
    {
        if (required_size == 0) 
        {
            ESP_LOGD(TAG, "No string saved yet!\n");
            return defaultValue;
        } 
        else
        {
            // Reading the string...
            ESP_LOGD(TAG, "Reading string from NVS...");
            char* buff = (char*)malloc(required_size);
            if (!buff)
                return defaultValue;
            err = nvs_get_str(_handle, key, buff, &required_size);
            if (err)
            {
                ESP_LOGE(TAG, "nvs_get_str fail: %s %s", key, esp_err_to_name(err));
                return defaultValue;
            }
            else
            {
                return buff;
            }
        }
    }

    return defaultValue; // ELIMINAR ESTO!
}

int32_t Persistence::getInt(const char *key, int32_t defaultValue)
{
    int32_t myint = 0; // value will default to 0, if not set yet in NVS
    const char *TAG = "nvs_getInt";

    if (!_started || !key)
    {
        return defaultValue;
    }

    ESP_LOGD(TAG, "Reading int from NVS...");
    esp_err_t err = nvs_get_i32(_handle, key, &myint);

    if (err)
    {
        ESP_LOGE(TAG, "nvs_get_int fail: %s %s", key, esp_err_to_name(err));
        return defaultValue;
    }
    else
    {
        return myint;
    }
}

uint8_t Persistence::getBool(const char *key, uint8_t defaultValue)
{
    uint8_t mybool = 0; // value will default to 0, if not set yet in NVS
    const char *TAG = "nvs_getBool";

    if (!_started || !key)
    {
        return defaultValue;
    }

    ESP_LOGD(TAG, "Reading bool from NVS...");
    esp_err_t err = nvs_get_u8(_handle, key, &mybool);

    if (err)
    {
        ESP_LOGE(TAG, "nvs_get_bool fail: %s %s", key, esp_err_to_name(err));
        return defaultValue;
    }
    else
    {
        return mybool;
    }
}
