/**
 * @file Websocket.h
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

#include "Hex.h"
#include "Serialization.h"
#include <cJSON.h>
#include <esp_https_server.h>

#include <cbor.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include <string>
#include <vector>

/**
 * @brief customer structure
 */
struct client_data_t {
    std::uint8_t shaUID[32]; // customer id
    int timestamp;           // time the connection was made
    bool is_alive;           // indicates if the connection is alive or not
    int fd;                  // connection identifier
};

/**
 * @brief message type
 */
enum class WsMsgType {
    Handshake,
    Message,
    Status,
    Action
};

struct async_resp_arg_t {
    httpd_handle_t hd;
    int fd;
};

/**
 * @brief uid of incoming messages
 */
struct uid_message_t {
    std::uint8_t from_uid[32]; // uid who's sending the message
    std::uint8_t to_uid[32];   // id of who receives the message
};


class Websocket
{
public:
    Websocket(Websocket const&) = delete; // Copy construct
    Websocket(Websocket&&) = delete;      // Move construct

    Websocket& operator=(Websocket const&) = delete; // Copy assign
    Websocket& operator=(Websocket&&) = delete;      // Move assign

    /**
     * @brief Get the unique instance of the object
     *
     * @return Websocket&
     */
    static Websocket& getInstance()
    {
        static Websocket instance;
        return instance;
    }

    /**
     * @brief handles the data that comes from the websocket client
     *
     */
    void onReceive(httpd_ws_frame_t ws_pkt, httpd_req_t* req);

    /**
     * @brief check active customers
     *
     */
    void checkConnection();

private:
    /**
   * @brief check active customers
   *  
   */
    Websocket();


    std::vector<client_data_t> m_client; // array of connected clients

    httpd_req_t* req_handler; // http_esp_server connection handler

    /**
     * @brief  get message type
     * @param  payload data sent by the client
     * @return 
     * 0 : when the message type is handshakey 
     * 1 : when the message type is message
     * 2 : when the message type is status 
     * 4 : when the message type is action 
     * -1: when the message type is invalid
     */
    int getTypeMessage(std::uint8_t* payload);

    /**
     * @brief get the client data that comes from the websocket client 
     * @param  payload data sent by the client
     * @param  client  where all customer data will be saved
     * @return esp_err_t
     */
    esp_err_t getClientData(std::uint8_t* payload, client_data_t* client);

    /**
     * @brief function for sending asynchronous messages 
     * @param  payload data sent by the client
     * @param  client  where all customer data will be saved
     * @return esp_err_t
     */
    esp_err_t trigger_async_send(httpd_handle_t handle, int fd);

    /**
     * @brief validate and obtain the uid of who sends the message and who will receive it
     * @param  payload data sent by the client
     * @param  uid_receiving  variable where the fromUID and toUID of the message will be stored
     * @param  null_to_uid  variable assigned to the toUID if it reaches null
     * @return esp_err_t
     */
    esp_err_t messageRecipient(std::uint8_t* payload, uid_message_t* uid_receiving, const char* null_to_uid);
    /**
     * @brief Verify that the data received is correct and send the message
     * @param  client_uid contains the uid to which the message will be sent 
     * @param  ws_pkt  message sent by the client and that will be returned by the socket
     * @param  null_to_uid variable assigned to the toUID if it reaches null
     * @return esp_err_t
     */
    esp_err_t sendWsData(uid_message_t client_uid, httpd_ws_frame_t ws_pkt, const char* null_to_uid, bool uart);

    /**
     * @brief send pong messages to verify connected clients
     * 
     */
    void pong(httpd_req_t* req);

    /**
     * @brief send encrypted message by radio
     * @param ws_pkt frame that delivers the socket this contains the data that the client sent
     */
    esp_err_t sendUart(httpd_ws_frame_t ws_pkt);

    /**
     * @brief received encrypted message by radio
     * @param data_received encrypted message
     */
    esp_err_t receiveFromUart(std::uint8_t* data_received);
    void checkMessageType(httpd_ws_frame_t ws_pkt, bool uart);
};


#endif // HTTP_SOCKET_H
