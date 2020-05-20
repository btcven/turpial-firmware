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
 * @file HttpServer.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#include "HttpServer.h"
#include <esp_log.h>

static const char* TAG = "HttpServer";

namespace http {

HttpServer::HttpServer()
    : m_server(nullptr)
{
}

esp_err_t HttpServer::start()
{
    ESP_LOGI(TAG, "Starting Web server ...");

    // ToDo:
    // - The cert. must be generated on demand.
    // - first boot: a fresh Cert. MUST BE generated.
    //
    extern const unsigned char cacert_pem_start[] asm("_binary_cacert_pem_start");
    extern const unsigned char cacert_pem_end[] asm("_binary_cacert_pem_end");
    extern const unsigned char prvtkey_pem_start[] asm("_binary_prvtkey_pem_start");
    extern const unsigned char prvtkey_pem_end[] asm("_binary_prvtkey_pem_end");

    httpd_ssl_config_t conf = HTTPD_SSL_CONFIG_DEFAULT();

    conf.cacert_pem = cacert_pem_start;
    conf.cacert_len = cacert_pem_end - cacert_pem_start;
    conf.prvtkey_pem = prvtkey_pem_start;
    conf.prvtkey_len = prvtkey_pem_end - prvtkey_pem_start;

    esp_err_t ret = httpd_ssl_start(&m_server, &conf);
    if (ESP_OK != ret) {
        ESP_LOGI(TAG, "Error starting server!");
    }

    ESP_LOGI(TAG, "Start HTTPS server");

    return ret;
}

esp_err_t HttpServer::registerUri(const char* uri, httpd_method_t method, esp_err_t (*handler)(httpd_req_t* r), void* ctx, bool is_websocket)
{
    ESP_LOGD(TAG, "Registering URI handler");
    httpd_uri_t uri_description = {
        .uri = uri,
        .method = method,
        .handler = handler,
        .user_ctx = ctx,
        .is_websocket = is_websocket};
    esp_err_t ret = httpd_register_uri_handler(m_server, &uri_description);

    return ret;
}

} // namespace http
