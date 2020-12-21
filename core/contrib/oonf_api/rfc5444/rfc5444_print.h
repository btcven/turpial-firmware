
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
#ifndef PRINT_RFC5444_H_
#define PRINT_RFC5444_H_

#include "common/autobuf.h"
#include "common/common_types.h"
#include "rfc5444_reader.h"

/**
 * RFC5444 printer session
 */
struct rfc5444_print_session {
  /*! output buffer */
  struct autobuf *output;

  /**
   * Callback to print RFC5444 packet text representation
   * @param session printer session
   */
  void (*print_packet)(struct rfc5444_print_session *session);

  /*! packet consumer */
  struct rfc5444_reader_tlvblock_consumer _pkt;

  /*! message consumer */
  struct rfc5444_reader_tlvblock_consumer _msg;

  /*! address consumer */
  struct rfc5444_reader_tlvblock_consumer _addr;

  /*! rfc5444 reader */
  struct rfc5444_reader *_reader;
};

EXPORT void rfc5444_print_add(struct rfc5444_print_session *, struct rfc5444_reader *reader);
EXPORT void rfc5444_print_remove(struct rfc5444_print_session *session);

EXPORT enum rfc5444_result rfc5444_print_direct(struct autobuf *out, void *buffer, size_t length);

EXPORT int rfc5444_print_raw(struct autobuf *out, void *buffer, size_t length);

#endif /* PRINT_RFC5444_H_ */
