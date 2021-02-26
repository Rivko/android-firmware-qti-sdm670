/*
 * osal_list.h
 *
 * This header file defines linked list operations helper functions for GCS.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 *
 * Not a Contribution.
 * Apache license notifications and license are retained
 * for attribution purposes only.
 *
 * Copyright (C) 2008-2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __OSAL_LIST_H__
#define __OSAL_LIST_H__

/*
 * osal_get_list_item - get a container item of given type, from a list node
 *
 * @ptr:    the osal_list_t pointer.
 * @type:   the type of the item where the list is embedded in.
 * @node:   the name of the osal_list_t struct within the item.
 *
 * Given the list node pointer, get the container item of given type.
 */
#define osal_get_list_item(ptr, type, node) ((type *)( \
    (char *)(ptr)- \
    (unsigned long)(&((type *)0)->node)))

struct list_node {
    struct list_node *prev;
    struct list_node *next;
};

typedef struct list_node osal_list_t;

/*
 * osal_init_list - Initialize a linked list
 *
 * @list:    linked list to be initialized
 *
 * Prerequisites
 *    None
 */
static inline void osal_init_list(osal_list_t *list) {
    list->prev = list;
    list->next = list;
}

/*
 * osal_list_add_head - Adds the new node after the specific list head
 *
 * @new_node:    element to be added
 * @list:        list head after which the new node is to be added after
 *
 * Prerequisites
 *    list must be created
 */
static inline void osal_list_add_head(osal_list_t *new_node,
                                      osal_list_t *list) {
    new_node->next = list->next;
    list->next = new_node;
    new_node->prev = list;
    list->next->prev = new_node;
}

/*
 * osal_list_add_tail - Adds a new node as the last element of the list
 *
 * @new_node:    element to be added
 * @list:        list head before which the new node is to be added
 *
 * Prerequisites
 *    list must be created
 */
static inline void osal_list_add_tail(osal_list_t *new_node,
                                      osal_list_t *list) {
    new_node->next = list;
    list->prev->next = new_node;
    new_node->prev = list->prev;
    list->prev = new_node;
}

/*
 * osal_list_del - Delete entry from the list and set prev/next to NULL
 *
 * @entry:     element to be deleted
 *
 * Prerequisites
 *    list must be created
 */
static inline void osal_list_del(osal_list_t *entry) {
    entry->prev->next = entry->next;
    entry->next->prev = entry->prev;
    entry->prev = NULL;
    entry->next = NULL;
}

/*
 * osal_list_is_empty - Tests whether @list is empty
 *
 * @list:    the head of the list to test
 *
 * Prerequisites
 *    list must be initialized
 */
#define osal_list_is_empty(list) ((list)->next == list)

/*
 * osal_list_for_each_entry - Iterate over list of given type
 *
 * @type:    type of elements in the list
 * @pos:     the @type * to use as a loop cursor.
 * @list:    the list head for list to iterate.
 * @node:    the name of the list_node struct within the struct.
 *
 * Prerequisites
 *    list must be created
 */
#define osal_list_for_each_entry(type, pos, list, node)            \
    for (pos = osal_get_list_item((list)->next, type, node);       \
         &pos->node != (list);                                     \
         pos = osal_get_list_item(pos->node.next, type, node))

/*
 * osal_list_for_each_entry_safe - Iterate over list of given type safe
 *                  against removal of list entry
 *
 * @type:    type of elements in the list
 * @pos:     the @type * to use as a loop cursor.
 * @n:       another @type * to use as a loop cursor.
 * @list:    the list head for list to iterate.
 * @node:    the name of the list_node struct within the struct.
 *
 * Prerequisites
 *    list must be created
 */
#define osal_list_for_each_entry_safe(type, pos, n, list, node)     \
    for (pos = osal_get_list_item((list)->next, type, node),        \
         n = osal_get_list_item(pos->node.next, type, node);        \
         &pos->node != (list);                                      \
         pos = n, n = osal_get_list_item(n->node.next, type, node))

/*
 * osal_list_for_each_entry_from - Iterate over list of given type from
 *                                the current point
 * @pos:     the type * to use as a loop cursor.
 * @list:    the head for your list.
 * @node:    the name of the list_struct within the struct.
 *
 * Prerequisites
 *    list must be created
 */
#define osal_list_for_each_entry_from(pos, list, node)                 \
    for (; &pos->node != (list);                                       \
         pos = osal_get_list_item(pos->node.next, typeof(*pos), node))

#endif /* __OSAL_LIST_H__ */
