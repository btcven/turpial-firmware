
/*
 * The olsr.org Optimized Link-State Routing daemon version 2 (olsrd2)
 * Copyright (c) 2004-2015, the olsr.org team - see HISTORY file
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 * * Neither the name of olsr.org, olsrd nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * Visit http://www.olsr.org for more information.
 *
 * If you find this software useful feel free to make a donation
 * to the project. For more information see the website or contact
 * the copyright holders.
 *
 */

/**
 * @file
 */
#include "common/common_types.h"
#include "common/netaddr.h"

#include "rfc5444_iana.h"

static const char *_rfc7182_hashes[RFC7182_ICV_HASH_COUNT] = {
  [RFC7182_ICV_HASH_IDENTITY] = "identity",
  [RFC7182_ICV_HASH_SHA_1] = "sha1",
  [RFC7182_ICV_HASH_SHA_224] = "sha224",
  [RFC7182_ICV_HASH_SHA_256] = "sha256",
  [RFC7182_ICV_HASH_SHA_384] = "sha384",
  [RFC7182_ICV_HASH_SHA_512] = "sha512",
};

static const char *_rfc7182_crypt[RFC7182_ICV_CRYPT_COUNT] = {
  [RFC7182_ICV_CRYPT_IDENTITY] = "identity",
  [RFC7182_ICV_CRYPT_RSA] = "rsa",
  [RFC7182_ICV_CRYPT_DSA] = "dsa",
  [RFC7182_ICV_CRYPT_HMAC] = "hmac",
  [RFC7182_ICV_CRYPT_3DES] = "3des",
  [RFC7182_ICV_CRYPT_AES] = "aes",
  [RFC7182_ICV_CRYPT_ECDSA] = "ecdsa",
};

/**
 * Give name of rfc 7182 hash
 * @param hash hash-id
 * @return name of hash
 */
const char *
rfc7182_get_hash_name(enum rfc7182_icv_hash hash) {
  if (hash < 0 || hash >= RFC7182_ICV_HASH_COUNT) {
    return "unknown";
  }
  return _rfc7182_hashes[hash];
}

/**
 * Give name of rfc 7182 crypto
 * @param crypt crypto-id
 * @return name of crypto
 */
const char *
rfc7182_get_crypt_name(enum rfc7182_icv_crypt crypt) {
  if (crypt < 0 || crypt >= RFC7182_ICV_CRYPT_COUNT) {
    return "unknown";
  }
  return _rfc7182_crypt[crypt];
}

/**
 * @return array of rfc7182 hash names
 */
const char **
rfc7182_get_hashes(void) {
  return _rfc7182_hashes;
}

/**
 * @return array of rfc7182 crypto names
 */
const char **
rfc7182_get_crypto(void) {
  return _rfc7182_crypt;
}

/**
 * @param name rfc7182 hash name
 * @return rfc7182 hash-id
 */
enum rfc7182_icv_hash
rfc7182_get_hash_id(const char *name)
{
  size_t i;

  for (i = 0; i < ARRAYSIZE(_rfc7182_hashes); i++) {
    if (strcmp(_rfc7182_hashes[i], name) == 0) {
      return i;
    }
  }
  return RFC7182_ICV_HASH_UNKNOWN;
}

/**
 * @param name rfc7182 crypto name
 * @return rfc7182 crypto-id
 */
enum rfc7182_icv_crypt
rfc7182_get_crypt_id(const char *name)
{
  size_t i;

  for (i = 0; i < ARRAYSIZE(_rfc7182_crypt); i++) {
    if (strcmp(_rfc7182_crypt[i], name) == 0) {
      return i;
    }
  }
  return RFC7182_ICV_CRYPT_UNKNOWN;
}
