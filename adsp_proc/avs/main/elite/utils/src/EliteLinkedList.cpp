/*========================================================================
Copyright (c) 2017 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
========================================================================= */

/**
@file elite_linked_list.cpp

@brief A general purpose linked list.
*/

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/utils/src/EliteLinkedList.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/19/17   claguna Create files for use in enc/dec to support custom params
========================================================================== */

#include "EliteLinkedList.h"

/*===========================================================================
 * FUNCTION      elite_linked_list_node_create
 *
 * DESCRIPTION   Allocates a node and stores the data within.
 *               
 * DOCUMENTATION  EliteLinkedList.h
 ============================================================================*/
ADSPResult elite_linked_list_node_create(elite_linked_list_node_t** new_node_pp,
                                         void* data) {
    elite_linked_list_node_t *new_node_p = 
        (elite_linked_list_node_t *) qurt_elite_memory_malloc(sizeof(elite_linked_list_node_t), 
                                                              QURT_ELITE_HEAP_DEFAULT);
    if (NULL == new_node_p) {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
              "EliteLinkedList: Out of memory trying to malloc node. Malloc size: %lu.",
              sizeof(elite_linked_list_node_t));
        return ADSP_ENOMEMORY;
    }

    new_node_p->data = data;
    new_node_p->next = NULL;
    *new_node_pp = new_node_p;

    return ADSP_EOK;
}

/*===========================================================================
 * FUNCTION      elite_linked_list_node_delete
 *
 * DESCRIPTION   Deallocates this node and its data.
 *               
 * DOCUMENTATION  EliteLinkedList.h
 ============================================================================*/
void elite_linked_list_node_delete(elite_linked_list_node_t* node) {
    if(node != NULL) {
        qurt_elite_memory_free(node);
    }
}

/*===========================================================================
 * FUNCTION       elite_linked_list_append
 *
 * DESCRIPTION    Appends new_node to the back of the linked list.
 *
 * DOCUMENTATION  EliteLinkedList.h
 ============================================================================*/
elite_linked_list_node_t* elite_linked_list_append(elite_linked_list_node_t *head, 
                                                   elite_linked_list_node_t *new_node) {
    elite_linked_list_node_t *cur_node;

    // If linked list is empty, new_node becomes the head.
    if(head == NULL) {
        return new_node;
    }

    // Search for the current leaf.
    cur_node = head;
    while(cur_node->next != NULL) {
        cur_node = cur_node->next;
    }

    // New_node becomes the new leaf.
    cur_node->next = new_node;

    return head;
}

/*===========================================================================
 * FUNCTION       elite_linked_list_delete
 *
 * DESCRIPTION    Empties the linked list.
 *
 * DOCUMENTATION  EliteLinkedList.h
 ============================================================================*/
void elite_linked_list_delete(elite_linked_list_node_t *head) {
    elite_linked_list_node_t *cur_node = head;

    // Need to retain a pointer to the next node after deallocating the 
    // current node.
    elite_linked_list_node_t *next_node = NULL;

    while(cur_node != NULL) {
        next_node = cur_node->next;
        elite_linked_list_node_delete(cur_node);
        cur_node = next_node;
    }
}
