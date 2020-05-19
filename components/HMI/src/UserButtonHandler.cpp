
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
 * @file UserButtonHandler.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#include "UserButtonHandler.h"

#include "defaults.h"
#include <esp_log.h>


#define LED_R DEFAULT_RGB_LED_R
#define LED_G DEFAULT_RGB_LED_G
#define LED_B DEFAULT_RGB_LED_B

#define RGB_GPIO_OUTPUT_PIN_SEL ((1ULL << LED_R) | (1ULL << LED_G) | (1ULL << LED_B))

static bool toggle_r = false;
static bool toggle_g = false;
static bool toggle_b = false;

void UserButtonHandler::initRGBPort(void)
{
    gpio_config_t io_conf;
    // //disable interrupt
    io_conf.intr_type = static_cast<gpio_int_type_t>(GPIO_PIN_INTR_DISABLE);
    // //set as output mode
    io_conf.mode = GPIO_MODE_OUTPUT;
    // //bit mask of the pins that you want to set,e.g.GPIO0/2/4
    io_conf.pin_bit_mask = RGB_GPIO_OUTPUT_PIN_SEL;
    // //disable pull-down mode
    io_conf.pull_down_en = static_cast<gpio_pulldown_t>(0);
    // //disable pull-up mode
    io_conf.pull_up_en = static_cast<gpio_pullup_t>(0);
    // //configure GPIO with the given settings
    gpio_config(&io_conf);
}

void UserButtonHandler::click(void)
{
    gpio_set_level(static_cast<gpio_num_t>(LED_R), toggle_r = !toggle_r);
}

void UserButtonHandler::doubleClick(void)
{
    gpio_set_level(static_cast<gpio_num_t>(LED_G), toggle_g = !toggle_g);
}

void UserButtonHandler::longClick(void)
{
    gpio_set_level(static_cast<gpio_num_t>(LED_B), toggle_b = !toggle_b);
}
