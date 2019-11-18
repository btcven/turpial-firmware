/**
 * @file BQ27441_Constants.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-14
 * 
 * @copyright Copyright (c) 2019
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
/// I2C Timeout
const TickType_t TIMEOUT = 2000;

/**
 * @brief BQ27441 Commands
 * 
 */
enum Command {
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
 * @brief Control command
 * 
 */
enum Control {
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

const std::uint16_t BQ27441_STATUS_SHUTDOWNEN = (1 << 15);
const std::uint16_t BQ27441_STATUS_WDRESET = (1 << 14);
const std::uint16_t BQ27441_STATUS_SS = (1 << 13);
const std::uint16_t BQ27441_STATUS_CALMODE = (1 << 12);
const std::uint16_t BQ27441_STATUS_CCA = (1 << 11);
const std::uint16_t BQ27441_STATUS_BCA = (1 << 10);
const std::uint16_t BQ27441_STATUS_QMAX_UP = (1 << 9);
const std::uint16_t BQ27441_STATUS_RES_UP = (1 << 8);
const std::uint16_t BQ27441_STATUS_INITCOMP = (1 << 7);
const std::uint16_t BQ27441_STATUS_HIBERNATE = (1 << 6);
const std::uint16_t BQ27441_STATUS_SLEEP = (1 << 4);
const std::uint16_t BQ27441_STATUS_LDMD = (1 << 3);
const std::uint16_t BQ27441_STATUS_RUP_DIS = (1 << 2);
const std::uint16_t BQ27441_STATUS_VOK = (1 << 1);

const std::uint16_t BQ27441_FLAG_OT = (1 << 15);
const std::uint16_t BQ27441_FLAG_UT = (1 << 14);
const std::uint16_t BQ27441_FLAG_FC = (1 << 9);
const std::uint16_t BQ27441_FLAG_CHG = (1 << 8);
const std::uint16_t BQ27441_FLAG_OCVTAKEN = (1 << 7);
const std::uint16_t BQ27441_FLAG_ITPOR = (1 << 5);
const std::uint16_t BQ27441_FLAG_CFGUPMODE = (1 << 4);
const std::uint16_t BQ27441_FLAG_BAT_DET = (1 << 3);
const std::uint16_t BQ27441_FLAG_SOC1 = (1 << 2);
const std::uint16_t BQ27441_FLAG_SOCF = (1 << 1);
const std::uint16_t BQ27441_FLAG_DSG = (1 << 0);

const std::uint8_t BQ27441_EXTENDED_OPCONFIG = 0x3A;
const std::uint8_t BQ27441_EXTENDED_CAPACITY = 0x3C;
const std::uint8_t BQ27441_EXTENDED_DATACLASS = 0x3E;
const std::uint8_t BQ27441_EXTENDED_DATABLOCK = 0x3F;
const std::uint8_t BQ27441_EXTENDED_BLOCKDATA = 0x40;
const std::uint8_t BQ27441_EXTENDED_CHECKSUM = 0x60;
const std::uint8_t BQ27441_EXTENDED_CONTROL = 0x61;

const std::uint8_t BQ27441_ID_SAFETY = 2;
const std::uint8_t BQ27441_ID_CHG_TERMINATION = 36;
const std::uint8_t BQ27441_ID_CONFIG_DATA = 48;
const std::uint8_t BQ27441_ID_DISCHARGE = 49;
const std::uint8_t BQ27441_ID_REGISTERS = 64;
const std::uint8_t BQ27441_ID_POWER = 68;
const std::uint8_t BQ27441_ID_IT_CFG = 80;
const std::uint8_t BQ27441_ID_CURRENT_THRESH = 81;
const std::uint8_t BQ27441_ID_STATE = 82;
const std::uint8_t BQ27441_ID_R_A_RAM = 89;
const std::uint8_t BQ27441_ID_CALIB_DATA = 104;
const std::uint8_t BQ27441_ID_CC_CAL = 105;
const std::uint8_t BQ27441_ID_CURRENT = 107;
const std::uint8_t BQ27441_ID_CODES = 112;

const std::uint16_t BQ27441_OPCONFIG_BIE = (1 << 13);
const std::uint16_t BQ27441_OPCONFIG_BI_PU_EN = (1 << 12);
const std::uint16_t BQ27441_OPCONFIG_GPIOPOL = (1 << 11);
const std::uint16_t BQ27441_OPCONFIG_SLEEP = (1 << 5);
const std::uint16_t BQ27441_OPCONFIG_RMFCC = (1 << 4);
const std::uint16_t BQ27441_OPCONFIG_BATLOWEN = (1 << 2);
const std::uint16_t BQ27441_OPCONFIG_TEMPS = (1 << 0);

} // namespace esc

#endif // BQ27441_CONSTANTS_H