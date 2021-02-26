/*========================================================================

*//** @file qurt_elite_timer.cpp
This file contains utilities of Timers , such as
timer create, start/restart,delete.

Copyright (c) 2009 Qualcomm Technologies, Incorporated.  All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government, Diversion contrary to U.S. law prohibited.
*//*====================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/elite/qurt_elite/src/qurt_elite_timer.cpp#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
06/13/09   mwc     Created file.

========================================================================== */


/* =======================================================================
INCLUDE FILES FOR MODULE
========================================================================== */
#include "qurt_elite.h"
#include "audio_basic_op.h"

#include "timer.h"

/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */


/* =======================================================================
**                          Function Definitions
** ======================================================================= */


/****************************************************************************
** Timers
*****************************************************************************/

/*
APIs: create, delete, get duration
   Handler: signal
   Timer Type: ONESHOT, PERIODIC, SLEEP
   Duration: In us
 */


/*
qurt_elite_timer_create:
*
* Default timer create, deferrable timer group
*
* @param[in] pTimer Pointer to qurt_elite timer object
* @param[in] timerType  One of : QURT_ELITE_TIMER_ONESHOT_DURATION,
*               QURT_ELITE_TIMER_PERIODIC,QURT_ELITE_TIMER_ONESHOT_ABSOLUTE
* @param[in] clockSource   Only QURT_ELITE_TIMER_USER is implemented currently.
* @param[in] pSignal pointer to signal to be generated when timer expires.
*
* @return indication of success (0) or failure (non-0)
*/
int qurt_elite_timer_create(qurt_elite_timer_t *pTimer, qurt_elite_timer_duration_t timerType,
        qurt_elite_timer_src_t clockSource, qurt_elite_signal_t *pSignal)
{
    qurt_elite_timer_attr_t pTimerAttr = {0};
    pTimerAttr.is_deferrable_group = TRUE;
    return qurt_elite_timer_create2(pTimer, timerType, &pTimerAttr, clockSource, pSignal);
}

/*
qurt_elite_timer_create2:
*
* timer create with defer setting parameter, can set deferrable/non-deferrable
*
* @param[in] pTimer Pointer to qurt_elite timer object
* @param[in] timerType  One of : QURT_ELITE_TIMER_ONESHOT_DURATION,
*               QURT_ELITE_TIMER_PERIODIC,QURT_ELITE_TIMER_ONESHOT_ABSOLUTE
* @param[in] pTimerAttr : timer attributes struct set by clients
* @param[in] clockSource   Only QURT_ELITE_TIMER_USER is implemented currently.
* @param[in] pSignal pointer to signal to be generated when timer expires.
*
* @return indication of success (0) or failure (non-0)
*/
int qurt_elite_timer_create2(qurt_elite_timer_t *pTimer, qurt_elite_timer_duration_t timerType,
        qurt_elite_timer_attr_t *pTimerAttr, qurt_elite_timer_src_t clockSource, qurt_elite_signal_t *pSignal)
{
    int nStatus;
    timer_group_ptr group_ptr = NULL;

    /* Do error checks */
    if (pTimer == NULL){
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "pTimer handle is NULL");
        return ADSP_EBADPARAM;
    }

    if (QURT_ELITE_TIMER_USER != clockSource)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Only USER TIMER supported for now");
        return ADSP_EBADPARAM;
    }

    pTimer->pChannel = qurt_elite_signal_get_channel(pSignal);

    if (NULL==pTimer->pChannel)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "signal does not belong to any channel");
        return ADSP_EBADPARAM;
    }

    pTimer->uTimerType = (unsigned int)timerType;
    pTimer->timer_sigmask = qurt_elite_signal_get_channel_bit(pSignal);
    pTimer->istimerCreated = FALSE;
    pTimer->duration = ATS_TIMER_MAX_DURATION;

    pTimer->qTimerObj = (unsigned int) qurt_elite_memory_malloc(sizeof(timer_type), QURT_ELITE_HEAP_DEFAULT);

    if (pTimer->qTimerObj == 0)
    {
        MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed creating timer because couldn't malloc sizeof(timer_type)");
        return ADSP_EFAILED;
    }

    pTimer->is_deferrable_group = pTimerAttr->is_deferrable_group;
    if (pTimerAttr->is_deferrable_group == FALSE)
    {
        group_ptr = &timer_non_defer_group;
    }

    nStatus = timer_def_osal((timer_ptr_type)(pTimer->qTimerObj), group_ptr,
            TIMER_NATIVE_OS_SIGNAL2_TYPE,(time_osal_notify_obj_ptr)&(pTimer->pChannel->anysig),
            (time_osal_notify_data)(pTimer->timer_sigmask));
    if ( TE_SUCCESS != nStatus)
    {
        MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed creating timer with status=%d!", nStatus);
        qurt_elite_memory_free((void *)pTimer->qTimerObj);
        return ADSP_EFAILED;
    }

    pTimer->istimerCreated = TRUE;

#ifdef DEBUG_QURT_ELITE_TIMER
    MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "Qurt timer setup done");
#endif //DEBUG_QURT_ELITE_TIMER

    return ADSP_EOK;
}

/*
Creates a synchronous sleep timer. Control returns to the callee after the
timer expires.

Remark: This was ported from using Qurt Timer to ATS Timers. ATS timer_sleep
has return type Void, so this function always returns 0
*/
int qurt_elite_timer_sleep(int64_t llMicrosec)
{
#ifdef DEBUG_QURT_ELITE_TIMER
   MSG(MSG_SSID_QDSP6, DBG_LOW_PRIO, "wait until sleep timer expires");
#endif //DEBUG_QURT_ELITE_TIMER

   timer_sleep((time_timetick_type) llMicrosec , T_USEC, FALSE);
   return 0;
}


int qurt_elite_timer_delete(qurt_elite_timer_t *pTimer)
{

  if (NULL == pTimer || 0 == pTimer->qTimerObj)
  {
     return ADSP_EBADPARAM;
  }

  int   nStatus = 0;

  nStatus=timer_undef((timer_ptr_type)(pTimer->qTimerObj));

  if(TE_SUCCESS != nStatus && TE_TIMER_ALREADY_IN_SAME_STATE != nStatus)
  {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed deleting timer with status=%d!", nStatus);
      return ADSP_EFAILED;
  }
  else if (nStatus == TE_TIMER_ALREADY_IN_SAME_STATE)
  {
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
              "Could not stop timer with status=%d! Timer was already stopped or expired", nStatus);
  }

  if (pTimer->qTimerObj != 0)
  {
      qurt_elite_memory_free((void *)pTimer->qTimerObj);
  }

  memset(pTimer,0,sizeof(qurt_elite_timer_t));
  return ADSP_EOK;
}

unsigned long long qurt_elite_timer_get_duration(qurt_elite_timer_t *pTimer)
{
    return pTimer->duration;
}

/*
qurt_elite_timer_get_time:
Gets the QURT system clock time
* Converts ticks into microseconds
* 1 tick = 1/19.2MHz seconds
* Micro Seconds = Ticks * 10ULL/192ULL. Compiler uses magic multiply functions to resolve this 
* repeasted fractional binary.  
* performance is 10 cycles

@returns the number of ticks elapsed on QURT system clock in terms of microseconds
*/
uint64_t qurt_elite_timer_get_time(void)
{
  uint64_t ullCurrTime =  (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  return ullCurrTime;

}


/*
qurt_elite_timer_get_time_in_msec:
* Converts ticks into milliseconds
* 1 tick = 1/19.2MHz seconds
* MilliSeconds = Ticks * 10ULL/192000ULL. Compiler uses magic multiply functions to resolve this 
* repeated fractional binary.  
* performance is 10 cycles

@returns the number of ticks elapsed on QURT system clock in terms of milliseconds
*/
uint64_t qurt_elite_timer_get_time_in_msec(void)
{
  uint64_t time =  (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192000ull);
  return time;

}


/*
Start/Restart Absolute One shot timer
@param[in] timer object
@param[in] time: absolute time of the timer in usec
@return indication of success (0) or failure (non-0)
*/
int qurt_elite_timer_oneshot_start_absolute(qurt_elite_timer_t *pTimer, int64_t time)
{

   QURT_ELITE_ASSERT(QURT_ELITE_TIMER_ONESHOT_ABSOLUTE == pTimer->uTimerType);
   int nStatus;
   time_timetick_type exp_timetick = qurt_sysclock_timetick_from_us((unsigned long long) time);
   time_timetick_type current_timetick = qurt_sysclock_get_hw_ticks();

   if (exp_timetick <= current_timetick)
   {
       MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO,
               "Failed to start Absolute timer, expiry timetick %d is before current timetick %d",
               exp_timetick, current_timetick);
       return ADSP_EBADPARAM;
   }

   if ( TE_SUCCESS != (nStatus = timer_set_absolute((timer_ptr_type)(pTimer->qTimerObj),
           exp_timetick)))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to start Absolute timer with status=%d!", nStatus);
       return ADSP_EFAILED;
   }

   pTimer->duration = QURT_TIMER_TIMETICK_TO_US(timer_expires_at_64((timer_ptr_type)(pTimer->qTimerObj))
              - timer_get_start_64((timer_ptr_type)(pTimer->qTimerObj)));
   return ADSP_EOK;
}



/*
Start/Restart Duration based One shot timer
@param[in] timer object
@param[in] duration : duration of the timer in usec
@return indication of success (0) or failure (non-0)
*/
int qurt_elite_timer_oneshot_start_duration(qurt_elite_timer_t *pTimer, int64_t duration)
{
   int nStatus;

   time_timetick_type timer_duration;

   QURT_ELITE_ASSERT(QURT_ELITE_TIMER_ONESHOT_DURATION == pTimer->uTimerType);

   if (ATS_TIMER_MAX_DURATION < (unsigned long long)duration)
   {
       MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Timer is too long!");
       return ADSP_EBADPARAM;
   }

   timer_duration = (time_timetick_type)duration;

   if (TE_SUCCESS != (nStatus = timer_set_64((timer_ptr_type)(pTimer->qTimerObj)
           , timer_duration, 0, T_USEC)))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed restarting Duration timer with status=%d!", nStatus);
       return ADSP_EFAILED;
   }

   pTimer->duration = (uint64_t) timer_duration;
   return ADSP_EOK;
}

/*
Cancel Duration based One shot timer
@param[in] timer object
@return indication of success (0) or failure (non-0)
*/
int qurt_elite_timer_oneshot_cancel(qurt_elite_timer_t *pTimer)
{
   int nStatus;
   time_timetick_type rem_time;

   QURT_ELITE_ASSERT( (QURT_ELITE_TIMER_ONESHOT_DURATION==pTimer->uTimerType)
           ||(QURT_ELITE_TIMER_ONESHOT_ABSOLUTE==pTimer->uTimerType) );

   nStatus = timer_stop((timer_ptr_type)(pTimer->qTimerObj), T_USEC, &rem_time);

   if (nStatus != TE_SUCCESS && nStatus != TE_TIMER_ALREADY_IN_SAME_STATE)
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed to stop oneshot timer with status=%d!", nStatus);
       return ADSP_EFAILED;
   }
   else if (nStatus == TE_TIMER_ALREADY_IN_SAME_STATE)
   {
       MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,
               "Could not cancel oneshot timer with status=%d! Timer was already stopped or expired", nStatus);
   }
   return ADSP_EOK;
}

/*
Start Periodic timer
@param[in] timer object
@param[in] duration : duration of the timer in usec
@return indication of success (0) or failure (non-0)
*/
int qurt_elite_timer_periodic_start(qurt_elite_timer_t *pTimer, int64_t duration)
{
   int nStatus;

   QURT_ELITE_ASSERT(QURT_ELITE_TIMER_PERIODIC == pTimer->uTimerType);

   if ( TE_SUCCESS != (nStatus = timer_set_64((timer_ptr_type)(pTimer->qTimerObj),
           (time_timetick_type)duration, (time_timetick_type)duration, T_USEC)))
   {
       MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "Failed Starting Periodic timer with status=%d!", nStatus);
       return ADSP_EFAILED;
   }
   pTimer->duration = (uint64_t)duration;

   return ADSP_EOK;
}
