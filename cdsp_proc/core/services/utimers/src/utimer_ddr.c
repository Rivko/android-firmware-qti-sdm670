/*=============================================================================

                 U T I M E R   S E R V I C E   S U B S Y S T E M

                     P U B L I C   A P I S   A N D   D A T A

GENERAL DESCRIPTION
  Implements the utimer functions that goes to DDR memory


REGIONAL FUNCTIONS

  utimer_init()
    Initializes the utimer subsytem.  Call through rcinit.

EXTERNALIZED FUNCTIONS

  utimer_def_osal(timer, group, tcb, sigs, func, data)
    Defines and initializes a timer.

  utimer_set(timer, set duration, reload duration, unit)
    Sets an inactive timer to expire after a given period of time, or changes
    an active timer to expire after a given period of time.
    Optionally, specifies the timer to repeatly expire with a given period.

  utimer_get(timer, unit)
    Get number of time units before utimer expires

  utimer_clr(timer, unit)
    Stops an active timer. Returns 0 if its already inactive
    
  utimer_undef(timer)
    Stops an active timer, relinquishes the handle and frees the timer

INITIALIZATION AND SEQUENCING REQUIREMENTS

    utimer_init() must be called once before any other timer functions.

    utimer_def() must be called, for each timer, before other timer
    functions are used with that timer.

    A valid timetick estimate is needed before timers can be set.
    
    Copyright (c) 2009 - 2017
    by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/services/utimers/src/utimer_ddr.c#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/09/14   ab      Created this file.
=============================================================================*/

#include "comdef.h"
#include "err.h"
//#include "stdint.h"
#include <stringl/stringl.h>

#include "qurt.h"
#include "utimer_osal.h"
#include "utimer_deps_v.h"
#include "utimer_v.h"
#include "qurt_rmutex.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/*-----------------------------------------------------------------------------
  GLOBAL VARIABLES
-----------------------------------------------------------------------------*/

/* UTimers array */
extern utimer_struct_type utimer_buffer[];


extern utimers_type                utimers;

/* Save Max Expiry in this global variable */
extern utimer_timetick_type     utimer_max_expiry;

/* Variables required for Handling timers of remote process */
extern utimer_process_cb_handler utimer_remote_callback_handler;
extern uint32 utimer_curr_process_idx;

extern utimer_non_defer_notification_client_type utimer_non_defer_expiry_notify_clients[];
extern uint32 utimer_non_defer_expiry_notify_clients_cnt;

#ifdef FEATURE_UTIMER_QDSP6

/*Timer signal registered with BLAST. BLAST will set this signal to
 UTIMER_BLAST_SIG_MASK when the match value == timetick value*/
extern qurt_anysignal_t        utimer_q6_signal;

/* Pointer to the mutex object for protecting critical-section of timer 
   code*/
extern qurt_mutex_t utimer_qemu_mutex;

/*MACRO for locking/unlocking the qmutex*/
#define UTIMER_LOCK()  \
  qurt_rmutex_lock(&utimer_qemu_mutex); \
  
#define UTIMER_FREE()  \
  qurt_rmutex_unlock(&utimer_qemu_mutex); \

#endif /* FEATURE_UTIMER_QDSP6 */

#ifdef FEATURE_UTIMER_TRACE
extern qurt_mutex_t utimer_trace_mutex;
#endif

/*=============================================================================
FUNCTION UTIMER_SET_REMOTE_PROCESS_CB_HANDLER

Sets the API that needs to be called when a remote process timer expires.
We will provide to this callback function, the timer callback details
=============================================================================*/
void utimer_drv_set_remote_process_cb_handler(utimer_process_cb_handler shim_cb_func)
{
   utimer_remote_callback_handler = shim_cb_func;
} /* utimer_drv_set_remote_process_cb_handler */

/*=============================================================================
FUNCTION UTIMER_DRV_SET_LOCAL_PROCESS_IDX

Local Process Index needs to be set by timer shim layer so that remote timers
can be differentiated from local process timers
=============================================================================*/
void utimer_drv_set_local_process_idx(uint32 local_process_index)
{
   utimer_curr_process_idx = local_process_index;
} /* utimer_drv_set_local_process_idx */

/*=============================================================================

FUNCTION UTIMER_DRV_DELETE_PROCESS_TIMERS

DESCRIPTION
  Deletes timers related to a process

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS if success
  Otherwise according to error will returns one of timer_error_type
  

SIDE EFFECTS
  None

=============================================================================*/
utimer_error_type utimer_drv_delete_process_timers(uint32 process_idx)
{
   int i;
   utimer_struct_ptr_type   utimer_ptr;
   utimer_timetick_type     ticks_now;
   
   /*---------------------------------------------------*/

   UTIMER_LOCK();   
   
   ticks_now = utimer_timetick_get();

   for(i=0, utimer_ptr = &utimer_buffer[0]; i<UTIMER_BUFFER_SIZE; i++, utimer_ptr++)
      if(utimer_ptr->info.process_idx == process_idx)
      {
         /* Intentionally not memsetting to 0. Just update what is required */
         utimer_ptr->info.timer_state = UTIMER_DEFAULT_FLAG;
         utimer_ptr->start  = ticks_now;
         utimer_ptr->expiry = 0;
      }
   
   utimer_update_timer_interrupt(UTIMER_MVS_DELETE_PROCESS_TIMERS, ticks_now);

   UTIMER_FREE();
   return UTE_SUCCESS;

} /* utimer_drv_delete_process_timers */

/*=============================================================================

FUNCTION UTIMER_DRV_REGISTER_FOR_NON_DEFER_EXPIRY_CHANGE

DESCRIPTION
Registers for first non-deferrable utimer expiry change notification.
Only signal type notifications are allowed, for others, error will be 
returned.

DEPENDENCIES
  Input objects should be initialized by client
  This function should be called from non-uimage

RETURN VALUE
UTE_INVALID_PARAMETERS   If passed sig obj addr is NULL or invalid notify type
UTE_SUCCESS :            If the operation completed successfully

SIDE EFFECTS
  None

=============================================================================*/
utimer_error_type utimer_drv_register_for_non_defer_expiry_change
(
  utimer_notify_type        notify_type,
  utimer_osal_notify_obj_ptr notify_obj,
  utimer_osal_notify_data    notify_mask,
  uint32                   process_idx
)
{
   int idx;
   
   /* This registration is allowed only for signal type notifications */
   if(notify_type != UTIMER_NATIVE_OS_SIGNAL_TYPE
       && notify_type != UTIMER_FUNC1_CB_TYPE
      // && notify_type =! UTIMER_REX_TCB_SIGNAL_TYPE
      )   
      return UTE_INVALID_PARAMETERS;

   if(notify_obj == NULL)   
      return UTE_INVALID_PARAMETERS;
      
   UTIMER_LOCK();
   
   if(utimer_non_defer_expiry_notify_clients_cnt == UTIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT)
   {
      UTIMER_FREE();
      return UTE_FAIL;
   }

   idx = utimer_non_defer_expiry_notify_clients_cnt++;
   utimer_non_defer_expiry_notify_clients[idx].notify_type = notify_type;
   utimer_non_defer_expiry_notify_clients[idx].notify_obj  = notify_obj;
   utimer_non_defer_expiry_notify_clients[idx].notify_mask = notify_mask;
   utimer_non_defer_expiry_notify_clients[idx].process_idx = process_idx;
   
   UTIMER_FREE();
   
   return UTE_SUCCESS;  
} /* utimer_drv_register_for_non_defer_expiry_change */


/*=============================================================================

                             INIT FUNCTIONS

=============================================================================*/

/*=============================================================================

FUNCTION UTIMER_TASK_INIT                                           REGIONAL

DESCRIPTION
  Calls other init routines and spawns utimer_task

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

=============================================================================*/
/* If FEATURE_TIMER_TASK is defined, then the time tick interrupt is
** serviced in the task context.  The timer task should be started
** before enabling the interrupt so that the task is running when an
** interrupt occurs.
** */
extern uint8 task_stack_arr[];
extern qurt_thread_t      utimer_task_thread_id;


void utimer_task_init(void)
{
  /* Initialize the signal mask that we want to wait on*/
  qurt_anysignal_init (&utimer_q6_signal);

  utimer_init();
  
  /* Task needs to be stated as TCM residing task. 
     So RCINIT couldnt be used as it doesnt provide that option */
  //RCINIT_INFO handle = rcinit_lookup("utimer");

  //if (RCINIT_NULL != handle)
  //{
  //  rcinit_initfn_spawn_task(handle, utimer_task);
  //}
  
{
   qurt_thread_attr_t tattr;
   unsigned int stackbase;
  
   stackbase = (unsigned int)&task_stack_arr;
   qurt_thread_attr_init (&tattr);
   qurt_thread_attr_set_stack_size (&tattr, (UTIMER_TASK_STACK_SIZE -8));
   qurt_thread_attr_set_stack_addr (&tattr, (void*)((stackbase + 7) & (~7)) );
   qurt_thread_attr_set_priority (&tattr, UTIMER_TASK_PRI-1);
   qurt_thread_attr_set_tcb_partition(&tattr, 1); // This task should reside in TCM Memory
   qurt_thread_attr_set_name(&tattr, "utimer");
   (void)qurt_thread_create(&utimer_task_thread_id, &tattr, utimer_task, NULL);
}
} /* utimer_task_init */


/*=============================================================================

FUNCTION UTIMER_INIT                                                    REGIONAL

DESCRIPTION
  Initializes the 32kHz slow clock based Timer Subsytem

DEPENDENCIES
  Must only be called from time_init()

RETURN VALUE
  None

SIDE EFFECTS
  May install an ISR for the 32-bit slow clock match register

=============================================================================*/

void utimer_init( void )
{
  int i = 0;
  utimer_struct_ptr_type ptr;
  //utimer_timetick_type ticks_now;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifndef FEATURE_UTIMER_USE_QURT_SYSCLOCK
  /* Dal Result */
  DALResult                       eRes;

  eRes = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &timers.dal_int_handle);
  if( eRes != DAL_SUCCESS || timers.dal_int_handle == NULL )
  {
    /* Error out, we should not start up timer task if we cannot set interrupts through DAL */
    ERR_FATAL("timer_init: Couldn't obtain DalInterrupt Handle",0,0,0);
  }
  #else

  utimer_qurt_sysclock_register(&utimer_q6_signal, UTIMER_BLAST_SIG_MASK);
  #endif /*FEATURE_UTIMER_USE_QURT_SYSCLOCK*/
  
  /*initialize the mutex here*/
  qurt_rmutex_init ( &utimer_qemu_mutex );

  #ifdef FEATURE_UTIMER_TRACE
  qurt_rmutex_init ( &utimer_trace_mutex );
  #endif
  
  //utimer_init_timetick_handle();

  /* Get current slow clock count */
  //ticks_now = utimer_timetick_get();

  //TIMER_API_LOG(TIMER_API_LOG_TIMER_INIT, NULL, ticks_now);
  
  /* Initialize the timers memory */
  for(i = 0, ptr = &utimer_buffer[0]; i < UTIMER_BUFFER_SIZE; i++, ptr++)
  {
    ptr->info.timer_state = UTIMER_DEFAULT_FLAG;
    //utimer_buffer[i].info.index       = i;
  }

  /* Initialize utimers.set_value, set_time values & last_set_time for tests 
     in set_next_interrupt() and update_timer_interrupt */
  utimers.match_value = utimer_max_expiry = QURT_TIMER_MAX_DURATION_TICKS;
  //utimers.set_time          = ticks_now - 1;
  //utimers.last_set_time     = ticks_now - 1;
  
#ifndef FEATURE_UTIMER_QDSP6

  #ifdef TIMETICK_INT_EDGE_TRIGGERED
  
  /* Setup edge-sensitive interrupt, instead of having the interrupt
     asserted for 30us */
  TIMETICK_INT_EDGE_TRIGGERED_INIT();

  #endif /* TIMETICK_INT_EDGE_TRIGGERED */

#endif
} /* utimer_init */

