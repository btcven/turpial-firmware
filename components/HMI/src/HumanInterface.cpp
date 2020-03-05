/**
 * @file HumanInterface.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-12-18
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "HumanInterface.h"
#include <cstdint>
#include <cstring>
#include <esp_log.h>

namespace hmi {
static xQueueHandle gpio_evt_queue = nullptr;
// ----- Initialization and Default Values -----
/**
 * @brief Construct a new OneButton object but not (yet) initialize the IO pin.
 */
Button::Button()
{
    _gpio_btn = static_cast<gpio_num_t>(-1);
}

/**
 * @brief Construct a new Button:: Button object
 * 
 * @param gpio 
 * @param activeLow 
 * @param pullupActive 
 */
Button::Button(gpio_num_t gpio, int activeLow, bool pullupActive)
{
} //Button

// save function for click event
/**
 * @brief save function for  click event
 * 
 * @param newFunction callback function
 */
void Button::attachClick(callbackFunction newFunction)
{
    _clickFunc = newFunction;
} // attachClick

/**
 * @brief save function for parameterized click event
 * 
 * @param newFunction 
 * @param parameter 
 */
void Button::attachClick(parameterizedCallbackFunction newFunction, void* parameter)
{
    _paramClickFunc = newFunction;
    _clickFuncParam = parameter;
} // attachClick


// save function for doubleClick event
void Button::attachDoubleClick(callbackFunction newFunction)
{
    _doubleClickFunc = newFunction;
} // attachDoubleClick


/**
 * @brief save function for parameterized doubleClick event
 * 
 * @param newFunction 
 * @param parameter 
 */
void Button::attachDoubleClick(parameterizedCallbackFunction newFunction, void* parameter)
{
    _paramDoubleClickFunc = newFunction;
    _doubleClickFuncParam = parameter;
} // attachDoubleClick

/**
 * @brief save function for long click event
 * 
 * @param newFunction 
 */
void Button::attachLongClick(callbackFunction newFunction)
{
    _longClickFunc = newFunction;
} // attachClick


/**
 * @brief save function for parameterized long click event
 * 
 * @param newFunction 
 * @param parameter 
 */
void Button::attachLongClick(parameterizedCallbackFunction newFunction, void* parameter)
{
    _paramLongClickFunc = newFunction;
    _longClickFuncParam = parameter;
} // attachClick

/**
 * @brief reset the state machine, the proccess function was finished
 * 
 */
void Button::reset(void)
{
    _state = 0; // restart.
    _startTime = 0;
    _stopTime = 0;
    _isLongPressed = false;
    state = BTN_RELEASE_0;
}

/**
 * @brief Advance the finite state machine (FSM) using the given level.
 */

void Button::tick(bool activeLevel, unsigned long time_now)
{
    unsigned long now = time_now / 1000;
    static unsigned long press_time;

    // Implementation of the state machine
    if (_state == 0) { // waiting for menu pin being pressed.
        if (activeLevel) {
            _state = 1;       // step to state 1
            _startTime = now; // remember starting time
        }                     // if

    } else if (_state == 1) { // waiting for menu pin being released.

        press_time = (now - _startTime);
       // printf("state = 1  work time = %lu\n", press_time);
        if ((!activeLevel) && //si se libera
            (press_time < _debounceTicks)) {
            // button was released to quickly so I assume some debouncing.
            // go back to state 0 without calling a function.
            _state = 0;

        } else if (!activeLevel) { //si se libra y es valido
            _state = 2;            // step to state 2
            _stopTime = now;       // remember stopping time
            printf("stop time : %lu", _stopTime);

        } else if ((activeLevel) &&
                   (press_time > _long_press_ticks)) {
            _isLongPressed = true; // Keep track of long press state
            printf("LONG -------  pres_time : %lu", press_time);
            //attach callback here
            if (_longClickFunc != NULL) {
                _longClickFunc();
            }
            _state = 0; // step to state 6
        } else {
            //just waitting
        } // if

    } else if (_state == 2) {
        if (now - _stopTime > _timeout) {
            if (press_time > _clickTicks) {
                // this was only a single short click
                if (_clickFunc != NULL) {
                    _clickFunc();
                }
                _state = 0; // restart.
            }
        } else if (activeLevel) {
            _state = 3;
        }

    } else if (_state == 3) { // waiting for menu pin being released finally.
        // Stay here for at least _debounceTicks because else we might end up in
        // state 1 if the button bounces for too long.
        if ((!activeLevel) &&
            ((unsigned long)(now - _startTime) > _debounceTicks)) {
            // this was a 2 click sequence.
            if (_doubleClickFunc)
                _doubleClickFunc();
            if (_paramDoubleClickFunc)
                _paramDoubleClickFunc(_doubleClickFuncParam);
            _state = 0;      // restart.
            _stopTime = now; // remember stopping time
        }                    // if
    }
} // Button.tick()


} // namespace hmi