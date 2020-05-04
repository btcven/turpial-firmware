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

#include <cstring>

#include <driver/uart.h>
#include <esp_log.h>
#include "freertos/queue.h"

#include "Websocket.h"

namespace radio {

enum class State {
    Length,
    Payload,
    Finished,
};

static const char* TAG = "Radio";

static QueueHandle_t g_uart0_queue;
static std::uint8_t g_rx_buf[CONFIG_RADIO_RX_BUF_SIZE];
static std::uint8_t g_len = 0;

static void eventLoop(void* data)
{
    std::size_t bytes_read = 0;
    std::size_t total_len = 0;
    int cnt = 0;
    State state = State::Length;

    while (1) {
        switch (state) {
            case State::Length:
                ESP_LOGI(TAG, "reading length");
                {
                    ESP_LOGI(TAG, "oh no");
                    cnt = uart_read_bytes(CONFIG_RADIO_UART, &g_len, sizeof(g_len),
                                          portMAX_DELAY);
                    ESP_LOGI(TAG, "read");
                    if (cnt != sizeof(g_len) || g_len == 0) {
                        ESP_LOGI(TAG, "true");
                        // We didn't read a valid length, so do it again.
                        state = State::Length;
                    } else {
                        ESP_LOGI(TAG, "false");
                        // Go to the read payload state
                        total_len = static_cast<std::size_t>(g_len);
                        state = State::Payload;
                    }
                }
                break;

            case State::Payload:
                ESP_LOGI(TAG, "reading payload");
                {
                    std::size_t remaining = total_len - bytes_read;
                    ESP_LOGI(TAG, "total len = %d, bytes_read = %d, remaining = %d",
                          (size_t)total_len, bytes_read, remaining);
                    cnt = uart_read_bytes(CONFIG_RADIO_UART,
                                          g_rx_buf + bytes_read, remaining,
                                          portMAX_DELAY);
                    bytes_read += cnt;
                    if (bytes_read < total_len) {
                        state = State::Payload;
                    } else {
                        // We're done
                        state = State::Finished;
                    }
                }
                break;

            case State::Finished:
                ESP_LOGI(TAG, "finished websocket is in charge!");
                {
                    Websocket& ws = Websocket::getInstance();
                    ws.websocketRadioRx(g_rx_buf, total_len);
                    state = State::Length;
                    // Reset buffer
                    std::memset(g_rx_buf, 0, sizeof(g_rx_buf));
                    bytes_read = 0;
                    cnt = 0;
                    total_len = 0;
                }
                break;
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

esp_err_t init()
{
    esp_err_t err;

    ESP_LOGI(TAG, "Initializing radio");

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

    BaseType_t ret = xTaskCreatePinnedToCore(eventLoop, "radio", 8192, NULL,
                                             5, NULL, tskNO_AFFINITY);
    if (ret != pdPASS) {
        return ESP_FAIL;
    }

    return ESP_OK;
}


} // namespace radio
