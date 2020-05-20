/**
 * Copyright 2020 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** 
 * @file Task.cpp
 * @author kolban
 *  
 */


#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>

#include "Task.h"
#include "sdkconfig.h"

static const char* LOG_TAG = "Task";

Task::Task(std::string task_name, uint16_t stack_size, uint8_t priority)
{
    m_task_name = task_name;
    m_stack_size = stack_size;
    m_priority = priority;
    m_task_data = nullptr;
    m_handle = nullptr;
    m_core_id = tskNO_AFFINITY;
}

Task::~Task()
{
}

void Task::delay(int ms)
{
    ::vTaskDelay(ms / portTICK_PERIOD_MS);
}

void Task::runTask(void* task_instance)
{
    Task* task = reinterpret_cast<Task*>(task_instance);
    ESP_LOGD(LOG_TAG, ">> runTask: taskName=%s", task->m_task_name.c_str());
    task->run(task->m_task_data);
    ESP_LOGD(LOG_TAG, "<< runTask: taskName=%s", task->m_task_name.c_str());
    task->stop();
}

void Task::start(void* task_data)
{
    if (m_handle != nullptr) {
        ESP_LOGW(LOG_TAG, "Task::start - There might be a task already running!");
    }
    m_task_data = task_data;
    ::xTaskCreatePinnedToCore(&runTask, m_task_name.c_str(), m_stack_size, this, m_priority, &m_handle, m_core_id);
}

void Task::stop()
{
    ESP_LOGD(LOG_TAG, "<< Stopping task");
    if (m_handle == nullptr) return;
    xTaskHandle temp = m_handle;
    m_handle = nullptr;
    ::vTaskDelete(temp);
}

void Task::setStackSize(uint16_t stack_size)
{
    m_stack_size = stack_size;
}

void Task::setPriority(uint8_t priority)
{
    m_priority = priority;
}

void Task::setName(std::string name)
{
    m_task_name = name;
}

void Task::setCore(BaseType_t core_id)
{
    m_core_id = core_id;
}
