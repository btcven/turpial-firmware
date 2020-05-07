/**
 * @file Credentials.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief
 * @version 0.1
 * @date 2020-04-27
 *
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 *
 */

#include "Credentials.h"
#include <esp_log.h>
#include "Storage.h"

#include <cstring>
#include <iostream>

static const char* TAG = "Credentials";
namespace credentials {

esp_err_t setInitialCredentials()
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

    char user_name[MAX_USER_NAME_LENGTH];
    char user_password[MAX_USER_PASSWORD_LENGTH];
    size_t ul = MAX_USER_NAME_LENGTH;
    size_t pl = MAX_USER_PASSWORD_LENGTH;
    err = app_nvs.getString(USER_NAME_KEY, user_name, &ul);
    if (err != ESP_OK) {
        ESP_LOGI(TAG, "There is no username in nvs, going to set it");
        err = app_nvs.setString(USER_NAME_KEY, DEFAUL_USERNAME);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "There are errors , was not possible to save in NVS the username");
            return ESP_FAIL;
        }
        err = app_nvs.setString(USER_PASSWORD_KEY, DEFAULT_USER_PASSWORD);
        if (err != ESP_OK) {
            ESP_LOGI(TAG, "Error trying to save default password");
            return ESP_FAIL;
        }
    } else {
        err = app_nvs.getString(USER_PASSWORD_KEY, user_password, &pl);
        if (err != ESP_OK) {
            err = app_nvs.setString(USER_PASSWORD_KEY, DEFAULT_USER_PASSWORD);
            if (err != ESP_OK) {
                return ESP_FAIL;
            }
        }
        ESP_LOGI(TAG, "\nUsername: %s, Password: %s --> inside eeprom", &user_name[0], &user_password[0]);
    }

    return ESP_OK;
}

esp_err_t getCredentials(store_credentials_t* user_credentials)
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

    size_t username_size = MAX_USER_NAME_LENGTH;
    err = app_nvs.getString(USER_NAME_KEY, user_credentials->nvs_username, &username_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "error getting username");
        return ESP_FAIL;
    }

    size_t password_size = MAX_USER_PASSWORD_LENGTH;
    err = app_nvs.getString(USER_PASSWORD_KEY, user_credentials->nvs_password, &password_size);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "error getting password");
        return ESP_FAIL;
    }
    return ESP_OK;
}

bool credentialCompare(const char* a, const char* b)
{
    return strcmp(a, b) == 0;
}


esp_err_t saveNewCredentials(store_credentials_t credentials)
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

    err = app_nvs.setString(USER_NAME_KEY, credentials.nvs_username);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error the save new username");
        return ESP_FAIL;
    }

    err = app_nvs.setString(USER_PASSWORD_KEY, credentials.nvs_password);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error the save new password");
        return ESP_FAIL;
    }

    return err;
}
} // namespace credentials
