/*=============================================================================

FILE:         qdss_ts.c

DESCRIPTION:  QDSS timer related functions. It is currently only 
              used in debug builds for delayed start of ETM tracing.

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#include "qdss_control_priv.h"
#include "tracer_event_ids.h"
#include "qdss_ts_micro.h"

#include "qdss_control.h"
#include "qdss_ts.h"

/**
  @brief  QDSS TS timer call back
 */
void qdss_ts_timer_callback(void) 
{
   if (0==qdss.ts.timer_callbacks) {
      qdss_control_set_etm(1);
   }
   qdss.ts.timer_callbacks++;
   qdss_ts_stop_timer();
}

extern void (*pfn_qdss_ts_timer_callback)(void);



/*-------------------------------------------------------------------------*/

/**
  @brief Starts the TS timer

  @return 0 if successful, error code otherwise
 */
int qdss_ts_start_timer(void)
{
   int nErr = 0;

   if (NULL == qdss.ts.hTimerEvent) {
      pfn_qdss_ts_timer_callback=qdss_ts_timer_callback;
      TRY(nErr,DAL_TimerDeviceAttach(DALDEVICEID_TIMER, &qdss.ts.hTimer));

      TRY(nErr,DALSYS_EventCreate
          (DALSYS_EVENT_ATTR_CALLBACK_EVENT | DALSYS_EVENT_ATTR_TIMER_EVENT|DALSYS_EVENT_ATTR_NON_DEFERRED,
           &qdss.ts.hTimerEvent,
           &qdss.ts.hTimerObject));

      TRY(nErr, DALSYS_SetupCallbackEvent
          (qdss.ts.hTimerEvent,
           qdss_ts_timer_callback_micro, (DALSYSCallbackFuncCtx) &qdss));
   }

   if ( (qdss.ts.timer_us > 0) && (!qdss.ts.bStarted)) {
      TRY(nErr,DalTimer_Register(qdss.ts.hTimer,qdss.ts.hTimerEvent, qdss.ts.timer_us ));
      qdss.ts.bStarted = TRUE;
   }


   CATCH(nErr) {}

   return nErr;
}

/*-------------------------------------------------------------------------*/

/**
  @brief  Stops the TS timer


  @return 0 if successful, error code otherwise
 */
int qdss_ts_stop_timer(void)
{
   int nErr = 0;

   if ((NULL != qdss.ts.hTimer)  &&  (NULL != qdss.ts.hTimerEvent))  {
      TRY(nErr,DalTimer_UnRegister(qdss.ts.hTimer,qdss.ts.hTimerEvent));
      qdss.ts.bStarted = FALSE;
   }

   CATCH(nErr) {}

   return nErr;


}
