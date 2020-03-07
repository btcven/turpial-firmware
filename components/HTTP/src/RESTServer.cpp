/**
 * @brief   HTTP Server
 */

#include "RESTServer.h"

#include <cJSON.h>
#include <esp_log.h>

#include "FuelGauge.h"
#include "HttpServer.h"
#include "WiFi.h"

#define REST_CHECK(expr, msg)                \
    do {                                     \
        if (!(expr)) {                       \
            ESP_LOGE(TAG, msg);              \
        }                                    \
    } while(false)

// 1 kB
#define SCRATCH_BUFSIZE (1024)

namespace rest_server {

typedef struct {
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;

static const char* TAG = "RESTServer";


void sendErrorResponse(httpd_req_t* req, const char* msg)
{
    cJSON* root = cJSON_CreateObject();

    // Construct error response
    cJSON_AddStringToObject(root, "error", "Content too long");

    char* payload = cJSON_Print(root);

    // Send response
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, payload);

    // Free memory allocated by cJSON
    free(payload);
    cJSON_Delete(root);
}

void sendOkResponse(httpd_req_t* req)
{
    cJSON* root = cJSON_CreateObject();

    // Construct error response
    cJSON_AddStringToObject(root, "status", "OK");

    char* payload = cJSON_Print(root);

    // Send response
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, payload);

    // Free memory allocated by cJSON
    free(payload);
    cJSON_Delete(root);
}

esp_err_t parseString(cJSON* item, void* dst, std::size_t max_len)
{
    if (cJSON_IsString(item)) {
        const char* val = cJSON_GetStringValue(item);
        std::size_t len = strlen(val);
        if (len <= max_len) {
            memcpy(dst, val, len + 1);
        } else {
            return ESP_FAIL;
        }
    } else {
        return ESP_FAIL;
    }

    return ESP_OK;
}

/**
 * @brief   Get device information
 */
esp_err_t getDeviceInfo(httpd_req_t* req)
{
    std::uint16_t voltage = 0;
    std::int16_t avg_current = 0;
    std::int16_t avg_power = 0;
    std::uint16_t temp = 0;
    std::size_t free_memory = 0;
    esc::FuelGauge& fuel_gauge = esc::FuelGauge::getInstance();

    // Obtain response values
    REST_CHECK(fuel_gauge.voltage(&voltage) == ESP_OK, "Can't get voltage\n");
    REST_CHECK(fuel_gauge.avgCurrent(&avg_current) == ESP_OK, "Can't get avg. current\n");
    REST_CHECK(fuel_gauge.avgPower(&avg_power) == ESP_OK, "Can't get avg. power\n");
    REST_CHECK(fuel_gauge.temperature(esc::TempMeasure::Internal, &temp) == ESP_OK,
               "Can't get internal temperature\n");
    free_memory = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);

    // Construct JSON object
    cJSON* root = cJSON_CreateObject();

    cJSON_AddNumberToObject(root, "voltage", voltage * 1.0);
    cJSON_AddNumberToObject(root, "avg_current", avg_current * 1.0);
    cJSON_AddNumberToObject(root, "avg_power", avg_power * 1.0);
    cJSON_AddNumberToObject(root, "avg_temp", avg_power * 1.0);
    cJSON_AddNumberToObject(root, "free_memory", free_memory * 1.0);

    char* payload = cJSON_Print(root);

    // Send JSON response
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, payload);

    // Free memory allocated by cJSON
    free(payload);
    cJSON_Delete(root);

    return ESP_OK;
}

/**
 * @brief   Setup STA/AP configuration
 */
esp_err_t setupStaAp(httpd_req_t *req)
{
    std::size_t total_len = req->content_len;

    httpd_resp_set_type(req, "application/json");

    // Verify that the request doesn't overflow our scratch buffer
    if (total_len >= SCRATCH_BUFSIZE) {
        sendErrorResponse(req, "Content too long");
        return ESP_FAIL;
    }

    // Buffer where we'll be reading
    char* buf = reinterpret_cast<rest_server_context_t*>(req->user_ctx)->scratch;

    // Read request content
    std::size_t cur_len = 0;
    std::size_t received = 0;
    while (cur_len < total_len) {
        received = httpd_req_recv(req, buf + cur_len, total_len);
        if (received <= 0) {
            sendErrorResponse(req, "Content receive data");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    cJSON* req_root = cJSON_Parse(buf);
    if (req_root != nullptr) {
        sendErrorResponse(req, "Malformed JSON");
        return ESP_FAIL;
    }

    wifi_config_t ap_config;
    wifi_config_t sta_config;

    bool change_ap_config = false;
    bool change_sta_config = false;

    cJSON* ap_root = cJSON_GetObjectItemCaseSensitive(req_root, "ap");
    cJSON* sta_root = cJSON_GetObjectItemCaseSensitive(req_root, "sta");

    if (cJSON_IsObject(ap_root)) {
        change_ap_config = true;

        cJSON* ap_ssid = cJSON_GetObjectItemCaseSensitive(ap_root, "ssid");
        cJSON* ap_password = cJSON_GetObjectItemCaseSensitive(ap_root, "password");

        // SSID should be null terminated
        ap_config.ap.ssid_len = 0;

        if (parseString(ap_ssid, ap_config.ap.ssid, 31) != ESP_OK) {
            sendErrorResponse(req, "Invalid AP configuration");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }

        if (parseString(ap_password, ap_config.ap.password, 63) != ESP_OK) {
            sendErrorResponse(req, "Invalid AP configuration");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }
    }

    if (cJSON_IsObject(sta_root)) {
        change_sta_config = true;

        cJSON* sta_ssid = cJSON_GetObjectItemCaseSensitive(sta_root, "ssid");
        cJSON* sta_password = cJSON_GetObjectItemCaseSensitive(sta_root, "password");

        if (parseString(sta_ssid, sta_config.sta.ssid, 31) != ESP_OK) {
            sendErrorResponse(req, "Invalid STA configuration");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }

        if (parseString(sta_password, sta_config.sta.password, 63) != ESP_OK) {
            sendErrorResponse(req, "Invalid STA configuration");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }
    }

    // Send OK response before changing any parameters, so we make sure it's
    // delivered.
    sendOkResponse(req);

    // Free the memory allocated by cJSON (for the request)
    cJSON_Delete(req_root);

    network::WiFi& wifi = network::WiFi::getInstance();
    if (change_ap_config) {
        ESP_LOGI(TAG, "Updating AP config");
        REST_CHECK(wifi.setApConfig(ap_config) == ESP_OK, "Couldn't update AP config.");
    }

    if (change_sta_config) {
        ESP_LOGI(TAG, "Updating STA config");
        REST_CHECK(wifi.setStaConfig(sta_config) == ESP_OK, "Couldn't update STA config.");
    }

    return ESP_OK;
}

esp_err_t setupCredentials(httpd_req_t *req)
{
    return ESP_OK;
}

void start_server(std::uint16_t port)
{
    // TODO(jeandudey): move port to defaults.h
    http::HttpServer http_server(port);

    rest_server_context_t *ctx = reinterpret_cast<rest_server_context_t*>(malloc(sizeof(rest_server_context_t)));

    http_server.registerUri("/get-device-info", HTTP_GET, getDeviceInfo, ctx);
    http_server.registerUri("/set-up-sta-ap", HTTP_POST, setupStaAp, ctx);
    http_server.registerUri("/set-up-credentials", HTTP_POST, setupCredentials, ctx);
}

} // namespace rest_server
