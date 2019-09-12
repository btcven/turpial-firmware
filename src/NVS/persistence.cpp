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
    if (!_started || !key || !value || _readOnly)
    {
        return 0;
    }

    esp_err_t err = nvs_set_str(_handle, key, value);

    if (err)
    {
        ESP_LOGE("nvs_setString", "nvs_set_str fail: %s %s", key, esp_err_to_name(err));
        return 0;
    }

    err = nvs_commit(_handle);

    if (err)
    {
        ESP_LOGE("nvs_setString", "nvs_commit fail: %s %s", key, esp_err_to_name(err));
        return 0;
    }
    else
    {
        return strlen(value);
    }
}

/**
 * @brief Get string  
 * 
 * @param key 
 * @param defaultValue 
 * @return const char* 
 */
const char *Persistence::getString(const char *key, const char *defaultValue)
{
    char *value = NULL;
    size_t len = 0;

    if (!_started || !key)
    {
        return defaultValue;
    }

    esp_err_t err = nvs_get_str(_handle, key, value, &len);

    if (err)
    {
        ESP_LOGE("nvs_get", "nvs_get_str len fail: %s %s", key, esp_err_to_name(err));
        return defaultValue;
    }
    else
    {
        char buf[len];
        value = buf;
        err = nvs_get_str(_handle, key, value, &len);

        if (err)
        {
            ESP_LOGE("nvs_get", "nvs_get_str fail: %s %s", key, esp_err_to_name(err));
            return defaultValue;
        }
        else
        {
            const char *toConstChar(buf);
            return toConstChar;
        }
    }
}