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

Battery::Battery(uint16_t capacity) 
{
    _capacity = capacity;
}

esp_err_t begin()
{
    esp_err_t err;

    
}