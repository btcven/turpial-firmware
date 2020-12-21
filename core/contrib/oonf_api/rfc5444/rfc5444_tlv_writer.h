
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

#ifndef RFC5444_TLV_WRITER_H_
#define RFC5444_TLV_WRITER_H_

#include "common/common_types.h"

/**
 * tlv write session
 */
struct rfc5444_tlv_writer_data {
  /*! pointer to binary buffer */
  uint8_t *buffer;

  /*! size of header */
  size_t header;

  /*! size of added TLVs */
  size_t added;

  /*! size of allocated TLVs */
  size_t allocated;

  /*! size of set TLVs */
  size_t set;

  /*! maximum size allowed for TLVs */
  size_t max;
};

/* internal functions that are not exported to the user */
void _rfc5444_tlv_writer_init(struct rfc5444_tlv_writer_data *data, size_t max, size_t mtu);

enum rfc5444_result _rfc5444_tlv_writer_add(
  struct rfc5444_tlv_writer_data *data, uint8_t type, uint8_t exttype, const void *value, size_t length);
enum rfc5444_result _rfc5444_tlv_writer_allocate(struct rfc5444_tlv_writer_data *data, bool has_exttype, size_t length);
enum rfc5444_result _rfc5444_tlv_writer_set(
  struct rfc5444_tlv_writer_data *data, uint8_t type, uint8_t exttype, const void *value, size_t length);

#endif /* RFC5444_TLV_WRITER_H_ */
