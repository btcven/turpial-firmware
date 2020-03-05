#ifndef USER_BUTTON_HANDLER_H_
#define USER_BUTTON_HANDLER_H_



#include <iostream>

/**
 * @brief Handler callback for different press button types
 */


class UserButtonHandler
{
public:
    UserButtonHandler();
    static void click(void);
    static void doubleClick();
    static void longClick();
}; // UserButtonHandler

#endif 