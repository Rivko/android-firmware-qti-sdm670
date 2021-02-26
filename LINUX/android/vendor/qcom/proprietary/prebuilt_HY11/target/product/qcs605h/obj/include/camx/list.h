//-------------------------------------------------------------------------
// Copyright (c) 2015-2018 Qualcomm Technologies, Inc.
// All Rights Reserved.
// Confidential and Proprietary - Qualcomm Technologies, Inc.
//------------------------------------------------------------------------

#ifndef _LIST_H_
#define _LIST_H_
#ifdef __cplusplus
extern "C" {
#endif
/*========================================================================
                          LIST . H

*//** @file list.h
  Public utility functions for handling lists


*//*====================================================================== */

/*========================================================================
                             Edit History

$Header: //components/rel/titan.icp/1.0/external/utils/inc/list.h#4 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
08/01/06   rg     First cut
09/18/06   mc     Added More apis
12/01/07   ab     Qdsp6-coding conventions/Doxygen Style Comments

========================================================================== */

/*------------------------------------------------------------------------
 *       Include Files
 * ----------------------------------------------------------------------- */
#include "global_typedef.h"

/*------------------------------------------------------------------------
 *       Definitions and Constants
 * ----------------------------------------------------------------------- */

/*------------------------------------------------------------------------
 *       Type Declarations
 * ----------------------------------------------------------------------- */

/// Structure defining a doubly linked list
typedef struct ListNode_T {

    void *pData; ///< List Data ptr, User can have any data type for this
    struct ListNode_T *pNextNode; ///< Ptr to next node in the list
    struct ListNode_T *pPrevNode; ///< Ptr to prev node in the list
} ListNode;


/// Structure to keep the list head and tail handles
typedef struct {
    ListNode *pListHead; //Pointer to List Head
    ListNode *pListTail; //Pointer to List Tails
} ListHandle;

/*------------------------------------------------------------------------
 *       External Variables
 * ----------------------------------------------------------------------- */

/*------------------------------------------------------------------------
 *       Function Declarations
 * ----------------------------------------------------------------------- */


/********************Interface functions**************************/
/* Note that the interface functions used (void * ) for Node Data*/
/* The user must make sure to type-cast appropriately            */
/*****************************************************************/
/**
 * Function to check if the list is empty or not
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @return
 *   - TRUE if list is empty
 *   - FALSE if list is not emtpy
 */
UInt32 List_IsEmpty(const ListHandle *pListHandle);

/**
 * Function to initialize the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 */
void List_Init(ListHandle *pListHandle);

/**
 * Function to set the data pointer for a node
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 */
void List_SetNodeData(ListNode *pListNode,void *pPayLoad);

/**
 * Function to get the data pointer of a node
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @return
 *   - List data pointer
 */
void *List_GetNodeData(const ListNode *pListNode);

/**
 * Function to insert a node (already created) at the head of the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @param[in]  pListNode
 *              Pointer to the Node to push
 */
void List_InsertHead(ListHandle *pListHandle, ListNode *pListNode);

/**
 * Function to insert a node (already created) at the tail of the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @param[in]  pListNode
 *              Pointer to the Node to push
 */
void List_InsertTail(ListHandle *pListHandle, ListNode *pListNode);

/**
 * Function to insert a node (already created) after a specified node
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @param[in]  pNewNode
 *              Pointer to the Node to push
 * @param[in]  pRefNode
 *              Pointer to the Node after which the given node is to be
 *               pushed
 */
void List_InsertAfter(ListHandle *pListHandle,
                      ListNode *pNewNode,
                      ListNode *pRefNode);

/**
 * Function to insert a node (already created) before a specified node
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @param[in]  pNewNode
 *              Pointer to the Node to push
 * @param[in]  pRefNode
 *              Pointer to the Node before which the given node is to be
 *               pushed
 */
void List_InsertBefore(ListHandle *pListHandle,
                       ListNode *pNewNode,
                       ListNode *pRefNode);

/**
 * Function to remove a specified node from the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @param[in]  pListNode
 *              Pointer to the Node to remove
 */
void List_RemoveNode(ListHandle *pListHandle,
                     ListNode *pListNode);

/**
 * Function to remove the head node from the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @return
 *    Pointer to the removed head node
 */
ListNode *List_RemoveHead(ListHandle *pListHandle);

/**
 * Function to remove the tail node from the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @return
 *    Pointer to the removed head node
 */
ListNode *List_RemoveTail(ListHandle *pListHandle);

/**
 * Function to get a node (not pop). The list is linearly searched to based
 * on node data (payload) and the node pointer  is returned to the caller. Note
 * that the ideal use of this function is when the caller wants to delete a
 * particular node based on node data
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @param[in]  pData
 *              data pointer for the node to search for
 * @param[in]  pfnCompare
 *              Function to compare the node's data ptr in the list
 * @return
 *    Pointer to the node if found, else returns NULL pointer
 */
ListNode *List_SearchNode(ListHandle *pListHandle, void *pData,
                          Int32(*pfnCompare)(void *pCompVal,void *pNode));

/**
 * Function to get the head node of the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @return
 *    Pointer to the head node
 */
ListNode *List_GetHead(const ListHandle *pListHandle);

/**
 * Function to get the tail node of the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @return
 *    Pointer to the tail node
 */
ListNode *List_GetTail(const ListHandle *pListHandle);

/**
 * Function to read the next node from the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListNode
 *              Pointer to the List Node whose next node is required
 * @return
 *    Pointer to the next node in the list
 */
ListNode *List_GetNextNode(const ListNode *pListNode);

/**
 * Function to read the previous node from the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListNode
 *              Pointer to the List Node whose previous node is required
 * @return
 *    Pointer to the previous node in the list
 */
ListNode *List_GetPrevNode(const ListNode *pListNode);

/**
 * Function to get the size of the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @return
 *    Size of the list (number of nodes linked in the list)
 */
UInt32 List_GetSize(const ListHandle *pListHandle);

/**
 * Function to check if a given node is in the list
 *    - Dependencies: none
 *    - Side effects: none
 *    - Re-entrant
 *
 * @param[in]  pListHandle
 *              Pointer to the List Handle
 * @param[in]  pListNode
 *              Pointer to the List Node to check
 * @return
 *    TRUE if node found, else returns FALSE
 */
UInt32 List_CheckNode(const ListHandle *pListHandle,
                      const ListNode *pListNode);

#ifdef __cplusplus
}
#endif
#endif //_LIST_H_

