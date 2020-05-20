/**
 * Copyright 2020 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/** 
 * @file FuelGauge.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 *  
 */

#include "FuelGauge.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>

#include <ErrUtil.h>

#include "BQ27441_Constants.h"

namespace esc {

static const char* TAG = "ESC";

static const i2c_port_t I2C_PORT = I2C_NUM_0;
static const int        I2C_FREQUENCY = 100000;
static const gpio_num_t I2C_SDA_PIN = GPIO_NUM_23;
static const gpio_num_t I2C_SCL_PIN = GPIO_NUM_22;

static const TickType_t I2C_TIMEOUT = 2000;

template <typename T>
static T constrain(T value, T min, T max)
{
    if (value < min) { return min; }
    if (value > max) { return max; }

    return value;
}

FuelGauge::FuelGauge()
    : m_conf{},
      m_seal_again(false),
      m_manual_config(false)
{
    m_conf.mode = I2C_MODE_MASTER;
    m_conf.sda_io_num = I2C_SDA_PIN;
    m_conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    m_conf.scl_io_num = I2C_SCL_PIN;
    m_conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    m_conf.master.clk_speed = I2C_FREQUENCY;
}

esp_err_t FuelGauge::temperature(TempMeasure type, std::uint16_t* temp)
{
	switch (type)
	{
    case TempMeasure::Battery:
		ESP_ERR_TRY(readWord(TEMP, temp));
		break;
    case TempMeasure::Internal:
		ESP_ERR_TRY(readWord(INT_TEMP, temp));
		break;
	}

	return ESP_OK;
}

esp_err_t FuelGauge::voltage(std::uint16_t* voltage)
{
    return readWord(VOLTAGE, voltage);
}

esp_err_t FuelGauge::avgCurrent(std::int16_t* avg_current)
{
    return readWord(AVG_CURRENT, reinterpret_cast<std::uint16_t*>(avg_current));
}

esp_err_t FuelGauge::avgPower(std::int16_t* avg_power)
{
    return readWord(AVG_POWER, reinterpret_cast<std::uint16_t*>(avg_power));
}

esp_err_t FuelGauge::soc(SocMeasure type, std::uint16_t* soc)
{
    switch (type) {
    case SocMeasure::Filtered:
        return readWord(SOC, soc);
        break;
    case SocMeasure::Unfiltered:
        return readWord(SOC_UNFL, soc);
        break;
    default:
        return ESP_FAIL;
        break;
    }

    return ESP_OK;
}

esp_err_t FuelGauge::setCapacity(std::uint16_t capacity)
{
    // Write to STATE subclass (82) of FuelGauge extended memory.
    // Offset 0x0A (10)
    // Design capacity is a 2-byte piece of data - MSB first
    std::uint8_t cap_msb = capacity >> 8;
    std::uint8_t cap_lsb = capacity & 0x00FF;
    std::uint8_t capacity_data[2] = {cap_msb, cap_lsb};
    return writeExtendedData(ID_STATE, 10, capacity_data, 2);
}

esp_err_t FuelGauge::status(std::uint16_t* result)
{
    return readControlWord(STATUS, result);
}

esp_err_t FuelGauge::flags(std::uint16_t* result)
{
    return readWord(FLAGS, result);
}

esp_err_t FuelGauge::GPOUTPolarity(bool* value)
{
    if (value == nullptr) return ESP_FAIL;

    std::uint16_t opconfig = 0;
    ESP_ERR_TRY(opConfig(&opconfig));

    *value = (opconfig & OPCONFIG_GPIOPOL) == OPCONFIG_GPIOPOL;

    return ESP_OK;
}

esp_err_t FuelGauge::setGPOUTPolarity(bool active_high)
{
    std::uint16_t old_opconfig = 0;
    ESP_ERR_TRY(opConfig(&old_opconfig));

    bool already_high = (active_high && (old_opconfig & OPCONFIG_GPIOPOL));
    bool already_low = (!active_high && !(old_opconfig & OPCONFIG_GPIOPOL));

    if (already_high || already_low) return ESP_OK;

    uint16_t new_opconfig = old_opconfig;
    if (active_high) {
        new_opconfig |= OPCONFIG_GPIOPOL;
    } else {
        new_opconfig &= ~(OPCONFIG_GPIOPOL);
    }

    ESP_ERR_TRY(writeOpConfig(new_opconfig));

    return ESP_OK;
}

esp_err_t FuelGauge::GPOUTFunction(bool* function)
{
    if (function == nullptr) return ESP_FAIL;

    std::uint16_t opconfig = 0;
    ESP_ERR_TRY(opConfig(&opconfig));

    *function = ((opconfig & OPCONFIG_BATLOWEN) == OPCONFIG_BATLOWEN);

    return ESP_OK;
}

esp_err_t FuelGauge::setGPOUTFunction(bool function)
{
    std::uint16_t old_opconfig = 0;
    ESP_ERR_TRY(opConfig(&old_opconfig));

    bool is_bat_low = (function && (old_opconfig & OPCONFIG_BATLOWEN));
    bool is_soc_int = (!function && !(old_opconfig & OPCONFIG_BATLOWEN));
    if (is_bat_low || is_soc_int) return ESP_OK;

    std::uint16_t new_opconfig = old_opconfig;
    if (function == BAT_LOW) {
        new_opconfig |= OPCONFIG_BATLOWEN;
    } else {
        new_opconfig &= ~(OPCONFIG_BATLOWEN);
    }

    ESP_ERR_TRY(writeOpConfig(new_opconfig));

    return ESP_OK;
}

esp_err_t FuelGauge::sociDelta(std::uint8_t* value)
{
    if (value == nullptr) return ESP_FAIL;

    std::uint8_t soci = 0;
    ESP_ERR_TRY(readExtendedData(ID_STATE, 26, &soci));

    return ESP_OK;
}

esp_err_t FuelGauge::setSOCIDelta(std::uint8_t delta)
{
    std::uint8_t soci = constrain<std::uint8_t>(delta, 0, 100);
    return writeExtendedData(ID_STATE, 26, &soci, 1);
}

esp_err_t FuelGauge::pulseGPOUT()
{
    return executeControlWord(PULSE_SOC_INT);
}

esp_err_t FuelGauge::sealed(bool* is_sealed)
{
    std::uint16_t stat = 0;
    ESP_ERR_TRY(status(&stat));
    *is_sealed = (stat & STATUS_SS) == STATUS_SS;

    return ESP_OK;
}

esp_err_t FuelGauge::seal(std::uint16_t* result)
{
    return readControlWord(SEALED, result);
}

esp_err_t FuelGauge::unseal(std::uint16_t* result)
{
    // To unseal the FuelGauge, write the key to the control
    // command. Then immediately write the same key to control again.

    ESP_ERR_TRY(readControlWord(UNSEAL_KEY, result));

    return readControlWord(UNSEAL_KEY, result);
}

esp_err_t FuelGauge::enterConfig(bool manual_config)
{
    bool is_sealed = false;
    ESP_ERR_TRY(sealed(&is_sealed));

    if (is_sealed) {
        // Must be unsealed before making changes
        std::uint16_t res = 0;
        ESP_ERR_TRY(unseal(&res));

        m_seal_again = true;

        // Wait 2 s to get values updated.
        vTaskDelay(pdMS_TO_TICKS(2000));
    }

    ESP_ERR_TRY(executeControlWord(SET_CFG_UPDATE));

    std::int16_t timeout = I2C_TIMEOUT;
    while (timeout != 0) {
        vTaskDelay(1);

        std::uint16_t flags_res = 0;
        ESP_ERR_TRY(flags(&flags_res));

        if (flags_res & FLAG_CFGUPMODE) {
            // We are now in CFG update mode.
            m_manual_config = manual_config;
            return ESP_OK;
        }

        timeout -= 1;
    }

    // Timeout reached, no change in mode.

    return ESP_FAIL;
}

esp_err_t FuelGauge::exitConfig(bool resim)
{
    // There are two methods for exiting config mode:
    //    1. Execute the EXIT_CFGUPDATE command
    //    2. Execute the SOFT_RESET command
    // EXIT_CFGUPDATE exits config mode _without_ an OCV (open-circuit voltage)
    // measurement, and without resimulating to update unfiltered-SoC and SoC.
    // If a new OCV measurement or resimulation is desired, SOFT_RESET or
    // EXIT_RESIM should be used to exit config mode.
    esp_err_t err = ESP_FAIL;
    if (resim) {
        err = softReset();
        if (err != ESP_OK) return err;

        std::int16_t timeout = I2C_TIMEOUT;
        while (timeout != 0) {
            vTaskDelay(1);

            std::uint16_t flags_res = 0;
            err = flags(&flags_res);
            if (err != ESP_OK) return err;

            if (!(flags_res & FLAG_CFGUPMODE)) {
                if (m_seal_again) {
                    // Seal back up if we IC was sealed coming in
                    std::uint16_t seal_res = 0;
                    err = seal(&seal_res);
                    if (err != ESP_OK) return err;

                    // Wait 2 s to get values updated.
                    vTaskDelay(pdMS_TO_TICKS(2000));
                }

                m_manual_config = false;
                return ESP_OK;
            }

            timeout -= 1;
        }
    } else {
        m_manual_config = false;
        err = executeControlWord(EXIT_CFGUPDATE);
        if (err != ESP_OK) return err;
        return ESP_OK;
    }

    return ESP_FAIL;
}

esp_err_t FuelGauge::opConfig(std::uint16_t* result)
{
    return readWord(EXTENDED_OPCONFIG, result);
}

esp_err_t FuelGauge::writeOpConfig(std::uint16_t value)
{
    std::uint8_t op_config_msb = value >> 8;
    std::uint8_t op_config_lsb = value & 0x00FF;
    std::uint8_t op_config_data[2] = {op_config_msb, op_config_lsb};

    // OpConfig register location: ID_REGISTERS id, offset 0
    return writeExtendedData(ID_REGISTERS, 0, op_config_data, 2);
}

esp_err_t FuelGauge::blockDataControl()
{
    std::uint8_t enable_byte = 0x00;
    return i2cWriteBytes(EXTENDED_CONTROL, &enable_byte, 1);
}

esp_err_t FuelGauge::blockDataClass(std::uint8_t id)
{
    return i2cWriteBytes(EXTENDED_DATACLASS, &id, 1);
}

esp_err_t FuelGauge::blockDataOffset(std::uint8_t offset)
{
    return i2cWriteBytes(EXTENDED_DATABLOCK, &offset, 1);
}

esp_err_t FuelGauge::blockDataChecksum(std::uint8_t* csum)
{
    if (csum == nullptr) return ESP_FAIL;

    std::uint8_t new_csum = 0;
    ESP_ERR_TRY(i2cReadBytes(EXTENDED_CHECKSUM, &new_csum, 1));
    *csum = new_csum;

    return ESP_OK;
}

esp_err_t FuelGauge::readBlockData(std::uint8_t offset, std::uint8_t* result)
{
    if (result == nullptr) return ESP_FAIL;

    std::uint8_t ret = 0;
    std::uint8_t address = EXTENDED_BLOCKDATA + offset;
    ESP_ERR_TRY(i2cReadBytes(address, &ret, 1));
    *result = ret;

    return ESP_OK;
}

esp_err_t FuelGauge::writeBlockData(std::uint8_t offset, std::uint8_t data)
{
    std::uint8_t address = EXTENDED_BLOCKDATA + offset;
    return i2cWriteBytes(address, &data, 1);
}

esp_err_t FuelGauge::computeBlockChecksum(std::uint8_t* checksum)
{
    if (checksum == nullptr) return ESP_FAIL;

    std::uint8_t data[32] = {0};
    ESP_ERR_TRY(i2cReadBytes(EXTENDED_BLOCKDATA, data, 32));

    std::uint8_t ret = 0;
    for (int i = 0; i < 32; i++) {
        ret += data[i];
    }
    *checksum = 255 - ret;

    return ESP_OK;
}

esp_err_t FuelGauge::writeBlockChecksum(std::uint8_t csum)
{
    return i2cWriteBytes(EXTENDED_CHECKSUM, &csum, 1);
}

esp_err_t FuelGauge::readExtendedData(std::uint8_t class_id,
                                      std::uint8_t offset,
                                      std::uint8_t* result)
{
    if (!m_manual_config) ESP_ERR_TRY(enterConfig(false));

    // Enable block data memory control
    ESP_ERR_TRY(blockDataControl());
    // Write class ID using DataBlockClass()
    ESP_ERR_TRY(blockDataClass(class_id));

    // Write 32-bit block offset (usually 0)
    ESP_ERR_TRY(blockDataOffset(offset / 32));

    std::uint8_t new_csum = 0;
    // Compute checksum going in
    ESP_ERR_TRY(computeBlockChecksum(&new_csum));

    std::uint8_t old_csum = 0;
    ESP_ERR_TRY(blockDataChecksum(&old_csum));

    // Read from offset (limit to 0-31)
    ESP_ERR_TRY(readBlockData(offset % 32, result));

    if (!m_manual_config) ESP_ERR_TRY(exitConfig(false));

    return ESP_OK;
}

esp_err_t FuelGauge::writeExtendedData(std::uint8_t class_id,
                                       std::uint8_t offset,
                                       std::uint8_t* data,
                                       std::uint8_t len)
{
    if (len == 0) return ESP_FAIL;
    if (len > 32) {
        ESP_LOGE(TAG, "max write length is 32");
        return ESP_FAIL;
    }
    if (data == nullptr) return ESP_FAIL;

    if (!m_manual_config) ESP_ERR_TRY(enterConfig(false));

    // Enable block data memory control
    ESP_ERR_TRY(blockDataControl());

    // Write class ID using DataBlockClass()
    ESP_ERR_TRY(blockDataClass(class_id));

    // Write 32-bit block offset (usually 0)
    ESP_ERR_TRY(blockDataOffset(offset / 32));

    /// Wait 2s to get values updated.
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Compute checksum going in
    std::uint8_t computed_csum = 0;
    ESP_ERR_TRY(computeBlockChecksum(&computed_csum));

    std::uint8_t old_csum = 0;
    ESP_ERR_TRY(blockDataChecksum(&old_csum));

    // Write data bytes:
    for (int i = 0; i < len; i++) {
        // Write to offset, mod 32 if offset is greater than 32
        // The blockDataOffset above sets the 32-bit block
        ESP_ERR_TRY(writeBlockData((offset % 32) + i, data[i]));
    }

    /// Wait 2s to get values updated.
    vTaskDelay(pdMS_TO_TICKS(2000));

    // Write new checksum using BlockDataChecksum (0x60)
    std::uint8_t new_csum = 0;
    ESP_ERR_TRY(computeBlockChecksum(&new_csum));

    ESP_ERR_TRY(writeBlockChecksum(new_csum));

    /// Wait 2s to get values updated.
    vTaskDelay(pdMS_TO_TICKS(2000));

    if (!m_manual_config) ESP_ERR_TRY(exitConfig(false));

    return ESP_OK;
}

esp_err_t FuelGauge::softReset()
{
    return executeControlWord(SOFT_RESET);
}

esp_err_t FuelGauge::readControlWord(std::uint16_t function,
                                     std::uint16_t* result)
{

    if (result == nullptr) return ESP_FAIL;

    std::uint8_t sub_command_lsb = static_cast<std::uint8_t>(function & 0x00FF);
    std::uint8_t sub_command_msb = static_cast<std::uint8_t>(function >> 8);
    std::uint8_t command[2] = {sub_command_lsb, sub_command_msb};
    ESP_ERR_TRY(i2cWriteBytes(0, command, 2));
    std::uint8_t data[2] = {0};
    ESP_ERR_TRY(i2cReadBytes(0, data, 2));

    std::uint16_t d0 = static_cast<std::uint16_t>(data[1]) << 8;
    std::uint16_t d1 = static_cast<std::uint16_t>(data[0]);
    *result = (d1 | d0);

    return ESP_OK;
}

esp_err_t FuelGauge::executeControlWord(std::uint16_t function)
{
    std::uint8_t sub_command_msb = (function >> 8);
    std::uint8_t sub_command_lsb = (function & 0x00FF);
    std::uint8_t command[2] = {sub_command_lsb, sub_command_msb};

    ESP_ERR_TRY(i2cWriteBytes(0, command, 2));

    return ESP_OK;
}

esp_err_t FuelGauge::readWord(std::uint8_t command, std::uint16_t* word)
{

    if (word == nullptr) return ESP_FAIL;

    std::uint8_t data[2] = {0};
    ESP_ERR_TRY(i2cReadBytes(command, data, 2));

    std::uint16_t d0 = static_cast<std::uint16_t>(data[1]) << 8;
    std::uint16_t d1 = static_cast<std::uint16_t>(data[0]);
    *word = (d1 | d0);

    return ESP_OK;
}

esp_err_t FuelGauge::i2cWriteBytes(std::uint8_t sub_address,
                                   std::uint8_t* bytes,
                                   std::size_t count)
{
    if (bytes == nullptr) return ESP_FAIL;
    if (count == 0) return ESP_FAIL;

    ESP_ERR_TRY(i2cInit());

    ESP_ERR_TRY(i2c_set_timeout(I2C_PORT, 14000));

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);

    i2c_master_write_byte(cmd,
                          (FUELGAUGE_ADDRESS << 1) | I2C_MASTER_WRITE,
                          I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, sub_address, I2C_MASTER_ACK);

    i2c_master_write(cmd, bytes, count, I2C_MASTER_ACK);

    i2c_master_stop(cmd);

    ESP_ERR_TRY(i2c_master_cmd_begin(I2C_PORT, cmd, I2C_TIMEOUT));

    i2c_cmd_link_delete(cmd);

    ESP_ERR_TRY(i2cDelete());

    return ESP_OK;
}

esp_err_t FuelGauge::i2cReadBytes(std::uint8_t sub_address,
                                  std::uint8_t* bytes,
                                  std::size_t count)
{
    if (bytes == nullptr) return ESP_FAIL;
    if (count == 0) return ESP_FAIL;

    ESP_ERR_TRY(i2cInit());

    ESP_ERR_TRY(i2c_set_timeout(I2C_PORT, 14000));

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,
                          (FUELGAUGE_ADDRESS << 1) | I2C_MASTER_WRITE,
                          I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, sub_address, I2C_MASTER_ACK);

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,
                          (FUELGAUGE_ADDRESS << 1) | I2C_MASTER_READ,
                          I2C_MASTER_ACK);
    i2c_master_read(cmd, bytes, count, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    ESP_ERR_TRY(i2c_master_cmd_begin(I2C_PORT, cmd, I2C_TIMEOUT));

    i2c_cmd_link_delete(cmd);

    ESP_ERR_TRY(i2cDelete());

    return ESP_OK;
}

esp_err_t FuelGauge::i2cInit()
{
    ESP_ERR_TRY(i2c_param_config(I2C_PORT, &m_conf));
    ESP_ERR_TRY(i2c_driver_install(I2C_PORT, m_conf.mode, 0, 0, 0));

    return ESP_OK;
}

esp_err_t FuelGauge::i2cDelete()
{
    ESP_ERR_TRY(i2c_driver_delete(I2C_PORT));

    return ESP_OK;
}

} // namespace esc
