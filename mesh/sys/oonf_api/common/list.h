
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

#ifndef LIST_H_
#define LIST_H_

#include <stddef.h>

#include "common/common_types.h"
#include "common/container_of.h"

#ifdef RIOT_VERSION
#include "kernel_defines.h"
#endif

/**
 * this struct is used as list head and list elements.
 * the list-nodes and the list-head contain two rings of
 * pointers (one forward, one backward), the first/last node
 * have a link to the head, no NULL element.
 */
struct oonf_list_entity {
  /*! pointer to next element in list or to list head if last element */
  struct oonf_list_entity *next;

  /*! Pointer to previous element in list or list head if first element */
  struct oonf_list_entity *prev;
};

/**
 * initialize a list-head
 * @param head to list-head
 */
static INLINE void
oonf_list_init_head(struct oonf_list_entity *head) {
  head->next = head->prev = head;
}

/**
 * initialize a list-node
 * @param entity to list-node
 */
static INLINE void
oonf_list_init_node(struct oonf_list_entity *entity) {
  entity->next = entity->prev = NULL;
}

/**
 * internal function to add a new node between two other nodes.
 * @param prev node before the insertion point
 * @param next node after the insertion point
 * @param new node which will be added to the list between 'prev' and 'next'
 */
static INLINE void
__oonf_list_add(struct oonf_list_entity *prev, struct oonf_list_entity *next, struct oonf_list_entity *new) {
  new->next = next;
  new->prev = prev;
  next->prev = new;
  prev->next = new;
}

/**
 * adds a node at the beginning of a list
 * @param head pointer to list head
 * @param new node which will be added to the list
 */
static INLINE void
oonf_list_add_head(struct oonf_list_entity *head, struct oonf_list_entity *new) {
  __oonf_list_add(head, head->next, new);
}

/**
 * adds a node at the end of a list
 * @param head pointer to list head
 * @param new node which will be added to the list
 */
static INLINE void
oonf_list_add_tail(struct oonf_list_entity *head, struct oonf_list_entity *new) {
  __oonf_list_add(head->prev, head, new);
}

/**
 * adds a node before another node
 * @param before reference node in the list
 * @param new node which will be added to the list
 */
static INLINE void
oonf_list_add_before(struct oonf_list_entity *before, struct oonf_list_entity *new) {
  __oonf_list_add(before->prev, before, new);
}

/**
 * adds a node after another node
 * @param after reference node in the list
 * @param new node which will be added to the list
 */
static INLINE void
oonf_list_add_after(struct oonf_list_entity *after, struct oonf_list_entity *new) {
  __oonf_list_add(after, after->next, new);
}

/**
 * internal function that removes the nodes between two other nodes.
 * @param prev node before the removed part of the list
 * @param next node after the removed part of the list
 */
static INLINE void
__oonf_list_remove(struct oonf_list_entity *prev, struct oonf_list_entity *next) {
  prev->next = next;
  next->prev = prev;
}

/**
 * removes a node from a list and clears node pointers
 * @param entity node to remove from the list
 */
static INLINE void
oonf_list_remove(struct oonf_list_entity *entity) {
  __oonf_list_remove(entity->prev, entity->next);
  oonf_list_init_node(entity);
}

/**
 * checks if list is empty
 * @param head pointer to list head
 * @return true if list is empty, false otherwise
 */
static INLINE bool
oonf_list_is_empty(const struct oonf_list_entity *head) {
  return head->next == head && head->prev == head;
}

/**
 * checks if node has been added to a list
 * @param node pointer to node
 * @return true if both pointers of the node are initialized,
 *   false otherwise
 */
static INLINE bool
oonf_list_is_node_added(const struct oonf_list_entity *node) {
  return node->next != NULL && node->prev != NULL;
}

/**
 * checks if a node is the first element of a list
 * @param head pointer to list head
 * @param entity pointer to node
 * @return true if node is first element of list, false otherwise
 */
static INLINE bool
oonf_list_is_first(const struct oonf_list_entity *head, const struct oonf_list_entity *entity) {
  return head->next == entity;
}

/**
 * checks if node is the last element of a list
 * @param head pointer to list head
 * @param entity pointer to node
 * @return true if node is last element of list, false otherwise
 */
static INLINE bool
oonf_list_is_last(const struct oonf_list_entity *head, const struct oonf_list_entity *entity) {
  return head->prev == entity;
}

/**
 * Merge two lists and clear the second head
 * @param add_to head of the list which will contain all elements
 * @param remove_from head of the list which elements will be added after the elements
 *   of the first one
 */
static INLINE void
oonf_list_merge(struct oonf_list_entity *add_to, struct oonf_list_entity *remove_from) {
  if (oonf_list_is_empty(remove_from)) {
    return;
  }

  add_to->prev->next = remove_from->next;
  remove_from->next->prev = add_to->prev;

  add_to->prev = remove_from->prev;
  remove_from->prev->next = add_to;

  oonf_list_init_head(remove_from);
}

/**
 * @param head pointer to list-head
 * @param element pointer to a node element
 *    (don't need to be initialized)
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 * @return pointer to the first element of the list
 *    (automatically converted to type 'element')
 */
#define oonf_list_first_element(head, element, oonf_list_member) container_of((head)->next, typeof(*(element)), oonf_list_member)

/**
 * @param head pointer to list-head
 * @param element pointer to a node element
 *    (don't need to be initialized)
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 * @return pointer to the last element of the list
 *    (automatically converted to type 'element')
 */
#define oonf_list_last_element(head, element, oonf_list_member) container_of((head)->prev, typeof(*(element)), oonf_list_member)

/**
 * This function must not be called for the last element of
 * a list
 *
 * @param element pointer to a node of the list
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 * @return pointer to the node after 'element'
 *    (automatically converted to type 'element')
 */
#define oonf_list_next_element(element, oonf_list_member)                                                                        \
  container_of((&(element)->oonf_list_member)->next, typeof(*(element)), oonf_list_member)

/**
 * This function must not be called for the first element of
 * a list
 *
 * @param element pointer to a node of the list
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 * @return pointer to the node before 'element'
 *    (automatically converted to type 'element')
 */
#define oonf_list_prev_element(element, oonf_list_member)                                                                        \
  container_of((&(element)->oonf_list_member)->prev, typeof(*(element)), oonf_list_member)

/**
 * Loop over a block of elements of a list, used similar to a for() command.
 * This loop should not be used if elements are removed from the list during
 * the loop.
 *
 * @param first_element first element of loop
 * @param last_element last element of loop
 * @param element iterator pointer to list element struct
 * @param oonf_list_member name of oonf_list_entity within list element struct
 */
#define oonf_list_for_element_range(first_element, last_element, element, oonf_list_member)                                      \
  for (element = (first_element); element->oonf_list_member.prev != &(last_element)->oonf_list_member;                           \
       element = oonf_list_next_element(element, oonf_list_member))

/**
 * Loop over a block of elements of a list backwards, used similar to a for() command.
 * This loop should not be used if elements are removed from the list during
 * the loop.
 *
 * @param first_element first element of range (will be last returned by the loop)
 * @param last_element last element of range (will be first returned by the loop)
 * @param element iterator pointer to list element struct
 * @param oonf_list_member name of oonf_list_entity within list element struct
 */
#define oonf_list_for_element_range_reverse(first_element, last_element, element, oonf_list_member)                              \
  for (element = (last_element); element->oonf_list_member.next != &(first_element)->oonf_list_member;                           \
       element = oonf_list_prev_element(element, oonf_list_member))

/**
 * Loop over all elements of a list, used similar to a for() command.
 * This loop should not be used if elements are removed from the list during
 * the loop.
 *
 * @param head pointer to list-head
 * @param element pointer to a node of the list, this element will
 *    contain the current node of the list during the loop
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 */
#define oonf_list_for_each_element(head, element, oonf_list_member)                                                              \
  oonf_list_for_element_range(oonf_list_first_element(head, element, oonf_list_member),                                               \
    oonf_list_last_element(head, element, oonf_list_member), element, oonf_list_member)

/**
 * Loop over all elements of a list backwards, used similar to a for() command.
 * This loop should not be used if elements are removed from the list during
 * the loop.
 *
 * @param head pointer to list-head
 * @param element pointer to a node of the list, this element will
 *    contain the current node of the list during the loop
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 */
#define oonf_list_for_each_element_reverse(head, element, oonf_list_member)                                                      \
  oonf_list_for_element_range_reverse(oonf_list_first_element(head, element, oonf_list_member),                                       \
    oonf_list_last_element(head, element, oonf_list_member), element, oonf_list_member)

/**
 * Loop over a block of elements of a list, used similar to a for() command.
 * This loop should not be used if elements are removed from the list during
 * the loop.
 * The loop runs from the element 'first' to the end of the list.
 *
 * @param head pointer to head of list
 * @param first pointer to first element of loop
 * @param element pointer to a node of the list, this element will
 *    contain the current node of the list during the loop
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 */
#define oonf_list_for_element_to_last(head, first, element, oonf_list_member)                                                    \
  oonf_list_for_element_range(first, oonf_list_last_element(head, element, oonf_list_member), element, oonf_list_member)

/**
 * Loop over a block of elements of a list backwards, used similar to a for() command.
 * This loop should not be used if elements are removed from the list during
 * the loop.
 * The loop runs from the end of the list to the element 'first'.
 *
 * @param head pointer to head of list
 * @param first pointer to first element of loop
 * @param element pointer to a node of the list, this element will
 *    contain the current node of the list during the loop
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 */
#define oonf_list_for_element_to_last_reverse(head, first, element, oonf_list_member)                                            \
  oonf_list_for_element_range_reverse(first, oonf_list_last_element(head, element, oonf_list_member), element, oonf_list_member)

/**
 * Loop over a block of elements of a list, used similar to a for() command.
 * This loop should not be used if elements are removed from the list during
 * the loop.
 * The loop runs from the start of the list to the element 'last'.
 *
 * @param head pointer to head of list
 * @param last pointer to last element of loop
 * @param element pointer to a node of the list, this element will
 *    contain the current node of the list during the loop
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 */
#define oonf_list_for_first_to_element(head, last, element, oonf_list_member)                                                    \
  oonf_list_for_element_range(oonf_list_first_element(head, element, oonf_list_member), last, element, oonf_list_member)

/**
 * Loop over a block of elements of a list backwards, used similar to a for() command.
 * This loop should not be used if elements are removed from the list during
 * the loop.
 * The loop runs from the element 'last' to the start of the list.
 *
 * @param head pointer to head of list
 * @param last pointer to last element of loop
 * @param element pointer to a node of the list, this element will
 *    contain the current node of the list during the loop
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 */
#define oonf_list_for_first_to_element_reverse(head, last, element, oonf_list_member)                                            \
  oonf_list_for_element_range_reverse(oonf_list_first_element(head, element, oonf_list_member), last, element, oonf_list_member)

/**
 * Loop over a block of elements of a list, used similar to a for() command.
 * This loop can be used if the current element might be removed from
 * the list during the loop. Other elements should not be removed during
 * the loop.
 *
 * @param first_element first element of loop
 * @param last_element last element of loop
 * @param element iterator pointer to list element struct
 * @param oonf_list_member name of oonf_list_entity within list element struct
 * @param ptr pointer to list element struct which is used to store
 *    the next node during the loop
 */
#define oonf_list_for_element_range_safe(first_element, last_element, element, oonf_list_member, ptr)                            \
  for (element = (first_element), ptr = oonf_list_next_element(element, oonf_list_member);                                       \
       element->oonf_list_member.prev != &(last_element)->oonf_list_member;                                                      \
       element = ptr, ptr = oonf_list_next_element(ptr, oonf_list_member))

/**
 * Loop over a block of elements of a list backwards, used similar to a for() command.
 * This loop can be used if the current element might be removed from
 * the list during the loop. Other elements should not be removed during
 * the loop.
 *
 * @param first_element first element of range (will be last returned by the loop)
 * @param last_element last element of range (will be first returned by the loop)
 * @param element iterator pointer to list element struct
 * @param oonf_list_member name of oonf_list_entity within list element struct
 * @param ptr pointer to list element struct which is used to store
 *    the previous node during the loop
 */
#define oonf_list_for_element_range_reverse_safe(first_element, last_element, element, oonf_list_member, ptr)                    \
  for (element = (last_element), ptr = oonf_list_prev_element(element, oonf_list_member);                                        \
       element->oonf_list_member.next != &(first_element)->oonf_list_member;                                                     \
       element = ptr, ptr = oonf_list_prev_element(ptr, oonf_list_member))

/**
 * Loop over all elements of a list, used similar to a for() command.
 * This loop can be used if the current element might be removed from
 * the list during the loop. Other elements should not be removed during
 * the loop.
 *
 * @param head pointer to list-head
 * @param element pointer to a node of the list, this element will
 *    contain the current node of the list during the loop
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 * @param ptr pointer to an list element struct which is used to store
 *    the next node during the loop
 */
#define oonf_list_for_each_element_safe(head, element, oonf_list_member, ptr)                                                    \
  oonf_list_for_element_range_safe(oonf_list_first_element(head, element, oonf_list_member),                                          \
    oonf_list_last_element(head, element, oonf_list_member), element, oonf_list_member, ptr)

/**
 * Loop over all elements of a list backwards, used similar to a for() command.
 * This loop can be used if the current element might be removed from
 * the list during the loop. Other elements should not be removed during
 * the loop.
 *
 * @param head pointer to list-head
 * @param element pointer to a node of the list, this element will
 *    contain the current node of the list during the loop
 * @param oonf_list_member name of the oonf_list_entity element inside the
 *    larger struct
 * @param ptr pointer to an list element struct which is used to store
 *    the next node during the loop
 */
#define oonf_list_for_each_element_reverse_safe(head, element, oonf_list_member, ptr)                                            \
  oonf_list_for_element_range_reverse_safe(oonf_list_first_element(head, element, oonf_list_member),                                  \
    oonf_list_last_element(head, element, oonf_list_member), element, oonf_list_member, ptr)

#endif /* LIST_H_ */
