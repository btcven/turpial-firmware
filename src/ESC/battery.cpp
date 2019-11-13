/**
 * @file battery.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "battery.h"

BQ27441 _battery; // Create instance of the BQ27441 class

Battery::Battery(uint16_t capacity, uint16_t low_bat_threshold, uint16_t critical_bat_threshold)
{
    _capacity = capacity;
    _low_bat_threshold = low_bat_threshold;
    _critical_bat_threshold = critical_bat_threshold;
}

esp_err_t Battery::begin()
{
    // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
    // connected and communicating.
    if (!_battery.begin()) // begin() will return true if communication is successful
    {
        // If communication fails, print an error message and loop forever.
        ESP_LOGE(__func__, "Error: Unable to communicate with BQ27441.");
        return ESP_FAIL;
    }
    ESP_LOGD(__func__, "Connected to BQ27441!");

    // Uset lipo.setCapacity(BATTERY_CAPACITY) to set the design capacity
    // of your battery.
    if (!_battery.setCapacity(_capacity))
    {
        // If communication fails, print an error message and loop forever.
        ESP_LOGE(__func__, "Error: Unable to set battery capacity.");
        return ESP_FAIL;
    }
    ESP_LOGD(__func__, "Battery capacity was set!");

    return ESP_OK;
}

uint16_t Battery::getBatteryLevel()
{
    return _battery.soc();
}

uint16_t Battery::getBatteryVoltage()
{
    return _battery.voltage();
}

int16_t Battery::getBatteryCurrent()
{
    return _battery.current();
}

bool Battery::isCharging()
{
    if (getBatteryCurrent() > 0)
    {
        return true; // Battery is being charged
    }
    else
    {
        return false; // Battery is not being charged (draining current)
    }
}

bool Battery::isBatteryLow()
{
    if (getBatteryLevel() <= _low_bat_threshold)
    {
        return true; // Battery is low
    }
    else
    {
        return false; // Stil have a good charge
    }    
}

bool Battery::isBatteryCritical()
{
    if (getBatteryLevel() <= _critical_bat_threshold)
    {
        return true; // Battery is critical
    }
    else
    {
        return false;
    }
}

uint16_t Battery::getTemperature()
{
    return _battery.temperature();
}

uint16_t Battery::getICTemperature()
{
    return _battery.temperature(INTERNAL_TEMP);
}
