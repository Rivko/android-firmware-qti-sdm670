/*============================================================================
  FILE:         coreutils.c

  OVERVIEW:     Provides core utils for adsppm 

  DEPENDENCIES: None
 
             
* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.

============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/qdsp_pm_mgr/resource_mgr/coreUtils.c#1 $$DateTime: 2018/07/30 01:21:36 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  20012-11-29 alisonc   create the file

============================================================================*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Core QUEUE    S E R V I C E S

GENERAL DESCRIPTION

  A queue is a simple data structure used for logically storing and re-
  trieving data blocks, in a first in - first out (FIFO) order without
  physically copying them. Software tasks and interrupt handlers can use
  queues for cleanly transferring data blocks to one another.

                        +-------+     +-------+     +-------+
                        | DATA  |     | DATA  |     | DATA  |
                        | BLOCK |     | BLOCK |     | BLOCK |
          +-------+     +-------+     +-------+     +-------+
          | QUEUE |---->| LINK  |---->| LINK  |---->| LINK  |---- |
   |----->+-------+     +-------+     +-------+     +-------+     |
   |      |       |     |       |     |       |     |       |     |
   |      +-------+     |       |     |       |     |       |     |
   |                    +-------+     +-------+     +-------+     |
   |                                                              |
   ---------------------------------------------------------------|

  The Queue Services provide a small set of declarations and functions for
  defining and initializing a queue, defining and initializing a links with-
  in a data block, placing a data block at the tail of a queue, removing a
  data block from the head of a queue, and removing a data block from any
  position in a queue.

  Aside from requiring each data block to contain a link, the Queue Services
  impose no restrictions on the size of structure of data blocks used with
  with queues. This allows software to pass virtually any type of data on
  queues. Notice that a data block may contain multiple links allowing it to
  be placed simultaneously on multiple queues.


EXTERNALIZED FUNCTIONS

  coreUtils_Q_Init
    This function initializes a queue. It should be called on behalf of a
    queue prior to using the queue.

  coreUtils_Q_Link
    This function initializes a link field. It should be called on behalf
    of a link field prior to using the link filed with the other Queue
    Services.

  coreUtils_Q_Get
    This function removes the data block at head of a queue and returns a
    pointer to the data block. If the queue is empty, then a NULL pointer
    is returned.

  coreUtils_Q_Check
    This function returns a pointer to the data block at the head of a queue.
    The data block is not removed from the queue. If the queue is empty, then
    a NULL pointer is returned.

  coreUtils_Q_LastGet
    This function removes the data block at the tail of a queue and returns a
    pointer to the data block. If the queue is empty, then a NULL pointer
    is returned.

  coreUtils_Q_Put
    This function places a data block at the tail of a queue.

  coreUtils_Q_Cnt
    This function returns the number of items currently on a queue.

  coreUtils_Q_LinearSearch
    Performs a linear search of the queue calling a users callback on each
    nocore visit to determine whether or not the search should be terminated.

  coreUtils_Q_LinearDelete
    Performs a linear traversal of the queue calling a compare callback on
    each node, should the compare callback indicate that the item should be
    deleted the item is removed from the queue and the users action callback
    is called passing to the callback the pointer to the now deleted item

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Prior to use, a queue must be initialized by calling q_init. Similarly,
  a link must be initialized prior to use by calling q_link.

Copyright (c) 1990-1992 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Copyright (c) 1996-1999 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Copyright (c) 2000,2001 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
                              Edit History

$Header: //components/rel/core.qdsp6/2.1.c4/power/qdsp_pm/src/driver/qdsp_pm_mgr/resource_mgr/coreUtils.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/29   alisonc     init file

===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */

#include "coreUtils.h"

/*==========================================================================
FUNCTION coreUtils_Q_Init

DESCRIPTION
  This function initializes a specified queue. It should be called for each
  queue prior to using the queue with the other Queue Services.

DEPENDENCIES
  None.

RETURN VALUE
  A pointer to the initialized queue.

SIDE EFFECTS
  The specified queue is initialized for use with Queue Services.
===========================================================================*/
coreUtils_Q_Type*  coreUtils_Q_Init(
   coreUtils_Q_Type  *pQ  /* Ptr to queue to be initialized. */
)
{
   pQ->link.pNext = (coreUtils_Q_LinkType *)(&pQ->link);  /* Points to q link. */
   pQ->link.pPrev = (coreUtils_Q_LinkType *)(&pQ->link);  /* Points to q link. */
   pQ->nCnt = 0;
   return pQ;
} /* END coreUtils_Q_Init */


/*===========================================================================
FUNCTION coreUtils_Q_Link

DESCRIPTION
  This function initializes a specified link. It should be called for each
  link prior to using the link with the other Queue Services.

DEPENDENCIES
  None.

RETURN VALUE
  A pointer to the initialized link.

SIDE EFFECTS
  The specified link is initialized for use with the Queue Services.
===========================================================================*/
coreUtils_Q_LinkType* coreUtils_Q_Link(
   void *pItem,
   coreUtils_Q_LinkType  *pLink   /* Ptr to link field within variable. */
)
{
   pLink->pNext = NULL;
   return pLink;
} /* END coreUtils_Q_Link */

/*===========================================================================
FUNCTION coreUtils_Q_Put

DESCRIPTION
  This function enqueues an item onto a specified queue using a specified
  link.

DEPENDENCIES
  The specified queue should have been previously initialized via a call
  to coreUtils_Q_Init. The specified link field of the item should have been prev-
  iously initialized via a call to coreUtils_Q_Link.

RETURN VALUE
  None.

SIDE EFFECTS
  The specified item is placed at the tail of the specified queue.
===========================================================================*/
void coreUtils_Q_Put(
   coreUtils_Q_Type  *pQ,    /* Ptr to queue. */
   coreUtils_Q_LinkType *pLink  /* Ptr to item link to use for queueing. */
)
{
   pLink->pNext = (coreUtils_Q_LinkType *)&pQ->link;

   pQ->link.pPrev->pNext = pLink;
   pQ->link.pPrev = pLink;
   pQ->nCnt++;
} /* END coreUtils_Q_Put */

/*===========================================================================
FUNCTION coreUtils_Q_Get

DESCRIPTION
  This function removes an item from the head of a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to coreUtils_Q_Init.

RETURN VALUE
  A pointer to the dequeued item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the specified queue.
===========================================================================*/
void* coreUtils_Q_Get(
  coreUtils_Q_Type  *pQ  /* Ptr to queue. */
)
{
   coreUtils_Q_LinkType  *pLink;
   coreUtils_Q_LinkType  *pRet = NULL;

   /* Get ptr to 1st queue item.
   */
   pLink = pQ->link.pNext;

   /* Can only get an item if the queue is non empty
   */
   if( pQ->nCnt > 0 )
   {
      pQ->link.pNext = pLink->pNext;

      if (pLink->pNext == (coreUtils_Q_LinkType *)pQ)
      {
         pQ->link.pPrev = (coreUtils_Q_LinkType *)(&pQ->link);
      }

      pQ->nCnt--;

      /* Mark item as no longer in a queue.
      */
      pLink->pNext = NULL;
      pRet = pLink;
   }

   return (void *)pRet;
} /* END coreUtils_Q_Get */

/*===========================================================================
FUNCTION coreUtils_Q_LastGet

DESCRIPTION
  This function returns the item which was most recently enqueued in a queue.

  Note, this is different from coreUtils_Q_Get() which returns the oldest item in a
  queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void * coreUtils_Q_LastGet(
   coreUtils_Q_Type  *pQ
)
{
   coreUtils_Q_LinkType  *pLink;
   coreUtils_Q_LinkType  *pRet=NULL;

   for( pLink = (coreUtils_Q_LinkType *)pQ;
      pLink->pNext != pQ->link.pPrev;
      pLink = pLink->pNext
   )
   {
      ;
   }

   if( pQ->nCnt > 0 )
   {
      pRet = pLink->pNext;
      pQ->link.pPrev = pLink;
      pLink->pNext = (coreUtils_Q_LinkType *)(&pQ->link);
      pQ->nCnt--;
      pRet->pNext = NULL;
   }

   return (void *)pRet;
}  /* coreUtils_Q_LastGet */

/*===========================================================================
FUNCTION coreUtils_Q_Next

DESCRIPTION
  This function returns a pointer to the next item on the queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to coreUtils_Q_Init.

RETURN VALUE
  A pointer to the next item on the queue. If the end of the queue is reached,
  then NULL is returned.

SIDE EFFECTS
  None.
===========================================================================*/
void* coreUtils_Q_Next(
   coreUtils_Q_Type  *pQ,
   coreUtils_Q_LinkType  *pLink
)
{
   if( (void *) pLink->pNext != (void *) pQ )
   {
      return pLink->pNext;
   }
   else
   {
      return NULL;
   }
} /* END coreUtils_Q_Next */


/*===========================================================================
FUNCTION coreUtils_Q_Insert

DESCRIPTION
  This function inserts an item before a specified item on a queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to coreUtils_Q_Init.

RETURN VALUE
  None.

SIDE EFFECTS
  Input item is inserted before input item.
===========================================================================*/
void coreUtils_Q_Insert(
   coreUtils_Q_Type  *pQ,          /* Ptr to the queue */
   coreUtils_Q_LinkType  *pLinkToInsert,   /* Ptr to link of item to insert */
   coreUtils_Q_LinkType  *pPrev      /* Ptr to link item to insert before */
)
{
   coreUtils_Q_LinkType  *pLink;

   pLinkToInsert->pNext = pPrev;

   /* Start at beginning of queue and find the item that will be before the
   ** new item
   */
   pLink = (coreUtils_Q_LinkType *) pQ;

   while (pLink->pNext != pPrev)
   {
      pLink = pLink->pNext;
   }
   pLink->pNext = pLinkToInsert;

   pQ->nCnt++;
} /* END coreUtils_Q_Insert */

/*===========================================================================
FUNCTION coreUtils_Q_Delete

DESCRIPTION
  This function removes an item from a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to coreUtils_Q_Init.

RETURN VALUE
  None.

SIDE EFFECTS
  Input item is delete from the queue.
===========================================================================*/
void coreUtils_Q_Delete(
   coreUtils_Q_Type  *pQ,     /* Ptr to the Queue */
   coreUtils_Q_LinkType  *pLinkToDelete   /* Ptr to link of item to delete */
)
{
   coreUtils_Q_LinkType *pLinkTemp;
   coreUtils_Q_Type *pRealQ;
   int nCnt;

   pRealQ = pQ;

   for( nCnt = pQ->nCnt, pLinkTemp = (coreUtils_Q_LinkType *) pRealQ;
     pLinkTemp->pNext != pLinkToDelete && nCnt > 0;
     pLinkTemp =  pLinkTemp->pNext, nCnt--)
   {
      ;
   }

   if(nCnt > 0)
   {
      pLinkTemp->pNext = pLinkToDelete->pNext;

      if(pLinkTemp->pNext == (coreUtils_Q_LinkType *) pRealQ)
      {
         pRealQ->link.pPrev = pLinkTemp;
      }

      pQ->nCnt--;
      pLinkToDelete->pNext = NULL;
   }
} /* END coreUtils_Q_Delete */

/*===========================================================================
FUNCTION coreUtils_Q_DeleteExt

DESCRIPTION
  This function removes an item from a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to coreUtils_Q_Init.

RETURN VALUE
  FALSE : if the item is not found in the queue.
  TRUE  : if the item is found and removed from the queue.

SIDE EFFECTS
  Input item is deleted from the queue.
===========================================================================*/
boolean coreUtils_Q_DeleteExt(
   coreUtils_Q_Type  *pQ,     /* Ptr to the Queue */
   coreUtils_Q_LinkType  *pLinkToDelete   /* Ptr to link of item to delete */
)
{
   coreUtils_Q_LinkType *pLinkTemp;
   coreUtils_Q_Type *pRealQ;
   int nCnt;
   boolean bItemIsInQ = FALSE;

   pRealQ = pQ;

   for( nCnt = pQ->nCnt, pLinkTemp = (coreUtils_Q_LinkType *) pRealQ;
      pLinkTemp->pNext != pLinkToDelete && nCnt > 0;
      pLinkTemp = pLinkTemp->pNext, nCnt--)
   {
      ;
   }

   if(nCnt > 0)
   {
      pLinkTemp->pNext = pLinkToDelete->pNext;

      if(pLinkTemp->pNext == (coreUtils_Q_LinkType *) pRealQ)
      {
         pRealQ->link.pPrev = pLinkTemp;
      }

      pQ->nCnt--;
      pLinkToDelete->pNext = NULL;

      bItemIsInQ = TRUE;
   }

   return bItemIsInQ;
} /* END coreUtils_Q_DeleteExt */

/*===========================================================================
FUNCTION coreUtils_Q_Check

DESCRIPTION
  This function returns a pointer to the data block at the head of the queue.
  The data block is not removed from the queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to coreUtils_Q_Init.

RETURN VALUE
  A pointer to the queue item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  None
===========================================================================*/
void* coreUtils_Q_Check(
  coreUtils_Q_Type  *pQ
)
{
   coreUtils_Q_LinkType  *pLink;
   coreUtils_Q_LinkType  *pRet = NULL;

   pLink = pQ->link.pNext;

   if( pQ->nCnt > 0 )
   {
      pRet = pLink;
   }

   return (void *)pRet;
} /* END coreUtils_Q_Check */


/*===========================================================================

FUNCTION Q_LAST_CHECK

DESCRIPTION
  This function returns the item which was most recently enqueued in a queue.

  Note, this is different from coreUtils_Q_Check() which returns the oldest item in a
  queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void * coreUtils_Q_LastCheck
(
  coreUtils_Q_Type  *pQ           /* The queue from which the item will be removed */
)
{
   coreUtils_Q_LinkType  *pLink;                         /* For returning value. */
   coreUtils_Q_LinkType  *pRet=NULL;                     /* For returning value. */

 /*- - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - -*/

   for( pLink = (coreUtils_Q_LinkType *)pQ;
       pLink->pNext != pQ->link.pPrev;
       pLink = pLink->pNext)
   {
      ;
   }

   if (pQ->nCnt > 0)
   {
      pRet = pLink->pNext;
      pLink = pRet;
   }

   return  (void *)pRet;
}  /* coreUtils_Q_LastCheck */


/*===========================================================================
FUNCTION coreUtils_Q_Cnt

DESCRIPTION
  This function returns the number of items currently queued on a specified
  queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to coreUtils_Q_Init.

RETURN VALUE
  The number of items currently queued on the specified queue.

SIDE EFFECTS
  None.
===========================================================================*/
int coreUtils_Q_Cnt(
  coreUtils_Q_Type  *pQ
)
{
   return pQ->nCnt;
} /* END coreUtils_Q_Cnt */


/*===========================================================================
FUNCTION coreUtils_Q_LinearSearch

DESCRIPTION
  Given a comparison function, this function traverses the elements in
  a queue, calls the compare function, and returns a pointer to the
  current element being compared if the user passed compare function
  returns non zero.

  The user compare function should return 0 if the current element is
  not the element in which the compare function is interested.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to coreUtils_Q_Init.

  The user's queue elements must have coreUtils_Q_LinkType as the first element
  of the queued structure.

RETURN VALUE
  A pointer to the found element

SIDE EFFECTS
  None.
===========================================================================*/
void* coreUtils_Q_LinearSearch(
   coreUtils_Q_Type  *pQ,
   coreUtils_Q_CompareFuncType compareFunc,
   void *pCompareVal
)
{
   coreUtils_Q_GenericItemType *pItem = NULL;

   pItem = (coreUtils_Q_GenericItemType*)coreUtils_Q_Check( pQ );

   while( pItem != NULL )
   {
      if( compareFunc( pItem, pCompareVal ) != 0 )
      {
         return pItem;
      }
      pItem = (coreUtils_Q_GenericItemType*)coreUtils_Q_Next( pQ, &pItem->link );
   } /* END while traversing the queue */

   return NULL;
} /* END coreUtils_Q_LinearSearch */


/*===========================================================================
FUNCTION coreUtils_Q_LinearDelete

DESCRIPTION
  Given a comparison function, this function traverses the elements in
  a queue, calls the compare function, and returns a pointer to the
  current element being compared if the user passed compare function
  returns non zero.  In addition, the item will be removed from the queue.

  The user compare function should return 0 if the current element is
  not the element in which the compare function is interested.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to coreUtils_Q_Init.

  The user's queue elements must have coreUtils_Q_LinkType as the first element
  of the queued structure.

  The user's compare function will be passed NULL for the compare value.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void coreUtils_Q_LinearDelete(
   coreUtils_Q_Type  *pQ,
   coreUtils_Q_CompareFuncType compareFunc,
   void *pParam,
   coreUtils_Q_ActionFuncType  actionFunc
)
{
   coreUtils_Q_GenericItemType *pItem = NULL;
      /* Used in the traversal to point to the current item
      */
   coreUtils_Q_GenericItemType *pPrev = NULL;
      /* Used in the traversal to point to the item previous to
      ** the current item.  This makes removing the current item
      ** a constant time operation
      */

   /* User must provide a compare function, otherwise, this is
   ** meaningless.
   */
   if( compareFunc == NULL )
   {
      return;
   }

   /* pItem points to the first item on the list
   */
   pItem = (coreUtils_Q_GenericItemType*)coreUtils_Q_Check( pQ );
   pPrev = NULL;

   while( pItem != NULL )
   {
      if( compareFunc( pItem, NULL ) != 0 )
      {
         /* Remove the item
         */
         if( pPrev != NULL )
         {
            /* Remove from the middle or tail
            */
            pPrev->link.pNext = pItem->link.pNext;
            pItem->link.pNext = NULL;
            pQ->nCnt--;
         }
         else
         {
            /* Remove from the head
            */
            (void)coreUtils_Q_Get( pQ );
         }

         /* Call the action function if there is one
         */
         if( actionFunc )
         {
            actionFunc( pItem, pParam );
         }
         break;
      }

      /* Move on to the next item
      */
      pPrev = pItem;
      pItem = (coreUtils_Q_GenericItemType*)coreUtils_Q_Next( pQ, &pItem->link );
   } /* END while traversing the queue */
} /* END coreUtils_Q_LinearDelete */

