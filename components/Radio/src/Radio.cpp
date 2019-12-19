/**
 * @file Radio.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-12-08
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "Radio.h"

#include <driver/uart.h>
#include <esp_log.h>

#include "TxBuffer.h"

#include "defaults.h"

namespace radio {

static const char* TAG = "UART";

Radio::Radio()
    : Task("UART", 4096, 5)
{
}

void Radio::run(void* data)
{
    ESP_LOGD(TAG, "UART start");

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .use_ref_tick = false,
    };

    ESP_LOGD(TAG, "Configuring UART parameters");
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));

    ESP_LOGD(TAG, "Configuring UART pins");
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, RAD_TX_PIN, RAD_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    const int uart_buffer_size = 1024;
    QueueHandle_t uart_queue;

    ESP_LOGD(TAG, "Installing UART driver");
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, uart_buffer_size, 0, 10, &uart_queue, 0));

    TxBuffer& tx_buffer = TxBuffer::getInstance();

    std::size_t buf_size = 0;

    while (true) {
        // Get a packet of MTU-value bytes.
        std::uint8_t* byte_buf = tx_buffer.receive(&buf_size);

        ESP_LOG_BUFFER_HEXDUMP(TAG, byte_buf, TXBUFFER_MTU, ESP_LOG_INFO);

        // This function blocks until all bytes have been sent
        // or copied to the TX FIFO.
        uart_write_bytes(UART_NUM_2, reinterpret_cast<char*>(byte_buf), buf_size);
    }
}

} // namespace radio
