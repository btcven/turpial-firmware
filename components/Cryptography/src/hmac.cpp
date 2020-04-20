/**
 * @file hmac.h
 * @author Locha Mesh project developers (locha.io)
 * @brief 
 * @version 0.1
 * @date 2020-04-16
 * 
 * @copyright Copyright (c) 2019 Locha Mesh project developers
 * @license Apache 2.0, see LICENSE file for details
 */

#include "hmac.h"
#include <esp_log.h>
#include <mbedtls/sha256.h>



namespace crypto {

int sha256(const unsigned char* data_in, unsigned char* sha256_out)
{
    mbedtls_sha256_context sha256_ctx;

    mbedtls_sha256_init(&sha256_ctx);

    int res;
    res = mbedtls_sha256_starts_ret(&sha256_ctx, 0);
    if (res != 0) {
        ESP_LOGE(__func__, "Error: %d", res);
        return res;
    }

    res = mbedtls_sha256_update_ret(&sha256_ctx, data_in, sizeof(data_in));
    if (res != 0) {
        ESP_LOGE(__func__, "Error: %d", res);
        return res;
    }

    res = mbedtls_sha256_finish_ret(&sha256_ctx, sha256_out);
    if (res != 0) {
        ESP_LOGE(__func__, "Error: %d", res);
        return res;
    }

    mbedtls_sha256_free(&sha256_ctx);
    return 0;
}

} // namespace crypto
