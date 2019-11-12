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

#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "sdkconfig.h"
#include <Arduino.h>

#include "WiFi/WiFiMode.h"
#include "ESC/battery.h"
#include "NVS/SingleNVS.h"
//#include "WiFiDTO.h"
#include "testRTOSCPP/Hello.hpp"

#include "defaults.h"


//singleton instance for all the application
SingleNVS* nvs = SingleNVS::getInstance(); //create or recovery SingletonNVS instance as needed

const char* pcTask1 = "Task1\n";
const char* pcTask2 = "Task2\n";
static Hello *helloTask;
 

wifi_dto_config_t wifi_params = {
        wifi_params.apChannel = WAP_CHANNEL,
        wifi_params.apMaxConn = WAP_MAXCONN,
        wifi_params.WAP_enabled = WAP_ENABLED, // Default value
        wifi_params.WST_enabled = WST_ENABLED, // Default value
        wifi_params.isOpen = false,
        wifi_params.apSSID = WST_SSID,
        wifi_params.apPassword = WST_PASS, 
}; 

extern "C" void app_main()
{
    // iniciamos arduino como componente.
    initArduino();
    size_t length;
    char* buffer;
    char* key = "blob";

    /////////////////////////////////// SERIALIZATION////////////////////////////////////////

    WiFiDTO wifi_dto(wifi_params); //object to be serialized and simulated serialized buffer from external world
    length = wifi_dto.serialize_size(); //get the length of the dto class data fields to calculate serialized buffer length
    buffer = (char*)malloc(sizeof(char)*length); //allocate memory to the buffer
    wifi_dto.serialize(buffer); //serialize data into the buffer
    /////////////////////////////END SERIALIZATION/////////////////////////////////////////////////////////////////////
    
    /////////////////////////NVS///////////////////////////////////////
    nvs->begin();
    nvs->open(NVS_WIFI_NAMESPACE,NVS_READWRITE);
    //nvs->set(key,buffer,length); //to store the serialized buffer
    void* result_from_NVS; ///to recover the data blob from nvs
    result_from_NVS = malloc(sizeof(char)*length);  //allocate memory
    nvs->get(key,result_from_NVS,length);   //get the blob
    ////////////////////////////////////////////////////////////////////////
    /////////////////////END NVS//////////////////////////////////////////

    //////////////////////////////////DESERIALIZE//////////////////////////////////////////////////////////////////////////
     //new WiFiDTO object to deserialized the temp buffer and get the structure fields, we can use the same wifi_dto object 
     //without needed to create new ones, this cases is just in order to emulate buffers from external world
    WiFiDTO wifi_dto2; 
    wifi_dto2.deserialize((char*)result_from_NVS);
    wifi_dto2.printData();
    ///////////////////////////////////FIN DESERIALIZE//////////////////////////////////////////////////////////////////////////////////////

    wifi_dto_config_t data_to_config_wifi;
    data_to_config_wifi = wifi_dto2.getData(); //return the struct inside DTO object

    std::cout<< std::endl << "-------------------------WIFI CONFIGURATION-----------------------------------------------" << std::endl;
    std::cout << data_to_config_wifi.apSSID << std::endl;    
    std::cout << data_to_config_wifi.apPassword << std::endl; 
    
    WiFiMode* wlan = new WiFiMode(data_to_config_wifi);//this parameters need to arrives from any storage system or socket
    wlan->begin();

} 