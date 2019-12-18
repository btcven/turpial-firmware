/**
 * @file FuelGauge.h
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief
 * @version 0.1
 * @date 2019-11-22
 *
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 *
 */

#ifndef FUEL_GAUGE_H
#define FUEL_GAUGE_H

#include <cstdint>

#include <driver/gpio.h>
#include <driver/i2c.h>

#include <esp_err.h>

namespace esc {

class FuelGauge
{
public:
    /**
     * @brief Construct a new Fuel Gauge object
     *
     */
    FuelGauge();

    /**
     * @brief Get the battery voltage.
     *
     * @param[out] voltage: The voltage return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t voltage(std::uint16_t* voltage);

    /**
     * @brief Get the avg. battery current.
     *
     * @param[out] avg_current: The avg. current return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t avgCurrent(std::int16_t* avg_current);

    /**
     * @brief Get the Avg Power object
     *
     * @param[out] avg_power: The avg. power return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t avgPower(std::int16_t* avg_power);

private:
    /**
     * @brief read a Word (std::uint16_t) from the IC.
     *
     * @param[in]  command: Command.
     * @param[out] word: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - ESP_FAIL: `word` is a null pointer, or i2c failure.
     *      - (others): failed.
     */
    esp_err_t readWord(std::uint8_t command, std::uint16_t* word);

    /**
     * @brief Init i2c iface
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t i2cInit();

    /**
     * @brief delete i2c iface
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t i2cDelete();

    /// I2C port configuration.
    i2c_config_t m_conf;
};


} // namespace esc


#endif //FUEL_GAUGE_H
