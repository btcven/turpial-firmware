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

#include <memory>
#include <cstring>
#include <esp_log.h>

#define ESP_ERR_TRY(expr) \
do {                      \
    esp_err_t err = expr; \
    if (err != ESP_OK) {  \
        return err;       \
    }                     \
} while (false)           \

namespace bq27441 {

BQ27441::BQ27441()
    : _device_address(I2C_ADDRESS)
    , _port(I2C_NUM_0)
    , _seal_again(false) {
}

esp_err_t BQ27441::begin(i2c_port_t port) {
    _port = port;

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

esp_err_t BQ27441::setCapacity(std::uint16_t capacity) {
	// Write to STATE subclass (82) of BQ27441 extended memory.
	// Offset 0x0A (10)
	// Design capacity is a 2-byte piece of data - MSB first
	uint8_t cap_msb = capacity >> 8;
	uint8_t cap_lsb = capacity & 0x00FF;
	uint8_t capacity_data[2] = {cap_msb, cap_lsb};
	return writeExtendedData(ID_STATE, 10, capacity_data, 2);
}

esp_err_t BQ27441::voltage(std::uint16_t& voltage) {
    return readWord(Command::VOLTAGE, voltage);
}

esp_err_t BQ27441::current(CurrentMeasure measure, std::uint16_t& current) {
    switch (measure) {
    case CurrentMeasure::Average:
        return readWord(Command::AVG_CURRENT, current);
        break;
    case CurrentMeasure::StandBy:
        return readWord(Command::STDBY_CURRENT, current);
        break;
    case CurrentMeasure::Max:
        return readWord(Command::MAX_CURRENT, current);
        break;
    default:
        return ESP_FAIL;
        break;
    }

    return ESP_OK;
}

esp_err_t BQ27441::deviceType(std::uint16_t& result) {
    return readControlWord(Control::DEVICE_TYPE, result);
}

esp_err_t BQ27441::flags(std::uint16_t& result) {
	return readWord(Command::FLAGS, result);
}

esp_err_t BQ27441::status(std::uint16_t& result) {
	return readControlWord(Control::STATUS, result);
}

esp_err_t BQ27441::sealed(bool& is_sealed)
{
	std::uint16_t stat;
    ESP_ERR_TRY(status(stat));

    if (stat & STATUS_SS) {
        is_sealed = true;
    } else {
        is_sealed = false;
    }

    return ESP_OK;
}

esp_err_t BQ27441::seal(std::uint16_t& result) {
	return readControlWord(Control::SEALED, result);
}

esp_err_t BQ27441::unseal(std::uint16_t& result) {
	// To unseal the BQ27441, write the key to the control
	// command. Then immediately write the same key to control again.
	
    ESP_ERR_TRY(readControlWord(UNSEAL_KEY, result));
    
    return readControlWord(UNSEAL_KEY, result);
}

esp_err_t BQ27441::enterConfig() {
    esp_err_t err;

    bool is_sealed;
    ESP_ERR_TRY(sealed(is_sealed));
    if (is_sealed) {
        // Must be unsealed before making changes
        std::uint16_t res;
        ESP_ERR_TRY(unseal(res));
        _seal_again = true;
    }

    if (executeControlWord(Control::SET_CFG_UPDATE) == ESP_OK) {
        std::int16_t timeout = TIMEOUT;
        while (timeout--) {
            vTaskDelay(1);
            std::uint16_t flags_;
            ESP_ERR_TRY(flags(flags_));

            if (flags_ & FLAG_CFGUPMODE) {
                return ESP_OK;
            }
        }

        // Timeout wasn't reached, meaning that FLAG_CFGUPMODE is set.
		if (timeout > 0) {
            return ESP_OK;
        }
    }
	
    return ESP_FAIL;
}

// Exit configuration mode with the option to perform a resimulation
esp_err_t BQ27441::exitConfig(bool resim) {
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

            if (!(flags_ & FLAG_CFGUPMODE)) {
                return ESP_OK;
            }

            timeout--;
        }

		if (timeout > 0) {
            if (_seal_again) {
                // Seal back up if we IC was sealed coming in
                std::uint16_t res1;
                ESP_ERR_TRY(seal(res1));
            }
			return true;
		}
	} else {
		return executeControlWord(Control::EXIT_CFGUPDATE);
	}
}

// Issue a soft-reset to the BQ27441-G1A
esp_err_t BQ27441::softReset() {
	return executeControlWord(Control::SOFT_RESET);
}

esp_err_t BQ27441::readWord(std::uint16_t sub_address, std::uint16_t& result) {
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

esp_err_t BQ27441::readControlWord(std::uint16_t function, std::uint16_t& result) {
    std::uint8_t sub_command_msb = static_cast<std::uint8_t>(function >> 8);
    std::uint8_t sub_command_lsb = static_cast<std::uint8_t>(function & 0x00FF);
    std::uint8_t command[2] = {sub_command_lsb, sub_command_msb};
    std::uint8_t data[2] = {0, 0};

    esp_err_t err;
    err = i2cWriteBytes(0, command, 2);
    if (err != ESP_OK) {
        ESP_LOGD(__func__, "couldn't send control command to BQ27441 (%d)", function);
        return err;
    }

    err = i2cReadBytes(0, data, 2);
    if(err != ESP_OK) {
        ESP_LOGD(__func__, "couldn't read control command response from BQ27441 (%d)", function);
        return err;
    }

    std::uint16_t d0 = static_cast<std::uint16_t>(data[1]) << 8;
    std::uint16_t d1 = static_cast<std::uint16_t>(data[0]);
    std::uint16_t res = (d1 | d0);

    return res;
}

esp_err_t BQ27441::executeControlWord(std::uint16_t function) {
	std::uint8_t sub_command_msb = (function >> 8);
	std::uint8_t sub_command_lsb = (function & 0x00FF);
	std::uint8_t command[2] = {sub_command_lsb, sub_command_msb};

    return i2cWriteBytes(0, command, 2);
}

// Issue a BlockDataControl() command to enable BlockData access
esp_err_t BQ27441::blockDataControl() {
	std::uint8_t enable_byte = 0x00;
	return i2cWriteBytes(EXTENDED_CONTROL, &enable_byte, 1);
}

// Issue a DataClass() command to set the data class to be accessed
esp_err_t BQ27441::blockDataClass(std::uint8_t id) {
	return i2cWriteBytes(EXTENDED_DATACLASS, &id, 1);
}

// Issue a DataBlock() command to set the data block to be accessed
esp_err_t BQ27441::blockDataOffset(std::uint8_t offset) {
	return i2cWriteBytes(EXTENDED_DATABLOCK, &offset, 1);
}

// Read the current checksum using BlockDataCheckSum()
esp_err_t BQ27441::blockDataChecksum(std::uint8_t& csum) {
	std::uint8_t new_csum;
    esp_err_t err = i2cReadBytes(EXTENDED_CHECKSUM, &new_csum, 1);
    if (err == ESP_OK) {
        csum = new_csum;
    }

    return err;
}

// Use BlockData() to read a byte from the loaded extended data
esp_err_t BQ27441::readBlockData(std::uint8_t offset, std::uint8_t& result) {
    std::uint8_t ret;
    std::uint8_t address = offset + EXTENDED_BLOCKDATA;
    esp_err_t err = i2cReadBytes(address, &ret, 1);
	if (err == ESP_OK) {
        result = ret;
    }

    return err;
}

// Use BlockData() to write a byte to an offset of the loaded data
esp_err_t BQ27441::writeBlockData(std::uint8_t offset, std::uint8_t data) {
    std::uint8_t address = offset + EXTENDED_BLOCKDATA;
    return i2cWriteBytes(address, &data, 1);
}

// Read all 32 bytes of the loaded extended data and compute a 
// checksum based on the values.
esp_err_t BQ27441::computeBlockChecksum(std::uint8_t& checksum) {
    std::uint8_t data[32];
    ESP_ERR_TRY(i2cReadBytes(EXTENDED_BLOCKDATA, data, 32));

    std::uint8_t ret = 0;
    for (int i = 0; i < 32; i++) {
        ret += data[i];
    }
    ret = 255 - ret;
    checksum = ret;
	
	return ESP_OK;
}

// Use the BlockDataCheckSum() command to write a checksum value
esp_err_t BQ27441::writeBlockChecksum(std::uint8_t csum) {
	return i2cWriteBytes(EXTENDED_CHECKSUM, &csum, 1);	
}

// Read a byte from extended data specifying a class ID and position offset
esp_err_t BQ27441::readExtendedData(std::uint8_t class_id,
                                    std::uint8_t offset,
                                    std::uint8_t& result) {
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

// Write a specified number of bytes to extended data specifying a 
// class ID, position offset.
esp_err_t BQ27441::writeExtendedData(std::uint8_t class_id,
                                     std::uint8_t offset,
                                     std::uint8_t* data,
                                     std::uint8_t len) {
    if (len > 32) {
        ESP_LOGD(__func__, "max write length is 32");
        return ESP_FAIL;
    }
	
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

esp_err_t BQ27441::i2cWriteBytes(std::uint8_t sub_address,
                                 std::uint8_t* bytes,
                                 std::size_t length) {
    ESP_LOGD(__func__, "writing %d bytes to I2C BQ27441", length);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // TODO(jeandudey): can't this allocation be removed?
    std::unique_ptr<std::uint8_t[]> buf(new std::uint8_t[length + 1]);
    buf.get()[0] = sub_address;
    std::memcpy(&buf.get()[1], bytes, length);

    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (_device_address << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK);
    i2c_master_write(cmd, buf.get(), length + 1, I2C_MASTER_ACK);

    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(_port, cmd, TIMEOUT);
    i2c_cmd_link_delete(cmd);
    return err;
}

esp_err_t BQ27441::i2cReadBytes(std::uint8_t sub_address,
                                std::uint8_t* bytes,
                                std::size_t count) {
    ESP_LOGD(__func__, "reading %d bytes from BQ27441", count);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);

    i2c_master_write_byte(cmd, (_device_address << 1) | I2C_MASTER_READ, I2C_MASTER_ACK);
    i2c_master_write_byte(cmd, sub_address, I2C_MASTER_ACK);

    i2c_master_read(cmd, bytes, count - 1, I2C_MASTER_ACK);
    i2c_master_read_byte(cmd, &bytes[count - 1], I2C_MASTER_NACK);

    i2c_master_stop(cmd);

    esp_err_t err = i2c_master_cmd_begin(_port, cmd, TIMEOUT);
    i2c_cmd_link_delete(cmd);
    return err;
}

BQ27441 bq27441;

} // bq27441