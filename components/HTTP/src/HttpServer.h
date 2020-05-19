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
 * @file HttpServer.h
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#ifndef HTTP_HTTP_SERVER_H
#define HTTP_HTTP_SERVER_H

#include <cstdint>
#include <esp_https_server.h>

namespace http {

class HttpServer
{
public:
    HttpServer();
    esp_err_t start();
    esp_err_t registerUri(const char* uri, httpd_method_t method, esp_err_t (*handler)(httpd_req_t* r), void* ctx, bool is_websocket);

private:
    httpd_handle_t m_server;
};

} // namespace http

#endif /* HTTP_HTTP_SERVER_H */
