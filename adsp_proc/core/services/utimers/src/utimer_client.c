/*=============================================================================

                UTimer_Client.c

GENERAL DESCRIPTION
      UTimer Client Process Code

EXTERNAL FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

      Copyright (c) 2009 - 2017
      by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


$Header: //components/rel/core.qdsp6/2.1/services/utimers/src/utimer_client.c#1 $ 
$DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
11/18/13   ab      Add file that provides MultiPD changes
=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
//#include <assert.h>
#include "err.h"
#include "utimer.h"
#include "utimer_qdi_v.h"
#include "utimer_client_v.h"
#include "uTimetick.h"
//Enable this header once qurt supports signal2 in uimage
//#include "qurt_signal2.h"

/*****************************************************************************/
/*                          DATA DECLARATIONS                                */
/*****************************************************************************/

/* Current Process idx */
int utimer_client_pid;

/* Qdi Timer Client Handle to communicate with Guest os layer */
int utimer_client_qdi_handle = -1;

#ifdef FEATURE_UTIMER_CLIENT_TEST
uint8 utimer_client_test_stack_arr[UTIMER_CLIENT_TEST_STACK_SIZE];
qurt_thread_t      utimer_test_id;
#endif /* FEATURE_UTIMER_CLIENT_TEST */


#ifdef FEATURE_TIMER_CLIENT_WORKER_THREAD
uint8 utimer_client_stack_arr[UTIMER_CLIENT_STACK_SIZE];
qurt_thread_t      utimer_worker_thread_id;

#ifdef FEATURE_UTIMER_CLIENT_DEBUG

/*These values are set high to avoid the annoying F3 messages
 and an error fatal in case cb processing time is high. 
 1 sec = 19200000 ticks. 
*/
volatile static uint64 utimer_client_processing_time_warning_threshold = 19200;
#ifndef TIMER_CLIENT_ERROR_FATAL_THRESHOLD_ZEROED_OUT
volatile static uint64 utimer_client_processing_time_err_fatal_threshold=38400000;
#else
volatile static uint64 utimer_client_processing_time_err_fatal_threshold=0;
#endif


typedef struct utimer_client_debug_var_1
{
  utimer_ptr_type         timer;
  uint32                  cb_type;
  uint32                  sig_func_addr;
  uint32                  mask_data;
  utimer_timetick_type    processing_started;
  utimer_timetick_type    processing_time;
}utimer_client_expired_type;
#define MAX_UTIMER_CLIENT_EXPIRED 3
static utimer_client_expired_type utimer_client_expired[MAX_UTIMER_CLIENT_EXPIRED];
static int utimer_client_expired_cnt = 0;

#endif /* FEATURE_UTIMER_CLIENT_DEBUG */
#endif /* FEATURE_TIMER_CLIENT_WORKER_THREAD */

/*==============================================================================

                            A P I   D E F I N I T I O N S

=============================================================================*/



/*==============================================================================
                        DEFER-UNDEFER DEFINITIONS
=============================================================================*/

/**
Gets time to the next occurring non-deferrable timer expiry value.

@return
Number of ticks in 19MHz domain until the next timer expiry in a
 timer group which is non-deferrable.

@dependencies  
Must be called with interrupts locked. 
*/
utimer_timetick_type utimer_get_first_non_deferrable_timer_expiry
( 
   utimer_non_defer_data_ptr utimer_data_ptr
) 
{
   utimer_timetick_type first_non_def_tick;
   utimer_invoke_get_first_non_deferrable_timer_expiry(utimer_client_qdi_handle, &first_non_def_tick, utimer_data_ptr);
   return first_non_def_tick;
}

/**  
Sets timer match interrupt to the next occurring non-deferrable timer expiry
value. Complementary function is 
timer_undefer_match_interrupt_64(). 

@return
time_timetick_type -- Number of ticks in 19MHz domain 
until the next timer expiry in a timer group that is 
non-deferrable. 

@dependencies  
This function must be called with interrupts locked.
 

@sideeffects 
Adjusts the match value and prevents any subsequent timer calls from
updating the match value until timer_undefer_match_interrupt_64() is called.
*/
utimer_timetick_type utimer_defer_match_interrupt_64( void )
{
#ifdef FEATURE_UTIMER_DEFER_SUPPORT
   utimer_timetick_type ticks;
   utimer_invoke_defer_match_interrupt_64(utimer_client_qdi_handle, &ticks);
   return ticks;
#else
   return utimer_get_first_non_deferrable_timer_expiry();
#endif /* #ifdef FEATURE_UTIMER_DEFER_SUPPORT */
}

/** 
Sets the timer ISR to fire at the earliest expiring timer in the active list.
This function is meant to serve as the complementary function to 
timer_defer_match_interrupt().

@dependencies  
This function must be called with interrupts locked.

@sideeffects 
This function adjusts the match value.
*/
void utimer_undefer_match_interrupt_64( void )
{
#ifdef FEATURE_UTIMER_DEFER_SUPPORT
   utimer_invoke_undefer_match_interrupt_64(utimer_client_qdi_handle);
#endif /* #ifdef FEATURE_UTIMER_DEFER_SUPPORT */
}

/*==============================================================================

                     TIMER INTERNAL FUNCTION DEFINITIONS

=============================================================================*/
/**
Defines and initializes a timer.
                                  
Please refer timer.h
*/
utimer_error_type utimer_def_osal
(
  utimer_ptr_type                  timer,
  utimer_notify_type               cb_type,
  utimer_osal_notify_obj_ptr       sigs_func_addr,
  utimer_osal_notify_data          sigs_mask_data
)
{
   if(utimer_client_qdi_handle == -1)
   {
      //MSG(MSG_SSID_ATS, MSG_LEGACY_ERROR,"UTimer system not initialized");
      return UTE_TIMER_MODULE_NOT_INITIALIZED;
   }
   
   if(timer == NULL || cb_type >= UTIMER_INVALID_NOTIFY_TYPE || (sigs_func_addr == NULL && sigs_mask_data != 0))
      return UTE_INVALID_PARAMETERS;

   //group being NULL is allowed for timer_def, timer_def2
   return utimer_invoke_def_osal(utimer_client_qdi_handle, timer, cb_type, sigs_func_addr, sigs_mask_data);
}

#ifdef FEATURE_UTIMER_DEFER_SUPPORT

/**
Sets the timer deferrable status. 

@note1hang Timer groups default to being non-deferrable.

@param[in] pgroup    Timer to be (un)deferred.
@param[in] condition Boolean value: \n
                     TRUE -- Deferrable. \n
                     FALSE -- Otherwise.
@return
None.
 
@dependencies  
None.
*/
utimer_error_type utimer_set_deferrable
( 
  utimer_ptr_type ptimer, 
  boolean condition 
)
{
   if(!ptimer)
      return UTE_INVALID_PARAMETERS;
      
   return utimer_invoke_set_deferrable(utimer_client_qdi_handle, ptimer, condition);
}
#endif /* #ifdef FEATURE_UTIMER_DEFER_SUPPORT */

/**
Sets an inactive timer to expire after at given absolute time, or changes an
active timer to expire at a given time. 

Please refer utimer.h for more details
*/
utimer_error_type utimer_set_absolute
(
  utimer_ptr_type                  timer,
  utimer_timetick_type             time
)
{
   if(timer == NULL)
      return UTE_INVALID_PARAMETERS;
     
   return utimer_invoke_set_absolute(utimer_client_qdi_handle, timer, time, 0, UT_TICK);
}

/**
Sets an inactive timer to expire after a given period of time, or changes an
active timer to expire after a given period of time. Optionally, specifies
the timer to repeatly expire with a given period.  If the timer 
is to be set in units of ticks, it will be done in the 19MHz 
domain. 

Please refer utimer.h for more details
*/
utimer_error_type utimer_set_64
(
  utimer_ptr_type                  timer,
  utimer_timetick_type             time,
  utimer_timetick_type             reload,
  utimer_unit_type                  unit
)
{
   utimer_timetick_type     time_ticks, reload_ticks;
   
   if(timer == NULL)
      return UTE_INVALID_PARAMETERS;
      
   time_ticks   = (utimer_timetick_type) uTimetick_CvtToTicks((uTimetick_type)time, (uTimetick_unitType)unit);
   reload_ticks = (utimer_timetick_type) uTimetick_CvtToTicks((uTimetick_type)reload, (uTimetick_unitType)unit);

   time_ticks = uTimetick_Get() + time_ticks;
   	
   return utimer_invoke_set_absolute(utimer_client_qdi_handle, timer, time_ticks, reload_ticks, unit);
}

/**
Gets the number of units before the timer expires. If the unit 
is ticks, it will return value in 19MHz domain. 

Please refer timer.h for more details
*/
utimer_timetick_type utimer_get_64
(
  utimer_ptr_type                     timer,
  utimer_unit_type                     unit
)
{
   utimer_timetick_type rem_ticks;
   
   if(timer == NULL)
      return 0;
      
   utimer_invoke_get_64(utimer_client_qdi_handle, timer, unit, &rem_ticks);
   return rem_ticks;
}

/**
Stops an active timer.  If units are in ticks, then it will 
return the value in the 19MHz domain. 

Please refer timer.h for more details
*/
utimer_timetick_type utimer_clr_64
(
  utimer_ptr_type                 timer,
  utimer_unit_type                unit
)
{
   utimer_timetick_type ticks = 0;

   (void) utimer_stop(timer, unit, &ticks);
   
   return ticks;
}

/**
This api is same as timer_clr, except that it has error type as return value.
Stops an active timer.  If units are in ticks, then it will 
return the value in the 19MHz domain. 

Please refer timer.h for more details
*/
utimer_error_type utimer_stop
(
  utimer_ptr_type                  timer,
  utimer_unit_type                 unit,
  utimer_timetick_type            *rem_time
)
{
   utimer_timetick_type ticks = 0;
   utimer_error_type    status;
   
   if(timer == NULL)
      return UTE_INVALID_PARAMETERS;
      
   status = utimer_invoke_clr_64(utimer_client_qdi_handle, timer, unit, &ticks);
   
   if(rem_time)
      *rem_time = ticks;
   return status;
}

/**
Removes timer from active list and deallocates the timer memory

Please refer timer.h for more details
*/

utimer_error_type utimer_undef
(
  /* Timer to stop */
  utimer_ptr_type                  timer
)
{
   if(timer == NULL)
      return UTE_INVALID_PARAMETERS;

   return utimer_invoke_undef(utimer_client_qdi_handle, timer);
}

/*==============================================================================

                     CALLBACK IMPLEMENTATION

=============================================================================*/

/*===========================================================================
UTimerCallbackHandler
    Gets callback details and 
    either calls callback functions 
    or sets masks to signals
===========================================================================*/
#ifdef FEATURE_TIMER_CLIENT_WORKER_THREAD
void UTimerCallbackHandler(void *parameter)
{
   int result;
   utimer_cb_info cb_details;
   utimer_t1_cb_type cb_func1; 
   utimer_t2_cb_type cb_func2; 
#ifdef FEATURE_UTIMER_CLIENT_DEBUG
   unsigned long long cb_processing_started, cb_processing_time;
#endif /* FEATURE_UTIMER_CLIENT_DEBUG */
   
   while(1)
   {
      result = utimer_invoke_get_cb_info(utimer_client_qdi_handle, &cb_details);

      /* If there was a cancel signal or copy fail, try again */
      if( result == UTIMER_ERR_CANCEL_WAIT ||
          result == UTIMER_ERR_QDI_CPY_FAIL )
      {
         continue;
      }

      //printf("UTimerCallbackHandler: 0x%x in handle: %d \n", (unsigned int)cb_details.sig_func_addr, timer_client_qdi_handle);

#ifdef FEATURE_UTIMER_CLIENT_DEBUG
      cb_processing_started = uTimetick_Get();
#endif /* FEATURE_UTIMER_CLIENT_DEBUG */

      if(cb_details.cb_type == UTIMER_NATIVE_OS_SIGNAL_TYPE)
      {
        qurt_anysignal_set((qurt_anysignal_t *)cb_details.sig_func_addr, cb_details.mask_data1);
      }
      //else if(cb_details.cb_type == UTIMER_NATIVE_OS_SIGNAL2_TYPE)
      //{
      //  qurt_signal2_set((qurt_signal2_t *)cb_details.sig_func_addr, cb_details.mask_data1);
      //}
      else if(cb_details.cb_type == UTIMER_REX_TCB_SIGNAL_TYPE)
      {
#ifdef FEATURE_TIMER_REX_IS_AVAILABLE
    rex_set_sigs((rex_tcb_type *)cb_details.sig_func_addr, cb_details_mask_data1);
#else
    ERR_FATAL("On adsp, signal cannot be of rex type. Cb type: %d, sigs_obj: 0x%x, sigs: 0x%x", cb_details.cb_type, cb_details.sig_func_addr, cb_details.mask_data1);
#endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
      }
      else if(cb_details.cb_type == UTIMER_FUNC1_CB_TYPE)
      {
        cb_func1 = (utimer_t1_cb_type) cb_details.sig_func_addr;
        cb_func1(cb_details.mask_data1);
      }
      else if(cb_details.cb_type == UTIMER_FUNC2_CB_TYPE)
      {
         cb_func2 = (utimer_t2_cb_type) cb_details.sig_func_addr;
         cb_func2(cb_details.data2, cb_details.mask_data1);
      }

#ifdef FEATURE_UTIMER_CLIENT_DEBUG
      cb_processing_time = uTimetick_Get() - cb_processing_started;

      utimer_client_expired[utimer_client_expired_cnt].timer              = cb_details.timer;
      utimer_client_expired[utimer_client_expired_cnt].cb_type            = cb_details.cb_type;
      utimer_client_expired[utimer_client_expired_cnt].sig_func_addr      = cb_details.sig_func_addr;
      utimer_client_expired[utimer_client_expired_cnt].mask_data          = cb_details.mask_data1;
      utimer_client_expired[utimer_client_expired_cnt].processing_started = cb_processing_started;
      utimer_client_expired[utimer_client_expired_cnt].processing_time    = cb_processing_time;
      utimer_client_expired_cnt = (utimer_client_expired_cnt + 1) % MAX_UTIMER_CLIENT_EXPIRED;
      
      if(cb_processing_time > utimer_client_processing_time_warning_threshold)
      {
         /* MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"callback fn = 0x%x took %d ticks, process = %d",
               cb_details.sig_func_addr,
               cb_processing_time,
               utimer_client_pid);  */
      }
  

      if(utimer_client_processing_time_err_fatal_threshold != 0 && 
         cb_processing_time > utimer_client_processing_time_err_fatal_threshold)
      {
         ERR_FATAL("callback fn = 0x%x took %d ticks, process = %d",
               cb_details.sig_func_addr,
               cb_processing_time,
               utimer_client_pid);          
      }
#endif /* FEATURE_UTIMER_CLIENT_DEBUG */
   }
} /* UTimerCallbackHandler */
#endif /* FEATURE_TIMER_CLIENT_WORKER_THREAD */