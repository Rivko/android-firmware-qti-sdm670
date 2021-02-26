/*==============================================================================
@file  DALSYSEvent.c

DALSYS Event implementation

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "qurt.h"
#include "string.h"


/*=============================================================================
                    DEFINITIONS AND DECLARATIONS
=============================================================================*/

/*=============================================================================
      Constants and Macros
=============================================================================*/

/*=============================================================================
      Structures
=============================================================================*/

/*=============================================================================
                       FUNCTION DECLARATIONS FOR MODULE
=============================================================================*/
/*
   AllocateEvent() - internal helper function
*/

static DALResult AllocateEvent(_DALSYSEventObj **pEventObj)
{
   if(*pEventObj)
   {
      DALSYSCMN_RESET_ALL_BITS((*pEventObj)->dwObjInfo);
   }
   else
   {
      /*----------------------------------------------------------------------
      User has not passed in Event object, we need to allocate memory
      Always get the size form the DALSYS Macros. This is done to support
      static allocation and maintain consistency between sizes of statically
      and dynamically alloced structures.
      ASSERT COMPILER TIME if our structure exceeds the static size. This
      compiler error will show up as "case value '1' already exist"
      ----------------------------------------------------------------------*/
      DALSYSCMN_COMPILER_ASSERT((sizeof(_DALSYSEventObj) >
                                DALSYS_EVENT_OBJ_SIZE_BYTES));

      if(DAL_SUCCESS != DALSYS_Malloc(DALSYS_EVENT_OBJ_SIZE_BYTES,
                                      (void**)pEventObj))
            return DAL_ERROR;

      DALSYS_memset((*pEventObj),0,DALSYS_EVENT_OBJ_SIZE_BYTES);
      DALSYSCMN_SET_DYNAMIC_ALLOC_BIT((*pEventObj)->dwObjInfo);
   }
   (*pEventObj)->thisVirtualAddr = (DALSYSMemAddr)(*pEventObj);
   return DAL_SUCCESS;
}

/*
   DALSYS_EventInit()
*/
DALResult DALSYS_EventInit(DALSYSEventHandle hEvent)
{
   _DALSYSEventObj *pEventObj = (_DALSYSEventObj *) hEvent;

   if(pEventObj)
   {
      pEventObj->threadId = qurt_thread_get_id();
      if (!DALSYSCMN_IS_EVENT_A_CALLBACK(pEventObj->dwObjInfo) && 
          !DALSYSCMN_IS_WORKLOOP_EVENT(pEventObj->dwObjInfo))
      {
         pEventObj->p.sig.anySignal.raw = 0;
         pEventObj->p.sig.pLinkSignal = NULL;
      }
      return DAL_SUCCESS;
   }
   DALSYS_LOG_ERROR_EVENT("Failed- hEvent:0x%x", hEvent);
   return DAL_ERROR;
}

/*
   DALSYS_EventCreate()
*/
DALResult
DALSYS_EventCreate(uint32 dwEvtAttrib,  DALSYSEventHandle *phEvent,
                                   DALSYSEventObj *pObj)
{
   _DALSYSEventObj * pEventObj = (_DALSYSEventObj *)pObj;
   DALBOOL bInit = 1;

   if (!phEvent)
   {
      DALSYS_LOG_ERROR_EVENT("phEvent is NULL");
      return DAL_ERROR;
   }
   *phEvent = NULL;

   // cant be both timeout and interval timer event
   if((dwEvtAttrib & DALSYSCMN_EVENT_ATTR_TIMER_MASK)==DALSYSCMN_EVENT_ATTR_TIMER_MASK)
   {
      DALSYS_LOG_ERROR_EVENT(
         "Event cannot have both timeout and timer attributes in DALSYS_EventCreate- phEvent:0x%x dwEvtAttrib:0x%x",
         phEvent, dwEvtAttrib);
      return DAL_ERROR;
   }

   // allocate event object
   if(DAL_SUCCESS != AllocateEvent(&pEventObj))
   {
      DALSYS_LOG_ERROR_EVENT(
         "Failed to allocate event in DALSYS_EventCreate- phEvent:0x%x",
         phEvent);      
      return DAL_ERROR;
   }

   // initialize the event struct
   DALSYSCMN_SET_OBJECT_ID(pEventObj->dwObjInfo,DALSYSCMN_EVENT_OBJECT_ID);
   pEventObj->threadId = 0;
   pEventObj->pTimerObj = NULL;
   *phEvent = (DALSYSEventHandle )pEventObj;

   if(dwEvtAttrib & DALSYS_EVENT_ATTR_WORKLOOP_EVENT)
   {
      DALSYSCMN_SET_WORKLOOP_EVENT_BIT(pEventObj->dwObjInfo);
      pEventObj->p.wl.hWorkLoop = NULL;
      bInit = 0;
   }

   if(dwEvtAttrib & DALSYS_EVENT_ATTR_CALLBACK_EVENT)
   {
      DALSYSCMN_SET_CALLBACK_EVENT_BIT(pEventObj->dwObjInfo);
      bInit = 0;
      pEventObj->p.cb.cbFunc = NULL;
      pEventObj->p.cb.cbFuncCtx = NULL;
   }

   if(dwEvtAttrib & DALSYS_EVENT_ATTR_TIMER_EVENT)
   {
      DALSYSCMN_SET_TIMER_EVENT_BIT(pEventObj->dwObjInfo);
   }

   if(dwEvtAttrib & DALSYS_EVENT_ATTR_TIMEOUT_EVENT)
   {
      DALSYSCMN_SET_TIMEOUT_EVENT_BIT(pEventObj->dwObjInfo);
      DALSYSCMN_SET_CLIENT_DEFAULT_EVENT_BIT(pEventObj->dwObjInfo);
   }

   if(dwEvtAttrib & DALSYS_EVENT_ATTR_CLIENT_DEFAULT )
   {
      DALSYSCMN_SET_CLIENT_DEFAULT_EVENT_BIT(pEventObj->dwObjInfo);
   }

   if(dwEvtAttrib & DALSYS_EVENT_ATTR_NON_DEFERRED)
   {
      DALSYSCMN_SET_EVENT_NON_DEFERRED_BIT(pEventObj->dwObjInfo);
   }

   if(bInit)
   {
      qurt_anysignal_init(&pEventObj->p.sig.anySignal);
      qurt_pimutex_init(&pEventObj->p.sig.linkMutex);
      pEventObj->threadId = qurt_thread_get_id();
      pEventObj->p.sig.pLinkSignal = NULL;
   }
   return DAL_SUCCESS;
}

extern DALResult DALSYS_EnqueueAndTrigger(_DALSYSWorkLoopEvent * pWorkLoopEvent);
/*
   DALSYS_EventCtrl()
*/

DALResult
DALSYS_EventCtrlEx(DALSYSEventHandle hEvent, uint32 dwCtrl,uint32 dwParam,
                                  void *pPayload, uint32 dwPayloadSize)
{
   _DALSYSEventObj *pEventObj = (_DALSYSEventObj *) DALSYSCMN_HANDLE_TO_OBJECT(hEvent);
   DALResult nErr = DAL_ERROR;

   if(pEventObj)
   {
      switch(dwCtrl)
      {
         case DALSYS_EVENT_CTRL_TRIGGER:
            if(DALSYSCMN_IS_EVENT_A_CALLBACK(pEventObj->dwObjInfo))
            {
               if(pEventObj->p.cb.cbFunc)
               {
                  pEventObj->p.cb.cbFunc(pEventObj->p.cb.cbFuncCtx,
                                    dwParam,pPayload,dwPayloadSize);
                  nErr = DAL_SUCCESS;
               }
            }
            else if(DALSYSCMN_IS_WORKLOOP_EVENT(pEventObj->dwObjInfo))
            {
                nErr = DALSYS_EnqueueAndTrigger(&(pEventObj->p.wl));
            }
            else
            {
               if(pEventObj->threadId)
               {
                  qurt_pimutex_lock(&pEventObj->p.sig.linkMutex);
                  qurt_anysignal_set(&pEventObj->p.sig.anySignal, 
                        EVENT_DEFAULT_SIGNAL);
                  if (pEventObj->p.sig.pLinkSignal)
                  {
                     qurt_anysignal_set(pEventObj->p.sig.pLinkSignal, EVENT_GROUP_SIGNAL);
                  }
                  qurt_pimutex_unlock(&pEventObj->p.sig.linkMutex);
                  nErr = DAL_SUCCESS;
               }
            }
            break;

         case DALSYS_EVENT_CTRL_RESET:
            if (DALSYSCMN_IS_EVENT_A_CALLBACK(pEventObj->dwObjInfo))
            {
               /* resetting a callback event is a noop*/
               nErr = DAL_SUCCESS;
            }
            else if(DALSYSCMN_IS_WORKLOOP_EVENT(pEventObj->dwObjInfo))
            {
               /* resetting a workloop event is a noop*/
               nErr = DAL_SUCCESS;
            }
            else 
            {
               if (DALSYSCMN_IS_EVENT_A_CLIENT_DEFAULT(pEventObj->dwObjInfo))
               {
                  nErr = DALSYS_EventInit((DALSYSEventHandle )pEventObj);
               }
               else
               {
                  // re-init the event except the thread Id cannot be modified
                  pEventObj->p.sig.anySignal.raw = 0;
                  pEventObj->p.sig.pLinkSignal = NULL;
                  nErr = DAL_SUCCESS;
               }
            }
            break;
         case DALSYS_EVENT_CTRL_ACCQUIRE_OWNERSHIP:
            if (DALSYSCMN_IS_EVENT_A_CALLBACK(pEventObj->dwObjInfo))
            {  
               /* acquiring an ownership for callback event is a noop*/
               nErr = DAL_SUCCESS;
            }
            else if (DALSYSCMN_IS_WORKLOOP_EVENT(pEventObj->dwObjInfo))
            {
               /* acquiring an ownership for workloop event is a noop*/
               nErr = DAL_SUCCESS;
            }
            else 
            {
               if (DALSYSCMN_IS_EVENT_A_CLIENT_DEFAULT(pEventObj->dwObjInfo))
               {
                  pEventObj->threadId = qurt_thread_get_id();  // give ownership to this thread
                  nErr = DAL_SUCCESS;
               }
               else
               {  
                  /* acquiring an ownership for non client default timer event is a noop*/
                  nErr = DAL_SUCCESS;
               }
            }
            break;
         default:
            DALSYS_LOG_ERROR_EVENT("Invalid command- hEvent:0x%x dwCtrl:0x%x",
                  hEvent, dwCtrl);
            break;
      }
   }

   return nErr;
}

/*
   DALSYS_EventWait()
*/

DALResult
DALSYS_EventWait(DALSYSEventHandle hEvent)
{
   _DALSYSEventObj *pEventObj = (_DALSYSEventObj *) DALSYSCMN_HANDLE_TO_OBJECT(hEvent);

   if(pEventObj && (pEventObj->threadId == qurt_thread_get_id()))
   {
      if(EVENT_DEFAULT_SIGNAL & 
         qurt_anysignal_wait(&pEventObj->p.sig.anySignal, 
            EVENT_DEFAULT_SIGNAL))
      {
         qurt_anysignal_clear(&pEventObj->p.sig.anySignal, 
               EVENT_DEFAULT_SIGNAL);
         return DAL_SUCCESS;
      }
   }
   DALSYS_LOG_ERROR_EVENT("Failed- hEvent:0x%x", hEvent);
   return DAL_ERROR;
}

/*
   DALSYS_EventMultipleWait()
*/

DALResult
DALSYS_EventMultipleWait(DALSYSEventHandle* phEvent, int nEvents,
                          uint32 timeout, uint32 *pdwEventIdx)
{
   uint32 signalValue;
   uint32 idx;
   uint32 hasTimeoutEvent = 0;
   uint32 master_idx = 0;
   _DALSYSEventObj *pMasterEventObj;
   DALResult ret;

   // check the events to ensure they are valid, find the first timeout event,
   for (idx = 0; idx < nEvents; idx++)
   {
      _DALSYSEventObj *pEventObj = (_DALSYSEventObj *)
         DALSYSCMN_HANDLE_TO_OBJECT(phEvent[idx]);

      if (NULL == pEventObj)
      {
         DALSYS_LOG_ERROR_EVENT("Event Id %d is NULL\n", idx);
         return DAL_ERROR;
      }

      if(DALSYSCMN_IS_EVENT_A_CALLBACK(pEventObj->dwObjInfo) ||
         DALSYSCMN_IS_WORKLOOP_EVENT(pEventObj->dwObjInfo) ||
         DALSYSCMN_IS_EVENT_A_TIMER(pEventObj->dwObjInfo))
      {
         // invalid event
         DALSYS_LOG_ERROR_EVENT(
               "Event Id %d is of invalid type; dwObjInfo=0x%x\n", 
               idx, pEventObj->dwObjInfo);
         return DAL_ERROR;
      }

      if (pEventObj->threadId != qurt_thread_get_id())
      {
         // cannot wait on event owned by another thread
         DALSYS_LOG_ERROR_EVENT(
               "Event Id %d is not owned by current thread 0x%x; event thread=0x%x\n", 
               idx, qurt_thread_get_id(), pEventObj->threadId);
         return DAL_ERROR;
      }

      if(timeout != DALSYS_EVENT_TIMEOUT_INFINITE && 
         DALSYSCMN_IS_EVENT_A_TIMEOUT(pEventObj->dwObjInfo))
      {
         // no actions if a timeout event is already found
         if (!hasTimeoutEvent)
         {
            if (0 == timeout)
            {
               // timed out immediately
               DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_WARNING, 
                  "Timeout value of zero is provided\n");
               return DAL_ERROR_TIMEOUT;
            }
            hasTimeoutEvent = 1;
            master_idx = idx;
         }
         else
         {
            // allow only one time out event if time out value is given, 
            // no time out events are allowed otherwise
            // time-out event triggered by non-ATS timer does not work with this function
            DALSYS_LOG_ERROR_EVENT(
               "Too many time-out events\n");
            return DAL_ERROR;
         }
      }
   }

   if (hasTimeoutEvent)
   {
        DALResult result = DALSYS_TimerStart( phEvent[master_idx], timeout);
         if (DAL_SUCCESS != result)
         {
            DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_ERROR, 
                            "Unable to start timer\n");
           return result;
         }
      }

   pMasterEventObj = (_DALSYSEventObj *)
      DALSYSCMN_HANDLE_TO_OBJECT(phEvent[master_idx]);
   pMasterEventObj->p.sig.pLinkSignal = NULL;

   // we are free to clear the group signal here as the group signal value
   // is valid only in the context of EventMultipleWait for the 
   // master event owner; therefore its previous value has no meaning
   qurt_anysignal_clear(&pMasterEventObj->p.sig.anySignal, EVENT_GROUP_SIGNAL);

   // link all events to master event
   for (idx = 0; idx < nEvents; idx++)
   {
      _DALSYSEventObj *pEventObj = (_DALSYSEventObj *)
         DALSYSCMN_HANDLE_TO_OBJECT(phEvent[idx]);

      if (idx != master_idx)
      {
         qurt_pimutex_lock(&pEventObj->p.sig.linkMutex);
         signalValue = qurt_anysignal_get(&pEventObj->p.sig.anySignal);
         if (!(signalValue & EVENT_DEFAULT_SIGNAL))
         {
            pEventObj->p.sig.pLinkSignal = &pMasterEventObj->p.sig.anySignal;
         }
         qurt_pimutex_unlock(&pEventObj->p.sig.linkMutex);

         if (signalValue & EVENT_DEFAULT_SIGNAL)
         {
            // event is already set
            *pdwEventIdx = idx;
            ret = DAL_SUCCESS;
            DALSYS_LogEvent(NULL, DALSYS_LOGEVENT_INFO, 
               "Event Id %d is already set\n", 
               idx);

            qurt_anysignal_clear(&pEventObj->p.sig.anySignal, 
                  EVENT_DEFAULT_SIGNAL);
            goto DALSYS_EventMultipleWait_cleanup;
         }
      }
   }

   signalValue = qurt_anysignal_wait(&pMasterEventObj->p.sig.anySignal, 
            EVENT_DEFAULT_SIGNAL | EVENT_GROUP_SIGNAL);

   if (signalValue & EVENT_DEFAULT_SIGNAL)
   {
      // master event is signaled
      qurt_anysignal_clear(&pMasterEventObj->p.sig.anySignal, EVENT_DEFAULT_SIGNAL);
      if (hasTimeoutEvent)
      {
         ret = DAL_ERROR_TIMEOUT;
      }
      else
      {
         *pdwEventIdx = master_idx;
         ret = DAL_SUCCESS;
      }
   }
   else if (signalValue & EVENT_GROUP_SIGNAL)
   {
      ret = DAL_ERROR;

      // find the triggered event
      for (idx = 0; idx < nEvents; idx++)
      {
         _DALSYSEventObj *pEventObj = (_DALSYSEventObj *)
            DALSYSCMN_HANDLE_TO_OBJECT(phEvent[idx]);
         if (EVENT_DEFAULT_SIGNAL & 
             qurt_anysignal_get(&pEventObj->p.sig.anySignal))
         {
            qurt_anysignal_clear(&pEventObj->p.sig.anySignal, EVENT_DEFAULT_SIGNAL);
            *pdwEventIdx = idx;
            ret = DAL_SUCCESS;
            break;
         }
      }

      if (DAL_ERROR == ret)
      {
         DALSYS_LOG_ERROR_EVENT(
               "Group signal is triggered but no events were triggered");
      }
   }
   else
   {
      // got an unexpected signal
      DALSYS_LOG_FATAL_EVENT("Unexpected signal 0x%x is set");
      ret = DAL_ERROR;
   }

DALSYS_EventMultipleWait_cleanup:
   // stop timer
   if (hasTimeoutEvent)
   {
        DALSYS_TimerStop(pMasterEventObj);
   }

   // clear all event linkage
   for (idx = 0; idx < nEvents; idx++)
   {
      _DALSYSEventObj *pEventObj = (_DALSYSEventObj *)
         DALSYSCMN_HANDLE_TO_OBJECT(phEvent[idx]);
      qurt_pimutex_lock(&pEventObj->p.sig.linkMutex);
      pEventObj->p.sig.pLinkSignal = NULL;
      qurt_pimutex_unlock(&pEventObj->p.sig.linkMutex);
   }
   return ret;
}

/*
   DALSYS_DestroyEventObject()
*/

DALResult
DALSYS_DestroyEventObject(DALSYSEventHandle hEvent)
{
   _DALSYSEventObj *pEventObj = (_DALSYSEventObj *) DALSYSCMN_HANDLE_TO_OBJECT(hEvent);
   DALResult nErr = DAL_ERROR;

   if(DALSYSCMN_HANDLE_IS_COPY(hEvent))
      return DAL_SUCCESS;  // cant destroy a handle copy

   if(DALSYSCMN_EVENT_OBJECT_ID == DALSYSCMN_GET_OBJECT_ID(pEventObj->dwObjInfo))
   {
      if(DALSYSCMN_IS_WORKLOOP_EVENT(pEventObj->dwObjInfo))
      {
         DALSYS_DeleteEventFromWorkLoop(pEventObj->p.wl.hWorkLoop, pEventObj);
      }
      else if (!DALSYSCMN_IS_EVENT_A_CALLBACK(pEventObj->dwObjInfo))
      {
         qurt_anysignal_destroy(&pEventObj->p.sig.anySignal);
         // lock the mutex to ensure no threads are holding on to the mutex
         // this prevent a race condition where thread A triggers an event 
         // waited by thread B, which causes thread B to wake up and may 
         // destroy the event before thread A unlocks the mutex below
         qurt_pimutex_lock(&pEventObj->p.sig.linkMutex);
         qurt_pimutex_destroy(&pEventObj->p.sig.linkMutex);
      }
      
      if (pEventObj->dwObjInfo & DALSYSCMN_EVENT_ATTR_TIMER_MASK)
      {
         DALSYS_DestroyEventTimerObject(pEventObj);
      }
      
      // Was the memory dynamically allocated
      if(DALSYSCMN_IS_OBJ_DYNAMICALLY_ALLOCED(pEventObj->dwObjInfo))
      {
         nErr = DALSYS_Free(pEventObj);
      }
      else
      {
         DALSYSCMN_DESTROY_OBJECT(pEventObj->dwObjInfo);
         nErr = DAL_SUCCESS;
      }
   }

   return nErr;
}

DALResult DALSYS_SetupCallbackEvent(DALSYSEventHandle     hEvent,
                                DALSYSCallbackFunc    cbFunc,
                                DALSYSCallbackFuncCtx cbFuncCtx)
{
   _DALSYSEventObj *pEventObj = (_DALSYSEventObj *) DALSYSCMN_HANDLE_TO_OBJECT(hEvent);

   if(pEventObj && DALSYSCMN_IS_EVENT_A_CALLBACK(pEventObj->dwObjInfo))
   {
      pEventObj->p.cb.cbFunc = cbFunc;
      pEventObj->p.cb.cbFuncCtx = cbFuncCtx;
      return DAL_SUCCESS;
   }
   DALSYS_LOG_ERROR_EVENT("failed- hEvent:0x%x", hEvent);
   return DAL_ERROR;
}
