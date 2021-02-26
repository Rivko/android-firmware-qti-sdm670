/*============================================================================
@file CoreWorkQueue.c

Implementation of a Work Queue that can be added to Container Workloop.

Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1/power/utils/src/CoreWorkQueue.c#1 $
============================================================================*/
#include "DALSys.h"
#include "CoreWorkQueue.h"
#include "CoreHeap.h"
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
  DALResult ret = DAL_ERROR;
  CoreWorkContainerType *pContainer = NULL;
  DALBOOL bCleanupCtnr = FALSE, bCleanupLock = FALSE, bCleanupWkQ = FALSE;

  do
  {
    CoreWorkQueueType *pWkQ;
    uint32 uElemSize;
    CorePoolType *pPool;

    /* Allocate the CoreWorkContainerType to hold the function pointers, lock, and the linked list holder */
    pContainer = *phContainer = 
      (CoreWorkContainerType *) Core_Malloc( sizeof(CoreWorkContainerType) );
    if ( !pContainer )
    {
      break;
    }
    bCleanupCtnr = TRUE;

    memset( pContainer, 0, sizeof(CoreWorkContainerType) );
    
    /* set function pointers */
    pContainer->pfnPut = Core_WorkQueuePut;
    pContainer->pfnGet = Core_WorkQueueGet;
    pContainer->pfnProcess = pfnProcess;
    pContainer->pfnFree = Core_WorkQueueFree;

    /* create lock */
    ret = DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE, 
                             &pContainer->hLock, NULL );
    if ( DAL_SUCCESS != ret )
    {
      break;
    }
    bCleanupLock = TRUE;

    /* hWorkLoop and link_hdl are set to 0, don't need to do anything */

    /* Allocate the work queue type to hold the pHead, pTail, NumElems, and Core Pool */
    pWkQ = pContainer->pCntrData = 
      (CoreWorkQueueType *) Core_Malloc( sizeof(CoreWorkQueueType) );
    if ( !pWkQ )
    {
      break;
    }
    bCleanupWkQ = TRUE;

    memset( pWkQ, 0, sizeof(CoreWorkQueueType) );

    /* Allocate the work queue memory pool */
    DALSYS_SyncEnter( pContainer->hLock );
    pPool = &pWkQ->ElemPool;
    uElemSize = sizeof(CoreWorkQueueElemType) + uDataSize;
    Core_PoolCreate( pPool, uElemNum, uElemSize );
    if ( uElemNumSubsequent != 0 )
    {
      Core_PoolSetAllocationSize( pPool, uElemNumSubsequent );
    }
    DALSYS_SyncLeave( pContainer->hLock );
  } while( 0 );

  /* if everything succeeded, just return */
  if ( DAL_SUCCESS == ret )
  {
    return ret;
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

  return ret;
}


/**
 * Core_WorkQueueAlloc
 *
 * @brief Get a new WorkQueueElemType from the memory pool. 
 *
 * @param hContainer: The work queue to get the element from. 
 * @param ppData: [OUT] If non-NULL, pointer to the usable area of the
 * new WorkQueueElemType
 *  
 * @return DAL_SUCCESS or DAL_ERROR
 */
DALResult Core_WorkQueueAlloc( CoreWorkContainerHandle   hContainer,
                               void                    **ppData )
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
     remaining (usable) memory in this element */
  *ppData = pElem + sizeof(CoreWorkQueueElemType);

  return DAL_SUCCESS;
}

/**
 * Core_WorkQueuePersistAlloc
 *
 * @brief Get a new "persistent" workqueue element from the memory pool.
 * These elements are owned by the caller and will not be destroyed by
 * the workloop after processing.
 *
 * @param hContainer: The work queue to get the element from. 
 * @param ppData: [OUT] If non-NULL, pointer to the usable area of the
 * new WorkQueueElemType
 *  
 * @return DAL_SUCCESS or DAL_ERROR
 */
DALResult Core_WorkQueuePersistAlloc( CoreWorkContainerHandle   hContainer,
                                      void                    **ppData )
{
  DALResult ret = Core_WorkQueueAlloc( hContainer, ppData );
  
  if ( ret == DAL_SUCCESS )
  {
    char *pData = (char *)(*ppData);
    CoreWorkQueueElemType *pElem;

    /* pData is the "data" portion of CoreWorkQueueElemType. Backup this
       pointer to get at the full data structure */
    pElem = (CoreWorkQueueElemType *)(pData - sizeof(CoreWorkQueueElemType));
    pElem->uPersist = TRUE;
  }

  return ret;
}

/**
 * Core_WorkQueueFree
 *
 * @brief Return non-persistent CoreWorkQueueElemTypes to the memory pool.
 *
 * pData is a pointer to the data area of CoreWorkQueueElemType. We back the
 * pointer up to the start of the CoreWorkQueueElemType and then free it
 * back to the pool.
 *
 * @param hContainer: The work queue the element came from. 
 * @param pData: A pointer to the CoreWorkQueueElemType pElemData area
 *
 * @return Success or Failure 
 */ 
DALResult Core_WorkQueueFree(CoreWorkContainerHandle hContainer,
                             void                   *pData)
{
  CoreWorkQueueElemType *pElem;
  CoreWorkQueueType *pWkQ;

  if ( NULL == hContainer || NULL == hContainer->pCntrData || NULL == pData )
    return DAL_ERROR;

  pWkQ = (CoreWorkQueueType *)hContainer->pCntrData;

  /* pData is the "data" portion of CoreWorkQueueElemType. Backup this
     pointer to get at the full data structure */
  pElem = 
    (CoreWorkQueueElemType *)((char *)pData - sizeof(CoreWorkQueueElemType));

  if ( pElem->uPersist == FALSE )
  {
    Core_PoolFree( &pWkQ->ElemPool, (void *)pElem );
  }

  return DAL_SUCCESS;
}

/**
 * Core_WorkQueueForceFree
 *
 * @brief Return the given workqueue element to the pool, irrespective
 * of whether it is persistent or not.
 *
 * @param hContainer: The work queue the element came from. 
 * @param pData: A pointer to the CoreWorkQueueElemType pElemData area
 *
 * @return Success or Failure 
 */ 
DALResult Core_WorkQueueForceFree( CoreWorkContainerHandle hContainer,
                                   void                   *pData )
{
  CoreWorkQueueElemType *pElem;
  CoreWorkQueueType *pWkQ;

  if ( NULL == hContainer || NULL == hContainer->pCntrData || NULL == pData )
    return DAL_ERROR;

  pWkQ = (CoreWorkQueueType *)hContainer->pCntrData;

  /* pData is the "data" portion of CoreWorkQueueElemType. Backup this
     pointer to get at the full data structure */
  pElem = 
    (CoreWorkQueueElemType *)((char *)pData - sizeof(CoreWorkQueueElemType));

  Core_PoolFree( &pWkQ->ElemPool, (void *)pElem );

  return DAL_SUCCESS;
}

/**
 * Core_WorkQueueInsert
 *
 * @brief Put a CoreWorkQueueElemType in the queue.
 *
 * @param hContainer: The work queue to insert the element into
 * @param pData:      A pointer to a workqueue element
 * @param where:      CORE_WORKQ_HEAD or CORE_WORKQ_TAIL of the queue
 * @param no_signal:  If true, the workloop will not be signalled after insert
 *
 * @return Success or Failure 
 */
DALResult Core_WorkQueueInsert( CoreWorkContainerHandle hContainer,
                                void *pData,
                                unsigned int where,
                                unsigned int no_signal )
{
  CoreWorkQueueType *pWkQ = NULL;
  CoreContainerWorkLoopType *pWL = NULL;
  CoreWorkQueueElemType *pElem = NULL;
  DALResult ret = DAL_SUCCESS;

  if ( hContainer == NULL || hContainer->pCntrData == NULL || pData == NULL )
  {
    return DAL_ERROR;
  }

  pWkQ = (CoreWorkQueueType *)hContainer->pCntrData;

  /* pData is the "data" portion of CoreWorkQueueElemType. Backup this
     pointer to get at the full data structure */
  pElem = 
    (CoreWorkQueueElemType *)((char *)pData - sizeof(CoreWorkQueueElemType));

  DALSYS_SyncEnter( hContainer->hLock );

  /* If the element matches the head or tail or has its pNext pointer set,
     we deduce that it is already in the queue and do nothing. */
  if ( pElem->pNext || pWkQ->pTail == pElem || pWkQ->pHead == pElem )
  {
    DALSYS_SyncLeave( hContainer->hLock );
    return DAL_SUCCESS;
  }

  pElem->pNext = NULL;

  pWkQ->NumElems++;

  if ( pWkQ->pTail )
  {
    if ( where == CORE_WORKQ_TAIL )
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
  { /* is empty queue */
    pWkQ->pHead = pWkQ->pTail = pElem;

    /* signal workloop */
    pWL = hContainer->hWorkLoop;
    if ( pWL && no_signal == FALSE )
    {
      ret = DALSYS_EventCtrlEx( pWL->hContainerEvent, 
                                DALSYS_EVENT_CTRL_TRIGGER, 0, 0, 0 );
    }
  }

  DALSYS_SyncLeave( hContainer->hLock );
  return ret;
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
  return Core_WorkQueueInsert( hContainer, pData, CORE_WORKQ_TAIL, FALSE );
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
  return Core_WorkQueueInsert( hContainer, pData, CORE_WORKQ_HEAD, FALSE );
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
  DALResult ret = DAL_SUCCESS;

  if ( !ppData ) return DAL_ERROR;
  
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

    /* Return a pointer to the start of the data area of the element */
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
    ret = DAL_ERROR;
  }

  DALSYS_SyncLeave( hContainer->hLock );

  return ret;
}


/**
 * Core_WorkQueueIsEmpty
 *
 * @brief Returns TRUE if the work queue is empty. Note that return
 * values from the IsEmpty and Length routines may be incorrect if
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
 * @brief Returns the number of elements in the work queue. Note that 
 * return values from IsEmpty and Length routines may be incorrect if
 * the workqueue is subject to concurrent access from multiple threads 
 *
 * @param hContainer: The work queue to check.
 *
 * @return CoreWorkQueueType->NumElems
 */
unsigned int Core_WorkQueueLength( CoreWorkContainerHandle hContainer )
{
  if ( !hContainer || !hContainer->pCntrData )
    return 0;
  
  return ((CoreWorkQueueType *)hContainer->pCntrData)->NumElems;
}


/**
 * Core_WorkQueueLock
 *
 * @brief Call DALSYS_SyncEnter on the WorkQueue container's lock.
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
 * @brief Signal the workloop that there are elements on the workqueue.
 * In cases where Core_WorkQueueInsert was used with bSignalWorkloop
 * set to FALSE (when in STM, for e.g.), this rountine may be used
 * to subsequently signal the workloop.
 *
 * @param hContainer: The work queue whose associated workloop is to be 
 *                    signaled
 */
void Core_WorkQueueSignalWorkLoop( CoreWorkContainerHandle hContainer )
{
  CORE_VERIFY_PTR( hContainer );
  CORE_VERIFY_PTR( hContainer->hWorkLoop );

  DALSYS_EventCtrlEx( hContainer->hWorkLoop->hContainerEvent, 
                      DALSYS_EVENT_CTRL_TRIGGER, 0, 0, 0 );
}

#ifdef __cplusplus
}
#endif
