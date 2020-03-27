/**
 * @file Socket.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-03-25
 * 
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 * 
 */


#ifndef HTTP_WEBSOCKET_H
#define HTTP_WEBSOCKET_H

#include <esp_https_server.h>
#include <stdint.h>
#include <string>
#include <vector>

#include <cJSON.h>

struct client_data_t {
    char* shaUID;
    int timestamp;
    bool is_alive;
    httpd_req_t* req;
};


enum class WebsocketType {
    handshake,
    msg,
    status,
    action
};

class Websocket
{
public:
    Websocket(Websocket const&) = delete; // Copy construct
    Websocket(Websocket&&) = delete;      // Move construct

    Websocket& operator=(Websocket const&) = delete; // Copy assign
    Websocket& operator=(Websocket&&) = delete;      // Move assign


    static Websocket& getInstance()
    {
        static Websocket instance;
        return instance;
    }

    void onReceive(httpd_ws_frame_t ws_pkt, httpd_req_t* req);

private:
    Websocket();
    int getTypeMessage(uint8_t* payload);
    esp_err_t getClientData(uint8_t* payload, client_data_t* client, httpd_req_t* req);
    std::vector<client_data_t> m_client;
};


#endif // HTTP_SOCKET_H
