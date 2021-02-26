/*==============================================================================
@file  DALSysSync.c

DAL SYS Synchronization implementation for QURT

        Copyright © 2013 Qualcomm Technologies Incorporated.
               All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "qurt.h"
#include "string.h"
#include <dalconfig.h>

#ifdef DALSYS_SYNC_USE_PIMUTEX
#define MUTEX_INIT      qurt_pimutex_init
#define MUTEX_LOCK      qurt_pimutex_lock
#define MUTEX_TRYLOCK   qurt_pimutex_try_lock
#define MUTEX_UNLOCK    qurt_pimutex_unlock
#define MUTEX_DESTROY   qurt_pimutex_destroy
#else
#define MUTEX_INIT      qurt_rmutex_init
#define MUTEX_LOCK      qurt_rmutex_lock
#define MUTEX_TRYLOCK   qurt_rmutex_try_lock
#define MUTEX_UNLOCK    qurt_rmutex_unlock
#define MUTEX_DESTROY   qurt_rmutex_destroy
#endif

typedef struct _DALSYSSyncObj _DALSYSSyncObj;
struct _DALSYSSyncObj
{    
   INHERIT_DALSYS_BASE_OBJ();
   uint32 orig_prio; // for preemption disable on QURT
   qurt_mutex_t hMutex;
};

static void sync_disable_preemption(_DALSYSSyncObj *pSyncObj)
{
   if(!pSyncObj->orig_prio) /* is this a nested call to disable preemption ? */
   {
      int qurt_id = qurt_thread_get_id();
      /* store original priority */
      pSyncObj->orig_prio = (0x0000FFFF & qurt_thread_get_priority(qurt_id));
      qurt_thread_set_priority(qurt_id, DALSYS_SYNC_CEILING_PRIO);
   }
   pSyncObj->orig_prio += 0x10000; /* maintain a count in the top 16 bits to support nested calls */
}

static void sync_enable_preemption(_DALSYSSyncObj *pSyncObj)
{
   pSyncObj->orig_prio -= 0x10000;
   if(!(pSyncObj->orig_prio&0xFFFF0000)) /* check count of nested calls*/
   {
      qurt_thread_set_priority(qurt_thread_get_id(), pSyncObj->orig_prio);
      pSyncObj->orig_prio = 0x0;
   }
}

DALResult
DALSYS_SyncCreate(uint32 dwAttribs, 
                  DALSYSSyncHandle *phSync,
                  DALSYSSyncObj *pObj)
{
   int nRet = DAL_ERROR;
   _DALSYSSyncObj *pSyncObj = (_DALSYSSyncObj *)pObj;

   *phSync = NULL;

   if(!pSyncObj)
   {
      if(DAL_SUCCESS == DALSYS_Malloc(sizeof(_DALSYSSyncObj),
                                      (void**)&pSyncObj))
      {
         memset(pSyncObj, 0, sizeof(_DALSYSSyncObj));
         DALSYSCMN_SET_DYNAMIC_ALLOC_BIT(pSyncObj->dwObjInfo);
         nRet = DAL_SUCCESS;
      }
      else
      {
         return DAL_ERROR;
      }
   }
   else
   {
      DALSYSCMN_RESET_ALL_BITS(pSyncObj->dwObjInfo);
   }

   DALSYSCMN_SET_OBJECT_ID(pSyncObj->dwObjInfo,DALSYSCMN_SYNC_OBJECT_ID);
   DALSYSCMN_SET_ATTRIB(pSyncObj->dwObjInfo, dwAttribs);
   pSyncObj->thisVirtualAddr = (DALSYSMemAddr)pSyncObj;

   switch(dwAttribs)
   {
      case DALSYS_SYNC_ATTR_RESOURCE:
      case DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT:
      case DALSYS_SYNC_ATTR_NO_PREEMPTION:
      case DALSYS_SYNC_ATTR_HARD_NO_PREEMPTION:
         MUTEX_INIT(&pSyncObj->hMutex);
         *phSync = pSyncObj;
         nRet = DAL_SUCCESS;
         break;

      default:
         *phSync = NULL;
         nRet = DAL_ERROR;
         DALSYS_Free(pSyncObj);
         DALSYS_LOG_ERROR_EVENT(
               "Unsupported attribute 0x%x", dwAttribs);
         break;
   }

   return nRet;
}

void
DALSYS_SyncEnter(DALSYSSyncHandle hSync)
{       
   _DALSYSSyncObj *pSyncObj = (_DALSYSSyncObj *) hSync;
   
   if(pSyncObj)
   {
      uint32 dwAttribs;
      DALSYSCMN_GET_ATTRIB(pSyncObj->dwObjInfo, dwAttribs);
      switch(dwAttribs)
      {
         case DALSYS_SYNC_ATTR_RESOURCE:
         case DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT:
            MUTEX_LOCK(&pSyncObj->hMutex);
            break;

         case DALSYS_SYNC_ATTR_NO_PREEMPTION:
         case DALSYS_SYNC_ATTR_HARD_NO_PREEMPTION:
            MUTEX_LOCK(&pSyncObj->hMutex);
            sync_disable_preemption(pSyncObj);
            break;

         default:
             DALSYS_LOG_FATAL_EVENT("invalid attribute 0x%x", dwAttribs );
             break;
      }
   }
   else
   {
      DALSYS_LOG_FATAL_EVENT("NULL handle");
   }
}

DALResult
DALSYS_SyncTryEnter(DALSYSSyncHandle hSync)
{
   _DALSYSSyncObj *pSyncObj = (_DALSYSSyncObj *) hSync;

   if(pSyncObj)
   {
      uint32 dwAttribs;
      int lockAttemptStatus;

      DALSYSCMN_GET_ATTRIB(pSyncObj->dwObjInfo, dwAttribs);
      switch(dwAttribs)
      {
         case DALSYS_SYNC_ATTR_RESOURCE:
         case DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT:
            lockAttemptStatus = MUTEX_TRYLOCK(&pSyncObj->hMutex);
            if (!lockAttemptStatus)
            {
               return DAL_SUCCESS;
            } 
            else 
            {
               return DAL_SYNC_ENTER_FAILED;
            }

         case DALSYS_SYNC_ATTR_NO_PREEMPTION:
         case DALSYS_SYNC_ATTR_HARD_NO_PREEMPTION:
            lockAttemptStatus = MUTEX_TRYLOCK(&pSyncObj->hMutex);
            if (!lockAttemptStatus)
            {
               sync_disable_preemption(pSyncObj);
               return DAL_SUCCESS;
            }
            else
            {
               return DAL_SYNC_ENTER_FAILED;
            }

         default:
             DALSYS_LOG_ERROR_EVENT("invalid attribute 0x%x", dwAttribs );
             return DAL_ERROR;
      }
   }
   else
   {
      DALSYS_LOG_ERROR_EVENT("NULL handle");
      return DAL_ERROR;
   }
}

void
DALSYS_SyncLeave(DALSYSSyncHandle hSync)
{
   _DALSYSSyncObj *pSyncObj = (_DALSYSSyncObj *) hSync;
   
   if(pSyncObj)
   {
      uint32 dwAttribs;
      DALSYSCMN_GET_ATTRIB(pSyncObj->dwObjInfo, dwAttribs);
      switch(dwAttribs)
      {
         case DALSYS_SYNC_ATTR_RESOURCE:
         case DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT:
            MUTEX_UNLOCK(&pSyncObj->hMutex);
            break;

         case DALSYS_SYNC_ATTR_NO_PREEMPTION:
         case DALSYS_SYNC_ATTR_HARD_NO_PREEMPTION:
            sync_enable_preemption(pSyncObj);
            MUTEX_UNLOCK(&pSyncObj->hMutex);
            break;
      
         default:
            DALSYS_LOG_FATAL_EVENT("invalid attribute 0x%x", dwAttribs );
            break;
      }
   }
   else
   {
      DALSYS_LOG_FATAL_EVENT("NULL handle");
   }
}

DALResult
DALSYS_DestroySyncObject(DALSYSSyncHandle hSync)
{
   _DALSYSSyncObj *pSyncObj = (_DALSYSSyncObj *) hSync;

   if(DALSYSCMN_SYNC_OBJECT_ID == DALSYSCMN_GET_OBJECT_ID(pSyncObj->dwObjInfo))
   {
      uint32 dwAttribs;
      DALSYSCMN_GET_ATTRIB(pSyncObj->dwObjInfo, dwAttribs);
      switch(dwAttribs)
      {
         case DALSYS_SYNC_ATTR_RESOURCE:
         case DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT:
         case DALSYS_SYNC_ATTR_NO_PREEMPTION:
         case DALSYS_SYNC_ATTR_HARD_NO_PREEMPTION:
            MUTEX_DESTROY(&pSyncObj->hMutex);
            break;
      
         default:
            DALSYS_LOG_ERROR_EVENT("Unsupported attribute 0x%x", dwAttribs);
            break;
      }
      /*----------------------------------------------------------------------
      If dynamically alloced free memory
      ----------------------------------------------------------------------*/
      if(DALSYSCMN_IS_OBJ_DYNAMICALLY_ALLOCED(pSyncObj->dwObjInfo))
         DALSYS_Free(pSyncObj);
      else
         DALSYSCMN_DESTROY_OBJECT(pSyncObj->dwObjInfo);

      return DAL_SUCCESS;                         
   }

   DALSYS_LOG_ERROR_EVENT("Invalid object Id- pSyncObj:0x%x", pSyncObj);
   return DAL_ERROR;
}
