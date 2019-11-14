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

    esp_err_t begin(i2c_port_t port);

    esp_err_t deviceType(std::uint16_t& result);

private:
    esp_err_t readControlUint16(Control control, std::uint16_t& result);
    esp_err_t i2cWriteBytes(std::uint8_t sub_address, std::uint8_t* bytes, std::size_t length);
    esp_err_t i2cReadBytes(std::uint8_t sub_addres, std::uint8_t* bytes, std::size_t count);

    std::uint8_t _device_address;
    i2c_port_t _port;
};

extern BQ27441 bq27441;

}

#endif