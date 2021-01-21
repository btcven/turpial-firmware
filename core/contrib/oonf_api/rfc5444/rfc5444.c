
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
#include <assert.h>

#include "common/common_types.h"
#include "rfc5444.h"

/**
 * Retrieve a timetlv value from a vector of hopcount/value sequences.
 * See RFC 5497, Section 5 for details.
 * @param vector pointer to timetlv vector
 * @param vector_length length of vector
 * @param hopcount hopcount for value lookup
 * @return timetlv value, 255 (infinite) if an error happened.
 */
uint8_t
rfc5497_timetlv_get_from_vector(const uint8_t *vector, size_t vector_length, uint8_t hopcount) {
  size_t i;

  /* handle illegal vector length */
  if ((vector_length & 1) == 0) {
    /* return infinite value */
    return 255;
  }

  for (i = 1; i < vector_length; i += 2) {
    /* find our position in the vector */
    if (hopcount <= vector[i]) {
      return vector[i - 1];
    }
  }

  /* use the last field of the vector */
  return vector[i - 1];
}

/**
 * Converts a relative time value into its RFC 5497 (timetlv)
 * representation.
 * If the time value is larger than the largest timetlv encoding,
 * the largest encoding (255) will be returned.
 * If the time value is zero, the function returns zero.
 *
 * @param decoded relative timestamp in milliseconds
 * @return RFC 5497 encoded time
 */
uint8_t
rfc5497_timetlv_encode(uint64_t decoded) {
  uint32_t a, b;
  /*
   * t = (1 + a/8) * 2^b * 1000 / 1024
   *   = (1000 + 125 * a) * (2^b / 2^10)
   *   = (1000 + 125 * a) * 2 ^ (b-10)
   */

  if (decoded < RFC5497_TIMETLV_MIN) {
    return 0;
  }
  if (decoded > RFC5497_TIMETLV_MAX) {
    return 255;
  }

  b = 10;
  if (decoded >= 1000) {
    /* this means b >= 10 */
    while (decoded > 1875) {
      b++;

      /* divide by 2 and round up */
      decoded++;
      decoded >>= 1;
    }
  }
  else { /* decoded < 1000 */
    /* b < 10 */
    while (decoded < 1000) {
      b--;
      decoded <<= 1;
    }
  }

  a = (decoded - 1000 + 124) / 125;
  return a + (b << 3);
}

/**
 * Decode an RFC 5497 encoding into a relative time value.
 * If the encoded data is zero, the function returns 0.
 *
 * @param encoded RFC 5497 encoded time
 * @return relative time in milliseconds
 */
uint64_t
rfc5497_timetlv_decode(uint8_t encoded) {
  /*
   * time-value := (1 + a/8) * 2^b * C
   * time-code := 8 * b + a
   */
  uint8_t a, b;

  if (encoded == 0) {
    /* minimum valid time interval */
    return 0;
  }

  if (encoded == 255) {
    /* return 'infinite' */
    return UINT64_MAX;
  }

  a = encoded & 0x07;
  b = encoded >> 3;

  /*
   * C is 1000/1024 for us, because we calculate in ms
   *
   * t = (1 + a/8) * 2^b * 1000 / 1024
   *   = (1000 + 125 * a) * 2^b / 2^10
   *
   * Case 1: b <= 10
   *   = (1000 + 125 * a) >> (10 - b)
   *
   * Case 2: b > 10
   *   = (1000 + 125 * a) << (b - 10)
   */

  if (b <= 10) {
    return (1000ull + 125ull * a) >> (10ull - b);
  }
  return (1000ull + 125ull * a) << (b - 10ull);
}

/**
 * Encode a metric value in RFC7181 (OLSRv2) specified format.
 * A metric value of zero or larger than RFC7181_METRIC_INFINITE
 * will return an error.
 * A metric value larger than the RFC7181_METRIC_MAX will be encoded to 4095.
 * @param encoded pointer to encoded metric value
 * @param decoded metric value.
 * @return encoded -1 if an error happened, 0 otherwise.
 */
int
rfc7181_metric_encode(struct rfc7181_metric_field *encoded, uint32_t decoded) {
  uint8_t a, b;
  /*
   * metric-value := (257+b)2^a - 256
   * metric-code := 256 * a + b
   */

  if (decoded < RFC7181_METRIC_MIN || decoded >= RFC7181_METRIC_INFINITE) {
    return -1;
  }
  if (decoded > RFC7181_METRIC_MAX) {
    encoded->b[0] = 0x0f;
    encoded->b[1] = 0xff;
    return 0;
  }

  /* metric-value + 256 = (257+b)<<a */
  decoded += 256;

  a = 0;
  while (decoded > 512) {
    a++;

    /* divide by 2 and round up */
    decoded++;
    decoded >>= 1;
  }

  b = decoded - 257;

  encoded->b[0] = a;
  encoded->b[1] = b;

  return 0;
}

/**
 * Decode a RFC7181 (OLSRv2) encoded metric value.
 * @param encoded encoded metric
 * @return decoded metric value
 */
uint32_t
rfc7181_metric_decode(struct rfc7181_metric_field *encoded) {
  uint8_t exponent = encoded->b[0] & 15;
  return ((257 + encoded->b[1]) << exponent) - 256;
}

/**
 * Calculate the difference between two sequence numbers
 * @param seqno1 first sequence number
 * @param seqno2 second sequence number
 * @return difference between both
 */
int
rfc5444_seqno_difference(uint16_t seqno1, uint16_t seqno2) {
  int diff = (int)seqno1 - (int)(seqno2);

  // overflow ?
  if (diff > (1 << 15)) {
    diff -= (1 << 16);
  }
  else if (diff < -(1 << 15)) {
    diff += (1 << 16);
  }
  return diff;
}
