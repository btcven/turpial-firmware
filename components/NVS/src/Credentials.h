/**
 * Copyright 2020 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** 
 * @file Credentials.h
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */


#ifndef NVS_CREDENTIALS_H
#define NVS_CREDENTIALS_H

#include "defaults.h"
#include "esp_system.h"

struct store_credentials_t {
    char nvs_username[MAX_USER_NAME_LENGTH];
    char nvs_password[MAX_USER_PASSWORD_LENGTH];
};

namespace credentials {
/**
 * @brief this adds the initial credentials in memory 
 * 
 */
esp_err_t setInitialCredentials();

/**
 * @brief get credentials stored in memory 
 * @param[out] user_credentials credentials returned
 * 
 * @return esp_err_t
 */
esp_err_t getCredentials(store_credentials_t* user_credentials);
/**
  * @brief this function verify credentials sent by the client with the credentials stored in memory 
  * @param[in] a parameter to compare
  * @param[in] b parameter to compare
  * 
  * @return bool
  */
bool credentialCompare(const char* a, const char* b);
/**
  * @brief this function save the username and password sent by client 
  * @param[in] credentials 
  * 
  * @return esp_err_t
  */
esp_err_t saveNewCredentials(store_credentials_t credentials);

} // namespace credentials

#endif //
