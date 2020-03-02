#ifndef HTTP_SERVER_HANDLER_H_
#define HTTP_SERVER_HANDLER_H_

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "WiFi.h"
#include <Battery.h>
#include <FuelGauge.h>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <stdint.h>
/**
 * @brief Handle path matching for an incoming HTTP request.
 */
static const char* TAG1 = "HTTP_SERVER";

class HttpServerHandler
{
public:
    HttpServerHandler();
    void helloWorld(void);
   
}; // HttpServerHandler

#endif /* COMPONENTS_CPP_UTILS_HTTPSERVER_H_ */