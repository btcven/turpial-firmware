
#include "HttpServer.h"
#include "RESTServer.h"
#include "WiFi.h"
#include "defaults.h"
#include <unity.h>


esp_err_t echoTest(httpd_req_t* req)
{
    return ESP_OK;
}


TEST_CASE("start the server", "[https server]")
{
    http::HttpServer server_instance = http::HttpServer();

    esp_err_t ret = server_instance.start();
    if (ret != ESP_OK) TEST_FAIL();
}


TEST_CASE("registering uri", "[https server]")
{
    http::HttpServer server_instance = http::HttpServer();

    esp_err_t ret = server_instance.registerUri("/test", HTTP_GET, echoTest, NULL, false);

    if (ret != ESP_OK) TEST_FAIL();
}