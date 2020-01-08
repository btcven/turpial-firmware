/**
 * @file Battery.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief
 * @version 0.1
 * @date 2019-11-22
 *
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 *
 */

#ifndef ESC_INTERRUPT_H
#define ESC_INTERRUPT_H

#include <cstdint>

#include <esp_err.h>
#include <driver/gpio.h>

#include <Task.h>

namespace esc {

/**
 * @brief
 */
class Battery {
public:
    Battery(Battery const&) = delete;
    Battery(Battery&&) = delete;

    Battery& operator=(Battery const&) = delete;
    Battery& operator=(Battery&&) = delete;

    /**
     * @brief Get the Battery object unique instance.
     *
     * @return Battery& a reference to the Battery object.
     */
    static Battery& getInstance()
    {
        // Instatiated only once, won't be created again.
        static Battery g_instance;
        return g_instance;
    }

    /**
     * @brief
     *
     * @param[in] io_num:
     * @param[in] mode:
     * @param[in] max_capacity:
     */
    esp_err_t init(gpio_num_t gpout_io_num,
                   std::uint8_t soc_interval,
                   std::uint16_t max_capacity);

private:
    /**
     * @brief
     */
    Battery();
};


class Interrupt : public Task {
public:
    Interrupt();

    virtual void run(void* task_data);
};

} // namespace esc

#endif // ESC_INTERRUPT_H
