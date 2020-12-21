
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

#ifndef DLEP_SIGNAL_H_
#define DLEP_SIGNAL_H_

#include "common/common_types.h"

/*! keyword for setting all bits in a bitmap */
#define BITMAP256_ALL "all"

/*! keyword for clearing all bits in a bitmap */
#define BITMAP256_NONE "none"

/**
 * Bitarray with 256 bits length
 */
struct bitmap256 {
  /*! array for 256 bits */
  uint64_t b[256 / sizeof(uint64_t) / 8];
};

EXPORT bool bitmap256_is_subset(struct bitmap256 *set, struct bitmap256 *subset);

/**
 * get a bit of the bit array
 * @param map pointer to bit array
 * @param bit index of bit
 * @return content of the bit
 */
static inline bool
bitmap256_get(struct bitmap256 *map, uint8_t bit) {
  return ((map->b[bit >> 6]) & (1ull << (bit & 63ull))) != 0;
}

/**
 * set a bit of the bit array
 * @param map pointer to bit array
 * @param bit index of bit
 */
static inline void
bitmap256_set(struct bitmap256 *map, uint8_t bit) {
  map->b[bit >> 6] |= 1ull << (bit & 63ull);
}

/**
 * reset a bit of the bit array
 * @param map pointer to bit array
 * @param bit index of bit
 */
static inline void
bitmap256_reset(struct bitmap256 *map, uint8_t bit) {
  map->b[bit >> 6] &= ~(1ull << (bit & 63ull));
}

#endif /* DLEP_SIGNAL_H_ */
