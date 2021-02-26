/*==============================================================================
@file  DALSYSWorkLoop.c

DAL SYS Work Loop implementation

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "qurt.h"
#include "stdio.h"
#include "stddef.h"

#define DEFAULT_THREAD_NAME_FORMAT  "DAL_WL_%d"

#define SIGNAL_EVENT                0x1
#define SIGNAL_EXIT                 0x2

#define WL_EVENT_DEQUEUED           0
#define WL_EVENT_ENQUEUED           1

extern void DALSYS_WorkLoopThreadJoin(_DALSYSWorkLoopObj * pWorkLoopObj);
extern void DALSYS_WorkLoopThreadExit(_DALSYSWorkLoopObj * pWorkLoopObj);
extern DALResult DALSYS_WorkLoopThreadCreate(
      _DALSYSWorkLoopObj *pWorkLoopObj, char * pszname, 
      unsigned int dwStackSize, unsigned int dwPriority );

#ifdef DALSYS_WL_TRACK_CNT
// workloop tracking is enabled

// Data structures to track active workloops
unsigned int PD_VAR(DALSYSWorkLoopCnt) = DALSYS_WL_TRACK_CNT;
_DALSYSWorkLoopObj * PD_VAR(DALSYSWorkLoopList)[DALSYS_WL_TRACK_CNT];

static DALSYSSyncHandle workLoopListSyncHdl;

static void trackWorkLoop(_DALSYSWorkLoopObj * newWL)
{
   static unsigned int workLoopListIndex = (DALSYS_WL_TRACK_CNT-1);
   unsigned int index;

   DALSYS_SyncEnter(workLoopListSyncHdl);

   index = workLoopListIndex;
   do
   {
      index = (index + 1) % DALSYS_WL_TRACK_CNT;
   }
   while (PD_VAR(DALSYSWorkLoopList)[index] && index != workLoopListIndex);

   if (NULL == PD_VAR(DALSYSWorkLoopList)[index])
   {
      PD_VAR(DALSYSWorkLoopList)[index] = newWL;
      workLoopListIndex = index;
   }
   else
   {
      DALSYS_LOG_ERROR_EVENT("WL tracking list is full");
   }

   DALSYS_SyncLeave(workLoopListSyncHdl);
}

static void untrackWorkLoop(_DALSYSWorkLoopObj * deletedWL)
{
   unsigned int index;

   DALSYS_SyncEnter(workLoopListSyncHdl);
   for (index = 0; index < DALSYS_WL_TRACK_CNT; index++)
   {
      if (PD_VAR(DALSYSWorkLoopList)[index] == deletedWL)
      {
         PD_VAR(DALSYSWorkLoopList)[index] = NULL;
         break;
      }
   }
   DALSYS_SyncLeave(workLoopListSyncHdl);
}

void DALSYSWorkLoopInit(void)
{
   static DALSYSSyncObj workLoopListSyncObj;
   DALResult result;

   result = DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE, &workLoopListSyncHdl,
      &workLoopListSyncObj );
   if (DAL_SUCCESS != result)
   {
      DALSYS_LOG_ERROR_EVENT("Unable to create sync object for workloop list");
   }

   if (DALSYS_WL_TRACK_CNT != PD_VAR(DALSYSWorkLoopCnt))
   {
      // the useless comparisions and assignments
      // ensure above variables would not be stripped by linker so 
      // they are accessible by the DAL log CMM script
      PD_VAR(DALSYSWorkLoopCnt) = DALSYS_WL_TRACK_CNT;
   }
}

#else // DALSYS_WL_TRACK_CNT is not defined 
// workloop tracking is disabled - provide stubs

static void trackWorkLoop(_DALSYSWorkLoopObj * newWL) {}

static void untrackWorkLoop(_DALSYSWorkLoopObj * deletedWL) {}

void DALSYSWorkLoopInit(void) {}

#endif

static DALResult dequeue(_DALSYSWorkLoopEvent *pRemoveWorkLoopEvent)
{
   _DALSYSWorkLoopObj * pWorkLoopObj = (_DALSYSWorkLoopObj *)
      pRemoveWorkLoopEvent->hWorkLoop;

   if(NULL == pWorkLoopObj)
      return DAL_ERROR;

   /* Check if the event is in Work Loop Queue */
   if(WL_EVENT_ENQUEUED == pRemoveWorkLoopEvent->dwState)
   {
      DALSYS_SyncEnter(pWorkLoopObj->hSync);

      /* Work Loop queue update */
      if (pWorkLoopObj->hWorkLoopEventHead)
      {
         if (pWorkLoopObj->hWorkLoopEventHead == pRemoveWorkLoopEvent)
         {
            // deleted event is at the head of the queue
            pWorkLoopObj->hWorkLoopEventHead = pRemoveWorkLoopEvent->nextEvent;
            if (pWorkLoopObj->hWorkLoopEventTail == pRemoveWorkLoopEvent)
            {
               // deleted event is the only event on queue
               pWorkLoopObj->hWorkLoopEventTail = NULL;
            }
         }
         else 
         {
            _DALSYSWorkLoopEvent *pCurWorkLoopEvent = pWorkLoopObj->hWorkLoopEventHead;
            while (pCurWorkLoopEvent->nextEvent && pCurWorkLoopEvent->nextEvent != pRemoveWorkLoopEvent)
            {
               // iterate until the end of the list or the deleted event is found
               pCurWorkLoopEvent = pCurWorkLoopEvent->nextEvent;
            }
            if (pCurWorkLoopEvent->nextEvent == pRemoveWorkLoopEvent)
            {
               // found the deleted event
               pCurWorkLoopEvent->nextEvent = pRemoveWorkLoopEvent->nextEvent;
               if (pWorkLoopObj->hWorkLoopEventTail == pRemoveWorkLoopEvent)
               {
                  // deleted event is at the end of the event queue
                  pWorkLoopObj->hWorkLoopEventTail = pCurWorkLoopEvent;
               }
            }
         }
      }

      DALSYS_SyncLeave(pWorkLoopObj->hSync);

      pRemoveWorkLoopEvent->dwState = WL_EVENT_DEQUEUED;
      pRemoveWorkLoopEvent->nextEvent = NULL;
   }
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
Work Loops thread of execution
------------------------------------------------------------------------------*/
void _WorkLoopExecute(void *pArg)
{
   _DALSYSWorkLoopObj *pWorkLoopObj = (_DALSYSWorkLoopObj *) pArg;

   trackWorkLoop(pWorkLoopObj);

   for(;;)
   {
      unsigned int signal;

      /*----------------------------------------------------------------------
      Wait for commands
      ----------------------------------------------------------------------*/
      signal = qurt_anysignal_wait(&pWorkLoopObj->signal, 
            SIGNAL_EVENT | SIGNAL_EXIT );
      qurt_anysignal_clear(&pWorkLoopObj->signal, SIGNAL_EVENT | SIGNAL_EXIT);

      if (signal & SIGNAL_EXIT)
      {
         untrackWorkLoop(pWorkLoopObj);
         DALSYS_WorkLoopThreadExit(pWorkLoopObj);
         // this thread is terminated
         // DALSYS_DestroyWorkLoopObject must be called to free the 
         // workloop memory and resources
      }
      if (signal & SIGNAL_EVENT)
      {
         /* Process Work Loop queue */
         for(;;)
         {
            DALSYSEventHandle hEvent;
            _DALSYSWorkLoopEvent *pWorkLoopEvent;
            DALResult ret;

            DALSYS_SyncEnter(pWorkLoopObj->hSync);

            if(!pWorkLoopObj->hWorkLoopEventHead)
            {  /* Queue is empty */
               DALSYS_SyncLeave(pWorkLoopObj->hSync);
               break;
            }

            /* Remove Event from queue head */
            pWorkLoopEvent = pWorkLoopObj->hWorkLoopEventHead;
            pWorkLoopEvent->dwState = WL_EVENT_DEQUEUED;

            /* Adjust queue after the above removal */
            pWorkLoopObj->hWorkLoopEventHead = pWorkLoopEvent->nextEvent;
            if(!pWorkLoopObj->hWorkLoopEventHead) /* There was just 1  Event */
               pWorkLoopObj->hWorkLoopEventTail = NULL;

            DALSYS_SyncLeave(pWorkLoopObj->hSync);

            hEvent = DALSYSCMN_CONTAINEROF(pWorkLoopEvent, _DALSYSEventObj, p.wl);
            /* Process the event */
            if(pWorkLoopEvent->hSync)
                DALSYS_SyncEnter(pWorkLoopEvent->hSync);

            ret = pWorkLoopEvent->pWorkLoopFunc(hEvent, pWorkLoopEvent->pArg);

            if(pWorkLoopEvent->hSync)
                DALSYS_SyncLeave(pWorkLoopEvent->hSync);

            if (DAL_WORK_LOOP_EXIT == ret)
            {
               untrackWorkLoop(pWorkLoopObj);
               DALSYS_WorkLoopThreadExit(pWorkLoopObj);
               // this thread is terminated
               // DALSYS_DestroyWorkLoopObject must be called to free the 
               // workloop memory and resources
            }

         }
      }
   }
}

/*------------------------------------------------------------------------------
User API to Register Work Loop
-------------------------------------------------------------------------------*/
static DALResult
registerWorkLoop( char *pszname,
      uint32 dwStackSize,
      uint32 dwPriority,
      void * ownerRetAddr,
      DALSYSWorkLoopHandle *phWorkLoop,
      DALSYSWorkLoopObj *pObj)
{
   _DALSYSWorkLoopObj *pWorkLoopObj = (_DALSYSWorkLoopObj *)pObj;

   if (!phWorkLoop)
   {
      DALSYS_LOG_ERROR_EVENT("phWorkLoop is NULL- workloop:%s", 
            pszname);
      return DAL_ERROR;
   }
   *phWorkLoop = NULL;

   if(NULL == pWorkLoopObj)
   {
      /*----------------------------------------------------------------------
      User has not passed in WorkLoop object, we need to allocate memory.
      Always get the size form the DALSYS Macros. This is done to support
      static allocation and maintain consistency between sizes of statically
      and dynamically alloced structures.
      ----------------------------------------------------------------------*/

      if(DAL_SUCCESS != DALSYS_Malloc(sizeof(_DALSYSWorkLoopObj), (void**)&pWorkLoopObj))
         return DAL_ERROR;

      memset(pWorkLoopObj,0,sizeof(_DALSYSWorkLoopObj));
      DALSYSCMN_SET_DYNAMIC_ALLOC_BIT(pWorkLoopObj->dwObjInfo);
   }
   else
   {
      /*----------------------------------------------------------------------
      User has allocated memory allocate memory, we dont allocate memory
      ----------------------------------------------------------------------*/
      memset(pWorkLoopObj,0,sizeof(_DALSYSWorkLoopObj));
   }

   /*--------------------------------------------------------------------------
   Initialize the remaining data structure
   --------------------------------------------------------------------------*/
   if(DAL_SUCCESS != DALSYS_SyncCreate(DALSYS_SYNC_ATTR_NO_PREEMPTION,
         &pWorkLoopObj->hSync, NULL))
   {
      goto wl_reg_error_1;
   }

   // allocate memory for thread stack.
   if (DAL_SUCCESS != DALSYS_Malloc(dwStackSize, 
       (void**)&pWorkLoopObj->pStackBuffer ))
   {
      goto wl_reg_error_2;
   }

   DALSYSCMN_SET_OBJECT_ID(pWorkLoopObj->dwObjInfo,DALSYSCMN_WORKLOOP_OBJECT_ID);
   qurt_anysignal_init(&pWorkLoopObj->signal);

   if (!dwPriority || dwPriority > 255)
   {
      dwPriority = DALSYS_WL_DEFAULT_PRIO;
   }
   DALSYSCMN_SET_ATTRIB(pWorkLoopObj->dwObjInfo,(dwPriority<<4));

   // setting debug information
   pWorkLoopObj->ownerThreadId = qurt_thread_get_id();
   pWorkLoopObj->ownerRetAddr = ownerRetAddr;
   pWorkLoopObj->thisVirtualAddr = (DALSYSMemAddr)pWorkLoopObj;

   if (DAL_SUCCESS == DALSYS_WorkLoopThreadCreate( pWorkLoopObj, pszname, dwStackSize, dwPriority ) )
   {
      *phWorkLoop = pWorkLoopObj;
      return DAL_SUCCESS;
   }

   // workloop creation failed - need to cleanup
   qurt_anysignal_destroy(&pWorkLoopObj->signal);

   DALSYS_Free(pWorkLoopObj->pStackBuffer);

wl_reg_error_2:
   DALSYS_DestroyObject( pWorkLoopObj->hSync );

wl_reg_error_1:
   if(DALSYSCMN_IS_OBJ_DYNAMICALLY_ALLOCED(pWorkLoopObj->dwObjInfo))
      DALSYS_Free(pWorkLoopObj); /* If dynamically alloced free memory */
   else /*Statically alloced object, reset all fields to ZERO*/
      memset(pWorkLoopObj,0,sizeof(_DALSYSWorkLoopObj));

   DALSYS_LOG_ERROR_EVENT("failed- pszname:%s", pszname);
   return DAL_ERROR;
}

DALResult
DALSYS_RegisterWorkLoopEx( char * workLoopName,
      uint32 dwStackSize,
      uint32 dwPriority,
      uint32 dwMaxNumEvents,
      DALSYSWorkLoopHandle *phWorkLoop,
      DALSYSWorkLoopObj *pObj)
{
   if ((NULL == workLoopName) || (0 == strncmp(workLoopName, "", 1)))
   {
      DALSYS_LOG_ERROR_EVENT("workloop name is NULL");
      return DAL_ERROR;
   }

   return registerWorkLoop(workLoopName, dwStackSize, 
      dwPriority, __builtin_return_address(0), phWorkLoop, pObj);
}

DALResult
DALSYS_RegisterWorkLoop(uint32 dwPriority,
                   uint32 dwMaxNumEvents,
                   DALSYSWorkLoopHandle *phWorkLoop,
                   DALSYSWorkLoopObj *pObj)
{
   static unsigned int workLoopCount = 0;
   char workLoopName[QURT_THREAD_ATTR_NAME_MAXLEN];

   snprintf( workLoopName, sizeof(workLoopName), DEFAULT_THREAD_NAME_FORMAT,
         workLoopCount );
   workLoopCount++;

   return registerWorkLoop(workLoopName, DALSYS_WL_DEFAULT_STACK_SIZE, 
      dwPriority, __builtin_return_address(0), phWorkLoop, pObj);
}

/*------------------------------------------------------------------------------
Internal API to insert Event into Work Loop queue
-------------------------------------------------------------------------------*/
DALResult DALSYS_EnqueueAndTrigger(_DALSYSWorkLoopEvent *pWorkLoopEvent)
{
   _DALSYSWorkLoopObj *pWorkLoopObj;

   if(!pWorkLoopEvent)
   {
      DALSYS_LOG_ERROR_EVENT("pWorkLoopEvent is NULL");
      return DAL_ERROR;
   }

   pWorkLoopObj = (_DALSYSWorkLoopObj *)pWorkLoopEvent->hWorkLoop;

   if(NULL == pWorkLoopObj)
   {
      DALSYS_LOG_ERROR_EVENT("pWorkLoopObj is NULL");
      return DAL_ERROR;
   }

   /* Check if the event is not already in Work Loop Queue */
   if(WL_EVENT_ENQUEUED != pWorkLoopEvent->dwState)
   {
      DALSYS_SyncEnter(pWorkLoopObj->hSync);

      /* Work Loop queue update */
      (!pWorkLoopObj->hWorkLoopEventHead) ?
         (pWorkLoopObj->hWorkLoopEventHead = pWorkLoopEvent) :
         (pWorkLoopObj->hWorkLoopEventTail->nextEvent = pWorkLoopEvent);
      pWorkLoopObj->hWorkLoopEventTail = pWorkLoopEvent;

      pWorkLoopEvent->dwState = WL_EVENT_ENQUEUED;
      pWorkLoopEvent->nextEvent = NULL;

      DALSYS_SyncLeave(pWorkLoopObj->hSync);
   }

   /* Trigger the Work Loop signal */
   qurt_anysignal_set(&pWorkLoopObj->signal, SIGNAL_EVENT);
   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
User API to Add Events to Work Loop
------------------------------------------------------------------------------*/
DALResult
DALSYS_AddEventToWorkLoop(DALSYSWorkLoopHandle hWorkLoop,
                    DALSYSWorkLoopExecute pfnWorkLoopExecute,
                    void * pArg,
                    DALSYSEventHandle hEvent,
                    DALSYSSyncHandle hSync)
{
   _DALSYSEventObj *pEvent = (_DALSYSEventObj *)hEvent;
   _DALSYSWorkLoopEvent *pWorkLoopEvent = &(pEvent->p.wl);

   if(!hWorkLoop || !hEvent || !pfnWorkLoopExecute ||
      !DALSYSCMN_IS_WORKLOOP_EVENT(((DALSYSBaseObj *)hEvent)->dwObjInfo))
   {
      DALSYS_LOG_ERROR_EVENT("Failed- hWorkLoop:0x%x hEvent:0x%x", 
            hWorkLoop, hEvent);
      return DAL_ERROR;
   }

   pWorkLoopEvent->hWorkLoop = hWorkLoop;
   pWorkLoopEvent->pWorkLoopFunc = pfnWorkLoopExecute;
   pWorkLoopEvent->pArg = pArg;
   pWorkLoopEvent->hSync = hSync;
   pWorkLoopEvent->dwState = WL_EVENT_DEQUEUED;
   pWorkLoopEvent->nextEvent = NULL;

   return DAL_SUCCESS;
}

/*------------------------------------------------------------------------------
User API to delete events from Work Loop queue
------------------------------------------------------------------------------*/
DALResult
DALSYS_DeleteEventFromWorkLoop(DALSYSWorkLoopHandle hWorkLoop,
                         DALSYSEventHandle hEvent)
{
   _DALSYSEventObj *pEvent = (_DALSYSEventObj *)hEvent;
   _DALSYSWorkLoopEvent *pWorkLoopEvent = &(pEvent->p.wl);
   DALResult result;

   if(!hEvent ||
      !DALSYSCMN_IS_WORKLOOP_EVENT(((DALSYSBaseObj *)hEvent)->dwObjInfo))
   {
     DALSYS_LOG_ERROR_EVENT("Failed- hWorkLoop:0x%x hEvent:0x%x", 
         hWorkLoop, hEvent);
      return DAL_ERROR;
   }

   // remove event from workloop queue
   result = dequeue(pWorkLoopEvent);
   if (DAL_SUCCESS != result)
      return result;

   pWorkLoopEvent->hWorkLoop = NULL;
   return DAL_SUCCESS;
}

DALResult
DALSYS_DestroyWorkLoopObject(DALSYSWorkLoopHandle hWorkLoop)
{
   _DALSYSWorkLoopObj *pWorkLoopObj = (_DALSYSWorkLoopObj *) hWorkLoop;

   if(DALSYSCMN_WORKLOOP_OBJECT_ID ==
        DALSYSCMN_GET_OBJECT_ID(pWorkLoopObj->dwObjInfo))
   {
      // send signal to kill the workloop
      qurt_anysignal_set(&pWorkLoopObj->signal, SIGNAL_EXIT);
      DALSYS_WorkLoopThreadJoin(pWorkLoopObj);

      // delete all events on this workloop
      // users are still responsible for calling DALSYS_DeleteEventFromWorkLoop and 
      // DALSYS_DestroyObject on the events
      while (pWorkLoopObj->hWorkLoopEventHead)
      {
         _DALSYSWorkLoopEvent *pRemovedWLEvent = pWorkLoopObj->hWorkLoopEventHead;
         pWorkLoopObj->hWorkLoopEventHead = pRemovedWLEvent->nextEvent;
         pRemovedWLEvent->dwState = WL_EVENT_DEQUEUED;
         pRemovedWLEvent->nextEvent = NULL;
      }
      pWorkLoopObj->hWorkLoopEventTail = NULL;

      qurt_anysignal_destroy(&pWorkLoopObj->signal);

      if(pWorkLoopObj->hSync)
         DALSYS_DestroyObject(pWorkLoopObj->hSync);

      if (pWorkLoopObj->pStackBuffer)
         DALSYS_Free(pWorkLoopObj->pStackBuffer);

      if(DALSYSCMN_IS_OBJ_DYNAMICALLY_ALLOCED(pWorkLoopObj->dwObjInfo))
         DALSYS_Free(pWorkLoopObj); /* If dynamically alloced free memory */
      else /*Statically alloced object, reset all fields to ZERO*/
         memset(pWorkLoopObj,0,sizeof(_DALSYSWorkLoopObj));

      return DAL_SUCCESS;
   }

   // ensure workloop object size is valid
   DALSYSCMN_COMPILER_ASSERT((sizeof(_DALSYSWorkLoopObj) > 
            DALSYS_WORKLOOP_OBJ_SIZE_BYTES));

   DALSYS_LOG_ERROR_EVENT("not a workloop object- pWorkLoopObj:0x%x",
         pWorkLoopObj);
   return DAL_ERROR;
}
