/**
 * @file HumanInterface.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-12-18
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#ifndef HMI_HUMAN_INTERFACE_H
#define HMI_HUMAN_INTERFACE_H

#include "driver/gpio.h"
#include <cstdint>
#include <esp_err.h>

#include "Task.h"
#include "UserButton.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


namespace hmi {
#define LOW false
#define HIGH true

class UserButton;
class Interrupt;

typedef void (*callbackFunction)(void);

typedef void (*parameterizedCallbackFunction)(void*);

static enum states {
    BTN_RELEASE_0,
    BTN_PRESS_0,
    BTN_RELEASE_1,
    BTN_PRESS_1,
    BTN_END_PROCESS,
} state;

class Button //: Task
{
public:
    Button();
    Button(gpio_num_t gpio, int active, bool pullupActive = true);
    //virtual void run(void* arg);

    // attach functions that will be called when button was pressed in the
    // specified way.
    void attachClick(callbackFunction newFunction);
    void attachClick(parameterizedCallbackFunction newFunction, void* parameter);
    void attachDoubleClick(callbackFunction newFunction);
    void attachDoubleClick(parameterizedCallbackFunction newFunction, void* parameter);
    void attachLongClick(callbackFunction newFunction);
    void attachLongClick(parameterizedCallbackFunction newFunction, void* parameter);

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
    bool _updated_level;                    // hardware gpio number.
    unsigned int _debounceTicks = 200;      // number of ticks for debounce times.
    unsigned long _clickTicks = 250;        // number of ticks that have to pass by
                                            // before a click is detected.
    unsigned long _long_press_ticks = 1500; // number of ticks that have to pass by
                                            // before a long button press is detected

    int _buttonPressed;
    bool _isLongPressed = false;
    long int _timeout = 500;

    // These variables that hold information across the upcoming tick calls.
    // They are initialized once on program start and are updated every time the
    // tick function is called.
    int _state = 0;
    unsigned long _startTime; // will be set in state 1
    unsigned long _stopTime;  // will be set in state 2

    // These variables will hold functions acting as event source.
    callbackFunction _clickFunc = NULL;
    parameterizedCallbackFunction _paramClickFunc = NULL;
    void* _clickFuncParam = NULL;

    callbackFunction _doubleClickFunc = NULL;
    parameterizedCallbackFunction _paramDoubleClickFunc = NULL;
    void* _doubleClickFuncParam = NULL;


    callbackFunction _longClickFunc = NULL;
    parameterizedCallbackFunction _paramLongClickFunc = NULL;
    void* _longClickFuncParam = NULL;

};
} // namespace hmi

#endif // HMI_H