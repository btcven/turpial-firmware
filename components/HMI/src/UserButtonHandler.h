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