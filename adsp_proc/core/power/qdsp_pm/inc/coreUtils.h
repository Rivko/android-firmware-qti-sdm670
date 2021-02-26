#ifndef coreutils_H
#define coreutils_H
/*===========================================================================

            Q U E U E    S E R V I C E S    H E A D E R    F I L E

DESCRIPTION
 This file contains types and declarations associated with the Queue
 Services.


* Copyright (c) 2014 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Qualcomm Technologies, Inc. Confidential and Proprietary.

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/inc/coreUtils.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/11/06  alisonc    init the file

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "DALStdDef.h"
#include "DALFramework.h"
#include "adsppm_types.h"

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
typedef struct coreUtils_Q_LinkStruct
{
  struct coreUtils_Q_LinkStruct  *pNext;
    /* Ptr to next link in list. If NULL, there is no next link. */

  struct coreUtils_Q_LinkStruct  *pPrev;
    /* Ptr to the tail of link list */
} coreUtils_Q_LinkType;

/* ------------------------------------------------------------------------
** Queue Head Link Structure
**
** When queue items are linked in a singly link list, coreUtils_Q_HeadLinkType is
** used instead of coreUtils_Q_LinkType. This avoids the overhead of traversing 
** the whole of link list when queueing at the end of the link list.
** This structure should be accessed only by Queue services.
** ------------------------------------------------------------------------ */
typedef struct coreUtils_Q_HeadLinkStruct
{
  struct coreUtils_Q_LinkStruct  *pNext;
    /* Ptr to head of the link list */

  struct coreUtils_Q_LinkStruct  *pPrev;
    /* Ptr to the tail of link list */

} coreUtils_Q_HeadLinkType;


/* ------------------------------------------------------------------------
** Queue Structure
**
** The following structure format is used by the Queue Services to represent
** a queue.
** 
** Do NOT access the fields of a queue directly. Only the Queue Services should
** do this.
** ------------------------------------------------------------------------ */
typedef struct coreUtils_Q_Struct
{
  coreUtils_Q_LinkType link;
    /* Used for linking items into queue. */

  int nCnt;
    /* Keeps track of number of items enqueued. Though not necessary, having
       this field is tremendously helpful for debugging. */
} coreUtils_Q_Type;

/* ------------------------------------------------------------------------
** Queue Generic Item
**   Generic items must have coreUtils_Q_LinkType as the first element.  This allows
**   the linear search function to traverse the list without knowing
**   anything about the elements
** ------------------------------------------------------------------------ */
typedef struct {
   coreUtils_Q_LinkType link;
} coreUtils_Q_GenericItemType;

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
typedef int (*coreUtils_Q_CompareFuncType)(void* pItem, void *pCompareVal);

/* ------------------------------------------------------------------------
** Queue Action Function
**    Used by coreUtils_Q_LinearDelete to perform an action on an item which is 
**    being deleted from the list AFTER the item is deleted.  To perform
**    an action BEFORE the item is deleted, the user can call the action
**    function directly in the compare function call back.
** ------------------------------------------------------------------------ */
typedef void (*coreUtils_Q_ActionFuncType)(void *pItem, void *pParam);

/*===========================================================================

                             Macro Definitions

===========================================================================*/


/* ========================================================================
MACRO Q_ALREADY_QUEUED
DESCRIPTION
   Evaluates to true if the item passed in is already in a queue and to
   false otherwise.
=========================================================================== */
#define coreUtils_Q_ALREADY_QUEUED( q_link_ptr ) \
   ((q_link_ptr)->pNext != NULL)

/*===========================================================================

                            Function Declarations

===========================================================================*/
#ifdef __cplusplus
   extern "C" {
#endif

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
coreUtils_Q_Type* coreUtils_Q_Init ( coreUtils_Q_Type *q_ptr );


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
coreUtils_Q_LinkType* coreUtils_Q_Link ( void *pItem, coreUtils_Q_LinkType *pLink );


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
void  coreUtils_Q_Put ( coreUtils_Q_Type *pQ, coreUtils_Q_LinkType *pLink );


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
void* coreUtils_Q_Get ( coreUtils_Q_Type *pQ );


/*===========================================================================

FUNCTION coreUtils_Q_LastGet

DESCRIPTION
  This function removes an item from the tail of a specified queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to coreUtils_Q_Init.

RETURN VALUE
  A pointer to the dequeued item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the specified queue.

===========================================================================*/
void* coreUtils_Q_LastGet ( coreUtils_Q_Type *pQ );


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
int coreUtils_Q_Cnt  ( coreUtils_Q_Type *pQ );


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
void* coreUtils_Q_Check ( coreUtils_Q_Type *pQ );


/*===========================================================================

FUNCTION coreUtils_Q_LastCheck

DESCRIPTION
  This function returns a pointer to the data block at the tail of the queue.
  The data block is not removed from the queue.

DEPENDENCIES
  The specified queue should have been initialized previously via a call
  to coreUtils_Q_Init.

RETURN VALUE
  A pointer to the queue item. If the specified queue is empty, then
  NULL is returned.

SIDE EFFECTS
  The head item, if any, is removed from the specified queue.

===========================================================================*/
void* coreUtils_Q_LastCheck ( coreUtils_Q_Type *pQ );


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
);


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
void coreUtils_Q_Insert ( coreUtils_Q_Type *pQ, coreUtils_Q_LinkType *pLinkToInsert, coreUtils_Q_LinkType *pLinkPrev);


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
void coreUtils_Q_Delete ( coreUtils_Q_Type *pQ, coreUtils_Q_LinkType *pLink );


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
boolean coreUtils_Q_DeleteExt ( coreUtils_Q_Type *pQ, coreUtils_Q_LinkType *pLink );

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
  coreUtils_Q_Type *pQ,  
  coreUtils_Q_CompareFuncType compareFunc,
  void *pCompareVal
);

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
  coreUtils_Q_Type *pQ,  
  coreUtils_Q_CompareFuncType compareFunc,
  void *pParam,
  coreUtils_Q_ActionFuncType actionFunc
);

#ifdef __cplusplus
   }
#endif

#endif /* coreQueue_H */
