/*
 * Copyright (c) 2016-2019, Texas Instruments Incorporated
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

#ifndef MT_H
#define MT_H

#include <stdint.h>

#include <esp_err.h>

#include "mt_rpc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    RPC command/response message parameter block
 */
typedef struct {
    uint8_t cmd0; /**< RPC command/subsystem bit fields */
    uint8_t cmd1; /**< RPC subsystem command ID number */
    uint8_t block; /**< RPC fragmentation block index */
    uint8_t verid; /**< RPC version/stackId bit fields */
    uint16_t length; /**< RPC data packet data length */
    void *data; /**< Ptr to RPC packet data buffer */
} mt_mpb_t;

#define MT_MAC_RESET_REQ         (0x01) /*! MT command code - MAC Reset request */
#define MT_MAC_INIT_REQ          (0x02) /*! MT command code - MAC Init request */
#define MT_MAC_START_REQ         (0x03) /*! MT command code - MAC Start request */
#define MT_MAC_SYNC_REQ          (0x04) /*! MT command code - MAC Sync request */
#define MT_MAC_DATA_REQ          (0x05) /*! MT command code - MAC Data request */
#define MT_MAC_ASSOCIATE_REQ     (0x06) /*! MT command code - MAC Associate request */
#define MT_MAC_DISASSOCIATE_REQ  (0x07) /*! MT command code - MAC Disassociate request */
#define MT_MAC_GET_REQ           (0x08) /*! MT command code - MAC PIB Get request */
#define MT_MAC_SET_REQ           (0x09) /*! MT command code - MAC PIB Set request */
#define MT_MAC_SCAN_REQ          (0x0C) /*! MT command code - MAC Scan request */
#define MT_MAC_POLL_REQ          (0x0D) /*! MT command code - MAC Poll request */
#define MT_MAC_PURGE_REQ         (0x0E) /*! MT command code - MAC Purge request */
#define MT_MAC_SET_RX_GAIN_REQ   (0x0F) /*! MT command code - MAC Set RX Gain request */
#define MT_MAC_SECURITY_GET_REQ  (0x30) /*! MT command code - MAC Security PIB Get request */
#define MT_MAC_SECURITY_SET_REQ  (0x31) /*! MT command code - MAC Security PIB Set request */
#define MT_MAC_UPDATE_PANID_REQ  (0x32) /*! MT command code - MAC Security Update PanID request */
#define MT_MAC_ADD_DEVICE_REQ    (0x33) /*! MT command code - MAC Security Add Device request */
#define MT_MAC_DELETE_DEVICE_REQ (0x34) /*! MT command code - MAC Security Delete Device request */
#define MT_MAC_DELETE_ALL_REQ    (0x35) /*! MT command code - MAC Security Delete All Devices request */
#define MT_MAC_DELETE_KEY_REQ    (0x36) /*! MT command code - MAC Security PIB Delete Key request */
#define MT_MAC_READ_KEY_REQ      (0x37) /*! MT command code - MAC Security Read Key request */
#define MT_MAC_WRITE_KEY_REQ     (0x38) /*! MT command code - MAC Security Write Key request */
#define MT_MAC_SRC_MATCH_EN_REQ  (0x39) /*! MT command code - MAC Security Update PanID request */
#define MT_MAC_FH_ENABLE_REQ     (0x40) /*! MT command code - MAC Frequency Hop Enable request */
#define MT_MAC_FH_START_REQ      (0x41) /*! MT command code - MAC Frequency Hop Start request */
#define MT_MAC_FH_GET_REQ        (0x42) /*! MT command code - MAC Frequency Hop PIB Get request */
#define MT_MAC_FH_SET_REQ        (0x43) /*! MT command code - MAC Frequency Hop PIB Set request */
#define MT_MAC_WS_ASYNC_REQ      (0x44) /*! MT command code - MAC Wi-SUN Async request */
#define MT_MAC_ASSOCIATE_RSP     (0x50) /*! MT command code - MAC Associate response */
#define MT_MAC_ORPHAN_RSP        (0x51) /*! MT command code - MAC Orphan response */
#define MT_MAC_SYNC_LOSS_IND     (0x80) /**< MT feedback code - MAC Sync Loss indication */
#define MT_MAC_ASSOCIATE_IND     (0x81) /**< MT feedback code - MAC Associate indication */
#define MT_MAC_ASSOCIATE_CNF     (0x82) /**< MT feedback code - MAC Associate confirm */
#define MT_MAC_BEACON_NOTIFY_IND (0x83) /**< MT feedback code - MAC Beacon Notify indication */
#define MT_MAC_DATA_CNF          (0x84) /**< MT feedback code - MAC Data confirm */
#define MT_MAC_DATA_IND          (0x85) /**< MT feedback code - MAC Data indication */
#define MT_MAC_DISASSOCIATE_IND  (0x86) /**< MT feedback code - MAC Disassociate indication */
#define MT_MAC_DISASSOCIATE_CNF  (0x87) /**< MT feedback code - MAC Disassociate confirm */
#define MT_MAC_ORPHAN_IND        (0x8A) /**< MT feedback code - MAC Orphan indication */
#define MT_MAC_POLL_CNF          (0x8B) /**< MT feedback code - MAC Poll confirm */
#define MT_MAC_SCAN_CNF          (0x8C) /**< MT feedback code - MAC Scan confirm */
#define MT_MAC_COMM_STATUS_IND   (0x8D) /**< MT feedback code - MAC Comm Status indication */
#define MT_MAC_START_CNF         (0x8E) /**< MT feedback code - MAC Start confirm */
#define MT_MAC_PURGE_CNF         (0x90) /**< MT feedback code - MAC Purge confirm */
#define MT_MAC_POLL_IND          (0x91) /**< MT feedback code - MAC Poll indication */
#define MT_MAC_ASYNC_CNF         (0x92) /**< MT feedback code - MAC Wi-SUN Async confirm */
#define MT_MAC_ASYNC_IND         (0x93) /**< MT feedback code - MAC Wi-SUN Async indication */

#define MT_SYS_RESET_REQ  (0x00) /**< MT command code - SYS Reset request */
#define MT_SYS_PING       (0x01) /**< MT command code - SYS Ping request */
#define MT_SYS_VERSION    (0x02) /**< MT command code - SYS Version request */
#define MT_SYS_NV_CREATE  (0x30) /**< MT command code - SYS NV Create request */
#define MT_SYS_NV_DELETE  (0x31) /**< MT command code - SYS NV Delete request */
#define MT_SYS_NV_LENGTH  (0x32) /**< MT command code - SYS NV Length request */
#define MT_SYS_NV_READ    (0x33) /**< MT command code - SYS NV Read request */
#define MT_SYS_NV_WRITE   (0x34) /**< MT command code - SYS NV Write request */
#define MT_SYS_NV_UPDATE  (0x35) /**< MT command code - SYS NV Update request */
#define MT_SYS_NV_COMPACT (0x36) /**< MT command code - SYS NV Compact request */
#define MT_SYS_RESET_IND  (0x80) /**< MT feedback code - SYS Reset indication */

#define MT_UTIL_CALLBACK_SUB (0x06) /**< MT command code - UTIL Callback Subscribe request */
#define MT_UTIL_LOOPBACK     (0x10) /**< MT command code - UTIL Loopback Test request */
#define MT_UTIL_RANDOM       (0x12) /**< MT command code - UTIL Random Number request */
#define MT_UTIL_EXT_ADDR     (0xEE) /**< MT command code - UTIL Extended Address request */

/**
 * @brief    SubSystem Capabilities - returned in PING response
 * @{
 */
#define MT_CAP_SYS  (0x0001) /**< MT_SYS sub-system capability bit */
#define MT_CAP_MAC  (0x0002) /**< MT_MAC sub-system capability bit */
#define MT_CAP_UTIL (0x0040) /**< MT_UTIL sub-system capability bit */
#define MT_CAP_APP  (0x0100) /**< MT_APP sub-system capability bit */
/** @} */

esp_err_t mt_init(void);
esp_err_t mt_send(uint8_t cmd0, uint8_t cmd1, void *buf, uint8_t buf_len);
esp_err_t mt_send_ping(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* MT_H */
