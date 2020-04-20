/**
 * @file test_hmac.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-04-20
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */

#include "hmac.h"
#include <cstdio>
#include <iostream>
#include <string>
#include <unity.h>

const char* input_data = "digest_me";

const unsigned char* buff_in = reinterpret_cast<const unsigned char*> (input_data);

TEST_CASE("Can generate a sha256, return should be 0 ", "[]")
{
    unsigned char buff_out[32];
    TEST_ASSERT_EQUAL(0, crypto::sha256(buff_in, buff_out));
}

TEST_CASE("Can generate a sha256, return hex ", "[]")
{
    unsigned char buff_out[32];
    int res = crypto::sha256(buff_in, buff_out);

    printf("\nSHA256: ");
    
    for (size_t i = 0; i < sizeof(buff_out); i++) {
        printf("%02x", static_cast<const char>(buff_out[i]));
    }
    printf("\n\n");

    TEST_ASSERT_EQUAL(0, crypto::sha256(buff_in, buff_out));
}