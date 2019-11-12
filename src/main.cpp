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

extern "C" void app_main()
{
    // Initialize arduino as a component
    initArduino();
    

    auto wifi_params = wifi::DTOConfig(WAP_CHANNEL,
                                       WAP_MAXCONN,
                                       WAP_ENABLED,
                                       WST_ENABLED,
                                       false,
                                       WST_SSID,
                                       WST_PASS);

    wifi::mode::begin(wifi_params);
}