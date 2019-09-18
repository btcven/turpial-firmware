/**
 * @file battery.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#ifndef BATTERY_H
#define BATTERY_H

#include "Arduino.h"
#include "SparkFunBQ27441.h" // This is the library used for the battery energy management

class Battery
{
    public:
        /**
         * @brief Construct a new Battery object
         * 
         */
        Battery(uint16_t capacity, uint16_t low_bat_threshold);

        /**
         * @brief This method will initialize I2C and the IC
         * 
         * @return esp_err_t
         */
        esp_err_t begin();

        /**
         * @brief This method will read and return the battery level
         * 
         * @return int16_t
         */
        uint16_t getBatteryLevel();

        /**
         * @brief This method will read and return the battery voltage
         * 
         * @return uint16_t
         */
        uint16_t getBatteryVoltage();

        /**
         * @brief This method will read and return the battery current
         * 
         * @return int16_t
         */
        int16_t getBatteryCurrent();

        /**
         * @brief This method will return a status flag, to know if battery (the board) is being charged or not
         * 
         * @return bool
         */
        bool isCharging();

        /**
         * @brief This method will read battery level and compare with a reference value, to return if battery level is low or not
         * 
         * @return bool
         */
        bool isBatteryLow();

        /**
         * @brief This method will read and return the battery temperature reading value in Degrees C
         * 
         * @return uint16_t
         */
        uint16_t getTemperature();

        /**
         * @brief This method will read and return the IC temperature reading value in Degrees C
         * 
         * @return uint16_t
         */
        uint16_t getICTemperature();

    private:
        uint16_t _capacity;
        uint16_t _low_bat_threshold;
};

#endif // BATTERY_H