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

namespace radio {

#if RAD_ENABLED == true

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

    const int uart_buffer_size = (1024 * 2);
    QueueHandle_t uart_queue;

    ESP_LOGD(TAG, "Installing UART driver");
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));

    while (true) {
        ESP_LOGD(TAG, "Reading bytes");

        std::uint8_t buffer[13];
        int n = uart_read_bytes(UART_NUM_2, buffer, 12, 1000);
        buffer[12] = '\0';
        if (n == 12) {
            ESP_LOGI(TAG, "Received, buffer = %s", reinterpret_cast<char*>(buffer));
        } else {
            ESP_LOGE(TAG, "Error, n = %d", n);
        }
    }
}

#endif

} // namespace radio