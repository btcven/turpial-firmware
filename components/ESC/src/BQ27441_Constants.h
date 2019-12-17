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

/**
 * @brief Default I2C address for the BQ27441
 * 
 */
const std::uint8_t I2C_ADDRESS = 0x55;

/**
 * @brief Secret code to unseal the BQ27441
 * 
 */
const std::uint16_t UNSEAL_KEY = 0x8000;

/**
 * @brief Device type
 * 
 */
const std::uint16_t DEVICE_TYPE_ID = 0x0421;

/**
 * @brief BQ27441 Standard Command
 * 
 */
enum command {
    CONTROL = 0x00,
    TEMP = 0x02,
    VOLTAGE = 0x04,
    FLAGS = 0x06,
    NOM_CAPACITY = 0x08,
    AVAIL_CAPACITY = 0x0A,
    REM_CAPACITY = 0x0C,
    FULL_CAPACITY = 0x0E,
    AVG_CURRENT = 0x10,
    STDBY_CURRENT = 0x12,
    MAX_CURRENT = 0x14,
    AVG_POWER = 0x18,
    SOC = 0x1C,
    INT_TEMP = 0x1E,
    SOH = 0x20,
    REM_CAP_UNFL = 0x28,
    REM_CAP_FIL = 0x2A,
    FULL_CAP_UNFL = 0x2C,
    FULL_CAP_FIL = 0x2E,
    SOC_UNFL = 0x30
};

/**
 * @brief Control commands
 * 
 */
enum control {
    STATUS = 0x00,
    DEVICE_TYPE = 0x01,
    FW_VERSION = 0x02,
    DM_CODE = 0x04,
    PREV_MACWRITE = 0x07,
    CHEM_ID = 0x08,
    BAT_INSERT = 0x0C,
    BAT_REMOVE = 0x0D,
    SET_HIBERNATE = 0x11,
    CLEAR_HIBERNATE = 0x12,
    SET_CFG_UPDATE = 0x13,
    SHUTDOWN_ENABLE = 0x1B,
    SHUTDOWN = 0x1C,
    SEALED = 0x20,
    PULSE_SOC_INT = 0x23,
    RESET = 0x41,
    SOFT_RESET = 0x42,
    EXIT_CFGUPDATE = 0x43,
    EXIT_RESIM = 0x44
};

} // namespace esc

#endif //BQ27441_CONSTANTS_H