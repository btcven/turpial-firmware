/*
 * HttpServer.h
 *
 *  Created on: Aug 30, 2017
 *      Author: kolban
 *
 * Implementation of an HTTP server for the ESP32.
 *
 */

#ifndef COMPONENTS_CPP_UTILS_HTTPSERVER_H_
#define COMPONENTS_CPP_UTILS_HTTPSERVER_H_

#include <stdint.h>

#include "Sema.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <regex>
#include <vector>

class HttpServerTask;

/**
 * @brief Handle path matching for an incoming HTTP request.
 */
class PathHandler
{
public:
    PathHandler(
        std::string method,             // The method in the request to be matched.
        std::string pathPattern,        // The pattern in the request to be matched
        void(*pWebServerRequestHandler) // The handler function to be invoked upon a match.
        (
            HttpRequest* pHttpRequest,
            HttpResponse* pHttpResponse));

    bool match(std::string method, std::string path); // Does the request method and pattern match?
    void invokePathHandler(HttpRequest* request, HttpResponse* response);

private:
    std::string m_method;
    bool m_isRegex;
    std::string m_textPattern;
    void (*m_pRequestHandler)(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse);
}; // PathHandler


class HttpServer
{
public:
    /* typedef std::map<int, WebSocket*> ws_list_t; */
    HttpServer();
    virtual ~HttpServer();

    void addPathHandler(
        std::string method,
        std::string pathExpr,
        void (*webServerRequestHandler)(
            HttpRequest* pHttpRequest,
            HttpResponse* pHttpResponse));

    uint32_t getClientTimeout();             // Get client's socket timeout
    uint16_t getPort();                      // Get the port on which the Http server is listening.
    void setClientTimeout(uint32_t timeout); // Set client's socket timeout
    void start(uint16_t portNumber);
    void stop(); // Stop a previously started server.


private:
    friend class HttpServerTask;
    friend class WebSocket;
    std::vector<PathHandler> m_pathHandlers; // Vector of path handlers.
    uint16_t m_portNumber;                   // Port number on which server is listening.
    Socket m_socket;
    uint32_t m_clientTimeout; // Default Timeout
    util::Semaphore m_semaphoreServerStarted;
}; // HttpServer

#endif /* COMPONENTS_CPP_UTILS_HTTPSERVER_H_ */
