/**
 * @file main.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-09-11
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <cstdio>
#include <memory>
#include <sstream>

#include "esp_log.h"
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "BLEPreferences.h"
#include "NVS.h"
#include "WiFi.h"

#include "defaults.h"


#include "HttpServer.h"


HttpServer httpServer;
 //Socket my_Socket;
//WebSocket myWebsocket(my_Socket);
//WebSocketHandler* myHandler = new WebSocketHandler();

//WebSocketHandler* myHandler;


static const char* TAG = "app_main";

esp_err_t getIsConfigured(bool& is_configured)
{
    esp_err_t err;

    storage::NVS app_nvs;
    err = app_nvs.open(NVS_APP_NAMESPACE, NVS_READWRITE);
    if (err != ESP_OK) {
        const char* err_str = esp_err_to_name(err);
        ESP_LOGE(TAG,
            "Couldn't open namespace \"%s\" (%s)",
            NVS_APP_NAMESPACE,
            err_str);
        return err;
    }

    err = app_nvs.get_bool(NVS_IS_CONFIGURED_KEY, is_configured);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        // Set is_configured to true on flash so on next init the config is
        // readed directly by the ESP-IDF Wi-Fi library component.
        err = app_nvs.set_bool(NVS_IS_CONFIGURED_KEY, true);
        if (err != ESP_OK) return err;
        err = app_nvs.commit();
        if (err != ESP_OK) return err;
        // Set the return variable to "false" to forcibly set the default
        // configuration
        is_configured = false;
    } else {
        return err;
    }

    return ESP_OK;
}

void helloWorldHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse)
{
 /*pHttpResponse->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
  pHttpResponse->addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "text/plain");
  pHttpResponse->sendData("Hello world");
  pHttpResponse->close();*/
}


void webSocketHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse)
{
      WebSocketHandler* myHandler = new WebSocketHandler();
    if (pHttpRequest->isWebsocket()) {
        ESP_LOGI("WEBSOCKETHANDLER----->", "******************encontramos un websocket");
         ESP_LOGD("WEBSOCKETHANDLER----->", "*DDDDDDDDDDDDDDDDDDDDDD*****************encontramos un websocket");
        WebSocket* myWebsocket = new WebSocket(pHttpRequest->getSocket());
       pHttpRequest->getWebSocket()->setHandler(myHandler);
        //WebSocket myWebsocket(pHttpRequest->getSocket());
       // WebSocketHandler* myHandler;
        //myWebsocket->setHandler(myHandler);
        //xTaskCreate(&reader_task, "readerTask", 10 * 1024, NULL, 5, NULL);
       
    }



}

extern "C" void app_main()
{
    esp_err_t err;

    err = storage::init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't initialize NVS, error (%s)", esp_err_to_name(err));
        return;
    }

    bool is_configured = false;
    err = getIsConfigured(is_configured);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
            "Couldn't get \"is_configured\" value (%s)",
            esp_err_to_name(err));
    }

    network::WiFi& wifi = network::WiFi::getInstance();
    err = wifi.init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't initalize Wi-Fi interface (%s)", esp_err_to_name(err));
        return;
    }

    if (!is_configured) {
        wifi.setMode(WIFI_MODE);

        network::APConfig ap_config = {
            .ssid = WAP_SSID,
            .password = WAP_PASS,
            .authmode = WAP_AUTHMODE,
            .max_conn = WAP_MAXCONN,
            .channel = WAP_CHANNEL,
        };
        wifi.setApConfig(ap_config);

        network::STAConfig sta_config = {
            .ssid = WST_SSID,
            .password = WST_PASS,
        };
        wifi.setStaConfig(sta_config);
    }

    err = wifi.start();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Couldn't start Wi-Fi, err = %s", esp_err_to_name(err));
        return;
    }

    /* ble::ServerParams server_params;
    server_params.device_name = "Turpial-1234";
    server_params.static_passkey = 123456;
    server_params.app_id = 0;
    ble_preferences::start(server_params); */


    /*  HttpServer* pHttpServer = new HttpServer();
    pHttpServer->addPathHandler(HttpRequest::HTTP_METHOD_GET, "/", helloWorldHandler);
    pHttpServer->start(80); */
    //mySocket.setHandler(myHandler);

    httpServer.addPathHandler(HttpRequest::HTTP_METHOD_GET, "/", webSocketHandler);
    httpServer.start(80, false);
  

}