/**
 * @file Slip.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief
 * @version 0.1.1
 * @date 2020-05-13
 *
 * @copyright Copyright (c) 2020 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
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

}
