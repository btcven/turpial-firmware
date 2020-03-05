#ifndef USER_BUTTON_HANDLER_H_
#define USER_BUTTON_HANDLER_H_

#include "driver/gpio.h"
#include "driver/ledc.h"
#include <iostream>
/**
 * @brief Handler callback for different press button types
 */
class UserButtonHandler
{
public:
    static void click(void);
    static void doubleClick(void);
    static void longClick(void);
    static void initRGBPort(void);
}; // UserButtonHandler

#endif