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

    char* pcTaskName;
    pcTaskName = (char*)pvParameter;
    for (int i = 10; i >= 0; i--)
    {
        std::cout << "reestarting in" << i << "seconds" << std::endl; 
        std::cout << "the parameter is : " << pcTaskName<< std::endl;
        //printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
    //free(buffer);
    while(1){
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}





/* const char* pcTask1 = "Task1\n";
const char* pcTask2 = "Task2\n";
static Hello *helloTask;
 */

char ssid[] = "HOME-EB05";
char pass[] = "D8ED78ECC7362942";
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
    nvs->begin();
    WiFiMode* wlan = new WiFiMode(wifi_params);
    wlan->begin();
 
    
    
    //std::cout << "output NVS: " << nvs->open("test",NVS_READWRITE) << std::endl;
    
    //esp_err_t status = wlan->begin(wifi_params);

   /*  helloTask = new Hello();
    helloTask->setStackSize(2048);
    helloTask->start((void*)pcTask1);*/
} 