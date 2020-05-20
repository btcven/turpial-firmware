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
 * @file Websocket.h
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */


#ifndef HTTP_WEBSOCKET_H
#define HTTP_WEBSOCKET_H

#include "Hex.h"
#include "Serialization.h"
#include <cJSON.h>
#include <esp_https_server.h>

#include "Message.h"
#include <Radio.h>
#include <cbor.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdint.h>
#include <string>
#include <vector>



/**
 * @brief Customer data structure
 */
struct client_data_t {
    chat_id_t shaUID; /**< Customer id */
    int timestamp;    /**< Time the connection was made */
    bool is_alive;    /**< Indicates if the connection is alive or not */
    int fd;           /**< Connection identifier */
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
    httpd_ws_frame_t ws_pkt;
};

/**
 * @brief UID of incoming messages
 */
struct uid_message_t {
    chat_id_t from_uid; /**< UID who's sending the message */
    chat_id_t to_uid;   /**< UID of who's receiving the message */
};

class Websocket
{
public:
    Websocket(Websocket const&) = delete; // Copy construct
    Websocket(Websocket&&) = delete;      // Move construct

    Websocket& operator=(Websocket const&) = delete; // Copy assign
    Websocket& operator=(Websocket&&) = delete;      // Move assign
    void initRadioSerialLine(void);

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


    /**
     * @brief   Handles data received from radio UART
     *
     * @param[in] buffer Received data.
     * @param[in] length Length of buffer.
     */
    void websocketRadioRx(const std::uint8_t* buffer, std::size_t length);

private:
    /**
    * @brief check active customers
    *
    */
    Websocket();


    std::vector<client_data_t> m_client; // array of connected clients

    httpd_handle_t m_server; // http_esp_server connection handle
    httpd_req_t* req_handler;

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
    esp_err_t async_send(httpd_handle_t handle, int fd, httpd_ws_frame_t &ws_pkt);

    /**
     * @brief validate and obtain the uid of who sends the message and who will receive it
     * @param  payload data sent by the client
     * @param  uid_receiving  variable where the fromUID and toUID of the message will be stored
     * @return esp_err_t
     */
    esp_err_t messageRecipient(std::uint8_t* payload, uid_message_t* uid_receiving);

    /**
     * @brief Verify that the data received is correct and send the message
     * @param  client_uid contains the uid to which the message will be sent
     * @param  ws_pkt  message sent by the client and that will be returned by the socket
     * @return esp_err_t
     */
    esp_err_t sendWsData(uid_message_t client_uid, httpd_ws_frame_t ws_pkt, bool uart);

    /**
     * @brief send pong messages to verify connected clients
     */
    void pong(httpd_req_t* req);

    /**
     * @brief send encrypted message by radio
     * @param ws_pkt frame that delivers the socket this contains the data that the client sent
     */
    esp_err_t sendUart(httpd_ws_frame_t ws_pkt);

    void checkMessageType(httpd_ws_frame_t ws_pkt, bool uart);
};


#endif // HTTP_SOCKET_H
