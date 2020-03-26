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
#include <esp_https_server.h>

#define HTTP_SSL_CONFIG()                                                                                                                                                                                                               \
    {                                                                                                                                                                                                                                   \
        .httpd = {                                                                                                                                                                                                                      \
            .task_priority = tskIDLE_PRIORITY + 5,                                                                                                                                                                                      \
            .stack_size = 10240,                                                                                                                                                                                                        \
            .core_id = tskNO_AFFINITY,                                                                                                                                                                                                  \
            .server_port = 0,                                                                                                                                                                                                           \
            .ctrl_port = 32768,                                                                                                                                                                                                         \
            .max_open_sockets = 4,                                                                                                                                                                                                      \
            .max_uri_handlers = 8,                                                                                                                                                                                                      \
            .max_resp_headers = 8,                                                                                                                                                                                                      \
            .backlog_conn = 5,                                                                                                                                                                                                          \
            .lru_purge_enable = true,                                                                                                                                                                                                   \
            .recv_wait_timeout = 5,                                                                                                                                                                                                     \
            .send_wait_timeout = 5,                                                                                                                                                                                                     \
            .global_user_ctx = NULL,                                                                                                                                                                                                    \
            .global_user_ctx_free_fn = NULL,                                                                                                                                                                                            \
            .global_transport_ctx = NULL,                                                                                                                                                                                               \
            .global_transport_ctx_free_fn = NULL,                                                                                                                                                                                       \
            .open_fn = NULL,                                                                                                                                                                                                            \
            .close_fn = NULL,                                                                                                                                                                                                           \
            .uri_match_fn = NULL},                                                                                                                                                                                                      \
        .cacert_pem = NULL, .cacert_len = 0, .client_verify_cert_pem = NULL, .client_verify_cert_len = 0, .prvtkey_pem = NULL, .prvtkey_len = 0, .transport_mode = HTTPD_SSL_TRANSPORT_SECURE, .port_secure = 443, .port_insecure = 80, \
    }

namespace http {

class HttpServer
{
public:
    HttpServer();

    void registerUri(const char* uri, httpd_method_t method, esp_err_t (*handler)(httpd_req_t* r), void* ctx, bool is_websocket);

private:
    httpd_handle_t m_server;
};

} // namespace http

#endif /* HTTP_HTTP_SERVER_H */
