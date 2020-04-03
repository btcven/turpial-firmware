/**
 * @file Socket.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-04-03
 * 
 * @copyright Copyright (c) 2020 Locha Mesh Developers
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
    const TickType_t xFrequency = 1600;
    while (true) {
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        ws_instanse.checkConnection();
    }
}