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

namespace bq27441 {

enum class CurrentMeasure {
    Average,
    StandBy,
    Max,
};

/**
 * @brief BQ7411 Battery Fuel Gaugue
 * 
 */
class BQ27441 {
public:
    /**
     * @brief Construct a new BQ27441
     * 
     */
    BQ27441();

    /**
     * @brief Initializes the object to start working with the BQ27441.
     * 
     * @param port The I2C port used, please note that this port needs to be
     * configured as a master I2C device.
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t begin(i2c_port_t port);

    /**
     * @brief Configures the battery capacity
     * 
     * @param capacity battery capacity measured in mAh
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t setCapacity(std::uint16_t capacity);

    /**
     * @brief Get battery voltage
     * 
     * @param voltage The voltage in mV unit
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t voltage(std::uint16_t& voltage);

    /**
     * @brief Get battery current
     * 
     * @param measure the way current is measured
     * @param current the current return value
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t current(CurrentMeasure measure, std::int16_t& current);

    /**
     * @brief Get the device type used to identify the IC.
     * 
     * @param result The actual device ID
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t deviceType(std::uint16_t& result);

    /**
     * @brief Get BQ27441 flags
     * 
     * @param result the flags
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t flags(std::uint16_t& result);

    /**
     * @brief Get status flags
     * 
     * @param result the status
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t status(std::uint16_t& result);

    /**
     * @brief Enter configuration mode
     * 
     * @return esp_err_t ESP_OK on success.
     */
    esp_err_t enterConfig();

    /**
     * @brief Exit configuration mode
     * 
     * @param resim whether to resimulate or no
     * @return esp_err_t ESP_OK on success.
     */
    esp_err_t exitConfig(bool resim = true);
private:
    /**
     * @brief Checks if the BQ27441 is sealed
     * 
     * @param is_sealed the return variable 
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t sealed(bool& is_sealed);

    /**
     * @brief Seal the BQ27441
     * 
     * @param result return code
     * @return esp_err_t 
     */
    esp_err_t seal(std::uint16_t& result);

    /**
     * @brief Unseal the BQ27441
     * 
     * @param result return code
     * @return esp_err_t ESP_OK on success
     */
    esp_err_t unseal(std::uint16_t& result);

    esp_err_t softReset();
    esp_err_t readWord(std::uint16_t sub_address, std::uint16_t& result);
    esp_err_t readControlWord(std::uint16_t function, std::uint16_t& result);
    esp_err_t executeControlWord(std::uint16_t function);
    esp_err_t blockDataControl();
    esp_err_t blockDataClass(std::uint8_t id);
    esp_err_t blockDataOffset(std::uint8_t offset);
    esp_err_t blockDataChecksum(std::uint8_t& csum);
    esp_err_t readBlockData(std::uint8_t offset, std::uint8_t& result);
    esp_err_t writeBlockData(std::uint8_t offset, std::uint8_t data);
    esp_err_t computeBlockChecksum(std::uint8_t& checksum);
    esp_err_t writeBlockChecksum(std::uint8_t csum);
    esp_err_t readExtendedData(std::uint8_t class_id,
                               std::uint8_t offset,
                               std::uint8_t& result);
    esp_err_t writeExtendedData(std::uint8_t class_id,
                                std::uint8_t offset,
                                std::uint8_t* data,
                                std::uint8_t len);

    esp_err_t i2cWriteBytes(std::uint8_t sub_address,
                            std::uint8_t* bytes,
                            std::size_t length);
    esp_err_t i2cReadBytes(std::uint8_t sub_addres,
                           std::uint8_t* bytes,
                           std::size_t count);

    std::uint8_t _device_address;
    i2c_port_t _port;
    bool _seal_again;
};

extern BQ27441 bq27441;

}

#endif