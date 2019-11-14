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

namespace bq27441 {

/// Default I2C address for the BQ27441-G1A
const std::uint8_t I2C_ADDRESS = 0x55;
/// Secret code to unseal the BQ27441-G1A
const std::uint16_t UNSEAL_KEY = 0x8000;
/// Default device ID
const std::uint16_t DEVICE_ID = 0x0421;

/**
 * @brief BQ27441-G1A Commands
 * 
 */
enum class Command : std::uint8_t {
    Control = 0x00,
    Temp = 0x02,
    Voltage = 0x04,
    Flags = 0x06,
    NomCapacity = 0x08,
    AvailCapacity = 0x0A,
    RemCapacity = 0x0C,
    FullCapacity = 0x0E,
    AvgCurrent = 0x10,
    StdbyCurrent = 0x12,
    MaxCurrent = 0x14,
    AvgPower = 0x18,
    Soc = 0x1C,
    IntTemp = 0x1E,
    Soh = 0x20,
    RemCapUnfl = 0x28,
    RemCapFil = 0x2A,
    FullCapUnfl = 0x2C,
    FullCapFil = 0x2E,
    SocUnfl = 0x30
};

/**
 * @brief Control command
 * 
 */
enum class Control : std::uint8_t {
    Status = 0x00,
    DeviceType = 0x01,
    FwVersion = 0x02,
    DmCode = 0x04,
    PrevMacwrite = 0x07,
    ChemId = 0x08,
    BatInsert = 0x0C,
    BatRemove = 0x0D,
    SetHibernate = 0x11,
    ClearHibernate = 0x12,
    SetCfgUpdate = 0x13,
    ShutdownEnable = 0x1B,
    Shutdown = 0x1C,
    Sealed = 0x20,
    PulseSocInt = 0x23,
    Reset = 0x41,
    SoftReset = 0x42,
    ExitCfgUpdate = 0x43,
    ExitResim = 0x44
};

inline std::uint8_t raw_control(Control control) {
    return static_cast<std::uint8_t>(control);
}

} // bq27441

#endif // BQ27441_CONSTANTS_H