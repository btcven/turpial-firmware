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
