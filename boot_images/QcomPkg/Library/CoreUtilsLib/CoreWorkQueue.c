/*============================================================================
@file CoreWorkQueue.c

Implementation of a Work Queue that can be added to Container Workloop.

Copyright (c) 2010-2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
All Rights Reserved.
Qualcomm Confidential and Proprietary

============================================================================*/
#include "DALSys.h"
#include "CoreWorkQueue.h"
#include "CoreVerify.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Summing up the structure heirarchy used here: 
 *    "CoreWorkContainerHandle" is a pointer to a "CoreWorkContainerType"
 *         - functions,
 *         - lock,
 *         - pCntrData pointer to a: 
 *               "CoreWorkQueueType"
 *               - pHead (list of CoreWorkQueueElemTypes)
 *               - pTail (list of CoreWorkQueueElemTypes)
 *               - pointer to a Core Pool to create/free:
 *                    "CoreWorkQueueElemType" (same size/memory as what comes from the pool)
 *                    - pNext
 *                    - uPersist
 *                    - pElemData (flexible space for the data like npa_event_action structs)
 */


/**
 * Core_WorkQueueCreate
 *
 * @brief Allocate and initalize a work queue.
 * WorkQueues are passed around by their CoreWorkContainerHandles. This function
 * allocates the work container/handle, the work queue, and the Core Pool.
 *
 * @param phContainer: Handle for the work queue returned here. 
 * @param pfnProcess:  Function to be called to process queue elements. 
 * @param uElemNum:    Initial size of the queue.
 * @param uElemNumSubsequent: Size (in number of elems) of subsequent pool
 *                     block allocations.
 * @param uDataSize:   Size of data stored in queue. Elements in the linked list
 *                     end up being this size + sizeof(CoreWorkQueueElemType)
 *  
 * @return Success or Failure 
 */
DALResult Core_WorkQueueCreate( CoreWorkContainerHandle *phContainer,
                                CoreContainerProcessFcn  pfnProcess,
                                uint32                   uElemNum,
                                uint32                   uElemNumSubsequent,
                                uint32                   uDataSize )
{
  DALResult RetVal = DAL_ERROR;
  CoreWorkContainerType *pContainer = NULL;
  DALBOOL bCleanupCtnr = FALSE, bCleanupLock = FALSE, bCleanupWkQ = FALSE;

  do
  {
    CoreWorkQueueType *pWkQ;
    uint32 uElemSize;
    CorePoolType *pPool;

    /* Allocate the CoreWorkContainerType to hold the function pointers, lock, and the linked list holder */
    RetVal = DALSYS_Malloc( sizeof(CoreWorkContainerType), (void**)phContainer );
    if ( RetVal != DAL_SUCCESS || *phContainer == NULL ) /* latter check for Klocworks */
    {
      break;
    }
    bCleanupCtnr = TRUE;

    pContainer = (CoreWorkContainerType *)(*phContainer);
    memset( pContainer, 0, sizeof(CoreWorkContainerType) );
    
    /* set function pointers */
    pContainer->pfnPut = Core_WorkQueuePut;
    pContainer->pfnGet = Core_WorkQueueGet;
    pContainer->pfnProcess = pfnProcess;
    pContainer->pfnFree = Core_WorkQueueFree;

    /* create lock */
    RetVal = DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE, &pContainer->hLock, NULL );
    if ( DAL_SUCCESS != RetVal )
    {
      break;
    }
    bCleanupLock = TRUE;

    /* hWorkLoop and link_hdl are set to 0, don't need to do anything */

    /* Allocate the work queue type to hold the pHead, pTail, NumElems, and Core Pool */
    RetVal = DALSYS_Malloc( sizeof(CoreWorkQueueType), &pContainer->pCntrData );
    if ( RetVal != DAL_SUCCESS || pContainer->pCntrData == NULL )
    {
      break;
    }
    bCleanupWkQ = TRUE;

    pWkQ = (CoreWorkQueueType *)pContainer->pCntrData;
    memset( pWkQ, 0, sizeof(CoreWorkQueueType) );

    /* Allocate the work queue memory pool */
    DALSYS_SyncEnter( pContainer->hLock );
    pPool = &pWkQ->ElemPool;
    uElemSize = sizeof(CoreWorkQueueElemType) + uDataSize;
    Core_PoolCreate( pPool, uElemNum, uElemSize );
    if ( 0 != uElemNumSubsequent )
    {
      Core_PoolSetAllocationSize( pPool, uElemNumSubsequent );
    }
    DALSYS_SyncLeave( pContainer->hLock );
  } while( 0 );

  /* if everything succeeded, just return */
  if ( DAL_SUCCESS == RetVal )
  {
    return RetVal;
  }

  /* something failed, so we have cleanup to do */
  if ( bCleanupWkQ )
  {
    DALSYS_Free( pContainer->pCntrData );
  }

  if ( bCleanupLock )
  {
    DALSYS_DestroyObject( pContainer->hLock );
  }

  if ( bCleanupCtnr )
  {
    DALSYS_Free( pContainer );
    *phContainer = NULL;
  }

  return RetVal;
}


/**
 * Core_WorkQueueAlloc
 *
 * @brief Get a new WorkQueueElemType from the memory pool. 
 *
 * @param ppData: Pointer to the work queue  pElemData memory returned.
 * @param hContainer: The work queue to get the element from. 
 *  
 * @return Success or Failure 
 */
DALResult Core_WorkQueueAlloc( CoreWorkContainerHandle   hContainer,
                               void                    **ppData)
{
  char *pElem;
  CoreWorkQueueType *pWkQ;

  /* if failed or pool empty, return NULL in *ppData */
  *ppData = NULL;

  if ( NULL == hContainer || NULL == hContainer->pCntrData )
    return DAL_ERROR;

  pWkQ = (CoreWorkQueueType *)hContainer->pCntrData;

  pElem = (char *)Core_PoolAlloc( &pWkQ->ElemPool );
  if ( !pElem )
    return DAL_ERROR;

  memset( pElem, 0, pWkQ->ElemPool.uElemSize );
  
  /* Set the ppData pointer past the CoreWorkQueueElemType values to the
     remaining memory in this element.  Often, this is cast as a
     npa_event_action type by the higher level code.  */
  *ppData = pElem + sizeof(CoreWorkQueueElemType);

  return DAL_SUCCESS;
}


/**
 * Core_WorkQueueFree
 *
 * @brief Return non persistant CoreWorkQueueElemTypes to the memory pool.
 *
 * pData is a pointer to the data area of CoreWorkQueueElemType.  Backs 
 * the ptr up to the start of the CoreWorkQueueElemType and then adds it back
 * to the Core Pool
 *
 * @param hContainer: The work queue the element came from. 
 * @param pData: A pointer to the CoreWorkQueueElemType pElemData area
 *
 * @return Success or Failure 
 */ 
DALResult Core_WorkQueueFree(CoreWorkContainerHandle  hContainer,
                             void                     *pData)
{
  CoreWorkQueueElemType *pElem;
  CoreWorkQueueType *pWkQ;

  if ( NULL == hContainer || NULL == hContainer->pCntrData || NULL == pData )
    return DAL_ERROR;

  pWkQ = (CoreWorkQueueType *)hContainer->pCntrData;

  /* Before calling PoolFree, we set the pElem pointer back to the start of
     the CoreWorkQueueElemType/(pool memory block) we got earlier. */
  pElem = (CoreWorkQueueElemType *)((char *)pData - sizeof(CoreWorkQueueElemType));

  if ( pElem->uPersist == 0 )
  {
    Core_PoolFree( &pWkQ->ElemPool, (void *)pElem );
  }

  return DAL_SUCCESS;
}


/**
 * Core_WorkQueueInsert
 *
 * @brief Put a CoreWorkQueueElemType in the queue.
 *
 * @param hContainer: The work queue the element came from and is being used in. 
 * @param pData:      A pointer to a CoreWorkQueueElemType pElemData area
 * @param where:      CORE_WORKQUEUE_HEAD or CORE_WORKQUEUE_TAIL of the queue
 * @param no_dups     Queue elements can be allocated and owned by client code
 *                    and used repeatedly without being freed. Setting no_dups
 *                    indicates this is a persistant element and we
 *                    should not insert it into the queue more than once. 
 * @param no_signal:  Whether to signal the WorkLoop if there are new elements on 
 *                    the workloop.
 *
 * @return Success or Failure 
 */
DALResult Core_WorkQueueInsert( CoreWorkContainerHandle  hContainer,
                                       void *pData,
                                       unsigned int where,
                                unsigned int no_dups,
                                unsigned int no_signal )
{
  CoreWorkQueueType *pWkQ = NULL;
  CoreWorkQueueElemType *pElem = NULL;
  DALResult RetVal = DAL_SUCCESS;

  if ( hContainer == NULL || hContainer->pCntrData == NULL || pData == NULL )
    return DAL_ERROR;

  pWkQ = (CoreWorkQueueType *)hContainer->pCntrData;

  /* The higher level code only deals with the  pElemData area of the memory we
     manage in this list. We hide the CoreWorkQueueElemType pNext pointer and
     uPersist value from the higher level, but need to back the pointer here up to the start of
     the CoreWorkQueueElemType here. */
  pElem = (CoreWorkQueueElemType *)((char *)pData - sizeof(CoreWorkQueueElemType));

  if ( no_dups )
  {
    /* The queue is locked and if this element matches the head, tail,
       or has it's pNext pointer set, we can deduce it's currently in 
       the queue and we don't need to insert it. */
    if ( pElem->pNext || pWkQ->pTail == pElem || pWkQ->pHead == pElem )
      return DAL_SUCCESS;
  }
  else
  {
    /* no_dups users lock the work queue before calling this, everyone else needs to lock now. */
    DALSYS_SyncEnter( hContainer->hLock );
  }

  pElem->pNext = NULL;

  /* If no_dups, tag elem so that it isn't destroyed after processing. */
  pElem->uPersist = (no_dups ? 1 : 0);
 
  pWkQ->NumElems++;

  if ( pWkQ->pTail )
  {
    if ( where == CORE_WORKQUEUE_TAIL )
    { /* non-empty queue */
      pWkQ->pTail->pNext = pElem;
      pWkQ->pTail = pElem;
    }
    else
    { /* insert at head of queue */
      pElem->pNext = pWkQ->pHead;
      pWkQ->pHead = pElem;
    }
  }
  else
  { /* was empty queue */
    pWkQ->pHead = pWkQ->pTail = pElem;
    /* signal workloop */
    if ( hContainer->hWorkLoop )
    {
      CoreContainerWorkLoopHandle pCWL = hContainer->hWorkLoop;
      if ( pCWL->hContainerEvent && no_signal == FALSE )
      {
        RetVal = DALSYS_EventCtrlEx( pCWL->hContainerEvent, 
                                     DALSYS_EVENT_CTRL_TRIGGER, 0, 0, 0 );
      }
    }
  }

  if ( !no_dups )
  { 
    DALSYS_SyncLeave( hContainer->hLock );
  }

  return RetVal;
}


/**
 * Core_WorkQueuePut
 * 
 * @brief Insert at the tail of the queue. 
 *
 * @param hContainer: The work queue the element came from and is being used in. 
 * @param pData:      A pointer to a CoreWorkQueueElemType pElemData area
 *
 * @return Success or Failure 
 */
DALResult Core_WorkQueuePut( CoreWorkContainerHandle  hContainer,
                             void                    *pData)
{
  return Core_WorkQueueInsert( hContainer, pData, 
      CORE_WORKQUEUE_TAIL, FALSE, FALSE );
}


/**
 * Core_WorkQueuePutAtHead
 *
 * @brief Insert at the start of the queue.
 *
 * @param hContainer: The work queue the element came from and is being used in. 
 * @param pData:      A pointer to a CoreWorkQueueElemType pElemData area
 *
 * @return Success or Failure 
 */
DALResult Core_WorkQueuePutAtHead( CoreWorkContainerHandle  hContainer,
                                   void                    *pData )
{
  return Core_WorkQueueInsert( hContainer, pData, 
      CORE_WORKQUEUE_HEAD, FALSE, FALSE );
}


/**
 * Core_WorkQueueGet
 *
 * @brief Return an element from the head of the queue. 
 *
 * @param hContainer: The work queue the element came from and is being used in. 
 * @param pData:      Returns pointer to a CoreWorkQueueElemType pElemData area
 *                    or NULL.
 *
 * @return Success or Failure 
 */
DALResult Core_WorkQueueGet( CoreWorkContainerHandle   hContainer,
                             void                    **ppData)
{
  CoreWorkQueueType *pWkQ = NULL;
  CoreWorkQueueElemType *pElem = NULL;
  DALResult RetVal = DAL_SUCCESS;

  if ( NULL == ppData )
    return DAL_ERROR;
  
  if ( NULL == hContainer || NULL == hContainer->pCntrData )
  {
    *ppData = NULL;
    return DAL_ERROR;
  }

  pWkQ = (CoreWorkQueueType *)hContainer->pCntrData;
  
  DALSYS_SyncEnter( hContainer->hLock );

  pElem = pWkQ->pHead;
  if ( pElem )
  {
    pWkQ->NumElems--;	  

    /* Return a pointer to the start of the usable data area, pElemData.
       Most often cast as a npa_event_action type but the details
       of that cast are allways done by the caller who allocated this queue.*/
    *ppData = pElem->pElemData;

    /* update queue head */
    if ( pElem->pNext )
    { /* there's at least one more left */
      pWkQ->pHead = pElem->pNext;
    }
    else
    { /* we're taking the last element */
      pWkQ->pHead = NULL;
      pWkQ->pTail = NULL;
    }
    pElem->pNext = NULL;
  }
  else
  { /* nothing in queue */
    *ppData = NULL;
    RetVal = DAL_ERROR;
  }

  DALSYS_SyncLeave( hContainer->hLock );

  return RetVal;
}


/**
 * Core_WorkQueueIsEmpty
 *
 * Return values from IsEmpty and Length routines may be incorrect if
 * the workqueue is subject to concurrent access from multiple threads 
 *
 * @param hContainer: The work queue to check.
 *
 * @return True or False
 */
DALBOOL Core_WorkQueueIsEmpty( CoreWorkContainerHandle hContainer )
{
  if ( !hContainer || !hContainer->pCntrData )
    return TRUE;
  
  return ( NULL == ((CoreWorkQueueType *)hContainer->pCntrData)->pHead );
}


/**
 * Core_WorkQueueLength
 *
 * Return values from IsEmpty and Length routines may be incorrect if
 * the workqueue is subject to concurrent access from multiple threads 
 *
 * @param hContainer: The work queue to check.
 *
 * @return CoreWorkQueueType->NumElems
 */
unsigned int Core_WorkQueueLength( CoreWorkContainerHandle hContainer )
{
  if ( !hContainer || !hContainer->pCntrData )
    return TRUE;
  
  return ((CoreWorkQueueType *)hContainer->pCntrData)->NumElems;
}


/**
 * Core_WorkQueueLock
 *
 * @brief Call DALSYS_SyncEnter on the WorkQueue container's lock.
 *
 * Must be invoked before Core_WorkQueuePut(AtHead)NoDups 
 *
 * @param hContainer: The work queue to lock.
 */
void Core_WorkQueueLock( CoreWorkContainerHandle hContainer )
{
  CORE_VERIFY_PTR( hContainer );
  DALSYS_SyncEnter( hContainer->hLock );
}


/**
 * Core_WorkQueueUnlock
 *
 * @brief Call DALSYS_SyncLeave on the WorkQueue container's lock.
 *
 * Must be invoked after Core_WorkQueuePut(AtHead)NoDups
 *
 * @param hContainer: The work queue to unlock.
 */
void Core_WorkQueueUnlock( CoreWorkContainerHandle hContainer )
{
  CORE_VERIFY_PTR( hContainer );
  DALSYS_SyncLeave( hContainer->hLock );
}


/**
 * Core_WorkQueueSignalWorkLoop
 *
 * @brief Signal to the container workloop that there are elements
 *        on the work queue.
 *
 * This is usually used after Core_WorkQueueInsert is called with no_signal set
 * to true.
 *
 * @param hContainer: The work queue to signal with.
 */
void Core_WorkQueueSignalWorkLoop( CoreWorkContainerHandle hContainer )
{
  CORE_VERIFY_PTR( hContainer );
  DALSYS_EventCtrlEx( hContainer->hWorkLoop->hContainerEvent, 
                      DALSYS_EVENT_CTRL_TRIGGER, 0, 0, 0 );
}

#ifdef __cplusplus
}
#endif