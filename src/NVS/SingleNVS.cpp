#include "SingleNVS.h"
#include "GeneralUtils.h"
#include <nvs_flash.h>
//#include "nvs.h"
#include <iostream>
#include <stdlib.h>


static const char LOG_TAG[] = "CPPNVS";
// Define the static Singleton pointer
SingleNVS* SingleNVS::nvs_ = NULL;


SingleNVS::~SingleNVS() {
    ::nvs_close(_handle);
}

SingleNVS* SingleNVS::getInstance() {
   if (nvs_ == NULL) {
      nvs_ = new SingleNVS();
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

esp_err_t SingleNVS::begin() {
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

bool SingleNVS::open(const char *name, nvs_open_mode openMode) {
    if (_started)
    {
        return false;
    }
    _readOnly = (bool)!openMode;
    std::cout << "printing _readOnly  " << _readOnly << std::endl;

    esp_err_t err = nvs_open(name, openMode, &_handle);
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


void SingleNVS::close() {
    if (!_started)
    {
        return;
    }
    nvs_close(_handle);
    _started = false;
}


bool SingleNVS::clear() {
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
 * @brief Commit any work performed in the namespace.
 */
void SingleNVS::commit() {
	::nvs_commit(_handle);
} // commit



/**
 * @brief Erase ALL the keys in the namespace.
 */
void SingleNVS::erase() {
	::nvs_erase_all(_handle);
} // erase

/**
 * @brief Erase a specific key in the namespace.
 *
 * @param [in] key The key to erase from the namespace.
 */
void SingleNVS::erase(std::string key) {
	::nvs_erase_key(_handle, key.c_str());
} // erase


/**
 * @brief Retrieve a string/blob value by key.
 *
 * @param [in] key The key to read from the namespace.
 * @param [out] result The string read from the %NVS storage.
 */
int SingleNVS::get(std::string key, std::string* result, bool isBlob) {
	size_t length;
	if (isBlob) {
		esp_err_t rc = ::nvs_get_blob(_handle, key.c_str(), NULL, &length);
		if (rc != ESP_OK) {
			ESP_LOGI(LOG_TAG, "Error getting key: %i", rc);
			return rc;
		}
	} else {
		esp_err_t rc = ::nvs_get_str(_handle, key.c_str(), NULL, &length);
		if (rc != ESP_OK) {
			ESP_LOGI(LOG_TAG, "Error getting key: %i", rc);
			return rc;
		}
	}
	char* data = (char*) malloc(length);
	if (isBlob) {
		::nvs_get_blob(_handle, key.c_str(), data, &length);
	} else {
		::nvs_get_str(_handle, key.c_str(), data, &length);
	}
	*result = std::string(data);
	free(data);
	return ESP_OK;
} // get

int SingleNVS::get(std::string key, uint32_t& value) {
	return ::nvs_get_u32(_handle, key.c_str(), &value);
} // get - uint32_t


int SingleNVS::get(std::string key, uint8_t* result, size_t& length) {
	ESP_LOGD(LOG_TAG, ">> get: key: %s, blob: inputSize: %d", key.c_str(), length);
	esp_err_t rc = ::nvs_get_blob(_handle, key.c_str(), result, &length);
	if (rc != ESP_OK) {
		ESP_LOGD(LOG_TAG, "nvs_get_blob: %d", rc);
	}
	ESP_LOGD(LOG_TAG, "<< get: outputSize: %d", length);
	return rc;
} // get - blob



int SingleNVS::get(const char* key, void* result,size_t& length) {
    esp_err_t rc = nvs_get_blob(_handle, key, result, &length);
    std::cout << result << ":" << rc << std::endl;
    return rc;
}


/**
 * @brief Set the string/blob value by key.
 *
 * @param [in] key The key to set from the namespace.
 * @param [in] data The value to set for the key.
 */
void SingleNVS::set(std::string key, std::string data, bool isBlob) {
	ESP_LOGI(LOG_TAG, ">> set: key: %s, string: value=%s", key.c_str(), data.c_str());
	if (isBlob) {
		::nvs_set_blob(_handle, key.c_str(), data.data(), data.length());
	} else {
		::nvs_set_str(_handle, key.c_str(), data.c_str());
	}
	ESP_LOGI(LOG_TAG, "<< set");
} // set


void SingleNVS::set(std::string key, uint32_t value) {
	ESP_LOGD(LOG_TAG, ">> set: key: %s, u32: value=%d", key.c_str(), value);
	::nvs_set_u32(_handle, key.c_str(), value);
	ESP_LOGD(LOG_TAG, "<< set");
} // set - uint32_t


void SingleNVS::set(std::string key, uint8_t* data, size_t length) {
	ESP_LOGD(LOG_TAG, ">> set: key: %s, blob: length=%d", key.c_str(), length);
	esp_err_t rc = ::nvs_set_blob(_handle, key.c_str(), data, length);
	if (rc != ESP_OK) {
		ESP_LOGD(LOG_TAG, "nvs_get_blob: %d", rc);
	}
	ESP_LOGD(LOG_TAG, "<< set");
} // set (BLOB)


void SingleNVS::set(const char* key, char* data, size_t length) {
    std::cout << "vamos a guardar el blob" << std::endl;
    esp_err_t rc = ::nvs_set_blob(_handle, key, (void*)data, length);
    std::cout << rc << std::endl;
}

