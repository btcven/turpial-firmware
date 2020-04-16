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
#include <esp_log.h>
#include <Radio.h>

/**
 * @brief Chat ID
 */
typedef std::uint8_t chat_id_t[32];

/**
 * @brief Value for unspecified chat ID
 */
extern chat_id_t chat_id_unspecified;

/**
 * @brief Customer data structure
 */
struct client_data_t {
    chat_id_t shaUID; /**< Customer id */
    int timestamp; /**< Time the connection was made */
    bool is_alive; /**< Indicates if the connection is alive or not */
    int fd; /**< Connection identifier */
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
 * @brief UID of incoming messages
 */
struct uid_message_t {
    chat_id_t from_uid; /**< UID who's sending the message */
    chat_id_t to_uid; /**< UID of who's receving the message */
};


class Websocket {
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

private:
   /**
    * @brief check active customers
    *
    */
    Websocket();


    std::vector<client_data_t> m_client; // array of connected clients

    httpd_req_t* req_handler; // http_esp_server connection handler

    radio::Radio* radio_task;

    /**
     * @brief get message type
     *
     * @param[in] payload data sent by the client
     *
     * @return
     * -  0: when the message type is handshakey
     * -  1: when the message type is message
     * -  2: when the message type is status
     * -  4: when the message type is action
     * - -1: when the message type is invalid
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
