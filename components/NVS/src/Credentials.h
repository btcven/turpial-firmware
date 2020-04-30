/**
 * @file Credentials.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief
 * @version 0.1
 * @date 2020-04-27
 *
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 *
 */


#ifndef NVS_CREDENTIALS_H
#define NVS_CREDENTIALS_H

#include "Storage.h"
#include <iostream>
#include "defaults.h"
#include <cstring>


struct store_credentials_t 
{
  char nvs_username[MAX_USER_NAME_LENGTH];
  char nvs_password[MAX_USER_PASSWORD_LENGTH];
};

namespace credentials {
 
 esp_err_t setInitialCredentials();
 esp_err_t getCredentials(store_credentials_t* user_credentials);
 bool credentialCompare(const char* a, const char* b );
 esp_err_t saveNewCredentials(store_credentials_t credentials); 
 
 }

#endif //
