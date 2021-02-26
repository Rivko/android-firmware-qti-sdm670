/** @file FwLinkedList.h
   
  Linked list data structure and functions for  firmware update solution.

  Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when         who         what, where, why
 ----------    ---         --------------------------------------------------
 2015/04/15    mic        Added 64bit firmware update support
 2012/09/21    mic        Initial version

=============================================================================*/

#ifndef __QCOM_FW_LINKEDLIST_H_
#define __QCOM_FW_LINKEDLIST_H_

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>

typedef struct _FW_LIST_NODE
{  
    VOID                 *Data;    
    struct _FW_LIST_NODE *Next;  
} FW_LIST_NODE_TYPE;

typedef
EFI_STATUS
(EFIAPI *CB_NODE_DESTRUCTOR)(
    IN VOID *Data
  );

typedef struct _FW_LINKED_LIST
{
    UINTN            Count;
    FW_LIST_NODE_TYPE *Head;
    CB_NODE_DESTRUCTOR  NodeDestructor;
} FW_LINKED_LIST_TYPE;


/**
 * Function to register a destructor callback function
 * 
 * @param List         - Linked List
 * @param cbDestructor - destructor callback function
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
LinkedListRegisterDestructorCb(
  IN FW_LINKED_LIST_TYPE *List, 
  IN CB_NODE_DESTRUCTOR    cbDestructor
  );


/**
 * . Function to destruct a node 
 * 
 * @param List           - Linked List
 * @param NodeDestructor - Node Destructor
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
LinkedListRegisterNodeDestructor(
  IN FW_LINKED_LIST_TYPE *List, 
  IN CB_NODE_DESTRUCTOR NodeDestructor
  );

/**
 * . Function to add a node to linkedlist
 * 
 * @param List       - Linked List
 * @param Data       - Data
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
LinkedListAddNode(
  IN FW_LINKED_LIST_TYPE *List, 
  IN VOID                *Data
  );

/**
 * . Function to get the head node 
 * 
 * @param List       - Linked List
 * @param Node       - head node
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
LinkedListGetHeadNode(
  IN  FW_LINKED_LIST_TYPE *List,
  OUT FW_LIST_NODE_TYPE   **Node
  );

/**
 * . Function to delete the linked list 
 * 
 * @param List       - Linked List
 * @return : EFI_STATUS
 */
EFI_STATUS 
LinkedListDeleteAllNodes(
  IN FW_LINKED_LIST_TYPE *List
  );

/**
 * . Function to get the node count in linked list 
 * 
 * @param List       - Linked list
 * @param Count      - Total node count
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
LinkedListGetNodeCount(
  IN FW_LINKED_LIST_TYPE *List, 
  OUT UINTN             *Count
  );


#endif 
