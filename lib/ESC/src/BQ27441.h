/**
 * @file BQ27441.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-14
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#ifndef BQ27441_H
#define BQ27441_H

#include <driver/i2c.h>

#include "BQ27441_Constants.h"

namespace esc {

const bool ACTIVE_LOW = false;
const bool ACTIVE_HIGH = true;

const bool BAT_LOW = true;
const bool SOC_INT = false;

/**
 * @brief Measuring mode of current
 * 
 */
enum class CurrentMeasure {
    Average,
    StandBy,
    Max,
};

/**
 * @brief State Of Charge Measeure mode
 * 
 */
enum class SocMeasure {
    // State of Charge Filtered
    Filtered,
    // State of Charge Unfiltered
    Unfiltered
};

/**
 * @brief BQ27411 LiPo Battery Fuel Gaugue
 * 
 * This class manages the I2C communication with the BQ27441 IC, it uses the
 * esp-idf I2C API directly. This class is not thread safe, it exists as the
 * g_bq27441 global variable
 */
class BQ27441
{
public:
    /**
     * @brief Construct a new BQ27441
     * 
     */
    BQ27441();

    /**
     * @brief Initializes the object to start working with the BQ27441
     * 
     * @param[in] port: The I2C port used, please note that this port needs to
     * be configured as a master I2C device.
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t begin(i2c_port_t port);

    /**
     * @brief Tells the IC to use this maximum battery capacity
     * 
     * @param[in] capacity: battery capacity measured in mAh
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t setCapacity(std::uint16_t capacity);

    /**
     * @brief Get battery voltage
     * 
     * @param[out] voltage: The voltage in mV unit
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t voltage(std::uint16_t& voltage);

    /**
     * @brief Get battery current in mAh unit
     * 
     * @param[in]  measure: the way current is measured
     * @param[out] current: the return value, >0 mAh means charging 
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t current(CurrentMeasure measure, std::int16_t& current);

    /**
     * @brief Get the battery State Of Charge
     * 
     * @param[in]  type: Measuring mode
     * @param[out] soc: State-Of-Charge
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t soc(SocMeasure type, std::uint16_t& soc);

    /**
     * @brief Get GPOUT pin polarity
     * 
     * @param[out] polarity: the GPOUT polarity. true means active-high, false
     * active-low
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t GPOUTPolarity(bool& polarity);

    /**
     * @brief Set GPOUT pin polarity
     * 
     * @param[in] active_high: active-high or active-low
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t setGPOUTPolarity(bool active_high);

    /**
     * @brief Get GPOUT pin function
     * 
     * @param[out] batlow_en: if true "BAT_LOW" is enabled, on false "SOC_INT"
     * is enabled
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t GPOUTFunction(bool& batlow_en);

    /**
     * @brief Set GPOUT pin function
     * 
     * @param[in] batlow_en: "BAT_LOW" or "SOC_INT"
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t setGPOUTFunction(bool batlow_en);

    /**
     * @brief Get SOCI Delta value
     * 
     * @param[out] delta: SOCI delta value 
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t sociDelta(std::uint8_t& delta);

    /**
     * @brief Set SOCI Delta value
     * 
     * @param[in] delta: SOCI delta 
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t setSOCIDelta(std::uint8_t delta);

    /**
     * @brief Pulse GPOUT
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t pulseGPOUT();

    /**
     * @brief Get the device type used to identify the IC.
     * 
     * @param[out] result: the actual device ID
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t deviceType(std::uint16_t& result);

    /**
     * @brief Get BQ27441 flags
     * 
     * @param[out] result: the flags
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t flags(std::uint16_t& result);

    /**
     * @brief Get status flags
     * 
     * @param[out] result: the status
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t status(std::uint16_t& result);

    /**
     * @brief Enter configuration mode
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t enterConfig();

    /**
     * @brief Exit configuration mode
     * 
     * @param[in] resim: whether to resimulate or no
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t exitConfig(bool resim = true);

private:
    /**
     * @brief Checks if the BQ27441 is sealed
     * 
     * @param[out] is_sealed: the return variable
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t sealed(bool& is_sealed);

    /**
     * @brief Seal the BQ27441
     * 
     * @param[out] result: return code
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t seal(std::uint16_t& result);

    /**
     * @brief Unseal the BQ27441
     * 
     * @param[out] result: return code
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t unseal(std::uint16_t& result);

    /**
     * @brief Read OpConfig value
     *
     * @param[out] result: return value
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t opConfig(std::uint16_t& result);

    /**
     * @brief Write OpConfig
     * 
     * @param value the actual value
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t writeOpConfig(std::uint16_t value);

    /**
     * @brief Issue a soft-reset
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t softReset();

    /**
     * @brief Read a word from the IC
     * 
     * @param[in]  sub_address: the sub address
     * @param[out] result: the read result
     * 
     * @return
     *      - ESP_OK: on succes 
     */
    esp_err_t readWord(std::uint16_t sub_address, std::uint16_t& result);

    /**
     * @brief Execute a "Control" function and read it's result
     * 
     * @param[in]  function: the function to execute 
     * @param[out] result: the read result
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t readControlWord(std::uint16_t function, std::uint16_t& result);

    /**
     * @brief Execute control funcion
     * 
     * @param[in] function: the control function 
     * 
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t executeControlWord(std::uint16_t function);

    /**
     * @brief Enable block data memory control
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t blockDataControl();

    /**
     * @brief Write class ID
     * 
     * @param[in] id: class id
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t blockDataClass(std::uint8_t id);

    /**
     * @brief Write block data offset
     * 
     * @param[in] offset: data offset 
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t blockDataOffset(std::uint8_t offset);

    /**
     * @brief Get block data checksum
     * 
     * @param[out] csum: the checksum
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t blockDataChecksum(std::uint8_t& csum);

    /**
     * @brief Read block data
     * 
     * @param[in]  offset: data offset
     * @param[out] result: read result 
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t readBlockData(std::uint8_t offset, std::uint8_t& result);

    /**
     * @brief 
     * 
     * @param offset 
     * @param data 
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t writeBlockData(std::uint8_t offset, std::uint8_t data);

    /**
     * @brief 
     * 
     * @param checksum 
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t computeBlockChecksum(std::uint8_t& checksum);

    /**
     * @brief 
     * 
     * @param csum 
     * 
     * @return
     *      - ESP_OK: on success 
     */
    esp_err_t writeBlockChecksum(std::uint8_t csum);

    /**
     * @brief Read extended data
     * 
     * @param[in]  class_id: class ID
     * @param[in]  offset: offset to read from
     * @param[out] result: the read result
     *
     * @return
     *      - ESP_OK: on success
     */
    esp_err_t readExtendedData(std::uint8_t class_id,
        std::uint8_t offset,
        std::uint8_t& result);

    /**
     * @brief Write extended data
     * 
     * @param[in] class_id: class ID
     * @param[in] offset: offset to write to
     * @param[in] data: data buffer
     * @param[in] len: "data" len 
     * 
     * @return
     *      - ESP_OK: on success
     *      - ESP_FAIL: if "len" is equal to 0 or higher than 32, or if "data" is null 
     */
    esp_err_t writeExtendedData(std::uint8_t class_id,
        std::uint8_t offset,
        std::uint8_t* data,
        std::uint8_t len);

    /**
     * @brief Write n-bytes to the BQ27441 using the I2C protocol
     * 
     * @param[in] sub_address: the sub address
     * @param[in] bytes: buffer with the contents to be written 
     * @param[in] count: "bytes" count
     * 
     * @return
     *      - ESP_OK: on success
     *      - ESP_FAIL: when "bytes" is null, or "count" is 0
     */
    esp_err_t i2cWriteBytes(std::uint8_t sub_address,
        std::uint8_t* bytes,
        std::size_t count);

    /**
     * @brief Read n-bytes from the BQ27441 using the I2C protcol
     * 
     * @param[in]  sub_addres: sub address
     * @param[out] bytes: buffer where bytes are going to be read
     * @param[in]  count: number of bytes to read
     * 
     * @return
     *      - ESP_OK: on success
     *      - ESP_FAIL: returned when bytes buffer is null
     */
    esp_err_t i2cReadBytes(std::uint8_t sub_addres,
        std::uint8_t* bytes,
        std::size_t count);

    std::uint8_t m_device_address;
    i2c_port_t m_port;
    bool m_seal_again;
};

} // namespace esc

#endif