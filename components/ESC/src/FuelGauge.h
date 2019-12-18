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

static const bool BAT_LOW = true;  /*!< Battery low */
static const bool SOC_INT = false; /*!< SoC change */

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

    /**
     * @brief Set capacity
     *
     * @param[in] capacity: The capacity in mA.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t setCapacity(std::uint16_t capacity);

    /**
     * @brief GPOUT polarity
     *
     * @param[out] value: Return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t GPOUTPolarity(bool* value);

    /**
     * @brief Set GPOUT polarity.
     *
     * @param[in] active_high: true to set to active high, false for low.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t setGPOUTPolarity(bool active_high);

    /**
     * @brief GPOUT function.
     *
     * @param[out] function: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t GPOUTFunction(bool* function);

    /**
     * @brief Set GPOUT function.
     *
     * @param[in] function: The GPOUT function.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t setGPOUTFunction(bool function);

    /**
     * @brief SOC_INT delta.
     *
     * @param[out] value: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t sociDelta(std::uint8_t* value);

    /**
     * @brief SOC_INT delta.
     *
     * @param[in] delta: The SOC_INT delta.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t setSOCIDelta(std::uint8_t delta);

    /**
     * @brief Pulse GPOUT.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t pulseGPOUT();

    /**
     * @brief Enter configuration mode.
     *
     * @param[in] manual_mode: Used to indicate if the user
     * is manually configuring.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t enterConfig(bool manual_config);

    /**
     * @brief Exit configuration mode.
     *
     * @param[in] resim: Resimulate?
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t exitConfig(bool resim);

    /**
     * @brief Get opConfig.
     *
     * @param[out] result: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t opConfig(std::uint16_t* result);

    /**
     * @brief Write opConfig.
     *
     * @param[in] value: The opConfig value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t writeOpConfig(std::uint16_t value);

    /**
     * @brief Soft reset.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t softReset();

private:
    /**
     * @brief Get result of the STATUS command.
     *
     * @param[out] is_sealed: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t status(std::uint16_t* result);

    /**
     * @brief Get BQ27441 flags.
     *
     * @param[out] result: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t flags(std::uint16_t* result);

    /**
     * @brief Checks if the BQ27441 is sealed.
     *
     * @param[out] is_sealed: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t sealed(bool* is_sealed);

    /**
     * @brief Unseal the BQ27441.
     *
     * @param[out] result: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t unseal(std::uint16_t* result);

    /**
     * @brief Seal the BQ27441.
     *
     * @param[out] result: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t seal(std::uint16_t* result);

    /**
     * @brief Set block data control.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t blockDataControl();

    /**
     * @brief Set block data class.
     *
     * @param[in] result: Class ID.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t blockDataClass(std::uint8_t id);

    /**
     * @brief Set block data offset.
     *
     * @param[in] offset: Offset.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t blockDataOffset(std::uint8_t offset);

    /**
     * @brief Get block data checksum.
     *
     * @param[out] result: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t blockDataChecksum(std::uint8_t* csum);

    /**
     * @brief Read block data.
     *
     * @param[in]  offset: Offset.
     * @param[out] result: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t readBlockData(std::uint8_t offset, std::uint8_t* result);

    /**
     * @brief Write block data.
     *
     * @param[in] offset: Offset.
     * @param[in] data: Data.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t writeBlockData(std::uint8_t offset, std::uint8_t data);

    /**
     * @brief Compute block checksum.
     *
     * @param[out] result: Block checksum.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t computeBlockChecksum(std::uint8_t* checksum);

    /**
     * @brief Write block checksum.
     *
     * @param[in] csum: The checksum.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t writeBlockChecksum(std::uint8_t csum);

    /**
     * @brief Read extended data.
     *
     * @param[in]  class_id: Class ID.
     * @param[in]  offset: offset.
     * @param[out] result: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t readExtendedData(std::uint8_t class_id,
                               std::uint8_t offset,
                               std::uint8_t* result);

    /**
     * @brief Seal the BQ27441.
     *
     * @param[out] result: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t writeExtendedData(std::uint8_t class_id,
                                std::uint8_t offset,
                                std::uint8_t* data,
                                std::uint8_t len);
    /**
     * @brief Read the result of a Control Command.
     *
     * @param[in]  function: Control command.
     * @param[out] result: The return value.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t readControlWord(std::uint16_t function, std::uint16_t* result);

    /**
     * @brief Execute a Control Command.
     *
     * @param[in]  function: Control command.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - (others): failed.
     */
    esp_err_t executeControlWord(std::uint16_t function);

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
     * @brief Write bytes to I2C.
     *
     * @param[in] sub_addres: Sub address (command).
     * @param[in] bytes: Byte buffer.
     * @param[in] count: Byte buffer size.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - ESP_FAIL: `word` is a null pointer, or i2c failure.
     *      - (others): failed.
     */
    esp_err_t i2cWriteBytes(std::uint8_t sub_address,
                            std::uint8_t* bytes,
                            std::size_t count);

    /**
     * @brief Read bytes from the I2C.
     *
     * @param[in] sub_addres: Sub address (command).
     * @param[out] bytes: Byte buffer.
     * @param[in] count: Byte buffer size.
     *
     * @return
     *      - ESP_OK: succeed.
     *      - ESP_FAIL: `word` is a null pointer, or i2c failure.
     *      - (others): failed.
     */
    esp_err_t i2cReadBytes(std::uint8_t sub_address,
                           std::uint8_t* bytes,
                           std::size_t count);

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
    /// Seal again the BQ27441?
    bool m_seal_again;
    /// Indicates if the enterConfig is in manual mode.
    bool m_manual_config;
};


} // namespace esc


#endif //FUEL_GAUGE_H
