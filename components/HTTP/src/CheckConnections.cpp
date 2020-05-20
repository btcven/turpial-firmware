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
 * @file CheckConnections.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#include "CheckConnections.h"
#include <iostream>

CheckConnections::CheckConnections() : Task("CheckConnections", 2000, 5)
{
}

void CheckConnections::run(void* task_data)
{
    Websocket& ws_instanse = Websocket::getInstance();

    TickType_t xLastWakeTime;
    const TickType_t xFrequency = 15000 / portTICK_PERIOD_MS;
    while (true) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        ws_instanse.checkConnection();
    }
}