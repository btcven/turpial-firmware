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
    uint8_t shaUID[32];
    int timestamp;
    bool is_alive;
    int fd;
};


enum class WebsocketType {
    handshake,
    msg,
    status,
    action
};


struct async_resp_arg_t {
    httpd_handle_t hd;
    int fd;
};

struct uid_message_t {
    uint8_t from_uid[32];
    uint8_t to_uid[32];
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
    esp_err_t trigger_async_send(httpd_handle_t handle, int fd);
    // void ws_async_send(void* arg);
    std::vector<client_data_t> m_client;
    esp_err_t messageRecipient(uint8_t* payload, uid_message_t* uid_receiving);
};


#endif // HTTP_SOCKET_H
