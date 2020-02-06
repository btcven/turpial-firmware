/**
 * @file mt.c
 * @author Locha Mesh project developers (locha.io)
 * @brief
 * @version 0.1
 * @date 2019-12-08
 *
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "mt.h"

#include <string.h>

#include <driver/uart.h>
#include <esp_log.h>

#ifndef MT_UART_NUM
#define MT_UART_NUM (UART_NUM_2)
#endif

#ifndef MT_UART_TX
#define MT_UART_TX (17)
#endif

#ifndef MT_UART_RX
#define MT_UART_RX (16)
#endif

#ifndef MT_UART_BAUD_RATE
#define MT_UART_BAUD_RATE (115200)
#endif

#ifndef MT_TIMEOUT
#define MT_TIMEOUT (1000)
#endif

#ifndef MT_BUF_SIZE
#define MT_BUF_SIZE (253 + 2)
#endif

static const char* TAG = "mt";

static uint8_t mt_calc_fcs(const uint8_t *msg, uint8_t len) {
    uint8_t result = 0;
    while (len--) {
        result ^= *msg++;
    }
    return result;
}

static void mt_recv_task(void *pvParameters)
{
    static uint8_t recv_buf[MT_BUF_SIZE];
    (void)pvParameters;
    int ret;
    uint8_t sof;
    uint8_t fcs;

    for( ;; ) {
        ret = uart_read_bytes(MT_UART_NUM, &sof, 1, MT_TIMEOUT);
        if (ret == -1) {
            ESP_LOGD(TAG, "No data available");
            continue;
        }

        if (sof != 0xFE) {
            ESP_LOGD(TAG, "Not SOF");
            continue;
        }

        ret = uart_read_bytes(MT_UART_NUM, recv_buf, 3, MT_TIMEOUT);
        if (ret == -1) {
            ESP_LOGE(TAG, "Received SOF but no data available");
            uart_flush(MT_UART_NUM);
            continue;
        }

        /* Check length */
        if (recv_buf[0] > 250) {
            ESP_LOGE(TAG, "Message is too big!");
            uart_flush(MT_UART_NUM);
            continue;
        }

        ESP_LOG_BUFFER_HEXDUMP(TAG, recv_buf, 3, ESP_LOG_INFO);

        /* Read rest of message, including FCS */
        ret = uart_read_bytes(MT_UART_NUM, recv_buf + 3, recv_buf[0] + 1, MT_TIMEOUT);
        if (ret == -1) {
            ESP_LOGE(TAG, "Couldn't read message data!");
            continue;
        }

        /* Calculate FCS, doesn't include the last byte (which is the FCS to verify) */
        fcs = mt_calc_fcs(recv_buf, recv_buf[0] + 3);
        if (fcs != recv_buf[recv_buf[0] + 1]) {
            ESP_LOGE(TAG, "FCS doesn't match!!");
            continue;
        }

        ESP_LOGI(TAG, "shit it worked");

        /* TODO: process it */
    }
}

esp_err_t mt_init(void)
{
    uart_config_t uart_config = {
        .baud_rate = MT_UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .use_ref_tick = false,
    };

    ESP_LOGD(TAG, "Configuring UART parameters");
    ESP_ERROR_CHECK(uart_param_config(MT_UART_NUM, &uart_config));

    ESP_LOGD(TAG, "Configuring UART pins");
    ESP_ERROR_CHECK(uart_set_pin(MT_UART_NUM, MT_UART_TX, MT_UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    const int uart_buffer_size = MT_BUF_SIZE;
    QueueHandle_t uart_queue;

    ESP_LOGD(TAG, "Installing UART driver");
    ESP_ERROR_CHECK(uart_driver_install(MT_UART_NUM, uart_buffer_size, uart_buffer_size, 5, &uart_queue, 0));

    TaskHandle_t handle = NULL;
    xTaskCreate(mt_recv_task, "mt_recv", 1024, NULL, 15, handle);

    return ESP_OK;
}

esp_err_t mt_send(uint8_t cmd0, uint8_t cmd1, void *buf, uint8_t buf_len)
{
    uint8_t msg[MT_BUF_SIZE];

    if (buf_len > 250) {
        ESP_LOGE(TAG, "Message too big!");
        return ESP_FAIL;
    }

    msg[0] = 0xFE;
    msg[1] = buf_len;
    msg[2] = cmd0;
    msg[3] = cmd1;

    if (buf != NULL && buf_len != 0) {
        memcpy(msg + 4, buf, buf_len);
    }

    msg[buf_len + 1] = mt_calc_fcs(msg + 1, 3 + buf_len);

    /* SOF|LEN|CMD0|CMD1|BUF|FCS */
    if (uart_write_bytes(MT_UART_NUM, (const char *)msg, 4 + buf_len + 1) == -1) {
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t mt_send_ping(void)
{
    return mt_send(MT_RPC_CMD_SREQ | MT_RPC_SYS_SYS, MT_SYS_PING, NULL, 0);
}
