/**
 * Copyright 2020 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** 
 * @file UserButton.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#include "UserButton.h"
#include "defaults.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include <esp_log.h>
namespace hmi {

static xQueueHandle gpio_evt_queue = nullptr;

static void IRAM_ATTR interruptIsrHandler(void* arg)
{
    xQueueSendFromISR(gpio_evt_queue, arg, NULL);
}


UserButton::UserButton()
{
}

/**
 * @brief this is a public interface to interact with external user 
 * let us configure any Button as a Input to have multiple functionalities
 * 
 * @param user_button 
 * @param active 
 * @param fnClick callback to handle click event
 * @param fn2Click callback to handle double click event
 * @param fnLongClick callback to handle long click event
 */
void UserButton::init(gpio_num_t user_button,
    bool active,
    callbackFunction fnClick,
    callbackFunction fn2Click,
    callbackFunction fnLongClick)
{
    Button* btn = new Button(user_button, true);

    btn->_gpio_btn = user_button;
    active ? btn->m_button_pressed = LOW : btn->m_button_pressed = HIGH;

    btn->attachClick(fnClick);
    btn->attachDoubleClick(fn2Click);
    btn->attachLongClick(fnLongClick);

    const int ESP_INTR_FLAG_DEFAULT = 0;

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    //interrupt of rising edge
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = (1ULL << btn->_gpio_btn); //GPIO_INPUT_PIN_SEL;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    //change gpio interrupt type
    gpio_set_intr_type(btn->_gpio_btn, GPIO_INTR_ANYEDGE);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(btn->_gpio_btn, interruptIsrHandler, &btn->_gpio_btn);
    gpio_evt_queue = xQueueCreate(10, sizeof(gpio_num_t));

    static Interrupt g_int_task;
    g_int_task.start(btn);
}


Interrupt::Interrupt()
    : Task("Interrupt", 2000, 5)
{
}

/**
 * @brief Interrupt class is implementing run virtual method from Task class
 * to has ineer loop task, also we are using the same loop to handle state machine
 * to debounce input button.
 * The task first blocks waiting for a communications event inside firs do-while loop. 
 * The block time is relatively short. 
 * the second do-while block waiting for some states to decode the button input.
 * The method run is getting an instance of Button class as a parameter 
 * 
 * @param task_data 
 */

void Interrupt::run(void* task_data)
{
    Button* btn = static_cast<Button*>(task_data);
    unsigned long time_now;
    if (btn->doubleClickFunc != NULL) {
    }
    gpio_num_t io_pin = GPIO_NUM_MAX;
    while (true) {
        do {
            time_now = esp_timer_get_time();
            if (xQueueReceive(gpio_evt_queue, &io_pin, portMAX_DELAY)) {
                if (btn->m_state == 6) //restart the loop
                    btn->reset();
            }
        } while (uxQueueMessagesWaiting(gpio_evt_queue));

        do {
            time_now = esp_timer_get_time();
            btn->tick(gpio_get_level(btn->_gpio_btn) == btn->m_button_pressed, time_now);
        } while ((btn->m_state != 0 || btn->m_state == 2 || btn->m_state == 1)); // to ensure the state machine is able to detect long click and release the resource in right time
    }
}

} // namespace hmi
