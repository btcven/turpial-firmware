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
 * @file HumanInterface.h
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#ifndef HMI_HUMAN_INTERFACE_H
#define HMI_HUMAN_INTERFACE_H

#include "Task.h"
#include "UserButton.h"
#include "driver/gpio.h"
#include "freertos/queue.h"


namespace hmi {
#define LOW false
#define HIGH true

class UserButton;
class Interrupt;

typedef void (*callbackFunction)(void);

typedef void (*parameterizedCallbackFunction)(void*);

enum states_t {
    BTN_RELEASE_0,
    BTN_PRESS_0,
    BTN_RELEASE_1,
    BTN_PRESS_1,
    BTN_END_PROCESS,
};

class Button //: Task
{
public:
    Button();
    Button(gpio_num_t gpio, int active, bool pullup_active = true);

    // attach functions that will be called when button was pressed in the
    // specified way.
    void attachClick(callbackFunction newFunction);
    void attachDoubleClick(callbackFunction newFunction);
    void attachLongClick(callbackFunction newFunction);

    // ----- State machine functions -----

    /**
   * @brief Call this function every time the input level has changed.
   * Using this function no digital input pin is checked because the current
   * level is given by the parameter.
   */
    void tick(bool level, unsigned long time_now);

    bool isLongPressed();
    int getPressedTicks();
    void reset(void);

    gpio_num_t _gpio_btn;
    friend UserButton;
    friend Interrupt;

private:
    void gpioInit();
    bool m_updated_level;                    // hardware gpio number.
    unsigned int m_debounce_ticks = 200;     // number of ticks for debounce times.
    unsigned long m_click_ticks = 250;       // number of ticks that have to pass by
                                            // before a click is detected.
    unsigned long m_long_press_ticks = 1500; // number of ticks that have to pass by
                                            // before a long button press is detected

    int m_button_pressed;
    bool  m_is_long_pressed = false;
    long int m_timeout = 500;

    // These variables that hold information across the upcoming tick calls.
    // They are initialized once on program start and are updated every time the
    // tick function is called.
    int m_state = 0;
    unsigned long m_start_time; // will be set in state 1
    unsigned long m_stop_time;  // will be set in state 2

    // These variables will hold functions acting as event source.
    callbackFunction clickFunc = NULL;
    callbackFunction doubleClickFunc = NULL;
    callbackFunction longClickFunc = NULL;
};
} // namespace hmi

#endif // HMI_H
