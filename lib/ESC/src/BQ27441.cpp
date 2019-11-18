/**
 * @file BQ27441.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief
 * @version 0.1
 * @date 2019-11-14
 *
 * @copyright Copyright (c) 2019
 *
 */

#include "BQ27441.h"

#include <cstring>
#include <esp_log.h>
#include <memory>

#define ESP_ERR_TRY(expr)     \
    do {                      \
        esp_err_t err = expr; \
        if (err != ESP_OK) {  \
            return err;       \
        }                     \
    } while (false)

namespace esc {

template <class T>
const T& constrain(const T& x, const T& a, const T& b)
{
    if (x < a) {
        return a;
    } else if (b < x) {
        return b;
    } else {
        return x;
    }
}

BQ27441::BQ27441()
    : m_device_address(I2C_ADDRESS), m_port(I2C_NUM_0), m_seal_again(false) {}

esp_err_t BQ27441::begin(i2c_port_t port)
{
    m_port = port;

    std::uint16_t type;
    esp_err_t err = deviceType(type);
    if (err != ESP_OK) {
        ESP_LOGD(__func__, "couldn't get BQ27441 device type");
        return err;
    }

    if (type != DEVICE_TYPE_ID) {
        ESP_LOGD(__func__, "invalid device type 0x%04X", type);
        return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t BQ27441::setCapacity(std::uint16_t capacity)
{
    // Write to STATE subclass (82) of BQ27441 extended memory.
    // Offset 0x0A (10)
    // Design capacity is a 2-byte piece of data - MSB first
    uint8_t cap_msb = capacity >> 8;
    uint8_t cap_lsb = capacity & 0x00FF;
    uint8_t capacity_data[2] = {cap_msb, cap_lsb};
    return writeExtendedData(BQ27441_ID_STATE, 10, capacity_data, 2);
}

esp_err_t BQ27441::voltage(std::uint16_t& voltage)
{
    return readWord(Command::VOLTAGE, voltage);
}

esp_err_t BQ27441::current(CurrentMeasure measure, std::int16_t& current)
{
    std::uint16_t ret;
    esp_err_t err;

    switch (measure) {
    case CurrentMeasure::Average:
        err = readWord(Command::AVG_CURRENT, ret);
        break;
    case CurrentMeasure::StandBy:
        err = readWord(Command::STDBY_CURRENT, ret);
        break;
    case CurrentMeasure::Max:
        err = readWord(Command::MAX_CURRENT, ret);
        break;
    default:
        return ESP_FAIL;
        break;
    }

    if (err != ESP_OK) {
        return err;
    }

    current = static_cast<std::int16_t>(ret);

    return ESP_OK;
}

esp_err_t BQ27441::soc(SocMeasure type, std::uint16_t& soc)
{
    switch (type) {
    case SocMeasure::Filtered:
        return readWord(Command::SOC, soc);
        break;
    case SocMeasure::Unfiltered:
        return readWord(Command::SOC_UNFL, soc);
        break;
    default:
        return ESP_FAIL;
        break;
    }

    return ESP_OK;
}

esp_err_t BQ27441::GPOUTPolarity(bool& polarity)
{
    std::uint16_t op_config_register;
    esp_err_t err = opConfig(op_config_register);
    if (err != ESP_OK) return err;

    polarity = !!(op_config_register & BQ27441_OPCONFIG_GPIOPOL);
    return ESP_OK;
}

esp_err_t BQ27441::setGPOUTPolarity(bool active_high)
{
    std::uint16_t old_op_config;
    esp_err_t err = opConfig(old_op_config);
    if (err != ESP_OK) return err;

    // Check to see if we need to update opConfig:
    bool alrdy_act_high = active_high && (old_op_config & BQ27441_OPCONFIG_GPIOPOL);
    bool alrdy_act_low = !active_high && !(old_op_config & BQ27441_OPCONFIG_GPIOPOL);
    if (alrdy_act_high || alrdy_act_low) return ESP_OK;

    std::uint16_t new_op_config = old_op_config;
    if (active_high) {
        new_op_config |= BQ27441_OPCONFIG_GPIOPOL;
    } else {
        new_op_config &= ~(BQ27441_OPCONFIG_GPIOPOL);
    }

    return writeOpConfig(new_op_config);
}

esp_err_t BQ27441::GPOUTFunction(bool& batlow_en)
{
    std::uint16_t op_config_register;
    esp_err_t err = opConfig(op_config_register);
    if (err != ESP_OK) return err;

    batlow_en = !!(op_config_register & BQ27441_OPCONFIG_BATLOWEN);

    return ESP_OK;
}

esp_err_t BQ27441::setGPOUTFunction(bool batlow_en)
{
    std::uint16_t old_op_config;
    esp_err_t err = opConfig(old_op_config);

    // Check to see if we need to update opConfig:
    bool is_batlow = batlow_en && (old_op_config & BQ27441_OPCONFIG_BATLOWEN);
    bool is_socint = !batlow_en && !(old_op_config & BQ27441_OPCONFIG_BATLOWEN);
    if (is_batlow || is_socint) return ESP_OK;

    // Modify BATLOWN_EN bit of opConfig:
    uint16_t new_op_config = old_op_config;
    if (batlow_en)
        new_op_config |= BQ27441_OPCONFIG_BATLOWEN;
    else
        new_op_config &= ~(BQ27441_OPCONFIG_BATLOWEN);

    // Write new opConfig
    return writeOpConfig(new_op_config);
}

esp_err_t BQ27441::sociDelta(std::uint8_t& delta)
{
    return readExtendedData(BQ27441_ID_STATE, 26, delta);
}

esp_err_t BQ27441::setSOCIDelta(std::uint8_t delta)
{
    std::uint8_t soci = constrain<std::uint8_t>(delta, 0, 100);
    return writeExtendedData(BQ27441_ID_STATE, 26, &soci, 1);
}

esp_err_t BQ27441::pulseGPOUT()
{
    return executeControlWord(Control::PULSE_SOC_INT);
}

esp_err_t BQ27441::deviceType(std::uint16_t& result)
{
    return readControlWord(Control::DEVICE_TYPE, result);
}

esp_err_t BQ27441::flags(std::uint16_t& result)
{
    return readWord(Command::FLAGS, result);
}

esp_err_t BQ27441::status(std::uint16_t& result)
{
    return readControlWord(Control::STATUS, result);
}

esp_err_t BQ27441::sealed(bool& is_sealed)
{
    std::uint16_t stat;
    ESP_ERR_TRY(status(stat));

    if (stat & BQ27441_STATUS_SS) {
        is_sealed = true;
    } else {
        is_sealed = false;
    }

    return ESP_OK;
}

esp_err_t BQ27441::seal(std::uint16_t& result)
{
    return readControlWord(Control::SEALED, result);
}

esp_err_t BQ27441::unseal(std::uint16_t& result)
{
    // To unseal the BQ27441, write the key to the control
    // command. Then immediately write the same key to control again.

    ESP_ERR_TRY(readControlWord(UNSEAL_KEY, result));

    return readControlWord(UNSEAL_KEY, result);
}

esp_err_t BQ27441::enterConfig()
{
    esp_err_t err;

    bool is_sealed;
    ESP_ERR_TRY(sealed(is_sealed));
    if (is_sealed) {
        // Must be unsealed before making changes
        std::uint16_t res;
        ESP_ERR_TRY(unseal(res));
        m_seal_again = true;
    }

    if (executeControlWord(Control::SET_CFG_UPDATE) == ESP_OK) {
        std::int16_t timeout = TIMEOUT;
        while (timeout--) {
            vTaskDelay(1);
            std::uint16_t flags_;
            ESP_ERR_TRY(flags(flags_));

            if (flags_ & BQ27441_FLAG_CFGUPMODE) {
                return ESP_OK;
            }
        }

        // Timeout wasn't reached, meaning that BQ27441_FLAG_CFGUPMODE is set.
        if (timeout > 0) {
            return ESP_OK;
        }
    }

    return ESP_FAIL;
}

esp_err_t BQ27441::exitConfig(bool resim)
{
    // There are two methods for exiting config mode:
    //    1. Execute the EXIT_CFGUPDATE command
    //    2. Execute the SOFT_RESET command
    // EXIT_CFGUPDATE exits config mode _without_ an OCV (open-circuit voltage)
    // measurement, and without resimulating to update unfiltered-SoC and SoC.
    // If a new OCV measurement or resimulation is desired, SOFT_RESET or
    // EXIT_RESIM should be used to exit config mode.
    esp_err_t err;
    if (resim) {
        ESP_ERR_TRY(softReset());
        std::int16_t timeout = TIMEOUT;
        while (timeout--) {
            vTaskDelay(1);

            std::uint16_t flags_;
            ESP_ERR_TRY(flags(flags_));

            if (!(flags_ & BQ27441_FLAG_CFGUPMODE)) {
                return ESP_OK;
            }

            timeout--;
        }

        if (timeout > 0) {
            if (m_seal_again) {
                // Seal back up if we IC was sealed coming in
                std::uint16_t res1;
                ESP_ERR_TRY(seal(res1));
            }
            return ESP_OK;
        }
    } else {
        return executeControlWord(Control::EXIT_CFGUPDATE);
    }

    return ESP_FAIL;
}

esp_err_t BQ27441::opConfig(std::uint16_t& result)
{
    return readWord(BQ27441_EXTENDED_OPCONFIG, result);
}

esp_err_t BQ27441::writeOpConfig(std::uint16_t value)
{
    uint8_t op_config_msb = value >> 8;
    uint8_t op_config_lsb = value & 0x00FF;
    uint8_t op_config_data[2] = {op_config_msb, op_config_lsb};

    // OpConfig register location: BQ27441_ID_REGISTERS id, offset 0
    return writeExtendedData(BQ27441_ID_REGISTERS, 0, op_config_data, 2);
}

esp_err_t BQ27441::softReset()
{
    return executeControlWord(Control::SOFT_RESET);
}

esp_err_t BQ27441::readWord(std::uint16_t sub_address, std::uint16_t& result)
{
    std::uint8_t data[2];
    esp_err_t err = i2cReadBytes(sub_address, data, 2);
    if (err != ESP_OK) {
        return err;
    }

    std::uint16_t d0 = static_cast<std::uint16_t>(data[1]) << 8;
    std::uint16_t d1 = static_cast<std::uint16_t>(data[0]);
    result = (d1 | d0);

    return ESP_OK;
}

esp_err_t BQ27441::readControlWord(std::uint16_t function,
    std::uint16_t& result)
{
    std::uint8_t sub_command_msb = static_cast<std::uint8_t>(function >> 8);
    std::uint8_t sub_command_lsb = static_cast<std::uint8_t>(function & 0x00FF);
    std::uint8_t command[2] = {sub_command_lsb, sub_command_msb};
    std::uint8_t data[2] = {0, 0};

    esp_err_t err;
    err = i2cWriteBytes(0, command, 2);
    if (err != ESP_OK) {
        ESP_LOGD(__func__, "couldn't send control command to BQ27441 (%d)",
            function);
        return err;
    }

    err = i2cReadBytes(0, data, 2);
    if (err != ESP_OK) {
        ESP_LOGD(__func__,
            "couldn't read control command response from BQ27441 (%d)",
            function);
        return err;
    }

    std::uint16_t d0 = static_cast<std::uint16_t>(data[1]) << 8;
    std::uint16_t d1 = static_cast<std::uint16_t>(data[0]);
    std::uint16_t res = (d1 | d0);

    return res;
}

esp_err_t BQ27441::executeControlWord(std::uint16_t function)
{
    std::uint8_t sub_command_msb = (function >> 8);
    std::uint8_t sub_command_lsb = (function & 0x00FF);
    std::uint8_t command[2] = {sub_command_lsb, sub_command_msb};

    return i2cWriteBytes(0, command, 2);
}

esp_err_t BQ27441::blockDataControl()
{
    std::uint8_t enable_byte = 0x00;
    return i2cWriteBytes(BQ27441_EXTENDED_CONTROL, &enable_byte, 1);
}

esp_err_t BQ27441::blockDataClass(std::uint8_t id)
{
    return i2cWriteBytes(BQ27441_EXTENDED_DATACLASS, &id, 1);
}

esp_err_t BQ27441::blockDataOffset(std::uint8_t offset)
{
    return i2cWriteBytes(BQ27441_EXTENDED_DATABLOCK, &offset, 1);
}

esp_err_t BQ27441::blockDataChecksum(std::uint8_t& csum)
{
    std::uint8_t new_csum;
    esp_err_t err = i2cReadBytes(BQ27441_EXTENDED_CHECKSUM, &new_csum, 1);
    if (err == ESP_OK) csum = new_csum;

    return err;
}

esp_err_t BQ27441::readBlockData(std::uint8_t offset, std::uint8_t& result)
{
    std::uint8_t ret;
    std::uint8_t address = offset + BQ27441_EXTENDED_BLOCKDATA;
    esp_err_t err = i2cReadBytes(address, &ret, 1);
    if (err == ESP_OK) result = ret;

    return err;
}

esp_err_t BQ27441::writeBlockData(std::uint8_t offset, std::uint8_t data)
{
    std::uint8_t address = offset + BQ27441_EXTENDED_BLOCKDATA;
    return i2cWriteBytes(address, &data, 1);
}

esp_err_t BQ27441::computeBlockChecksum(std::uint8_t& checksum)
{
    std::uint8_t data[32];
    ESP_ERR_TRY(i2cReadBytes(BQ27441_EXTENDED_BLOCKDATA, data, 32));

    std::uint8_t ret = 0;
    for (int i = 0; i < 32; i++) {
        ret += data[i];
    }
    ret = 255 - ret;
    checksum = ret;

    return ESP_OK;
}

esp_err_t BQ27441::writeBlockChecksum(std::uint8_t csum)
{
    return i2cWriteBytes(BQ27441_EXTENDED_CHECKSUM, &csum, 1);
}

esp_err_t BQ27441::readExtendedData(std::uint8_t class_id, std::uint8_t offset, std::uint8_t& result)
{
    ESP_ERR_TRY(enterConfig());

    // Enable block data memory control
    ESP_ERR_TRY(blockDataControl());
    // Write class ID using DataBlockClass()
    ESP_ERR_TRY(blockDataClass(class_id));

    // Write 32-bit block offset (usually 0)
    ESP_ERR_TRY(blockDataOffset(offset / 32));

    std::uint8_t new_csum;
    // Compute checksum going in
    ESP_ERR_TRY(computeBlockChecksum(new_csum));

    std::uint8_t old_csum;
    ESP_ERR_TRY(blockDataChecksum(old_csum));

    // Read from offset (limit to 0-31)
    ESP_ERR_TRY(readBlockData(offset % 32, result));

    return exitConfig();
}

esp_err_t BQ27441::writeExtendedData(std::uint8_t class_id, std::uint8_t offset, std::uint8_t* data, std::uint8_t len)
{
    if (len == 0) return ESP_FAIL;
    if (len > 32) {
        ESP_LOGD(__func__, "max write length is 32");
        return ESP_FAIL;
    }
    if (data == nullptr) return ESP_FAIL;

    ESP_ERR_TRY(enterConfig());

    // Enable block data memory control
    ESP_ERR_TRY(blockDataControl());

    // Write class ID using DataBlockClass()
    ESP_ERR_TRY(blockDataClass(class_id));

    // Write 32-bit block offset (usually 0)
    ESP_ERR_TRY(blockDataOffset(offset / 32));

    std::uint8_t computed_csum;
    // Compute checksum going in
    ESP_ERR_TRY(computeBlockChecksum(computed_csum));

    std::uint8_t old_csum;
    ESP_ERR_TRY(blockDataChecksum(old_csum));

    // Write data bytes:
    for (int i = 0; i < len; i++) {
        // Write to offset, mod 32 if offset is greater than 32
        // The blockDataOffset above sets the 32-bit block
        ESP_ERR_TRY(writeBlockData((offset % 32) + i, data[i]));
    }

    // Write new checksum using BlockDataChecksum (0x60)
    std::uint8_t new_csum;
    ESP_ERR_TRY(computeBlockChecksum(new_csum));
    ESP_ERR_TRY(writeBlockChecksum(new_csum));

    ESP_ERR_TRY(exitConfig());

    return ESP_OK;
}

esp_err_t BQ27441::i2cWriteBytes(std::uint8_t sub_address, std::uint8_t* bytes, std::size_t count)
{
    ESP_LOGD(__func__, "writing %d bytes to I2C BQ27441", count);

    if (bytes == nullptr) return ESP_FAIL;
    if (count == 0) return ESP_FAIL;

    // In case of this ever happens, use a higher write_buf array number
    // or use an allocated buffer (not necessary by the moment).
    if (count >= 5) return ESP_FAIL;

    std::uint8_t write_buf[5];

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    write_buf[0] = sub_address;
    std::memcpy(&write_buf[1], bytes, count);

    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (m_device_address << 1) | I2C_MASTER_WRITE,
        I2C_MASTER_ACK);
    i2c_master_write(cmd, write_buf, count + 1, I2C_MASTER_ACK);

    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(m_port, cmd, TIMEOUT);
    i2c_cmd_link_delete(cmd);
    return err;
}

esp_err_t BQ27441::i2cReadBytes(std::uint8_t sub_address, std::uint8_t* bytes, std::size_t count)
{
    ESP_LOGD(__func__, "reading %d bytes from BQ27441", count);

    if (bytes == nullptr) return ESP_FAIL;

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (m_device_address << 1) | I2C_MASTER_READ,
        I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, sub_address, I2C_MASTER_ACK);

    i2c_master_read(cmd, bytes, count - 1, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &bytes[count - 1], I2C_MASTER_NACK);

    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(m_port, cmd, TIMEOUT);
    i2c_cmd_link_delete(cmd);
    return err;
}

} // namespace esc