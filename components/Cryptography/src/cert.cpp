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

#include <mbedtls/pk.h>
#include <mbedtls/rsa.h>
#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/x509_csr.h>

namespace crypto {

certificate::certificate(
    unsigned char* cert_data,
    std::uint16_t cert_len,
    unsigned char* pk_data,
    std::uint16_t pk_len) : m_cert_len(cert_len),
                            m_cert_data(cert_data),
                            m_pk_len(pk_len),
                            m_pk_data(pk_data) {}

certificate::~certificate()
{
    // destructor
}

std::uint16_t certificate::getCertLen(void)
{
    return this->m_cert_len;
}

std::uint16_t certificate::getPKLen(void)
{
    return this->m_pk_len;
}

unsigned char* certificate::getCertData(void)
{
    return this->m_cert_data;
}

unsigned char* certificate::getPKData(void)
{
    return this->m_pk_data;
}

void certificate::setPK(unsigned char* _pk_data, std::uint16_t _len)
{
    this->m_pk_data = _pk_data;
    this->m_pk_len = _len;
}
void certificate::setData(unsigned char* _cert_data, std::uint16_t _len)
{
    this->m_cert_data = _cert_data;
    this->m_cert_len = _len;
}

void certificate::clear(void)
{
    delete m_cert_data;
    delete m_pk_data;
    m_cert_len = 0;
    m_pk_len = 0;
}

esp_err_t certificate::init_entropy(void)
{
    mbedtls_entropy_init(&m_entropy);
    mbedtls_ctr_drbg_init(&m_rng);
    int res = mbedtls_ctr_drbg_seed(&m_rng, mbedtls_entropy_func, &m_entropy, NULL, 0);
    // ToDo handle errors defined into: mbedtls/ctr_drbg.h
    if (res != 0) {
        ESP_LOGE(__func__, "SEED_ERROR ID: %d ", res);
        mbedtls_entropy_free(&m_entropy);
        return ESP_FAIL;
    } else {
        return ESP_OK;
    }
}

esp_err_t certificate::init_pk(void)
{
    mbedtls_pk_init(&m_pk);
    int res = mbedtls_pk_setup(&m_pk, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA));
    if (res != 0) {
        ESP_LOGE(__func__, "KEYGEN_ERROR ID: %d", res);
        mbedtls_ctr_drbg_free(&m_rng);
        mbedtls_entropy_free(&m_entropy);
        return ESP_FAIL;
    } else {
        return ESP_OK;
    }
}

} // namespace crypto