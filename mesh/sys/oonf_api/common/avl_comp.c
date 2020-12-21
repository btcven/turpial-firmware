
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

#include <string.h>
#include <strings.h>

#include "common/avl_comp.h"
#include "common/string.h"

/**
 * AVL tree comparator for unsigned 32 bit integers
 * @param k1 pointer to key 1
 * @param k2 pointer to key 2
 * @return +1 if k1>k2, -1 if k1<k2, 0 if k1==k2
 */
int
avl_comp_uint32(const void *k1, const void *k2) {
  const uint32_t *u1 = k1;
  const uint32_t *u2 = k2;

  if (*u1 > *u2) {
    return 1;
  }
  if (*u2 > *u1) {
    return -1;
  }
  return 0;
}

/**
 * AVL tree comparator for signed 32 bit integers
 * @param k1 pointer to key 1
 * @param k2 pointer to key 2
 * @return +1 if k1>k2, -1 if k1<k2, 0 if k1==k2
 */
int
avl_comp_int32(const void *k1, const void *k2) {
  const int32_t *i1 = k1;
  const int32_t *i2 = k2;

  if (*i1 > *i2) {
    return 1;
  }
  if (*i2 > *i1) {
    return -1;
  }
  return 0;
}

/**
 * AVL tree comparator for unsigned 16 bit integers
 * @param k1 pointer to key 1
 * @param k2 pointer to key 2
 * @return +1 if k1>k2, -1 if k1<k2, 0 if k1==k2
 */
int
avl_comp_uint16(const void *k1, const void *k2) {
  const uint16_t *u1 = k1;
  const uint16_t *u2 = k2;

  if (*u1 > *u2) {
    return 1;
  }
  if (*u2 > *u1) {
    return -1;
  }
  return 0;
}

/**
 * AVL tree comparator for unsigned 8 bit integers
 * @param k1 pointer to key 1
 * @param k2 pointer to key 2
 * @return +1 if k1>k2, -1 if k1<k2, 0 if k1==k2
 */
int
avl_comp_uint8(const void *k1, const void *k2) {
  const uint8_t *u1 = k1;
  const uint8_t *u2 = k2;

  if (*u1 > *u2) {
    return 1;
  }
  if (*u2 > *u1) {
    return -1;
  }
  return 0;
}

/**
 * AVL tree comparator for netaddr objects.
 * @param k1 pointer to key 1
 * @param k2 pointer to key 2
 * @return +1 if k1>k2, -1 if k1<k2, 0 if k1==k2
 */
int
avl_comp_netaddr(const void *k1, const void *k2) {
  const struct netaddr *n1 = k1;
  const struct netaddr *n2 = k2;
  return memcmp(n1, n2, sizeof(struct netaddr));
}

/**
 * AVL tree comparator for netaddr objects.
 * @param k1 pointer to key 1
 * @param k2 pointer to key 2
 * @return +1 if k1>k2, -1 if k1<k2, 0 if k1==k2
 */
int
avl_comp_netaddr_socket(const void *k1, const void *k2) {
  const union netaddr_socket *s1 = k1;
  const union netaddr_socket *s2 = k2;
  return memcmp(s1, s2, sizeof(union netaddr_socket));
}

/**
 * AVL tree comparator for case insensitive strings.
 * @param txt1 pointer to string 1
 * @param txt2 pointer to string 2
 * @return +1 if k1>k2, -1 if k1<k2, 0 if k1==k2
 */
int
avl_comp_strcasecmp(const void *txt1, const void *txt2) {
  return strcasecmp(txt1, txt2);
}
