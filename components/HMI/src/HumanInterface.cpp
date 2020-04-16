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

/* TODO: remove global */
static states_t state;

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
 * @param active_low 
 * @param pullup_active 
 */
Button::Button(gpio_num_t gpio, int active_low, bool pullup_active)
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

// save function for doubleClick event
void Button::attachDoubleClick(callbackFunction newFunction)
{
    _doubleClickFunc = newFunction;
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
 * @brief reset the state machine, the process function was finished
 * 
 */
void Button::reset(void)
{
    _state = 0; // restart.
    _start_time = 0;
    _stop_time = 0;
    _is_long_pressed = false;
    state = BTN_RELEASE_0;
}

/**
 * @brief Advance the finite state machine (FSM) using the given level.
 */

void Button::tick(bool active_level, unsigned long time_now)
{
    unsigned long now = time_now / 1000;
    static unsigned long press_time;

    // Implementation of the state machine
    if (_state == 0) { // waiting for menu pin being pressed.
        if (active_level) {
            _state = 1;        // step to state 1
            _start_time = now; // remember starting time
        }                      // if

    } else if (_state == 1) { // waiting for menu pin being released.

        press_time = (now - _start_time);
        // printf("state = 1  work time = %lu\n", press_time);
        if ((!active_level) && //si se libera
            (press_time < _debounce_ticks)) {
            // button was released to quickly so I assume some debouncing.
            // go back to state 0 without calling a function.
            _state = 0;

        } else if (!active_level) { //si se libra y es valido
            _state = 2;             // step to state 2
            _stop_time = now;       // remember stopping time
            printf("stop time : %lu", _stop_time);

        } else if ((active_level) &&
                   (press_time > _long_press_ticks)) {
            _is_long_pressed = true; // Keep track of long press state
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
        if (now - _stop_time > _timeout) {
            if (press_time > _click_ticks) {
                // this was only a single short click
                if (_clickFunc != NULL) {
                    _clickFunc();
                }
                _state = 0; // restart.
            }
        } else if (active_level) {
            _state = 3;
        }

    } else if (_state == 3) { // waiting for menu pin being released finally.
        // Stay here for at least _debounce_ticks because else we might end up in
        // state 1 if the button bounces for too long.
        if ((!active_level) &&
            ((unsigned long)(now - _start_time) > _debounce_ticks)) {
            // this was a 2 click sequence.
            if (_doubleClickFunc)
                _doubleClickFunc();
            _state = 0;       // restart.
            _stop_time = now; // remember stopping time
        }                     // if
    }
} // Button.tick()


} // namespace hmi
