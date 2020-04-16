/**
 * @file RESTServer.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-03-10
 * 
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 * 
 */
#ifndef RESTSERVER_H
#define RESTSERVER_H

#include "HttpServer.h"
#include <cstdint>
#include <cstring>

namespace rest_server {
/**
 * @brief this function aims to setting up the api rest end points,
 * start the server amd initialize Radio module serial communication
 */
void start_server();
} // namespace rest_server

#endif /* RESTSERVER_H */
