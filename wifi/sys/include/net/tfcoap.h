/* Copyright 2019 btcven and Locha Mesh developers
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @defgroup    net_coap
 * @ingroup     net_coap
 *
 * @{
 * @file        tfcoap.h
 * @brief       Constrained Application Protocol
 *
 * @author      Locha Mesh Developers <developers@locha.io>
 * @}
 */


#ifndef NET_TFCOAP_H
#define NET_TFCOAP_H


#include "net/gcoap.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief init coap server
 */ 
void tf_coat_init(void);

/**
 * @brief  Adds link format params to resource list 
 */
ssize_t encode_link(const coap_resource_t *resource, char *buf,
                            size_t maxlen, coap_link_encoder_ctx_t *context);

/**
 * @brief  Response callback.
 */
ssize_t stats_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx);


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_VAINA_H */
