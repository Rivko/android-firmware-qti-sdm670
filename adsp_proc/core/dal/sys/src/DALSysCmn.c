/*==============================================================================
@file  DALSYSCmn.c

DALSYS initialization, common and any miscellaneous functions

        Copyright © 2013 Qualcomm Technologies Incorporated.
        All Rights Reserved.
        Qualcomm Confidential and Proprietary
==============================================================================*/
#include "qurt_atomic_ops.h"
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "DALStdErr.h"
#include "DALFramework.h"

#define MOD_UNINIT            0x0
#define MOD_INIT_IN_PROGRESS  0x1
#define MOD_INIT_DONE         0x2

// extern function declarations
extern void DALDeviceFwk_Init(void);
extern void DALDeviceFwk_DeInit(void);
extern void _DALSYS_InitMod(DALSYSConfig *pCfg);
extern void DALSYSWorkLoopInit(void);

/*------------------------------------------------------------------------------
DALSYS client stubs. Note - the Function pointer table intially points to
UnInitialized functions. Only After the DALSYS_InitMod will the proper entry
points be initialized
------------------------------------------------------------------------------*/
DALResult
DALSYS_MemDescAddBuf(DALSysMemDescList * pMemDesc, uint32 dwMemDescBufIdx,
                     uint32 dwOffset, uint32 dwSize)
{
   DALSYS_LOG_ERROR_EVENT("unsupported");
   return DAL_ERROR;
}

DALResult
DALSYS_MemDescPrepare(DALSysMemDescList * pMemDesc, uint32 dwBufIdx)
{
   DALSYS_LOG_ERROR_EVENT("unsupported");
   return DAL_ERROR;
}

DALResult
DALSYS_MemDescValidate(DALSysMemDescList * pMemDesc,uint32 dwBufIdx)
{
   DALSYS_LOG_ERROR_EVENT("unsupported");
   return DAL_ERROR;
}

DALResult
DALSYS_DestroyObject(DALSYSObjHandle hObj)
{
    DALSYSBaseObj * pSysObj = (DALSYSBaseObj *) DALSYSCMN_HANDLE_TO_OBJECT(hObj);

   if(NULL == pSysObj)
      return DAL_SUCCESS;

    /*--------------------------------------------------------------------------
    Determine type of DALSYS object and invoke appropriate destructor
    --------------------------------------------------------------------------*/
    switch(DALSYSCMN_GET_OBJECT_ID(pSysObj->dwObjInfo))
    {
    case DALSYSCMN_EVENT_OBJECT_ID:
      if(DALSYSCMN_HANDLE_IS_COPY(hObj))
      {
         /*---------------------------------------------------------------------
         are we doing payload events i.e. Callback or regular events i.e. WL 
         ---------------------------------------------------------------------*/   
         if(DALSYSCMN_IS_EVENT_A_CALLBACK(pSysObj->dwObjInfo))
            return DALSYS_DestroyEventObject(pSysObj);
      }
      else
      {
         return DALSYS_DestroyEventObject(hObj);
      }
    case DALSYSCMN_SYNC_OBJECT_ID:
        return DALSYS_DestroySyncObject(hObj);
    case DALSYSCMN_MEM_OBJECT_ID:
        return DALSYS_DestroyMemObject(hObj);
    case DALSYSCMN_WORKLOOP_OBJECT_ID:
        return DALSYS_DestroyWorkLoopObject(hObj);
    default:
        DALSYS_LOG_ERROR_EVENT(
            "Unsupported object type 0x%x- hObj:0x%x",
                  DALSYSCMN_GET_OBJECT_ID(pSysObj->dwObjInfo), hObj);
        return DAL_ERROR;
    }
}

DALResult
DALSYS_CopyObject(DALSYSObjHandle hObjOrig, DALSYSObjHandle *phObjCopy )
{
    DALSYSBaseObj * pObjOrig = (DALSYSBaseObj *)hObjOrig;
    /*--------------------------------------------------------------------------
    Determine type of DALSYS object and invoke appropriate destructor
    --------------------------------------------------------------------------*/
    switch(DALSYSCMN_GET_OBJECT_ID(pObjOrig->dwObjInfo))
    {
    case DALSYSCMN_MEM_OBJECT_ID:
        *phObjCopy = (DALSYSObjHandle *)DALSYSCMN_MemObjCopy((DALSYSMemObj *)hObjOrig,
                                                            NULL);
        if(*phObjCopy)
            return DAL_SUCCESS;
    default:
        DALSYS_LOG_ERROR_EVENT(
            "Unsupported object type 0x%x- hObjOrig:0x%x",
                  DALSYSCMN_GET_OBJECT_ID(pObjOrig->dwObjInfo), hObjOrig);
        return DAL_ERROR;
    }
}

uint32 DALSYS_SetThreadPriority(uint32 priority)
{
   uint32 cur_prio = (uint32)(0xFF&qurt_thread_get_priority(
            qurt_thread_get_id())); // current priority
   //will change only if within reasonable bounds
   if(priority > 1 && priority < 256)
   {
      qurt_thread_set_priority(qurt_thread_get_id(), (255-priority));
   }
   return (255-cur_prio);
}

void
DALSYS_BusyWait(uint32 pause_time_us)
{
   unsigned long long currentTime = qurt_sysclock_get_hw_ticks();
   unsigned long long waitInterval = 
      qurt_sysclock_timetick_from_us(pause_time_us);

   // poll and wait
   while (qurt_sysclock_get_hw_ticks() < currentTime + waitInterval);
}

void
DALSYS_InitMod(DALSYSConfig *pCfg)
{
   unsigned int volatile static DALSYSHasInit = MOD_UNINIT;
   
   // ensure initialization below is done only once on one thread and
   // other threads wait till initialization is completed
   if (qurt_atomic_compare_and_set((unsigned int *)&DALSYSHasInit, 
      MOD_UNINIT, MOD_INIT_IN_PROGRESS))
   {
      _DALSYS_InitMod(pCfg);
#ifdef DEVCFG_STRICT_XML
       extern void devcfgManagerInit(void);
       devcfgManagerInit();
#endif
      DALDeviceFwk_Init();
      DALSYSWorkLoopInit();
      DALSYSHasInit = MOD_INIT_DONE;
      qurt_futex_wake((unsigned int *)&DALSYSHasInit, 0xffff);
   }
   else
   {
      while (MOD_INIT_DONE != DALSYSHasInit)
      {
         qurt_futex_wait((unsigned int *)&DALSYSHasInit, DALSYSHasInit);
      }
   }
}

void
DALSYS_Init(void)
{
   DALSYS_InitMod(NULL);
}


void
DALSYS_DeInitMod(void)
{
   DALDeviceFwk_DeInit();
}
