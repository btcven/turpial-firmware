
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

#ifndef _AVL_H
#define _AVL_H

#include <stddef.h>

#include "common/common_types.h"
#include "container_of.h"
#include "list.h"

/**
 * Static initializer for AVL tree
 * @param avl avl_tree variable (not pointer!)
 * @param avl_comp comparator to be used
 * @param dups true if duplicates are allowed, false otherwise
 */
#define AVL_STATIC_INIT(avl, avl_comp, dups)                                                                           \
  {                                                                                                                    \
    .oonf_list_head = { .next = &avl.oonf_list_head, .prev = &avl.oonf_list_head }, .root = NULL, .count = 0, .allow_dups = dups,     \
    .comp = avl_comp                                                                                                   \
  }

/**
 * This element is a member of a avl-tree. It must be contained in all
 * larger structs that should be put into a tree.
 */
struct avl_node {
  /**
   * Linked list node for supporting easy iteration and multiple
   * elments with the same key.
   *
   * this must be the first element of an avl_node to
   * make casting for lists easier
   */
  struct oonf_list_entity list;

  /**
   * Pointer to parent node in tree, NULL if root node
   */
  struct avl_node *parent;

  /**
   * Pointer to left child
   */
  struct avl_node *left;

  /**
   * Pointer to right child
   */
  struct avl_node *right;

  /**
   * pointer to key of node
   */
  const void *key;

  /**
   * balance state of AVL tree (0,-1,+1)
   */
  signed char balance;

  /**
   * true if this is an additional node with the same key
   * as the one before
   */
  bool follower;
};

/**
 * This struct is the central management part of an avl tree.
 * One of them is necessary for each avl_tree.
 */
struct avl_tree {
  /**
   * Head of linked list node for supporting easy iteration
   * and multiple elments with the same key.
   */
  struct oonf_list_entity oonf_list_head;

  /**
   * pointer to the root node of the avl tree, NULL if tree is empty
   */
  struct avl_node *root;

  /**
   * number of nodes in the avl tree
   */
  uint32_t count;

  /**
   * true if multiple nodes with the same key are
   * allowed in the tree, false otherwise
   */
  bool allow_dups;

  /**
   * Prototype for avl comparators
   * @param k1 first key
   * @param k2 second key
   * @return +1 if k1>k2, -1 if k1<k2, 0 if k1==k2
   */
  int (*comp)(const void *k1, const void *k2);
};

EXPORT void avl_init(struct avl_tree *, int (*comp)(const void *k1, const void *k2), bool);
EXPORT struct avl_node *avl_find(const struct avl_tree *, const void *);
EXPORT struct avl_node *avl_find_greaterequal(const struct avl_tree *tree, const void *key);
EXPORT struct avl_node *avl_find_lessequal(const struct avl_tree *tree, const void *key);
EXPORT int avl_insert(struct avl_tree *, struct avl_node *);
EXPORT void avl_remove(struct avl_tree *, struct avl_node *);

/**
 * @param tree pointer to avl-tree
 * @param node pointer to node of the tree
 * @return true if node is the first one of the tree, false otherwise
 */
static INLINE bool
avl_is_first(const struct avl_tree *tree, const struct avl_node *node) {
  return tree->oonf_list_head.next == &node->list;
}

/**
 * @param tree pointer to avl-tree
 * @param node pointer to node of the tree
 * @return true if node is the last one of the tree, false otherwise
 */
static INLINE bool
avl_is_last(const struct avl_tree *tree, const struct avl_node *node) {
  return tree->oonf_list_head.prev == &node->list;
}

/**
 * @param tree pointer to avl-tree
 * @return true if the tree is empty, false otherwise
 */
static INLINE bool
avl_is_empty(const struct avl_tree *tree) {
  return tree->count == 0;
}

/**
 * @param node pointer to avl node
 * @return true if node is currently in a tree, false otherwise
 */
static INLINE bool
avl_is_node_added(const struct avl_node *node) {
  return oonf_list_is_node_added(&node->list);
}

/**
 * @param tree pointer to avl-tree
 * @param key pointer to key
 * @param element pointer to a node element
 *    (don't need to be initialized)
 * @param node_element name of the avl_node element inside the
 *    larger struct
 * @return pointer to tree element with the specified key,
 *    NULL if no element was found
 */
#define avl_find_element(tree, key, element, node_element)                                                             \
  container_of_if_notnull(avl_find(tree, key), typeof(*(element)), node_element)

/**
 * @param tree pointer to avl-tree
 * @param key pointer to specified key
 * @param element pointer to a node element
 *    (don't need to be initialized)
 * @param node_element name of the avl_node element inside the
 *    larger struct
 * return pointer to last tree element with less or equal key than specified key,
 *    NULL if no element was found
 */
#define avl_find_le_element(tree, key, element, node_element)                                                          \
  container_of_if_notnull(avl_find_lessequal(tree, key), typeof(*(element)), node_element)

/**
 * @param tree pointer to avl-tree
 * @param key pointer to specified key
 * @param element pointer to a node element
 *    (don't need to be initialized)
 * @param node_element name of the avl_node element inside the
 *    larger struct
 * return pointer to first tree element with greater or equal key than specified key,
 *    NULL if no element was found
 */
#define avl_find_ge_element(tree, key, element, node_element)                                                          \
  container_of_if_notnull(avl_find_greaterequal(tree, key), typeof(*(element)), node_element)

/**
 * This function must not be called for an empty tree
 *
 * @param tree pointer to avl-tree
 * @param element pointer to a node element
 *    (don't need to be initialized)
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @return pointer to the first element of the avl_tree
 *    (automatically converted to type 'element')
 */
#define avl_first_element(tree, element, node_member)                                                                  \
  container_of((tree)->oonf_list_head.next, typeof(*(element)), node_member.list)

/**
 * @param tree pointer to avl-tree
 * @param element pointer to a node element
 *    (don't need to be initialized)
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @return pointer to the first element of the avl_tree
 *    (automatically converted to type 'element'),
 *    NULL if tree is empty
 */
#define avl_first_element_safe(tree, element, node_member)                                                             \
  (avl_is_empty(tree) ? NULL : avl_first_element(tree, element, node_member))

/**
 * This function must not be called for an empty tree
 *
 * @param tree pointer to tree
 * @param element pointer to a node struct that contains the avl_node
 *    (don't need to be initialized)
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @return pointer to the last element of the avl_tree
 *    (automatically converted to type 'element')
 */
#define avl_last_element(tree, element, node_member)                                                                   \
  container_of((tree)->oonf_list_head.prev, typeof(*(element)), node_member.list)

/**
 * @param tree pointer to tree
 * @param element pointer to a node struct that contains the avl_node
 *    (don't need to be initialized)
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @return pointer to the last element of the avl_tree
 *    (automatically converted to type 'element'),
 *    NULL if tree is empty
 */
#define avl_last_element_safe(tree, element, node_member)                                                              \
  (avl_is_empty(tree) ? NULL : avl_last_element(tree, element, node_member))

/**
 * This function must not be called for the last element of
 * an avl tree
 *
 * @param element pointer to a node of the tree
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @return pointer to the node after 'element'
 *    (automatically converted to type 'element')
 */
#define avl_next_element(element, node_member)                                                                         \
  container_of((&(element)->node_member.list)->next, typeof(*(element)), node_member.list)

/**
 * @param tree pointer to avl-tree
 * @param element pointer to a node of the tree
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @return pointer to the node after 'element'
 *    (automatically converted to type 'element'),
 *    NULL if there is no element after 'element' or
 *    'element' is NULL
 */
#define avl_next_element_safe(tree, element, node_member)                                                              \
  ((element) == NULL || avl_is_last(tree, &(element)->node_member) ? NULL : avl_next_element(element, node_member))

/**
 * This function must not be called for the first element of
 * an avl tree
 *
 * @param element pointer to a node of the tree
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @return pointer to the node before 'element'
 *    (automatically converted to type 'element')
 */
#define avl_prev_element(element, node_member)                                                                         \
  container_of((&(element)->node_member.list)->prev, typeof(*(element)), node_member.list)

/**
 * @param tree pointer to avl-tree
 * @param element pointer to a node of the tree
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @return pointer to the node before 'element'
 *    (automatically converted to type 'element'),
 *    NULL if there is no element before 'element' or
 *    'element' is NULL
 */
#define avl_prev_element_safe(tree, element, node_member)                                                              \
  ((element) == NULL || avl_is_first(tree, &(element)->node_member) ? NULL : avl_prev_element(element, node_member))

/**
 * Loop over a block of elements of a tree, used similar to a for() command.
 * This loop should not be used if elements are removed from the tree during
 * the loop.
 *
 * @param first pointer to first element of loop
 * @param last pointer to last element of loop
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the list during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 */
#define avl_for_element_range(first, last, element, node_member)                                                       \
  for (element = (first); element->node_member.list.prev != &(last)->node_member.list;                                 \
       element = avl_next_element(element, node_member))

/**
 * Loop over a block of elements of a tree backwards, used similar to a for() command.
 * This loop should not be used if elements are removed from the tree during
 * the loop.
 *
 * @param first pointer to first element of loop
 * @param last pointer to last element of loop
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the list during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 */
#define avl_for_element_range_reverse(first, last, element, node_member)                                               \
  for (element = (last); element->node_member.list.next != &(first)->node_member.list;                                 \
       element = avl_prev_element(element, node_member))

/**
 * Loop over all elements of an avl_tree, used similar to a for() command.
 * This loop should not be used if elements are removed from the tree during
 * the loop.
 *
 * @param tree pointer to avl-tree
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the tree during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 */
#define avl_for_each_element(tree, element, node_member)                                                               \
  avl_for_element_range(                                                                                               \
    avl_first_element(tree, element, node_member), avl_last_element(tree, element, node_member), element, node_member)

/**
 * Loop over all elements of an avl_tree backwards, used similar to a for() command.
 * This loop should not be used if elements are removed from the tree during
 * the loop.
 *
 * @param tree pointer to avl-tree
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the tree during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 */
#define avl_for_each_element_reverse(tree, element, node_member)                                                       \
  avl_for_element_range_reverse(                                                                                       \
    avl_first_element(tree, element, node_member), avl_last_element(tree, element, node_member), element, node_member)

/**
 * Loop over a block of elements of a tree, used similar to a for() command.
 * This loop should not be used if elements are removed from the tree during
 * the loop.
 * The loop runs from the element 'first' to the end of the tree.
 *
 * @param tree pointer to avl-tree
 * @param first pointer to first element of loop
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the list during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 */
#define avl_for_element_to_last(tree, first, element, node_member)                                                     \
  avl_for_element_range(first, avl_last_element(tree, element, node_member), element, node_member)

/**
 * Loop over a block of elements of a tree backwards, used similar to a for() command.
 * This loop should not be used if elements are removed from the tree during
 * the loop.
 * The loop runs from the element 'first' to the end of the tree.
 *
 * @param tree pointer to avl-tree
 * @param first pointer to first element of loop
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the list during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 */
#define avl_for_element_to_last_reverse(tree, first, element, node_member)                                             \
  avl_for_element_range_reverse(first, avl_last_element(tree, element, node_member), element, node_member)

/**
 * Loop over a block of elements of a tree, used similar to a for() command.
 * This loop should not be used if elements are removed from the tree during
 * the loop.
 * The loop runs from the start of the tree to the element 'last'.
 *
 * @param tree pointer to avl-tree
 * @param last pointer to last element of loop
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the list during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 */
#define avl_for_first_to_element(tree, last, element, node_member)                                                     \
  avl_for_element_range(avl_first_element(tree, element, node_member), last, element, node_member)

/**
 * Loop over a block of elements of a tree backwards, used similar to a for() command.
 * This loop should not be used if elements are removed from the tree during
 * the loop.
 * The loop runs from the start of the tree to the element 'last'.
 *
 * @param tree pointer to avl-tree
 * @param last pointer to last element of loop
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the list during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 */
#define avl_for_first_to_element_reverse(tree, last, element, node_member)                                             \
  avl_for_element_range_reverse(avl_first_element(tree, element, node_member), last, element, node_member)

/**
 * Loop over a block of elements of a tree with a certain key, used similar
 * to a for() command.
 * This loop should not be used if elements are removed from the tree during
 * the loop.
 *
 * @param tree pointer to avl-tree
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the list during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @param start helper pointer to remember start of iteration
 * @param key pointer to key
 */
#define avl_for_each_elements_with_key(tree, element, node_member, start, key)                                         \
  for (start = element = avl_find_element(tree, key, element, node_member);                                            \
       element != NULL && &element->node_member.list != &(tree)->oonf_list_head &&                                          \
       (element == start || element->node_member.follower);                                                            \
       element = avl_next_element(element, node_member))

/**
 * Loop over a block of nodes of a tree, used similar to a for() command.
 * This loop can be used if the current element might be removed from
 * the tree during the loop. Other elements should not be removed during
 * the loop.
 *
 * @param first_element first element of loop
 * @param last_element last element of loop
 * @param element iterator pointer to tree element struct
 * @param node_member name of avl_node within tree element struct
 * @param ptr pointer to tree element struct which is used to store
 *    the next node during the loop
 */
#define avl_for_element_range_safe(first_element, last_element, element, node_member, ptr)                             \
  for (element = (first_element), ptr = avl_next_element(element, node_member);                                        \
       element->node_member.list.prev != &(last_element)->node_member.list;                                            \
       element = ptr, ptr = avl_next_element(ptr, node_member))

/**
 * Loop over a block of elements of a tree backwards, used similar to a for() command.
 * This loop can be used if the current element might be removed from
 * the tree during the loop. Other elements should not be removed during
 * the loop.
 *
 * @param first_element first element of range (will be last returned by the loop)
 * @param last_element last element of range (will be first returned by the loop)
 * @param element iterator pointer to node element struct
 * @param node_member name of avl_node within node element struct
 * @param ptr pointer to node element struct which is used to store
 *    the previous node during the loop
 */
#define avl_for_element_range_reverse_safe(first_element, last_element, element, node_member, ptr)                     \
  for (element = (last_element), ptr = avl_prev_element(element, node_member);                                         \
       element->node_member.list.next != &(first_element)->node_member.list;                                           \
       element = ptr, ptr = avl_prev_element(ptr, node_member))

/**
 * Loop over all elements of an avl_tree, used similar to a for() command.
 * This loop can be used if the current element might be removed from
 * the tree during the loop. Other elements should not be removed during
 * the loop.
 *
 * @param tree pointer to avl-tree
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the tree during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @param ptr pointer to a tree element which is used to store
 *    the next node during the loop
 */
#define avl_for_each_element_safe(tree, element, node_member, ptr)                                                     \
  avl_for_element_range_safe(avl_first_element(tree, element, node_member),                                            \
    avl_last_element(tree, element, node_member), element, node_member, ptr)

/**
 * Loop over all elements of an avl_tree backwards, used similar to a for() command.
 * This loop can be used if the current element might be removed from
 * the tree during the loop. Other elements should not be removed during
 * the loop.
 *
 * @param tree pointer to avl-tree
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the tree during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @param ptr pointer to a tree element which is used to store
 *    the next node during the loop
 */
#define avl_for_each_element_reverse_safe(tree, element, node_member, ptr)                                             \
  avl_for_element_range_reverse_safe(avl_first_element(tree, element, node_member),                                    \
    avl_last_element(tree, element, node_member), element, node_member, ptr)

/**
 * A special loop that removes all elements of the tree and cleans up the tree
 * root. The loop body is responsible to free the node elements of the tree.
 *
 * This loop is faster than a normal one for clearing the tree because it
 * does not rebalance the tree after each removal. Do NOT use a break command
 * inside.
 * You can free the memory of the elements within the loop.
 * Do NOT call avl_delete() on the elements within the loop,
 *
 * @param tree pointer to avl-tree
 * @param element pointer to a node of the tree, this element will
 *    contain the current node of the tree during the loop
 * @param node_member name of the avl_node element inside the
 *    larger struct
 * @param ptr pointer to a tree element which is used to store
 *    the next node during the loop
 */
#define avl_remove_all_elements(tree, element, node_member, ptr)                                                       \
  for (element = avl_first_element(tree, element, node_member), ptr = avl_next_element(element, node_member),          \
      oonf_list_init_head(&(tree)->oonf_list_head), (tree)->root = NULL;                                                         \
       (tree)->count > 0; element = ptr, ptr = avl_next_element(ptr, node_member), (tree)->count--)

#endif /* _AVL_H */
