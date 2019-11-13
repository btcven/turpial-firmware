/**
 * @file WAP.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1.1
 * @date 2019-08-15
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#ifndef WAP_H
#define WAP_H

#include "WiFi.h"

namespace wifi {

namespace wap {

struct Config {
    const char* apSSID;
    const char* apPass;
    int8_t apChannel;
    int8_t apMaxConn;
};

esp_err_t begin(Config config);

} // namespace wap

} // namespace wifi

#endif // WAP_H