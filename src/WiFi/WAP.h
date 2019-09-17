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

#include "Arduino.h"
#include "WiFi.h"

class WAP 
{
    public:
        /**
         * @brief Construct a new WiFiMode object
         * 
         */
        //WAP();

        /**
         * @brief Destroy the WiFiMode object
         * 
         */
        //~WAP();

        /**
         * @brief Start the Wifi AP
         * 
         * @return esp_err_t
         */
        esp_err_t begin();

        /**
         * @brief 
         * 
         * @param client 
         */
        //void responses_WAP(WiFiClient client);

    private:
};   

#endif // WAP_H
