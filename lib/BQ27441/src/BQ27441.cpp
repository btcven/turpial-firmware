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

const TickType_t TIMEOUT = 2000;

BQ27441::BQ27441()
    : _device_address(I2C_ADDRESS)
    , _port(I2C_NUM_0) {
}

esp_err_t BQ27441::begin(i2c_port_t port) {
    _port = port;

    return ESP_OK;
}

esp_err_t BQ27441::deviceType(std::uint16_t& result) {
    return readControlUint16(Control::DeviceType, result);
}

esp_err_t BQ27441::readControlUint16(Control control, std::uint16_t& result) {
    std::uint8_t cmd = raw_control(control);
    std::uint8_t sub_command_msb = static_cast<std::uint8_t>(cmd >> 8);
	std::uint8_t sub_command_lsb = static_cast<std::uint8_t>(cmd & 0x00FF);
	std::uint8_t command[2] = {sub_command_lsb, sub_command_msb};
	std::uint8_t data[2] = {0, 0};

    esp_err_t err;
    err = i2cWriteBytes(0, command, 2);
    if (err != ESP_OK) {
        ESP_LOGD(__func__, "couldn't send control command to BQ27441 (%d)", cmd);
        return err;
    }

    err = i2cReadBytes(0, data, 2);
    if(err != ESP_OK) {
        ESP_LOGD(__func__, "couldn't read control command response from BQ27441 (%d)", cmd);
        return err;
    }

    std::uint16_t d0 = static_cast<std::uint16_t>(data[1]) << 8;
    std::uint16_t d1 = static_cast<std::uint16_t>(data[0]);
    std::uint16_t res = (d1 | d0);

    return res;
}

esp_err_t BQ27441::i2cWriteBytes(std::uint8_t sub_address, std::uint8_t* bytes, std::size_t length) {
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