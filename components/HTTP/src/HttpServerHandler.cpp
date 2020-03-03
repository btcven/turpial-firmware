#include "HttpServerHandler.h"
#include "defaults.h"
#include "esp_log.h"
#include <Storage.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>


static const char* TAG = "HTTP-SERVER-HANDLER";

HttpServerHandler::HttpServerHandler()
{
}

void configureSta(std::map<std::string, std::string>& params)
{
    network::WiFi& wifi = network::WiFi::getInstance();
    wifi_config_t config;
    wifi.getStaConfig(config);

    std::map<std::string, std::string>::iterator iterator = params.find("ssid_name");
    if (iterator != params.end()) {
        if (iterator->second.length() >= 31) {
            ESP_LOGW(TAG, "STA SSID value is higher than 31 characters, trimming");
            std::memcpy(config.sta.ssid, iterator->second.c_str(), 31);
            config.sta.ssid[31] = '\0';
        } else if (iterator->second.length() == 0) {
            ESP_LOGW(TAG, "AP SSID value is empty, not changing");
            return;
        } else {
            std::memcpy(config.sta.ssid, iterator->second.c_str(), iterator->second.length());
            config.sta.ssid[iterator->second.length()] = '\0';
        }


        wifi.stop();
        wifi.setStaConfig(config);
        wifi.start();
    }
}

void configureAp(std::map<std::string, std::string>& params)
{
    network::WiFi& wifi = network::WiFi::getInstance();
    wifi_config_t config;
    wifi.getApConfig(config);

    std::map<std::string, std::string>::iterator iterator = params.find("ssid_name");
    if (iterator != params.end()) {
        if (iterator->second.length() >= 32) {
            ESP_LOGW(TAG, "AP SSID value is higher than 32 characters, trimming");
            std::memcpy(config.ap.ssid, iterator->second.c_str(), 32);
            config.ap.ssid_len = 32;
        } else if (iterator->second.length() == 0) {
            ESP_LOGW(TAG, "AP SSID value is empty, not changing");
            return;
        } else {
            std::memcpy(config.ap.ssid, iterator->second.c_str(), iterator->second.length());
            config.ap.ssid_len = iterator->second.length();
        }
        network::sanitizeSsid(config.ap.ssid, config.ap.ssid_len);
        wifi.stop();
        wifi.setApConfig(config);
        wifi.start();
    }
}


void configureStaAp(std::map<std::string, std::string>& params)
{
    std::map<std::string, std::string>::iterator iterator = params.find("type");

    if (iterator != params.end()) {
        if (iterator->second == "wst") {
            std::cout << "wst" << std::endl;
            configureSta(params);
        } else if (iterator->second == "ap") {
            std::cout << "ap" << std::endl;
            configureAp(params);
        }
        esp_restart();
    }
}

esp_err_t checkCredentials(const char* user, const char* pass)
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

    char user_name[MAX_USER_NAME_LENGTH];
    char user_password[MAX_USER_PASSWORD_LENGTH];
    size_t ul = MAX_USER_NAME_LENGTH;
    size_t pl = MAX_USER_PASSWORD_LENGTH;
    err = app_nvs.getString(USER_NAME_KEY, user_name, &ul);
    if (err != ESP_OK) {
        return ESP_FAIL;
    } else {
        err = app_nvs.getString(USER_PASSWORD_KEY, user_password, &pl);
        if (err != ESP_OK) {
            return ESP_FAIL;
        }
    }
    ESP_LOGI(TAG, "Username: %s, Password: %s --> inside eeprom", &user_name[0], &user_password[0]);
    if (strcmp(user_name, user) == 0 && strcmp(user_password, pass) == 0) {
        ESP_LOGI(TAG, "The password and user are the same...");
        return ESP_OK;
    } else {
        return ESP_FAIL;
    }
}

void HttpServerHandler::readDeviceInfoHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse)
{
    std::ostringstream json_ostring;
    std::uint16_t voltage = 0;
    std::int16_t avg_current = 0;
    std::int16_t avg_power = 0;
    std::uint16_t temp = 0;
    esc::FuelGauge& fuel_gauge = esc::FuelGauge::getInstance();

    esp_err_t err = fuel_gauge.voltage(&voltage);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Can't get voltage, err = %s",
            esp_err_to_name(err));
    }

    err = fuel_gauge.avgCurrent(&avg_current);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Can't get avg. current, err = %s",
            esp_err_to_name(err));
    }

    err = fuel_gauge.avgPower(&avg_power);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Can't get avg. power, err = %s",
            esp_err_to_name(err));
    }

    err = fuel_gauge.temperature(esc::TempMeasure::Internal, &temp);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Can't get internal temperature, err = %s",
            esp_err_to_name(err));
    }

    pHttpResponse->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
    pHttpResponse->addHeader(HttpRequest::HTTP_HEADER_CONTENT_TYPE, "application/json");

    json_ostring << "{";

    json_ostring << "\"voltage\":\"";
    json_ostring << voltage;
    json_ostring << "\",";

    json_ostring << "\"avg_current\":\"";
    json_ostring << avg_current;
    json_ostring << "\",";

    json_ostring << "\"temp\":\"";
    json_ostring << temp;
    json_ostring << "\",";

    json_ostring << "\"avg_power\":\"";
    json_ostring << avg_power;
    json_ostring << "\",";

    json_ostring << "\"free_memory\":\"";
    json_ostring << heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
    json_ostring << "\"";

    json_ostring << "}";
    checkCredentials("gustavo", "grisales");

    pHttpResponse->sendData(json_ostring.str());
    pHttpResponse->close();
}


std::map<std::string, std::string> parseJson(std::istringstream& stream, size_t num_of_fields_in_json)
{
    std::string token[num_of_fields_in_json]; //length of json object, hoe many field there are
    std::map<std::string, std::string> json_map;
    int i = 0;
    size_t found;

    while (std::getline(stream, token[i], ',')) {
        //to remove open brace in json string packet
        if (i == 0) {
            found = token[i].find('{');
            if (found != std::string::npos) {
                token[i].erase(std::remove(token[i].begin(), token[i].end(), '{'), token[i].end());
            }
        }

        size_t length = sizeof(token) / sizeof(*token);
        //to remove clode brace in json string packet
        if (i == length - 1) {
            found = token[i].find('}');
            if (found != std::string::npos) {
                token[i].erase(std::remove(token[i].begin(), token[i].end(), '}'), token[i].end());
            }
        }
        //to remove double quote in key value pair
        found = token[i].find('\"');
        if (found != std::string::npos) {
            token[i].erase(std::remove(token[i].begin(), token[i].end(), '\"'), token[i].end());
        }
        //to remove tabs in key and value pair
        found = token[i].find('\t');
        if (found != std::string::npos) {
            token[i].erase(std::remove(token[i].begin(), token[i].end(), '\t'), token[i].end());
        }
        //to remove new line in key value pair
        found = token[i].find('\n');
        if (found != std::string::npos) {
            token[i].erase(std::remove(token[i].begin(), token[i].end(), '\n'), token[i].end());
        }

        std::istringstream stream(token[i]);
        std::string field[2];
        int j = 0;

        while (std::getline(stream, field[j], ':')) {
            j++;
        }
        json_map.insert(std::make_pair(field[0], field[1]));

        i++;
    }

    return json_map;
}

void HttpServerHandler::setUpStaApHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse)
{
    std::string body = pHttpRequest->getBody();
    std::istringstream ss(body);
    std::string token[3]; //length of json object, hoe many field there are
    std::map<std::string, std::string> json_map;

    json_map = parseJson(ss, 3);

    // //this reply need to be send before to stop wifi adapter but in this moment the server is not able to send  a reply
    pHttpResponse->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
    //return response to the remote client
    pHttpResponse->sendData("{\"status\":\"OK\"}");

    configureStaAp(json_map);
}

esp_err_t saveCredentials(std::string new_user, std::string new_pass) 
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
    err = app_nvs.setString(USER_NAME_KEY, new_user);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "There are any errors , was not possible to save in NVS the username /n");
            return ESP_FAIL;
        }
        err = app_nvs.setString(USER_PASSWORD_KEY, new_pass);
        if (err != ESP_OK) {
            ESP_LOGI(TAG,"Error trying to save default password/n");
            return ESP_FAIL;
        }
    return ESP_OK;
}


void HttpServerHandler::setUpCredentialHandler(HttpRequest* pHttpRequest, HttpResponse* pHttpResponse)
{
    std::string body = pHttpRequest->getBody();
    std::istringstream ss(body);
    std::string token[4]; //length of json object, hoe many field there are
    std::map<std::string, std::string> json_map;
    esp_err_t err;

    std::string user;
    std::string pass;
    std::string new_user;
    std::string new_pass;
    json_map = parseJson(ss, 4);

    std::map<std::string, std::string>::iterator iterator =
        json_map.find("user_name");
    if (iterator != json_map.end()) {
        user = iterator->second;
        iterator = json_map.find("password");
        if (iterator != json_map.end()) {
            pass = iterator->second;
            iterator = json_map.find("new_username");
            if (iterator != json_map.end()) {
                new_user = iterator->second;
                iterator = json_map.find("new_password");
                if (iterator != json_map.end()) {
                    new_pass = iterator->second;
                    esp_err_t err = checkCredentials(user.c_str(), pass.c_str());
                    if (err == ESP_OK) {
                        //we need to change credentials in eeprom
                        err = saveCredentials(new_user,new_pass);
                        if(err != ESP_OK) {
                            pHttpResponse->setStatus(HttpResponse::HTTP_STATUS_INTERNAL_SERVER_ERROR, "Error saving credentials");
                            pHttpResponse->sendData("{\"status\":\"ERR\"}"); 
                            return;
                        }
                        pHttpResponse->setStatus(HttpResponse::HTTP_STATUS_OK, "OK");
                        pHttpResponse->sendData("{\"status\":\"OK\"}");

                    } else {
                        pHttpResponse->setStatus(HttpResponse::HTTP_STATUS_BAD_REQUEST, "credentials wrong");
                        pHttpResponse->sendData("{\"status\":\"ERR\"}");
                    }
                    return;
                }
            }
        }
    }

    pHttpResponse->setStatus(HttpResponse::HTTP_STATUS_BAD_REQUEST, "BAD REQUEST");
    pHttpResponse->sendData("{\"status\":\"ERR\"}");
}