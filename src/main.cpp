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

#include <Arduino.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "hal/hardware.h"
#include "WiFi/WiFiMode.h"
#include "ESC/battery.h"
#include  "WiFiDTO.h" //Data transfer object for wifi parameters
#include "NVS/SingletonNVS.h"


#include <iostream>
#include <string>
#include <vector>


//singleton instance for all the application
SingletonNVS* nvs = SingletonNVS::getInstance(); //create or recovery SingletonNVS instance as needed

void setup() {
    char ssid[] = "miSSIDPersona";
    char pass[] = "miPassword123";
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
    wifi_dto.printData();

   
}

void loop()
{
    //
}