/*==============================================================================
@file CoreWorkContainer.h

Core Container Workloop API.

Copyright (c) 2010-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.qdsp6/2.1.c4/power/utils/inc/CoreWorkContainer.h#1 $
==============================================================================*/
#ifndef COREWORKCONTAINER_H
#define COREWORKCONTAINER_H

#include "DALSysTypes.h"


/* declare struct handle types */
typedef struct CoreContainerWorkLoopType *     CoreContainerWorkLoopHandle;
typedef struct CoreWorkContainerType *         CoreWorkContainerHandle;

typedef DALResult (*CoreContainerPutFcn) (CoreWorkContainerHandle hContainer, void *pData );
typedef DALResult (*CoreContainerGetFcn) (CoreWorkContainerHandle hContainer, void **ppData );
typedef DALResult (*CoreContainerProcessFcn)(void *pData );
typedef DALResult (*CoreContainerFreeFcn)(CoreWorkContainerHandle hContainer,void *pData);


typedef struct CoreWorkContainerType
{
  CoreContainerPutFcn          pfnPut;        /* function for adding an element to this container */
  CoreContainerGetFcn          pfnGet;        /* function for retrieving the next element from this container */
  CoreContainerProcessFcn      pfnProcess;    /* (optional) function for processing an element from this container */
  CoreContainerFreeFcn         pfnFree;       /* (optional) function for freeing resources of an element after processing */
  DALSYSSyncHandle             hLock;         /* lock object to protect operations on this container */
  CoreContainerWorkLoopHandle  hWorkLoop;     /* the container-workloop this workloop is a part of */
  CoreWorkContainerHandle      hLink;         /* link pointer for when containers are added to workloops */
  void                       * pCntrData;     /* placeholder for container-type-specific data */
} CoreWorkContainerType;


typedef struct CoreContainerWorkLoopType
{
  DALSYSWorkLoopHandle    hWorkLoop;                   /* Handle to the Container WorkLoop */
  DALSYSEventHandle       hWorkLoopEvent;              /* Used to trigger the infinite loop in the WorkLoop */
  DALSYSEventHandle       hContainerEvent;             /* Used to wait on new container items */
  DALSYSEventHandle       hContainerProcessStartEvent; /* Used to sync the calling context and the WorkLoop */
  DALSYSSyncHandle        hContainerListLock;          /* Lock to protect the container list */
  CoreWorkContainerHandle hContainerList;              /* Handle to the container list */
} CoreContainerWorkLoopType;


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
DALResult Core_RegisterContainerWorkLoopEx( char                         *threadName,
                                            uint32                        dwStackSize,
                                            uint32                        dwPriority,
                                            CoreContainerWorkLoopHandle  *phContainerWorkLoop );


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
DALResult Core_RegisterContainerWorkLoop( uint32                        dwPriority,
                                          CoreContainerWorkLoopHandle  *phContainerWorkLoop );


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
DALResult Core_AddContainerToWorkLoop( CoreWorkContainerHandle         hContainer,
                                       CoreContainerWorkLoopHandle     hWorkLoop );


/**
 * <!-- Core_WorkContainerProcess -->
 *
 * @brief Processes a single container as much a possible with Core_WorkElementProcess
 *
 * @param pWC : Pointer  to the work container to be processed
 *
 * @return DAL_SUCCESS if the workloop is not NULL. 
 */
DALResult Core_WorkContainerProcess( CoreWorkContainerType *pWC );


/**
 * <!-- Core_WorkElementProcess -->
 *
 * @brief If a work element can be removed from the CoreWorkContainer call the
 * work container process and free functions on it.
 *
 * @param *pWC : Pointer to the work container to be processed 
 *
 * @return DAL_SUCCESS if an element was available, DAL_ERROR if no elements available.
 */
DALResult Core_WorkElementProcess( CoreWorkContainerType *pWC );


#endif /* COREWORKCONTAINER_H */
