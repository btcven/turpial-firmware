#ifndef HTTP_SERVER_HANDLER_H_
#define HTTP_SERVER_HANDLER_H_

#include "HttpRequest.h"
#include "HttpResponse.h"
#include "WiFi.h"
#include <Battery.h>
#include <FuelGauge.h>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <stdint.h>
/**
 * @brief Handle path matching for an incoming HTTP request.
 */
static const char* TAG1 = "HTTP_SERVER";

class HttpServerHandler
{
public:
    HttpServerHandler();
    void helloWorld(void);
    // static void configureWstAp(std::map<std::string, std::string>& params)
    // {
    //     network::WiFi& wifi = network::WiFi::getInstance();
    //     std::map<std::string, std::string>::iterator iterator = params.find("type");

    //     if (iterator != params.end()) {
    //         if (iterator->second == "wst") {
    //             configureWst(params);
    //         } else if (iterator->second == "ap") {
    //             configureAp(params);
    //         }
    //     }
    // }

    // static void readDeviceInfoHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse)
    // {
    //     ostringstream s1;
    //     std::uint16_t voltage = 0;
    //     std::int16_t avg_current = 0;
    //     std::int16_t avg_power = 0;
    //     std::uint16_t temp = 0;
    //     esc::FuelGauge& fuel_gauge = esc::FuelGauge::getInstance();

    //     esp_err_t err = fuel_gauge.voltage(&voltage);
    //     if (err != ESP_OK) {
    //         ESP_LOGE(TAG1, "Can't get voltage, err = %s",
    //             esp_err_to_name(err));
    //     }

    //     err = fuel_gauge.avgCurrent(&avg_current);
    //     if (err != ESP_OK) {
    //         ESP_LOGE(TAG1, "Can't get avg. current, err = %s",
    //             esp_err_to_name(err));
    //     }

    //     err = fuel_gauge.avgPower(&avg_power);
    //     if (err != ESP_OK) {
    //         ESP_LOGE(TAG1, "Can't get avg. power, err = %s",
    //             esp_err_to_name(err));
    //     }

    //     err = fuel_gauge.temperature(esc::TempMeasure::Internal, &temp);
    //     if (err != ESP_OK) {
    //         ESP_LOGE(TAG1, "Can't get internal temperature, err = %s",
    //             esp_err_to_name(err));
    //     }

    //     ESP_LOGI("available memory ram--->>", "%d\n", heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
    //     pHttpResponse->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
    //     pHttpResponse->addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "application/json");

    //     s1 << "{";

    //     s1 << "\"voltage\":\"";
    //     s1 << voltage;
    //     s1 << "\",";

    //     s1 << "\"avg_current\":\"";
    //     s1 << avg_current;
    //     s1 << "\",";

    //     s1 << "\"temp\":\"";
    //     s1 << temp;
    //     s1 << "\",";

    //     s1 << "\"avg_power\":\"";
    //     s1 << avg_power;
    //     s1 << "\",";

    //     s1 << "\"free_memory\":\"";
    //     s1 << heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    //     s1 << "\"";

    //     s1 << "}";

    //     pHttpResponse->sendData(s1.str());
    //     pHttpResponse->close();
    // }

    // static void setUpWstApHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse)
    // {
    //     std::string body = pHttpRequest->getBody();
    //     std::istringstream ss(body);
    //     std::string token[3]; //length of json object, hoe many field there are
    //     std::map<std::string, std::string> json_map;
    //     int i = 0;
    //     size_t found;

    //     while (std::getline(ss, token[i], ',')) {
    //         //to remove open brace in json string packet
    //         if (i == 0) {
    //             found = token[i].find('{');
    //             if (found != std::string::npos) {
    //                 token[i].erase(std::remove(token[i].begin(), token[i].end(), '{'), token[i].end());
    //             }
    //         }

    //         size_t length = sizeof(token) / sizeof(*token);
    //         //to remove clode brace in json string packet
    //         if (i == length - 1) {
    //             found = token[i].find('}');
    //             if (found != std::string::npos) {
    //                 token[i].erase(std::remove(token[i].begin(), token[i].end(), '}'), token[i].end());
    //             }
    //         }
    //         //to remove double quote in key value pair
    //         found = token[i].find('\"');
    //         if (found != std::string::npos) {
    //             token[i].erase(std::remove(token[i].begin(), token[i].end(), '\"'), token[i].end());
    //         }
    //         //to remove tabs in key and value pair
    //         found = token[i].find('\t');
    //         if (found != std::string::npos) {
    //             token[i].erase(std::remove(token[i].begin(), token[i].end(), '\t'), token[i].end());
    //         }
    //         //to remove new line in key value pair
    //         found = token[i].find('\n');
    //         if (found != std::string::npos) {
    //             token[i].erase(std::remove(token[i].begin(), token[i].end(), '\n'), token[i].end());
    //         }

    //         std::istringstream stream(token[i]);
    //         std::string field[2];
    //         int j = 0;

    //         while (std::getline(stream, field[j], ':')) {
    //             j++;
    //         }
    //         json_map.insert(std::make_pair(field[0], field[1]));

    //         i++;
    //     }

    //     //this reply need to be send before to stop wifi adapter but in this moment the server is not able to send  a reply
    //     pHttpResponse->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
    //     //return response to the remote client
    //     pHttpResponse->sendData("{\"status\":\"OK\"}");

    //     configureWstAp(json_map);
    // }

    // static void configureWst(std::map<std::string, std::string>& params)
    // {
    //     network::WiFi& wifi = network::WiFi::getInstance();
    //     wifi_config_t config;
    //     wifi.getStaConfig(config);


    //     std::map<std::string, std::string>::iterator iterator = params.find("ssid_name");
    //     if (iterator != params.end()) {
    //         if (iterator->second.length() >= 31) {
    //             ESP_LOGW(TAG1, "STA SSID value is higher than 31 characters, trimming");
    //             std::memcpy(config.sta.ssid, iterator->second.c_str(), 31);
    //             config.sta.ssid[31] = '\0';
    //         } else if (iterator->second.length() == 0) {
    //             ESP_LOGW(TAG1, "AP SSID value is empty, not changing");
    //             return;
    //         } else {
    //             std::memcpy(config.sta.ssid, iterator->second.c_str(), iterator->second.length());
    //             config.sta.ssid[iterator->second.length()] = '\0';
    //         }


    //         wifi.stop();
    //         wifi.setStaConfig(config);
    //         wifi.start();
    //     }
    // }

    // static void configureAp(std::map<std::string, std::string>& params)
    // {
    //     network::WiFi& wifi = network::WiFi::getInstance();
    //     wifi_config_t config;
    //     wifi.getApConfig(config);

    //     std::map<std::string, std::string>::iterator iterator = params.find("ssid_name");
    //     if (iterator != params.end()) {
    //         if (iterator->second.length() >= 32) {
    //             ESP_LOGW(TAG1, "AP SSID value is higher than 32 characters, trimming");
    //             std::memcpy(config.ap.ssid, iterator->second.c_str(), 32);
    //             config.ap.ssid_len = 32;
    //         } else if (iterator->second.length() == 0) {
    //             ESP_LOGW(TAG1, "AP SSID value is empty, not changing");
    //             return;
    //         } else {
    //             std::memcpy(config.ap.ssid, iterator->second.c_str(), iterator->second.length());
    //             config.ap.ssid_len = iterator->second.length();
    //         }
    //         network::sanitizeSsid(config.ap.ssid, config.ap.ssid_len);
    //         wifi.stop();
    //         wifi.setApConfig(config);
    //         wifi.start();
    //     }
    // }

    // static void turnWstHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse)
    // {
    // }

    // static void turnWapHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse)
    // {
    // }


}; // HttpServerHandler

#endif /* COMPONENTS_CPP_UTILS_HTTPSERVER_H_ */
