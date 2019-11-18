/**
 * @file Battery.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "Battery.h"

#include <esp_log.h>
#include <gpio/driver.h>

namespace esc {

Battery::Battery() : m_bq27441() {}

esp_err_t Battery::begin(BatteryConfig battery_config)
{
    esp_err_t err;

    i2c_config_t i2c_config;
    i2c_config.mode = I2C_MODE_MASTER;
    i2c_config.sda_io_num = battery_config.sda;
    i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.scl_io_num = battery_config.scl;
    i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.master.clk_speed = battery_config.clk_speed;

    err = i2c_param_config(battery_config.port, &i2c_config);
    if (err != ESP_OK) {
        ESP_LOGD("can't configure I2C configuration parameters");
        return err;
    }

    err = i2c_driver_install(battery_config.port, i2c_config.mode, 0, 0, 0);
    if (err != ESP_OK) {
        ESP_LOGD("can't install I2C driver")
        return err;
    }

    err = m_bq27441.begin(battery_config.port);
    if (err != ESP_OK) {
        ESP_LOGD("couldn't init BQ27441");
        return err;
    }

    std::uint64_t pin = static_cast<std::uint64_t>(battery_config.gpout);
    gpio_config_t gpout_config;

    gpout_config.pin_bit_mask = (1ull << pin);
    gpout_config.mode = GPIO_MODE_INPUT;
    gpout_config.pull_up_en = GPIO_PULLUP_ENABLE;
    gpout_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    gpout_config.intr_type = GPIO_INTR_LOW_LEVEL;

    err = gpio_config(&gpout_config);
    if (err != ESP_OK) {
        ESP_LOGD("couldn't configure GPOUT input pin");
        return err;
    }

    err = gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    if (err != ESP_OK) {
        ESP_LOGD("couldn't install GPIO ISR handler");
        return err;
    }

    void* arg = reinterpret_cast<void*>(this);
    gpio_isr_handler_add(battery_config.gpout, Battery::interrupt_handler, arg);

    m_bq27441.enterConfig(true);
    m_bq27441.setCapacity(battery_config.capacity);
    m_bq27441.setGPOUTPolarity(ACTIVE_LOW);
    m_bq27441.setGPOUTFunction(SOC_INT);
    m_bq27441.setSOCIDelta(battery_config.percentage_interval);
    m_bq27441.exitConfig();

    return ESP_OK;
}

void Battery::interrupt_handler(void* arg)
{
    Battery* battery = reinterpret_cast<Battery*>(arg);

    // TODO: handle event
}

} // namespace esc