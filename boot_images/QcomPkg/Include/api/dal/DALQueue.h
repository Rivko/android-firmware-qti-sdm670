#ifndef DALQueue_H
#define DALQueue_H
/*===========================================================================
   Copyright (c) 1990 - 2001, 2014 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary

            Q U E U E    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
 This file contains types and declarations associated with the Queue
 Services.



===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/06   sgh     Enabled mutex for WinMobile
01/10/06    ck     Added prototype for q_delete_ext function that returns a 
                   boolean
05/16/02    gr     Added macro Q_ALREADY_QUEUED.
09/28/01    gr     Merged in changes from the MSP archive. Includes
                   queue-specific mutual exclusion for Win32 targets and the
                   new function q_linear_delete. Removed support for UTF_ASP.
01/25/01   day     Merged from MSM5105_COMMON.00.00.05.
                   Added support for MSM5000_IRAM_FWD.
                   Added support for UTF_ASP.
01/13/00   jct     Add q_linear_search
12/10/99   gr      Merged from the MSM3000 Rel 4.0c.
                   Re-arranged data in link structures to fix bugs that were
                   occurring when FEATURE_Q_SINGLE_LINK was defined but
                   FEATURE_Q_NO_SELF_QPTR was not.
                   Introduced q_last_get().
                   Added extern "C" to allow this file to export the queue
                   routines to a C++ program.  Changed use of word to int to 
                   count the number of elements in a queue.  Upgraded comments
                   to include function headers.  Removed the inclusion of
                   comdef.h since it is no longer needed.
                   Added macros to support renaming of queue functions and
                   to intercept calls to queue functions. 
04/09/99    sk     Comments changed.
04/06/99    sk     Introduced FEATURE_Q_NO_SELF_QPTR and FEATURE_Q_SINGLE_LINK
12/16/98   jct     Removed 80186 support
09/24/98   jct     Updated copyright
12/20/96   rdh     Changed to Pascal call convention.
02/08/96   jca     Added missing #include.
04/22/92   ip      Initial porting of file from Brassboard to DMSS.
02/20/92   arh     Added new function q_check.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "DALStdDef.h"
#include "DALFramework.h"

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* -------------------------------------------------------------------------
** Queue Link Structure
**
** The following link structure is really the heart of the Queue Services. 
** It is used as a link field with variables which allows them to be moved
** on and off queues. It is also used in the definition of queues themselves.
**
** Do NOT directly access the fields of a link! Only the Queue Services should
** do this.
** ------------------------------------------------------------------------- */
typedef struct DALFW_Q_LinkStruct
{
  struct DALFW_Q_LinkStruct  *pNext;
    /* Ptr to next link in list. If NULL, there is no next link. */
} DALFW_Q_LinkType;

/* ------------------------------------------------------------------------
** Queue Head Link Structure
**
** When queue items are linked in a singly link list, DALFW_Q_HeadLinkType is
** used instead of DALFW_Q_LinkType. This avoids the overhead of traversing 
** the whole of link list when queueing at the end of the link list.
** This structure should be accessed only by Queue services.
** ------------------------------------------------------------------------ */
typedef struct DALFW_Q_HeadLinkStruct
{
  struct DALFW_Q_LinkStruct  *pNext;
    /* Ptr to head of the link list */

  struct DALFW_Q_LinkStruct  *pPrev;
    /* Ptr to the tail of link list */

} DALFW_Q_HeadLinkType;


/* ------------------------------------------------------------------------
** Queue Structure
**
** The following structure format is used by the Queue Services to represent
** a queue.
** 
** Do NOT access the fields of a queue directly. Only the Queue Services should
** do this.
** ------------------------------------------------------------------------ */
typedef struct DALFW_Q_Struct
{
  DALFW_Q_HeadLinkType link;
    /* Used for linking items into queue. */

  int nCnt;
    /* Keeps track of number of items enqueued. Though not necessary, having
       this field is tremendously helpful for debugging. */
} DALFW_Q_Type;

/* ------------------------------------------------------------------------
** Queue Generic Item
**   Generic items must have DALFW_Q_LinkType as the first element.  This allows
**   the linear search function to traverse the list without knowing
**   anything about the elements
** ------------------------------------------------------------------------ */
typedef struct {
   DALFW_Q_LinkType link;
} DALFW_Q_GenericItemType;

/* ------------------------------------------------------------------------
** Queue Compare Function
**    Used by the searching functions to determine if an item is in
**       the queue.
**    Returns non zero if the element should be operated upon, 0 otherwise
**    For linear searching, the operation is to return a pointer to the
**       item and terminate the search
**    For linear deleting, the operation is to remove the item from the
**       queue and continue the traversal
** ------------------------------------------------------------------------ */
typedef int (*DALFW_Q_CompareFuncType)(void* pItem, void *pCompareVal);

/* ------------------------------------------------------------------------
** Queue Action Function
**    Used by DALFW_Q_LinearDelete to perform an action on an item which is 
**    being deleted from the list AFTER the item is deleted.  To perform
**    an action BEFORE the item is deleted, the user can call the action
**    function directly in the compare function call back.
** ------------------------------------------------------------------------ */
typedef void (*DALFW_Q_ActionFuncType)(void *pItem, void *pParam);

/*===========================================================================

                             Macro Definitions

===========================================================================*/


/* ========================================================================
MACRO Q_ALREADY_QUEUED
DESCRIPTION
   Evaluates to true if the item passed in is already in a queue and to
   false otherwise.
=========================================================================== */
#define DALFW_Q_ALREADY_QUEUED( q_link_ptr ) \
   ((q_link_ptr)->pNext != NULL)

/*===========================================================================

                            Function Declarations

===========================================================================*/
#ifdef __cplusplus
   extern "C" {
#endif

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
DALFW_Q_Type* DALFW_Q_Init ( DALFW_Q_Type *q_ptr );


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
DALFW_Q_LinkType* DALFW_Q_Link ( void *pItem, DALFW_Q_LinkType *pLink );


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
void  DALFW_Q_Put ( DALFW_Q_Type *pQ, DALFW_Q_LinkType *pLink );


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
void* DALFW_Q_Get ( DALFW_Q_Type *pQ );


/*===========================================================================

FUNCTION DALFW_Q_LastGet

DESCRIPTION
  This function removes an item from the tail of a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to DALFW_Q_Init.

RETURN VALUE
  A pointer to the dequeued item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the specified queue.

===========================================================================*/
void* DALFW_Q_LastGet ( DALFW_Q_Type *pQ );


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
int DALFW_Q_Cnt  ( DALFW_Q_Type *pQ );


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
void* DALFW_Q_Check ( DALFW_Q_Type *pQ );


/*===========================================================================

FUNCTION DALFW_Q_LastCheck

DESCRIPTION
  This function returns a pointer to the data block at the tail of the queue.
  The data block is not removed from the queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to DALFW_Q_Init.

RETURN VALUE
  A pointer to the queue item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the specified queue.

===========================================================================*/
void* DALFW_Q_LastCheck ( DALFW_Q_Type *pQ );


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
void* DALFW_Q_Next ( DALFW_Q_Type *pQ, DALFW_Q_LinkType *pLink );


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
void DALFW_Q_Insert ( DALFW_Q_Type *pQ, DALFW_Q_LinkType *pLinkToInsert, DALFW_Q_LinkType *pLinkPrev);


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
void DALFW_Q_Delete ( DALFW_Q_Type *pQ, DALFW_Q_LinkType *pLink );


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
DALBOOL DALFW_Q_DeleteExt ( DALFW_Q_Type *pQ, DALFW_Q_LinkType *pLink );

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
  DALFW_Q_Type *pQ,  
  DALFW_Q_CompareFuncType compareFunc,
  void *pCompareVal
);

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
  DALFW_Q_Type *pQ,  
  DALFW_Q_CompareFuncType compareFunc,
  void *pParam,
  DALFW_Q_ActionFuncType actionFunc
);

#ifdef __cplusplus
   }
#endif

#endif /* DALQueue_H */
