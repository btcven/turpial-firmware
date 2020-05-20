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
 * @defgroup    network
 * @ingroup     network
 * @{
 *
 * @file
 * @author      Locha Mesh Developers (contact@locha.io)
 * @brief       Network interfaces
 */

#ifndef NETWORK_NETWORK_H
#define NETWORK_NETWORK_H

#include "Slip.h"
#include "WiFi.h"

namespace network {

extern Slip netif_radio; /**< SLIP network interface to the radio */
extern WiFi netif_wifi;  /**< Wi-Fi network interface */

/**
 * @brief   Initialize network interfaces
 *
 * @note Must be called only once!
 *
 * @return
 *      - ESP_OK: success.
 *      - (others): failure.
 */
esp_err_t init();

} // namespace network

#endif // NETWORK_NETWORK_H
/** @} */
