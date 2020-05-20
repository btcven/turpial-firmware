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
 * @brief       SLIP network interface
 * @}
 */

#include "Slip.h"

namespace network {

Slip::Slip()
    : m_netif(nullptr)
{
}

esp_err_t Slip::start(uart_port_t uart, int tx, int rx, uint32_t baudrate)
{
    esp_netif_slip_config_t config = {};

    // TODO: make this a parameter.
    IP6_ADDR(&config.addr,
        lwip_htonl(0xfd000000),
        lwip_htonl(0x00000000),
        lwip_htonl(0x00000000),
        lwip_htonl(0x000000001)
    );
    config.uart_dev = uart;
    config.uart_tx_pin = tx;
    config.uart_rx_pin = rx;
    config.uart_baud = baudrate;
    config.rx_filter = nullptr;
    config.rx_filter_ctx = nullptr;
    // TODO: verify this matches the RIOT RX buffer len
    config.rx_buffer_len = 1024;

    esp_netif_config_t cfg = ESP_NETIF_DEFAULT_SLIP();
    m_netif = esp_netif_new(&cfg);

    ESP_ERROR_CHECK(esp_netif_slip_set_params(m_netif, &config));

    void* slip_driver = esp_slip_new_netif_glue(m_netif);
    return esp_netif_attach(m_netif, slip_driver);
}

} // namespace network
