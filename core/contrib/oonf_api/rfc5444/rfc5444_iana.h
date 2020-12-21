
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

#ifndef RFC5444_IANA_H_
#define RFC5444_IANA_H_

#include "common/common_types.h"
#include "common/netaddr.h"

/**
 * IANA network registration for RFC5444 (RFC 5498)
 */
enum rfc5444_iana
{
  /*! IP protocol for RFC5444 */
  RFC5444_MANET_IPPROTO = 138,

  /*! UDP port number for RFC5444 */
  RFC5444_MANET_UDP_PORT = 269,
};

/*
 * text variants of the constants above for defaults in
 * configuration sections
 */

/*! IP protocol for RFC5444 */
#define RFC5444_MANET_IPPROTO_TXT "138"

/*! UDP port number for RFC5444 */
#define RFC5444_MANET_UDP_PORT_TXT "269"

/*! IPv4 multicast address for RFC5444 */
#define RFC5444_MANET_MULTICAST_V4_TXT "224.0.0.109"

/*! IPv6 multicast address for RFC5444 */
#define RFC5444_MANET_MULTICAST_V6_TXT "ff02::6d"

/**
 * RFC5444 message types defined by IANA
 */
enum rfc5444_msgtype_iana
{
  /*! RFC 6130 Hello (NHDP) */
  RFC6130_MSGTYPE_HELLO = 0,

  /*! RFC 7181 Topology Control (OLSRv2) */
  RFC7181_MSGTYPE_TC = 1,
};

/**
 * packet TLVs defined by IANA in RFC7182 (rfc5444-sec)
 */
enum rfc7182_pkttlvs_iana
{
  /*! integrity check value */
  RFC7182_PKTTLV_ICV = 5,
  RFC7182_PKTTLV_TIMESTAMP = 6,
};

/**
 * generic message TLV defined by IANA in RFC5497 (timetlv)
 */
enum rfc5497_msgtlvs_iana
{
  RFC5497_MSGTLV_INTERVAL_TIME = 0,
  RFC5497_MSGTLV_VALIDITY_TIME = 1,
};

/**
 * generic message TLV defined by IANA in RFC7181 (OLSRv2)
 */
enum rfc7181_msgtlvs_iana
{
  /*! MPR willingness */
  RFC7181_MSGTLV_MPR_WILLING = 7,

  /*! continuous sequence number */
  RFC7181_MSGTLV_CONT_SEQ_NUM = 8,
};

/**
 * generic message TLVs defined by IANA in RFC7182 (rfc5444-sec)
 */
enum rfc7182_msgtlvs_iana
{
  /*! integrity check value */
  RFC7182_MSGTLV_ICV = 5,
  RFC7182_MSGTLV_TIMESTAMP = 6,
};

/**
 * generic message TLVs defined in RFC7722 (olsrv2-mt)
 */
enum rfc7722_mt_iana
{
  RFC7722_MSGTLV_MPR_TYPES = RFC7181_MSGTLV_MPR_WILLING,
  RFC7722_MSGTLV_MPR_TYPES_EXT = 1,
};

/**
 * generic message TLVs for source specific routing
 */
enum draft_olsrv2_ssr_iana
{
  DRAFT_SSR_MSGTLV_CAPABILITY = RFC7181_MSGTLV_MPR_WILLING,
  DRAFT_SSR_MSGTLV_CAPABILITY_EXT = 2,
};

/*! default MPR_WILLINGNESS */
#define RFC7181_WILLINGNESS_DEFAULT_STRING "7"

/**
 * Values for RFC7181 MPR willingness
 */
enum rfc7181_willingness_values
{
  RFC7181_WILLINGNESS_UNDEFINED = -1,
  RFC7181_WILLINGNESS_MIN = 0,
  RFC7181_WILLINGNESS_NEVER = 0,
  RFC7181_WILLINGNESS_DEFAULT = 7,
  RFC7181_WILLINGNESS_ALWAYS = 15,
  RFC7181_WILLINGNESS_MAX = 15,

  /*! bitmask to get willingness from octet stream */
  RFC7181_WILLINGNESS_MASK = 0xf,

  /*! bitshift between two values of willingness in octet stream */
  RFC7181_WILLINGNESS_SHIFT = 4,
};

/**
 * extension types of CONT_SEQ_NUM TLV of RFC7181 (OLSRv2)
 */
enum rfc7181_cont_seq_num_ext
{
  /*! TC message is complete */
  RFC7181_CONT_SEQ_NUM_COMPLETE = 0,

  /*! TC has been split up into multiple messages */
  RFC7181_CONT_SEQ_NUM_INCOMPLETE = 1,

  /*! bitmask to get continous sequence number flag */
  RFC7181_CONT_SEQ_NUM_BITMASK = 1,
};

/**
 * generic address TLV defined by IANA in RFC5497 (timetlv)
 */
enum rfc5497_addrtlv_iana
{
  RFC5497_ADDRTLV_INTERVAL_TIME = 0,
  RFC5497_ADDRTLV_VALIDITY_TIME = 1,
};

/**
 * generic address TLV defined by IANA in RFC6130 (NHDP)
 */
enum rfc6130_addrtlv_iana
{
  /*! local interface status */
  RFC6130_ADDRTLV_LOCAL_IF = 2,

  RFC6130_ADDRTLV_LINK_STATUS = 3,

  /*! other neighbor status */
  RFC6130_ADDRTLV_OTHER_NEIGHB = 4,
};

/**
 * generic address TLVs defined by IANA in RFC7182 (rfc5444-sec)
 */
enum rfc7182_addrtlv_iana
{
  /*! integrity check value */
  RFC7182_ADDRTLV_ICV = 5,
  RFC7182_ADDRTLV_TIMESTAMP = 6,
};

/**
 * generic address TLV defined by IANA in RFC7181 (OLSRv2)
 */
enum rfc7181_addrtlv_iana
{
  RFC7181_ADDRTLV_LINK_METRIC = 7,

  /*! multipoint relay */
  RFC7181_ADDRTLV_MPR = 8,

  /*! neigbor address type */
  RFC7181_ADDRTLV_NBR_ADDR_TYPE = 9,
  RFC7181_ADDRTLV_GATEWAY = 10,
};

/**
 * extension types of GATEWAY TLV of RFC7181 (OLSRv2)
 */
enum rfc7181_gateway_ext
{
  /*! destination specific gateway */
  RFC7181_DSTSPEC_GATEWAY = 0,

  /*! source-destination specific gateway */
  RFC7181_SRCSPEC_GATEWAY = 1,

  /*! source-specific gateway */
  RFC7181_SRCSPEC_DEF_GATEWAY = 2,
};

/**
 * generic address TLV for source-specific gateway extension
 */
enum custom_srcspec_gateways
{
  /*! source prefix for source-destination specific gateway */
  SRCSPEC_GW_ADDRTLV_SRC_PREFIX = 224,
};

/**
 * values of LOCALIF TLV of RFC6130 (NHDP)
 */
enum rfc6130_localif_values
{
  /*! this is the interface that send the HELLO */
  RFC6130_LOCALIF_THIS_IF = 0,

  /*! this is not the interface that send the HELLO */
  RFC6130_LOCALIF_OTHER_IF = 1,

  /*! bitmask to extract localif value */
  RFC6130_LOCALIF_BITMASK = 1,
};

/**
 * values of LINKSTATUS TLV of RFC6130 (NHDP)
 */
enum rfc6130_linkstatus_values
{
  /*! this link neighbor was lost */
  RFC6130_LINKSTATUS_LOST = 0,

  /*! this link neighbor is symmetric */
  RFC6130_LINKSTATUS_SYMMETRIC = 1,

  /*! this link neighbor is asymmetric */
  RFC6130_LINKSTATUS_HEARD = 2,

  /*! bitmask to get LINKSTATUS flag */
  RFC6130_LINKSTATUS_BITMASK = 3,
};

/**
 * values of OTHERNEIGH TLV of RFC6130 (NHDP)
 */
enum rfc6130_otherneigh_values
{
  /*! this neighbor is symmetric on other interface */
  RFC6130_OTHERNEIGHB_SYMMETRIC = 1,

  /*! bitmask to get OTHERNEIGH flag */
  RFC6130_OTHERNEIGHB_BITMASK = 1,
};

/**
 * bit-flags for LINK_METRIC address TLV
 */
enum rfc7181_linkmetric_flags
{
  RFC7181_LINKMETRIC_INCOMING_LINK = 1 << 7,
  RFC7181_LINKMETRIC_OUTGOING_LINK = 1 << 6,
  RFC7181_LINKMETRIC_INCOMING_NEIGH = 1 << 5,
  RFC7181_LINKMETRIC_OUTGOING_NEIGH = 1 << 4,
};

/**
 * bit-flags for MPR address TLV
 */
enum rfc7181_mpr_bitmask
{
  RFC7181_MPR_FLOODING = 1 << 0,
  RFC7181_MPR_ROUTING = 1 << 1,
};

/**
 * bit-flags for neighbor address type TLV of RFC7181 (OLSRv2)
 */
enum rfc7181_nbr_addr_bitmask
{
  RFC7181_NBR_ADDR_TYPE_ORIGINATOR = 1,
  RFC7181_NBR_ADDR_TYPE_ROUTABLE = 2,
};

/**
 * type extensions for ICV tlv of RFC7182 (rfc5444-sec)
 */
enum rfc7182_icv_ext
{
  /*! generic integrity check value */
  RFC7182_ICV_EXT_GENERIC = 0,

  /*! encrypted hash ICV */
  RFC7182_ICV_EXT_CRYPTHASH = 1,

  /*! source specific encrypted hash ICV */
  RFC7182_ICV_EXT_SRCSPEC_CRYPTHASH = 2,
};

/**
 * hash type registry for ICV tlv of RFC7182 (rfc5444-sec)
 */
enum rfc7182_icv_hash
{
  RFC7182_ICV_HASH_UNKNOWN = -1,

  /*! identity hash (for testing!) */
  RFC7182_ICV_HASH_IDENTITY = 0,

  RFC7182_ICV_HASH_SHA_1 = 1,
  RFC7182_ICV_HASH_SHA_224 = 2,
  RFC7182_ICV_HASH_SHA_256 = 3,
  RFC7182_ICV_HASH_SHA_384 = 4,
  RFC7182_ICV_HASH_SHA_512 = 5,

  /*! number of defined hashes */
  RFC7182_ICV_HASH_COUNT,
};

/**
 * crypto algorithm registry for ICV tlv of RFC7182 (rfc5444-sec)
 */
enum rfc7182_icv_crypt
{
  RFC7182_ICV_CRYPT_UNKNOWN = -1,

  /*! identity hash (for testing!) */
  RFC7182_ICV_CRYPT_IDENTITY = 0,
  RFC7182_ICV_CRYPT_RSA = 1,
  RFC7182_ICV_CRYPT_DSA = 2,
  RFC7182_ICV_CRYPT_HMAC = 3,
  RFC7182_ICV_CRYPT_3DES = 4,
  RFC7182_ICV_CRYPT_AES = 5,
  RFC7182_ICV_CRYPT_ECDSA = 6,

  /*! number of defined crypto algorithms */
  RFC7182_ICV_CRYPT_COUNT,
};

/**
 * extension types for TIMESTAMP TLVs of RFC7182 (rfc5444-sec)
 */
enum rfc7182_timestamp_ext
{
  /*! strong monotonic increasing number */
  RFC7182_TIMESTAMP_EXT_MONOTONIC = 0,

  /*! unix timestamp */
  RFC7182_TIMESTAMP_EXT_UNIX = 1,

  /*! ntp timestamp */
  RFC7182_TIMESTAMP_EXT_NTP = 2,

  /*! random nounce */
  RFC7182_TIMESTAMP_EXT_RANDOM = 3,
};

EXPORT const char *rfc7182_get_hash_name(enum rfc7182_icv_hash);
EXPORT const char *rfc7182_get_crypt_name(enum rfc7182_icv_crypt);

EXPORT const char **rfc7182_get_hashes(void);
EXPORT const char **rfc7182_get_crypto(void);

EXPORT enum rfc7182_icv_hash rfc7182_get_hash_id(const char *name);
EXPORT enum rfc7182_icv_crypt rfc7182_get_crypt_id(const char *name);

#endif /* RFC5444_IANA_H_ */
