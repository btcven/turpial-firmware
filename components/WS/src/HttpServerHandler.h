#ifndef HTTP_SERVER_HANDLER_H
#define HTTP_SERVER_HANDLER_H

#include "HttpServer.h"

namespace http_server_handler {

/**
 * @brief   Read device info callback
 *
 * Method: GET
 */
void readDeviceInfoHandler(HttpRequest *request, HttpResponse *response);

}

#endif /* HTTP_SERVER_HANDLER_H */
