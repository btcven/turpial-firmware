/*
 * Task.cpp
 *
 *  Created on: Mar 4, 2017
 *      Author: kolban
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
    ESP_LOGD(LOG_TAG, "<< stoping task");
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
