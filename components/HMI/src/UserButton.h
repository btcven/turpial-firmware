#ifndef HMI_USER_BUTTON_H
#define HMI_USER_BUTTON_H

#include "HumanInterface.h"
#include "Task.h"
#include "driver/gpio.h"

namespace hmi {
typedef void (*callbackFunction)(void);
typedef void (*parameterizedCallbackFunction)(void*);
class UserButton
{
public:
    UserButton();
    void init(gpio_num_t button,
        bool active,
        callbackFunction fnClick,
        callbackFunction fnDClick,
        callbackFunction fnLongClick);
};

class Interrupt : public Task
{
public:
    Interrupt();
    virtual void run(void* task_data);
};

} // namespace hmi

#endif