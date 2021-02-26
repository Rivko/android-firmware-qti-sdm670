/*==============================================================================
@file CoreWorkContainer.c

Implementation of Container Workloop.

Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/src/CoreWorkContainer.c#1 $
==============================================================================*/
#include "DALSys.h"
#include "CoreVerify.h"
#include "CoreWorkContainer.h"
#include "CoreString.h"
#include <stdio.h>

#ifdef WINSIM
#include "npa_offline_dal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_WL_STACK_SIZE          0x2000  /* 8K */
#define DEFAULT_WL_THREAD_NAME_FORMAT  "CORE_WL_%d"

#ifndef FEATURE_NO_WORKLOOPS
/**
 * <!-- Core_WorkLoopProcess -->
 *
 * @brief Processes a workloop asynchronously by the worker thread 
 *
 * @param hEvent : Not Used
 * @param pData : A CoreContainerWorkLoopHandle
 *
 * @return Will only return if there is an error.  
 */
static DALResult Core_WorkLoopProcess( DALSYSEventHandle hEvent, void *pData )
{
  DALResult RetVal;
  DALBOOL bKeepGoing;
  CoreContainerWorkLoopHandle hWorkLoop = (CoreContainerWorkLoopHandle)pData;
  CoreContainerWorkLoopType *pCWL = (CoreContainerWorkLoopType *)hWorkLoop;
  CoreWorkContainerType *pWC;

  if(NULL == pCWL)
    return DAL_ERROR;

  /* initially reset the event or DAL won't block on it */
  RetVal = DALSYS_EventCtrl(pCWL->hContainerEvent, DALSYS_EVENT_CTRL_RESET);
  if(DAL_SUCCESS != RetVal) return RetVal;

  RetVal = DALSYS_EventCtrl(pCWL->hContainerProcessStartEvent, DALSYS_EVENT_CTRL_TRIGGER);
  if(DAL_SUCCESS != RetVal) return RetVal;

  while(1)
  {
    /* wait for any of our containers to be ready, DALSYS_EventWait() consumes
       the event and reset the event flag (pEventObj->eventRcvd) */
    if(DAL_SUCCESS != DALSYS_EventWait(pCWL->hContainerEvent))
      continue; /* wait error?  try again. */

    /* lock to protect the container list */
    DALSYS_SyncEnter(pCWL->hContainerListLock);

    /* loop through all attached containers until there's nothing left to do */
    do 
    {
      bKeepGoing = FALSE;
      
      /* retrieve the head container */
      pWC = (CoreWorkContainerType*)pCWL->hContainerList;

      /* highest priority container, so process all of it */
      if (DAL_SUCCESS == Core_WorkContainerProcess(pWC))
        pWC = (CoreWorkContainerType*)pWC->hLink;

      while(pWC) 
      {
        /* if an element can be processed, do so then break 
         * to check the higher priority containers */
        if (DAL_SUCCESS == Core_WorkElementProcess(pWC)) 
        {
          bKeepGoing = TRUE;
          break;
        }
        pWC = (CoreWorkContainerType*)pWC->hLink;
      }
    } while(bKeepGoing);
    DALSYS_SyncLeave(pCWL->hContainerListLock);
  }
}
#endif


/**
 * <!-- Core_WorkElementProcess -->
 *
 * @brief If a work element can be removed from the CoreWorkContainer call the
 * work container process and free functions on it.
 *
 * @param *pWC  : Pointer to the work container to be processed 
 *
 * @return DAL_SUCCESS if an element was available, DAL_ERROR if no elements available.
 */
DALResult Core_WorkElementProcess( CoreWorkContainerType *pWC ) 
{
  void *pElement;
  DALResult RetVal = DAL_SUCCESS;
  CORE_VERIFY_PTR(pWC);
  CORE_VERIFY(pWC->pfnGet && pWC->pfnProcess);
  /* try to get something from this container */
  if(DAL_SUCCESS == pWC->pfnGet(pWC, &pElement))
  {
    /* found something from this container */
    pWC->pfnProcess(pElement);
    if(pWC->pfnFree)
      pWC->pfnFree(pWC, pElement);
    RetVal = DAL_SUCCESS;
  }
  else 
  {
    RetVal = DAL_ERROR;
  }
  return RetVal;
}


/**
 * <!-- Core_WorkContainerProcess -->
 *
 * @brief Processes a single container as much a possible with Core_WorkElementProcess
 *
 * @param pWC : Pointer  to the work container to be processed
 *
 * @return DAL_SUCCESS if the workloop is not NULL. 
 */
DALResult Core_WorkContainerProcess( CoreWorkContainerType *pWC )
{
  DALResult RetVal = DAL_SUCCESS;

  /* make sure its a valid container, if it is process the first element */
  if (pWC)
    while(DAL_SUCCESS == Core_WorkElementProcess(pWC)){}
  else 
    RetVal = DAL_ERROR;
  return RetVal;
}


#ifndef FEATURE_NO_WORKLOOPS
/**
 * <!-- Core_RegisterContainerWorkLoopEx -->
 * 
 * @brief Create a container workloop with a name and stack size
 *
 * This function creates a Container WorkLoop, a subclass of the normal DALSYS
 * WorkLoop that supports the processing of items from multiple Work Containers.
 *
 * Users should create a Container WorkLoop and one or more Work Containers, 
 * such as a Work Queue, and call Core_AddContainerToWorkLoop() to 
 * associate the created container(s) with the Container WorkLoop. 
 * Then users can put items into the container(s) for them to be processed 
 * by the Container WorkLoop.
 *
 * Example:
 *   Core_WorkQueueCreate(&hWkC, <Process Fcn>);
 *   Core_WorkQueuePreallocate(hWkC, <...>, <...>, <Data Size>);
 *   Core_RegisterContainerWorkLoopEx(<Name>, <StackSize>, <Priority>, &hWL);
 *   Core_AddContainerToWorkLoop(hWkC, hWL);
 *   ......
 *   // Allocate a buffer from the preallocated memory pool.
 *   Core_WorkQueueAlloc(hWkC, <pointer to item handle>);
 *   // Initialize the item data
 *   ......
 *
 *   // Put the item into the Work Container (Work Queue)
 *   Core_WorkQueuePut(hWkC, <item handle>);
 * 
 * @param threadName : Name of this execution context
 * @param dwStackSize : Stack size of this execution context
 * @param dwPriority : Priority of this execution context
 * @param phContainerWorkLoop : Return parameter, points to the WorkLoop Handle
 * upon successful invocation
 *
 * @return DAL_SUCCESS is everything was successful. phContainerWorkLoop will 
 * hold the new WorkLoop Handle
 */
DALResult Core_RegisterContainerWorkLoopEx( char *threadName,
                                            uint32 dwStackSize,
                                            uint32 dwPriority,
                                            CoreContainerWorkLoopHandle *phContainerWorkLoop )
{
  DALResult RetVal;
  CoreContainerWorkLoopType *pCWL;
  DALBOOL CleanupMalloc = FALSE, CleanupWorkLoop = FALSE, CleanupWLEvt = FALSE, 
          CleanupWLAdd = FALSE, CleanupCtnrEvt = FALSE, CleanupStartEvt = FALSE, 
          CleanupLock = FALSE;

  if((NULL == phContainerWorkLoop) || (NULL == threadName) || (0 == dwStackSize))
    return DAL_ERROR;

  do
  {
    /* allocate memory for our structure */
    RetVal = DALSYS_Malloc(sizeof(CoreContainerWorkLoopType), (void**)phContainerWorkLoop);
    pCWL = (CoreContainerWorkLoopType *)(*phContainerWorkLoop);
    if(DAL_SUCCESS != RetVal) break; else CleanupMalloc = TRUE;
    DALSYS_memset(pCWL, 0, sizeof(CoreContainerWorkLoopType));
  
    /* create the DAL base work loop to work with */
    RetVal = DALSYS_RegisterWorkLoopEx( threadName,      /* pass through the intended thread name */
                                     dwStackSize,        /* pass through the stack size */
                                     dwPriority,         /* pass through user priority */
                                     1,                  /* max num events */
                                     &(pCWL->hWorkLoop), /* return pointer to user */
                                     NULL);              /* workloop is hard to preallocate for */
    if(DAL_SUCCESS != RetVal) break; else CleanupWorkLoop = TRUE;

    /* create the workloop event that we'll auto-fire to trigger the container process function */
    RetVal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_WORKLOOP_EVENT, /* workloop event */
                               &(pCWL->hWorkLoopEvent),          /* handle for it */
                               NULL); /* worry about preallocation support later */
    if(DAL_SUCCESS != RetVal) break; else CleanupWLEvt = TRUE;

    /* add the workloop event to the workloop */
    RetVal = DALSYS_AddEventToWorkLoop(pCWL->hWorkLoop,
                                Core_WorkLoopProcess,
                                (void*)pCWL,
                                pCWL->hWorkLoopEvent,
                                NULL);
    if(DAL_SUCCESS != RetVal) break; else CleanupWLAdd = TRUE;

    /* create a 'client default' event which is supposed to map to native signal types */
    RetVal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
                                &(pCWL->hContainerEvent),
                                NULL); /* worry about preallocation support later */
    if(DAL_SUCCESS != RetVal) break; else CleanupCtnrEvt = TRUE;

    RetVal = DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT,
                                &(pCWL->hContainerProcessStartEvent),
                                NULL); /* worry about preallocation support later */
    if(DAL_SUCCESS != RetVal) break; else CleanupStartEvt = TRUE;

    RetVal = DALSYS_EventCtrl(pCWL->hContainerProcessStartEvent, DALSYS_EVENT_CTRL_RESET);
    if(DAL_SUCCESS != RetVal) break;

    /* create a lock to protect the container list */
    RetVal = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE,
                               &(pCWL->hContainerListLock),
                               NULL); /* worry about preallocation support later */
    if(DAL_SUCCESS != RetVal) break; else CleanupLock = TRUE;

    RetVal = DALSYS_EventCtrl(pCWL->hWorkLoopEvent, DALSYS_EVENT_CTRL_TRIGGER);
    if(DAL_SUCCESS != RetVal) break;
    
    RetVal = DALSYS_EventWait(pCWL->hContainerProcessStartEvent);
  } while(FALSE);

  /* If we succeeded, exit now with DAL_SUCCESS. */
  if(DAL_SUCCESS == RetVal) return RetVal;

  /* Something failed, so clean up. */
  if(CleanupLock)
  {
    DALSYS_DestroyObject(pCWL->hContainerListLock);
  }

  if(CleanupStartEvt)
  {
    DALSYS_DestroyObject(pCWL->hContainerProcessStartEvent);
  }

  if(CleanupCtnrEvt)
  {
    DALSYS_DestroyObject(pCWL->hContainerEvent);
  }
  
  if(CleanupWLAdd)
  {
    DALSYS_DeleteEventFromWorkLoop(pCWL->hWorkLoop,
                                   pCWL->hWorkLoopEvent);
  }
  
  if(CleanupWLEvt)
  {
    DALSYS_DestroyObject(pCWL->hWorkLoopEvent);
  }

  if(CleanupWorkLoop)
  {
    /* destroy object is not "recommended" for workloops, but it doesn't say
     * you can't use it.  it seems like this function should manage adding an
     * event to the workloop that does nothing but exit with an error code if
     * that's really the "right" way to kill off a workloop. */
    DALSYS_DestroyObject(pCWL->hWorkLoop);
  }
  
  if(CleanupMalloc)
  {
    DALSYS_Free(*phContainerWorkLoop);
    *phContainerWorkLoop = NULL;
  }

  return RetVal;
}


/**
 * <!-- Core_RegisterContainerWorkLoop -->
 *
 * @brief Setup a container workloop with an incremented loop count as part of
 * the name. 
 *
 * Adds the number to the name and then calls Core_RegisterContainerWorkLoopEx
 *
 * @param dwPriority : Desired thread priority to pass to the thread creation.
 * @param phContainerWorkLoop : Pointer to a CoreContainerWorkLoopHandle to be
 * filled in
 *
 * @return DAL_SUCCESS is everything was successful. phContainerWorkLoop will 
 * hold the new WorkLoop Handle
 */
DALResult Core_RegisterContainerWorkLoop( uint32 dwPriority,
                                          CoreContainerWorkLoopHandle *phContainerWorkLoop )
{
  static unsigned int workLoopCount = 0;
  char workLoopName[128];

  core_snprintf( workLoopName, sizeof(workLoopName), DEFAULT_WL_THREAD_NAME_FORMAT, workLoopCount );
  workLoopCount++;

  return Core_RegisterContainerWorkLoopEx(workLoopName, DEFAULT_WL_STACK_SIZE,
                                          dwPriority, phContainerWorkLoop);
}


/**
 * <!-- Core_AddContainerToWorkLoop -->
 *
 * @brief Add a CoreWorkContainerHandle to the CoreContainerWorkLoop linked list
 *
 * The order in which containers are added determines the priority in which 
 * the work loop processes items from the containers.  That is:
 *  - If the first container added has an item that item is processed.
 *  - Otherwise if the second container has an item, that item is proceesed.
 *  - This pattern repeats, onward to the nth container.
 *  - After an item is processed, the workloop restarts at the first container.
 *
 * @param hContainer : Container to add
 * @param hWorkLoop : Workloop to add the container to. 
 * 
 * @return DAL_SUCCESS if the addition was successful. 
 */
DALResult Core_AddContainerToWorkLoop( CoreWorkContainerHandle    hContainer,
                                       CoreContainerWorkLoopHandle hWorkLoop )
{
  CoreContainerWorkLoopType *pCWL = (CoreContainerWorkLoopType *)hWorkLoop;
  CoreWorkContainerType     *pWC = (CoreWorkContainerType *)hContainer;
  CoreWorkContainerType     *pInsertPoint;
 
  /* Check for bad arguments. */
  if((NULL == pWC) || (NULL == pCWL) || (NULL == pCWL->hContainerListLock))
    return DAL_ERROR;
 
  /* Check that this container isn't already a part of a workloop. */
  if((NULL != pWC->hWorkLoop) || (NULL != pWC->hLink))
    return DAL_ERROR;
 
  /* this operation should be ok--lock the workloop and append the container */
  DALSYS_SyncEnter(pCWL->hContainerListLock);
 
  /* find where to put the container */
  if(NULL == pCWL->hContainerList)
  {
    /* first container added! */
    pCWL->hContainerList = hContainer;
  }
  else
  {
    /* find the last container in the loop */
    pInsertPoint = (CoreWorkContainerType *)pCWL->hContainerList;
    while(pInsertPoint->hLink)
      pInsertPoint = (CoreWorkContainerType *)pInsertPoint->hLink;
 
    /* found it--now insert */
    pInsertPoint->hLink = hContainer;
  }
 
  /* also populate the back-link */
  pWC->hWorkLoop = hWorkLoop;
 
  DALSYS_SyncLeave(pCWL->hContainerListLock);
 
  return DAL_SUCCESS;
}
#endif


#ifdef __cplusplus
}
#endif
