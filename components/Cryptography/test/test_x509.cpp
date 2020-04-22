/**
 * @file test_x509.cpp
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-04-20
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */
#include <iostream>
#include "cert.h"
#include <unity.h>

crypto::certificate* ssl_cert = new crypto::certificate();

TEST_CASE("Can generate a certificate object", "[]")
{
    TEST_ASSERT_NOT_NULL(ssl_cert);
}

TEST_CASE("Can return 0 for the cert. length", "[]")
{
    TEST_ASSERT_NOT_NULL(ssl_cert);
}