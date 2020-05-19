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
 * @file Battery.h
 * @author Locha Mesh Developers (contact@locha.io)
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
                   gpio_num_t sysoff_num_t,
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
