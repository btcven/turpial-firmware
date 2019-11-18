/**
 * @file test_esc_bq27441.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief Test ESC library
 * @version 0.1
 * @date 2019-11-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <cstdlib>
#include <iostream>
#include <unity.h>

#include "BQ27441.h"
#include "defaults.h"

#ifdef ESC_ENABLED

using esc::BQ27441;
using esc::CurrentMeasure;
using esc::SocMeasure;

BQ27441 g_bq27441;

/**
 * @brief Initialize Battery subsystem
 * 
 */
void initBattery()
{
    esp_err_t err;
    i2c_config_t i2c_config;

    i2c_config.mode = I2C_MODE_MASTER;
    i2c_config.sda_io_num = ESC_I2C_SDA;
    i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.scl_io_num = ESC_I2C_SCL;
    i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.master.clk_speed = ESC_I2C_CLOCK_FREQ;

    err = i2c_param_config(ESC_I2C_PORT, &i2c_config);
    if (err != ESP_OK) std::abort();

    err = i2c_driver_install(ESC_I2C_PORT, i2c_config.mode, 0, 0, 0);
    if (err != ESP_OK) std::abort();

    g_bq27441.begin(ESC_I2C_PORT);
}

void testSetCapacity()
{
    esp_err_t err = g_bq27441.setCapacity(ESC_MAX_BATTERY_CAPACITY);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void testVoltage()
{
    std::uint16_t voltage;
    esp_err_t err = g_bq27441.voltage(voltage);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

void testCurrent()
{
    std::int16_t current_avg;
    std::int16_t current_stdby;
    std::int16_t current_max;
    esp_err_t err;

    err = g_bq27441.current(CurrentMeasure::Average, current_avg);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    err = g_bq27441.current(CurrentMeasure::StandBy, current_stdby);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    err = g_bq27441.current(CurrentMeasure::Max, current_max);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    std::cout << "Current Average: " << current_avg << std::endl;
    std::cout << "Current StandBy: " << current_stdby << std::endl;
    std::cout << "Current Max: " << current_max << std::endl;
}

void testSoc()
{
    std::uint16_t soc;
    esp_err_t err = g_bq27441.soc(SocMeasure::Filtered, soc);
    TEST_ASSERT_EQUAL(ESP_OK, err);
    TEST_ASSERT_LESS_OR_EQUAL(100, soc);

    std::cout << "State Of Charge: " << soc << std::endl;
}

void testGPOUTPolarity()
{
    bool polarity;
    esp_err_t err = g_bq27441.GPOUTPolarity(polarity);
    TEST_ASSERT_EQUAL(ESP_OK, err);

    std::cout << "GPOUT Polarity: " << polarity << std::endl;
}

extern "C" void app_main()
{
    initBattery();
    UNITY_BEGIN();
    RUN_TEST(testSetCapacity);
    RUN_TEST(testVoltage);
    RUN_TEST(testCurrent);
    RUN_TEST(testSoc);
    RUN_TEST(testGPOUTPolarity);
    UNITY_END();
}

#endif // ESC_ENABLED