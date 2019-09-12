/**
 * @file main.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <Arduino.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "hal/hardware.h"

// Defines for NVS
#define NVS_BOOL_NAMESPACE "booleans"
#define NVS_INT_NAMESPACE "integers"
#define NVS_STR_NAMESPACE "strings"
#define NVS_BOOL_KEY "bool"
#define NVS_INT_KEY "int"
#define NVS_STR_KEY "str"

/*
esp_err_t batteryTest() {
    // Put the code here for battery test
    esp_err_t err;
}

esp_err_t bleTest() {
    // Put the code here for BLE test
    esp_err_t err;
}

esp_err_t wifiTest() {
    // Put the code here for Wi-Fi test
    esp_err_t err;
}
*/

// NVS related functions
char *getStr(const char* K) {
    esp_err_t err;
    const char* TAG = "NVS";
    nvs_handle str_handle;
    char* mystr = NULL; 
    size_t required_size;

    // Open
    ESP_LOGD(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_open(NVS_STR_NAMESPACE, NVS_READONLY, &str_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "NVS opened\n");

        // Read
        ESP_LOGD(TAG, "Reading string from NVS...");
        //err = nvs_get_str(str_handle, K, NULL, &required_size);
        //str = (char*)malloc(required_size);
        err = nvs_get_str(str_handle, K, mystr, &required_size);
        switch (err) {
            case ESP_OK:
                ESP_LOGD(TAG, "NVS read done\n");
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGE(TAG, "The value is not initialized yet!\n");
                break;
            default :
                ESP_LOGE(TAG, "Error (%s) reading!\n", esp_err_to_name(err));
                break;
        }
        //ESP_ERROR_CHECK(err);

        // Close
        nvs_close(str_handle);
    }

    return mystr;
}


int getInt(const char* K) {
    esp_err_t err;
    const char* TAG = "NVS";
    nvs_handle int_handle;
    int16_t myint = 0; // value will default to 0, if not set yet in NVS

    // Open
    ESP_LOGD(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_open(NVS_INT_NAMESPACE, NVS_READONLY, &int_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "NVS opened\n");

        // Read
        ESP_LOGD(TAG, "Reading int from NVS...");
        err = nvs_get_i16(int_handle, K, &myint);
        switch (err) {
            case ESP_OK:
                ESP_LOGD(TAG, "NVS read done\n");
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGE(TAG, "The value is not initialized yet!\n");
                break;
            default :
                ESP_LOGE(TAG, "Error (%s) reading!\n", esp_err_to_name(err));
                break;
        }

        // Close
        nvs_close(int_handle);
    }

    return myint;
}


uint8_t getBool(const char *K) {
    esp_err_t err;
    const char* TAG = "NVS";
    nvs_handle bool_handle;
    uint8_t mybool = 0; // value will default to 0, if not set yet in NVS

    // Open
    ESP_LOGD(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_open(NVS_BOOL_NAMESPACE, NVS_READONLY, &bool_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "NVS opened\n");

        // Read
        ESP_LOGD(TAG, "Reading boolean from NVS...");
        err = nvs_get_u8(bool_handle, K, &mybool);
        switch (err) {
            case ESP_OK:
                ESP_LOGD(TAG, "NVS read done\n");
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                ESP_LOGE(TAG, "The value is not initialized yet!\n");
                break;
            default :
                ESP_LOGE(TAG, "Error (%s) reading!\n", esp_err_to_name(err));
                break;
        }

        // Close
        nvs_close(bool_handle);
    }

    return mybool;
}

void setStr(const char* K, const char* value) {
    esp_err_t err;
    const char* TAG = "NVS";
    nvs_handle str_handle;
    const char* var = "HOLAMUNDOOOO";

    // Open
    ESP_LOGD(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_open(NVS_STR_NAMESPACE, NVS_READWRITE, &str_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "NVS opened\n");

        // Write
        ESP_LOGD(TAG, "Updating string value in NVS...\n");
        err = nvs_set_str(str_handle, K, var);
        if (err == ESP_OK) {
            ESP_LOGD(TAG, "NVS write done\n");
        } else {
            ESP_LOGE(TAG, "An error occurred while writing the value to NVS");
        }
        // Commit written value.
        ESP_LOGD(TAG, "Committing updates in NVS\n");
        err = nvs_commit(str_handle);
        if (err == ESP_OK) {
            ESP_LOGD(TAG, "Commit done\n");
        } else {
            ESP_LOGE(TAG, "An error occurred while committing changes");
        }

        // Close
        nvs_close(str_handle);
    }
}

void setInt(const char* K, int16_t value) {
    esp_err_t err;
    const char* TAG = "NVS";
    nvs_handle int_handle;

    // Open
    ESP_LOGD(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_open(NVS_INT_NAMESPACE, NVS_READWRITE, &int_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "NVS opened\n");

        // Write
        printf("Updating int value in NVS ... ");
        err = nvs_set_i16(int_handle, K, value);
        if (err == ESP_OK) {
            ESP_LOGD(TAG, "NVS write done\n");
        } else {
            ESP_LOGE(TAG, "An error occurred while writing the value to NVS");
        }
        // Commit written value.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(int_handle);
        if (err == ESP_OK) {
            ESP_LOGD(TAG, "Commit done\n");
        } else {
            ESP_LOGE(TAG, "An error occurred while committing changes");
        }

        // Close
        nvs_close(int_handle);
    }
}

void setBool(const char *K, uint8_t value) {
    esp_err_t err;
    const char* TAG = "NVS";
    nvs_handle bool_handle;

    // Open
    ESP_LOGD(TAG, "Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_open(NVS_BOOL_NAMESPACE, NVS_READWRITE, &bool_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    } else {
        ESP_LOGI(TAG, "NVS opened\n");

        // Write
        printf("Updating boolean value in NVS ... ");
        err = nvs_set_u8(bool_handle, K, value);
        if (err == ESP_OK) {
            ESP_LOGD(TAG, "NVS write done\n");
        } else {
            ESP_LOGE(TAG, "An error occurred while writing the value to NVS");
        }
        // Commit written value.
        printf("Committing updates in NVS ... ");
        err = nvs_commit(bool_handle);
        if (err == ESP_OK) {
            ESP_LOGD(TAG, "Commit done\n");
        } else {
            ESP_LOGE(TAG, "An error occurred while committing changes");
        }

        // Close
        nvs_close(bool_handle);
    }
}

void nvsTest() {
    //nvs_flash_erase();
    esp_err_t err;

    // Initialize NVS
    err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );

    // =======================================================
    // Attempting to get boolean value from NVS
    bool mybool = getBool(NVS_BOOL_KEY);
    ESP_LOGI(__func__, "Boolean value from NVS: %i", mybool);
    // Attempting to update boleean value to NVS
    setBool(NVS_BOOL_KEY, 1);
    // =======================================================
    // Attempting to get int value from NVS
    int myint = getInt(NVS_INT_KEY);
    ESP_LOGI(__func__, "Int value from NVS: %i", myint);
    // Attempting to update int value to NVS
    setInt(NVS_INT_KEY, 2492);
    // =======================================================
    // Attempting to get const char array value from NVS
    char* mystr = getStr(NVS_STR_KEY);
    //char * mystr = NULL;
    if (mystr != NULL) {
        ESP_LOGD(__func__, "String value from NVS: %s", mystr);
    } else {
        ESP_LOGW(__func__, "NVS returned a NULL!");
    }
    //free(mystr);
    // Attempting to update int value to NVS
    setStr(NVS_STR_KEY, "HOLA");
}

void setup() {
    nvsTest();
}

void loop() {

}