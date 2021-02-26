/** @file FwLinkedList.c
   
  Linked list data structure and functions for  firmware update solution.

  Copyright (c) 2012-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/
/*=============================================================================
                              EDIT HISTORY
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.



 when          who         what, where, why
 ----------    ---         --------------------------------------------------
 2015/04/15    mic        Added 64bit firmware update support
 2012/09/21    mic        Initial version

=============================================================================*/

#include <Library/FwCommonLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>

#include "FwLinkedList.h"

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
  )
{
  if((List == NULL) || (cbDestructor == NULL))
  { 
    return EFI_INVALID_PARAMETER;
  }

  List->NodeDestructor = cbDestructor;
  return EFI_SUCCESS;
};


/**
 * Function to add a node to the head of a linkedlist
 * 
 * @param List       - Linked List
 * @param Data       - the data pointer, the data pointer must be allocated by 
 *                     the caller, and the linked list will delete the memory 
 *                     during destruction of the list. Therefore the memory can
 *                     not be a global variable, ect and must be allocated.
 *
 * @return : EFI_STATUS
 */
EFI_STATUS 
LinkedListAddNode(
  IN FW_LINKED_LIST_TYPE *List, 
  IN VOID                *Data
  )
{
  EFI_STATUS        Status = EFI_SUCCESS;
  FW_LIST_NODE_TYPE *pNew  = NULL;

  if ((List == NULL) || (Data == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->AllocatePool(EfiBootServicesData, sizeof(FW_LIST_NODE_TYPE),(VOID**)&pNew);
  HANDLE_ERROR_LABEL(Status,gBS->AllocatePool,LinkedListAddNodeExit);

  ZeroMem(pNew,sizeof(FW_LIST_NODE_TYPE));

  pNew->Data = Data;
  pNew->Next = List->Head;
  List->Head = pNew;
  List->Count++;
  return EFI_SUCCESS;

LinkedListAddNodeExit:
  if(pNew != NULL)
  {
    gBS->FreePool(pNew);
    pNew = NULL;
  }
  return Status;
}

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
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (List == NULL || Node == NULL) 
  {
    return EFI_INVALID_PARAMETER;
  }
  
  *Node = List->Head;
  
  return Status;
}

/**
 * . Function to delete all linked list nodes
 * 
 * @param List       - Linked List
 * @return : EFI_STATUS
 */
EFI_STATUS 
LinkedListDeleteAllNodes(
  IN FW_LINKED_LIST_TYPE *List
  )
{
  EFI_STATUS        Status      = EFI_SUCCESS;
  FW_LIST_NODE_TYPE *pCurrent   = NULL;

  CB_NODE_DESTRUCTOR cbDataDestructor = NULL;
  

  if(List == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }

  if(List->NodeDestructor == NULL)
  {
    return EFI_INVALID_PARAMETER;
  }
  else
  {
    cbDataDestructor = List->NodeDestructor;
  }
  
  while (List->Head != NULL)
  {
    pCurrent = List->Head;
    List->Head = List->Head->Next;

    if(pCurrent->Data != NULL)
    {
      Status = cbDataDestructor(pCurrent->Data);
      ERR_HANDLE(Status);
      pCurrent->Data = NULL;
    }

    Status = gBS->FreePool(pCurrent);
    ERR_HANDLE(Status);
    pCurrent = NULL;

    List->Count--;
  }
  
  return Status;
}

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
  OUT UINTN              *Count
  )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if ( (List == NULL) || (Count == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *Count = List->Count;
  return Status;
}



