#include "Namespace.h"

#include "GeneralUtils.h"
#include <nvs_flash.h>
#include <iostream>
#include <cstdlib>


static const char LOG_TAG[] = "CPPNVS";

namespace nvs {

Namespace::Namespace()
    : _handle(0)
    , _isOpened(false) {}

Namespace::~Namespace() {
    if (_isOpened) {
        nvs_close(_handle);
    }
}

esp_err_t Namespace::open(const char* name, nvs_open_mode mode) {
    // A namespace is already opened in this instance.
    if (_isOpened) {
        return ESP_FAIL;
    }
    const auto mode_ = static_cast<nvs_open_mode>(mode);
    std::cout << mode_ << std::endl;
    const auto err = nvs_open(name, mode, &_handle);
    
    if (err != ESP_OK) {
        return err;
    } else {
        _isOpened = true;
        return err;
    }
}

void Namespace::close() {
    if (_isOpened)
    {
        nvs_close(_handle);
        _isOpened = false;
    }
}

esp_err_t Namespace::erase_all() {
    if (!_isOpened) {
        return ESP_FAIL;
    }

    return nvs_erase_all(_handle);
}

esp_err_t Namespace::erase_key(const char* key) {
    esp_err_t err;
	if (!_isOpened) {
        std::cout << "no esta abierto ahora ****************" << std:: endl;
        return ESP_FAIL;
    }
    std::cout << "Trying to delete Key:  ----> " << key <<std:: endl;
    err = nvs_erase_key(_handle, key);
    if (err != ESP_OK) {
        return err;
    }
    std::cout << "Doing Commit for erase key" << std:: endl;
    return nvs_commit(_handle);
}

esp_err_t Namespace::get_blob(const char* key, std::ostream& result) {
    if (!_isOpened) {
        return ESP_FAIL;
    }

    std::size_t length;
    auto err = nvs_get_blob(_handle, key, nullptr, &length);
    if (err != ESP_OK) {
        ESP_LOGE(__func__, "Can't get key \"%s\" blob length", key);
        return err;
    }

    auto buf = new char[length];
    auto out_data = reinterpret_cast<void*>(buf);
    err = nvs_get_blob(_handle, key, out_data, &length);

    if (err != ESP_OK) {
        delete[] buf;
        return err;
    }

    result.write(buf, length);
    delete[] buf;

    return ESP_OK;
}

esp_err_t Namespace::set_blob(const char* key, const void* data, const std::size_t length) {
    esp_err_t err;
    if (!_isOpened) {
        return ESP_FAIL;
    }
    err = nvs_set_blob(_handle, key, data, length);
    if(err != ESP_OK) {
        return err;
    }
    return nvs_commit(_handle);    
}

esp_err_t Namespace::commit() {
    if (!_isOpened) {
        return ESP_FAIL;
    }

	return nvs_commit(_handle);
}

esp_err_t begin() {
    // Initialize NVS
    ESP_LOGD(__func__, "Initializing NVS...\n");
    auto status = nvs_flash_init();

    if (status == ESP_OK) {
        ESP_LOGD(__func__,"NVS has been initialized correctly");
        return status;
    } else if (status == ESP_ERR_NVS_NO_FREE_PAGES || status == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        auto flash_err = nvs_flash_erase();
        if (flash_err != ESP_OK) {
            ESP_LOGE(__func__, "Couldn't erase NVS partition");
            return flash_err;
        }

        return nvs_flash_init();
    }

    return ESP_FAIL;
}

} // namespace nvs