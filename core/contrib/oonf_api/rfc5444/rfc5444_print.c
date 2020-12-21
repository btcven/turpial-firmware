
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
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>

#include "common/netaddr.h"
#include "rfc5444_print.h"
#include "rfc5444_reader.h"

static enum rfc5444_result _cb_print_pkt_start(struct rfc5444_reader_tlvblock_context *context);
static enum rfc5444_result _cb_print_pkt_tlv(
  struct rfc5444_reader_tlvblock_entry *tlv, struct rfc5444_reader_tlvblock_context *context);
static enum rfc5444_result _cb_print_pkt_end(struct rfc5444_reader_tlvblock_context *context, bool);
static enum rfc5444_result _cb_print_msg_start(struct rfc5444_reader_tlvblock_context *context);
static enum rfc5444_result _cb_print_msg_tlv(
  struct rfc5444_reader_tlvblock_entry *tlv, struct rfc5444_reader_tlvblock_context *context);
static enum rfc5444_result _cb_print_msg_end(struct rfc5444_reader_tlvblock_context *context, bool);
static enum rfc5444_result _cb_print_addr_start(struct rfc5444_reader_tlvblock_context *context);
static enum rfc5444_result _cb_print_addr_tlv(
  struct rfc5444_reader_tlvblock_entry *tlv, struct rfc5444_reader_tlvblock_context *context);
static enum rfc5444_result _cb_print_addr_end(struct rfc5444_reader_tlvblock_context *context, bool);

/**
 * Add a printer for a rfc5444 reader
 * @param session pointer to initialized rfc5444 printer session
 * @param reader pointer to initialized reader
 */
void
rfc5444_print_add(struct rfc5444_print_session *session, struct rfc5444_reader *reader) {
  /* memorize reader */
  session->_reader = reader;

  session->_pkt.start_callback = _cb_print_pkt_start;
  session->_pkt.tlv_callback = _cb_print_pkt_tlv;
  session->_pkt.end_callback = _cb_print_pkt_end;
  rfc5444_reader_add_packet_consumer(reader, &session->_pkt, NULL, 0);

  session->_msg.default_msg_consumer = true;
  session->_msg.start_callback = _cb_print_msg_start;
  session->_msg.tlv_callback = _cb_print_msg_tlv;
  session->_msg.end_callback = _cb_print_msg_end;
  rfc5444_reader_add_message_consumer(reader, &session->_msg, NULL, 0);

  session->_addr.default_msg_consumer = true;
  session->_addr.addrblock_consumer = true;
  session->_addr.start_callback = _cb_print_addr_start;
  session->_addr.tlv_callback = _cb_print_addr_tlv;
  session->_addr.end_callback = _cb_print_addr_end;
  rfc5444_reader_add_message_consumer(reader, &session->_addr, NULL, 0);
}

/**
 * Remove printer from rfc5444 reader
 * @param session pointer to initialized rfc5444 printer session
 */
void
rfc5444_print_remove(struct rfc5444_print_session *session) {
  rfc5444_reader_remove_message_consumer(session->_reader, &session->_addr);
  rfc5444_reader_remove_message_consumer(session->_reader, &session->_msg);
  rfc5444_reader_remove_packet_consumer(session->_reader, &session->_pkt);
}

/**
 * This function converts a rfc5444 buffer into a human readable
 * form and print it into an buffer. To do this it allocates its own
 * rfc5444 reader, hooks in the printer macros, parse the packet and
 * cleans up the reader again.
 *
 * @param out pointer to output buffer
 * @param buffer pointer to packet to be printed
 * @param length length of packet in bytes
 * @return return code of reader, see rfc5444_result enum
 */
enum rfc5444_result
rfc5444_print_direct(struct autobuf *out, void *buffer, size_t length)
{
  struct rfc5444_reader reader;
  struct rfc5444_print_session session;
  enum rfc5444_result result;

  memset(&reader, 0, sizeof(reader));
  memset(&session, 0, sizeof(session));

  session.output = out;

  rfc5444_reader_init(&reader);
  rfc5444_print_add(&session, &reader);

  result = rfc5444_reader_handle_packet(&reader, buffer, length);
  if (result) {
    abuf_appendf(out, "Error while parsing rfc5444: %s\n", rfc5444_strerror(result));
  }
  rfc5444_print_remove(&session);
  rfc5444_reader_cleanup(&reader);

  return result;
}

static void
_print_hex(struct autobuf *out, uint8_t *ptr, size_t length) {
  size_t i;

  for (i = 0; i < length; i++) {
    abuf_appendf(out, "%s%02x", i == 0 ? "" : " ", ptr[i]);
  }
}

static int
_print_raw_tlvblock(struct autobuf *out, const char *prefix, uint8_t *blockptr, size_t *idx, size_t length) {
  char valueprefix[128];
  uint16_t blocklength, tlv_len, tlv_singlelength;
  uint8_t *tlvptr;
  size_t idx2;
  uint8_t tlv_flags, startidx, endidx;

  if (2 > length) {
    return -1;
  }

  abuf_appendf(out, "%s,-------------------\n", prefix);
  abuf_appendf(out, "%s|  TLV BLOCK\n", prefix);
  abuf_appendf(out, "%s|-------------------\n", prefix);

  blocklength = (blockptr[*idx] << 8) | blockptr[*idx + 1];
  abuf_appendf(out, "%s| * TLV Block Size: %u\n", prefix, blocklength);

  if (blocklength + 2u > length) {
    return -1;
  }

  *idx += 2;
  tlvptr = &blockptr[*idx];
  for (idx2 = 0; idx2 < blocklength;) {
    if (idx2 + 2 > blocklength) {
      return -1;
    }

    abuf_appendf(out, "%s|    ,-------------------\n", prefix);
    abuf_appendf(out, "%s|    |  TLV\n", prefix);
    abuf_appendf(out, "%s|    |-------------------\n", prefix);

    abuf_appendf(out, "%s|    | type:        %u\n", prefix, tlvptr[idx2]);
    idx2++;

    tlv_flags = tlvptr[idx2];
    abuf_appendf(out, "%s|    | flags:       0x%02x\n", prefix, tlv_flags);
    idx2++;

    if (tlv_flags & RFC5444_TLV_FLAG_TYPEEXT) {
      if (idx2 + 1 > blocklength) {
        return -1;
      }
      abuf_appendf(out, "%s|    | ext-type:    %u\n", prefix, tlvptr[idx2]);
      idx2++;
    }

    startidx = 0;
    endidx = 0;
    if (tlv_flags & (RFC5444_TLV_FLAG_SINGLE_IDX | RFC5444_TLV_FLAG_MULTI_IDX)) {
      if (idx2 + 1 > blocklength) {
        return -1;
      }
      startidx = tlvptr[idx2];
      endidx = startidx;
      abuf_appendf(out, "%s|    | index-start: %u\n", prefix, startidx);
      idx2++;
    }
    if (tlv_flags & (RFC5444_TLV_FLAG_MULTI_IDX)) {
      if (idx2 + 1 > blocklength) {
        return -1;
      }
      endidx = tlvptr[idx2];
      if (endidx < startidx) {
        return -1;
      }
      abuf_appendf(out, "%s|    | index-end:   %u\n", prefix, endidx);
      idx2++;
    }
    tlv_len = 0;
    if (tlv_flags & (RFC5444_TLV_FLAG_EXTVALUE)) {
      if (idx2 + 1 > blocklength) {
        return -1;
      }
      tlv_len = tlvptr[idx2] << 8;
      idx2++;
    }
    if (tlv_flags & (RFC5444_TLV_FLAG_VALUE)) {
      if (idx2 + 1 > blocklength) {
        return -1;
      }
      tlv_len |= tlvptr[idx2];
      idx2++;
    }
    if (tlv_flags & (RFC5444_TLV_FLAG_EXTVALUE | RFC5444_TLV_FLAG_VALUE)) {
      abuf_appendf(out, "%s|    | length:      %u\n", prefix, tlv_len);
    }

    if (idx2 + tlv_len > blocklength) {
      return -1;
    }
    if (tlv_flags & RFC5444_TLV_FLAG_MULTIVALUE) {
      if (tlv_len % (endidx - startidx + 1)) {
        return -1;
      }
      tlv_singlelength = tlv_len / (endidx - startidx + 1);
    }
    else {
      tlv_singlelength = tlv_len;
      endidx = startidx;
    }

    snprintf(valueprefix, sizeof(valueprefix), "%s|    |   ", prefix);
    for (; startidx <= endidx; startidx++) {
      if (idx2 + tlv_singlelength > blocklength) {
        return -1;
      }
      abuf_hexdump(out, valueprefix, &tlvptr[idx2], tlv_singlelength);
      idx2 += tlv_singlelength;
      abuf_puts(out, "\n");
    }
  }

  if (blocklength != idx2) {
    return -1;
  }
  *idx += blocklength;
  return 0;
}

int
rfc5444_print_raw(struct autobuf *out, void *buffer, size_t length) {
  static uint8_t ZEROTAIL[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  uint8_t *ptr;
  size_t idx, idx2, prefix_idx, i;
  uint8_t flags, head_len, tail_len, *head, *tail, num_addr;
  uint16_t msg_size, addr_length, mid_len;

  ptr = buffer;
  idx = 0;

  if (idx + 1 > length) {
    return -1;
  }

  abuf_puts(out, "\t,------------------\n");
  abuf_puts(out, "\t|  PACKET\n");
  abuf_puts(out, "\t|------------------\n");

  flags = ptr[0];
  abuf_appendf(out, "\t| Packet version:    %u\n", flags >> 4);
  abuf_appendf(out, "\t| Packet flags:      0x%02x\n", flags & 0x0f);
  idx++;

  if (flags & RFC5444_PKT_FLAG_SEQNO) {
    if (idx + 2 > length) {
      return -1;
    }

    abuf_appendf(out, "\t| Packet seq number: %u\n", (ptr[0] << 8) | ptr[1]);
    idx += 2;
  }

  if (flags & RFC5444_PKT_FLAG_TLV) {
    if (idx + 2 > length) {
      return -1;
    }
    if (_print_raw_tlvblock(out, "\t|    | ", ptr, &idx, length)) {
      return -1;
    }
  }

  while (idx < length) {
    idx2 = idx;

    /* print messages */
    if (idx2 + 4 > length) {
      return -1;
    }

    abuf_puts(out, "\t|    ,-------------------\n");
    abuf_puts(out, "\t|    |  MESSAGE\n");
    abuf_puts(out, "\t|    |-------------------\n");
    abuf_appendf(out, "\t|    | Message type:       %u\n", ptr[idx]);

    flags = ptr[idx2 + 1];
    abuf_appendf(out, "\t|    | Message flags:      0x%02x\n", flags >> 4);

    addr_length = (flags & 15) + 1;
    abuf_appendf(out, "\t|    | Address length:     %u\n", addr_length);

    msg_size = (ptr[idx2 + 2] << 8) + ptr[idx + 3];
    abuf_appendf(out, "\t|    | Size:             %u\n", msg_size);
    idx2 += 4;

    if (flags & RFC5444_MSG_FLAG_ORIGINATOR) {
      if (idx2 + addr_length > idx + msg_size) {
        return -1;
      }
      abuf_appendf(out, "\t|    | Originator address: ");
      _print_hex(out, &ptr[idx2], addr_length);
      idx2 += addr_length;
    }
    if (flags & RFC5444_MSG_FLAG_HOPLIMIT) {
      if (idx2 + 1 > idx + msg_size) {
        return -1;
      }
      abuf_appendf(out, "\t|    | Hop limit:          %u\n", ptr[idx2]);
      idx2++;
    }
    if (flags & RFC5444_MSG_FLAG_HOPCOUNT) {
      if (idx2 + 1 > idx + msg_size) {
        return -1;
      }
      abuf_appendf(out, "\t|    | Hop count:          %u\n", ptr[idx2]);
      idx2++;
    }
    if (flags & RFC5444_MSG_FLAG_SEQNO) {
      if (idx2 + 2 > idx + msg_size) {
        return -1;
      }
      abuf_appendf(out, "\t|    | Sequence Number:    %u\n", (ptr[idx2] << 8) | ptr[idx2 + 1]);
      idx2 += 2;
    }

    if (_print_raw_tlvblock(out, "\t|    |    ", ptr, &idx2, msg_size)) {
      return -1;
    }

    while (idx2 < idx + msg_size) {
      /* print address blocks */
      if (idx2 + 2 > idx + msg_size) {
        return -1;
      }

      abuf_puts(out, "\t|    |    ,-------------------\n");
      abuf_puts(out, "\t|    |    |  ADDRESS-BLOCK\n");
      abuf_puts(out, "\t|    |    |-------------------\n");

      num_addr = ptr[idx2];
      abuf_appendf(out, "\t|    |    | Num-Addr: %u\n", num_addr);

      flags = ptr[idx2 + 1];
      abuf_appendf(out, "\t|    |    | Flags:    0x%02x\n", flags);

      idx2 += 2;

      head_len = tail_len = 0;
      head = NULL;
      tail = NULL;
      if (flags & RFC5444_ADDR_FLAG_HEAD) {
        if (idx2 + 1 > idx + msg_size) {
          return -1;
        }

        head_len = ptr[idx2];
        idx2++;
        if (idx2 + head_len > idx + msg_size) {
          return -1;
        }

        head = &ptr[idx2];

        abuf_appendf(out, "\t|    |    | Head:     ");
        _print_hex(out, head, head_len);
        abuf_puts(out, "\n");

        idx2 += head_len;
      }
      if (flags & RFC5444_ADDR_FLAG_FULLTAIL) {
        if (idx2 + 1 > idx + msg_size) {
          return -1;
        }

        tail_len = ptr[idx2];
        idx2++;
        if (idx2 + tail_len > idx + msg_size) {
          return -1;
        }

        tail = &ptr[idx2];
        abuf_appendf(out, "\t|    |    | Tail:     ");
        _print_hex(out, tail, tail_len);
        abuf_puts(out, "\n");
        idx2 += tail_len;
      }
      if (flags & RFC5444_ADDR_FLAG_ZEROTAIL) {
        if (idx2 + 1 > idx + msg_size) {
          return -1;
        }

        tail_len = ptr[idx2];

        tail = ZEROTAIL;
        abuf_appendf(out, "\t|    |    | ZeroTail: ");
        _print_hex(out, tail, tail_len);
        abuf_puts(out, "\n");

        idx2++;
      }

      if (head_len + tail_len >= addr_length) {
        return -1;
      }

      mid_len = (addr_length - head_len - tail_len) * num_addr;
      if (idx2 + mid_len > idx + msg_size) {
        return -1;
      }

      prefix_idx = idx + mid_len;
      if (flags & RFC5444_ADDR_FLAG_SINGLEPLEN) {
        if (prefix_idx + 1 > idx + msg_size) {
          return -1;
        }
      }
      else if (flags & RFC5444_ADDR_FLAG_MULTIPLEN) {
        if (prefix_idx + num_addr > idx + msg_size) {
          return -1;
        }
      }
      else {
        prefix_idx = 0;
      }

      for (i = 0; i < num_addr; i++) {
        abuf_puts(out, "\t|    |    |    ,-------------------\n");
        abuf_puts(out, "\t|    |    |    |  Address\n");
        abuf_puts(out, "\t|    |    |    |-------------------\n");

        abuf_appendf(out, "\t|    |    |    | Address: ");

        if (head_len) {
          _print_hex(out, head, head_len);

          abuf_puts(out, " | ");
        }
        _print_hex(out, &ptr[idx2], addr_length - head_len - tail_len);
        idx2 += addr_length - head_len - tail_len;

        if (tail_len) {
          abuf_puts(out, " | ");

          _print_hex(out, tail, tail_len);
        }

        if (prefix_idx) {
          abuf_appendf(out, " / %u", ptr[prefix_idx]);
        }
        if (flags & RFC5444_ADDR_FLAG_MULTIPLEN) {
          prefix_idx++;
        }

        abuf_puts(out, "\n");
      }

      if (_print_raw_tlvblock(out, "\t|    |    |    ", ptr, &idx2, msg_size)) {
        return -1;
      }
    }

    if (idx + msg_size != idx2) {
      return -1;
    }

    idx = idx2;
  }

  return 0;
}

/**
 * Clear output buffer and print start of packet
 * @param context rfc5444 tlvblock reader context
 * @return see rfc5444_result enum
 */
static enum rfc5444_result
_cb_print_pkt_start(struct rfc5444_reader_tlvblock_context *context) {
  struct rfc5444_print_session *session;

  assert(context->type == RFC5444_CONTEXT_PACKET);

  session = container_of(context->consumer, struct rfc5444_print_session, _pkt);

  abuf_puts(session->output, "\t,------------------\n");
  abuf_puts(session->output, "\t|  PACKET\n");
  abuf_puts(session->output, "\t|------------------\n");
  abuf_appendf(session->output, "\t| * Packet version:    %u\n", context->pkt_version);
  abuf_appendf(session->output, "\t| * Packet flags:      0x%x\n", context->pkt_flags);
  if (context->has_pktseqno) {
    abuf_appendf(session->output, "\t| * Packet seq number: %u\n", context->pkt_seqno);
  }

  return RFC5444_OKAY;
}

/**
 * Print packet TLVs
 * @param tlv rfc5444 tlvblock entry
 * @param context rfc5444 tlvblock reader context
 * @return see rfc5444_result enum
 */
enum rfc5444_result
_cb_print_pkt_tlv(struct rfc5444_reader_tlvblock_entry *tlv, struct rfc5444_reader_tlvblock_context *context)
{
  struct rfc5444_print_session *session;

  assert(context->type == RFC5444_CONTEXT_PACKET);

  session = container_of(context->consumer, struct rfc5444_print_session, _pkt);

  abuf_puts(session->output, "\t|    | - TLV\n");
  abuf_appendf(session->output, "\t|    |     Flags = 0x%02x\n", tlv->flags);
  abuf_appendf(session->output, "\t|    |     Type = %u", tlv->type);
  if (tlv->type_ext != 0) {
    abuf_appendf(session->output, "; Type ext. = %u", tlv->type_ext);
  }
  abuf_puts(session->output, "\n");
  if (tlv->length > 0) {
    abuf_appendf(session->output, "\t|    |     Value length: %u\n", tlv->length);
    abuf_hexdump(session->output, "\t|    |       ", tlv->single_value, tlv->length);
  }
  return RFC5444_OKAY;
}

/**
 * Print end of packet and call print callback if necessary
 * @param context rfc5444 tlvblock reader context
 * @param dropped unused
 * @return see rfc5444_result enum
 */
enum rfc5444_result
_cb_print_pkt_end(struct rfc5444_reader_tlvblock_context *context, bool dropped __attribute__((unused)))
{
  struct rfc5444_print_session *session;
  session = container_of(context->consumer, struct rfc5444_print_session, _pkt);

  abuf_puts(session->output, "\t`------------------\n");

  if (session->print_packet) {
    session->print_packet(session);
  }
  return RFC5444_OKAY;
}

/**
 * Print start of message
 * @param context rfc5444 tlvblock reader context
 * @return see rfc5444_result enum
 */
enum rfc5444_result
_cb_print_msg_start(struct rfc5444_reader_tlvblock_context *context)
{
  struct rfc5444_print_session *session;
  struct netaddr_str buf;

  assert(context->type == RFC5444_CONTEXT_MESSAGE);

  session = container_of(context->consumer, struct rfc5444_print_session, _msg);

  abuf_puts(session->output, "\t|    ,-------------------\n");
  abuf_puts(session->output, "\t|    |  MESSAGE\n");
  abuf_puts(session->output, "\t|    |-------------------\n");
  abuf_appendf(session->output, "\t|    | * Message type:       %u\n", context->msg_type);
  abuf_appendf(session->output, "\t|    | * Message flags:      0x%02x\n", context->msg_flags);
  abuf_appendf(session->output, "\t|    | * Address length:     %u\n", context->addr_len);

  if (context->has_origaddr) {
    abuf_appendf(session->output, "\t|    | * Originator address: %s\n", netaddr_to_string(&buf, &context->orig_addr));
  }
  if (context->has_hoplimit) {
    abuf_appendf(session->output, "\t|    | * Hop limit:          %u\n", context->hoplimit);
  }
  if (context->has_hopcount) {
    abuf_appendf(session->output, "\t|    | * Hop count:          %u\n", context->hopcount);
  }
  if (context->has_seqno) {
    abuf_appendf(session->output, "\t|    | * Message seq number: %u\n", context->seqno);
  }

  return RFC5444_OKAY;
}

/**
 * Print message TLV
 * @param tlv tlvblock entry
 * @param context rfc5444 tlvblock reader context
 * @return see rfc5444_result enum
 */
enum rfc5444_result
_cb_print_msg_tlv(struct rfc5444_reader_tlvblock_entry *tlv, struct rfc5444_reader_tlvblock_context *context)
{
  struct rfc5444_print_session *session;

  assert(context->type == RFC5444_CONTEXT_MESSAGE);

  session = container_of(context->consumer, struct rfc5444_print_session, _msg);

  abuf_puts(session->output, "\t|    |    | - TLV\n");
  abuf_appendf(session->output, "\t|    |    |     Flags = 0x%02x\n", tlv->flags);
  abuf_appendf(session->output, "\t|    |    |     Type = %u", tlv->type);
  if (tlv->type_ext != 0) {
    abuf_appendf(session->output, "; Type ext. = %u", tlv->type_ext);
  }
  abuf_puts(session->output, "\n");
  if (tlv->length > 0) {
    abuf_appendf(session->output, "\t|    |    |     Value length: %u\n", tlv->length);
    abuf_hexdump(session->output, "\t|    |    |       ", tlv->single_value, tlv->length);
  }
  return RFC5444_OKAY;
}

/**
 * Print end of message
 * @param context rfc5444 tlvblock reader context
 * @param dropped unused
 * @return see rfc5444_result enum
 */
enum rfc5444_result
_cb_print_msg_end(struct rfc5444_reader_tlvblock_context *context, bool dropped __attribute__((unused)))
{
  struct rfc5444_print_session *session;

  assert(context->type == RFC5444_CONTEXT_MESSAGE);

  session = container_of(context->consumer, struct rfc5444_print_session, _msg);

  abuf_puts(session->output, "\t|    `-------------------\n");
  return RFC5444_OKAY;
}

/**
 * Print start of address
 * @param context rfc5444 tlvblock reader context
 * @return see rfc5444_result enum
 */
enum rfc5444_result
_cb_print_addr_start(struct rfc5444_reader_tlvblock_context *context)
{
  struct rfc5444_print_session *session;
  struct netaddr_str buf;

  assert(context->type == RFC5444_CONTEXT_ADDRESS);

  session = container_of(context->consumer, struct rfc5444_print_session, _addr);

  abuf_puts(session->output, "\t|    |    ,-------------------\n");
  abuf_appendf(session->output, "\t|    |    |  Address: %s\n", netaddr_to_string(&buf, &context->addr));
  return RFC5444_OKAY;
}

/**
 * Print address tlv
 * @param tlv tlvblock entry
 * @param context rfc5444 tlvblock reader context
 * @return see rfc5444_result enum
 */
enum rfc5444_result
_cb_print_addr_tlv(struct rfc5444_reader_tlvblock_entry *tlv, struct rfc5444_reader_tlvblock_context *context)
{
  struct rfc5444_print_session *session;

  assert(context->type == RFC5444_CONTEXT_ADDRESS);

  session = container_of(context->consumer, struct rfc5444_print_session, _addr);

  abuf_puts(session->output, "\t|    |    |    | - TLV\n");
  abuf_appendf(session->output, "\t|    |    |    |     Flags = 0x%02x\n", tlv->flags);
  abuf_appendf(session->output, "\t|    |    |    |     Type = %u", tlv->type);
  if (tlv->type_ext != 0) {
    abuf_appendf(session->output, "; Type ext. = %u", tlv->type_ext);
  }
  abuf_puts(session->output, "\n");
  if (tlv->length > 0) {
    abuf_appendf(session->output, "\t|    |    |    |     Value length: %u\n", tlv->length);
    abuf_hexdump(session->output, "\t|    |    |    |       ", tlv->single_value, tlv->length);
  }
  return RFC5444_OKAY;
}

/**
 * Print end of address
 * @param context rfc5444 tlvblock reader context
 * @param dropped unused
 * @return see rfc5444_result enum
 */
enum rfc5444_result
_cb_print_addr_end(struct rfc5444_reader_tlvblock_context *context, bool dropped __attribute__((unused)))
{
  struct rfc5444_print_session *session;

  assert(context->type == RFC5444_CONTEXT_ADDRESS);

  session = container_of(context->consumer, struct rfc5444_print_session, _addr);

  abuf_puts(session->output, "\t|    |    `-------------------\n");
  return RFC5444_OKAY;
}
