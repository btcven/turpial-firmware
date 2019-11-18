/**
 * @file test_esc_battery.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-18
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include <unity.h>

#include "Battery.h"
#include "defaults.h"

#ifdef ESC_ENABLED

esc::Battery g_battery;

void testBegin()
{
    esc::BatteryConfig battery_config;
    battery_config.sda = ESC_I2C_SDA;
    battery_config.scl = ESC_I2C_SCL;
    battery_config.gpout = ESC_GPIO_GPOUT;
    battery_config.port = ESC_I2C_PORT;
    battery_config.clk_speed = ESC_I2C_CLOCK_FREQ;
    battery_config.capacity = ESC_MAX_BATTERY_CAPACITY;
    battery_config.percentage_interval = ESC_PERCENTAGE_CHANGE_INTERVAL;

    esp_err_t err = g_battery.begin(battery_config);
    TEST_ASSERT_EQUAL(ESP_OK, err);
}

extern "C" void app_main()
{
    UNITY_BEGIN();
    RUN_TEST(testBegin);
    UNITY_END();
}

#endif // ESC_ENABLED