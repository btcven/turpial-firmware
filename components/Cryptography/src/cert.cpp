/**
 * @file cert.cpp
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
#include <esp_log.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/entropy.h>
#include <mbedtls/pk.h>
#include <mbedtls/rsa.h>
#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/x509_csr.h>

namespace crypto {

class certificate
{
private:
    uint16_t _cert_length;
    unsigned char* _cert_data;
    uint16_t _pk_length;
    unsigned char* _pk_data;

public:
    certificate(/* args */);
    ~certificate();
};

certificate::certificate(unsigned char* certData, uint16_t certLength, unsigned char* pkData, uint16_t pkLength) : _certLength(certLength),
                                                                                                                   _certData(certData),
                                                                                                                   _pkLength(pkLength),
                                                                                                                   _pkData(pkData)
{
}

certificate::~certificate()
{
}


} // namespace crypto