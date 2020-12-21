
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
#ifndef RFC5444_CONVERSION_H_
#define RFC5444_CONVERSION_H_

#include "common/common_types.h"
#include "rfc5444_iana.h"

enum
{
  /* timetlv_max = 14 * 2^28 * 1000 / 1024 = 14000 << 18 = 3 670 016 000 ms */
  RFC5497_TIMETLV_MAX = 0xdac00000,

  /* timetlv_min = 1000/1024 ms */
  RFC5497_TIMETLV_MIN = 0x00000001,

  /* metric_max = 1<<24 - 256 */
  RFC7181_METRIC_MAX = 0xffff00,

  /* metric_min = 1 */
  RFC7181_METRIC_MIN = 0x000001,

  /* larger than possible metric value */
  RFC7181_METRIC_INFINITE = 0xffffff,

  /* infinite path cost */
  RFC7181_METRIC_INFINITE_PATH = 0xffffffff,
};

/**
 * Binary Metric TLV value of RFC7181
 */
struct rfc7181_metric_field {
  /*! two byte value */
  uint8_t b[2];
};

EXPORT uint8_t rfc5497_timetlv_get_from_vector(const uint8_t *vector, size_t vector_length, uint8_t hopcount);
EXPORT uint8_t rfc5497_timetlv_encode(uint64_t);
EXPORT uint64_t rfc5497_timetlv_decode(uint8_t);

EXPORT int rfc7181_metric_encode(struct rfc7181_metric_field *, uint32_t);
EXPORT uint32_t rfc7181_metric_decode(struct rfc7181_metric_field *);

EXPORT int rfc5444_seqno_difference(uint16_t, uint16_t);

/**
 * Compares two RFC5444 sequence numbers
 * @param s1 first sequence number
 * @param s2 second sequence number
 * @return true if first is larger than second number
 */
static INLINE bool
rfc5444_seqno_is_larger(uint16_t s1, uint16_t s2) {
  /*
   * The sequence number S1 is said to be "greater than" the sequence
   * number S2 if:
   * o  S1 > S2 AND S1 - S2 < MAXVALUE/2 OR
   * o  S2 > S1 AND S2 - S1 > MAXVALUE/2
   */
  return (s1 > s2 && (s1 - s2) < (1 << 15)) || (s2 > s1 && (s2 - s1) > (1 << 15));
}

/**
 * Compares two RFC5444 sequence numbers
 * @param s1 first sequence number
 * @param s2 second sequence number
 * @return true if first is smaller than second number
 */
static INLINE bool
rfc5444_seqno_is_smaller(uint16_t s1, uint16_t s2) {
  return s1 != s2 && !rfc5444_seqno_is_larger(s1, s2);
}

/**
 * Test for metric flag
 * @param metric metric field
 * @param flag flag to test for
 * @return true if flag is set, false otherwise
 */
static INLINE bool
rfc7181_metric_has_flag(struct rfc7181_metric_field *metric, enum rfc7181_linkmetric_flags flag) {
  return (metric->b[0] & (uint8_t)flag) != 0;
}

/**
 * Set a metric flag
 * @param metric metric field
 * @param flag flag to set
 */
static INLINE void
rfc7181_metric_set_flag(struct rfc7181_metric_field *metric, enum rfc7181_linkmetric_flags flag) {
  metric->b[0] |= (uint8_t)flag;
}

/**
 * Reset a metric flag
 * @param metric metric field
 * @param flag flag to reset
 */
static INLINE void
rfc7181_metric_reset_flag(struct rfc7181_metric_field *metric, enum rfc7181_linkmetric_flags flag) {
  metric->b[0] &= ~((uint8_t)flag);
}

#endif /* RFC5444_CONVERSION_H_ */
