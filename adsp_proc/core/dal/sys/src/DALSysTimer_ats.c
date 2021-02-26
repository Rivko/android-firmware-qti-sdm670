/*==============================================================================
@file  DALSYSTimer_ats.c

DALSYS timer implementation using ATS timer

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
#include "timer.h"

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
static void timeoutCB(timer_cb_data_type data)
{
    int hEventValue = (int)data; // needed to avoid compiler warning
    DALSYSEventHandle hEvent = (DALSYSEventHandle)hEventValue;

    if(hEvent)
        DALSYS_EventCtrl(hEvent, DALSYS_EVENT_CTRL_TRIGGER);
}

DALResult DALSYS_TimerStart(DALSYSEventHandle hEvent, uint32 time) 
{
    _DALSYSEventObj * pEventObj = (_DALSYSEventObj *)hEvent;
   timer_error_type timerRet;
   DALResult dalRet;

   if (!hEvent) return DAL_ERROR_INVALID_HANDLE;

   // allocate and initialize timer object
   if ( !pEventObj->pTimerObj )
   {
      int hEventValue = (int)hEvent; // used to avoid compiler warnings

      dalRet = DALSYS_Malloc(sizeof(timer_type), 
            (void**)&pEventObj->pTimerObj);

      if (DAL_ERROR_RETURNED(dalRet)) return dalRet;

      if(DALSYSCMN_IS_EVENT_NON_DEFERRED(pEventObj->dwObjInfo))
      {
         timerRet = timer_def_osal( pEventObj->pTimerObj, 
               &timer_non_defer_group, 
               TIMER_FUNC1_CB_TYPE, 
               (timer_t1_cb_type)timeoutCB,
               (timer_cb_data_type)hEventValue);
      }
      else
      {
         timerRet = timer_def_osal( pEventObj->pTimerObj, 
               NULL, 
               TIMER_FUNC1_CB_TYPE, 
               (timer_t1_cb_type)timeoutCB,
               (timer_cb_data_type)hEventValue);
      }

      if (TE_SUCCESS != timerRet)
      {
         DALSYS_Free( pEventObj->pTimerObj );
         pEventObj->pTimerObj = NULL;
         return DAL_ERROR_INTERNAL;
      }
   }

   if (time)
   {
      if(DALSYSCMN_IS_EVENT_A_TIMER(pEventObj->dwObjInfo))
      {
         // periodic
         timerRet = timer_set_64(pEventObj->pTimerObj, time, time, T_USEC);
      }
      else
      {
         // one-shot
         timerRet = timer_set_64(pEventObj->pTimerObj, time, 0, T_USEC);
      }

      if (TE_SUCCESS != timerRet) return DAL_ERROR_INTERNAL;
   }
   else
   {
      timer_clr_64( pEventObj->pTimerObj, T_USEC );
   }
   return DAL_SUCCESS;
}

DALResult DALSYS_TimerStop(DALSYSEventHandle hEvent) 
{
   _DALSYSEventObj * pEventObj = (_DALSYSEventObj *)hEvent;

   if (!hEvent) return DAL_ERROR_INVALID_HANDLE;

   // free underlying timer
   if(pEventObj->pTimerObj)
   {
      timer_clr_64( pEventObj->pTimerObj, T_USEC );
   }
   return DAL_SUCCESS;
}

void DALSYS_DestroyEventTimerObject(DALSYSEventHandle hEvent)
{
   _DALSYSEventObj * pEventObj = (_DALSYSEventObj *)hEvent;

   if(pEventObj && pEventObj->pTimerObj)
   {
      timer_clr_64( pEventObj->pTimerObj, T_USEC );
      timer_undef( pEventObj->pTimerObj );
      DALSYS_Free( pEventObj->pTimerObj );
      pEventObj->pTimerObj = NULL;
   }
}
