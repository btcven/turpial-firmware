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
#include "cert.h"
#include "esp_err.h"
#include "unity.h"
#include <iostream>

crypto::certificate* ssl_cert = new crypto::certificate();

TEST_CASE("Can generate a empty certificate object", "[Certificate]")
{
    TEST_ASSERT_NOT_NULL(ssl_cert);
    // cert. length
    TEST_ASSERT_EQUAL_UINT16(0, ssl_cert->getCertLen());
    // pk length
    TEST_ASSERT_EQUAL_UINT16(0, ssl_cert->getPKLen());
    // cert. data
    TEST_ASSERT_EQUAL(NULL, ssl_cert->getCertData());
    // pk data
    TEST_ASSERT_EQUAL(NULL, ssl_cert->getPKData());
    //

    // initialize entropy
};

TEST_CASE("init_entropy", "[]")
{
    std::cout << "before: mbedtls - accumulator_started: " << ssl_cert->m_entropy.accumulator_started << "\n";
    std::cout << "before: mbedtls - source count: " << ssl_cert->m_entropy.source_count << "\n";
    std::cout << "before: mbedtls - source - size: " << ssl_cert->m_entropy.source->size << "\n";

    TEST_ASSERT_EQUAL(ESP_OK, ssl_cert->init_entropy());

    std::cout << "after: mbedtls - accumulator_started: " << ssl_cert->m_entropy.accumulator_started << "\n";
    std::cout << "after: mbedtls - source count: " << ssl_cert->m_entropy.source_count << "\n";
    std::cout << "after: mbedtls - source - size: " << ssl_cert->m_entropy.source->size << "\n";

    TEST_ASSERT_EQUAL(ESP_OK, ssl_cert->init_pk());
    
};