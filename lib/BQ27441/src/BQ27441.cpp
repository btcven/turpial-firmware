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

namespace bq27441 {

BQ27441::BQ27441()
    : _device_address(I2C_ADDRESS)
    , _port(I2C_NUM_0) {
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
    esp_err_t err = status(stat);
    if (err != ESP_OK) {
        return err;
    }

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
	
    esp_err_t err = readControlWord(UNSEAL_KEY, result);
    if (err != ESP_OK) {
        return err;
    }
    
    return readControlWord(UNSEAL_KEY, result);
}

esp_err_t BQ27441::enterConfig() {
    esp_err_t err;

    bool is_sealed;
    err = sealed(is_sealed);
    if (is_sealed) {
        // Must be unsealed before making changes
        std::uint16_t res;
        unseal(res);
    }

    if (executeControlWord(Control::SET_CFG_UPDATE)) {
        std::int16_t timeout = TIMEOUT;
        while (timeout--) {
            vTaskDelay(1);
            std::uint16_t flags_;
            err = flags(flags_);
            if (err != ESP_OK) {
                return err;
            }

            if (flags_ & FLAG_CFGUPMODE) {
                return ESP_OK;
            }
        }

		if (timeout > 0) {
            return ESP_OK;
        }
    }
	
    return ESP_FAIL;
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