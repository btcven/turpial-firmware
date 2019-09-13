/**
 * @file NVStorage.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-12
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "NVStorage.h"

/**
 * @brief Construct a new NVStorage:: NVStorage object
 * 
 */
NVStorage::NVStorage()
    : _handle(0), _started(false), _readOnly(false)
{
}

/**
 * @brief Destroy the NVStorage:: NVStorage object
 * 
 */
NVStorage::~NVStorage()
{
    close();
}

/**
 * @brief 
 * 
 * @param name 
 * @param readOnly 
 * @return true 
 * @return false 
 */
bool NVStorage::begin()
{
    if (_started)
    {
        return false;
    }

    const char *TAG = "nvs_init";

    //nvs_flash_erase();
    // Initialize NVS
    ESP_LOGD(TAG, "Initializing NVS...\n");
    esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
        return true;
    }
    else
    {
        return true;
    }
    ESP_ERROR_CHECK(err);

    return false;
}

bool NVStorage::open(const char *name, bool readOnly)
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
void NVStorage::close()
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
bool NVStorage::clear()
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
bool NVStorage::remove(const char *key)
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

void NVStorage::setParam(const char *key, nvs_param_t type, void *value)
{
    if (!_started || !key || !value || _readOnly)
    {
        return;
    }

    esp_err_t err;
    char* param = NULL;
    ESP_LOGD(__func__, "Updating value in NVS...\n");
    switch (type)
    { 
        case NVS_STR:
            param = "str";
            err = nvs_set_str(_handle, key, *((char **)value));
            break;
        case NVS_INT:
            param = "int";
            err = nvs_set_i32(_handle, key, *((int32_t *)value));
            break;
        case NVS_BOOL:
            param = "bool";
            err = nvs_set_u8(_handle, key, *((uint8_t *)value));
            break;
        default:
            ESP_LOGE(__func__, "The value type attempting to write on NVS does not exist!\n");
            return;
    }

    if (err)
    {
        ESP_LOGE(__func__, "nvs_set_%s fail: %s %s", param, key, esp_err_to_name(err));
        return;
    }
    else
    {
        ESP_LOGD(__func__, "nvs_set_%s write done\n", param);
    }

    err = nvs_commit(_handle);
    if (err)
    {
        ESP_LOGE(__func__, "nvs_commit fail: %s %s", key, esp_err_to_name(err));
        return;
    }
    else
    {
        ESP_LOGD(__func__, "nvs_commit done\n");
    }
}

/**
 * @brief Set key: value
 * 
 * @param key 
 * @param value 
 * @return size_t 
 */
void NVStorage::setString(const char *key, const char *value)
{
    setParam(key, NVS_STR, &value);
}

void NVStorage::setInt(const char *key, int32_t value)
{
    setParam(key, NVS_INT, &value);
}

void NVStorage::setBool(const char *key, uint8_t value)
{
    setParam(key, NVS_BOOL, &value);
}

/**
 * @brief Get string  
 * 
 * @param key 
 * @param defaultValue 
 * @return const char* 
 */
char *NVStorage::getString(const char *key, char *defaultValue)
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

int32_t NVStorage::getInt(const char *key, int32_t defaultValue)
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

uint8_t NVStorage::getBool(const char *key, uint8_t defaultValue)
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
