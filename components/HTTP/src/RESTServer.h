/**
 * @file RESTServer.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-03-10
 * 
 * @copyright Copyright (c) 2020 Locha Mesh Developers
 * 
 */
#ifndef RESTSERVER_H
#define RESTSERVER_H

#include "HttpServer.h"
#include <cstdint>
#include <cstring>
#include <cJSON.h>

namespace rest_server {
/**
 * @brief this function aims to setting up the api rest end points,
 * start the server amd initialize Radio module serial communication
 */
void start_server();

/**
 * @brief Async send function, which we put into the httpd work queue
 * @param[in] req  https handler
 * 
 * @return esp_err_t
 */
esp_err_t websocketHandler(httpd_req_t* req);

/**
 * @brief   Setup STA configuration
 * @param[in] req https handler
 * 
 * return esp_err_t
 */
esp_err_t wifiStaHandler(httpd_req_t* req);

/**
 * @brief   Setup AP configuration
 * @param[in] req https handler
 * 
 * @return esp_err_t
 */

esp_err_t wifiApHandler(httpd_req_t* req);
/**
 * @brief credentials configuration
 * @param[in] req https handler
 * 
 * @return esp_err_t
 */
esp_err_t systemCredentialsHandler(httpd_req_t* req);

/**
 * @brief   Get device information
 * @param[in] req https handler
 * 
 * @return esp_err_t
 */
esp_err_t systemInfoHandler(httpd_req_t* req);


/**
 * @brief Parse string
 *
 * @param[in]  item    cJSON string.
 * @param[out] dst     Destination
 * @param[in]  max_len Maximum length of the field.
 *
 * @return esp_err_t
 */
esp_err_t parseString(cJSON* item, void* dst, std::size_t max_len);

/**
 * @brief  execute a successfull answer
 *
 * @param req https handler
 */
void sendOkResponse(httpd_req_t* req);

/**
 * @brief execute a fail answer
 *
 * @param[in] req  https handler
 * @param[in] msg The error message.
 */
void sendErrorResponse(httpd_req_t* req, const char* msg);

/**
 * 
 * @brief check credentials sent by client
 * @param[in] req  https handler
 * 
 * @return bool
 */
bool verifyCredentials(httpd_req_t* req);
/**
 * @brief 
 *
 * @param req  https handler
 * @param root cJSON object
 * @return esp_err_t
 */
esp_err_t receiveJson(httpd_req_t* req, cJSON** root);
} // namespace rest_server

#endif /* RESTSERVER_H */
