#include <esp_log.h>
#include <WiFi.h>
#include <FreeRTOS.h>
#include <string>
#include <cstdio>
#include <Task.h>
#include "sdkconfig.h"
#include <iostream>


class Hello: public Task {
    
    void run(void *data) override {
        char* pcTaskName;
        pcTaskName = (char*)data;

        for (int i = 10; i >= 0; i--)
        {
            std::cout << "reestarting in" << i << "seconds" << std::endl; 
            std::cout << "the parameter is : " << pcTaskName<< std::endl;
            //printf("Restarting in %d seconds...\n", i);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }

        std::printf("Restarting now.\n");
        std::fflush(stdout);
        esp_restart();
        return ;
    }
};