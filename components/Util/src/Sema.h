/**
 * @file Sema.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-22
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
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
