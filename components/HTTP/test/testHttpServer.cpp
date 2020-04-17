
#include "HttpServer.h"
#include "RESTServer.h"
#include "WiFi.h"
#include "defaults.h"
#include <unity.h>


TEST_CASE("start the server", "[https server]")
{
    http::HttpServer server_instance = http::HttpServer();

    esp_err_t ret server_instance.start();
    if (err != ESP_OK) TEST_FAIL();
}