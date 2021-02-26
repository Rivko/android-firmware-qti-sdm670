/*============================================================================
  FILE:         DALQueue.c

  OVERVIEW:     Provides Queue services used by the ADC DAL

  DEPENDENCIES: None
 
                Copyright (c) 2009 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.


  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2009-03-24  jdt  Rename functions for DAL
  2009-03-16  jdt  Branch from //depot/asic/msmshared/services/common/queue.c#17

============================================================================*/
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        Q U E U E    S E R V I C E S

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

  DALFW_Q_Init
    This function initializes a queue. It should be called on behalf of a
    queue prior to using the queue.

  DALFW_Q_Link
    This function initializes a link field. It should be called on behalf
    of a link field prior to using the link filed with the other Queue
    Services.

  DALFW_Q_Get
    This function removes the data block at head of a queue and returns a
    pointer to the data block. If the queue is empty, then a NULL pointer
    is returned.

  DALFW_Q_Check
    This function returns a pointer to the data block at the head of a queue.
    The data block is not removed from the queue. If the queue is empty, then
    a NULL pointer is returned.

  DALFW_Q_LastGet
    This function removes the data block at the tail of a queue and returns a
    pointer to the data block. If the queue is empty, then a NULL pointer
    is returned.

  DALFW_Q_Put
    This function places a data block at the tail of a queue.

  DALFW_Q_Cnt
    This function returns the number of items currently on a queue.

  DALFW_Q_LinearSearch
    Performs a linear search of the queue calling a users callback on each
    nodal visit to determine whether or not the search should be terminated.

  DALFW_Q_LinearDelete
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


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/06   sgh     Enabled mutex for WinMobile
01/17/06    ck     Fixed a compilation issue with a hanging brace for LTK builds
01/10/06    ck     Added function q_delete_ext that returns a boolean indicating
                   if the node was found and removed
05/20/05    ck     Fixed an issue with the function q_delete which was stuck in
                   an infinite loop when the node to be deleted was not in the
                   queue.
09/28/01    gr     Merged in changes from the MSP archive. Includes clean-up
                   of source code and comments, queue-specific mutual
                   exclusion for Win32 targets and the new function
                   q_linear_delete. Removed support for UTF_ASP.
01/25/01   day     Merged from MSM5105_COMMON.00.00.05.
                   Added support for MSM5000_IRAM_FWD.
                   Added support for UTF_ASP.
01/13/00   jct     Add q_linear_search
12/10/99   gr      Merged from MSM3000 Rel 4.0c.
                   Fixes to q_insert, q_delete.
                   Introduced q_last_get().
                   Changed type of queue count from word to int.
                   Added calls to Q_XCEPT macros in all queue functions.
04/09/99    sk     Comments changed.
04/06/99    sk     Introduced FEATURE_Q_NO_SELF_QPTR and FEATURE_Q_SINGLE_LINK
12/16/98   jct     Removed 80186 support
10/08/98   jct     Moved INTLOCK in q_put to beginning of function, added
                   setting link pointer to NULL in q_delete
09/24/98   jct     Update copyright
12/20/96   rdh     Removed from Boot Block, changed to Pascal call convention.
04/08/96   dna     Added comment to each func. saying it is in the Boot Block
02/28/96   dna     Prepended bb_ to all function names and put module in the
                   Boot Block.
04/23/92   ip      Initial porting of file from Brassboard to DMSS.
02/20/92   arh     Added q_check routine

===========================================================================*/

/* ------------------------------------------------------------------------
** Includes
** ------------------------------------------------------------------------ */

#include "DALQueue.h"


/*==========================================================================
FUNCTION DALFW_Q_Init

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
DALFW_Q_Type* DALFW_Q_Init(
   DALFW_Q_Type  *pQ  /* Ptr to queue to be initialized. */
)
{
   pQ->link.pNext = (DALFW_Q_LinkType *)(&pQ->link);  /* Points to q link. */
   pQ->link.pPrev = (DALFW_Q_LinkType *)(&pQ->link);  /* Points to q link. */
   pQ->nCnt = 0;

   return pQ;
} /* END DALFW_Q_Init */


/*===========================================================================
FUNCTION DALFW_Q_Link

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
DALFW_Q_LinkType* DALFW_Q_Link(
   void         *pItem,  /* Ptr to item or variable containing link. */
   DALFW_Q_LinkType  *pLink   /* Ptr to link field within variable. */
)
{
   pLink->pNext = NULL;

   return pLink;
} /* END DALFW_Q_Link */

/*===========================================================================
FUNCTION DALFW_Q_Put

DESCRIPTION
  This function enqueues an item onto a specified queue using a specified
  link.

DEPENDENCIES
  The specified queue should have been previously initialized via a call
  to DALFW_Q_Init. The specified link field of the item should have been prev-
  iously initialized via a call to DALFW_Q_Link.

RETURN VALUE
  None.

SIDE EFFECTS
  The specified item is placed at the tail of the specified queue.
===========================================================================*/
void DALFW_Q_Put(
   DALFW_Q_Type  *pQ,    /* Ptr to queue. */
   DALFW_Q_LinkType *pLink  /* Ptr to item link to use for queueing. */
)
{
   pLink->pNext = (DALFW_Q_LinkType *)&pQ->link;

   pQ->link.pPrev->pNext = pLink;
   pQ->link.pPrev = pLink;
   pQ->nCnt++;
} /* END DALFW_Q_Put */

/*===========================================================================
FUNCTION DALFW_Q_Get

DESCRIPTION
  This function removes an item from the head of a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to DALFW_Q_Init.

RETURN VALUE
  A pointer to the dequeued item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the specified queue.
===========================================================================*/
void* DALFW_Q_Get(
  DALFW_Q_Type  *pQ  /* Ptr to queue. */
)
{
   DALFW_Q_LinkType  *pLink;
   DALFW_Q_LinkType  *pRet = NULL;

   /* Get ptr to 1st queue item.
   */
   pLink = pQ->link.pNext;

   /* Can only get an item if the queue is non empty
   */
   if( pQ->nCnt > 0 )
   {
      pQ->link.pNext = pLink->pNext;

      if (pLink->pNext == (DALFW_Q_LinkType *)pQ)
      {
         pQ->link.pPrev = (DALFW_Q_LinkType *)(&pQ->link);
      }

      pQ->nCnt--;

      /* Mark item as no longer in a queue.
      */
      pLink->pNext = NULL;
      pRet = pLink;
   }

   return (void *)pRet;
} /* END DALFW_Q_Get */

/*===========================================================================
FUNCTION DALFW_Q_LastGet

DESCRIPTION
  This function returns the item which was most recently enqueued in a queue.

  Note, this is different from DALFW_Q_Get() which returns the oldest item in a
  queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void * DALFW_Q_LastGet(
   DALFW_Q_Type  *pQ
)
{
   DALFW_Q_LinkType  *pLink;
   DALFW_Q_LinkType  *pRet=NULL;

   for( pLink = (DALFW_Q_LinkType *)pQ;
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
      pLink->pNext = (DALFW_Q_LinkType *)(&pQ->link);
      pQ->nCnt--;
      pRet->pNext = NULL;
   }

   return (void *)pRet;
}  /* DALFW_Q_LastGet */

/*===========================================================================
FUNCTION DALFW_Q_Next

DESCRIPTION
  This function returns a pointer to the next item on the queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to DALFW_Q_Init.

RETURN VALUE
  A pointer to the next item on the queue. If the end of the queue is reached,
  then NULL is returned.

SIDE EFFECTS
  None.
===========================================================================*/
void* DALFW_Q_Next(
   DALFW_Q_Type  *pQ,
   DALFW_Q_LinkType  *pLink
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
} /* END DALFW_Q_Next */


/*===========================================================================
FUNCTION DALFW_Q_Insert

DESCRIPTION
  This function inserts an item before a specified item on a queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to DALFW_Q_Init.

RETURN VALUE
  None.

SIDE EFFECTS
  Input item is inserted before input item.
===========================================================================*/
void DALFW_Q_Insert(
   DALFW_Q_Type  *pQ,          /* Ptr to the queue */
   DALFW_Q_LinkType  *pLinkToInsert,   /* Ptr to link of item to insert */
   DALFW_Q_LinkType  *pPrev      /* Ptr to link item to insert before */
)
{
   DALFW_Q_LinkType  *pLink;

   pLinkToInsert->pNext = pPrev;

   /* Start at beginning of queue and find the item that will be before the
   ** new item
   */
   pLink = (DALFW_Q_LinkType *) pQ;

   while (pLink->pNext != pPrev)
   {
      pLink = pLink->pNext;
   }
   pLink->pNext = pLinkToInsert;

   pQ->nCnt++;
} /* END DALFW_Q_Insert */

/*===========================================================================
FUNCTION DALFW_Q_Delete

DESCRIPTION
  This function removes an item from a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to DALFW_Q_Init.

RETURN VALUE
  None.

SIDE EFFECTS
  Input item is delete from the queue.
===========================================================================*/
void DALFW_Q_Delete(
   DALFW_Q_Type  *pQ,     /* Ptr to the Queue */
   DALFW_Q_LinkType  *pLinkToDelete   /* Ptr to link of item to delete */
)
{
   DALFW_Q_LinkType *pLinkTemp;
   DALFW_Q_Type *pRealQ;
   int nCnt;

   pRealQ = pQ;

   for( nCnt = pQ->nCnt, pLinkTemp = (DALFW_Q_LinkType *) pRealQ;
     pLinkTemp->pNext != pLinkToDelete && nCnt > 0;
     pLinkTemp =  pLinkTemp->pNext, nCnt--)
   {
      ;
   }

   if(nCnt > 0)
   {
      pLinkTemp->pNext = pLinkToDelete->pNext;

      if(pLinkTemp->pNext == (DALFW_Q_LinkType *) pRealQ)
      {
         pRealQ->link.pPrev = pLinkTemp;
      }

      pQ->nCnt--;
      pLinkToDelete->pNext = NULL;
   }
} /* END DALFW_Q_Delete */

/*===========================================================================
FUNCTION DALFW_Q_DeleteExt

DESCRIPTION
  This function removes an item from a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to DALFW_Q_Init.

RETURN VALUE
  FALSE : if the item is not found in the queue.
  TRUE  : if the item is found and removed from the queue.

SIDE EFFECTS
  Input item is deleted from the queue.
===========================================================================*/
DALBOOL DALFW_Q_DeleteExt(
   DALFW_Q_Type  *pQ,     /* Ptr to the Queue */
   DALFW_Q_LinkType  *pLinkToDelete   /* Ptr to link of item to delete */
)
{
   DALFW_Q_LinkType *pLinkTemp;
   DALFW_Q_Type *pRealQ;
   int nCnt;
   DALBOOL bItemIsInQ = FALSE;

   pRealQ = pQ;

   for( nCnt = pQ->nCnt, pLinkTemp = (DALFW_Q_LinkType *) pRealQ;
      pLinkTemp->pNext != pLinkToDelete && nCnt > 0;
      pLinkTemp = pLinkTemp->pNext, nCnt--)
   {
      ;
   }

   if(nCnt > 0)
   {
      pLinkTemp->pNext = pLinkToDelete->pNext;

      if(pLinkTemp->pNext == (DALFW_Q_LinkType *) pRealQ)
      {
         pRealQ->link.pPrev = pLinkTemp;
      }

      pQ->nCnt--;
      pLinkToDelete->pNext = NULL;

      bItemIsInQ = TRUE;
   }

   return bItemIsInQ;
} /* END DALFW_Q_DeleteExt */

/*===========================================================================
FUNCTION DALFW_Q_Check

DESCRIPTION
  This function returns a pointer to the data block at the head of the queue.
  The data block is not removed from the queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to DALFW_Q_Init.

RETURN VALUE
  A pointer to the queue item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  None
===========================================================================*/
void* DALFW_Q_Check(
  DALFW_Q_Type  *pQ
)
{
   DALFW_Q_LinkType  *pLink;
   DALFW_Q_LinkType  *pRet = NULL;

   pLink = pQ->link.pNext;

   if( pQ->nCnt > 0 )
   {
      pRet = pLink;
   }

   return (void *)pRet;
} /* END DALFW_Q_Check */


/*===========================================================================

FUNCTION Q_LAST_CHECK

DESCRIPTION
  This function returns the item which was most recently enqueued in a queue.

  Note, this is different from DALFW_Q_Check() which returns the oldest item in a
  queue.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void * DALFW_Q_LastCheck
(
  DALFW_Q_Type  *pQ           /* The queue from which the item will be removed */
)
{
   DALFW_Q_LinkType  *pLink;                         /* For returning value. */
   DALFW_Q_LinkType  *pRet=NULL;                     /* For returning value. */

 /*- - - - - - - - - - - -  - - - - - - - - - - - - - - - - - - - - - - - -*/

   for( pLink = (DALFW_Q_LinkType *)pQ;
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
}  /* DALFW_Q_LastCheck */


/*===========================================================================
FUNCTION DALFW_Q_Cnt

DESCRIPTION
  This function returns the number of items currently queued on a specified
  queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to DALFW_Q_Init.

RETURN VALUE
  The number of items currently queued on the specified queue.

SIDE EFFECTS
  None.
===========================================================================*/
int DALFW_Q_Cnt(
  DALFW_Q_Type  *pQ
)
{
   return pQ->nCnt;
} /* END DALFW_Q_Cnt */


/*===========================================================================
FUNCTION DALFW_Q_LinearSearch

DESCRIPTION
  Given a comparison function, this function traverses the elements in
  a queue, calls the compare function, and returns a pointer to the
  current element being compared if the user passed compare function
  returns non zero.

  The user compare function should return 0 if the current element is
  not the element in which the compare function is interested.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to DALFW_Q_Init.

  The user's queue elements must have DALFW_Q_LinkType as the first element
  of the queued structure.

RETURN VALUE
  A pointer to the found element

SIDE EFFECTS
  None.
===========================================================================*/
void* DALFW_Q_LinearSearch(
   DALFW_Q_Type  *pQ,
   DALFW_Q_CompareFuncType compareFunc,
   void *pCompareVal
)
{
   DALFW_Q_GenericItemType *pItem = NULL;

   pItem = (DALFW_Q_GenericItemType*)DALFW_Q_Check( pQ );

   while( pItem != NULL )
   {
      if( compareFunc( pItem, pCompareVal ) != 0 )
      {
         return pItem;
      }
      pItem = (DALFW_Q_GenericItemType*)DALFW_Q_Next( pQ, &pItem->link );
   } /* END while traversing the queue */

   return NULL;
} /* END DALFW_Q_LinearSearch */


/*===========================================================================
FUNCTION DALFW_Q_LinearDelete

DESCRIPTION
  Given a comparison function, this function traverses the elements in
  a queue, calls the compare function, and returns a pointer to the
  current element being compared if the user passed compare function
  returns non zero.  In addition, the item will be removed from the queue.

  The user compare function should return 0 if the current element is
  not the element in which the compare function is interested.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to DALFW_Q_Init.

  The user's queue elements must have DALFW_Q_LinkType as the first element
  of the queued structure.

  The user's compare function will be passed NULL for the compare value.

RETURN VALUE
  None

SIDE EFFECTS
  None.
===========================================================================*/
void DALFW_Q_LinearDelete(
   DALFW_Q_Type  *pQ,
   DALFW_Q_CompareFuncType compareFunc,
   void *pParam,
   DALFW_Q_ActionFuncType  actionFunc
)
{
   DALFW_Q_GenericItemType *pItem = NULL;
      /* Used in the traversal to point to the current item
      */
   DALFW_Q_GenericItemType *pPrev = NULL;
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
   pItem = (DALFW_Q_GenericItemType*)DALFW_Q_Check( pQ );
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
            (void)DALFW_Q_Get( pQ );
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
      pItem = (DALFW_Q_GenericItemType*)DALFW_Q_Next( pQ, &pItem->link );
   } /* END while traversing the queue */
} /* END DALFW_Q_LinearDelete */

