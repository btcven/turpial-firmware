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


#include <sstream>
#include <cstdio>


#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include "WiFiMode.h"
#include "ESC/battery.h"
#include "testRTOSCPP/Hello.hpp"

#include "Namespace.h"

#include "defaults.h"

/* #include "nvs_flash.h"
#include "nvs.h" */

esp_err_t readWiFiParams(wifi::DTOConfig& wifi_params) {
    ESP_LOGD(__func__, "Reading WiFi configuration from NVS");
    ESP_LOGI(__func__, "----------------------Reading WiFi configuration from NVS-------------------------------------");

    nvs::Namespace wifi_nvs;
    auto err = wifi_nvs.open("wifi", NVS_READWRITE);
    if (err != ESP_OK) {
    ESP_LOGE(__func__, "---------------------------Erro abriendo el namespace---------------------------");
        return err;
    }

    std::stringstream blob;
    err = wifi_nvs.get_blob("wifi_dto_config", blob);
    if (err != ESP_OK) {
        return err;
    }

    wifi_params.deserialize(blob);

    return ESP_OK;
}

void setDefaultWiFiParams(wifi::DTOConfig& wifi_params) {
    wifi_params.apChannel = WAP_CHANNEL;
    wifi_params.apMaxConn = WAP_MAXCONN;
    wifi_params.WAP_enabled = WAP_ENABLED;
    wifi_params.WST_enabled = WST_ENABLED;
    wifi_params.isOpen = false;
    wifi_params.apSSID = tinystring::String(WAP_SSID);
    wifi_params.apPassword = tinystring::String(WAP_PASS);

     std::cout << "out1: " << wifi_params.apSSID.c_str() << std::endl;
    std::cout << "out2: " << wifi_params.apPassword.c_str()  << std::endl;
    std::cout << "SSID: " << wifi_params.apSSID.c_str() << std::endl;
    std::cout << "PASSWORD: " << wifi_params.apPassword.c_str() << std::endl;
    std::cout << "APCHANNEL " << wifi_params.apChannel << std::endl;
    std::cout << "apMACCONNECT " << wifi_params.apMaxConn<< std::endl;
    std::cout << "ISOPEN " << wifi_params.isOpen << std::endl;
    std::cout << "WAPENABLED " << wifi_params.WAP_enabled << std::endl;
    std::cout << "WST ENABLED " << wifi_params.WST_enabled << std::endl; 
} 

extern "C" void app_main()
{
    // Initialize arduino as a component
    initArduino();
    // Initialize NVS.
    auto nvs_err = nvs::begin();
    wifi::DTOConfig wifi_params;
    nvs::Namespace wifi_nvs;
    wifi_nvs.open("wifi",NVS_READWRITE);







    /* if (nvs_err != ESP_OK) {
        ESP_LOGI(__func__, "------------------------HUBO UN ERROR---------------------------\n");
        ESP_LOGE(__func__, "Couldn't initialize NVS, error %s", esp_err_to_name(nvs_err));
        ESP_LOGD(__func__, "Using default WiFi parameters");
        
        setDefaultWiFiParams(wifi_params);
    } else {
         ESP_LOGI(__func__, "------------------------SE INICIALIZO---------------------------\n");
        auto err = readWiFiParams(wifi_params);
        ESP_LOGI(__func__, "------------------------saliendo de read-------------------------\n");
        std::cout << err << std::endl;

        if (err != ESP_OK) {
            auto estr = esp_err_to_name(err);
            ESP_LOGE(__func__, "Couldn't read WiFi parameters %s", estr);
            ESP_LOGD(__func__, "Using default WiFi parameters");
            
            setDefaultWiFiParams(wifi_params);
        }
    }
 */


    wifi::DTOConfig wifi_params2;
    setDefaultWiFiParams(wifi_params);
    std::stringstream blob;
    std::stringstream blob2;

    
    std::cout << "------------------------going to Serialized------------------------------" << std::endl;
    std::cout << "------------------------WIFI_PARMS WORKING AS A SERILIZER------------------------------" << std::endl;
    //wifi_params.serialize(blob);
    //auto stream_in = std::istringstream(blob.str());
    //stream_in.

    //void* temp = "hola";
    //std::size_t tempSize = 5;
    //wifi_nvs.set_blob("wifi",blob.str().c_str() , wifi_params.serialize_size());

    auto err = wifi_nvs.get_blob("wifi", blob2);
    std::cout << "--------**********************---------------salida "<< err << std::endl;

    wifi_params2.deserialize(blob2);
   std::cout << "------------------------going to Deserialized------------------------------" << std::endl;
    std::cout << "------------------------WIFI_PARMS-2 WORKING AS A DESERILIZER------------------------------" << std::endl;

     //wifi_params2.deserialize(stream_in);

    std::cout << "SSID: " << wifi_params2.apSSID.c_str() << std::endl;
    std::cout << "PASSWORD: " << wifi_params2.apPassword.c_str() << std::endl;
    std::cout << "APCHANNEL " << (int)wifi_params2.apChannel << std::endl;
    std::cout << "apMACCONNECT " << (int)wifi_params2.apMaxConn<< std::endl;
    std::cout << "ISOPEN " << wifi_params2.isOpen << std::endl;
    std::cout << "WAPENABLED " << wifi_params2.WAP_enabled << std::endl;
    std::cout << "WST ENABLED " << wifi_params2.WST_enabled << std::endl; 
 




    /*wifi::mode::begin(wifi_params); */

    // TODO: app loop
}

 