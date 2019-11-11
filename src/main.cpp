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
    //nvs->set(key2,buffer,length);
    //nvs->get(key2,bufferTest,length);
    //std::cout <<"gettingResult: " << nvs->get(key,buff2,true) << std::endl;

/*     helloTask = new Hello();
    helloTask->setStackSize(2048);
    helloTask->start((void*)pcTask1); */


    char ssid[] = "miSSIDPersona";
    char pass[] = "miPassword";

    char ssid2[] = "hell";
    char pass2[] = "Camb";
    size_t length;
    char* buffer;
    char** pBuffer = &buffer; //when serialized buffer point to different address , that is why pBuffer can hold the initial address
    
    char* key = "blob";


    //initial test object
    wifi_dto_config_t wifi_params = {
         8,
        7,
       1, // Default value
         1, // Default value
        1,
        ssid,
        pass, 
    }; 
     wifi_dto_config_t wifi_paramsChange = {
         12,
        14,
       0, // Default value
         0, // Default value
        2,
        ssid2,
        pass2, 
    }; 

    WiFiDTO wifi_dto(wifi_params); //object to be serialized and simulated serialized buffer from external world
    length = wifi_dto.serialize_size(); //get the length of the dto class data fields to calculate serialized buffer length
    buffer = (char*)malloc(sizeof(char)*length); //allocate memory to the buffer
    wifi_dto.serialize(buffer); //serialize data into the buffer
    
    nvs->begin();
    nvs->open("blobtest",NVS_READWRITE);
    nvs->set(key,buffer,length);
    void* temp;
    temp = malloc(sizeof(char)*length);
    nvs->get(key,temp,length);
    WiFiDTO wifi_dto2;
    wifi_dto2.deserialize((char*)temp);

    //wifi_dto.setData(wifi_paramsChange);
    wifi_dto2.printData();
    //change the data inside structure just to know if deserialization is able to recover the information and interpolate

    //having a serialized buffer we can deserialize it with other different object  "wifi_dto2"
    //to demostrate deserialization working, just we need to fix the end of the string when print in stdout
    buffer = *pBuffer; //recover the initial address to deserialized information

  

} 