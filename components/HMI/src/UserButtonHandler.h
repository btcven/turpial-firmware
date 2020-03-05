#ifndef USER_BUTTON_HANDLER_H_
#define USER_BUTTON_HANDLER_H_

#include <iostream>
#include "driver/ledc.h"
#include "driver/gpio.h"
/**
 * @brief Handler callback for different press button types
 */
class UserButtonHandler
{
public:
    static gpio_config_t io_conf;

    static void click(void);
    static void doubleClick(void);
    static void longClick(void);
    static void initRGB(void);
}; // UserButtonHandler

#endif 