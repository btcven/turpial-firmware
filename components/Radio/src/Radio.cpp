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

#include "freertos/queue.h"
#include <driver/uart.h>
#include <esp_log.h>
#include <string.h>



#define FTDI_PORT UART_NUM_0
#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

namespace radio {

#if RAD_ENABLED == true

static QueueHandle_t uart0_queue;
static const char* TAG = "UART";

Radio::Radio()
    : Task("UART", 4096, 5)
{
}

void Radio::init() 
{
    ESP_LOGD(TAG, "Initializing UART");
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
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));

    ESP_LOGD(TAG, "Configuring UART pins");
   // ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, RAD_TX_PIN, RAD_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    //const int uart_buffer_size = (1024 * 2);
    //QueueHandle_t uart_queue;

    ESP_LOGD(TAG, "Installing UART driver");
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, BUF_SIZE * 2, BUF_SIZE * 2, 10, &uart0_queue, 0));

}

void Radio::run(void* data)
{
    uart_event_t event;
    size_t buffered_size;
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            bzero(dtmp, RD_BUF_SIZE);
            ESP_LOGI(TAG, "uart[%d] event:", FTDI_PORT);
            switch(event.type) {
                //Event of UART receving data
                /*We'd better handler data event fast, there would be much more data events than
                other types of events. If we take too much time on data event, the queue might
                be full.*/
                case UART_DATA:
                    ESP_LOGI(TAG, "[UART DATA]: %d", event.size);
                    uart_read_bytes(FTDI_PORT, dtmp, event.size, portMAX_DELAY);
                    ESP_LOGI(TAG, "[DATA EVT]:");
                    uart_write_bytes(FTDI_PORT, (const char*) dtmp, event.size);
                    break;
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow");
                    // If fifo overflow happened, you should consider adding flow control for your application.
                    // The ISR has already reset the rx FIFO,
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(FTDI_PORT);
                    xQueueReset(uart0_queue);
                    break;
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
                    // If buffer full happened, you should consider encreasing your buffer size
                    // As an example, we directly flush the rx buffer here in order to read more data.
                    uart_flush_input(FTDI_PORT);
                    xQueueReset(uart0_queue);
                    break;
                //Event of UART RX break detected
                case UART_BREAK:
                    ESP_LOGI(TAG, "uart rx break");
                    break;
                //Event of UART parity check error
                case UART_PARITY_ERR:
                    ESP_LOGI(TAG, "uart parity error");
                    break;
                //Event of UART frame error
                case UART_FRAME_ERR:
                    ESP_LOGI(TAG, "uart frame error");
                    break;
                default:
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
            }
        }
    }
    free(dtmp);
    dtmp = NULL;
    vTaskDelete(NULL);
}

#endif

} // namespace radio