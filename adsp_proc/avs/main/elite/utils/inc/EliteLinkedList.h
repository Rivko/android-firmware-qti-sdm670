/*========================================================================
Copyright (c) 2017 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
========================================================================= */

/**
@file elite_linked_list.h

@brief A general-purpose linked list.
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/utils/inc/EliteLinkedList.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/19/17   claguna Create files for use in enc/dec to support custom params
==========================================================================*/

#ifndef ELITE_LINKED_LIST_H
#define ELITE_LINKED_LIST_H

#include "qurt_elite.h"

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

/*
 * The linked list node is publicly exposed to provide maximum flexibility and
 * minimum overhead.
 */
typedef struct elite_linked_list_node_t
{
	// The actual data. Memory allocation is handled by the client.
    void *data;

    // Pointer to next node. NULL if leaf.
    elite_linked_list_node_t *next;

} elite_linked_list_node_t;


/*===========================================================================
 * FUNCTION      elite_linked_list_node_create
 *
 * DESCRIPTION   Allocates a node and stores the data within. Data is not
 *               copied: only the pointer is copied.
 *               
 * @param[out]   new_node  The node to be created. Only needs to be declared;
 *                         memory allocation will happen within this function.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  ADSPResult  ADSP_EOK on success, ADSP_ENOMEMORY if memory
 *                           allocation fails.
 ============================================================================*/
ADSPResult elite_linked_list_node_create(elite_linked_list_node_t** new_node,
	                                     void* data);

/*===========================================================================
 * FUNCTION      elite_linked_list_node_delete
 *
 * DESCRIPTION   Deallocates this node and its data.
 *               
 * @param[in]    node  The node to delete.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  void
 ============================================================================*/
void elite_linked_list_node_delete(elite_linked_list_node_t* node);

/*===========================================================================
 * FUNCTION      elite_linked_list_append
 *
 * DESCRIPTION   Appends new_node to the back of the linked list. The client 
 *               must allocate memory for new_node and set its data before
 *               calling this function. 
 *               
 * @param[in]    head      The head of the linked list.
 * @param[in]    new_node  The node to append to the end of the linked list.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  elite_linked_list_node_t*  The new head of the linked list. In
 *               standard cases, head is returned. If head is NULL, new_node
 *               is returned. This simplifies code for appending to an empty
 *               linked list.
 ============================================================================*/
elite_linked_list_node_t* elite_linked_list_append(elite_linked_list_node_t *head, 
                                                   elite_linked_list_node_t *new_node);


/*===========================================================================
 * FUNCTION      elite_linked_list_delete
 *
 * DESCRIPTION   Empties the linked list. Deallocates all nodes in the linked
 *               list and their data in the process.
 *               
 * @param[in]    head  The head of the linked list.
 *
 * DEPENDENCIES  None
 *
 * RETURN VALUE  void
 ============================================================================*/
void elite_linked_list_delete(elite_linked_list_node_t *head);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // #ifndef ELITE_LINKED_LIST_H

