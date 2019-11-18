/**
 * @file Battery.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BATTERY_H
#define BATTERY_H

#include "BQ27441.h"

namespace esc {

/**
 * @brief Battery on-init configuration.
 * 
 */
struct BatteryConfig {
    /// SDA GPIO pin
    gpio_num_t sda;
    /// SCL GPIO pin
    gpio_num_t scl;
    /// GPOUT GPIO pin
    gpio_num_t gpout;
    /// I2C port number
    i2c_port_t port;
    /// I2C clock speed
    std::uint32_t clk_speed;
    /// Battery capacity in mAh
    std::uint16_t capacity;
    /// The SoC percentage interval usted to be notified;
    std::uint8_t percentage_interval;
};

/**
 * @brief Battery management
 * 
 */
class Battery
{
public:
    /**
     * @brief Construct a new Battery object
     * 
     */
    Battery();

    /**
     * @brief Initialize Battery
     * 
     * @param[in] battery_config: battery configuration
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t begin(BatteryConfig battery_config);

private:
    static void interrupt_handler(void* arg);
    BQ27441 m_bq27441;
};

} // namespace esc

#endif // BATTERY_H