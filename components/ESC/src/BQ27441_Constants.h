/**
 * @file BQ27441_Constants.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-12-17
 * 
 * @copyright Copyright (c) 2019 Locha Mesh Project
 * 
 */
#ifndef BQ27441_CONSTANTS_H
#define BQ27441_CONSTANTS_H

#include <cstdint>

namespace esc {

/// Default I2C address for the BQ27441
const std::uint8_t I2C_ADDRESS = 0x55;
/// Secret code to unseal the BQ27441
const std::uint16_t UNSEAL_KEY = 0x8000;
/// Device type
const std::uint16_t DEVICE_TYPE_ID = 0x0421;


} // namespace esc

#endif //BQ27441_CONSTANTS_H