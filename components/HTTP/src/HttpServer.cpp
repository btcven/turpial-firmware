#include "HttpServer.h"

#include <esp_log.h>

static const char* TAG = "HttpServer";

namespace http {

HttpServer::HttpServer(std::uint16_t port)
    : m_server(nullptr)
{
    ESP_LOGI(TAG, "Starting HTTP server on port %u", port);
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = port;
    esp_err_t err = httpd_start(&m_server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Coudln't initialize RESTServer");
    }
}

void HttpServer::registerUri(const char* uri, httpd_method_t method,
                             esp_err_t (* handler)(httpd_req_t *r), void* ctx)
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
