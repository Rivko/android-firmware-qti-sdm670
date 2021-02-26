/*=============================================================================

                Timer_Client.c

GENERAL DESCRIPTION
      Timer Client Process Code

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


$Header: //components/rel/core.qdsp6/2.1/services/time/src/timer_client.c#2 $ 
$DateTime: 2017/07/31 13:45:36 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/03/14   psu     Update diag macros using optimized versions.
06/20/13   ab      Add file that provides MultiPD changes
=============================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "timer.h"
#include "timer_qdi_v.h"
#include "qurt_signal2.h"

#include "msg.h"
#include "err.h"
/*****************************************************************************/
/*                          DATA DECLARATIONS                                */
/*****************************************************************************/

/* Current Process idx */
int timer_client_pid;

/* Qdi Timer Client Handle to communicate with Guest os layer */
int timer_client_qdi_handle = -1;

/* Timers declared to be in the NULL group are reparented to this group */
timer_group_type                  timer_null_group;

/* Global Non-deferrable group for various clients use */
/* Note: This group shouldn't be called with disable/enable group functions as
   this will be used by other clients too. */
timer_group_type                  timer_non_defer_group;

//#define FEATURE_TIMER_CLIENT_DEBUG
#ifdef FEATURE_TIMER_CLIENT_DEBUG

/*These values are set high to avoid the annoying F3 messages
 and an error fatal in case cb processing time is high. 
 1 sec = 19200000 ticks. 
*/
volatile static uint64 timer_client_processing_time_warning_threshold = 19200;
#ifndef TIMER_CLIENT_ERROR_FATAL_THRESHOLD_ZEROED_OUT
volatile static uint64 timer_client_processing_time_err_fatal_threshold=38400000;
#else
volatile static uint64 timer_client_processing_time_err_fatal_threshold=0;
#endif


typedef struct timer_client_debug_var_1
{
  timer_ptr_type         timer;
  uint32                 cb_type;
  uint32                 sig_func_addr;
  uint32                 mask_data;
  time_timetick_type     processing_started;
  time_timetick_type     processing_time;
}timer_client_expired_type;
#define MAX_TIMER_CLIENT_EXPIRED 3
static timer_client_expired_type timer_client_expired[MAX_TIMER_CLIENT_EXPIRED];
static int timer_client_expired_cnt = 0;

#endif /* FEATURE_TIMER_CLIENT_DEBUG */


/*==============================================================================

                            A P I   D E F I N I T I O N S

=============================================================================*/

/*==============================================================================

                        TIMER GROUP DEFINITIONS

=============================================================================*/
/**
Enables a timer group. Timers in the timer group that expired during the
diabled period have their expiry processing executed.

@param[in] timer_group Timer group to be enabled.

@return
None.

@dependencies  
None.

@sideeffects 
This function may generate signals to a task, which can cause task switches. \n
This function may queue an asynchronous procedure call.
*/
timer_error_type timer_group_enable
(
  timer_group_ptr                 timer_group
)
{
   if(!timer_group)
      return TE_INVALID_PARAMETERS;
	  
   return timer_invoke_group_enable(timer_client_qdi_handle, timer_group);
}

/**
Disables a timer group. Timers in the timer group continue to count
down, but they do not expire.

@param[in] timer_group Timer group to be disabled.

@return
None.
 
@dependencies
None.
*/
timer_error_type timer_group_disable
(
  timer_group_ptr                 timer_group
)
{
   if(!timer_group)
      return TE_INVALID_PARAMETERS;
	  
   return timer_invoke_group_disable(timer_client_qdi_handle, timer_group);
}

/*==============================================================================

                        DEFER-UNDEFER DEFINITIONS

=============================================================================*/
/**
Sets the timer group deferrable status. 

@note1hang Timer groups default to being non-deferrable.

@param[in] pgroup    Timer group to be undeferred.
@param[in] condition Boolean value: \n
                     TRUE -- Deferrable. \n
                     FALSE -- Otherwise.
@return
None.
 
@dependencies  
None.
*/
timer_error_type timer_group_set_deferrable
( 
  timer_group_ptr pgroup, 
  boolean condition 
)
{
   if(!pgroup)
      return TE_INVALID_PARAMETERS;
      
   if(pgroup == &timer_non_defer_group && condition == TRUE)
      return TE_INVALID_PARAMETERS;

   if( pgroup == &timer_null_group && condition == FALSE )
	 return TE_INVALID_PARAMETERS;

   return timer_invoke_group_set_deferrable(timer_client_qdi_handle, pgroup, condition);
}

/**
Gets the first non-deferrable timer's expiry value.

@return
Absolute expiry of first occuring non-deferrable timer.
Returns TIMER_MAX_EXPIRY in case there is no non-deferrable timer present.

@dependencies  
Must be called with interrupts locked. 
*/
time_timetick_type timer_get_first_non_deferrable_timer_expiry
(
  timer_non_defer_data_ptr timer_data_ptr 
)
{
   time_timetick_type first_non_def_tick = 0;
   timer_invoke_get_first_non_deferrable_timer_expiry(timer_client_qdi_handle,&first_non_def_tick, timer_data_ptr);
   return first_non_def_tick;
}

/**
Gets time to the next occurring non-deferrable timer expiry value.

@return
Number of ticks in 19MHz domain until the next timer expiry in a
 timer group which is non-deferrable.

@dependencies  
Must be called with interrupts locked. 
*/
time_timetick_type timer_get_time_till_first_non_deferrable_timer_64( void ) 
{
   time_timetick_type tt_ret = 0;
   time_timetick_type first_non_def_expiry = 0;
   time_timetick_type ticks_now = 0;
   first_non_def_expiry = timer_get_first_non_deferrable_timer_expiry(NULL);
   ticks_now = qurt_sysclock_get_hw_ticks();
   /* If future_expiry is in past */
   if (first_non_def_expiry < ticks_now )
   {  
     tt_ret = 0;
   }
   else
   {  
     /* Return the difference between now and the above future expiry */
     tt_ret = (first_non_def_expiry - ticks_now);
   }   
   return tt_ret;
}

/**  
Sets timer match interrupt to the next occurring non-deferrable timer expiry
value. Complementary function is 
timer_undefer_match_interrupt(). 

@return
time_timetick_type -- Number of ticks in 19MHz domain 
until the next timer expiry in a timer group that is 
non-deferrable. 

@dependencies  
This function must be called with interrupts locked.
 

@sideeffects 
Adjusts the match value and prevents any subsequent timer calls from
updating the match value until timer_undefer_match_interrupt() is called.
*/
time_timetick_type timer_defer_match_interrupt_64( void )
{
   time_timetick_type ticks = 0;
   timer_invoke_defer_match_interrupt_64(timer_client_qdi_handle, &ticks);
   return ticks;
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
void timer_undefer_match_interrupt( void )
{
   timer_invoke_undefer_match_interrupt(timer_client_qdi_handle);
}

/*==============================================================================

                     TIMER INTERNAL FUNCTION DEFINITIONS

=============================================================================*/
/**
Defines and initializes a timer.
                                  
Please refer timer.h
*/
timer_error_type timer_def_osal
(
  timer_ptr_type                  timer,
  timer_group_ptr                 group,
  timer_notify_type               cb_type,
  time_osal_notify_obj_ptr        sigs_func_addr,
  time_osal_notify_data           sigs_mask_data
)
{
   if(timer_client_qdi_handle == -1)
   {
      MSG(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Timer system not initialized");
      return TE_TIMER_MODULE_NOT_INITIALIZED;
   }
   
   if(timer == NULL)
      return TE_INVALID_PARAMETERS;
      
   if(cb_type >= TIMER_INVALID_NOTIFY_TYPE)
      return TE_INVALID_PARAMETERS;

   /* Reparent timers in the NULL group to be in the timer_null_group */
   if ( group == NULL )
   {
	 group = &timer_null_group;
   }
	  
   //group being NULL is allowed for timer_def, timer_def2
   return timer_invoke_def_osal(timer_client_qdi_handle, timer, group, cb_type, sigs_func_addr, sigs_mask_data);
}

/**
Defines a clock callback-style timer. When the timer expires, the callback 
function with the arguments time_ms and data is called from the interrupt context. 
Timers may also be staticly declared via the TIMER_DEF2() macro.

Please refer timer.h for more details
*/
timer_error_type timer_def2
(
  timer_ptr_type                  timer,
  timer_group_ptr                 group
)
{
   if(timer_client_qdi_handle == -1)
   {
      MSG(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Timer system not initialized");
      return TE_TIMER_MODULE_NOT_INITIALIZED;
   }
   
   if(timer == NULL)
      return TE_INVALID_PARAMETERS;

   /* Reparent timers in the NULL group to be in the timer_null_group */
   if ( group == NULL )
   {
	 group = &timer_null_group;
   }
   
   //group being NULL is allowed for timer_def, timer_def2
   return timer_invoke_def2(timer_client_qdi_handle, timer, group);
}

/**
Initializes a clock callback style timer.

Please refer timer.h for more details
*/
timer_error_type timer_reg
(
  timer_ptr_type                  timer,
  timer_t2_cb_type                func,
  timer_cb_data_type              data,
  timetick_type                   time_ms,
  timetick_type                   reload_ms
)
{
   if(timer == NULL)
      return TE_INVALID_PARAMETERS;
      
   return timer_invoke_reg(timer_client_qdi_handle, timer, func, data, time_ms, reload_ms);
}

/**
Sets an inactive timer to expire after a given period of time, or changes an
active timer to expire after a given period of time. Optionally, specifies
the timer to repeatly expire with a given period.  If the timer 
is to be set in units of ticks, it will be done in the 19MHz 
domain. 

Please refer timer.h for more details
*/
timer_error_type timer_set_64
(
  timer_ptr_type                  timer,
  time_timetick_type              time,
  time_timetick_type              reload,
  timer_unit_type                 unit
)
{
   if(timer == NULL)
      return TE_INVALID_PARAMETERS;
      
   return timer_invoke_set_64(timer_client_qdi_handle, timer, time, reload, unit);
}

/**
* 
*Sets timer to expire at absolute ticks (instead of relative expiry) 

Please refer timer.h for more details
*/
timer_error_type timer_set_absolute
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Time (in XOs) until first timer expiry */
  time_timetick_type              time

)
{
   if(timer == NULL)
      return TE_INVALID_PARAMETERS;
      
   return timer_invoke_set_absolute(timer_client_qdi_handle, timer, time);
}

/**
Gets the number of units before the timer expires. If the unit 
is ticks, it will return value in 19MHz domain. 

Please refer timer.h for more details
*/
time_timetick_type timer_get_64
(
  timer_ptr_type                  timer,
  timer_unit_type                     unit
)
{
   time_timetick_type rem_ticks = 0;
   
   if(timer == NULL)
      return 0;
      
   timer_invoke_get_64(timer_client_qdi_handle, timer, unit, &rem_ticks);
   return rem_ticks;
}

/**
Queries whether a timer is active. A timer that has been set to expire at a 
certain time is considered to be active even if the group it is a member of is 
disabled. A timer that is explicitly paused is not active until it has been 
resumed.

Please refer timer.h for more details
*/
boolean timer_is_active
(
  timer_ptr_type  timer
)
{
   if(timer == NULL)
      return FALSE;
      
   return timer_invoke_is_active(timer_client_qdi_handle, timer);
}

/**
Returns the XO counter value when the timer expires.

Please refer timer.h for more details
*/
time_timetick_type timer_expires_at_64
(
  timer_ptr_type  timer
)
{
   time_timetick_type ticks = 0;
   
   if(timer == NULL)
      return 0;
      
   timer_invoke_expires_at_64(timer_client_qdi_handle, timer, &ticks);
   return ticks;
}

/**
Returns the slow-clock counter value in the 19MHz domain when 
the timer started. 

Please refer timer.h for more details
*/
time_timetick_type timer_get_start_64
(
  timer_ptr_type  timer
)
{
   time_timetick_type ticks = 0;

   if(timer == NULL)
      return 0;
      
   timer_invoke_get_start_64(timer_client_qdi_handle, timer, &ticks);
   return ticks;
}

/**
Stops an active timer.  If units are in ticks, then it will 
return the value in the 19MHz domain. 

Please refer timer.h for more details
*/
time_timetick_type timer_clr_64
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit
)
{
   time_timetick_type ticks = 0;
   
   if(timer == NULL)
      return 0;
   
   (void) timer_invoke_clr_64(timer_client_qdi_handle, timer, unit, &ticks);
   
   return ticks;
}

/**
This api is same as timer_clr, except that it has error type as return value.
Stops an active timer.  If units are in ticks, then it will 
return the value in the 19MHz domain. 

Please refer timer.h for more details
*/
timer_error_type timer_stop
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit,
  time_timetick_type              *rem_time
)
{
   if(timer == NULL)
      return TE_INVALID_PARAMETERS;
      
   return timer_invoke_clr_64(timer_client_qdi_handle, timer, unit, rem_time);
}

/**
Removes timer from active list and deallocates the timer memory

Please refer timer.h for more details
*/

timer_error_type timer_undef
(
  /* Timer to stop */
  timer_ptr_type                  timer
)
{
   if(timer == NULL)
      return TE_INVALID_PARAMETERS;

   return timer_invoke_undef(timer_client_qdi_handle, timer);
}


/**
Pauses an active timer.

Please refer timer.h for more details
*/
timer_error_type timer_pause
(
  timer_ptr_type                  timer
)
{
   if(timer == NULL)
      return TE_INVALID_PARAMETERS;

   return timer_invoke_pause(timer_client_qdi_handle, timer);
}


/**
Resumes a previously paused timer. When a timer is paused 5 seconds before its
expiry and timer_resume( ) is called 30 seconds later, the timer expires
5 seconds after timer_resume( ) was called.

Please refer timer.h for more details
*/
timer_error_type timer_resume
(
  timer_ptr_type                  timer
)
{
   if(timer == NULL)
      return TE_INVALID_PARAMETERS;

   return timer_invoke_resume(timer_client_qdi_handle, timer);
}


/**
Stops active timers belonging to the given task.

Please refer timer.h for more details
*/
void timer_clr_osal_thread_timers
(
  time_osal_thread_id                    thread_id
)
{
   timer_invoke_clr_osal_thread_timers(timer_client_qdi_handle, thread_id);
}

/**
Returns the number of slow clocks in 19MHz domain until the 
expiry of the next timer to expire. 

Please refer timer.h for more details
*/
time_timetick_type timer_get_sclk_till_expiry_64( void )
{
   time_timetick_type ticks = 0;
   timer_invoke_get_sclk_till_expiry_64(timer_client_qdi_handle, &ticks);
   return ticks;
}

/**
Sets the slow-clock timer interrupt to expire in advance of the next expiring 
timer expiry point to allow for TCXO to turn on.

Please refer timer.h for more details
*/
timer_error_type timer_set_wakeup_64
(
  time_timetick_type                   wakeup_sclks
)
{
   return timer_invoke_set_wakeup_64(timer_client_qdi_handle, wakeup_sclks);
}

/**
Readjusts the timetick counter values to synchronize with the base timetick 
counter when waking up from a power collapse. The match value must be adjusted 
after a power collapse and this function performs that adjustment. This 
function is called after waking up. 

Please refer timer.h for more details
*/
void timer_restart(void)
{
   timer_invoke_restart(timer_client_qdi_handle);
}

/*=============================================================================

FUNCTION TIMER_SLEEP

DESCRIPTION
 Suspends the client task for the specified time.
 If non_deferrable option is True, task will surely get resumed at specified time
 If non_deferrable option is False, and if Proc goes into sleep at the time of
 specified timeout, task will resume only after Proc wakesup.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
void timer_sleep
(
  time_timetick_type              timeout,
  timer_unit_type                 unit,
  boolean                         non_deferrable
)
{
  /* Timer for the client to handle the sleeping */
  timer_type		      sleep_timer;
  unsigned int            dummy;
  
  /* Signal object for timer callback notification */
  qurt_anysignal_t        sleep_signal_obj;
  
  /* memset the external timer memory to zero */
  memset(&sleep_timer, 0, sizeof(timer_type));

  /* Initialize the qurt signal object */
  qurt_anysignal_init (&sleep_signal_obj);

  if (non_deferrable != TRUE)
    {
	  /* Create a deferrable timer to wake up the calling task */
	  timer_def_osal( &sleep_timer, 
	             NULL, /* Null Timer Group */
				 TIMER_NATIVE_OS_SIGNAL_TYPE, 
				 &sleep_signal_obj, 
				 0x1 
			   );
	}
  else
    {
	  /* Create a non-deferrable timer for the clinet */
	  timer_def_osal( &sleep_timer, 
	             &timer_non_defer_group, 
				 TIMER_NATIVE_OS_SIGNAL_TYPE, 
				 &sleep_signal_obj, 
				 0x1 
			   );
	}
	
  /* Set the timer to expire after the specified duration */
    (void)timer_set_64(&sleep_timer,
                 timeout,
                 0,
                 unit);				 

  
//  MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"TIMER_SLEEP %d: Entering timer_sleep, timeout=%d; unit=%d",
//             timer_client_pid,
//			(uint32)timeout,
//			(uint32)unit);
				 
  /* Wait on the Qurt signal object */
  qurt_signal_wait_cancellable (&sleep_signal_obj, 0x1,QURT_SIGNAL_ATTR_WAIT_ANY, &dummy);

  /* Delete the signal */
  qurt_anysignal_destroy (&sleep_signal_obj);
  
  /* Undefine the timer */
  timer_undef(&sleep_timer);

//  MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"TIMER_SLEEP %d: Exiting from timer_sleep, timeout=%d; unit=%d",
//            timer_client_pid,
//			(uint32)timeout,
//			(uint32)unit);
 
  return;
} /* timer_sleep */

/*=============================================================================
FUNCTION TIMER_REGISTER_FOR_NON_DEFER_EXPIRY_CHANGE

DESCRIPTION
Registers for first non-deferrable timer expiry change notification.
Only signal type notifications are allowed, for others, error will be 
returned.

DEPENDENCIES
  Input objects should be initialized by client

RETURN VALUE
TE_INVALID_PARAMETERS   If passed sig obj addr is NULL or invalid notify type
TE_SUCCESS :            If the operation completed successfully

SIDE EFFECTS
  None
=============================================================================*/
timer_error_type timer_register_for_non_defer_expiry_change
(
  timer_notify_type			notify_type,
  time_osal_notify_obj_ptr 	notify_obj,
  time_osal_notify_data 	notify_mask
)
{
   return timer_invoke_register_for_non_defer_expiry_change(timer_client_qdi_handle,
   	                                                        notify_type,
   	                                                        notify_obj,
   	                                                        notify_mask);
}

/*==============================================================================

                     CALLBACK IMPLEMENTATION

=============================================================================*/
/*===========================================================================
TimerCallbackHandler
    Gets callback details and 
    either calls callback functions 
    or sets masks to signals
===========================================================================*/
void TimerCallbackHandler(void *parameter)
{
   int result;
   timer_cb_info cb_details;
   timer_t1_cb_type cb_func1; 
   timer_t2_cb_type cb_func2; 
#ifdef FEATURE_TIMER_CLIENT_DEBUG
   unsigned long long cb_processing_started, cb_processing_time;
#endif /* FEATURE_TIMER_CLIENT_DEBUG */
   
   while(1)
   {
      result = timer_invoke_get_cb_info(timer_client_qdi_handle, &cb_details);

      /* If there was a cancel signal or copy fail, try again */
      if( result == TIMER_ERR_CANCEL_WAIT ||
          result == TIMER_ERR_QDI_CPY_FAIL )
      {
         continue;
      }

      //printf("TimerCallbackHandler: 0x%x in handle: %d \n", (unsigned int)cb_details.sig_func_addr, timer_client_qdi_handle);

#ifdef FEATURE_TIMER_CLIENT_DEBUG
      cb_processing_started = qurt_sysclock_get_hw_ticks();
#endif /* FEATURE_TIMER_CLIENT_DEBUG */

      if(cb_details.cb_type == TIMER_NATIVE_OS_SIGNAL_TYPE)
      {
        qurt_anysignal_set((qurt_anysignal_t *)cb_details.sig_func_addr, cb_details.mask_data1);
      }
      else if(cb_details.cb_type == TIMER_NATIVE_OS_SIGNAL2_TYPE)
      {
        qurt_signal2_set((qurt_signal2_t *)cb_details.sig_func_addr, cb_details.mask_data1);
      }
      else if(cb_details.cb_type == TIMER_REX_TCB_SIGNAL_TYPE)
      {
#ifdef FEATURE_TIMER_REX_IS_AVAILABLE
    rex_set_sigs((rex_tcb_type *)cb_details.sig_func_addr, cb_details_mask_data1);
#else
    ERR_FATAL("On adsp, signal cannot be of rex type. Cb type: %d, sigs_obj: 0x%x, sigs: 0x%x", cb_details.cb_type, cb_details.sig_func_addr, cb_details.mask_data1);
#endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
      }
      else if ( cb_details.cb_type == TIMER_DALSYS_EVENT_SIGNAL_TYPE)  
      {      
         timer_dalsys_sig_set(cb_details.sig_func_addr);  
      } 
      else if(cb_details.cb_type == TIMER_FUNC1_CB_TYPE)
      {
        cb_func1 = (timer_t1_cb_type) cb_details.sig_func_addr;
        cb_func1(cb_details.mask_data1);
      }
      else if(cb_details.cb_type == TIMER_FUNC2_CB_TYPE)
      {
         cb_func2 = (timer_t2_cb_type) cb_details.sig_func_addr;
         cb_func2(cb_details.data2, cb_details.mask_data1);
      }

#ifdef FEATURE_TIMER_CLIENT_DEBUG
      cb_processing_time = qurt_sysclock_get_hw_ticks() - cb_processing_started;

      timer_client_expired[timer_client_expired_cnt].timer              = cb_details.timer;
      timer_client_expired[timer_client_expired_cnt].cb_type            = cb_details.cb_type;
      timer_client_expired[timer_client_expired_cnt].sig_func_addr      = cb_details.sig_func_addr;
      timer_client_expired[timer_client_expired_cnt].mask_data          = cb_details.mask_data1;
      timer_client_expired[timer_client_expired_cnt].processing_started = cb_processing_started;
      timer_client_expired[timer_client_expired_cnt].processing_time    = cb_processing_time;
      timer_client_expired_cnt = (timer_client_expired_cnt + 1) % MAX_TIMER_CLIENT_EXPIRED;
      
      if(cb_processing_time > timer_client_processing_time_warning_threshold)
      {
         MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"callback fn = 0x%x took %d ticks, process = %d",
               cb_details.sig_func_addr,
               cb_processing_time,
               timer_client_pid);          
      }
  

      if(timer_client_processing_time_err_fatal_threshold != 0 && 
         cb_processing_time > timer_client_processing_time_err_fatal_threshold)
      {
         ERR_FATAL("callback fn = 0x%x took %d ticks, process = %d",
               cb_details.sig_func_addr,
               cb_processing_time,
               timer_client_pid);          
      }
#endif /* FEATURE_TIMER_CLIENT_DEBUG */
   }
} /* TimerCallbackHandler */


/*===========================================================================
DESCRIPTION
   Creates worker thread that waits inside Guest OS for callback details.
   Once the thread gets callback details it will either signal or call the
   callback depending on the callback type.
===========================================================================*/
#define TIMER_PRI 20   /* Todo: Talk to Qurt team, what should be priority of this callback thread.. can it be 10? */
#define TIMER_CLIENT_STACK_SIZE 4096
qurt_thread_t      timer_worker_thread_id;

int CreateWorkerThread(unsigned int process_idx)
{
   int ret_value = 0;
   qurt_thread_attr_t tattr;
   unsigned int stackbase;
   char thread_name[QURT_THREAD_ATTR_NAME_MAXLEN];
  
   snprintf(thread_name, sizeof(thread_name), "TMR_CLNT_%u", process_idx);
   stackbase = (unsigned int)qurt_malloc(TIMER_CLIENT_STACK_SIZE);
   qurt_thread_attr_init (&tattr);
   qurt_thread_attr_set_stack_size (&tattr, (TIMER_CLIENT_STACK_SIZE -8));
   qurt_thread_attr_set_stack_addr (&tattr, (void*)((stackbase + 7) & (~7)) );
   qurt_thread_attr_set_priority (&tattr, TIMER_PRI-1);
   qurt_thread_attr_set_name(&tattr, thread_name);
   ret_value =  qurt_thread_create(&timer_worker_thread_id, &tattr, TimerCallbackHandler, NULL);
  
   return ret_value;
}  /* CreateWorkerThread() */


#if 0
/*===========================================================================
DESCRIPTION
   Creates Test Thread for testing Qurt timer apis and their delays
===========================================================================*/
#define TIMER_TEST_PRI 100 
//unsigned long long timer_test_client_stack[4096];
#define TIMER_CLIENT_TEST_STACK_SIZE 4096
qurt_thread_t      timer_test_id;

/* Defined in timer_client_test.c */
extern void timer_client_test
(
  void *parameter
);

int CreateTestThread(void)
{
   int ret_value = 0;
   qurt_thread_attr_t tattr;
   
   unsigned int stackbase;
  
   stackbase = (unsigned int)qurt_malloc(TIMER_CLIENT_TEST_STACK_SIZE);
   qurt_thread_attr_init (&tattr);
   qurt_thread_attr_set_stack_size (&tattr, (TIMER_CLIENT_TEST_STACK_SIZE -8));
   qurt_thread_attr_set_stack_addr (&tattr, (void*)((stackbase +7) &(~7)) );
   qurt_thread_attr_set_priority (&tattr, TIMER_TEST_PRI-1);
   qurt_thread_attr_set_name(&tattr,"TIMER_TEST_CLIENT");
   ret_value =  qurt_thread_create(&timer_test_id, &tattr, timer_client_test, NULL);
  
   return ret_value;
}  /* CreateTestThread() */
#endif /* #if 0 */

/*===========================================================================

FUNCTION    TIMER_CLIENT_INIT

DESCRIPTION
  Initialize the Timer Client service.

DEPENDENCIES
  None.

RETURN VALUE
  
SIDE EFFECTS
  None

===========================================================================*/
void timer_client_init(void) 
{
   //unsigned int ret;
 
   timer_client_pid = qurt_getpid();
    
   timer_client_qdi_handle = qurt_qdi_open(TIMER_DRIVER_NAME);
   if(timer_client_qdi_handle < 0)
   {
      printf("timer_client_init: qdi_open failed");
      ERR_FATAL("timer_client_init :qdi_open failed\n", 0, 0, 0);
      return;
   }
   
   /* 0 is considered as Guest OS Process ID */
   if(timer_client_pid != 0)
   {
      if(CreateWorkerThread(timer_client_pid) != 0)
      {
          printf("timer_client_init : CreateWorkerThread failed\n");
          ERR_FATAL("timer_client_init : CreateWorkerThread failed\n", 0, 0, 0);
          return;
      }
   }

  /* mark the timer null group as deferrable */
  timer_group_set_deferrable(&timer_null_group, TRUE);
 
  /* For testing only */
#if 0       
   if(CreateTestThread() != 0)
   {
       printf("timer_client_init : CreateTestThread failed\n");
       return;
   }
#endif /* #if 0 */

   return;
}  /* timer_client_init () */
