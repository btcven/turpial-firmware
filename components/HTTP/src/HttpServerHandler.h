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


class HttpServerHandler
{
public:
    HttpServerHandler();
    static void readDeviceInfoHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse);
    static void setUpStaApHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse);
}; // HttpServerHandler

#endif 