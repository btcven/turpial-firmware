#include "FuelGauge.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"
#include <cstdlib>
#include <cstring>


namespace esc {

static const char* TAG = "ESC";


FuelGauge::FuelGauge()
{
    m_conf.mode = esc_i2c_mode;
    m_conf.sda_io_num = esc_sda_pin;
    m_conf.sda_pullup_en = GPIO_PULLUP_DISABLE;
    m_conf.scl_io_num = esc_scl_pin;
    m_conf.scl_pullup_en = GPIO_PULLUP_DISABLE;
    m_conf.master.clk_speed = esc_i2c_frequency;
}

esp_err_t FuelGauge::i2cInit()
{
    esp_err_t res;
    res = i2c_param_config(m_i2c_master_port, &m_conf);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Config: %s", esp_err_to_name(res));
        return res;
    }
    res = i2c_driver_install(m_i2c_master_port, m_conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Driver install: %s", esp_err_to_name(res));
    }
    return res;
}

esp_err_t FuelGauge::i2cDelete()
{
    esp_err_t res = i2c_driver_delete(m_i2c_master_port);
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "Delete %s", esp_err_to_name(res));
    }
    return res;
}

uint16_t FuelGauge::readWord(uint8_t command)
{
    uint8_t data[2];

    // init i2c interface
    i2cInit();
    // set timeout
    i2c_set_timeout(m_i2c_master_port, 14000);

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // i2c start
    i2c_master_start(cmd);
    // Select the slave by address
    i2c_master_write_byte(cmd, (0x55 << 1) | I2C_MASTER_WRITE, I2C_MASTER_ACK_EN);
    i2c_master_write_byte(cmd, command, I2C_MASTER_ACK_EN);
    // Start read
    i2c_master_start(cmd);
    // Select the slave by addr
    i2c_master_write_byte(cmd, (fuelgauge_addr << 1) | I2C_MASTER_READ, I2C_MASTER_ACK_EN);
    // Read data from slave
    i2c_master_read(cmd, data, 2, I2C_MASTER_LAST_NACK);
    // Stop transaction
    i2c_master_stop(cmd);
    // Begin transaction
    esp_err_t res = i2c_master_cmd_begin(m_i2c_master_port, cmd, (m_timeout < 0 ? m_ticksToWait : pdMS_TO_TICKS(m_timeout)));
    if (res != ESP_OK) {
        ESP_LOGE(TAG, "transaction Begin: %s", esp_err_to_name(res));
    }
    // i2c driver delete
    i2cDelete();

    return (data[1] << 8) | data[0];
}

int16_t FuelGauge::getVoltage()
{
    return readWord(0x04);
}

int16_t FuelGauge::getAvgCurrent()
{
    return readWord(0x10);
}

int16_t FuelGauge::getAvgPower()
{
    return readWord(0x18);
}

FuelGauge::~FuelGauge()
{
}

} // namespace esc