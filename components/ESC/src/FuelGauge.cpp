/**
 * @file FuelGauge.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-12-17
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project 
 * 
 */

#include "FuelGauge.h"

#include <esp_err.h>
#include <esp_log.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>

#include "BQ27441_Constants.h"

namespace esc {

static const char* TAG = "ESC";

/**
 * @brief Enable ack check for master
 */
static const bool I2C_MASTER_ACK_EN = true;   

/**
 * @brief Disable ack check for master
 */
static const bool I2C_MASTER_ACK_DIS = false; 

static const i2c_port_t I2C_PORT = I2C_NUM_0;
static const int        I2C_FREQUENCY = 400000;
static const gpio_num_t I2C_SDA_PIN = GPIO_NUM_23;
static const gpio_num_t I2C_SCL_PIN = GPIO_NUM_22;

static const std::int32_t TIMEOUT = 2000;


FuelGauge::FuelGauge()
    : m_conf{}
{
    m_conf.mode = I2C_MODE_MASTER;
    m_conf.sda_io_num = I2C_SDA_PIN;
    m_conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    m_conf.scl_io_num = I2C_SCL_PIN;
    m_conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    m_conf.master.clk_speed = I2C_FREQUENCY;
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

esp_err_t FuelGauge::readWord(std::uint8_t command, std::uint16_t* word)
{
    std::uint8_t data[2] = {0};
    esp_err_t err = ESP_FAIL;

    if (word == nullptr) return ESP_FAIL;

    err = i2cInit();
    if (err != ESP_OK) return err;

    err = i2c_set_timeout(I2C_PORT, 14000);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Can't set timeout");
        return err;
    }

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,
                          (FUELGAUGE_ADDRESS << 1) | I2C_MASTER_WRITE,
                          I2C_MASTER_ACK_EN);
    i2c_master_write_byte(cmd, command, I2C_MASTER_ACK_EN);
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd,
                          (FUELGAUGE_ADDRESS << 1) | I2C_MASTER_READ,
                          I2C_MASTER_ACK_EN);
    i2c_master_read(cmd, data, 2, I2C_MASTER_LAST_NACK);
    i2c_master_stop(cmd);

    err = i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(TIMEOUT));

    i2c_cmd_link_delete(cmd);

    // Ignore timeout errors.
    if (err != ESP_OK && err != ESP_ERR_TIMEOUT) {
        ESP_LOGE(TAG, "I2C command failed, err = %s",
                 esp_err_to_name(err));
        return err;
    }

    err = i2cDelete();
    if (err != ESP_OK) return err;
    
    *word = (data[1] << 8) | data[0];

    return ESP_OK;
}

esp_err_t FuelGauge::i2cInit()
{
    esp_err_t err = ESP_FAIL;

    err = i2c_param_config(I2C_PORT, &m_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Config failed, err = %s", esp_err_to_name(err));
        return err;
    }

    err = i2c_driver_install(I2C_PORT, m_conf.mode, 0, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Driver install failed, err = %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

esp_err_t FuelGauge::i2cDelete()
{
    esp_err_t err = i2c_driver_delete(I2C_PORT);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Delete failed, err = %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

} // namespace esc
