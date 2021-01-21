
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
#ifndef RFC5444_CONTEXT_H_
#define RFC5444_CONTEXT_H_

#include "common/common_types.h"
#include "rfc5444_api_config.h"

/**
 * Return values for reader callbacks and API calls (and internal functions)
 * The RFC5444_DROP_... constants are ordered, higher values mean
 * dropping more of the context.
 * All return values less than zero represent an error.
 */
enum rfc5444_result
{
#if DISALLOW_CONSUMER_CONTEXT_DROP == false
  /*! maximum value of result */
  RFC5444_RESULT_MAX = 5,

  /*! drop whole packet */
  RFC5444_DROP_PACKET = 5,

  /*! drop message */
  RFC5444_DROP_MESSAGE = 4,

  /*! drop message but still forward it if possible */
  RFC5444_DROP_MSG_BUT_FORWARD = 3,

  /*! drop address and attached TLVs */
  RFC5444_DROP_ADDRESS = 2,

  /*! drop TLV */
  RFC5444_DROP_TLV = 1,
#endif
  /*! everything worked fine */
  RFC5444_OKAY = 0,

  /*! version field of rfc5444 is not 0 */
  RFC5444_UNSUPPORTED_VERSION = -1,

  /*! end of rfc5444 data stream before end of message/tlv */
  RFC5444_END_OF_BUFFER = -2,

  /*! illegal combination of thassingleindex and thasmultiindex flags */
  RFC5444_BAD_TLV_IDXFLAGS = -3,

  /*! illegal combination of thasvalue and thasextlen flag */
  RFC5444_BAD_TLV_VALUEFLAGS = -4,

  /*! length of tlv is no multiple of number of values */
  RFC5444_BAD_TLV_LENGTH = -5,

  /*! dynamic memory allocation failed */
  RFC5444_OUT_OF_MEMORY = -6,

  /*! address block with 0 addresses found */
  RFC5444_EMPTY_ADDRBLOCK = -7,

  /*! illegal combination of ahasfulltail and ahaszerotail flag */
  RFC5444_BAD_MSG_TAILFLAGS = -8,

  /*! illegal combination of ahassingleprelen and ahasmultiprelen flag */
  RFC5444_BAD_MSG_PREFIXFLAGS = -9,

  /*! address tlv already exists */
  RFC5444_DUPLICATE_TLV = -10,

  /*! internal buffer for address tlv values too small */
  RFC5444_OUT_OF_ADDRTLV_MEM = -11,

  /*! non-fragmentable part of message does not fit into max sizes packet */
  RFC5444_MTU_TOO_SMALL = -12,

  /*! cannot create a message without a message creator */
  RFC5444_NO_MSGCREATOR = -13,

  /*! bad format of forwarded message, does not fit into max sized packet */
  RFC5444_FW_MESSAGE_TOO_LONG = -14,

  /*! bad format of forwarded message, size field wrong */
  RFC5444_FW_BAD_SIZE = -15,

  /*! buffer for reader is larger than 64k */
  RFC5444_TOO_LARGE = -16,

  /*! transformer failed for forwarded message */
  RFC5444_FW_BAD_TRANSFORM = -17,

  /*! head length is not between 1 and 15 */
  RFC5444_BAD_ADDR_HEAD_LENGTH = -18,

  /*! tail length is not between 1 and 15 */
  RFC5444_BAD_ADDR_TAIL_LENGTH = -19,

  /*! either start or end index is beyond the number of addresses (minus 1) */
  RFC5444_BAD_TLV_INDEX = -20,

  /*! minimal value of result */
  RFC5444_RESULT_MIN = -20,
};

EXPORT const char *rfc5444_strerror(enum rfc5444_result result);

/* maximum address length */
enum
{
  RFC5444_MAX_ADDRLEN = 16
};

/* packet flags */
enum
{
  RFC5444_PKT_FLAGMASK = 0x0f,
  RFC5444_PKT_FLAG_SEQNO = 0x08,
  RFC5444_PKT_FLAG_TLV = 0x04,

  /* message flags */
  RFC5444_MSG_FLAG_ORIGINATOR = 0x80,
  RFC5444_MSG_FLAG_HOPLIMIT = 0x40,
  RFC5444_MSG_FLAG_HOPCOUNT = 0x20,
  RFC5444_MSG_FLAG_SEQNO = 0x10,

  RFC5444_MSG_FLAG_ADDRLENMASK = 0x0f,

  /* addressblock flags */
  RFC5444_ADDR_FLAG_HEAD = 0x80,
  RFC5444_ADDR_FLAG_FULLTAIL = 0x40,
  RFC5444_ADDR_FLAG_ZEROTAIL = 0x20,
  RFC5444_ADDR_FLAG_SINGLEPLEN = 0x10,
  RFC5444_ADDR_FLAG_MULTIPLEN = 0x08,

  /* tlv flags */
  RFC5444_TLV_FLAG_TYPEEXT = 0x80,
  RFC5444_TLV_FLAG_SINGLE_IDX = 0x40,
  RFC5444_TLV_FLAG_MULTI_IDX = 0x20,
  RFC5444_TLV_FLAG_VALUE = 0x10,
  RFC5444_TLV_FLAG_EXTVALUE = 0x08,
  RFC5444_TLV_FLAG_MULTIVALUE = 0x04,
};

#endif /* RFC5444_CONTEXT_H_ */
