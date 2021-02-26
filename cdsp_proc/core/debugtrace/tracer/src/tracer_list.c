/*===========================================================================
  FILE: tracer_list.c

  OVERVIEW:     QUALCOMM Debug Subsystem (QDSS) Tracer doubly-linked list
                utility

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/*===========================================================================
  EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/tracer/src/tracer_list.c#1 $$DateTime: 2018/07/30 01:21:36 $$Author: pwbldsvc $

  when       who what, where, why
  ---------- --- ------------------------------------------------------------
  2011-10-19 lht Initial revision.
===========================================================================*/

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "stringl/stringl.h"   // memset (not available Venus, LPASS)

#include "tracer_list.h"
#include "tracer_malloc.h"

/*-------------------------------------------------------------------------*/
// Double-linked list notes.
// Only head can have pPrev=NULL, only tail can have pNext=NULL.
// If not then linked list is broken and memory leaks likely to occur.
// Test cases: 0) Empty list, 1a) first and only element, 1b) first of 2
// plus elements, 2a) 2nd element of 3+ elements, 2b) 2nd to last of 3+
// elements 2c) middle element of 5 elements, 3) last element of 2 plus
// elements.

//---------------------------------------------------------------------------

/*-------------------------------------------------------------------------*/
void tracer_list_init(tracer_list_t *pList)
{
   if (NULL != pList)
   {
      pList->pHead = NULL;
      pList->nodeCount = 0;
   }
   return;
}

/*-------------------------------------------------------------------------*/
tracer_node_t *tracer_list_new_node(tracer_list_t *pList, size_t data_size)
{
   tracer_node_t *pNewNode;

   // Allocate resources for new node.
   pNewNode = (tracer_node_t *)_tracer_malloc(sizeof(tracer_node_t));
   if (NULL == pNewNode)
   {
      return NULL;
   }
   memset (pNewNode, 0, sizeof(tracer_node_t));

   // Allocate resources for new node's data.
   pNewNode->pData = (void *) _tracer_malloc(data_size);
   if (NULL == pNewNode->pData)
   {
      _tracer_free(pNewNode);
      return NULL;
   }
   memset (pNewNode->pData, 0, data_size);

   // Remember number of bytes allocated. Clear before freeing.
   pNewNode->dataSize = data_size;

   // Add to head of client list (link nodes; adjust list ptrs)
   pNewNode->pPrev = NULL;
   pNewNode->pNext = pList->pHead;
   if (NULL != pList->pHead)
   {
      pList->pHead->pPrev = pNewNode;
   }
   pList->pHead = pNewNode;
   pList->nodeCount++;  // Track number of nodes in list.

   return (pNewNode);
}

/*-------------------------------------------------------------------------*/
// Clears and deallocates the specified node's data and node.
void _tracer_list_node_free(tracer_node_handle_t pNode)
{
   if (NULL != pNode)
   {
      if (NULL != pNode->pData)
      {
         memset (pNode->pData, 0, pNode->dataSize);
         _tracer_free ((void*)(pNode->pData));
      }
      memset (pNode, 0, sizeof(tracer_node_t));
      _tracer_free ((void*)pNode);
       pNode = NULL;
   }
   return;
}

/*-------------------------------------------------------------------------*/
// Unlinks and frees all nodes in the list.
// Not used by tracer: Tracer needs to free stm port so uses
// tracer_unregister_all instead.

void tracer_list_clear(tracer_list_t *pList)
{
   if (NULL != pList)
   {
      while (NULL != pList->pHead->pNext)
      {  // All nodes except first.
         pList->pHead = pList->pHead->pNext;
         _tracer_list_node_free(pList->pHead->pPrev);
      }
      if (NULL != pList->pHead)
      {  // First node.
         _tracer_list_node_free(pList->pHead);
      }
      tracer_list_init(pList);
   }
   return;
}

/*-------------------------------------------------------------------------*/
int tracer_list_delete_node(tracer_list_t *pList, tracer_node_handle_t pNode)
{
   tracer_node_handle_t pTemp_node;
   if (NULL == pNode)
   {
      return 0; // Success.
   }
   // Verify pNode is in pList.
   pTemp_node = pList->pHead;
   while (pTemp_node != pNode && pTemp_node != NULL)
   {
      pTemp_node = pTemp_node->pNext;
   }
   if (pTemp_node == NULL)
   {  // Not in list.
      return 1;  // Failure.
   }

   // Remove the node.
   if (NULL == pNode->pPrev)
   {  // Removing head node
      pList->pHead = pNode->pNext;
   }
   else
   {
      pNode->pPrev->pNext = pNode->pNext;
   }
   if (NULL != pNode->pNext)
   {
      pNode->pNext->pPrev = pNode->pPrev;
   }
   _tracer_list_node_free(pNode);
   pList->nodeCount--;

   return 0;
}

