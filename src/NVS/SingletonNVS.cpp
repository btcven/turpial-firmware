#include "SingletonNVS.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <iostream>

// Define the static Singleton pointer
SingletonNVS* SingletonNVS::nvs_ = NULL;

SingletonNVS* SingletonNVS::getInstance() {
   if (nvs_ == NULL) {
      nvs_ = new SingletonNVS();
   }
   return(nvs_);
}


/**
 * @brief 
 * 
 * @param name 
 * @param readOnly 
 * @return status of nvs_flash_init 
 * @return codes here https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/error-codes.html
 */

esp_err_t SingletonNVS::begin()
{
    if (_started)
    {
        std::cout<<"retornando de la nvs.."<<std::endl;
        return ESP_OK;
    }

    // Initialize NVS
    ESP_LOGD(__func__, "Initializing NVS...\n");
    esp_err_t status = nvs_flash_init();

    if(status == ESP_OK) 
    {
        ESP_LOGE(__func__,"Se inicializo correctamente la NVS");
        return status;
    } else if (status == ESP_ERR_NVS_NO_FREE_PAGES || status == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        status = nvs_flash_init();
        return status;
    }

    ESP_ERROR_CHECK(status);
    return ESP_FAIL;
}

/**
 * @brief 
 * 
 * @param name 
 * @param readOnly 
 * @return true 
 * @return false 
 */

bool SingletonNVS::open(const char *name, bool readOnly)
{
    if (_started)
    {
        return false;
    }
    _readOnly = readOnly;

    esp_err_t err = nvs_open(name, readOnly ? NVS_READONLY : NVS_READWRITE, &_handle);
    if (err)
    {
        ESP_LOGE(__func__, "nvs_open failed: %s", esp_err_to_name(err));
        return false;
    }
    else
    {
        _started = true;
        return true;
    }
}
void SingletonNVS::close()
{
    if (!_started)
    {
        return;
    }
    nvs_close(_handle);
    _started = false;
}


bool SingletonNVS::clear()
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


bool SingletonNVS::remove(const char *key)
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


void SingletonNVS::setParam(const char *key, nvs_param_t type, void *value)
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


void SingletonNVS::setString(const char *key, const char *value)
{
    setParam(key, NVS_STR, &value);
}

void SingletonNVS::setInt(const char *key, int32_t value)
{
    setParam(key, NVS_INT, &value);
}

void SingletonNVS::setBool(const char *key, uint8_t value)
{
    setParam(key, NVS_BOOL, &value);
}

void *SingletonNVS::getParam(const char *key, nvs_param_t type, void *defaultValue)
{
    if (!_started || !key)
    {
        return defaultValue;
    }

    esp_err_t err;
    void *value;
    char* param;
    ESP_LOGD(__func__, "Reading value from NVS...");
    switch (type)
    { 
        case NVS_STR:
            param = "str";
            size_t required_size;

            err = nvs_get_str(_handle, key, NULL, &required_size);
            if (err)
            {
                ESP_LOGE(__func__, "nvs_get_str required_size fail: %s %s", key, esp_err_to_name(err));
                return defaultValue;
            }
            else
            {
                if (required_size == 0)
                {
                    ESP_LOGD(__func__, "No string saved yet!\n");
                    return defaultValue;
                }
                else
                {
                    // Reading the string...
                    char *buff = (char *)malloc(required_size);
                    if (!buff)
                        return defaultValue;
                    err = nvs_get_str(_handle, key, buff, &required_size);
                    value = &buff;
                }
            }
            break;
        case NVS_INT:
            param = "int";
            int32_t myint;
            err = nvs_get_i32(_handle, key, &myint);
            value = &myint;
            break;
        case NVS_BOOL:
            param = "bool";
            uint8_t mybool;
            err = nvs_get_u8(_handle, key, &mybool);
            value = &mybool;
            break;
        default:
            ESP_LOGE(__func__, "The value type attempting to write on NVS does not exist!\n");
            return defaultValue;
    }

    if (err)
    {
        ESP_LOGE(__func__, "nvs_get_%s fail: %s %s", param, key, esp_err_to_name(err));
        return defaultValue;
    }
    else
    {
        return value;
    }
}

char *SingletonNVS::getString(const char *key, char *defaultValue)
{
    void *mystr;
    mystr = getParam(key, NVS_STR, &defaultValue);

    return *((char **)mystr);
}

int32_t SingletonNVS::getInt(const char *key, int32_t defaultValue)
{
    void *myint;
    myint = getParam(key, NVS_INT, &defaultValue);

    return *((int32_t *)myint);
}

uint8_t SingletonNVS::getBool(const char *key, uint8_t defaultValue)
{
    void *mybool;
    mybool = getParam(key, NVS_BOOL, &defaultValue);

    return *((uint8_t *)mybool);
}