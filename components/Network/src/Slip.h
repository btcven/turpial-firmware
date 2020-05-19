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
 * @file Slip.h
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */


#ifndef NETWORK_SLIP_H
#define NETWORK_SLIP_H

#include <driver/uart.h>
#include <esp_system.h>
#include <esp_netif.h>
#include <esp_netif_slip.h>

namespace network {

/**
 * @brief   SLIP network interface
 */
class Slip {
public:
    /**
     * @brief   SLIP constructor
     *
     */
    Slip();

    /**
     * @brief   Initializes the SLIP ESP-NETIF interface.
     * 
     * @param[in] uart     The UART device to use.
     * @param[in] tx       SLIP TX pin.
     * @param[in] rx       SLIP RX pin.
     * @param[in] baudrate SLIP baudrate.
     *
     * @return
     *      - ESP_OK on success.
     *      - otherwise failure.
     */
    esp_err_t start(uart_port_t uart, int tx, int rx, uint32_t baudrate);

    /**
     * @brief   Get the underlying network interface
     *
     * @return
     *      - esp_netif_t pointer.
     *      - nullptr is interface hasn't been initialized
     */
    esp_netif_t *netif() {
        return m_netif;
    }

private:
    esp_netif_t *m_netif; /**< SLIP ESP-NETIF */
};

}

#endif // NETWOR_SLIP_H
