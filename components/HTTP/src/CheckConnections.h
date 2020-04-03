/**
 * @file CheckConnections.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-04-03
 * 
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 * 
 */


#include "Task.h"
#include "Websocket.h"


#ifndef HTTP_CONECTIONS_H
#define HTTP_CONECTIONS_H

class CheckConnections : public Task
{
public:
    CheckConnections();
    virtual void run(void* task_data);
};


#endif // HTTP_CONECTIONS_H
