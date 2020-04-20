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

#include "freertos/queue.h"

#include <cstring>

namespace radio {

static const char* TAG = "UART";

static QueueHandle_t g_uart0_queue;
static radio_rx_cb_t g_rx_cb;

static void _event_loop(void* data)
{
    uart_event_t event;
    std::uint8_t rx_buf[CONFIG_RADIO_RX_BUF_SIZE];

    while (1) {
        void *ev = reinterpret_cast<void*>(&event);
        portTickType wait_time = static_cast<portTickType>(portMAX_DELAY);

        // Wait for UART event
        if (xQueueReceive(g_uart0_queue, ev, wait_time)) {
            bzero(rx_buf, sizeof(rx_buf));
            switch(event.type) {
                case UART_DATA:
                    uart_read_bytes(CONFIG_RADIO_UART, rx_buf, event.size,
                                    portMAX_DELAY);
                    g_rx_cb(rx_buf, event.size);
                    break;

                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    uart_flush_input(CONFIG_RADIO_UART);
                    xQueueReset(g_uart0_queue);
                    break;

                case UART_BUFFER_FULL:
                    ESP_LOGW(TAG, "ring buffer full");
                    uart_flush_input(CONFIG_RADIO_UART);
                    xQueueReset(g_uart0_queue);
                    break;

                case UART_BREAK:
                    ESP_LOGE(TAG, "uart rx break");
                    break;

                case UART_PARITY_ERR:
                    ESP_LOGW(TAG, "uart parity error");
                    break;

                case UART_FRAME_ERR:
                    ESP_LOGE(TAG, "uart frame error");
                    break;

                default:
                    ESP_LOGE(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }
    vTaskDelete(NULL);
}

int write(const std::uint8_t *buffer, std::size_t length)
{
    int cnt;
    assert(length <= UINT8_MAX);

    ESP_LOGI(TAG, "length = %d", length);

    /* Write the length byte */
    std::uint8_t len_byte = static_cast<std::uint8_t>(length);
    cnt = uart_write_bytes(CONFIG_RADIO_UART,
                           reinterpret_cast<const char*>(&len_byte),
                           sizeof(len_byte));
    if (cnt < 0) {
        return -1;
    }

    /* Write the rest of the packet */
    cnt = uart_write_bytes(CONFIG_RADIO_UART,
                           reinterpret_cast<const char*>(buffer), length);
    if (cnt < 0) {
        return -1;
    } else {
        return cnt + 1;
    }
}

esp_err_t init(radio_rx_cb_t rx_cb)
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing radio");

    // Save the RX handler
    g_rx_cb = rx_cb;

    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 127,
        .use_ref_tick = false,
    };

    ESP_LOGI(TAG, "Configuring UART parameters");
    err = uart_param_config(CONFIG_RADIO_UART, &uart_config);
    if (err != ESP_OK) {
        return err;
    }

    ESP_LOGI(TAG, "Configuring UART pins");
    err = uart_set_pin(CONFIG_RADIO_UART, CONFIG_RADIO_TX, CONFIG_RADIO_RX,
                       UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (err != ESP_OK) {
        return err;
    }

    ESP_LOGI(TAG, "Installing UART driver");
    err = uart_driver_install(CONFIG_RADIO_UART, CONFIG_RADIO_RX_BUF_SIZE,
                              0, 10, &g_uart0_queue, 0);
    if (err != ESP_OK) {
        return err;
    }

    BaseType_t ret = xTaskCreatePinnedToCore(_event_loop, "radio", 1024, NULL,
                                             5, NULL, tskNO_AFFINITY);
    if (ret != pdPASS) {
        return ESP_FAIL;
    }

    return ESP_OK;
}


} // namespace radio
