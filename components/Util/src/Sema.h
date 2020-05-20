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
 * @file Sema.h
 * @author Locha Mesh Developers (contact@locha.io)
 * 
 */

#ifndef UTIL_SEMA_H
#define UTIL_SEMA_H

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <cstdint>

namespace util {

/**
 * @brief A FreeRTOS Semaphore abstraction
 * 
 */
class Semaphore
{
public:
    /**
     * @brief Construct a new
     * 
     * @param dbg_name: name of the semaphore for debugging purposes
     */
    Semaphore(const char* dbg_name);

    /**
     * @brief Construct a new Semaphore from other moved Semaphore
     * 
     * @param other 
     */
    Semaphore(Semaphore&& other);

    ~Semaphore();

    Semaphore(Semaphore const&) = delete;
    Semaphore& operator=(Semaphore const&) = delete;
    Semaphore& operator=(Semaphore&&) = delete;

    /**
     * @brief Take or "lock" this semaphore
     * 
     */
    void take();

	/**
	 * @brief Take a semaphore.
	 *
	 * Take a semaphore but return if we haven't obtained it in the given
	 * period of milliseconds.
	 *
	 * @param[in] timeout_ms: Timeout in milliseconds.
	 *
	 * @return
	 *		- true: we took the semaphore.
	 *		- false: we didn't take the semaphore.
	 */
	bool take(std::uint32_t timeout_ms);

    /**
     * @brief Give or "unlock" this semaphore
     * 
     */
    void give();

    /**
     * @brief Waits for this semaphore to be "given".
     * 
     */
    void wait();

private:
    /**
     * @brief Construct a new Semaphore
     * 
     */
    Semaphore() {}

    SemaphoreHandle_t m_handle; /**< raw semaphore handle */
    const char* m_dbg_name;     /**< debug name */
};

} // namespace util

#endif // UTIL_SEMA_H
