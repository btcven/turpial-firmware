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

#include <cstdio>
#include <sstream>

#include "esp_log.h"
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "NVS.h"
#include "WiFiMode.h"

#include "defaults.h"


static const char* TAG = "app_main";





static wifi::WiFiMode wifi_mode;
static storage::NVS nvs_app;

extern "C" void app_main() {

    nvs_app.setStackSize(4096);
    nvs_app.start();

    vTaskDelay(5000 / portTICK_PERIOD_MS); //don't remove this line while for nvs init, just fo test
    tcpip_adapter_init();
    
    
    //we need to pass parameters in a serialized way
    wifi_mode.setStackSize(4096);
    wifi_mode.start((void*)1); // 0 no inicializada 1 inicializada 2 configurada

    /*
    need to be done, shared data b/w task, in order to send nvs state to wifi task
    serialized data to shared b/w task 
    */

}