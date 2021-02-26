/*==============================================================================
  FILE:         sleep_npa_scheduler.c
  
  OVERVIEW:     This file implements the sleep interface to the NPA 
                scheduler subsystem. 

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/npa_scheduler/sleep_npa_scheduler.c#2 $
$DateTime: 2017/09/13 08:52:52 $
==============================================================================*/
#include "comdef.h"
#include "sleep_npa_scheduler.h"
#include "sleep_log.h"
#include "sleepi.h"
#include "sleep_osi.h"
#include "CoreTime.h"
#include "CoreVerify.h"
#include "sleepActive.h"
#include "sleepActivei.h"
#include "sleep_solveri.h"
#include "CoreMutex.h"

/*==============================================================================
                              INTERNAL DEFINITIONS
 =============================================================================*/
/* This variable reflects the NPA scheduler deadline.  This is 
 * expressed in absolute ticks.  It is only written by the 
 * function sleepNPAScheduler_setDeadline, which is only 
 * called from a locked context.  Sleep uses this to determine 
 * the hard wakeup time. */
static uint64 g_NPADeadlineAbsTick;

/* Indicates whether the time reflected in 
 * g_NPADeadlineAbsTick is a valid value.  If not valid, 
 * then sleep should not use it. */
static boolean g_NPAdeadlineValid = FALSE;

/* Pointer to the lock  */
static CoreMutexType *g_sleepNPALock = NULL;

/*==============================================================================
                               MACRO DEFINITIONS
 =============================================================================*/
#define SLEEP_NPA_LOCK()    CORE_VERIFY_PTR(g_sleepNPALock); Core_MutexLock(g_sleepNPALock)
#define SLEEP_NPA_UNLOCK()  Core_MutexUnlock(g_sleepNPALock)

/*==============================================================================
                         EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * sleepNPAScheduler_init
 */
void sleepNPAScheduler_init(void)
{
  CORE_VERIFY_PTR(g_sleepNPALock = Core_MutexCreate(CORE_MUTEXATTR_DEFAULT));
  return;
}

/* 
 * sleepNPAScheduler_setDeadline
 */
void sleepNPAScheduler_setDeadline( uint64 deadlineAbsTick )
{
  /* Lock resource */
  SLEEP_NPA_LOCK();

  /* Record the new deadline, for sleep to use the next time through. */
  g_NPADeadlineAbsTick = deadlineAbsTick;

  /* Sleep will now start using this deadline. */
  g_NPAdeadlineValid = TRUE;

  /* Unlock resource */
  SLEEP_NPA_UNLOCK();

  /* Let ATS know about change in NPA scheduler deadline. */
  sleepActive_SetSignal( SLEEP_SIGNAL_HARD_DURATION ); 

  return;
}

/*
 * sleepNPAScheduler_getDeadline
 */
uint64 sleepNPAScheduler_getDeadline( void )
{
  uint64 rtnValue;

  SLEEP_NPA_LOCK();
  rtnValue = (TRUE == g_NPAdeadlineValid) ? g_NPADeadlineAbsTick : UINT64_MAX;
  SLEEP_NPA_UNLOCK();

  return rtnValue;
}

/* 
 * sleepNPAScheduler_adjustDeadline
 */
void sleepNPAScheduler_adjustDeadline(uint64 new_npa_deadline)
{
  uint64                timer_deadline;
  uint64                old_npa_deadline  = g_NPADeadlineAbsTick;
  sleep_solver_input   *solverInput       = sleepActive_GetSolverInput();
  int64                 offset            = 0;

  timer_deadline = solverInput->hard_deadlines.overall_timer_min;

  SLEEP_NPA_LOCK();

  /* The deadline should always be valid before adjusting it during sleep. */
  CORE_VERIFY( TRUE == g_NPAdeadlineValid );

  /* If the NPA deadline was the limiting factor in deciding the hard deadline,
   * then force a recalculation of the hard deadline, with the new value. */
  if ( old_npa_deadline < timer_deadline )
  {
    /* If the NPA deadline is still the next deadline, then calculate the
     * offset between the new NPA deadline and the old one. */
    if ( new_npa_deadline <= timer_deadline )
    {
      offset = new_npa_deadline - old_npa_deadline;
    }
    else
    {
      /* The timer deadline is next, so calculate the offset between that and
       * the old NPA deadline. */
      offset = timer_deadline - old_npa_deadline;
    }

    /* The offset should never be negative.  If it is, print a warning and skip
     * the adjustments. */
    if ( offset < 0 )
    {
      sleepLog_printf( SLEEP_LOG_LEVEL_WARNING, 1*2, 
                       "Negative offset for NPA scheduler adjustment, "
                       "offset not applied (offset: 0x%llx)", 
                       ULOG64_DATA(offset) );
    }
    else
    {
      /* Offset the sleep duration.  The new value will be passed into the
       * next enter function. */
      sleepOS_offsetSleepDuration( offset );

    }

    sleepLog_printf( SLEEP_LOG_LEVEL_INFO, 4*2, 
                     " Adjusted NPA scheduler deadline "
                     "(old NPA deadline: 0x%llx) (new NPA deadline: 0x%llx) "
                     "(timer deadline: 0x%llx) (offset: 0x%llx) ", 
                     ULOG64_DATA(old_npa_deadline), 
                     ULOG64_DATA(new_npa_deadline), 
                     ULOG64_DATA(timer_deadline), 
                     ULOG64_DATA(offset) ); 
  }

  SLEEP_NPA_UNLOCK();

  return;
}

/* 
 * sleepNPAScheduler_cancelDeadline
 */
void sleepNPAScheduler_cancelDeadline()
{
  SLEEP_NPA_LOCK();
  g_NPAdeadlineValid = FALSE;
  SLEEP_NPA_UNLOCK();

  /* Let ATS know about change in NPA scheduler deadline. */
  sleepActive_SetSignal( SLEEP_SIGNAL_HARD_DURATION );

  return;
}

