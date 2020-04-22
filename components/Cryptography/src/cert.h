/**
 * @file cert.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-04-20
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 * 
 */
#include <cstddef>
#include <cstdint>
#include <esp_err.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/pk.h>
namespace crypto {

class certificate
{
private:
    // ToDo: move functions to private ctx after unit-testing:
    // - getCertLen
    // - getPKLen
    // - getCertData
    // - getPKData

    // properties
    std::uint16_t m_cert_len;
    unsigned char* m_cert_data;
    std::uint16_t m_pk_len;
    unsigned char* m_pk_data;

    mbedtls_entropy_context m_entropy;
    mbedtls_ctr_drbg_context m_rng;
    mbedtls_pk_context m_pk;

public:
    certificate(
        unsigned char* cert_data = NULL,
        std::uint16_t cert_len = 0,
        unsigned char* pk_data = NULL,
        std::uint16_t pk_len = 0);

    virtual ~certificate();

    std::uint16_t getCertLen(void);

    std::uint16_t getPKLen(void);

    unsigned char* getCertData(void);

    unsigned char* getPKData(void);

    void setPK(unsigned char* _pk_data, std::uint16_t len);

    void setData(unsigned char* _cert_data, std::uint16_t len);

    void clear(void);

    esp_err_t init_entropy(void);
    
    esp_err_t init_pk(void);
};

} // namespace crypto
