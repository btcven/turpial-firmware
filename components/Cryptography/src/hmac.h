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

#ifndef HMAC_H
#define HMAC_H

namespace crypto {

/**
 * @brief compute a hmac from const unsigned char*
 * 
 * @param data_in 
 * @param sha256_out 
 * @return int OK if 0, any integer other than 0 is ERROR. 
 */
int sha256(const unsigned char* data_in, unsigned char* sha256_out);


} // namespace crypto


#endif // HMAC_H
