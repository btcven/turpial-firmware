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

#include "net/gcoap.h"

void tf_coat_init(void);

ssize_t encode_link(const coap_resource_t *resource, char *buf,
                            size_t maxlen, coap_link_encoder_ctx_t *context);

ssize_t stats_handler(coap_pkt_t* pdu, uint8_t *buf, size_t len, void *ctx);