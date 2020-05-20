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
 * @file UserButton.h
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#ifndef HMI_USER_BUTTON_H
#define HMI_USER_BUTTON_H

#include "HumanInterface.h"
#include "Task.h"
#include "driver/gpio.h"

namespace hmi {
typedef void (*callbackFunction)(void);
typedef void (*parameterizedCallbackFunction)(void*);

/**
 * @brief This is a public interface to setting User button up 
 * to have callback for each different operation
 * 
 */
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

/**
 * @brief this Interrupt class is handling all the process to decode the button when press it
 * 
 */
class Interrupt : public Task
{
public:
    Interrupt();
    virtual void run(void* task_data);
};

} // namespace hmi

#endif