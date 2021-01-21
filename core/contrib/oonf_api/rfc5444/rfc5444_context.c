
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

#include "rfc5444_context.h"

static const char *_rfc5444_positive_result_texts[] = {
  [RFC5444_OKAY] = "Okay",
  [RFC5444_DROP_TLV] = "Drop TLV",
  [RFC5444_DROP_MSG_BUT_FORWARD] = "Drop message but forward it",
  [RFC5444_DROP_MESSAGE] = "Drop message",
  [RFC5444_DROP_PACKET] = "Drop packet",
};

static const char *_rfc5444_negative_result_texts[] = {
  [RFC5444_OKAY] = "Okay",
  [-RFC5444_UNSUPPORTED_VERSION] = "Version of rfc5444 not supported",
  [-RFC5444_END_OF_BUFFER] = "Early end of packet",
  [-RFC5444_BAD_TLV_IDXFLAGS] = "Bad combination of tlv index flags",
  [-RFC5444_BAD_TLV_VALUEFLAGS] = "Bad combination of tlv value flags",
  [-RFC5444_BAD_TLV_LENGTH] = "TLV length is no multiple of number of values",
  [-RFC5444_OUT_OF_MEMORY] = "Memory allocation failed",
  [-RFC5444_EMPTY_ADDRBLOCK] = "Address block with zero addresses",
  [-RFC5444_BAD_MSG_TAILFLAGS] = "Bad combination of address tail flags",
  [-RFC5444_BAD_MSG_PREFIXFLAGS] = "Bad combination of address prefix length flags",
  [-RFC5444_DUPLICATE_TLV] = "Duplicate address TLV",
  [-RFC5444_OUT_OF_ADDRTLV_MEM] = "Not enough memory for address-TLVs",
  [-RFC5444_MTU_TOO_SMALL] = "Configured MTU size too small",
  [-RFC5444_NO_MSGCREATOR] = "Cannot create message without message creator",
  [-RFC5444_FW_MESSAGE_TOO_LONG] = "Cannot forward message, content too long",
  [-RFC5444_FW_BAD_SIZE] = "Bad length field of message to be forwarded",
  [-RFC5444_TOO_LARGE] = "RFC5444 packet larger than 64k",
  [-RFC5444_FW_BAD_TRANSFORM] = "RFC5444 transformer failed",
  [-RFC5444_BAD_ADDR_HEAD_LENGTH] = "Bad addrblock head length",
  [-RFC5444_BAD_ADDR_TAIL_LENGTH] = "Bad addrblock tail length",
  [-RFC5444_BAD_TLV_INDEX] = "Bad tlv index fields",
};

/**
 * @param result RFC5445 API result code
 * @return text text representation of the result code
 */
const char *
rfc5444_strerror(enum rfc5444_result result) {
  const char *UNKNOWN = "Unknown RFC5444-API result";
  if (result >= RFC5444_OKAY && result <= RFC5444_RESULT_MAX) {
    return _rfc5444_positive_result_texts[result];
  }
  if (result < RFC5444_OKAY && result >= RFC5444_RESULT_MIN) {
    return _rfc5444_negative_result_texts[-result];
  }
  return UNKNOWN;
}
