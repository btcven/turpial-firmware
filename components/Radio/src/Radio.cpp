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

static const char* TAG = "UART";
static QueueHandle_t uart0_queue;

Radio::Radio()
    : Task("UART", 4096, 5)
{
}

void Radio::init(callbackFunction fn) 
{
   //The callback handler represent the function passed from any module that implement 
   //serial communication and want to receive serial through callback
   callback_handler = fn;
   
    ESP_LOGD(TAG, "Initializing UART");
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 127,
        .use_ref_tick = false,
    };

    ESP_LOGD(TAG, "Configuring UART parameters");
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));

    ESP_LOGD(TAG, "Configuring UART pins");
   //ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, RAD_TX_PIN, RAD_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_LOGD(TAG, "Installing UART driver");
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, BUF_SIZE * 2, BUF_SIZE * 2, 10, &uart0_queue, 0));

}

void Radio::sendDataToRadio(void* data, size_t length)
{
    uart_write_bytes(FTDI_PORT, (const char*) data, length);
}

void Radio::run(void* data)
{   
    uart_event_t event;
    size_t buffered_size;
    const char* X = "testing";
    int s=7;
    uint8_t* dtmp = (uint8_t*) malloc(RD_BUF_SIZE);
    for(;;) {
        //Waiting for UART event.
        if(xQueueReceive(uart0_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
            bzero(dtmp, RD_BUF_SIZE);
            //ESP_LOGI(TAG, "uart[%d] event:", (int)FTDI_PORT);
            switch(event.type) {
                case UART_DATA:
                    uart_read_bytes(FTDI_PORT, dtmp, event.size, portMAX_DELAY);
                    callback_handler((void*)dtmp,(void*)event.size);
                    break;
                //Event of HW FIFO overflow detected
                case UART_FIFO_OVF:
                    ESP_LOGI(TAG, "hw fifo overflow"); 
                    uart_flush_input(FTDI_PORT);
                    xQueueReset(uart0_queue);
                    break;
                //Event of UART ring buffer full
                case UART_BUFFER_FULL:
                    ESP_LOGI(TAG, "ring buffer full");
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