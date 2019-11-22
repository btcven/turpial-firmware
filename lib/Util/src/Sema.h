/**
 * @file Sema.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2019-11-22
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef UTIL_SEMA_H
#define UTIL_SEMA_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

namespace util {

class Semaphore
{
public:
    Semaphore(const char* dbg_name);
    Semaphore(Semaphore&& other);

    ~Semaphore();

    Semaphore(Semaphore const&) = delete;
    Semaphore& operator=(Semaphore const&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;

    void take();
    void give();
    void wait();

private:
    Semaphore() {}
    SemaphoreHandle_t m_handle;
    const char* m_dbg_name;
};

} // namespace util

#endif // UTIL_SEMA_H