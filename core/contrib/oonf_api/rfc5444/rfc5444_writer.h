
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

#ifndef RFC5444_WRITER_H_
#define RFC5444_WRITER_H_

struct rfc5444_writer;
struct rfc5444_writer_message;

#include "common/avl.h"
#include "common/common_types.h"
#include "common/list.h"
#include "common/netaddr.h"
#include "rfc5444_context.h"
#include "rfc5444_reader.h"
#include "rfc5444_tlv_writer.h"

/**
 * state machine values for the writer.
 * If compiled with WRITE_STATE_MACHINE, this can check if the functions
 * of the writer are called from the right context.
 */
enum rfc5444_internal_state
{
  RFC5444_WRITER_NONE,
  RFC5444_WRITER_ADD_PKTHEADER,
  RFC5444_WRITER_ADD_PKTTLV,
  RFC5444_WRITER_ADD_HEADER,
  RFC5444_WRITER_ADD_MSGTLV,
  RFC5444_WRITER_ADD_ADDRESSES,
  RFC5444_WRITER_FINISH_MSGTLV,
  RFC5444_WRITER_FINISH_HEADER,
  RFC5444_WRITER_FINISH_PKTTLV,
  RFC5444_WRITER_FINISH_PKTHEADER
};

enum
{
  /*! Maximum packet size for this RFC5444 multiplexer */
  RFC5444_MAX_PACKET_SIZE = 1500 - 20 - 8,

  /**
   * Maximum message size for this RFC5444 multiplexer
   * (minimal ipv6 mtu - ipv6/udp/rfc5444packet/vlan-header)
   */
  RFC5444_MAX_MESSAGE_SIZE = 1280 - 40 - 8 - 3 - 4,

  /*! msg_type id for packet post-processor */
  RFC5444_WRITER_PKT_POSTPROCESSOR = -1,
};

/**
 * This INTERNAL struct represents a single address tlv
 * of an address during message serialization.
 */
struct rfc5444_writer_addrtlv {
  /*! backpointer to tlvtype */
  struct rfc5444_writer_tlvtype *tlvtype;

  /*! tree _target_node of tlvs used by a single address */
  struct avl_node addrtlv_node;

  /*! backpointer to address */
  struct rfc5444_writer_address *address;

  /* tlv type and extension is stored in writer_tlvtype */

  /*! tlv value length */
  uint16_t length;

  /**
   * if multiple tlvs with the same type/ext have the same
   * value for a continuous block of addresses, they should
   * use the same storage for the value (the pointer should
   * be the same)
   */
  void *value;

  /*! addresstlv has same value than for last address */
  bool _same_value;

  /*! addresstlv has same length than for last address */
  bool _same_length;

  /*! hook into current list of nodes */
  struct oonf_list_entity _current_tlv_node;
};

/**
 * This struct represents a single address during the RFC5444
 * message creation.
 */
struct rfc5444_writer_address {
  /*! index of the address */
  uint32_t index;

  /*! address/prefix */
  struct netaddr address;

  /*! node of address list in writer_message */
  struct oonf_list_entity _addr_oonf_list_node;

  /*! node of address list for current fragment */
  struct oonf_list_entity _addr_fragment_node;

  /*! node for quick access ( O(log n)) to addresses */
  struct avl_node _addr_tree_node;

  /*! tree to connect all TLVs of this address */
  struct avl_tree _addrtlv_tree;

  /*! address block head length */
  uint8_t _block_headlen;

  /*! true if addresses have multiple prefix lengths */
  bool _block_multiple_prefixlen;

  /*! pointer to end of the block, NULL if this is not the start address */
  struct rfc5444_writer_address *_block_end;

  /*! pointer to start of the block, NULL if this is not the end address */
  struct rfc5444_writer_address *_block_start;

  /*! true if address is mandatory in all fragments */
  bool _mandatory_addr;

  /*! true if address has already been handled in earlier fragment */
  bool _done;
};

/**
 * This INTERNAL struct is preallocated for each tlvtype that can be added
 * to an address of a certain message type.
 */
struct rfc5444_writer_tlvtype {
  /*! tlv type and extension is stored in writer_tlvtype */
  uint8_t type;

  /*! tlv extension type */
  uint8_t exttype;

  /*! node of tlvtype list in rfc5444_writer_message */
  struct oonf_list_entity _tlvtype_node;

  /*! back pointer to message creator */
  struct rfc5444_writer_message *_creator;

  /*! tlv type*256 + tlv_exttype */
  int _full_type;

  /*! number of TLVs already compressed for specific head length */
  int _tlvblock_count[RFC5444_MAX_ADDRLEN];

  /*! true if tlv has multiple values for specific head length */
  bool _tlvblock_multi[RFC5444_MAX_ADDRLEN];

  /*! true if tlv has same value */
  bool _same_value;

  /*! list of active nodes for tlv */
  struct oonf_list_entity _active_tlv_node;

  /*! list of tlvs for current fragment */
  struct oonf_list_entity _current_tlv_list;
};

/**
 * This struct represents a single content provider of
 * tlvs for a message context.
 */
struct rfc5444_writer_content_provider {
  /*! priority of content provider */
  int32_t priority;

  /*! message type for this content provider */
  uint8_t msg_type;

  /**
   * Callback to add message tlvs to message
   * @param writer rfc5444 writer
   */
  void (*addMessageTLVs)(struct rfc5444_writer *writer);

  /**
   * Callback to add addresses with TLVs to message
   * @param writer rfc5444 writer
   */
  void (*addAddresses)(struct rfc5444_writer *writer);

  /**
   * Callback to notify that all addresses have been written
   * and user can now set the earlier allocated message tlvs.
   * This is called once per message fragment
   * @param writer rfc5444 writer
   * @param start first address of fragment
   * @param end last address of fragment
   * @param complete false if message has been fragmented,
   *    true if message fit into MTU
   */
  void (*finishMessageTLVs)(struct rfc5444_writer *writer, struct rfc5444_writer_address *start,
    struct rfc5444_writer_address *end, bool complete);

  /*! node for tree of content providers for a message creator */
  struct avl_node _provider_node;

  /*! back pointer to message creator */
  struct rfc5444_writer_message *creator;
};

/**
 * This struct represents a single target (IP) for
 * the rfc5444 writer
 */
struct rfc5444_writer_target {
  /*! buffer for packet generation */
  uint8_t *packet_buffer;

  /*! maximum number of bytes per packets allowed for target */
  size_t packet_size;

  /**
   * Callback to set RFC5444 packet header
   * @param writer rfc5444 writer
   * @param target rfc5444 target
   */
  void (*addPacketHeader)(struct rfc5444_writer *writer, struct rfc5444_writer_target *target);

  /**
   * Callback to notify that all messages have been added and
   * user can now modify the packet header again
   * @param writer rfc5444 writer
   * @param target rfc5444 target
   */
  void (*finishPacketHeader)(struct rfc5444_writer *writer, struct rfc5444_writer_target *target);

  /**
   * Callback to send a RFC5444 packet to a socket or destination
   * @param writer rfc5444 writer
   * @param target rfc5444 target
   * @param ptr pointer to packet data
   * @param len length of packet
   */
  void (*sendPacket)(struct rfc5444_writer *writer, struct rfc5444_writer_target *target, void *ptr, size_t len);

  /*! true if packet should have a sequence number */
  bool _has_seqno;

  /*! last used packet sequence number */
  uint16_t _seqno;

  /*! node for list of all targets*/
  struct oonf_list_entity _target_node;

  /*! packet buffer is currently flushed */
  bool _is_flushed;

  /*! buffer for constructing the current packet */
  struct rfc5444_tlv_writer_data _pkt;

  /*! number of bytes used by messages */
  size_t _bin_msgs_size;
};

/**
 * This struct is allocated for each message type that can
 * be generated by the writer.
 */
struct rfc5444_writer_message {
  /*! target node for tree of message creators */
  struct avl_node _msgcreator_node;

  /*! tree of message content providers */
  struct avl_tree _provider_tree;

  /**
   * true if the creator has already registered
   * false if the creator was registered because of a tlvtype or content
   * provider registration
   */
  bool _registered;

  /*! true if a different message must be generated for each target */
  bool target_specific;

  /*! message type */
  uint8_t type;

  /*! true if message should have a hopcount */
  bool has_hopcount;

  /*! message hopcount */
  uint8_t hopcount;

  /*! true if message should have a hoplimit */
  bool has_hoplimit;

  /*! message hoplimit */
  uint8_t hoplimit;

  /*! true if message should have an originator */
  bool has_origaddr;

  /*! message originator */
  uint8_t orig_addr[RFC5444_MAX_ADDRLEN];

  /*! true if message should have a sequence number */
  bool has_seqno;

  /*! message sequence number */
  uint16_t seqno;

  /*! list of non-mandatory writer_addresses */
  struct oonf_list_entity _non_mandatory_addr_head;

  /*! list of all writer_addresses */
  struct oonf_list_entity _addr_head;

  /*! tree of writer addresses */
  struct avl_tree _addr_tree;

  /*! head of message specific tlvtype list */
  struct oonf_list_entity _msgspecific_tlvtype_head;

  /**
   * Callback to set message header
   * @param writer rfc5444 writer
   * @param msg rfc5444 message
   * @return -1 if an error happened, 0 otherwise
   */
  int (*addMessageHeader)(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg);

  /**
   * Callback to notify that all addresses have been written
   * and user can now modify the message header.
   * This is called once per message fragment
   * @param writer rfc5444 writer
   * @param msg rfc5444 message
   * @param start first address of fragment
   * @param end last address of fragment
   * @param complete false if message has been fragmented,
   *    true if message fit into MTU
   */
  void (*finishMessageHeader)(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg,
    struct rfc5444_writer_address *first, struct rfc5444_writer_address *last, bool complete);

  /**
   * callback to determine if a message shall be forwarded
   * @param target rfc5444 target
   * @param context rfc5444 message context
   * @return true if message should be forwarded, false otherwise
   */
  bool (*forward_target_selector)(
    struct rfc5444_writer_target *target, struct rfc5444_reader_tlvblock_context *context);

  /*! number of bytes necessary for addressblocks including tlvs */
  size_t _bin_addr_size;

  /*! custom user data */
  void *user;
};

/**
 * This struct represents a content provider for adding
 * tlvs to a packet header.
 */
struct rfc5444_writer_pkthandler {
  /*! node for list of packet handlers */
  struct oonf_list_entity _pkthandle_node;

  /**
   * Callback to add packet TLVs to packet
   * @param writer rfc5444 packet
   * @param target rfc5444 target
   */
  void (*addPacketTLVs)(struct rfc5444_writer *writer, struct rfc5444_writer_target *target);

  /**
   * Callback to notify that all messages have been added and
   * user can now set the earlier allocated packet TLVs.
   * @param writer rfc5444 writer
   * @param target rfc5444 target
   */
  void (*finishPacketTLVs)(struct rfc5444_writer *writer, struct rfc5444_writer_target *target);
};

/**
 * a post-processor for rfc5444 messages/packets
 */
struct rfc5444_writer_postprocessor {
  /*! node for treee of message post processors */
  struct avl_node _node;

  /*! order of message post-processors */
  int32_t priority;

  /*! number of bytes allocated for post-processor */
  uint16_t allocate_space;

  /**
   * true if post-processing must be done per target,
   * unused for packet post-processors
   */
  bool target_specific;

  /**
   * checks if post-processor applies to a message/packet
   * @param processor this post-processor
   * @param msg_type rfc5444 message type, -1 for packet signature
   * @return true if signature applies to message type, false otherwise
   */
  bool (*is_matching_signature)(struct rfc5444_writer_postprocessor *processor, int msg_type);

  /**
   * Process binary data in post-processor
   * @param processor rfc5444 post-processor
   * @param target rfc5444 target
   * @param msg rfc5444 message, NULL for packet signature
   * @param data pointer to binary data
   * @param length pointer to length of binary data, can be overwritten by function
   * @return -1 if an error happened, 0 otherwise
   */
  int (*process)(struct rfc5444_writer_postprocessor *processor, struct rfc5444_writer_target *target,
    struct rfc5444_writer_message *msg, uint8_t *data, size_t *length);
};

/**
 * a post-processor for forwarded rfc5444 messages
 */
struct rfc5444_writer_forward_handler {
  /*! node for treee of message post processors */
  struct avl_node _node;

  /*! order of message post-processors */
  int32_t priority;

  /*! number of bytes allocated for post-processor */
  uint16_t allocate_space;

  /**
   * true if post-processing must be done per target,
   */
  bool target_specific;

  /**
   * checks if post-processor applies to a forwarded message
   * @param handler this post-processor
   * @param msg_type rfc5444 message type
   * @return true if signature applies to message type, false otherwise
   */
  bool (*is_matching_signature)(struct rfc5444_writer_forward_handler *handler, uint8_t msg_type);

  /**
   * Process binary data in post-processor
   * @param handler rfc5444 forwarding post-processor
   * @param target rfc5444 target
   * @param context RFC5444 message context
   * @param data pointer to binary data
   * @param length pointer to length of binary data, can be overwritten by function
   * @return -1 if an error happened, 0 otherwise
   */
  int (*process)(struct rfc5444_writer_forward_handler *handler, struct rfc5444_writer_target *target,
    struct rfc5444_reader_tlvblock_context *context, uint8_t *data, size_t *length);
};

/**
 * This struct represents the internal state of a
 * rfc5444 writer.
 */
struct rfc5444_writer {
  /*! buffer for messages */
  uint8_t *msg_buffer;

  /*! address length of current message */
  uint8_t msg_addr_len;

  /*! length of message buffer */
  size_t msg_size;

  /*! buffer for addrtlv values of a message */
  uint8_t *addrtlv_buffer;

  /*! length of addrtlv buffer */
  size_t addrtlv_size;

  /**
   * Callback to notify an instance that a message was put into a
   * target buffer
   * @param target pointer to rfc5444 target where
   *   the message has been placed
   */
  void (*message_generation_notifier)(struct rfc5444_writer_target *target);

  /**
   * Callback to allocate a writer_address, NULL for use calloc()
   * @return writer address, NULL if out of memory
   */
  struct rfc5444_writer_address *(*malloc_address_entry)(void);

  /**
   * Callback to allocate an address tlv, NULL for use calloc()
   * @return address tlv, NULL if out of memory
   */
  struct rfc5444_writer_addrtlv *(*malloc_addrtlv_entry)(void);

  /**
   * Callback to free a writer_address, NULL for use free()
   * @param addr writer address
   */
  void (*free_address_entry)(struct rfc5444_writer_address *addr);

  /**
   * Callback to free a address tlv, NULL for use free()
   * @param addrtlv address tlv
   */
  void (*free_addrtlv_entry)(struct rfc5444_writer_addrtlv *addrtlv);

  /**
   * target of current generated message
   * only used for target specific message types
   */
  struct rfc5444_writer_target *msg_target;

  /*! tree of all message handlers */
  struct avl_tree _msgcreators;

  /*! list of all packet handlers */
  struct oonf_list_entity _pkthandlers;

  /*! tree of packet post-processors */
  struct avl_tree _processors;

  /*! tree of forwarding post-processors */
  struct avl_tree _forwarding_processors;

  /*! list of all targets */
  struct oonf_list_entity _targets;

  /*! list of generic tlvtypes */
  struct oonf_list_entity _addr_tlvtype_head;

  /*! buffer for constructing the current message */
  struct rfc5444_tlv_writer_data _msg;

  /*! number of bytes of addrtlv buffer currently used */
  size_t _addrtlv_used;

  /*! internal state of writer */
  enum rfc5444_internal_state _state;
};

/* functions that can be called from addAddress callback */
EXPORT struct rfc5444_writer_address *rfc5444_writer_add_address(
  struct rfc5444_writer *writer, struct rfc5444_writer_message *msg, const struct netaddr *, bool mandatory);
EXPORT enum rfc5444_result rfc5444_writer_add_addrtlv(struct rfc5444_writer *writer,
  struct rfc5444_writer_address *addr, struct rfc5444_writer_tlvtype *tlvtype, const void *value, size_t length,
  bool allow_dup);

/* functions that can be called from add/finishMessageTLVs callback */
EXPORT enum rfc5444_result rfc5444_writer_add_messagetlv(
  struct rfc5444_writer *writer, uint8_t type, uint8_t exttype, const void *value, size_t length);
EXPORT enum rfc5444_result rfc5444_writer_allocate_messagetlv(
  struct rfc5444_writer *writer, bool has_exttype, size_t length);
EXPORT enum rfc5444_result rfc5444_writer_set_messagetlv(
  struct rfc5444_writer *writer, uint8_t type, uint8_t exttype, const void *value, size_t length);

/* functions that can be called from add/finishMessageHeader callback */
EXPORT void rfc5444_writer_set_msg_header(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg,
  bool has_originator, bool has_hopcount, bool has_hoplimit, bool has_seqno);
EXPORT void rfc5444_writer_set_msg_originator(
  struct rfc5444_writer *writer, struct rfc5444_writer_message *msg, const void *originator);
EXPORT void rfc5444_writer_set_msg_hopcount(
  struct rfc5444_writer *writer, struct rfc5444_writer_message *msg, uint8_t hopcount);
EXPORT void rfc5444_writer_set_msg_hoplimit(
  struct rfc5444_writer *writer, struct rfc5444_writer_message *msg, uint8_t hoplimit);
EXPORT void rfc5444_writer_set_msg_seqno(
  struct rfc5444_writer *writer, struct rfc5444_writer_message *msg, uint16_t seqno);

/* functions that can be called from add/finishPacketTLVs callback */
EXPORT enum rfc5444_result rfc5444_writer_add_packettlv(struct rfc5444_writer *writer,
  struct rfc5444_writer_target *target, uint8_t type, uint8_t exttype, void *value, size_t length);
EXPORT enum rfc5444_result rfc5444_writer_allocate_packettlv(
  struct rfc5444_writer *writer, struct rfc5444_writer_target *target, bool has_exttype, size_t length);
EXPORT enum rfc5444_result rfc5444_writer_set_packettlv(struct rfc5444_writer *writer,
  struct rfc5444_writer_target *target, uint8_t type, uint8_t exttype, void *value, size_t length);

/* functions that can be called from add/finishPacketHeader */
EXPORT void rfc5444_writer_set_pkt_header(
  struct rfc5444_writer *writer, struct rfc5444_writer_target *target, bool has_seqno);
EXPORT void rfc5444_writer_set_pkt_seqno(
  struct rfc5444_writer *writer, struct rfc5444_writer_target *target, uint16_t seqno);

/* functions that can be called outside the callbacks */
EXPORT int rfc5444_writer_register_addrtlvtype(
  struct rfc5444_writer *writer, struct rfc5444_writer_tlvtype *type, int msgtype);
EXPORT void rfc5444_writer_unregister_addrtlvtype(
  struct rfc5444_writer *writer, struct rfc5444_writer_tlvtype *tlvtype);

EXPORT int rfc5444_writer_register_msgcontentprovider(struct rfc5444_writer *writer,
  struct rfc5444_writer_content_provider *cpr, struct rfc5444_writer_tlvtype *addrtlvs, size_t addrtlv_count);
EXPORT void rfc5444_writer_unregister_content_provider(struct rfc5444_writer *writer,
  struct rfc5444_writer_content_provider *cpr, struct rfc5444_writer_tlvtype *addrtlvs, size_t addrtlv_count);

EXPORT void rfc5444_writer_register_postprocessor(
  struct rfc5444_writer *writer, struct rfc5444_writer_postprocessor *processor);
EXPORT void rfc5444_writer_unregister_postprocessor(
  struct rfc5444_writer *, struct rfc5444_writer_postprocessor *processor);

EXPORT void rfc5444_writer_register_forward_handler(
  struct rfc5444_writer *writer, struct rfc5444_writer_forward_handler *processor);
EXPORT void rfc5444_writer_unregister_forward_handler(
  struct rfc5444_writer *, struct rfc5444_writer_forward_handler *processor);

EXPORT struct rfc5444_writer_message *rfc5444_writer_register_message(
  struct rfc5444_writer *writer, uint8_t msgid, bool if_specific);
EXPORT void rfc5444_writer_unregister_message(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg);

EXPORT void rfc5444_writer_register_pkthandler(struct rfc5444_writer *writer, struct rfc5444_writer_pkthandler *pkt);
EXPORT void rfc5444_writer_unregister_pkthandler(struct rfc5444_writer *writer, struct rfc5444_writer_pkthandler *pkt);

EXPORT void rfc5444_writer_register_target(struct rfc5444_writer *writer, struct rfc5444_writer_target *target);
EXPORT void rfc5444_writer_unregister_target(struct rfc5444_writer *writer, struct rfc5444_writer_target *target);

/**
 * prototype for message creation target filter
 * @param writer rfc5444 writer
 * @param target rfc5444 target
 * @param custom custom data pointer
 * @return true if target is selected
 */
typedef bool (*rfc5444_writer_targetselector)(
  struct rfc5444_writer *writer, struct rfc5444_writer_target *target, void *custom);

EXPORT bool rfc5444_writer_singletarget_selector(struct rfc5444_writer *, struct rfc5444_writer_target *, void *);
EXPORT bool rfc5444_writer_alltargets_selector(struct rfc5444_writer *, struct rfc5444_writer_target *, void *);

EXPORT enum rfc5444_result rfc5444_writer_create_message(
  struct rfc5444_writer *writer, uint8_t msgid, uint8_t addr_len, rfc5444_writer_targetselector useIf, void *param);

EXPORT enum rfc5444_result rfc5444_writer_forward_msg(
  struct rfc5444_writer *writer, struct rfc5444_reader_tlvblock_context *context, const uint8_t *msg, size_t len);

EXPORT void rfc5444_writer_flush(struct rfc5444_writer *, struct rfc5444_writer_target *, bool);

EXPORT void rfc5444_writer_init(struct rfc5444_writer *);
EXPORT void rfc5444_writer_cleanup(struct rfc5444_writer *writer);

/* internal functions that are not exported to the user */
void _rfc5444_writer_free_addresses(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg);
void _rfc5444_writer_begin_packet(struct rfc5444_writer *writer, struct rfc5444_writer_target *target);

/**
 * creates a message of a certain ID for a single target
 * @param writer pointer to writer context
 * @param msgid type of message
 * @param addr_len length of address for this message
 * @param target pointer to outgoing target
 * @return RFC5444_OKAY if message was created and added to packet buffer,
 *   RFC5444_... otherwise
 */
static INLINE enum rfc5444_result
rfc5444_writer_create_message_singletarget(
  struct rfc5444_writer *writer, uint8_t msgid, uint8_t addr_len, struct rfc5444_writer_target *target) {
  return rfc5444_writer_create_message(writer, msgid, addr_len, rfc5444_writer_singletarget_selector, target);
}

/**
 * creates a message of a certain ID for all target
 * @param writer pointer to writer context
 * @param msgid type of message
 * @param addr_len length of address for this message
 * @return RFC5444_OKAY if message was created and added to packet buffer,
 *   RFC5444_... otherwise
 */
static INLINE enum rfc5444_result
rfc5444_writer_create_message_alltarget(struct rfc5444_writer *writer, uint8_t msgid, uint8_t addr_len) {
  return rfc5444_writer_create_message(writer, msgid, addr_len, rfc5444_writer_alltargets_selector, NULL);
}

#endif /* RFC5444_WRITER_H_ */
