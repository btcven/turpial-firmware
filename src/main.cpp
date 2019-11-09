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

void loop_task(void *pvParameter)
{   
    /* char ssid[] = "miSSIDPersona,text muy arog";
    char pass[] = "miPassword123-gustavo gitskdlsdmksmdksmd,s";
    size_t length;
    char* buffer;
    char** pBuffer = &buffer; //when serialized buffer point to different address , that is why pBuffer can hold the initial address
    
 
    //initial test object
    wifi_dto_config_t wifi_params = {
        wifi_params.apChannel = 8,
        wifi_params.apMaxConn = 7,
        wifi_params.WAP_enabled = 1, // Default value
        wifi_params.WST_enabled = 1, // Default value
        wifi_params.isOpen = 1,
        wifi_params.apSSID = ssid,
        wifi_params.apPassword = pass, 
    }; 
 
    WiFiDTO wifi_dto(wifi_params); //object to be serialized
    length = wifi_dto.serialize_size(); //get the length of the dto class
    
    //buffer to store the serialized data
    buffer = (char*)malloc(sizeof(char)*length); //allocate memory to the buffer
    wifi_dto.serialize(buffer); //serialize data into the buffer


  //change the data inside structure just to know if deserialization is able to recover the information and interpolate

    buffer = *pBuffer; //recover the initial address to deserialized information
    wifi_dto.deserialize(buffer);//buffer with initial address
    //to check deserialization 
     wifi_dto.printData(); */

}





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
    /* WiFiMode* wlan = new WiFiMode(wifi_params);//this parameters need to arrives from any storage system or socket
    wlan->begin(); */
 
    std::string value = "Hello world my friend";
    std::string key = "test";
    std::string *buff = new std::string();
    std::string *buff2 = new std::string();
    nvs->open(NVS_WIFI_NAMESPACE,NVS_READWRITE);
    nvs->set(key,value,true);
    //std::cout << "output NVS: " << nvs->open("test",NVS_READWRITE) << std::endl;
    nvs->get(key,buff,true);
    std::cout <<"gettingResult: " << nvs->get(key,buff,true) << std::endl;
    std::cout <<"--------------------------" << *buff << "--------------------------" << std::endl;

    delete buff;

   
    size_t length;
    char* buffer;
    char** pBuffer = &buffer; //when serialized buffer point to different address , that is why pBuffer can hold the initial address
    
     //initial test object
    wifi_dto_config_t wifi_params = {
        wifi_params.apChannel = 8,
        wifi_params.apMaxConn = 7,
        wifi_params.WAP_enabled = 1, // Default value
        wifi_params.WST_enabled = 1, // Default value
        wifi_params.isOpen = 1,
        wifi_params.apSSID = WST_SSID,
        wifi_params.apPassword = WST_PASS, 
    }; 
 
    WiFiDTO wifi_dto(wifi_params); //object to be serialized
    length = wifi_dto.serialize_size(); //get the length of the dto class
    
    //buffer to store the serialized data
    buffer = (char*)malloc(sizeof(char)*length); //allocate memory to the buffer
    wifi_dto.serialize(buffer); //serialize data into the buffer

   
    nvs->set(key,buffer,true);

    nvs->get(key,buff2,true);
    std::cout <<"gettingResult: " << nvs->get(key,buff2,true) << std::endl;
    std::cout << *buff2 << std::endl;
    std::cout << buff2->length() << std::endl;
/*     helloTask = new Hello();
    helloTask->setStackSize(2048);
    helloTask->start((void*)pcTask1); */


} 