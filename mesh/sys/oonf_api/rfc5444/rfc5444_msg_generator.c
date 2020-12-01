
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

// #define DEBUG_OUTPUT

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#ifdef DEBUG_OUTPUT
#include <stdio.h>
#endif

#include "common/avl.h"
#include "common/common_types.h"
#include "common/list.h"
#include "common/netaddr.h"

#include "rfc5444_api_config.h"
#include "rfc5444_writer.h"

/**
 * data necessary for automatic address compression
 */
struct _rfc5444_internal_addr_compress_session {
  /*! address at the start of the current block */
  struct rfc5444_writer_address *ptr;

  /*! total number of bytes if this compression strategy is used */
  int total;

  /*! number of bytes of current address block */
  int current;

  /*! true if address block has multiple prefix lengths */
  bool multiplen;

  /*! true if address block has to be closed */
  bool closed;
};

static void _close_addrblock(struct _rfc5444_internal_addr_compress_session *acs, struct rfc5444_writer *writer,
  struct rfc5444_writer_address *last_addr, int);
static void _finalize_message_fragment(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg,
  struct oonf_list_entity *fragment_addrs, bool not_fragmented, rfc5444_writer_targetselector useIf, void *param);
static int _compress_address(struct _rfc5444_internal_addr_compress_session *acs, struct rfc5444_writer *writer,
  struct oonf_list_entity *addr_list, int same_prefixlen);
static void _write_addresses(
  struct rfc5444_writer *writer, struct rfc5444_writer_message *msg, struct oonf_list_entity *fragment_addrs);
static void _write_msgheader(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg);
static uint8_t *_write_addresstlvs(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg,
  struct rfc5444_writer_address *first, struct rfc5444_writer_address *last, uint8_t *ptr);

/*! temporary buffer for messages when going through a postprocessor */
static uint8_t _msg_buffer[RFC5444_MAX_MESSAGE_SIZE];

/**
 * Create a message with a defined type
 * This function must NOT be called from the rfc5444 writer callbacks.
 *
 * @param writer pointer to writer context
 * @param msgid type of message
 * @param addr_len length of address for this message
 * @param useIf pointer to interface selector
 * @param param last parameter of interface selector
 * @return RFC5444_OKAY if message was created and added to packet buffer,
 *   RFC5444_... otherwise
 */
enum rfc5444_result
rfc5444_writer_create_message(
  struct rfc5444_writer *writer, uint8_t msgid, uint8_t addr_len, rfc5444_writer_targetselector useIf, void *param)
{
  struct rfc5444_writer_message *msg;
  struct rfc5444_writer_content_provider *prv;
  struct oonf_list_entity *ptr1;
  struct rfc5444_writer_address *addr;
  struct rfc5444_writer_address *first_addr;
  struct rfc5444_writer_address *first_processed, *last_processed;
  struct rfc5444_writer_tlvtype *tlvtype;
  struct rfc5444_writer_target *target;
  struct oonf_list_entity current_list;

  struct rfc5444_writer_postprocessor *processor;
  size_t processor_preallocation;

  struct _rfc5444_internal_addr_compress_session acs[RFC5444_MAX_ADDRLEN];
  int best_size, best_head, same_prefixlen;
  int i, idx, non_mandatory;
  bool first;
  bool not_fragmented;
  size_t max_msg_size;
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif

  /* do nothing if no target is defined */
  if (oonf_list_is_empty(&writer->_targets)) {
    return RFC5444_OKAY;
  }

  /* find message create instance for the requested message */
  msg = avl_find_element(&writer->_msgcreators, &msgid, msg, _msgcreator_node);
  if (msg == NULL) {
    /* error, no msgcreator found */
    return RFC5444_NO_MSGCREATOR;
  }

  /*
   * test if we need target specific messages
   * and this is not the single_if selector
   */
  if (!msg->target_specific) {
    /* not target specific */
    writer->msg_target = NULL;
  }
  else if (useIf == rfc5444_writer_singletarget_selector) {
    /* target specific, but single_if selector is used */
    writer->msg_target = param;
  }
  else {
    /* target specific, but generic selector is used */
    enum rfc5444_result result;

    oonf_list_for_each_element(&writer->_targets, target, _target_node) {
      /* check if we should send over this target */
      if (!useIf(writer, target, param)) {
        continue;
      }

      /* create an unique message by recursive call */
      result = rfc5444_writer_create_message(writer, msgid, addr_len, rfc5444_writer_singletarget_selector, target);
      if (result != RFC5444_OKAY) {
        return result;
      }
    }
    return RFC5444_OKAY;
  }

  /* set address length */
  writer->msg_addr_len = addr_len;

  /*
   * initialize packet buffers for all interfaces if necessary
   * and calculate message MTU
   */

  /* loop over post-processors */
  processor_preallocation = 0;
  avl_for_each_element(&writer->_processors, processor, _node) {
    if (processor->is_matching_signature(processor, msg->type)) {
      processor_preallocation += processor->allocate_space;
    }
  }
  max_msg_size = writer->msg_size;
  oonf_list_for_each_element(&writer->_targets, target, _target_node) {
    size_t interface_msg_mtu;

    /* check if we should send over this target */
    if (!useIf(writer, target, param)) {
      continue;
    }

    /* start packet if necessary */
    if (target->_is_flushed) {
      _rfc5444_writer_begin_packet(writer, target);
    }

    interface_msg_mtu = target->packet_size - processor_preallocation -
                        (target->_pkt.header + target->_pkt.added + target->_pkt.allocated);
    if (interface_msg_mtu < max_msg_size) {
      max_msg_size = interface_msg_mtu;
    }
  }

  /* initialize message tlvdata */
  _rfc5444_tlv_writer_init(&writer->_msg, max_msg_size, writer->msg_size);

#if WRITER_STATE_MACHINE == true
  writer->_state = RFC5444_WRITER_ADD_HEADER;
#endif
  /* let the message creator write the message header */
  rfc5444_writer_set_msg_header(writer, msg, false, false, false, false);
  if (msg->addMessageHeader) {
    if (msg->addMessageHeader(writer, msg)) {
      /* message handler does not want this message */
#if WRITER_STATE_MACHINE == true
      writer->_state = RFC5444_WRITER_NONE;
#endif
      writer->msg_addr_len = 0;
      return RFC5444_OKAY;
    }
  }

#if WRITER_STATE_MACHINE == true
  writer->_state = RFC5444_WRITER_ADD_MSGTLV;
#endif

  /* call content providers for message TLVs */
  avl_for_each_element(&msg->_provider_tree, prv, _provider_node) {
    if (prv->addMessageTLVs) {
      prv->addMessageTLVs(writer);
    }
  }

#if WRITER_STATE_MACHINE == true
  writer->_state = RFC5444_WRITER_ADD_ADDRESSES;
#endif
  /* call content providers for addresses */
  avl_for_each_element(&msg->_provider_tree, prv, _provider_node) {
    if (prv->addAddresses) {
      prv->addAddresses(writer);
    }
  }

  /* join mandatory and normal address list */
  oonf_list_merge(&msg->_addr_head, &msg->_non_mandatory_addr_head);

  /* initialize list of current addresses */
  oonf_list_init_head(&current_list);
  not_fragmented = true;

  /* no addresses ? */
  if (oonf_list_is_empty(&msg->_addr_head)) {
    _finalize_message_fragment(writer, msg, &current_list, true, useIf, param);
#if WRITER_STATE_MACHINE == true
    writer->_state = RFC5444_WRITER_NONE;
#endif
    _rfc5444_writer_free_addresses(writer, msg);
    return RFC5444_OKAY;
  }

  /* start address compression */
  first = true;
  addr = first_addr = oonf_list_first_element(&msg->_addr_head, addr, _addr_oonf_list_node);

  same_prefixlen = 0;

  first_processed = NULL;
  last_processed = NULL;

  max_msg_size -= writer->_msg.header;
  max_msg_size -= writer->_msg.allocated;
  max_msg_size -= writer->_msg.added;

  /* loop through addresses */
  idx = 0;
  non_mandatory = 0;
  ptr1 = msg->_addr_head.next;
  while (ptr1 != &msg->_addr_head) {
    addr = container_of(ptr1, struct rfc5444_writer_address, _addr_oonf_list_node);
    if (addr->_done && !addr->_mandatory_addr) {
      ptr1 = ptr1->next;
      continue;
    }

    if (first) {
      /* clear message specific tlvtype information for address compression */
      oonf_list_for_each_element(&msg->_msgspecific_tlvtype_head, tlvtype, _tlvtype_node) {
        memset(tlvtype->_tlvblock_count, 0, sizeof(tlvtype->_tlvblock_count));
        memset(tlvtype->_tlvblock_multi, 0, sizeof(tlvtype->_tlvblock_multi));
      }

      /* clear generic tlvtype information for address compression */
      oonf_list_for_each_element(&writer->_addr_tlvtype_head, tlvtype, _tlvtype_node) {
        memset(tlvtype->_tlvblock_count, 0, sizeof(tlvtype->_tlvblock_count));
        memset(tlvtype->_tlvblock_multi, 0, sizeof(tlvtype->_tlvblock_multi));
      }

      /* clear address compression session */
      memset(acs, 0, sizeof(acs));
      same_prefixlen = 1;

      first_processed = addr;
    }

    addr->index = idx++;
    oonf_list_add_tail(&current_list, &addr->_addr_fragment_node);

    /* update session with address */
    same_prefixlen = _compress_address(acs, writer, &current_list, same_prefixlen);
    first = false;

    /* look for best current compression */
    best_head = -1;
    best_size = max_msg_size + 1;
    for (i = 0; i < writer->msg_addr_len; i++) {
      int size = acs[i].total + acs[i].current;
      int count = addr->index - acs[i].ptr->index;

      /* a block of 255 addresses have an index difference of 254 */
      if (size < best_size && count <= 254) {
        best_head = i;
        best_size = size;
      }
    }

    /* fragmentation necessary ? */
    if (best_head == -1) {
      if (non_mandatory == 0) {
        /* the mandatory addresses plus one non-mandatory do not fit into a block! */
#if WRITER_STATE_MACHINE == true
        writer->_state = RFC5444_WRITER_NONE;
#endif
        _rfc5444_writer_free_addresses(writer, msg);
        return RFC5444_MTU_TOO_SMALL;
      }
      not_fragmented = false;

      /* one address too many */
      oonf_list_remove(&addr->_addr_fragment_node);

      _close_addrblock(acs, writer, last_processed, 0);
#ifdef DEBUG_OUTPUT
      printf("Finalize with head length: %d\n", last_processed->_block_headlen);
#endif
      /* write message fragment */
      _finalize_message_fragment(writer, msg, &current_list, not_fragmented, useIf, param);

      /* reset loop */
      oonf_list_init_head(&current_list);
      ptr1 = &first_processed->_addr_oonf_list_node;
      first = true;

      /* continue without stepping forward */
      continue;
    }
    else {
      /* add cost for this address to total costs */
      for (i = 0; i < writer->msg_addr_len; i++) {
        acs[i].total += acs[i].current;

#if DEBUG_CLEANUP == true
        acs[i].current = 0;
#endif
      }
      last_processed = addr;

      if (!addr->_done) {
        addr->_done = true;

        if (!addr->_mandatory_addr) {
          non_mandatory++;
        }
      }
    }

    ptr1 = ptr1->next;
  }

  if (last_processed) {
    _close_addrblock(acs, writer, last_processed, 0);

    /* write message fragment */
    _finalize_message_fragment(writer, msg, &current_list, not_fragmented, useIf, param);
  }

  /* free storage of addresses and address-tlvs */
  _rfc5444_writer_free_addresses(writer, msg);

#if WRITER_STATE_MACHINE == true
  writer->_state = RFC5444_WRITER_NONE;
#endif
  writer->msg_addr_len = 0;

  return RFC5444_OKAY;
}

/**
 * Single interface selector callback for message creation
 * @param writer RFC5444 writer instance
 * @param interf RFC5444 writer target
 * @param param pointer to the specified interface
 * @return true if param equals interf, false otherwise
 */
bool
rfc5444_writer_singletarget_selector(
  struct rfc5444_writer *writer __attribute__((unused)), struct rfc5444_writer_target *interf, void *param) {
  return interf == param;
}

/**
 * All interface selector callback for message creation
 * @param writer RFC5444 writer instance
 * @param interf RFC5444 writer target
 * @param param unused
 * @return always true
 */
bool
rfc5444_writer_alltargets_selector(struct rfc5444_writer *writer __attribute__((unused)),
  struct rfc5444_writer_target *interf __attribute__((unused)), void *param __attribute__((unused))) {
  return true;
}

/**
 * Write a binary rfc5444 message into the writers buffer to
 * forward it. This function handles the modification of hopcount
 * and hoplimit field. The original message will not be modified.
 * This function must NOT be called from the rfc5444 writer callbacks.
 *
 * The function does demand the writer context pointer as void*
 * to be compatible with the readers forward_message callback.
 *
 * @param writer pointer to writer context
 * @param context RFC5444 reader context of message
 * @param msg pointer to message to be forwarded
 * @param len number of bytes of message
 * @return RFC5444_OKAY if the message was put into the writer buffer,
 *   RFC5444_... if an error happened
 */
enum rfc5444_result
rfc5444_writer_forward_msg(
  struct rfc5444_writer *writer, struct rfc5444_reader_tlvblock_context *context, const uint8_t *msg, size_t len)
{
  struct rfc5444_writer_target *target;
  struct rfc5444_writer_message *rfc5444_msg;
  struct rfc5444_writer_forward_handler *handler;
  int cnt, hopcount, hoplimit;
  size_t max, transformer_overhead;
  size_t generic_size, msg_size;
  uint8_t flags, addr_len;
  uint8_t *ptr;
  bool shall_forward;
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif

  rfc5444_msg = avl_find_element(&writer->_msgcreators, &context->msg_type, rfc5444_msg, _msgcreator_node);
  if (rfc5444_msg == NULL) {
    /* error, no msgcreator found */
    return RFC5444_NO_MSGCREATOR;
  }

  if (!rfc5444_msg->forward_target_selector) {
    /* no forwarding handler, do not forward */
    return RFC5444_OKAY;
  }

  /* 1.) first flush all interfaces that have (too) full buffers */
  shall_forward = false;
  oonf_list_for_each_element(&writer->_targets, target, _target_node) {
    if (!rfc5444_msg->forward_target_selector(target, context)) {
      continue;
    }

    shall_forward = true;
    transformer_overhead = 0;
    avl_for_each_element(&writer->_forwarding_processors, handler, _node) {
      if (handler->is_matching_signature(handler, msg[0])) {
        transformer_overhead += handler->allocate_space;
      }
    }

    max = 0;
    if (!target->_is_flushed) {
      max =
        target->_pkt.max - (target->_pkt.header + target->_pkt.added + target->_pkt.allocated + target->_bin_msgs_size);

      if (len + transformer_overhead > max) {
        /* flush the old packet */
        rfc5444_writer_flush(writer, target, false);
      }
    }

    if (target->_is_flushed) {
      /* begin a new packet */
      _rfc5444_writer_begin_packet(writer, target);
      max =
        target->_pkt.max - (target->_pkt.header + target->_pkt.added + target->_pkt.allocated + target->_bin_msgs_size);
    }

    if (len + transformer_overhead > max) {
      /* message too long, too much data in it */
      return RFC5444_FW_MESSAGE_TOO_LONG;
    }
  }

  if (!shall_forward) {
    /* no target to forward message, everything done */
    return RFC5444_OKAY;
  }

  /* 2.) generate message and do non-target specific post processors */
  ptr = _msg_buffer;
  memcpy(ptr, writer->_msg.buffer, len);

  /* remember length */
  generic_size = len;

  /* run processors (unpspecific) forwarding processors */
  avl_for_each_element(&writer->_forwarding_processors, handler, _node) {
    if (handler->is_matching_signature(handler, msg[0]) && !handler->target_specific) {
      if (handler->process(handler, target, context, _msg_buffer, &generic_size)) {
        /* error, we have not modified the _bin_msgs_size, so we can just return */
        return RFC5444_FW_BAD_TRANSFORM;
      }

      if (generic_size == 0) {
        return RFC5444_OKAY;
      }
    }
  }

  /* 3) grab index of header structures */
  flags = msg[1];
  addr_len = (flags & RFC5444_MSG_FLAG_ADDRLENMASK) + 1;

  cnt = 4;
  hopcount = -1;
  hoplimit = -1;

  if ((flags & RFC5444_MSG_FLAG_ORIGINATOR) != 0) {
    cnt += addr_len;
  }
  if ((flags & RFC5444_MSG_FLAG_HOPLIMIT) != 0) {
    hoplimit = cnt++;
  }
  if ((flags & RFC5444_MSG_FLAG_HOPCOUNT) != 0) {
    hopcount = cnt++;
  }
  if ((flags & RFC5444_MSG_FLAG_SEQNO) != 0) {
    cnt += 2;
  }

  if (hoplimit != -1 && msg[hoplimit] <= 1) {
    /* do not forward a message with hopcount 1 or 0 */
    return RFC5444_OKAY;
  }

  /* forward message */
  oonf_list_for_each_element(&writer->_targets, target, _target_node) {
    if (!rfc5444_msg->forward_target_selector(target, context)) {
      continue;
    }

    ptr =
      &target->_pkt.buffer[target->_pkt.header + target->_pkt.added + target->_pkt.allocated + target->_bin_msgs_size];

    /* copy message into packet buffer */
    assert(ptr + generic_size <= target->_pkt.buffer + target->_pkt.max);
    memcpy(ptr, msg, generic_size);

    /* remember position of first copy */
    msg_size = generic_size;

    /* run processors */
    avl_for_each_element(&writer->_forwarding_processors, handler, _node) {
      if (handler->is_matching_signature(handler, msg[0]) && handler->target_specific) {
        if (handler->process(handler, target, context, ptr, &msg_size)) {
          /* error, we have not modified the _bin_msgs_size, so we can just return */
          return RFC5444_FW_BAD_TRANSFORM;
        }
        if (msg_size == 0) {
          break;
        }
      }
    }

    if (msg_size > 0) {
      target->_bin_msgs_size += msg_size;

      /* correct message */
      ptr[2] = msg_size >> 8;
      ptr[3] = msg_size & 0xff;

      /* correct hoplimit if necesssary */
      if (hoplimit != -1) {
        ptr[hoplimit]--;
      }

      /* correct hopcount if necessary */
      if (hopcount != -1) {
        ptr[hopcount]++;
      }

      if (writer->message_generation_notifier) {
        writer->message_generation_notifier(target);
      }
    }
  }
  return RFC5444_OKAY;
}

/**
 * Adds a tlv to a message.
 * This function must not be called outside the message add_tlv callback.
 *
 * @param writer pointer to writer context
 * @param type tlv type
 * @param exttype tlv extended type, 0 if no extended type
 * @param value pointer to tlv value, NULL if no value
 * @param length number of bytes in tlv value, 0 if no value
 * @return RFC5444_OKAY if tlv has been added to packet, RFC5444_... otherwise
 */
enum rfc5444_result
rfc5444_writer_add_messagetlv(
  struct rfc5444_writer *writer, uint8_t type, uint8_t exttype, const void *value, size_t length)
{
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_ADD_MSGTLV);
#endif
  return _rfc5444_tlv_writer_add(&writer->_msg, type, exttype, value, length);
}

/**
 * Allocate memory for message tlv.
 * This function must not be called outside the message add_tlv callback.
 *
 * @param writer pointer to writer context
 * @param has_exttype true if tlv has an extended type
 * @param length number of bytes in tlv value, 0 if no value
 * @return RFC5444_OKAY if memory for tlv has been allocated, RFC5444_... otherwise
 */
enum rfc5444_result
rfc5444_writer_allocate_messagetlv(struct rfc5444_writer *writer, bool has_exttype, size_t length)
{
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_ADD_MSGTLV);
#endif
  return _rfc5444_tlv_writer_allocate(&writer->_msg, has_exttype, length);
}

/**
 * Sets a tlv for a message, which memory has been already allocated.
 * This function must not be called outside the message finish_tlv callback.
 *
 * @param writer pointer to writer context
 * @param type tlv type
 * @param exttype tlv extended type, 0 if no extended type
 * @param value pointer to tlv value, NULL if no value
 * @param length number of bytes in tlv value, 0 if no value
 * @return RFC5444_OKAY if tlv has been set to packet, RFC5444_... otherwise
 */
enum rfc5444_result
rfc5444_writer_set_messagetlv(
  struct rfc5444_writer *writer, uint8_t type, uint8_t exttype, const void *value, size_t length)
{
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_FINISH_MSGTLV);
#endif
  return _rfc5444_tlv_writer_set(&writer->_msg, type, exttype, value, length);
}

/**
 * Initialize the header of a message.
 * This function must not be called outside the message add_header callback.
 *
 * @param writer pointer to writer context
 * @param msg pointer to message object
 * @param has_originator true if header contains an originator address
 * @param has_hopcount true if header contains a hopcount
 * @param has_hoplimit true if header contains a hoplimit
 * @param has_seqno true if header contains a sequence number
 */
void
rfc5444_writer_set_msg_header(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg, bool has_originator,
  bool has_hopcount, bool has_hoplimit, bool has_seqno) {
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_ADD_HEADER);
#endif

  msg->has_origaddr = has_originator;
  msg->has_hoplimit = has_hoplimit;
  msg->has_hopcount = has_hopcount;
  msg->has_seqno = has_seqno;

  /* fixed parts: _msg type, flags, length, tlvblock-length */
  writer->_msg.header = 6;

  if (has_originator) {
    writer->_msg.header += writer->msg_addr_len;
  }
  if (has_hoplimit) {
    writer->_msg.header++;
  }
  if (has_hopcount) {
    writer->_msg.header++;
  }
  if (has_seqno) {
    writer->_msg.header += 2;
  }
}

/**
 * Set originator address of a message header
 * This function must not be called outside the message
 * add_header or finish_header callback.
 *
 * @param writer pointer to writer context
 * @param msg pointer to message object
 * @param originator pointer to originator address buffer
 */
void
rfc5444_writer_set_msg_originator(
  struct rfc5444_writer *writer, struct rfc5444_writer_message *msg, const void *originator) {
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_ADD_HEADER || writer->_state == RFC5444_WRITER_FINISH_HEADER);
#endif

  memcpy(&msg->orig_addr[0], originator, writer->msg_addr_len);
}

/**
 * Set hopcount of a message header
 * This function must not be called outside the message
 * add_header or finish_header callback.
 *
 * @param writer pointer to writer context
 * @param msg pointer to message object
 * @param hopcount hopcount to set
 */
void
rfc5444_writer_set_msg_hopcount(
  struct rfc5444_writer *writer __attribute__((unused)), struct rfc5444_writer_message *msg, uint8_t hopcount) {
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_ADD_HEADER || writer->_state == RFC5444_WRITER_FINISH_HEADER);
#endif
  msg->hopcount = hopcount;
}

/**
 * Set hoplimit of a message header
 * This function must not be called outside the message
 * add_header or finish_header callback.
 *
 * @param writer pointer to writer context
 * @param msg pointer to message object
 * @param hoplimit hoplimit to set
 */
void
rfc5444_writer_set_msg_hoplimit(
  struct rfc5444_writer *writer __attribute__((unused)), struct rfc5444_writer_message *msg, uint8_t hoplimit) {
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_ADD_HEADER || writer->_state == RFC5444_WRITER_FINISH_HEADER);
#endif
  msg->hoplimit = hoplimit;
}

/**
 * Set sequence number of a message header
 * This function must not be called outside the message
 * add_header or finish_header callback.
 *
 * @param writer pointer to writer context
 * @param msg pointer to message object
 * @param seqno sequence number of message header
 */
void
rfc5444_writer_set_msg_seqno(
  struct rfc5444_writer *writer __attribute__((unused)), struct rfc5444_writer_message *msg, uint16_t seqno) {
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_ADD_HEADER || writer->_state == RFC5444_WRITER_FINISH_HEADER);
#endif
  msg->seqno = seqno;
}

/**
 * Update address compression session when a potential address block
 * is finished.
 *
 * @param acs pointer to address compression session
 * @param writer pointer to rfc5444 writer
 * @param last_addr pointer to last address object
 * @param common_head length of common head
 */
static void
_close_addrblock(struct _rfc5444_internal_addr_compress_session *acs, struct rfc5444_writer *writer,
  struct rfc5444_writer_address *last_addr, int common_head) {
  int best;
  int i, size;
  if (common_head > writer->msg_addr_len) {
    /* nothing to do */
    return;
  }

  /* check for best compression at closed blocks */
  best = common_head;
  size = acs[common_head].total;
  for (i = common_head + 1; i < writer->msg_addr_len; i++) {
    if (acs[i].total < size) {
      size = acs[i].total;
      best = i;
    }
  }

  if (!acs[best].closed || !last_addr->_block_start) {
    /* store address block for later binary generation */
    last_addr->_block_start = acs[best].ptr;
    last_addr->_block_multiple_prefixlen = acs[best].multiplen;
    last_addr->_block_headlen = best;
  }

  for (i = common_head + 1; i < writer->msg_addr_len; i++) {
    /* remember best block compression */
    acs[i].total = size;
  }
  return;
}

/**
 * Update the address compression session with a new address.
 *
 * @param acs pointer to address compression session
 * @param writer pointer to rfc5444 writer
 * @param addr_list list of addresses
 * @param same_prefixlen number of addresses (up to this) with the same
 *   prefix length
 * @return new number of messages with same prefix length
 */
static int
_compress_address(struct _rfc5444_internal_addr_compress_session *acs, struct rfc5444_writer *writer,
  struct oonf_list_entity *addr_list, int same_prefixlen) {
  struct rfc5444_writer_address *addr, *last_addr;
  struct rfc5444_writer_addrtlv *tlv, *last_tlv;
  struct rfc5444_writer_tlvtype *tlvtype;
  uint32_t i, common_head;
  const uint8_t *addrptr, *last_addrptr;
  int cost, new_cost, continue_cost;
  uint8_t addrlen;
  bool special_prefixlen;
  bool closed;

  addr = oonf_list_last_element(addr_list, addr, _addr_fragment_node);
  if (!oonf_list_is_first(addr_list, &addr->_addr_fragment_node)) {
    last_addr = oonf_list_prev_element(addr, _addr_fragment_node);
  }
  else {
    last_addr = NULL;
  }

  addrlen = writer->msg_addr_len;
  common_head = 0;
  special_prefixlen = netaddr_get_prefix_length(&addr->address) != addrlen * 8;

  addrptr = netaddr_get_binptr(&addr->address);

#ifdef DEBUG_OUTPUT
  {
    struct netaddr_str nbuf1, nbuf2;
    printf("Compress Address %s (last: %s)\n", netaddr_to_string(&nbuf1, &addr->address),
      last_addr == NULL ? "" : netaddr_to_string(&nbuf2, &last_addr->address));
    printf("\ttotal:   ");
    for (i = 0; i < addrlen; i++) {
      printf(" %4d ", acs[i].total);
    }
    printf("\n");
  }
#endif
  /* add size for address part (and header if necessary) */
  if (last_addr) {
    /* remember how many entries with the same prefixlength we had */
    if (netaddr_get_prefix_length(&last_addr->address) == netaddr_get_prefix_length(&addr->address)) {
      same_prefixlen++;
    }
    else {
      same_prefixlen = 1;
    }

    /* add bytes to continue encodings with same prefix */
    last_addrptr = netaddr_get_binptr(&last_addr->address);
    for (common_head = 0; common_head < addrlen; common_head++) {
      if (last_addrptr[common_head] != addrptr[common_head]) {
        break;
      }
    }
    _close_addrblock(acs, writer, last_addr, common_head);
#ifdef DEBUG_OUTPUT
    printf("\tt-closed:");
    for (i = 0; i < addrlen; i++) {
      printf(" %4d ", acs[i].total);
    }
    printf("\n");
#endif
  }

#ifdef DEBUG_OUTPUT
  printf("\tcurrent:");
#endif

  /* calculate tlv flags */
  avl_for_each_element(&addr->_addrtlv_tree, tlv, addrtlv_node) {
    tlvtype = tlv->tlvtype;

    tlv->_same_length = false;
    tlv->_same_value = false;

    if (last_addr) {
      last_tlv = avl_find_element(&last_addr->_addrtlv_tree, &tlvtype->_full_type, last_tlv, addrtlv_node);
      if (last_tlv && last_tlv->length == tlv->length) {
        tlv->_same_length = true;
        tlv->_same_value = memcmp(tlv->value, last_tlv->value, tlv->length) == 0;
      }
    }
  }

  /* calculate new costs for next address including tlvs */
  for (i = 0; i < addrlen; i++) {
    new_cost = 0;
    continue_cost = 0;
    closed = false;

    closed = last_addr == NULL;
    if (common_head < i) {
      closed = true;
    }
    /* cost of new address header */
    new_cost = 2 + (i > 0 ? 1 : 0) + writer->msg_addr_len;
    if (special_prefixlen) {
      new_cost++;
    }

    if (!closed) {
      /* cost of continuing the last address header */
      continue_cost = writer->msg_addr_len - i;
      if (acs[i].multiplen) {
        /* will stay multi_prefixlen */
        continue_cost++;
      }
      else if (same_prefixlen == 1) {
        /* will become multi_prefixlen */
        continue_cost += (acs[i].ptr->index - addr->index + 1);
        acs[i].multiplen = true;
      }
    }

    /* mandatory TLV block */
    new_cost += 2;

    /* calculate costs for breaking/continuing tlv sequences */
    avl_for_each_element(&addr->_addrtlv_tree, tlv, addrtlv_node) {
      tlvtype = tlv->tlvtype;

      /* type + flags */
      cost = 2;

      if (tlvtype->exttype > 0) {
        cost++;
      }

      /* TODO: dynamic index fields? */
      cost += 2;

      if (tlv->length > 255) {
        /* 2 byte length field */
        cost++;
      }
      if (tlv->length > 0) {
        /* 1 or 2 byte length field */
        cost++;
      }

      /* value */
      cost += tlv->length;

      /* add to cost for a new TLV block */
      new_cost += cost;

      /* check if we are forced to do a new tlv block anyways */
      if (closed || !tlv->_same_length) {
        /*
         * this TLV does not continue, either because address block is ending or because
         * value length changed
         */
        continue_cost += cost;
        continue;
      }

      if (tlvtype->_tlvblock_multi[i]) {
        /* we are already within a TLV with multiple values, so add another one */
        continue_cost += tlv->length;
      }
      else if (!tlv->_same_value) {
        /* ups, value changed. Change cost estimate to multivalue TLV */
        continue_cost += tlv->length * tlvtype->_tlvblock_count[i];
      }
    }
#ifdef DEBUG_OUTPUT
    printf(" %2d/%2d", continue_cost, closed ? -1 : new_cost);
#endif
    if (closed || acs[i].total + continue_cost > acs[addrlen - 1].total + new_cost) {
      /* forget the last addresses, longer prefix is better. */
      /* store address block for later binary generation */
#if 0
      if (last_addr) {
        last_addr->_block_start = acs[addrlen-1].ptr;
        last_addr->_block_multiple_prefixlen = acs[addrlen-1].multiplen;
        last_addr->_block_headlen = addrlen-1;
      }
#endif
      /* Create a new address block */
      acs[i].ptr = addr;
      acs[i].multiplen = false;

      acs[i].total = acs[addrlen - 1].total;
      acs[i].current = new_cost;
      closed = true;
    }
    else {
      acs[i].current = continue_cost;
    }

    if (last_addr) {
      acs[i].closed = closed;
    }

    /* update internal tlv calculation */
    avl_for_each_element(&addr->_addrtlv_tree, tlv, addrtlv_node) {
      tlvtype = tlv->tlvtype;

      if (closed || !tlv->_same_length) {
        tlvtype->_tlvblock_count[i] = 1;
        tlvtype->_tlvblock_multi[i] = false;
      }
      else {
        tlvtype->_tlvblock_count[i]++;
        if (!tlv->_same_value) {
          tlvtype->_tlvblock_multi[i] = true;
        }
      }
    }
  }
#ifdef DEBUG_OUTPUT
  printf("\n");
#endif
  return same_prefixlen;
}

static uint8_t *
_write_addresstlv(struct rfc5444_writer_tlvtype *tlvtype, struct rfc5444_writer_address *addr_first,
  struct rfc5444_writer_address *addr_last, uint8_t *ptr) {
  struct rfc5444_writer_addrtlv *tlv_first, *tlv_last, *tlv;
  uint16_t total_len;
  uint8_t *flag;

  /* get edge tlvs */
  tlv_first = oonf_list_first_element(&tlvtype->_current_tlv_list, tlv_first, _current_tlv_node);
  tlv_last = oonf_list_last_element(&tlvtype->_current_tlv_list, tlv_first, _current_tlv_node);

  /* write tlv */
  *ptr++ = tlvtype->type;

  /* remember flag pointer */
  flag = ptr;
  *ptr++ = 0;
  if (tlvtype->exttype) {
    *flag |= RFC5444_TLV_FLAG_TYPEEXT;
    *ptr++ = tlvtype->exttype;
  }

  /* copy original length field */
  total_len = tlv_first->length;

  if (tlv_first->address == addr_first && tlv_last->address == addr_last) {
    /* no index necessary */
  }
  else if (tlv_first->address->index == tlv_last->address->index) {
    *flag |= RFC5444_TLV_FLAG_SINGLE_IDX;
    *ptr++ = tlv_first->address->index - addr_first->index;
  }
  else {
    *flag |= RFC5444_TLV_FLAG_MULTI_IDX;
    *ptr++ = tlv_first->address->index - addr_first->index;
    *ptr++ = tlv_last->address->index - addr_first->index;
  }

  /* length field is single_length*num for multivalue tlvs */
  if (!tlvtype->_same_value) {
    total_len = total_len * ((tlv_last->address->index - tlv_first->address->index) + 1);
    *flag |= RFC5444_TLV_FLAG_MULTIVALUE;
  }

  /* write length field and corresponding flags */
  if (total_len > 255) {
    *flag |= RFC5444_TLV_FLAG_EXTVALUE;
    *ptr++ = total_len >> 8;
  }
  if (total_len > 0) {
    *flag |= RFC5444_TLV_FLAG_VALUE;
    *ptr++ = total_len & 255;
  }

  if (tlv_first->length > 0) {
    /* write value */
    if (tlvtype->_same_value) {
      memcpy(ptr, tlv_first->value, tlv_first->length);
      ptr += tlv_first->length;
    }
    else {
      oonf_list_for_each_element(&tlvtype->_current_tlv_list, tlv, _current_tlv_node) {
        memcpy(ptr, tlv->value, tlv->length);
        ptr += tlv->length;
      }
    }
  }
  return ptr;
}

/**
 * Write the address-TLVs of a specific type
 * @param writer RFC5444 writer instance
 * @param msg RFC5444 message to be generated
 * @param first first address for TLVs
 * @param last last address for TLVs
 * @param ptr target buffer pointer
 * @return modified target buffer pointer
 */
static uint8_t *
_write_addresstlvs(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg,
  struct rfc5444_writer_address *first, struct rfc5444_writer_address *last, uint8_t *ptr) {
  struct rfc5444_writer_tlvtype *tlvtype;
  struct rfc5444_writer_address *addr, *prev_addr;
  struct rfc5444_writer_addrtlv *tlv, *last_tlv;
  struct oonf_list_entity active_tlvtypes;

  /* clear markers for tlv blocks */
  oonf_list_for_each_element(&msg->_msgspecific_tlvtype_head, tlvtype, _tlvtype_node) {
    oonf_list_init_head(&tlvtype->_current_tlv_list);
  }
  oonf_list_for_each_element(&writer->_addr_tlvtype_head, tlvtype, _tlvtype_node) {
    oonf_list_init_head(&tlvtype->_current_tlv_list);
  }

  oonf_list_init_head(&active_tlvtypes);

  /* loop over all addresses */
  prev_addr = NULL;
  oonf_list_for_element_range(first, last, addr, _addr_fragment_node) {
    avl_for_each_element(&addr->_addrtlv_tree, tlv, addrtlv_node) {
      tlvtype = tlv->tlvtype;

      if (!oonf_list_is_empty(&tlvtype->_current_tlv_list)) {
        last_tlv = oonf_list_last_element(&tlvtype->_current_tlv_list, last_tlv, _current_tlv_node);

        if (last_tlv->address != prev_addr || last_tlv->length != tlv->length) {
          /* we need to end this TLV and start a new one */
          ptr = _write_addresstlv(tlvtype, first, last, ptr);

          /* clear list of tlvs of this type */
          oonf_list_init_head(&tlvtype->_current_tlv_list);
          tlvtype->_same_value = true;
        }
        else {
          /* keep track if the value stays the same */
          tlvtype->_same_value &= tlv->_same_value;
        }
      }
      else {
        /* this tlvtype is now active */
        oonf_list_add_tail(&active_tlvtypes, &tlvtype->_active_tlv_node);
        tlvtype->_same_value = true;
      }

      /* add current tlv to list */
      oonf_list_add_tail(&tlvtype->_current_tlv_list, &tlv->_current_tlv_node);
    }

    /* remember last address for next loop iteration */
    prev_addr = addr;
  }

  oonf_list_for_each_element(&active_tlvtypes, tlvtype, _active_tlv_node) {
    ptr = _write_addresstlv(tlvtype, first, last, ptr);
  }
  return ptr;
}

/**
 * Write the address blocks to the message buffer.
 * @param writer pointer to writer context
 * @param msg pointer to message context
 * @param fragment_addrs list of fragmented addresses
 */
static void
_write_addresses(
  struct rfc5444_writer *writer, struct rfc5444_writer_message *msg, struct oonf_list_entity *fragment_addrs) {
  struct rfc5444_writer_address *addr_start, *addr_end, *addr_first, *addr_last, *addr;
  const uint8_t *addr_start_ptr, *addr_ptr;
  uint8_t *start, *ptr, *flag, *tlvblock_length;
  uint8_t head_len, tail_len, mid_len;
  bool zero_tail;
#ifdef DEBUG_OUTPUT
  uint8_t *debug_ptr;
#endif

  addr_first = oonf_list_first_element(fragment_addrs, addr_first, _addr_fragment_node);
  addr_last = oonf_list_last_element(fragment_addrs, addr_last, _addr_fragment_node);

  assert(addr_first->_block_end);

  addr_start = addr_first;
  ptr = &writer->_msg.buffer[writer->_msg.header + writer->_msg.added + writer->_msg.allocated];

  /* remember start */
  start = ptr;

#ifdef DEBUG_OUTPUT
  debug_ptr = ptr;
#endif

  /* loop through address blocks */
  do {
    head_len = 0;
    tail_len = 0;
    mid_len = 0;
    zero_tail = false;

    addr_start_ptr = netaddr_get_binptr(&addr_start->address);
    addr_end = addr_start->_block_end;

#ifdef DEBUG_OUTPUT
    {
      struct netaddr_str nbuf1, nbuf2;
      printf("Write address block from %s to %s\n", netaddr_to_string(&nbuf1, &addr_start->address),
        netaddr_to_string(&nbuf2, &addr_end->address));
    }
#endif
    if (addr_start != addr_end) {
      /* only use head/tail for address blocks with multiple addresses */
      uint8_t tail;
      head_len = addr_start->_block_headlen;
      tail_len = writer->msg_addr_len - head_len - 1;

      /* calculate tail length and netmask length */
      oonf_list_for_element_range(addr_start, addr_end, addr, _addr_fragment_node) {
        addr_ptr = netaddr_get_binptr(&addr->address);

        /* stop if no tail is left */
        if (tail_len == 0) {
          break;
        }

        for (tail = 1; tail <= tail_len; tail++) {
          if (addr_start_ptr[writer->msg_addr_len - tail] != addr_ptr[writer->msg_addr_len - tail]) {
            tail_len = tail - 1;
            break;
          }
        }
      }

      zero_tail = tail_len > 0;
      for (tail = 0; zero_tail && tail < tail_len; tail++) {
        if (addr_start_ptr[writer->msg_addr_len - tail - 1] != 0) {
          zero_tail = false;
        }
      }
    }

    mid_len = writer->msg_addr_len - head_len - tail_len;

    /* write addrblock header */
    *ptr++ = addr_end->index - addr_start->index + 1;

    /* erase flag */
    flag = ptr;
    *ptr++ = 0;

    /* write head */
    if (head_len) {
      *flag |= RFC5444_ADDR_FLAG_HEAD;
      *ptr++ = head_len;
      memcpy(ptr, addr_start_ptr, head_len);
      ptr += head_len;
    }

    /* write tail */
    if (tail_len > 0) {
      *ptr++ = tail_len;
      if (zero_tail) {
        *flag |= RFC5444_ADDR_FLAG_ZEROTAIL;
      }
      else {
        *flag |= RFC5444_ADDR_FLAG_FULLTAIL;
        memcpy(ptr, &addr_start_ptr[writer->msg_addr_len - tail_len], tail_len);
        ptr += tail_len;
      }
    }

    /* loop through addresses in block for MID part */
    oonf_list_for_element_range(addr_start, addr_end, addr, _addr_fragment_node) {
      addr_ptr = netaddr_get_binptr(&addr->address);
      memcpy(ptr, &addr_ptr[head_len], mid_len);
      ptr += mid_len;
    }

    /* loop through addresses in block for prefixlen part */
    if (addr_start->_block_multiple_prefixlen) {
      /* multiple prefixlen */
      *flag |= RFC5444_ADDR_FLAG_MULTIPLEN;
      oonf_list_for_element_range(addr_start, addr_end, addr, _addr_fragment_node) {
        *ptr++ = netaddr_get_prefix_length(&addr->address);
      }
    }
    else if (netaddr_get_prefix_length(&addr_start->address) != writer->msg_addr_len * 8) {
      /* single prefixlen */
      *flag |= RFC5444_ADDR_FLAG_SINGLEPLEN;
      *ptr++ = netaddr_get_prefix_length(&addr_start->address);
    }

#ifdef DEBUG_OUTPUT
    printf("Generated %zd bytes for address block\n", ptr - debug_ptr);
    debug_ptr = ptr;
#endif

    /* remember pointer for tlvblock length */
    tlvblock_length = ptr;
    ptr += 2;

    /* generate tlvs */
    ptr = _write_addresstlvs(writer, msg, addr_start, addr_end, ptr);

    tlvblock_length[0] = (ptr - tlvblock_length - 2) >> 8;
    tlvblock_length[1] = (ptr - tlvblock_length - 2) & 255;
    addr_start = oonf_list_next_element(addr_end, _addr_fragment_node);
#ifdef DEBUG_OUTPUT
    printf("Generated %zd bytes for address tlvs\n", ptr - debug_ptr);
    debug_ptr = ptr;
#endif
  } while (addr_end != addr_last);

  /* store size of address(tlv) data */
  msg->_bin_addr_size = ptr - start;
}

/**
 * Write header of message including mandatory tlvblock length field.
 * @param writer pointer to writer context
 * @param msg pointer to message object
 */
static void
_write_msgheader(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg) {
  uint8_t *ptr, *flags;
  uint16_t total_size;
  ptr = writer->_msg.buffer;

  /* type */
  *ptr++ = msg->type;

  /* flags & addrlen */
  flags = ptr;
  *ptr++ = writer->msg_addr_len - 1;

  /* size */
  total_size = writer->_msg.header + writer->_msg.added + writer->_msg.set + msg->_bin_addr_size;
  *ptr++ = total_size >> 8;
  *ptr++ = total_size & 255;

  if (msg->has_origaddr) {
    *flags |= RFC5444_MSG_FLAG_ORIGINATOR;
    memcpy(ptr, msg->orig_addr, writer->msg_addr_len);
    ptr += writer->msg_addr_len;
  }
  if (msg->has_hoplimit) {
    *flags |= RFC5444_MSG_FLAG_HOPLIMIT;
    *ptr++ = msg->hoplimit;
  }
  if (msg->has_hopcount) {
    *flags |= RFC5444_MSG_FLAG_HOPCOUNT;
    *ptr++ = msg->hopcount;
  }
  if (msg->has_seqno) {
    *flags |= RFC5444_MSG_FLAG_SEQNO;
    *ptr++ = msg->seqno >> 8;
    *ptr++ = msg->seqno & 255;
  }

  /* write tlv-block size */
  total_size = writer->_msg.added + writer->_msg.set;
  *ptr++ = total_size >> 8;
  *ptr++ = total_size & 255;
}

/**
 * Finalize a message fragment, copy it into the packet buffer and
 * cleanup message internal data.
 * @param writer pointer to writer context
 * @param msg pointer to message object
 * @param fragment_addrs list of fragmented addresses
 * @param not_fragmented true if this is the only fragment of this message
 * @param useIf pointer to callback for selecting outgoing _targets
 * @param param custom parameter for callback
 */
static void
_finalize_message_fragment(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg,
  struct oonf_list_entity *fragment_addrs, bool not_fragmented, rfc5444_writer_targetselector useIf, void *param) {
  struct rfc5444_writer_postprocessor *processor;
  struct rfc5444_writer_content_provider *prv;
  struct rfc5444_writer_target *target;
  struct rfc5444_writer_address *addr, *first, *last;
  uint8_t *ptr;
  size_t msg_minsize, msg_size, generic_size;
  bool error;

  /* reset optional tlv length */
  writer->_msg.set = 0;

#if WRITER_STATE_MACHINE == true
  writer->_state = RFC5444_WRITER_FINISH_MSGTLV;
#endif

  if (!oonf_list_is_empty(fragment_addrs)) {
    first = oonf_list_first_element(fragment_addrs, first, _addr_fragment_node);
    last = oonf_list_last_element(fragment_addrs, last, _addr_fragment_node);
  }
  else {
    first = last = NULL;
  }

  /* inform message providers */
  avl_for_each_element_reverse(&msg->_provider_tree, prv, _provider_node) {
    if (prv->finishMessageTLVs) {
      prv->finishMessageTLVs(writer, first, last, not_fragmented);
    }
  }

  if (!oonf_list_is_empty(fragment_addrs)) {
    addr = last;

    /* generate forward pointers */
    while (true) {
      addr->_block_start->_block_end = addr;
      addr->_block_start->_block_multiple_prefixlen = addr->_block_multiple_prefixlen;
      addr->_block_start->_block_headlen = addr->_block_headlen;
      if (addr->_block_start == first) {
        break;
      }

      /* consistency check */
      assert(addr->_block_start->index > first->index);

      /* get address block before this one */
      addr = oonf_list_prev_element(addr->_block_start, _addr_fragment_node);
    }
    _write_addresses(writer, msg, fragment_addrs);
  }

#if WRITER_STATE_MACHINE == true
  writer->_state = RFC5444_WRITER_FINISH_HEADER;
#endif

  /* inform message creator */
  if (msg->finishMessageHeader) {
    msg->finishMessageHeader(writer, msg, first, last, not_fragmented);
  }

  /* write header */
  _write_msgheader(writer, msg);

#if WRITER_STATE_MACHINE == true
  writer->_state = RFC5444_WRITER_NONE;
#endif

  /* precalculate number of fixed bytes of message header */
  msg_minsize = writer->_msg.header + writer->_msg.added;

  /* 1.) first flush all interfaces that have full buffers */
  oonf_list_for_each_element(&writer->_targets, target, _target_node) {
    /* do we need to handle this interface ? */
    if (!useIf(writer, target, param)) {
      continue;
    }

    /* calculate total size of packet and message, see if it fits into the current packet */
    if (target->_pkt.header + target->_pkt.added + target->_pkt.set + target->_bin_msgs_size + writer->_msg.header +
          writer->_msg.added + writer->_msg.set + msg->_bin_addr_size >
        target->_pkt.max) {
      /* flush the old packet */
      rfc5444_writer_flush(writer, target, false);

      /* begin a new one */
      _rfc5444_writer_begin_packet(writer, target);
    }
  }

  /* 2.) generate message and do non-target specific post processors */

  /* copy message header and message tlvs into packet buffer */
  ptr = _msg_buffer;
  memcpy(ptr, writer->_msg.buffer, msg_minsize + writer->_msg.set);

  /* copy address blocks and address tlvs into packet buffer */
  ptr += msg_minsize + writer->_msg.set;
  memcpy(ptr, &writer->_msg.buffer[msg_minsize + writer->_msg.allocated], msg->_bin_addr_size);

  /* remember position of first copy */
  generic_size = msg_minsize + writer->_msg.set + msg->_bin_addr_size;

  /* run processors */
  avl_for_each_element(&writer->_processors, processor, _node) {
    if (processor->is_matching_signature(processor, msg->type) && !processor->target_specific) {
      if (processor->process(processor, target, msg, _msg_buffer, &generic_size)) {
        /* error, we have not modified the _bin_msgs_size, so we can just return */
        return;
      }
    }
  }

  /* 3.) generate target specific messages and add them to the buffers */
  oonf_list_for_each_element(&writer->_targets, target, _target_node) {
    /* do we need to handle this interface ? */
    if (!useIf(writer, target, param)) {
      continue;
    }

    /* get pointer to end of _pkt buffer  and copy it */
    ptr =
      &target->_pkt.buffer[target->_pkt.header + target->_pkt.added + target->_pkt.allocated + target->_bin_msgs_size];
    memcpy(ptr, _msg_buffer, generic_size);

    /* now run the target specific processors */
    msg_size = generic_size;
    error = false;
    avl_for_each_element(&writer->_processors, processor, _node) {
      if (processor->is_matching_signature(processor, msg->type) && processor->target_specific && msg_size > 0) {
        if (processor->process(processor, target, msg, ptr, &msg_size)) {
          error = true;
          break;
        }
      }
    }

    if (!error && msg_size > 0) {
      /* increase byte count of packet */
      target->_bin_msgs_size += msg_size;

      if (writer->message_generation_notifier) {
        writer->message_generation_notifier(target);
      }
    }
  }

  /* clear length value of message address size */
  msg->_bin_addr_size = 0;

  /* reset message tlv variables */
  writer->_msg.set = 0;

  /* clear message buffer */
#if DEBUG_CLEANUP == true
  memset(&writer->_msg.buffer[msg_minsize], 253, writer->_msg.max - msg_minsize);
#endif
}
