/**
 * @file HttpServer.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-03-10
 * 
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 * 
 */

#ifndef HTTP_HTTP_SERVER_H
#define HTTP_HTTP_SERVER_H

#include <cstdint>

#include <esp_http_server.h>

namespace http {

class HttpServer {
public:
    HttpServer(std::uint16_t port);

    void registerUri(const char* uri, httpd_method_t method,
                     esp_err_t (* handler)(httpd_req_t *r), void* ctx);

private:
    httpd_handle_t m_server;
};

} // namespace http

#endif /* HTTP_HTTP_SERVER_H */
