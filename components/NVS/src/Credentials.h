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

namespace credentials {
 
 esp_err_t getSetCredentials();
 esp_err_t test();
}

#endif //
