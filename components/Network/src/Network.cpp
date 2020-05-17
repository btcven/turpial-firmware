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
 * @ingroup     network
 * @{
 *
 * @file
 * @author      Locha Mesh Developers (contact@locha.io)
 * @brief       Network interfaces
 * @}
 */

#include "Network/Network.h"

#include <esp_log.h>

namespace network {

static const char* TAG = "Network";

Slip netif_radio;
WiFi netif_wifi;

esp_err_t init()
{
    esp_err_t err;

    // Initialize Wi-Fi network interface, this is where users will connect to
    // the Turpial device, this acts as an AP.
    err = g_wifi_netif.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't initialize Wi-Fi interface: %s",
            esp_err_to_name(err));
        return err;
    }

    err = g_wifi_netif.start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't initialize Wi-Fi interface: %s",
            esp_err_to_name(err));
        return err;
    }

    // Initialize radio network interface, this is an SLIP connection to the
    // radio module which will get us integrated/connected into the Locha Mesh
    // Network, in our case, for the Turpial, it's the CC1312 operating system
    // using RIOT-OS.
    err = netif_radio.start(CONFIG_RADIO_SLIP_UART, CONFIG_RADIO_SLIP_TX,
                            CONFIG_RADIO_SLIP_RX, CONFIG_RADIO_SLIP_BAUDRATE);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't initialize radio interface: %s",
            esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

} // namespace network
