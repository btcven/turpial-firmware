/*
 * Task.h
 *
 *  Created on: Mar 4, 2017
 *      Author: kolban
 */

#ifndef TASK_H_
#define TASK_H_

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>

/**
 * @brief Encapsulate a runnable task.
 *
 * This class is designed to be subclassed with the method:
 *
 * @code{.cpp}
 * void run(void *data) { ... }
 * @endcode
 *
 * For example:
 *
 * @code{.cpp}
 * class CurlTestTask : public Task {
 *    void run(void *data) {
 *       // Do something
 *    }
 * };
 * @endcode
 *
 * implemented.
 */
class Task
{
public:
    /**
     * @brief Construct a new Task
     * 
     * @param task_name The task name
     * @param stack_size The task stack size in bytes
     * @param priority The task priority
     */
    Task(std::string task_name = "Task", uint16_t stack_size = 10000, uint8_t priority = 5);

    /**
     * @brief Destroy the Task object
     * 
     */
    virtual ~Task();

    /**
     * @brief Set the task stack size
     * 
     * @param stack_size stack size in bytes
     */
    void setStackSize(uint16_t stack_size);

    /**
     * @brief Set the task priority
     * 
     * @param priority 
     */
    void setPriority(uint8_t priority);

    /**
     * @brief Set the task Name
     * 
     * @param[in] name: the task name 
     */
    void setName(std::string name);

    /**
     * @brief Set the CoreID
     * 
     * @param[in] core_id: Core ID 
     */
    void setCore(BaseType_t core_id);

    /**
     * @brief Start task 
     * 
     * @param[in] task_data: data that's going to be passed to task handler
     */
    void start(void* task_data = nullptr);

    /**
     * @brief Stop this task
     * 
     */
    void stop();

    /**
	 * @brief Body of the task to execute.
	 *
	 * This function must be implemented in the subclass that represents the
     * actual task to run. When a task is started by calling start(), this is
     * the code that is executed in the newly created task.
	 *
	 * @param[in] data The data passed in to the newly started task.
	 */
    virtual void run(void* data) = 0; // Make run pure virtual

    /**
     * @brief Suspend the stask for milliseconds
     * 
     * @param[in] ms: millisecionds 
     */
    static void delay(int ms);

private:
    xTaskHandle m_handle;
    void* m_task_data;

    /**
     * Static class member that actually runs the target task.
     *
     * The code here will run on the task thread.
     * 
     * @param[in] task_instance: the task to run.
     */
    static void runTask(void* task_instance);

    std::string m_task_name;
    uint16_t m_stack_size;
    uint8_t m_priority;
    BaseType_t m_core_id;
};

#endif /* COMPONENTS_CPP_UTILS_TASK_H_ */
