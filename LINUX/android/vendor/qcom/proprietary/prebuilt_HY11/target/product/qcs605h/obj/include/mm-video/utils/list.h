/**
 * @file      list.h
 * @brief     Doubly-linked list API.
 * @copyright
 *            Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
 *            All Rights Reserved.
 *            Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef _LIST_H_
#define _LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

#define offset_of(type, field) \
     &(((type *)0)->field)

#define list_get_node_data(ptr, type, field) \
    (type*)((char*) ptr - (char*)offset_of(type, field))

#define list_clear(type,entry,phead,member)  while (!list_is_empty(phead)) {\
                                      entry = list_get_node_data(list_get_head(phead), type, member); \
                                      list_remove_node(phead, &entry->member);\
                                      free(entry); }\

#define list_for_each_entry_safe(type, entry, pMap, field) \
    for( entry = list_get_node_data(pMap->p_next_node, type, field);\
         &entry->field != pMap ; \
         entry = list_get_node_data(list_get_next_node(&entry->field), type, field))

#define utils_list_for_each_entry(type, entry, pMap, field) \
    for( entry = list_get_node_data(list_get_head(pMap), type, field);\
         &entry->field != pMap ; \
         entry = list_get_node_data(list_get_next_node(&entry->field), type, field))

#define list_for_each_entry_reverse(type, entry, pMap, field) \
    for( entry = list_get_node_data(list_get_tail(pMap), type, field);\
         &entry->field != pMap ; \
         entry = list_get_node_data(list_get_prev_node(&entry->field), type, field))

/// doubly linked list node structure
typedef struct LIST_NODE_T {
    struct LIST_NODE_T *p_prev_node; ///< pointer to prev node in list
    struct LIST_NODE_T *p_next_node; ///< pointer to next node in list
} LIST_NODE;


/// function pointer for key value comparison
typedef int (PFN_COMPARE)(void *p_key_value, void *p_node);

/**
 * @brief Check if list is empty.
 *
 * @param[in] p_list_head: Pointer to list handle.
 *
 * @retval true  if empty.
 * @retval false if not empty.
 */
BOOL list_is_empty(LIST_NODE *p_list_head);

/**
 * @brief Initialize list.
 *
 * @param[in] p_list_head: Pointer to list handle.
 *
 * @retval void
 */
void list_init(LIST_NODE *p_list_head);

/**
 * @brief Insert a node (already created) at the head of the list.
 *
 * @param[in] p_list_head: Pointer to list handle.
 * @param[in] p_list_node:   Pointer to list node.
 *
 * @retval void
 */
void list_insert_head(LIST_NODE *p_list_node, LIST_NODE *p_list_head);

/**
 * @brief Insert a node (already created) at the tail of the list.
 *
 * @param[in] p_list_head: Pointer to list handle.
 * @param[in] p_list_node:   Pointer to list node.
 *
 * @retval void
 */
void list_insert_tail(LIST_NODE *p_list_node, LIST_NODE *p_list_head);

/**
 * @brief Insert a node (already created) after a specified node.
 *
 * @param[in] p_list_head: Pointer to list handle.
 * @param[in] p_list_node_new: Pointer to new list node.
 * @param[in] p_list_node_ref: Pointer to reference list node.
 *
 * @retval void
 */
void list_insert_after(LIST_NODE *p_list_head,
                       LIST_NODE   *p_list_node_new,
                       LIST_NODE   *p_list_node_ref);

/**
 * @brief Insert a node (already created) before a specified node.
 *
 * @param[in] p_list_head: Pointer to list handle.
 * @param[in] p_list_node_new: Pointer to new list node.
 * @param[in] p_list_node_ref: Pointer to reference list node.
 *
 * @retval void
 */
void list_insert_before(LIST_NODE *p_list_head,
                        LIST_NODE   *p_list_node_new,
                        LIST_NODE   *p_list_node_ref);

/**
 * @brief Remove the specified node from list.
 *
 * @param[in] p_list_head: Pointer to list handle.
 * @param[in] p_list_node:   Pointer to list node to remove.
 *
 * @retval void
 */
void list_remove_node(LIST_NODE *p_list_head, LIST_NODE *p_list_node);

/**
 * @brief Remove head node from list.
 *
 * @param[in] p_list_head: Pointer to list handle.
 *
 * @retval Pointer to removed head node.
 */
LIST_NODE *list_remove_head(LIST_NODE *p_list_head);

/**
 * @brief Remove tail node from list.
 *
 * @param[in] p_list_head: Pointer to list handle.
 *
 * @retval Pointer to removed tail node.
 */
LIST_NODE *list_remove_tail(LIST_NODE *p_list_head);

/**
 * @brief Get head node of list.
 *
 * @param[in] p_list_head: Pointer to list handle.
 *
 * @retval Pointer to head node.
 */
LIST_NODE *list_get_head(LIST_NODE *p_list_head);

/**
 * @brief Get tail node of list.
 *
 * @param[in] p_list_head: Pointer to list handle.
 *
 * @retval Pointer to tail node.
 */
LIST_NODE *list_get_tail(LIST_NODE *p_list_head);

/**
 * @brief Get specified list node's next node.
 *
 * @param[in] p_list_node: Pointer to list node.
 *
 * @retval Pointer to list node's next node.
 */
LIST_NODE *list_get_next_node(LIST_NODE *p_list_node);

/**
 * @brief Get specified list node's previous node.
 *
 * @param[in] p_list_node: Pointer to list node.
 *
 * @retval Pointer to list node's previous node.
 */
LIST_NODE *list_get_prev_node(LIST_NODE *p_list_node);

/**
 * @brief Get size of list.
 *
 * @param[in] p_list_head: Pointer to list handle.
 *
 * @retval Number of nodes in list.
 */
unsigned int list_get_size(LIST_NODE *p_list_head);

/**
 * @brief Check if node is present in list.
 *
 * @param[in] p_list_head: Pointer to list handle.
 * @param[in] p_list_node:   Pointer to list node to be checked for.
 *
 * @retval true  if list node found.
 * @retval false if list node not found.
 */
BOOL list_check_node(LIST_NODE *p_list_head, LIST_NODE *p_list_node);

#ifdef __cplusplus
} // closing brace for: extern "C" {
#endif

#endif // #ifndef _LIST_H_
