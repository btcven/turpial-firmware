/* Copyright 2020 btcven and Locha Mesh developers
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
 * @ingroup     main
 *
 * @{
 * @file
 * @brief       IPv6 address autogeneration
 *
 * @author      Locha Mesh Developers <developers@locha.io>
 */

#ifndef IPV6_AUTOGEN_H
#define IPV6_AUTOGEN_H

#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C"  {
#endif

/**
 * @brief   Generate an IPv6 address
 *
 * @experimental
 *
 * @param[out] addr The generated address.
 */
void turpial_autogen_ipv6(ipv6_addr_t *addr);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* IPV6_AUTOGEN_H */
/** @} */
