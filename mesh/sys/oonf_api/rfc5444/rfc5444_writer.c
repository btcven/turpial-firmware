
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
#include <stdlib.h>
#include <string.h>

#include "common/avl.h"
#include "common/avl_comp.h"
#include "common/common_types.h"
#include "common/list.h"
#include "rfc5444_api_config.h"
#include "rfc5444_context.h"
#include "rfc5444_writer.h"

static void _register_addrtlvtype(
  struct rfc5444_writer *writer, struct rfc5444_writer_message *msg, struct rfc5444_writer_tlvtype *type);
static void *_copy_addrtlv_value(struct rfc5444_writer *writer, const void *value, size_t length);
static void _lazy_free_message(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg);
static struct rfc5444_writer_message *_get_message(struct rfc5444_writer *writer, uint8_t msgid);
static struct rfc5444_writer_address *_malloc_address_entry(void);
static struct rfc5444_writer_addrtlv *_malloc_addrtlv_entry(void);
static void _free_address_entry(struct rfc5444_writer_address *addr);
static void _free_addrtlv_entry(struct rfc5444_writer_addrtlv *addrtlv);

/**
 * @param type TLV type
 * @param exttype TLV extension type
 * @return combined type
 */
static INLINE int
_get_fulltype(uint8_t type, uint8_t exttype) {
  return type * 256 + exttype;
}

/**
 * Creates a new rfc5444 writer context
 * @param writer pointer to writer context
 */
void
rfc5444_writer_init(struct rfc5444_writer *writer) {
  assert(writer->msg_buffer != NULL && writer->msg_size > 0);
  assert(writer->addrtlv_buffer != NULL && writer->addrtlv_size > 0);

  /* set default memory handler functions */
  if (!writer->malloc_address_entry)
    writer->malloc_address_entry = _malloc_address_entry;
  if (!writer->malloc_addrtlv_entry)
    writer->malloc_addrtlv_entry = _malloc_addrtlv_entry;
  if (!writer->free_address_entry)
    writer->free_address_entry = _free_address_entry;
  if (!writer->free_addrtlv_entry)
    writer->free_addrtlv_entry = _free_addrtlv_entry;

  oonf_list_init_head(&writer->_targets);

  /* initialize packet buffer */
  writer->_msg.buffer = writer->msg_buffer;
  _rfc5444_tlv_writer_init(&writer->_msg, 0, writer->msg_size);

  oonf_list_init_head(&writer->_pkthandlers);
  oonf_list_init_head(&writer->_targets);
  oonf_list_init_head(&writer->_addr_tlvtype_head);

  avl_init(&writer->_msgcreators, avl_comp_uint8, false);
  avl_init(&writer->_processors, avl_comp_int32, true);
  avl_init(&writer->_forwarding_processors, avl_comp_int32, true);

#if WRITER_STATE_MACHINE == true
  writer->_state = RFC5444_WRITER_NONE;
#endif
}

/**
 * Cleanup a writer context and free all reserved memory
 * This function must not be called outside the message_addresses callback.
 *
 * @param writer pointer to writer context
 */
void
rfc5444_writer_cleanup(struct rfc5444_writer *writer) {
  struct rfc5444_writer_message *msg, *safe_msg;
  struct rfc5444_writer_pkthandler *pkt, *safe_pkt;
  struct rfc5444_writer_content_provider *provider, *safe_prv;
  struct rfc5444_writer_tlvtype *tlvtype, *safe_tt;
  struct rfc5444_writer_target *interf, *safe_interf;
  struct rfc5444_writer_postprocessor *processor, *safe_proc;

  assert(writer);
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif

  /* remove all packet handlers */
  oonf_list_for_each_element_safe(&writer->_pkthandlers, pkt, _pkthandle_node, safe_pkt) {
    rfc5444_writer_unregister_pkthandler(writer, pkt);
  }

  /* remove all _targets */
  oonf_list_for_each_element_safe(&writer->_targets, interf, _target_node, safe_interf) {
    rfc5444_writer_unregister_target(writer, interf);
  }

  /* remove all generic address tlvtypes */
  oonf_list_for_each_element_safe(&writer->_addr_tlvtype_head, tlvtype, _tlvtype_node, safe_tt) {
    rfc5444_writer_unregister_addrtlvtype(writer, tlvtype);
  }

  /* remove all packet postprocessors */
  avl_for_each_element_safe(&writer->_processors, processor, _node, safe_proc) {
    rfc5444_writer_unregister_postprocessor(writer, processor);
  }

  /* remove all message creators */
  avl_for_each_element_safe(&writer->_msgcreators, msg, _msgcreator_node, safe_msg) {
    /* prevent message from being freed in the middle of the processing */
    msg->_registered = true;

    /* remove all message content providers */
    avl_for_each_element_safe(&msg->_provider_tree, provider, _provider_node, safe_prv) {
      rfc5444_writer_unregister_content_provider(writer, provider, NULL, 0);
    }

    /* remove all registered address tlvs */
    oonf_list_for_each_element_safe(&msg->_msgspecific_tlvtype_head, tlvtype, _tlvtype_node, safe_tt) {
      rfc5444_writer_unregister_addrtlvtype(writer, tlvtype);
    }

    /* remove message and addresses */
    rfc5444_writer_unregister_message(writer, msg);
  }
}

/**
 * Adds a tlv to an address.
 * This function must not be called outside the message_addresses callback.
 *
 * @param writer pointer to writer context
 * @param addr pointer to address object
 * @param tlvtype pointer to predefined tlvtype object
 * @param value pointer to value or NULL if no value
 * @param length length of value in bytes or 0 if no value
 * @param allow_dup true if multiple TLVs of the same type are allowed,
 *   false otherwise
 * @return RFC5444_OKAY if tlv has been added successfully, RFC5444_... otherwise
 */
enum rfc5444_result
rfc5444_writer_add_addrtlv(struct rfc5444_writer *writer, struct rfc5444_writer_address *addr,
  struct rfc5444_writer_tlvtype *tlvtype, const void *value, size_t length, bool allow_dup)
{
  struct rfc5444_writer_addrtlv *addrtlv;

#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_ADD_ADDRESSES);
#endif

  /* check for collision if necessary */
  if (!allow_dup && avl_find(&addr->_addrtlv_tree, &tlvtype->_full_type) != NULL) {
    return RFC5444_DUPLICATE_TLV;
  }

  if ((addrtlv = writer->malloc_addrtlv_entry()) == NULL) {
    /* out of memory error */
    return RFC5444_OUT_OF_MEMORY;
  }

  /* set back pointer */
  addrtlv->address = addr;
  addrtlv->tlvtype = tlvtype;

  /* copy value(length) */
  addrtlv->length = length;
  if (length > 0 && (addrtlv->value = _copy_addrtlv_value(writer, value, length)) == NULL) {
    writer->free_addrtlv_entry(addrtlv);
    return RFC5444_OUT_OF_ADDRTLV_MEM;
  }

  /* add to address tree */
  addrtlv->addrtlv_node.key = &tlvtype->_full_type;
  avl_insert(&addr->_addrtlv_tree, &addrtlv->addrtlv_node);

  return RFC5444_OKAY;
}

/**
 * Add a network prefix to a rfc5444 message.
 * This function must not be called outside the message_addresses callback.
 *
 * @param writer pointer to writer context
 * @param msg pointer to message object
 * @param naddr pointer network address
 * @param mandatory true if address is mandatory for all fragments of message
 * @return pointer to address object, NULL if an error happened
 */
struct rfc5444_writer_address *
rfc5444_writer_add_address(struct rfc5444_writer *writer __attribute__((unused)), struct rfc5444_writer_message *msg,
  const struct netaddr *naddr, bool mandatory) {
  struct rfc5444_writer_address *address;

#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_ADD_ADDRESSES);
#endif

  address = avl_find_element(&msg->_addr_tree, naddr, address, _addr_tree_node);
  if (address == NULL) {
    if ((address = writer->malloc_address_entry()) == NULL) {
      return NULL;
    }

    memcpy(&address->address, naddr, sizeof(*naddr));

    /* add address to address list */
    if (mandatory) {
      oonf_list_add_tail(&msg->_addr_head, &address->_addr_oonf_list_node);
    }
    else {
      oonf_list_add_tail(&msg->_non_mandatory_addr_head, &address->_addr_oonf_list_node);
    }

    /* add address into message address tree */
    address->_addr_tree_node.key = &address->address;
    avl_insert(&msg->_addr_tree, &address->_addr_tree_node);

    avl_init(&address->_addrtlv_tree, avl_comp_uint32, true);
  }

  address->_mandatory_addr |= mandatory;

  return address;
}

/**
 * Register an tlv type for addressblocks of a certain message.
 * This function must NOT be called from the rfc5444 writer callbacks.
 *
 * @param writer pointer to writer context
 * @param type pointer to tlvtype structure, type and exttype must be already initialized
 * @param msgtype message type this TLV belongs too, -1 for address TLV
 * @return 0 if addresstlvtype was registered, -1 otherwise
 */
int
rfc5444_writer_register_addrtlvtype(struct rfc5444_writer *writer, struct rfc5444_writer_tlvtype *type, int msgtype) {
  struct rfc5444_writer_message *msg = NULL;

#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif
  if (msgtype >= 0 && msgtype <= 255) {
    if ((msg = _get_message(writer, msgtype)) == NULL) {
      /* out of memory error ? */
      return -1;
    }
  }

  _register_addrtlvtype(writer, msg, type);
  return 0;
}

/**
 * Remove registration of tlvtype for addresses.
 * This function must not be called outside the message_addresses callback.
 *
 * @param writer pointer to writer context
 * @param tlvtype pointer to tlvtype object
 */
void
rfc5444_writer_unregister_addrtlvtype(struct rfc5444_writer *writer, struct rfc5444_writer_tlvtype *tlvtype) {
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif
  if (!oonf_list_is_node_added(&tlvtype->_tlvtype_node)) {
    return;
  }

  oonf_list_remove(&tlvtype->_tlvtype_node);

  if (tlvtype->_creator) {
    /* message specific address tlv, see if we need to remove the message itself */
    _lazy_free_message(writer, tlvtype->_creator);
  }
}

/**
 * Register a content provider for a message type
 * This function must not be called outside the message_addresses callback.
 *
 * @param writer pointer to writer context
 * @param cpr pointer to message content provider object
 * @param addrtlvs pointer to array of tlvtype definitions
 * @param addrtlvs_count length of addressblock tlv array
 * @return -1 if an error happened, 0 otherwise
 */
int
rfc5444_writer_register_msgcontentprovider(struct rfc5444_writer *writer, struct rfc5444_writer_content_provider *cpr,
  struct rfc5444_writer_tlvtype *addrtlvs, size_t addrtlvs_count) {
  struct rfc5444_writer_message *msg;
  size_t i;

#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif
  /* first allocate the message if necessary */
  if ((msg = _get_message(writer, cpr->msg_type)) == NULL) {
    return -1;
  }

  for (i = 0; i < addrtlvs_count; i++) {
    _register_addrtlvtype(writer, msg, &addrtlvs[i]);
  }

  cpr->creator = msg;
  cpr->_provider_node.key = &cpr->priority;

  avl_insert(&msg->_provider_tree, &cpr->_provider_node);
  return 0;
}

/**
 * Unregister a message content provider
 * This function must not be called outside the message_addresses callback.
 * @param writer pointer to writer context
 * @param cpr pointer to message context provider object
 * @param addrtlvs pointer to array of addressblock tlv definitions
 * @param addrtlvs_count length of addressblock tlv array
 */
void
rfc5444_writer_unregister_content_provider(struct rfc5444_writer *writer, struct rfc5444_writer_content_provider *cpr,
  struct rfc5444_writer_tlvtype *addrtlvs, size_t addrtlvs_count) {
  size_t i;
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif

  if (!avl_is_node_added(&cpr->_provider_node)) {
    return;
  }

  for (i = 0; i < addrtlvs_count; i++) {
    rfc5444_writer_unregister_addrtlvtype(writer, &addrtlvs[i]);
  }
  avl_remove(&cpr->creator->_provider_tree, &cpr->_provider_node);
  _lazy_free_message(writer, cpr->creator);
}

/**
 * Register a message type for the writer
 * This function must not be called outside the message_addresses callback.
 *
 * @param writer pointer to writer context
 * @param msgid message type
 * @param if_specific true if an unique message must be created for each
 *   interface
 * @return message object, NULL if an error happened
 */
struct rfc5444_writer_message *
rfc5444_writer_register_message(struct rfc5444_writer *writer, uint8_t msgid, bool if_specific) {
  struct rfc5444_writer_message *msg;

#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif

  msg = _get_message(writer, msgid);
  if (msg == NULL) {
    /* out of memory error */
    return NULL;
  }

  if (msg->_registered) {
    /* message was already _registered */
    return NULL;
  }

  /* mark message as _registered */
  msg->_registered = true;

  /* set target_specific flag */
  msg->target_specific = if_specific;
  return msg;
}

/**
 * Unregisters a message type
 * This function must not be called outside the message_addresses callback.
 *
 * @param writer pointer to writer context
 * @param msg pointer to message object
 */
void
rfc5444_writer_unregister_message(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg) {
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif

  if (!avl_is_node_added(&msg->_msgcreator_node)) {
    return;
  }

  /* free addresses */
  _rfc5444_writer_free_addresses(writer, msg);

  /* mark message as unregistered */
  msg->_registered = false;
  _lazy_free_message(writer, msg);
}

/**
 * Registers a new post-processor
 * @param writer rfc5444 writer
 * @param processor rfc5444 post-processor
 */
void
rfc5444_writer_register_postprocessor(struct rfc5444_writer *writer, struct rfc5444_writer_postprocessor *processor) {
  processor->_node.key = &processor->priority;
  avl_insert(&writer->_processors, &processor->_node);
}

/**
 * Unregisters a post-processor
 * @param writer rfc5444 writer
 * @param processor rfc5444 post-processor
 */
void
rfc5444_writer_unregister_postprocessor(struct rfc5444_writer *writer, struct rfc5444_writer_postprocessor *processor) {
  if (avl_is_node_added(&processor->_node)) {
    avl_remove(&writer->_processors, &processor->_node);
  }
}

/**
 * Registers a new forward handler
 * @param writer rfc5444 writer
 * @param forward rfc5444 forward handler
 */
void
rfc5444_writer_register_forward_handler(struct rfc5444_writer *writer, struct rfc5444_writer_forward_handler *forward) {
  forward->_node.key = &forward->priority;
  avl_insert(&writer->_forwarding_processors, &forward->_node);
}

/**
 * Unregisters a forward handler
 * @param writer rfc5444 writer
 * @param forward rfc5444 forward handler
 */
void
rfc5444_writer_unregister_forward_handler(
  struct rfc5444_writer *writer, struct rfc5444_writer_forward_handler *forward) {
  if (avl_is_node_added(&forward->_node)) {
    avl_remove(&writer->_forwarding_processors, &forward->_node);
  }
}

/**
 * Registers a packet handler for the writer
 * This function must not be called outside the message_addresses callback.
 *
 * @param writer pointer to writer context
 * @param pkt pointer to packet handler object
 */
void
rfc5444_writer_register_pkthandler(struct rfc5444_writer *writer, struct rfc5444_writer_pkthandler *pkt) {
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif

  oonf_list_add_tail(&writer->_pkthandlers, &pkt->_pkthandle_node);
}

/**
 * Unregisters a packet handler
 * This function must not be called outside the message_addresses callback.
 *
 * @param writer pointer to writer context
 * @param pkt pointer to packet handler object
 */
void
rfc5444_writer_unregister_pkthandler(
  struct rfc5444_writer *writer __attribute__((unused)), struct rfc5444_writer_pkthandler *pkt) {
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif
  if (oonf_list_is_node_added(&pkt->_pkthandle_node)) {
    oonf_list_remove(&pkt->_pkthandle_node);
  }
}

/**
 * Registers a new outgoing interface for the writer context
 * @param writer pointer to writer context
 * @param interf pointer to interface object
 */
void
rfc5444_writer_register_target(struct rfc5444_writer *writer, struct rfc5444_writer_target *interf) {
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif

  assert(interf->packet_buffer != NULL && interf->packet_size > 0);

  interf->_pkt.buffer = interf->packet_buffer;
  _rfc5444_tlv_writer_init(&interf->_pkt, interf->packet_size, interf->packet_size);

  interf->_is_flushed = true;

  oonf_list_add_tail(&writer->_targets, &interf->_target_node);
}

/**
 * Unregisters an interface from the writer context and free
 * all allocated memory associated with the interface
 * @param writer pointer to writer context
 * @param interf pointer to interface object
 */
void
rfc5444_writer_unregister_target(
  struct rfc5444_writer *writer __attribute__((unused)), struct rfc5444_writer_target *interf) {
#if WRITER_STATE_MACHINE == true
  assert(writer->_state == RFC5444_WRITER_NONE);
#endif

  /* remove interface from writer */
  if (oonf_list_is_node_added(&interf->_target_node)) {
    oonf_list_remove(&interf->_target_node);
  }
}

/**
 * Creates a rfc5444 writer object
 * @param writer pointer to writer context
 * @param msgid message type
 * @return pointer to message object, NULL if an error happened
 */
static struct rfc5444_writer_message *
_get_message(struct rfc5444_writer *writer, uint8_t msgid) {
  struct rfc5444_writer_message *msg;

  msg = avl_find_element(&writer->_msgcreators, &msgid, msg, _msgcreator_node);
  if (msg != NULL) {
    return msg;
  }

  if ((msg = calloc(1, sizeof(*msg))) == NULL) {
    return NULL;
  }

  /* initialize key */
  msg->type = msgid;
  msg->_msgcreator_node.key = &msg->type;
  if (avl_insert(&writer->_msgcreators, &msg->_msgcreator_node)) {
    free(msg);
    return NULL;
  }

  /* initialize list/tree heads */
  avl_init(&msg->_provider_tree, avl_comp_int32, true);
  oonf_list_init_head(&msg->_msgspecific_tlvtype_head);

  avl_init(&msg->_addr_tree, avl_comp_netaddr, false);
  oonf_list_init_head(&msg->_addr_head);
  oonf_list_init_head(&msg->_non_mandatory_addr_head);
  return msg;
}

/**
 * Register an tlv type for addressblocks of a certain message.
 * This function must NOT be called from the rfc5444 writer callbacks.
 *
 * @param writer pointer to writer context
 * @param msg pointer to allocated rfc5444_writer_message, NULL
 *   if generic address tlvtype
 * @param tlvtype pointer to preallocated rfc5444_writer_tlvtype,
 *    type and exttype must already be initialized
 */
static void
_register_addrtlvtype(
  struct rfc5444_writer *writer, struct rfc5444_writer_message *msg, struct rfc5444_writer_tlvtype *tlvtype) {
  /* initialize addrtlv fields */
  tlvtype->_creator = msg;
  tlvtype->_full_type = _get_fulltype(tlvtype->type, tlvtype->exttype);

  assert(!oonf_list_is_node_added(&tlvtype->_tlvtype_node));

  if (msg) {
    /* add to message creator list */
    oonf_list_add_tail(&msg->_msgspecific_tlvtype_head, &tlvtype->_tlvtype_node);
  }
  else {
    /* add to generic address tlvtype list */
    oonf_list_add_tail(&writer->_addr_tlvtype_head, &tlvtype->_tlvtype_node);
  }

  oonf_list_init_head(&tlvtype->_current_tlv_list);
}

/**
 * Copies the value of an address tlv into the internal static buffer
 * @param writer pointer to writer context
 * @param value pointer to tlv value
 * @param length number of bytes of tlv value
 */
static void *
_copy_addrtlv_value(struct rfc5444_writer *writer, const void *value, size_t length) {
  void *ptr;
  if (writer->_addrtlv_used + length > writer->addrtlv_size) {
    /* not enough memory for addrtlv values */
    return NULL;
  }

  ptr = &writer->addrtlv_buffer[writer->_addrtlv_used];
  memcpy(ptr, value, length);
  writer->_addrtlv_used += length;

  return ptr;
}

/**
 * Free all allocated addresses in a writers context
 * @param writer pointer to writer context
 * @param msg pointer to message object
 */
void
_rfc5444_writer_free_addresses(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg) {
  struct rfc5444_writer_address *addr, *safe_addr;
  struct rfc5444_writer_addrtlv *addrtlv, *safe_addrtlv;

  avl_remove_all_elements(&msg->_addr_tree, addr, _addr_tree_node, safe_addr) {
    /* remove from list too */
    oonf_list_remove(&addr->_addr_oonf_list_node);

    avl_remove_all_elements(&addr->_addrtlv_tree, addrtlv, addrtlv_node, safe_addrtlv) {
      writer->free_addrtlv_entry(addrtlv);
    }
    writer->free_address_entry(addr);
  }

  /* allow overwriting of addrtlv-value buffer */
  writer->_addrtlv_used = 0;
}

/**
 * Free message object if not in use anymore
 * @param writer pointer to writer context
 * @param msg pointer to message object
 */
static void
_lazy_free_message(struct rfc5444_writer *writer, struct rfc5444_writer_message *msg) {
  if (!msg->_registered && oonf_list_is_empty(&msg->_addr_head) && oonf_list_is_empty(&msg->_msgspecific_tlvtype_head) &&
      avl_is_empty(&msg->_provider_tree)) {
    avl_remove(&writer->_msgcreators, &msg->_msgcreator_node);
    free(msg);
  }
}

/**
 * Default allocater for address objects
 * @return pointer to cleaned address object, NULL if an error happened
 */
static struct rfc5444_writer_address *
_malloc_address_entry(void) {
  return calloc(1, sizeof(struct rfc5444_writer_address));
}

/**
 * Default allocator for address tlv object.
 * @return pointer to cleaned address tlv object, NULL if an error happened
 */
static struct rfc5444_writer_addrtlv *
_malloc_addrtlv_entry(void) {
  return calloc(1, sizeof(struct rfc5444_writer_addrtlv));
}

/**
 * Default deallocater for address objects
 * @param addr pointer to address object
 */
static void
_free_address_entry(struct rfc5444_writer_address *addr) {
  free(addr);
}

/**
 * Default deallocator for address tlv object.
 * @param addrtlv pointer to address tlv object
 */
static void
_free_addrtlv_entry(struct rfc5444_writer_addrtlv *addrtlv) {
  free(addrtlv);
}
