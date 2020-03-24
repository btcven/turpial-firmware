/**
 * @file HttpServer.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-03-10
 * 
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 * 
 */
#include "HttpServer.h"
#include <esp_log.h>

static const char* TAG = "HttpServer";

namespace http {

HttpServer::HttpServer(std::uint16_t port)
    : m_server(nullptr)
{
    ESP_LOGI(TAG, "Starting Web server ...");

    // httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

    // ToDo:
    // - The cert. must be generated on demand.
    // - first boot: a fresh Cert. MUST BE generated.
    //
    extern const unsigned char cacert_pem_start[] asm("_binary_cacert_pem_start");
    extern const unsigned char cacert_pem_end[] asm("_binary_cacert_pem_end");
    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[] asm("_binary_prvtkey_pem_end");

    // The next httpd_ssl_config_t is only a temporal workaround.
    // As described in: https://github.com/espressif/esp-idf/pull/4981 | esp_https_server: fix HTTPD_SSL_CONFIG_DEFAULT (IDFGH-2948)
    httpd_ssl_config_t conf;

    conf.httpd = {
        .task_priority = tskIDLE_PRIORITY + 5,
        .stack_size = 10240,
        .core_id = tskNO_AFFINITY,
        .server_port = 0,
        .ctrl_port = 32768,
        .max_open_sockets = 4,
        .max_uri_handlers = 8,
        .max_resp_headers = 8,
        .backlog_conn = 5,
        .lru_purge_enable = true,
        .recv_wait_timeout = 5,
        .send_wait_timeout = 5,
        .global_user_ctx = NULL,
        .global_user_ctx_free_fn = NULL,
        .global_transport_ctx = NULL,
        .global_transport_ctx_free_fn = NULL,
        .open_fn = NULL,
        .close_fn = NULL,
        .uri_match_fn = NULL};

    conf.cacert_pem = cacert_pem_start;
    conf.cacert_len = cacert_pem_end - cacert_pem_start;
    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;
    conf.client_verify_cert_pem = NULL,
    conf.client_verify_cert_len = 0,
    conf.transport_mode = HTTPD_SSL_TRANSPORT_SECURE;
    conf.port_secure = 433;
    conf.port_insecure = 80;

    esp_err_t ret = httpd_ssl_start(&m_server, &conf);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting server!");
    }

    ESP_LOGI(TAG, "Start HTTPS server on port %u", port);
}

void HttpServer::registerUri(const char* uri, httpd_method_t method, esp_err_t (*handler)(httpd_req_t* r), void* ctx)
{
    ESP_LOGD(TAG, "Registering URI handler");
    httpd_uri_t uri_description = {
        .uri = uri,
        .method = method,
        .handler = handler,
        .user_ctx = ctx
    };
    httpd_register_uri_handler(m_server, &uri_description);
}

} // namespace http
