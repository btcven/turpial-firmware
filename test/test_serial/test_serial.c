
/**
 * @file test_nvs.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-12-13
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include <esp_log.h>


/**
 * @brief 
 * Test UART
 * it try to show how to work in a proper way with uart ports
 * platformio test -e wemosbat -f test_serial
 *  
 */

#define ECHO_TEST_TXD  (GPIO_NUM_21)
#define ECHO_TEST_RXD  (GPIO_NUM_22)
#define ECHO_TEST_RTS  (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS  (UART_PIN_NO_CHANGE)

#define BUF_SIZE (1024)

static void echo_task()
{
    /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_2, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Configure a temporary buffer for the incoming data
    uint8_t *data = (uint8_t *) malloc(BUF_SIZE);
    const char* dta = "Hola mundo";
    while (1) {
        
        // Read data from the UART
        uart_write_bytes(UART_NUM_2, (const char *) dta, 10);
        int len = uart_read_bytes(UART_NUM_2, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        ESP_LOGI("llEGO DATA","***************************%d  ",len);
        // Write data back to the UART
        //uart_write_bytes(UART_NUM_1, (const char *) data, len);
    }
}

void app_main()
{
    xTaskCreate(echo_task, "uart_echo_task", 1024, NULL, 10, NULL);
}
