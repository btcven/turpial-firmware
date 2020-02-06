/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef MT_RPC_H
#define MT_RPC_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MT_RPC_FRAME_HDR_SZ  3 /*! Length of MT header for general format RPC frame */

/*!
 Position of fields in the general format RPC frame:
   1st byte is length of the data field
   2nd byte is command type/sub-system
   3rd byte is sub-system command ID
   4th+ bytes are data block (if any)
 */
#define MT_RPC_POS_LEN   0 /*! General RPC frame header data length byte position */
#define MT_RPC_POS_CMD0  1 /*! General RPC frame header command type/sub-system byte position */
#define MT_RPC_POS_CMD1  2 /*! General RPC frame header sub-system command ID byte position */
#define MT_RPC_POS_DAT0  3 /*! General RPC frame header data block byte position */

/*!
 Maximum length of data in the general frame format. The upper limit is 255
 because of the 1-byte length protocol. The operational limit is lower for
 code size and ram savings so that the uart driver can use 256 byte rx/tx
 queues: (MT_RPC_DATA_MAX + MT_RPC_FRAME_HDR_SZ + MTUART_FRAME_OVHD) < 256
 */
#define MT_RPC_DATA_MAX  250 /*! General RPC frame maximum data block size */

/*!
 The CMD0 frame header byte is composed of two bit fields:
   3 MSB's (bits 05-07) contain the command type
   5 LSB's (bits 00-04) contain the command sub-system ID
 */
#define MT_RPC_CMD_TYPE_MASK   0xE0 /*! General RPC frame command type bit mask */
#define MT_RPC_SUBSYSTEM_MASK  0x1F /*! General RPC frame command sub-system bit mask */

#define MT_RPC_SUCCESS         0 /*! General RPC format status: no error */
#define MT_RPC_ERR_SUBSYSTEM   1 /*! General RPC format status: invalid sub-system */
#define MT_RPC_ERR_COMMAND_ID  2 /*! General RPC format status: invalid command ID */
#define MT_RPC_ERR_PARAMETER   3 /*! General RPC format status: invalid parameter */
#define MT_RPC_ERR_LENGTH      4 /*! General RPC format status: invalid length */
#define MT_RPC_ERR_EXTTYPE     5 /*! General RPC format status: unsupported extended type */
#define MT_RPC_ERR_NOMEMORY    6 /*! General RPC format status: memory allocation failed */

#define MT_RPC_FRAG_ACK_SZ  3 /*! Length of extended header for fragmentation ACK frame */
#define MT_RPC_FRAG_HDR_SZ  4 /*! Length of extended header for fragmentation RPC frame */

/*!
 Position of fields in the extended format RPC header:
   1st byte is version/stackId bit fields
   2nd byte is fragmentation block index
   3rd-4th bytes are full data packet length
 */
#define MT_RPC_POS_VERS  0 /*! Extended RPC frame header version/stackId position */
#define MT_RPC_POS_FBLK  1 /*! Extended RPC frame header fragmentation block index position */
#define MT_RPC_POS_ACKS  2 /*! Extended RPC frame header fragmentation ACK status position */
#define MT_RPC_POS_FLEN  2 /*! Extended RPC frame header fragmentation packet length position */
#define MT_RPC_POS_DATX  4 /*! Extended RPC frame header fragmentation data block byte position */

/**
 * @brief   The VERS frame header byte is composed of two bit fields:
 *          - 5 MSB's (bits 03-07) contain the extended frame version number
 *          - 3 LSB's (bits 00-02) contain the extended frame stack ID number
 * @{
 */
#define MT_RPC_EXTVERS_MASK  0xF8 /*! Extended RPC frame extended version number bit mask */
#define MT_RPC_STACKID_MASK  0x07 /*! Extended RPC frame extended stackId number bit mask */
/** @} */

#define MT_RPC_EXT_SUCCESS    0 /*! Extended RPC format status: no error */
#define MT_RPC_EXT_RESEND     1 /*! Extended RPC resend last frame */
#define MT_RPC_EXT_BADSTACK   2 /*! Extended RPC unsupported Stack ID  */
#define MT_RPC_EXT_BADBLOCK   3 /*! Extended RPC fragmentation block out-of-order */
#define MT_RPC_EXT_BADLENGTH  4 /*! Extended RPC fragmentation bad block length */
#define MT_RPC_EXT_NOMEMORY   5 /*! Extended RPC memory allocation error */
#define MT_RPC_EXT_FRAGDONE   6 /*! Extended RPC fragmentation process is done */
#define MT_RPC_EXT_FRAGABORT  7 /*! Extended RPC fragmentation process aborted */
#define MT_RPC_EXT_BADACK     8 /*! Extended RPC fragmentation unknown ACK  */

/*! General header command codes, shifted to bits 05-07 position */
typedef enum {
    MT_RPC_CMD_POLL = 0x00, /*! RPC poll command code */
    MT_RPC_CMD_SREQ = 0x20, /*! RPC synchronous request command code */
    MT_RPC_CMD_AREQ = 0x40, /*! RCP asynchrounous request command code */
    MT_RPC_CMD_SRSP = 0x60, /*! RPC synchronous response code */
    MT_RPC_CMD_EXTN = 0x80 /*! RPC extended header indicator */
} mt_rpc_cmdtype_t;

typedef enum
{
    MT_RPC_SYS_RES0, /*! RPC sub-system code */
    MT_RPC_SYS_SYS, /*! MTSYS sub-system code */
    MT_RPC_SYS_MAC, /*! MTMAC sub-system code */
    MT_RPC_SYS_RESERVED03, /*! Reserved sub-system code */
    MT_RPC_SYS_RESERVED04, /*! Reserved sub-system code */
    MT_RPC_SYS_RESERVED05, /*! Reserved sub-system code */
    MT_RPC_SYS_RESERVED06, /*! Reserved sub-system code */
    MT_RPC_SYS_UTIL, /*!  MTUTIL sub-system code */
    MT_RPC_SYS_RESERVED08, /*! Reserved sub-system code */
    MT_RPC_SYS_APP, /*! MTAPP sub-system code */
    MT_RPC_SYS_MAX /*! Maximum sub-system code, must be last in list */
} mt_rpc_systype_t;

/*! Extended header version codes, shifted to bits 03-07 position */
typedef enum {
    MT_RPC_EXT_NOTUSED = 0x00, /*! Extended RPC not used */
    MT_RPC_EXT_STACKID = 0x08, /*! Extended RPC StackID frame */
    MT_RPC_EXT_FRAGBLK = 0x10, /*! Extended RPC Fragmentation data frame */
    MT_RPC_EXT_FRAGACK = 0x18, /*! Extended RPC Fragmentation ACK frame */
    MT_RPC_EXT_FRAGSTS = 0x20 /*! Extended RPC Status frame */
} mt_rpc_exttype_t;

#ifdef __cplusplus
}; /* extern "C" */
#endif

#endif /* MT_RPC_H */

