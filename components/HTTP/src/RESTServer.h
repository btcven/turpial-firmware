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

#include <cstdint>

namespace rest_server {

/**
 * @brief Start the request server in a given port
 * 
 * @param port 
 */
void start_server(std::uint16_t port);

} /* namespace restserver */


#endif /* RESTSERVER_H */
