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
 * @file
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
 * @brief   Initialize CoAP server.
 */ 
void tf_coat_init(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_VAINA_H */
/** @} */
