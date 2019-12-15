/**
 * @file test_wifi_mode.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-12-11
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * platformio test -e featheresp32 -f test_nvs
 */

#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include <esp_log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

/**
 * Brief:
 * This test code shows how to configure gpio , how to use gpio interrupt  and how to use PWM.
 *
 * GPIO status:
 * GPIO25: output
 * GPIO26: output
 * GPIO33: output
 * GPIO27: output, 
 * GPIO32:  input, pulled up, interrupt from rising edge.  the GPIO32 is not able to work with interrupts
 *
 * Test:
 *
 */


#define LEDC_HS_TIMER LEDC_TIMER_0
#define LEDC_HS_MODE LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_CHANNEL LEDC_CHANNEL_0
#define LEDC_HS_CH1_CHANNEL LEDC_CHANNEL_1

#define LEDC_LS_TIMER LEDC_TIMER_1
#define LEDC_LS_MODE LEDC_LOW_SPEED_MODE
#define LEDC_LS_CH2_CHANNEL LEDC_CHANNEL_2

#define LEDC_TEST_CH_NUM (3)
#define LEDC_TEST_DUTY (4000)
#define LEDC_TEST_FADE_TIME (3000)
#define LEDC_TEST_DUTY_ZERO (0)

// to test with esp32 wrover kit we can use RGB GPIO
/* #define LED_R 2 
#define LED_G 0  
#define LED_B 4   */

#define LED_R 25  //34 wrong in schematic
#define LED_G 26  //35 wrong in schematic
#define LED_B 33  //4 

#define USER_BUTTON 21   //32 in schematic but not work with interrupt service  //works 27,25,23,21,26   //not work 32,34,36 this pins not work with interrupt handler
#define SYSOFF 27    //32 wrong in schematic not accept IRQ

#define GPIO_OUTPUT_PIN_SEL ((1ULL << SYSOFF))
#define GPIO_INPUT_PIN_SEL ((1ULL << USER_BUTTON))
#define ESP_INTR_FLAG_DEFAULT 0


int gpio_level;
bool validPress;
int count = 0;

bool toggle = false;
ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM];
static xQueueHandle gpio_evt_queue = NULL;


static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    gpio_intr_disable(static_cast<gpio_num_t>(USER_BUTTON));
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
    //vTaskDelay(1000 / portTICK_PERIOD_MS);
}

static void gpioInit(void)
{
    gpio_config_t io_conf;
    //disable interrupt
    io_conf.intr_type = static_cast<gpio_int_type_t>(GPIO_PIN_INTR_DISABLE);
    //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    //bit mask of the pins that you want to set,e.g.GPIO0/2/4
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    //disable pull-down mode
    io_conf.pull_down_en = static_cast<gpio_pulldown_t>(0);
    //disable pull-up mode
    io_conf.pull_up_en = static_cast<gpio_pullup_t>(0);
    //configure GPIO with the given settings
    gpio_config(&io_conf);

    //interrupt of rising edge
    io_conf.intr_type = static_cast<gpio_int_type_t>(GPIO_PIN_INTR_POSEDGE);
    //bit mask of the pins, use GPIO0/27/32 here
    io_conf.pin_bit_mask = GPIO_INPUT_PIN_SEL;
    //set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //enable pull-up mode
    io_conf.pull_up_en = static_cast<gpio_pullup_t>(1);
    gpio_config(&io_conf);

    //change gpio intrrupt type
    gpio_set_intr_type(static_cast<gpio_num_t>(USER_BUTTON), GPIO_INTR_ANYEDGE);

    //create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(static_cast<gpio_num_t>(USER_BUTTON), gpio_isr_handler, (void*)USER_BUTTON);
}


void pwmInit(void)
{
    int ch;

    ledc_timer_config_t ledc_timer;
    ledc_timer.duty_resolution = LEDC_TIMER_13_BIT; // resolution of PWM duty
    ledc_timer.freq_hz = 5000;                      // frequency of PWM signal
    ledc_timer.speed_mode = LEDC_HS_MODE;           // timer mode
    ledc_timer.timer_num = LEDC_HS_TIMER;           // timer index

    // Set configuration of timer0 for high speed channels
    ledc_timer_config(&ledc_timer);

    // Prepare and set configuration of timer1 for low speed channels
    ledc_timer.speed_mode = LEDC_LS_MODE;
    ledc_timer.timer_num = LEDC_LS_TIMER;
    ledc_timer_config(&ledc_timer);


    //ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM];
    ledc_channel[0].channel = LEDC_HS_CH0_CHANNEL;
    ledc_channel[0].duty = 0;
    ledc_channel[0].gpio_num = LED_R;
    ledc_channel[0].speed_mode = LEDC_HS_MODE;
    ledc_channel[0].hpoint = 0;
    ledc_channel[0].timer_sel = LEDC_HS_TIMER;

    ledc_channel[1].channel = LEDC_HS_CH0_CHANNEL;
    ledc_channel[1].duty = 0;
    ledc_channel[1].gpio_num = LED_G;
    ledc_channel[1].speed_mode = LEDC_HS_MODE;
    ledc_channel[1].hpoint = 0;
    ledc_channel[1].timer_sel = LEDC_HS_TIMER;

    ledc_channel[2].channel = LEDC_HS_CH0_CHANNEL;
    ledc_channel[2].duty = 0;
    ledc_channel[2].gpio_num = LED_B;
    ledc_channel[2].speed_mode = LEDC_HS_MODE;
    ledc_channel[2].hpoint = 0;
    ledc_channel[2].timer_sel = LEDC_HS_TIMER;

    // Set LED Controller with previously prepared configuration
    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
        ledc_channel_config(&ledc_channel[ch]);
    }

    // Initialize fade service.
    ledc_fade_func_install(0);
}


void gpio_task(void* arg)
{
    for (;;) {
        validPress = true; // innocent until proven guilty
        uint32_t io_num;
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            gpio_level = gpio_get_level(static_cast<gpio_num_t>(io_num));
            if (gpio_level != 0) {
                validPress = false;
            }
            if (validPress) {
                printf("*********************************GPIO[%d] valid button press received. Count: %d\n", io_num, count++);
                gpio_set_level(static_cast<gpio_num_t>(SYSOFF), toggle = !toggle);
            }
            gpio_intr_enable(static_cast<gpio_num_t>(USER_BUTTON));
        }
    }
}


void runTest(void) {
     int cnt = 0;
    int ch;
    gpioInit();
    pwmInit();
    //start gpio task
    xTaskCreate(gpio_task, "gpio_task", 2048, NULL, 10, NULL);
   
    while (1) {
        printf("1. LEDC fade up to duty = %d\n", LEDC_TEST_DUTY);
        for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            ledc_set_fade_with_time(ledc_channel[ch].speed_mode,
                ledc_channel[ch].channel, LEDC_TEST_DUTY, LEDC_TEST_FADE_TIME);
            ledc_fade_start(ledc_channel[ch].speed_mode,
                ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);
        }
        vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);

        printf("2. LEDC fade down to duty = 0\n");
        for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            ledc_set_fade_with_time(ledc_channel[ch].speed_mode,
                ledc_channel[ch].channel, 0, LEDC_TEST_FADE_TIME);
            ledc_fade_start(ledc_channel[ch].speed_mode,
                ledc_channel[ch].channel, LEDC_FADE_NO_WAIT);
        }
        vTaskDelay(LEDC_TEST_FADE_TIME / portTICK_PERIOD_MS);

        printf("3. LEDC set duty = %d without fade\n", LEDC_TEST_DUTY);
        for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, LEDC_TEST_DUTY);
            ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        printf("4. LEDC set duty = 0 without fade\n");
        for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, 0);
            ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

extern "C" void app_main()
{
    //vTaskDelay(2000);
    UNITY_BEGIN();
    RUN_TEST(runTest);
    UNITY_END();
 
}
