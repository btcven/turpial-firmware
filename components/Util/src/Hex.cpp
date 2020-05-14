/**
 * @file Hex.cpp
 * @author Locha Mesh Developers (contact@locha.io)
 * @brief 
 * @version 0.1
 * @date 2019-11-26
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#include "Hex.h"

namespace util {

esp_err_t hexToInt(const char c, int& r)
{
    if (c >= '0' && c <= '9') {
        r = c - '0';
        return ESP_OK;
    }
    if (c >= 'A' && c <= 'F') {
        r = c - 'A' + 10;
        return ESP_OK;
    }
    if (c >= 'a' && c <= 'f') {
        r = c - 'a' + 10;
        return ESP_OK;
    }

    return ESP_FAIL;
}

esp_err_t hexToBytes(const char* src, std::uint8_t* target)
{
    esp_err_t err;

    while (src[0] != '\0' && src[1] != '\0') {
        int digit0 = 0;
        int digit1 = 0;
        err = hexToInt(src[0], digit0);
        if (err != ESP_OK) return err;

        err = hexToInt(src[1], digit1);
        if (err != ESP_OK) return err;

        target[0] = static_cast<std::uint8_t>((digit0 * 16) + digit1);
        target += 1;
        src += 2;
    }

    return ESP_OK;
}


esp_err_t bytesToHex(std::uint8_t* buf, char* dst, std::size_t len)
{
    static const char hexmap[] = {
        '0', '1', '2', '3', '4', '5',
        '6', '7', '8', '9', 'a', 'b',
        'c', 'd', 'e', 'f'};

    for (size_t i = 0; i < len; i++) {
        const uint8_t upper = (buf[i] & 0xf0) >> 4;
        const uint8_t lower = (buf[i] & 0x0f);
        dst[i * 2] = hexmap[upper];
        dst[i * 2 + 1] = hexmap[lower];
    }

    dst[len * 2] = '\0';

    return ESP_OK;
}


void decToHexa(int n, uint8_t* hex_data)
{
   
    // char array to store hexadecimal number
    char hexaDeciNum[100];

    // counter for hexadecimal number array
    int i = 0;
    while (n != 0) {
        // temporary variable to store remainder
        int temp = 0;

        // storing remainder in temp variable.
        temp = n % 16;

        // check if temp < 10
        if (temp < 10) {
            hexaDeciNum[i] = temp + 48;
            i++;
        } else {
            hexaDeciNum[i] = temp + 55;
            i++;
        }

        n = n / 16;
    }

    int d = 0;
    for (int j = i - 1; j >= 0; j--) {
        hex_data[d] = hexaDeciNum[j];
        d++;
    }
}

} // namespace util
