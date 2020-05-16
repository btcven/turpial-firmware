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
 * @file RESTServer.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#include "RESTServer.h"

#include <cstring>

#include <cJSON.h>
#include <esp_log.h>

#include "Credentials.h"
#include "FuelGauge.h"
#include "HttpServer.h"
#include "Websocket.h"
#include "defaults.h"
#include <Storage.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <iostream>

#include <Network/WiFi.h>

#define REST_CHECK(expr, msg)   \
    do {                        \
        if (!(expr)) {          \
            ESP_LOGE(TAG, msg); \
        }                       \
    } while (false)

// 1 kB
#define SCRATCH_BUFSIZE (1024)

namespace rest_server {

typedef struct {
    char scratch[SCRATCH_BUFSIZE];
} rest_server_context_t;


static const char* TAG = "RESTServer";

esp_err_t receiveJson(httpd_req_t* req, cJSON** root)
{
    std::size_t total_len = req->content_len;

    // Verify that the request doesn't overflow our scratch buffer
    if (total_len >= SCRATCH_BUFSIZE) {
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
            return ESP_FAIL;
        }
        cur_len += received;
    }
    buf[total_len] = '\0';

    *root = cJSON_Parse(buf);
    return ESP_OK;
}

bool verifyCredentials(httpd_req_t* req)
{
    size_t user_len;
    size_t password_len;
    char* user;
    char* password;
    store_credentials_t user_credentials;
    storage::NVS app_nvs;

    user_len = httpd_req_get_hdr_value_len(req, "user") + 1;
    password_len = httpd_req_get_hdr_value_len(req, "password") + 1;

    if (user_len < 1 && password_len < 1) {
        return false;
    }

    // get_header_username
    user = (char*)malloc(user_len);
    httpd_req_get_hdr_value_str(req, "user", user, user_len);

    // get_header_password
    password = (char*)malloc(password_len);
    httpd_req_get_hdr_value_str(req, "password", password, password_len);

    credentials::getCredentials(&user_credentials);

    if (
        credentials::credentialCompare(user, user_credentials.nvs_username) &&
        credentials::credentialCompare(password, user_credentials.nvs_password)) {
        free(user);
        free(password);
        return true;

    } else {
        free(user);
        free(password);
        return false;
    }
}


void sendErrorResponse(httpd_req_t* req, const char* msg)
{
    ESP_LOGI(TAG, "Request failed");
    cJSON* root = cJSON_CreateObject();

    // Construct error response
    cJSON_AddStringToObject(root, "error", msg);

    char* payload = cJSON_Print(root);

    // Send response
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, payload);

    // Free memory allocated by cJSON
    free(payload);
    cJSON_Delete(root);
}

void sendOkResponse(httpd_req_t* req)
{
    ESP_LOGI(TAG, "Request successful");
    cJSON* root = cJSON_CreateObject();

    // Construct error response
    cJSON_AddStringToObject(root, "status", "OK");

    char* payload = cJSON_Print(root);

    // Send response
    httpd_resp_sendstr(req, payload);

    // Free memory allocated by cJSON
    free(payload);
    cJSON_Delete(root);
}

esp_err_t parseString(cJSON* item, void* dst, std::size_t max_len)
{
    const char* val = cJSON_GetStringValue(item);
    std::size_t len = strlen(val);
    if (val != NULL) {
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


esp_err_t systemInfoHandler(httpd_req_t* req)
{
    if (verifyCredentials(req) != 1) {
        ESP_LOGE(TAG, "fail credential verification");
        sendErrorResponse(req, "fail credential verification");
        return ESP_FAIL;
    }
    
    esc::FuelGauge& fuel_gauge = esc::FuelGauge::getInstance();
    network::WiFi& wifi = network::WiFi::getInstance();

    // Obtain response values
    std::uint16_t voltage = 0;
    REST_CHECK(fuel_gauge.voltage(&voltage) == ESP_OK, "Can't get voltage\n");
    std::int16_t avg_current = 0;
    REST_CHECK(fuel_gauge.avgCurrent(&avg_current) == ESP_OK, "Can't get avg. current\n");
    std::int16_t avg_power = 0;
    REST_CHECK(fuel_gauge.avgPower(&avg_power) == ESP_OK, "Can't get avg. power\n");
    std::uint16_t temp = 0;
    REST_CHECK(fuel_gauge.temperature(esc::TempMeasure::Internal, &temp) == ESP_OK,
        "Can't get internal temperature\n");
    std::size_t free_memory = 0;
    free_memory = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);


    wifi_config_t ap_config;
    wifi_config_t sta_config;

    // Read AP configuration
    char ap_ssid[32] = {};
    if (wifi.getApConfig(ap_config) == ESP_OK) {
        // Get AP SSID
        std::size_t ssid_len = 0;
        if (ap_config.ap.ssid_len != 0) {
            ssid_len = ap_config.ap.ssid_len;
        } else {
            ssid_len = std::strlen(reinterpret_cast<char*>(ap_config.ap.ssid));
        }

        std::memcpy(ap_ssid, ap_config.ap.ssid, ssid_len);
        ap_ssid[ssid_len] = '\0';
    }

    // Read STA configuration
    char sta_ssid[32] = {};
    if (wifi.getStaConfig(sta_config) == ESP_OK) {
        // Get STA SSID
        std::size_t ssid_len = std::strlen(reinterpret_cast<char*>(sta_config.sta.ssid));
        
        std::memcpy(sta_ssid, sta_config.ap.ssid, ssid_len);
        sta_ssid[ssid_len] = '\0';
    }

    bool sta_enabled = false;
    sta_enabled = wifi.isSta();

    // Construct JSON object
    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "device_name", CONFIG_DEVICE_NAME);
    cJSON_AddStringToObject(root, "device_version", CONFIG_DEVICE_VERSION);

    cJSON_AddNumberToObject(root, "voltage", voltage * 1.0);
    cJSON_AddNumberToObject(root, "avg_current", avg_current * 1.0);
    cJSON_AddNumberToObject(root, "avg_power", avg_power * 1.0);
    cJSON_AddNumberToObject(root, "avg_temp", avg_power * 1.0);
    cJSON_AddNumberToObject(root, "free_memory", free_memory * 1.0);

    cJSON* ap_root = cJSON_AddObjectToObject(root, "ap");
    cJSON_AddStringToObject(ap_root, "ssid", ap_ssid);

    cJSON* sta_root = cJSON_AddObjectToObject(root, "sta");
    cJSON_AddStringToObject(sta_root, "ssid", sta_ssid);
    cJSON_AddBoolToObject(sta_root, "enabled", sta_enabled);

    char* payload = cJSON_Print(root);

    // Send JSON response
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, payload);

    // Free memory allocated by cJSON
    free(payload);
    cJSON_Delete(root);

    return ESP_OK;
}

esp_err_t systemCredentialsHandler(httpd_req_t* req)
{

    bool result = verifyCredentials(req);
    if (result != true) {
        ESP_LOGE(TAG, "fail credential verification");
        sendErrorResponse(req, "fail credential verification");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    cJSON* req_root;
    if (receiveJson(req, &req_root) != ESP_OK) {
        sendErrorResponse(req, "Couldn't receive JSON data");
        return ESP_FAIL;
    }

    if (!cJSON_IsObject(req_root)) {
        sendErrorResponse(req, "Malformed JSON");
        return ESP_FAIL;
    }

    cJSON* username = cJSON_GetObjectItemCaseSensitive(req_root, "username");
    cJSON* password = cJSON_GetObjectItemCaseSensitive(req_root, "password");

    store_credentials_t new_credencial;
    if (cJSON_IsString(username) && cJSON_IsString(password)) {
        if (parseString(username, &new_credencial.nvs_username, MAX_USER_NAME_LENGTH) != ESP_OK) {
            sendErrorResponse(req, "username type invalid");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }

        if (parseString(password, &new_credencial.nvs_password, MAX_USER_PASSWORD_LENGTH) != ESP_OK) {
            sendErrorResponse(req, "password invalid");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }
    } else {
        sendErrorResponse(req, "invalid format");
        cJSON_Delete(req_root);
        return ESP_FAIL;
    }

   esp_err_t err = credentials::saveNewCredentials(new_credencial);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Error the save new credentials");
        cJSON_Delete(req_root);
        return err;
    }
    sendOkResponse(req);
    cJSON_Delete(req_root);
    return ESP_OK;
}


esp_err_t wifiApHandler(httpd_req_t* req)
{
    if (verifyCredentials(req) != 1) {
        ESP_LOGE(TAG, "fail credential verification");
        sendErrorResponse(req, "fail credential verification");
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "application/json");

    // Read JSON from the Request
    cJSON* req_root;
    if (receiveJson(req, &req_root) != ESP_OK) {
        sendErrorResponse(req, "Couldn't receive JSON data");
        return ESP_FAIL;
    }

    // Read current AP configuration
    network::WiFi& wifi = network::WiFi::getInstance();

    bool change_config = false;
    wifi_config_t config;
    wifi.getApConfig(config);

    if (!cJSON_IsObject(req_root)) {
        sendErrorResponse(req, "Malformed JSON");
        return ESP_FAIL;
    }

    cJSON* ssid = cJSON_GetObjectItemCaseSensitive(req_root, "ssid");
    if (cJSON_IsString(ssid)) {
        change_config = true;

        // SSID should be null terminated
        config.ap.ssid_len = 0;

        if (parseString(ssid, config.ap.ssid, 31) != ESP_OK) {
            sendErrorResponse(req, "Invalid AP SSID");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }
    }

    cJSON* password = cJSON_GetObjectItemCaseSensitive(req_root, "password");
    if (cJSON_IsString(password)) {
        change_config = true;

        if (parseString(password, config.ap.password, 63) != ESP_OK) {
            sendErrorResponse(req, "Invalid AP password");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }

        if (std::strlen(reinterpret_cast<char*>(config.ap.password)) < 8) {
            sendErrorResponse(req, "Password is less than 8 characters");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }
    }

    // Send OK response before changing any parameters, so we make sure it's
    // delivered.
    sendOkResponse(req);

    // Free the memory allocated by cJSON (for the request)
    cJSON_Delete(req_root);

    if (change_config) {
        ESP_LOGI(TAG, "Updating AP config");
        vTaskDelay(50 / portTICK_PERIOD_MS);
        REST_CHECK(wifi.setApConfig(config) == ESP_OK, "Couldn't update AP config.");
    }

    return ESP_OK;
}


esp_err_t wifiStaHandler(httpd_req_t* req)
{
    if (verifyCredentials(req) != 1) {
        ESP_LOGE(TAG, "fail credential verification");
        sendErrorResponse(req, "fail credential verification");
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");

    // Read JSON from the Request
    cJSON* req_root;
    if (receiveJson(req, &req_root) != ESP_OK) {
        sendErrorResponse(req, "Couldn't receive JSON data");
        return ESP_FAIL;
    }

    // Read current STA configuration

    wifi_config_t config;
    network::WiFi& wifi = network::WiFi::getInstance();
    wifi.getStaConfig(config);

    if (!cJSON_IsObject(req_root)) {
        sendErrorResponse(req, "Malformed JSON");
        return ESP_FAIL;
    }

    bool enable = false;
    cJSON* enable_root = cJSON_GetObjectItemCaseSensitive(req_root, "enable");
    if (cJSON_IsTrue(enable_root)) {
        enable = true;
    }


    cJSON* ssid = cJSON_GetObjectItemCaseSensitive(req_root, "ssid");
    bool change_config = false;
    if (cJSON_IsString(ssid)) {
        change_config = true;

        // SSID should be null terminated
        config.ap.ssid_len = 0;

        if (parseString(ssid, config.sta.ssid, 31) != ESP_OK) {
            sendErrorResponse(req, "Invalid STA SSID");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }
    }

    cJSON* password = cJSON_GetObjectItemCaseSensitive(req_root, "password");
    if (cJSON_IsString(password)) {
        change_config = true;

        if (parseString(password, config.sta.password, 63) != ESP_OK) {
            sendErrorResponse(req, "Invalid STA password");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }

        if (std::strlen(reinterpret_cast<char*>(config.sta.password)) < 8) {
            sendErrorResponse(req, "Password is less than 8 characters");
            cJSON_Delete(req_root);
            return ESP_FAIL;
        }
    }

    // Free the memory allocated by cJSON (for the request)
    cJSON_Delete(req_root);

    // Check if we need to enable STA
    bool is_sta = wifi.isSta();

    if (!is_sta && enable) {
        if (wifi.setMode(WIFI_MODE_APSTA) != ESP_OK) {
            sendErrorResponse(req, "Couldn't set mode to AP/STA.");
            return ESP_FAIL;
        }
    } else if (is_sta && !enable) {
        if (wifi.setMode(WIFI_MODE_AP) != ESP_OK) {
            sendErrorResponse(req, "Couldn't set mode to AP.");
            return ESP_FAIL;
        }
    }

    // Update config if new parameters are provided
    if (change_config) {
        ESP_LOGI(TAG, "Updating STA config");

        if (wifi.setStaConfig(config) != ESP_OK) {
            sendErrorResponse(req, "Couldn't update STA config.");
            return ESP_FAIL;
        }
    }

    // Finally send the response
    sendOkResponse(req);

    return ESP_OK;
}


esp_err_t websocketHandler(httpd_req_t* req)
{
    Websocket& ws_instance = Websocket::getInstance();

    httpd_ws_frame_t ws_pkt;
    memset(&ws_pkt, 0, sizeof(httpd_ws_frame_t));
    uint8_t buf[2000] = {0};
    ws_pkt.payload = buf;
    esp_err_t ret = httpd_ws_recv_frame(req, &ws_pkt, sizeof(buf));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "httpd_ws_recv_frame failed with %d", ret);
        return ret;
    }

    ESP_LOGI(TAG, "Packet type: %d", ws_pkt.type);
    ws_instance.onReceive(ws_pkt, req);
    return ret;
}

void start_server()
{
    http::HttpServer server_instance = http::HttpServer();

    server_instance.start();
    rest_server_context_t* ctx = reinterpret_cast<rest_server_context_t*>(malloc(sizeof(rest_server_context_t)));

    server_instance.registerUri("/system/info", HTTP_GET, systemInfoHandler, ctx, false);
    server_instance.registerUri("/system/credentials", HTTP_POST, systemCredentialsHandler, ctx, false);
    server_instance.registerUri("/wifi/sta", HTTP_POST, wifiStaHandler, ctx, false);
    server_instance.registerUri("/wifi/ap", HTTP_POST, wifiApHandler, ctx, false);
    server_instance.registerUri("/ws", HTTP_GET, websocketHandler, ctx, true);
}

} // namespace rest_server
