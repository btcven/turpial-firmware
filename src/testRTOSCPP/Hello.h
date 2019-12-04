#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Task.h"
#include <cstdio>
#include <esp_log.h>
#include <iostream>
#include <string>


class Hello : public Task
{
    void run(void* data) override
    {
        char* pc_task_name = reinterpret_cast<char*>(data);

        for (int i = 10; i >= 0; i--) {
            std::cout << "reestarting in" << i << "seconds" << std::endl;
            std::cout << "the parameter is : " << pc_task_name << std::endl;
            //printf("Restarting in %d seconds...\n", i);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }

        std::printf("Restarting now.\n");
        std::fflush(stdout);
        esp_restart();
        return;
    }
};