/*=============================================================================

                 U T I M E R   S E R V I C E   S U B S Y S T E M

                     P U B L I C   A P I S   A N D   D A T A

GENERAL DESCRIPTION
  Implements the utimer functions


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
    
    Copyright (c) 2009 - 2017, 2018
    by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/

/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/services/utimers/src/utimer.c#4 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/14/18   abh     Fix for correction in non-defer expiry notifications.
09/09/16   abh     Changes in the inline function to maintain C99 std.
04/03/14   psu     Update diag macros using optimized versions.
11/07/13   ab      Created this file.
=============================================================================*/

#include "comdef.h"
#include "err.h"
//#include "stdint.h"
#include <stringl/stringl.h>

#ifdef FEATURE_UTIMER_QDSP6
#include "qurt.h"
//Enable this header and necessary code once qurt supports signal2 in uimage
//#include "qurt_signal2.h"
#endif /*FEATURE_UTIMER_QDSP6*/

//#include "assert.h"
//#include "msg.h"
//#include "queue.h"

//#ifndef FEATURE_UTIMER_USE_QURT_SYSCLOCK
//#include "DDIInterruptController.h"
//#include "DALStdErr.h"
//#include "DALDeviceId.h"
//#include "DalDevice.h"
//#endif /*FEATURE_UTIMER_USE_QURT_SYSCLOCK*/

//#if defined(FEATURE_RCINIT)
//#include "rcevt.h"
//#include "rcinit.h"
//#endif

#include "utimer_osal.h"
#include "utimer_deps_v.h"
#include "utimer_v.h"

//#include "../../../core/kernel/qurt/qurtos/include/qurtos_sclk.h"
#include "qurt_rmutex.h"
//#include "qurt_cycles.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/*-----------------------------------------------------------------------------
  Signals used by the utimer task.
-----------------------------------------------------------------------------*/
/* Signal for a timer expiring */
#define UTIMER_EXPIRY_SIG          0x00000001

/* Signal for a change in the active timer list */
#define UTIMER_CHANGE_SIG          0x00000002

/* Signals which must cause the active timer list to be processed */
#define UTIMER_PROCESS_SIGS        (UTIMER_EXPIRY_SIG | UTIMER_CHANGE_SIG)

/* Signals which must be processed to be proper citizens */
#define UTIMER_TASK_SIGS           (UTASK_STOP_SIG  | UTASK_OFFLINE_SIG)

#define MAGIC_INITIALIZER         0xDEADBEEF
#define MAGIC_INITIALIZER_64      0xDEADBEEFD00DFEEDuLL

/* Factor to divide 19.2MHz clock into 32kHz */
#define XO_TO_SCLK_CONV_FACTOR 586

/*-----------------------------------------------------------------------------
  GLOBAL VARIABLES
-----------------------------------------------------------------------------*/

/* If FEATURE_TIMER_TASK is defined, then the time tick interrupt is
** serviced in the task context.  The timer task should be started
** before enabling the interrupt so that the task is running when an
** interrupt occurs.
** */
uint8 task_stack_arr[UTIMER_TASK_STACK_SIZE];
qurt_thread_t      utimer_task_thread_id;

/* UTimers array */
utimer_struct_type utimer_buffer[UTIMER_BUFFER_SIZE];


utimers_type                utimers=
{

  /* Timestamp when call to _set_next_interrupt updates match_value */
  0,

  /* Timestamp of last attempt to update match value */
  0,

  /* Value passed to _set_next_interrupt */
  0,

  /* Timestamp when timer isr occurred at */
  0,

  /* timers_process_active_timers is not executing */
  FALSE,

  /* defer_cnt */
  0,
  
  /* Active Timers count */
  0,
  
  QURT_TIMER_MAX_DURATION_TICKS,/*first_non_def_timer_expiry */

  /* Paused timers count */
  //0,

  //NULL,
};

/* Save Max Expiry in this global variable */
utimer_timetick_type     utimer_max_expiry = 0;

/* Global variable that has recent tick value */
static utimer_timetick_type     ticks_now = 0;

/* Variables required for Handling timers of remote process */
utimer_process_cb_handler utimer_remote_callback_handler = NULL;
uint32 utimer_curr_process_idx = 0;

/* First non-defer expiry change notification clients structure */
#define UTIMER_NON_DEFER_NOTIFICATION_MOCK_PTR ((utimer_ptr_type)0x88888888)
utimer_non_defer_notification_client_type utimer_non_defer_expiry_notify_clients[UTIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT];
uint32 utimer_non_defer_expiry_notify_clients_cnt = 0;

static void utimer_notify_first_non_defer_utimer_expiry_change(void);

/***********************************************
Debug variables
***********************************************/

#ifdef UTIMER_MIN_DEBUG
typedef struct utimer_debug_var_1
{
  utimer_timetick_type now_supplied;
  utimer_timetick_type match_value_supplied;
  utimer_timetick_type match_value_actual_set;
  utimer_timetick_type now_actual;
  //uint32 utimer_address;
  uint8  mv_setter;  
}utimer_match_value_record_type;
#define UTIMER_MAX_MATCH_REC 6
static utimer_match_value_record_type utimer_record_match_val[UTIMER_MAX_MATCH_REC];
static int utimer_record_match_val_counter = 0;


typedef struct utimer_debug_var_3
{
  utimer_timetick_type     processing_started;
  utimer_ptr_type          timer_expired;
  utimer_timetick_type     expiry_value; /* copy the expiry value of the timer */
  utimer_timetick_type     processing_time;
  uint32                   fn_address;
  utimer_properties_type   info;
}utimer_expired_type;
#define MAX_UTIMER_EXPIRED 3

/* For Local process Timers */
static utimer_expired_type utimers_expired[MAX_UTIMER_EXPIRED];
static int utimer_expired_cnt = 0;
int utimer_expired_cnt_global = 0;

/* For Remote process Timers */
static utimer_expired_type utimers_remote_expired[MAX_UTIMER_EXPIRED];
static int utimer_remote_expired_cnt = 0;
int utimer_remote_expired_cnt_global = 0;
#endif /* UTIMER_MIN_DEBUG */

/*These are timer debug variables that were useful to catch many 
 bugs. As of now they are featurized to save space*/
#ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES

typedef struct utimer_debug_var_2
{
  utimer_timetick_type   ts;
  utimer_ptr_type        timer_to_be_set;
  utimer_struct_ptr_type timer_int;
  utimer_timetick_type   expiry;
}utimer_set_type;
#define MAX_UTIMER_SET 3
static utimer_set_type utimer_sets[MAX_UTIMER_SET];
static int utimer_set_cnt = 0;
static int utimer_set_cnt_global = 0;

typedef struct utimer_debug_var_4
{
  utimer_timetick_type   ts;
  utimer_ptr_type        timer_to_be_clr;
  utimer_struct_ptr_type timer_int;
  utimer_error_type      err_returned;
}utimer_clr_type;
#define MAX_UTIMER_CLR 3
static utimer_clr_type utimer_clr_log[MAX_UTIMER_CLR];
static int utimer_clr_cnt = 0;

typedef struct utimer_expired_debug_struct
{
  utimer_timetick_type ts;
  uint32 whileloopcnt;
  utimer_ptr_type timer_exp_ptr;
  utimer_struct_ptr_type timer_int;
  utimer_timetick_type expiry;
  utimer_timetick_type start;
  uint32 tcb_or_func;
  uint8 slave_assigned;
  uint8 callback_type;
}utimer_expired_debug_type;
#define MAX_UTIMER_EXPIRED_DEBUG 6
static utimer_expired_debug_type utimer_expired_debug[MAX_UTIMER_EXPIRED_DEBUG];
static int utimer_expired_debug_cnt = 0;
static uint32 utimer_while_cnt = 0;


/*These values are set high to avoid the annoying F3 messages
 and an error fatal in case cb processing time is high. 
 1 sec = 19200000 ticks. 
*/
volatile static uint64 utimer_processing_time_warning_threshold = 19200;
#ifndef UTIMER_ERROR_FATAL_THRESHOLD_ZEROED_OUT
volatile static uint64 utimer_processing_time_err_fatal_threshold=38400000;
#else
volatile static uint64 utimer_processing_time_err_fatal_threshold=0;
#endif

#endif /* FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES */

#ifdef FEATURE_UTIMER_TRACE

/* Type of timer events to log */
typedef enum
{
  UTIMER_TRACE_ISR,
  UTIMER_TRACE_PROCESS_START,
  UTIMER_TRACE_EXPIRE,
  UTIMER_TRACE_PROCESS_END,
  UTIMER_TRACE_MATCH_VALUE_PROG,
} utimer_trace_event_type;

/* A single timer event trace */
typedef struct
{
  utimer_timetick_type          ts;
  utimer_trace_event_type       event;
  /* The following only apply for EXPIRE events */
  utimer_ptr_type               timer;
  utimer_osal_notify_obj_ptr    sigs_func_addr;
  utimer_osal_notify_data       sigs_mask_data;
  uint32                        callback_type;
  utimer_timetick_type          match_value_programmed;
} utimer_trace_data_type;


/* Structure of all timer log events */
#define UTIMER_TRACE_LENGTH  30
typedef struct
{
  uint32                 index;
  utimer_trace_data_type events[UTIMER_TRACE_LENGTH];
} utimer_trace_type;

/* Declare the log structure */
utimer_trace_type utimer_trace;

qurt_mutex_t utimer_trace_mutex;

#define UTIMER_TRACE_MUTEX_LOCK()  \
  qurt_rmutex_lock(&utimer_trace_mutex); \
  
#define UTIMER_TRACE_MUTEX_UNLOCK()  \
  qurt_rmutex_unlock(&utimer_trace_mutex); \


#define UTIMER_UNDEFINED_DURATION 0xFFFFFFFFFFFFFFFFuLL
/* Macros to perform logging */
  #define UTIMER_TRACE(event)              utimer_trace_event(UTIMER_TRACE_##event, NULL, UTIMER_UNDEFINED_DURATION)
  #define UTIMER_TRACE_TS(event,ts)        utimer_trace_event(UTIMER_TRACE_##event, NULL, ts)
  #define UTIMER_TRACE_EXPIRE(timer)       utimer_trace_event(UTIMER_TRACE_EXPIRE, timer, UTIMER_UNDEFINED_DURATION)
  #define UTIMER_TRACE_EXPIRE_TS(timer,ts) utimer_trace_event(UTIMER_TRACE_EXPIRE, timer, ts)
#else /* if NOT FEATURE_UTIMER_TRACE */
  #define UTIMER_TRACE(event)
  #define UTIMER_TRACE_TS(event,ts)
  #define UTIMER_TRACE_EXPIRE(timer)
  #define UTIMER_TRACE_EXPIRE_TS(timer,ts)
#endif /*FEATURE_UTIMER_TRACE */


#ifdef FEATURE_UTIMER_QDSP6

/*Timer signal registered with BLAST. BLAST will set this signal to
 UTIMER_BLAST_SIG_MASK when the match value == timetick value*/
qurt_anysignal_t        utimer_q6_signal;

/* Pointer to the mutex object for protecting critical-section of timer 
   code*/
qurt_mutex_t utimer_qemu_mutex;

/*MACRO for locking/unlocking the qmutex*/
#define UTIMER_LOCK()  \
  qurt_rmutex_lock(&utimer_qemu_mutex); \
  
#define UTIMER_FREE()  \
  qurt_rmutex_unlock(&utimer_qemu_mutex); \

#define UTIMER_OS_EARLY_TOL 192

#ifdef FEATURE_UTIMER_USE_QURT_SYSCLOCK
/*this function will be called in Q6 for updating the timer
 variables*/
static void utimer_update_isr_time
(
  utimer_timetick_type* now
);
#endif /*FEATURE_UTIMER_USE_QURT_SYSCLOCK*/

#endif /*FEATURE_UTIMER_QDSP6*/


/*=============================================================================

                             MACRO DEFINITIONS

=============================================================================*/
#define UTIMER_INVALID_HANDLE  0xDEADDEAD
#define UTIMER_HANDLE_CONSTANT 0xC3C3

static inline uint32 utimer_index_to_handle(uint32 timer_idx)
{
   uint32 handle;
   handle = ((timer_idx & 0xFFFF) ^ UTIMER_HANDLE_CONSTANT)  << 16 | (timer_idx & 0xFFFF);
   return handle;
}
#define UTIMER_INDEX_TO_HANDLE(tid)   ( utimer_index_to_handle(tid) )

static inline boolean utimer_handle_to_index(uint32 handle, uint32 *timer_idx)
{
   uint16 temp, tid;
  
   /* timer_idx will be from internal functions and assuming it will always be valid */  
//   if(timer_idx == NULL)
//      return FALSE;
   
   temp = (handle >> 16) & 0xFFFF;
   tid  = handle & 0xFFFF;
   
   if((tid ^ UTIMER_HANDLE_CONSTANT) != temp)
      return FALSE;

   *timer_idx = tid;
   return TRUE;
}
#define UTIMER_HANDLE_TO_INDEX(h, tid_ptr) (utimer_handle_to_index(h, tid_ptr))

/*=============================================================================

                             INTERNAL FUNCTIONS

=============================================================================*/

utimer_error_type utimer_get_timer_struct
(
   utimer_ptr_type              timer,
   utimer_ptr_type              timer_actual_addr,   
   utimer_struct_ptr_type      *timer_struct_ptr
)
{
  uint32                        tid;
  utimer_struct_ptr_type        temp_clnt = NULL;
  utimer_state_struct_type      timer_state;
  utimer_error_type             status = UTE_INVALID_TIMER_HANDLE;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Assuming timer NULL check will be done in calling function and 
     timer_struct_ptr will be from internal functions and thus shouldnt need */
//  if(timer == NULL)
//  {
//     return UTE_INVALID_TIMER_HANDLE;
//  }

  *timer_struct_ptr = NULL;
     
  if(*timer != UTIMER_INVALID_HANDLE && UTIMER_HANDLE_TO_INDEX(*timer, &tid) && tid < UTIMER_BUFFER_SIZE)
  {
      temp_clnt = &utimer_buffer[tid];
      timer_state = temp_clnt->info.timer_state;
      
      if(timer_state < UTIMER_DEFAULT_FLAG || timer_state > UTIMER_UNDEF_FLAG)
        ERR_FATAL("Timer 0x%x corrupted. state 0x%x tid: 0x%x", timer, timer_state, tid);
      
      /* Only if the timer structure is in use, otherwise it may be stale Handle */
      if(timer_state != UTIMER_DEFAULT_FLAG
         && temp_clnt->utimer_client_ptr == timer_actual_addr)
      {
         *timer_struct_ptr = temp_clnt;
         status = UTE_SUCCESS;
      }
  }
  
  return status;
} /* utimer_get_timer_struct */

#ifdef FEATURE_UTIMER_TRACE
/*=============================================================================

FUNCTION UTIMER_TRACE_EVENT

DESCRIPTION
  This function saved a timer trace event packet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

static void utimer_trace_event
(
  utimer_trace_event_type event,
  utimer_struct_ptr_type  timer,
  uint64 ts
)
{
  /* Pointer to the trace structure to fill in */
  utimer_trace_data_type *trace;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  UTIMER_TRACE_MUTEX_LOCK();
  /* Get the next trace structure */
  trace = &utimer_trace.events[utimer_trace.index];

  /* Fill in the common fields */
  trace->event = event;

  /* Determine whether or not we need to get a new timestamp */
  if (ts == UTIMER_UNDEFINED_DURATION)
  {
    /* Get a new timestamp */
    trace->ts = utimer_timetick_get();
  }
  else
  {
    /* Use a passed in timestamp */
    trace->ts = ts;
  }

  if ( NULL != timer )
    trace->timer = timer->utimer_client_ptr;
  else
    trace->timer = NULL;

  /*record the match value in the structure*/
  trace->match_value_programmed = utimers.match_value; 

  /* If a timer was given, save some parameters from it */
  if (timer != NULL)
  {
    trace->sigs_func_addr = timer->cbfn.sigs_obj;
    trace->sigs_mask_data = timer->cbdt.sigs;
    trace->callback_type  = timer->info.callback_type;
  }
  else
  {
    trace->sigs_func_addr = NULL;
    trace->sigs_mask_data = 0;
    trace->callback_type  = 0;
  }

  /* Go to the next index */
  utimer_trace.index++;
  if (utimer_trace.index >= UTIMER_TRACE_LENGTH)
  {
    utimer_trace.index = 0;
  }
  UTIMER_TRACE_MUTEX_UNLOCK();

} /* timer_trace_event */
#endif /* FEATURE_TIMER_TRACE */

/*=============================================================================

FUNCTION UTIMER_SET_NEXT_INTERRUPT

DESCRIPTION
  Program the TIMETICK HW to generate interrupt at given match_value

DEPENDENCIES
  Must be called from UTIMER_LOCK context.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt when slow clock counter reaches given value

=============================================================================*/

static void utimer_set_next_interrupt
(
  /* Slow clock count at which next interrupt will occur */
  utimer_timetick_type                   match_count,

  /* Current slow clock count */
  utimer_timetick_type                   ticks_now,

  /* If TRUE, forces MATCH_VAL register to be updated */
  boolean                                force,

  uint8                                  caller
)
{                               
   utimer_timetick_type first_non_def_expiry;
   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Don't re-write the same value to the MATCH_VAL register. */
  if ((force == TRUE || ( utimers.defer_cnt == 0 && match_count != utimers.match_value)))
  {
    #ifdef UTIMER_MIN_DEBUG
    utimer_record_match_val[utimer_record_match_val_counter].mv_setter = caller;
    utimer_record_match_val[utimer_record_match_val_counter].match_value_supplied = match_count;
    utimer_record_match_val[utimer_record_match_val_counter].match_value_actual_set= MAGIC_INITIALIZER_64;
    utimer_record_match_val[utimer_record_match_val_counter].now_supplied = ticks_now;
    //utimer_record_match_val[utimer_record_match_val_counter].timer_address = (uint32)utimers.active.list.first;
    //utimer_record_match_val[utimer_record_match_val_counter].now_actual = MAGIC_INITIALIZER_64;
    #endif /* UTIMER_MIN_DEBUG */
    
    /* Program the match count */
    utimers.match_value = utimer_timetick_set_next_interrupt(match_count, ticks_now);
    
    #ifdef UTIMER_MIN_DEBUG
    utimer_record_match_val[utimer_record_match_val_counter].match_value_actual_set = utimers.match_value;
    utimer_record_match_val_counter = (utimer_record_match_val_counter+1)%UTIMER_MAX_MATCH_REC;
    //DalTimetick_GetTimetick64(hTimerHandle, &utimer_record_match_val[utimer_record_match_val_counter].now_actual);

    /* This is a dummy statement to remove compiler warning */
    utimer_record_match_val[utimer_record_match_val_counter].now_supplied = \
                  utimer_record_match_val[utimer_record_match_val_counter].now_supplied;
    #endif /* UTIMER_MIN_DEBUG */

    /* Save a trace packet with the match value */
    UTIMER_TRACE(MATCH_VALUE_PROG);  

    /* Record when this value was actually written */
    utimers.set_time    = ticks_now;


  /* Record when this match value was logically written, even if we don't
    bother to write the value if it is unchanged. */
    utimers.last_set_time = ticks_now ;
  }

  /* Notify clients of 1st non-def expiry change, if there is any */
    first_non_def_expiry = utimer_drv_get_first_non_deferrable_timer_expiry(NULL);
    if (utimers.first_non_def_timer_expiry != first_non_def_expiry)
    {
      /*there is change in first non-def timer expiry*/
      utimers.first_non_def_timer_expiry = first_non_def_expiry;
      utimer_notify_first_non_defer_utimer_expiry_change();
    }


} /* utimer_set_next_interrupt */


/*=============================================================================

FUNCTION UTIMER_UPDATE_TIMER_INTERRUPT

DESCRIPTION
  If there is a change in timers, update timer match value for next interrupt

DEPENDENCIES
  Must be called from UTIMER_LOCK context.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt when slow clock counter reaches the first timer's expiry point

=============================================================================*/
void utimer_update_timer_interrupt
(
  uint8                   caller,

  utimer_timetick_type    now
)
{
  int                     i;

  utimer_struct_ptr_type  ptr;
  /* Time of first expiring timer */
  utimer_timetick_type    first;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* If a timer is being altered inside "timer_process_active_timers" ... */
  if ( utimers.processing )
  {
    /* ... don't bother updating the timer match value!  Caller will. */
    return;
  }

   /* Are there active timers on the timer list? */
   for(first = utimer_max_expiry, i=0, ptr = &utimer_buffer[0]; i<UTIMER_BUFFER_SIZE; i++, ptr++)
      if(ptr->info.timer_state == UTIMER_SET_FLAG && 
          ptr->expiry < first)
         first = ptr->expiry;
   
  /* Set the next interrupt match value (unless it is unchanged) */
  utimer_set_next_interrupt( first, now, FALSE, caller);

} /* utimer_update_timer_interrupt */


/*=============================================================================

FUNCTION UTIMER_REMOTE_EXPIRE

DESCRIPTION
  Calls Shim layer callback function which will push client timer's callback
   details into that process pipe/queue according to its implementation.

DEPENDENCIES
  Internal timer function

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/
static void utimer_remote_expire
(
  /* Expiring timer to be processed */
  utimer_struct_ptr_type          timer,

  /* Duration timer ran for. */
  utimer_timetick_type            run_time_ms,
  
  utimer_timetick_type*           time_now
)
{
#if (defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES) || (defined UTIMER_MIN_DEBUG)
  /*to logs when the cb processing starts*/
  utimer_timetick_type cb_processing_started = 0;

  /*to log when the cb processing ends*/
  utimer_timetick_type cb_processing_ended = 0;

  /*to log the processing time*/
  utimer_timetick_type cb_processing_time = MAGIC_INITIALIZER_64;
#endif
  
  /*fn address*/
  utimer_t1_cb_type  fn_address_of_timer = timer->cbfn.func1;
    
  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_post_cmd_params_type slave_cmd;
  memset(&slave_cmd, 0, sizeof(timer_slave_post_cmd_params_type));
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  //#ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
  #ifdef UTIMER_MIN_DEBUG
  utimers_remote_expired[utimer_remote_expired_cnt].timer_expired  = timer->utimer_client_ptr;
  utimers_remote_expired[utimer_remote_expired_cnt].info          = timer->info;
  utimers_remote_expired[utimer_remote_expired_cnt].fn_address     = 0;
  utimers_remote_expired[utimer_remote_expired_cnt].processing_started = 0;
  utimers_remote_expired[utimer_remote_expired_cnt].processing_time = MAGIC_INITIALIZER_64;
  #endif

  
#if (defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES) || (defined UTIMER_MIN_DEBUG)
  /*log the time when processing started*/
  cb_processing_started = *time_now;
#endif

  //#ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
  #ifdef UTIMER_MIN_DEBUG
  /*Log the time to indicate the start of processing of this timer*/
  utimers_remote_expired[utimer_remote_expired_cnt].processing_started = cb_processing_started;
  #endif

  /* Call Shim callback function Initially which should be 
     called back for expiring other process related timers */
  if(utimer_remote_callback_handler == NULL)
  {
     ERR_FATAL("No Shim Cb func. Call timer_set_remote_process_timer_handler", 0, 0, 0);
     return;
  }
  
  if ( timer->info.callback_type == UTIMER_NATIVE_OS_SIGNAL_TYPE ||
       //timer->info.callback_type == UTIMER_NATIVE_OS_SIGNAL2_TYPE ||
       timer->info.callback_type == UTIMER_REX_TCB_SIGNAL_TYPE ||
       timer->info.callback_type == UTIMER_FUNC1_CB_TYPE ||
       timer->info.callback_type == UTIMER_FUNC2_CB_TYPE)
  {
  /* Call Shim layer callback which will push callback details into process pipe/queue */
  utimer_remote_callback_handler(timer->info.process_idx, timer->utimer_client_ptr, timer->info.callback_type, 
                          timer->cbfn.sigs_obj, timer->cbdt.data, run_time_ms);
  }
  else if( timer->info.callback_type != UTIMER_NO_NOTIFY_TYPE)
  {
     ERR_FATAL("cb type corrupted timer: 0x%x,cb_type: %d,func: 0x%x", 
                                                                        timer->utimer_client_ptr, 
                                                                        timer->info.callback_type, 
                                                                        fn_address_of_timer);
  }
  
#if (defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES) || (defined UTIMER_MIN_DEBUG)
  /*log the cb ended time*/
  cb_processing_ended = utimer_timetick_get();
  *time_now = cb_processing_ended;
  
  /*to take care of the wrap around case*/
  if(cb_processing_ended >= cb_processing_started)
  {
    cb_processing_time = cb_processing_ended - cb_processing_started;       
  }
  else /*if it is a wrap around case*/
  {  
    cb_processing_time = 0xFFFFFFFFFFFFFFuLL - 
                        (cb_processing_started - cb_processing_ended)+1;
  }
#endif /* (defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES) || (defined UTIMER_MIN_DEBUG) */

  //#ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
  #ifdef UTIMER_MIN_DEBUG
  utimers_remote_expired[utimer_remote_expired_cnt].processing_time = cb_processing_time;

  {
    utimer_remote_expired_cnt = (utimer_remote_expired_cnt+1)%MAX_UTIMER_EXPIRED;
    utimer_remote_expired_cnt_global++;
  }
  #endif /* UTIMER_MIN_DEBUG */

  /* generate a warning or error fatal out if the processing time > threshold*/
  #if (defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES)
  if(cb_processing_time > utimer_processing_time_warning_threshold)
  {
    #ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
    //F3s may not be present in uimage
    /* MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"callback fn = 0x%x took %d ticks, max = %d ticks",
               fn_address_of_timer,
               cb_processing_time,
               utimer_processing_time_warning_threshold); */
    #endif    
  }
  

  if(utimer_processing_time_err_fatal_threshold != 0 && 
     cb_processing_time > utimer_processing_time_err_fatal_threshold)
  {
    #ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
    ERR_FATAL("callback fn 0x%x took %d ticks, err thre %d ticks",
               fn_address_of_timer,
               cb_processing_time,
               utimer_processing_time_err_fatal_threshold);          
    #endif 
   
  }

  #endif /*(defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES)*/
} /* utimer_remote_expire */


/*=============================================================================

FUNCTION UTIMER_EXPIRE

DESCRIPTION
  Processes an expiring timer, according to the timer's definition.
  Internal timer_ function()

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

static void utimer_expire
(
  /* Expiring timer to be processed */
  utimer_struct_ptr_type          timer,

  /* Duration timer ran for. */
  utimer_timetick_type            run_time_ms,
  
  utimer_timetick_type*           time_now
)
{
  /* Timer 'type 1' callback function */
  utimer_t1_cb_type                cb1;

  /* Timer 'type 2' callback function */
  utimer_t2_cb_type                cb2;

#if (defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES) || (defined UTIMER_MIN_DEBUG)
  /* to logs when the cb processing starts */
  utimer_timetick_type cb_processing_started = 0;

  /* to log when the cb processing ends */
  utimer_timetick_type cb_processing_ended = 0;

  /*to log the processing time*/
  utimer_timetick_type cb_processing_time = MAGIC_INITIALIZER_64;
#endif
  
  /*fn address*/
  uint32  fn_address_of_timer = 0;

  /*fn callback type*/
  //uint8 fn_cb_type = 0;
    
  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_post_cmd_params_type slave_cmd;
  memset(&slave_cmd, 0, sizeof(timer_slave_post_cmd_params_type));
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Cache information from timer structure since it could get deleted any
  time after the first callback is made */
  cb1 = timer->cbfn.func1;
  cb2 = timer->cbfn.func2;
  fn_address_of_timer = (uint32)cb1;
  
  //#ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
  #ifdef UTIMER_MIN_DEBUG
  utimers_expired[utimer_expired_cnt].timer_expired  = timer->utimer_client_ptr;
  utimers_expired[utimer_expired_cnt].info          = timer->info;
  utimers_expired[utimer_expired_cnt].fn_address     = 0;
  utimers_expired[utimer_expired_cnt].processing_started = 0;
  utimers_expired[utimer_expired_cnt].processing_time = MAGIC_INITIALIZER_64;
  #endif

#if (defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES) || (defined UTIMER_MIN_DEBUG)
  /*log the time when processing started*/
  cb_processing_started = *time_now;
#endif

  //#ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
  #ifdef UTIMER_MIN_DEBUG
  /*Log the time to indicate the start of processing of this timer*/
  utimers_expired[utimer_expired_cnt].processing_started = cb_processing_started;
  #endif

  /* If signal object is native os object, call native os function to set sigs */

  if ( timer->info.callback_type == UTIMER_NATIVE_OS_SIGNAL_TYPE )
  {
    qurt_anysignal_set((qurt_anysignal_t *)timer->cbfn.sigs_obj, timer->cbdt.sigs);
    
    #ifdef UTIMER_MIN_DEBUG
    utimers_expired[utimer_expired_cnt].fn_address = (uint32)timer->cbfn.sigs_obj;
    #endif
    //fn_cb_type = UTIMER_NATIVE_OS_SIGNAL_TYPE;
  }
  //else if (timer->info.callback_type == UTIMER_NATIVE_OS_SIGNAL2_TYPE) 
  //{
  // qurt_signal2_set((qurt_signal2_t *)timer->cbfn.sigs_obj, timer->cbdt.sigs);
  //  
  //  #ifdef UTIMER_MIN_DEBUG
  //  utimers_expired[utimer_expired_cnt].fn_address = (uint32)timer->cbfn.sigs_obj;
  //  #endif
  //  fn_cb_type = UTIMER_NATIVE_OS_SIGNAL2_TYPE;
  //}
  
  /* If there are signals to be set, and a task to set the signals to,
     then set those signals in that task's task control block */
  else if( timer->info.callback_type == UTIMER_REX_TCB_SIGNAL_TYPE )
  {
#ifdef FEATURE_TIMER_REX_IS_AVAILABLE
    rex_set_sigs((rex_tcb_type *)timer->cbfn.sigs_obj, timer->cbdt.sigs);
    
    #ifdef UTIMER_MIN_DEBUG
    utimers_expired[utimer_expired_cnt].fn_address = (uint32)timer->cbfn.sigs_obj;
    #endif
    //fn_cb_type = UTIMER_REX_TCB_SIGNAL_TYPE;
#else
    ERR_FATAL("On adsp sigs cannot be of rex type. Cb type:%d, sigs_obj:0x%x, sigs:0x%x", timer->info.callback_type, timer->cbfn.sigs_obj, timer->cbdt.sigs);
#endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
  }


  /* If the timer was defined with a type-1 style function callback,
     call the function, or post the function as an asynchronous call */

  else if ( timer->info.callback_type == UTIMER_FUNC1_CB_TYPE)
  {
     //#ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
     #ifdef UTIMER_MIN_DEBUG
     utimers_expired[utimer_expired_cnt].fn_address = (uint32)cb1;
     #endif

    fn_address_of_timer = (uint32)cb1;
    //fn_cb_type          = UTIMER_FUNC1_CB_TYPE;
    
    #ifdef FEATURE_TIMER_SLAVE1
    slave_cmd.timer_ext = timer->utimer_client_ptr;
    slave_cmd.cb_type = TIMER_SLAVE_CB_TYPE1;
    slave_cmd.cb1 = cb1;
    slave_cmd.data = timer->cbdt.data;
    slave_cmd.timer_slave_task_num = timer->info.slave_task_idx;      
    timer_slave_post_cmd(&slave_cmd);
    #else
    cb1( timer->cbdt.data );
    #endif
    
  } /* timer->func1 != NULL */


  /* If the timer was defined with a type-2 style function callback,
     (clock callback style), call the function with the time (in
     milliseconds) between timer start and now. */

  else if ( timer->info.callback_type == UTIMER_FUNC2_CB_TYPE)
  {
    //#ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
    #ifdef UTIMER_MIN_DEBUG
    utimers_expired[utimer_expired_cnt].fn_address = (uint32)cb2;
    #endif

    fn_address_of_timer = (uint32)cb2;
    //fn_cb_type          = UTIMER_FUNC2_CB_TYPE;

    /* Call the callback function */
    #ifdef FEATURE_TIMER_SLAVE1
    slave_cmd.timer_ext = timer->utimer_client_ptr;
    slave_cmd.cb_type   = TIMER_SLAVE_CB_TYPE2;
    slave_cmd.cb2       = cb2;
    slave_cmd.run_time  = (utimer_timetick_type)run_time_ms;
    slave_cmd.data      = timer->cbdt.data;
    slave_cmd.timer_slave_task_num = timer->info.slave_task_idx;
    timer_slave_post_cmd(&slave_cmd);
    #else
    cb2( (utimer_timetick_type) run_time_ms, timer->cbdt.data);
    #endif
  }
  else if( timer->info.callback_type != UTIMER_NO_NOTIFY_TYPE)
  {
     ERR_FATAL("cb type corrupted timer: 0x%x, cb_type: %d, func: 0x%x", 
                                                                        timer->utimer_client_ptr, 
                                                                        timer->info.callback_type, 
                                                                        fn_address_of_timer);
  }

#if (defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES) || (defined UTIMER_MIN_DEBUG)

  /* log the cb ended time */
  cb_processing_ended = utimer_timetick_get();
  *time_now = cb_processing_ended;

  /*to take care of the wrap around case*/
  if(cb_processing_ended >= cb_processing_started)
  {
    cb_processing_time = cb_processing_ended - cb_processing_started;       
  }
  else /*if it is a wrap around case*/
  {  
    cb_processing_time = 0xFFFFFFFFFFFFFFuLL - 
                        (cb_processing_started - cb_processing_ended)+1;
  }
#endif /* (defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES) || (defined UTIMER_MIN_DEBUG) */

  //#ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
  #ifdef UTIMER_MIN_DEBUG
  utimers_expired[utimer_expired_cnt].processing_time = cb_processing_time;

  /*if the timer slave processing is not enabled, then 
    unconditionally increment the counter.
    if the timer slave processing is enabled, then increment 
    only if the signals were set*/
  /* In Adsp, there are no slave tasks and thus all local timers will expire from timer task itself */
  //if(fn_cb_type == UTIMER_NATIVE_OS_SIGNAL_TYPE || fn_cb_type == UTIMER_NATIVE_OS_SIGNAL2_TYPE || fn_cb_type == UTIMER_REX_TCB_SIGNAL_TYPE)
  {
    utimer_expired_cnt = (utimer_expired_cnt+1)%MAX_UTIMER_EXPIRED;
    utimer_expired_cnt_global++;
  }
  #endif

  /*generate a warning or error fatal out if the processing time > threshold*/
  #if (defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES)
  if(cb_processing_time > utimer_processing_time_warning_threshold)
  {
    #ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
    //F3s may not be present in uimage
    /* MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"callback fn = 0x%x took %d ticks, max = %d ticks",
               fn_address_of_timer,
               cb_processing_time,
               utimer_processing_time_warning_threshold); */
    #endif    
  }
  

  if(utimer_processing_time_err_fatal_threshold != 0 && 
     cb_processing_time > utimer_processing_time_err_fatal_threshold)
  {
    #ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
    ERR_FATAL("callback fn = 0x%x took %d ticks, err thre = %d ticks",
               fn_address_of_timer,
               cb_processing_time,
               utimer_processing_time_err_fatal_threshold);          
    #endif 
   
  }

  #endif /*(defined FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES)*/
} /* utimer_expire */


/*=============================================================================

FUNCTION UTIMER_UPDATE_ISR_TIME

DESCRIPTION
  This function is called in BLAST, when the timer signal is set. In Q6, 
  there is no timer interrupt, but for backward compatibility, the variables
  timer.isr_time, etc. need to be populated.

DEPENDENCIES
  This function is applicable only for BLAST.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
#ifdef FEATURE_UTIMER_USE_QURT_SYSCLOCK
void utimer_update_isr_time
(
  utimer_timetick_type* now
)
{
  /* Timer sclk time-stamp values */
  utimer_timetick_type                   set_time, set_value;

  /* Latency in ISR handling */
  utimer_timetick_type                   latency_sclk;

  /* Save a trace packet */
  UTIMER_TRACE_TS(ISR, *now);
  
  utimers.isr_time = *now;

  /* Capture timer set values, for messaging outside of UTIMER_LOCK. */
  set_time  = utimers.set_time;
  set_value = utimers.match_value;
  
  if ( *now - set_time >= set_value - set_time)
  {
    /* Determine the difference between when the interrupt was programmed to
       occur at, and when the interrupt was actually handled. */
    latency_sclk = *now - set_value;

    if ( latency_sclk > UTIMER_SCLK_LATENCY )
    {
      /* Timer interrupt was handled over btw 5 (60kHz) to 10ms (30kHz) late.
         (Timers may expire much later than 5-10ms, if they are in a disabled
         group, or if multiple timers expire due to the same interrupt.
         This is only a measure of the timer isr latency.) */

      //MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Late Timer ISR: ST=%d SV=%d IT-SV=%d",
      //          set_time, set_value, latency_sclk);
    }
  }

}
#endif /*FEATURE_UTIMER_USE_QURT_SYSCLOCK*/

/*=============================================================================

FUNCTION UTIMER_PROCESS_ACTIVE_TIMERS

DESCRIPTION
  Internal timer function
  Process timers, decrementing elapsed time from each one.

DEPENDENCIES
  Must be called from inside UTIMER_LOCK context, to
  prevent a large delay between reading the current timetick count, and
  writing the new timetick match value.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

void utimer_process_active_timers( void )
{
  /* Timer being processed */
  utimer_struct_ptr_type               timer;

  /* Length of time the timer ran for */
  utimer_timetick_type                 run_time;
  
    /* Duration timer ran for, in milliseconds */
  utimer_timetick_type                 run_time_ms = 0;

/* Todo: Delete all the commented code and keep only what is required */
#if 0
  /* Duration timer ran for, in milliseconds */
  utimer_timetick_type                 run_time_sclk;
#endif /* #if 0 */

  /* Minimum advance required for reloading timer */
  utimer_timetick_type                 min_advance;

  /* Temporary value to compute the new expiry point */
  utimer_timetick_type                 new_expiry;

  /* Temp client timer object */
  utimer_struct_type                   temp_clnt_timer;

  int                                  i = 0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Lock interrupts while manipulating the active timer list */
  UTIMER_LOCK();
  
  /* Record the fact that timers are being processed, to prevent re-entry
     into this function, and unnecessary modification of TIMETICK_MATCH. */
  utimers.processing = TRUE;

  /* Get current slow clock count */
  ticks_now = utimer_timetick_get();
  
  /*record the time at which the blast signal came*/
  #ifdef FEATURE_UTIMER_USE_QURT_SYSCLOCK
  utimer_update_isr_time(&ticks_now);
  #endif /*FEATURE_UTIMER_USE_QURT_SYSCLOCK*/
  
  /* Save a trace packet */
  UTIMER_TRACE_TS(PROCESS_START, ticks_now);
  
#if defined (FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES)
  utimer_while_cnt = 0;
#endif 
  
  /* Check to determine if the timer has past its expiry point. If it 
     has, remove it, expire the timer, and repeat with the next timer. */
  for ( i=0; i<UTIMER_BUFFER_SIZE; i++)
  {
    /* Get the timer pointer */
    timer = &utimer_buffer[i];
    
    if(timer->info.timer_state == UTIMER_SET_FLAG && 
        timer->expiry <= (ticks_now + UTIMER_OS_EARLY_TOL))
    {

    #if defined (FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES)
    utimer_while_cnt++;
    #endif
    
    #ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
    utimer_expired_debug[utimer_expired_debug_cnt].whileloopcnt = utimer_while_cnt;
    utimer_expired_debug[utimer_expired_debug_cnt].timer_exp_ptr = timer->utimer_client_ptr;
    utimer_expired_debug[utimer_expired_debug_cnt].timer_int = timer;
    utimer_expired_debug[utimer_expired_debug_cnt].ts = ticks_now;
    utimer_expired_debug[utimer_expired_debug_cnt].start = timer->start;
    utimer_expired_debug[utimer_expired_debug_cnt].expiry = timer->expiry;
    utimer_expired_debug[utimer_expired_debug_cnt].callback_type = timer->info.callback_type;
    utimer_expired_debug[utimer_expired_debug_cnt].tcb_or_func = (uint32)timer->cbfn.sigs_obj;
    utimer_expired_debug_cnt = (utimer_expired_debug_cnt+1)%MAX_UTIMER_EXPIRED_DEBUG;
    #endif /*FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES*/

    /* Compute length of time the timer ran for.  This may be longer than
       programmed due to sleep or other delays in expiry processing. */
    run_time = ticks_now - timer->start;

    /* Compute time (in milliseconds) timer has been running for.
       This may be larger than expected if the timer expires during sleep,
       or otherwise has its expiry processing delayed */
    run_time_ms = utimer_timetick_conv_from_ticks( run_time, UT_MSEC);

    /* Reactivate timer, if required */
    if ( timer->reload > 0 )
    {
      /* Determine how late the timer expired; this is the minimum
         amount the timer must be advanced by for the next expiry. */
      min_advance = ticks_now - timer->expiry;
  
      new_expiry = timer->expiry + timer->reload;
      
      if(new_expiry  <= ticks_now)
      {
        /* Temporary value to compute the new expiry point */
        new_expiry = timer->expiry;
        
        /* Timer expired 1 or more reload period ago.  This can happen if
           the timer belongs to a timer group which gets disabled, such as
           the default timer groups during sleep. */

        /* Round min_advance up to the next multiple of reload periods. */
        min_advance += timer->reload - min_advance % timer->reload;

        /* Add the rounded-up minimum advance to the timer expiry */
        new_expiry += min_advance;
      }

        /* Check to make sure that the new expiry point is further in the future
           than the old one.  This prevents the cases where overflow in the
           calculation could occur or wrap around past the active timer list
           zero. */
        
        if ( new_expiry  > timer->expiry )
        {
          /* New expiry point is further in the future than the old one, use it */
          timer->expiry = new_expiry;
        }
        else
        {
          /* The recurring timer’s new expiry point is at a point so far in the 
             future that it exceeds timer dynamic range.  Because of this, move  
             the expiry to just fit the available range */

          /* Move the expiry point as far out as possible */
          timer->expiry = utimer_max_expiry; 
        }

      /* Record the new start time for the next expiry */
      timer->info.timer_state = UTIMER_SET_FLAG; 
      timer->start            = ticks_now;

#if 0 /* This code may not be required for XO ticks. */
        /* If a timer is a reloading timer, the millisecond conversion will have
           a rounding error.  We can adjust the start time to account for the
           residual.  This is done before signalling the timer expiry, since
           the timer expiry might alter the timer data */
      if ( timer->info.callback_type == UTIMER_FUNC2_CB_TYPE)
      {
          /* Difference btw run_time & run_time_sclk is rounding error (+/-.5ms) */
          run_time_sclk = utimer_timetick_conv_to_ticks( run_time_ms, T_MSEC);

          /* Nudge start time, so the next expiry includes this rounding error */
          if ( run_time >= run_time_sclk )
          {
            /* Rounded down: move start backwards, so next duration is longer */
            timer->start -= run_time - run_time_sclk;
          }
          else if ( timer->expiry - timer->start > run_time_sclk - run_time )
          {
            /* Rounded up: move start forward, so next duration is shorter */
            timer->start += run_time_sclk - run_time;
          }
          else
          {
            /* Rounded up, but next expiry is too close for rounding nudging. */
            timer->start = timer->expiry - 1;
          }
      }
#endif /* #if 0 */
    }
    else
    {
      timer->info.timer_state = UTIMER_EXPIRED_FLAG; 
      //timer->start            = ticks_now; 
      timer->expiry           = 0; 
    }
    
      /* After intfree() call, timer may get deleted or cleared. Therefore, backup func, sigs details before releasing lock */
      memscpy(&temp_clnt_timer, sizeof(temp_clnt_timer), timer, sizeof(utimer_struct_type));

      /* Save a trace packet */
      UTIMER_TRACE_EXPIRE_TS(&temp_clnt_timer, ticks_now);
    
      /* We've finished manipulating the active timer list.  Unlock
       interrupts for the duration of the timer's expiry processing */
      UTIMER_FREE();
  
      if(temp_clnt_timer.info.remote_process == UTIMER_FROM_REMOTE_PROCESS)
      {
         /* Expire timer */
         utimer_remote_expire(&temp_clnt_timer, run_time_ms, &ticks_now);
      }
      else
      {
        /* Expire timer */
        utimer_expire( &temp_clnt_timer, run_time_ms, &ticks_now );
      }

    /* NOTE: The above call may run user code, which can do anything ...
       including installing and uninstall other timers.  This can cause
       the value of timers.active.list.zero to change at this point.
       We must use the new value in the next iteration, so any local
       variable which holds computations involving that value must
       be considered invalid after the above call.  */


    /* Lock interrupts in preparation of the next iteration of the loop,
       which tests & manipulates the active timer list */
    UTIMER_LOCK( );
    }

    /* Get current tick count */
    ticks_now = utimer_timetick_get();

  } /* for timers in buffer */
  
#ifndef FEATURE_UTIMER_QDSP6

  #ifndef TIMETICK_INT_EDGE_TRIGGERED

  /* Install the timer expiry ISR */
  tramp_set_isr( TRAMP_SLP_TICK_ISR, timer_task_isr );

  /* Ensure interrupt source has deasserted before continuing.
     This must occur before the timer_set_next_interrupt(), to prevent
     accidently clearing the interrupt for the next timer. */
  tramp_block_till_deasserted( TRAMP_SLP_TICK_ISR, 30 );

  #endif /* !TIMETICK_INT_EDGE_TRIGGERED */
#endif /* FEATURE_UTIMER_QDSP6 */

    /* Timer processing has completed */
    utimers.processing = FALSE;
  
  /* Timers that expire at and before "ticks_now" have been processed.
     Set interrupt for when next timer expires. */
    utimer_update_timer_interrupt(UTIMER_MVS_TIMER_PROCESSED, ticks_now);

    /* Save a trace packet */
    UTIMER_TRACE_TS(PROCESS_END, ticks_now);

  /* We've finished manipulating the active timer list.  */
  UTIMER_FREE( );
  
  #if defined(T_QUASAR) && !defined(T_QUASAR_X)
  /* Because of a hardware problem with QUASar implementation of this
     timer, do read on the timer count until it's not zero...
     The problem is that for some reason, the SLEEP_XTAL_TIMETICK_COUNT
     will read zero for a while (maybe a 10-20 us) if you read it right
     after the ..._VAL is written to.  This is an easy workaround, instead
     having to check for the actual condition of ..._VAL being written
     (_VAL is written in timer_set_next_interrupt above). */
  while ( MSM_R( SLEEP_XTAL_TIMETICK_COUNT ) == 0 )
  {
    /* Do nothing */
  }
  #endif /* T_QUASAR */

} /* utimer_process_active_timers */


/*=============================================================================

FUNCTION UTIMER_EXPIRES_IN

DESCRIPTION
  Get number of ticks before timer expires

DEPENDENCIES
  Timer must be active
  Internal function, called from UTIMER_LOCK context.

RETURN VALUE
  Number of ticks before timer expires.
  Zero is returned for inactive timers, and timers that have expired or
  are expiring.

SIDE EFFECTS
  None

=============================================================================*/

utimer_timetick_type utimer_expires_in
(
  /* Timer to get */
  utimer_struct_ptr_type                utimer_ptr,

  /* Current slow clock tick count */
  utimer_timetick_type*                 now
)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Determine if the timer has expired or not */

  if ( utimer_ptr->expiry   >=  (*now)  )
  {
    /* Timer has not expired - compute time till expiry. */

    return ( utimer_ptr->expiry - (*now) );
  }
  else
  {
    /* Timer mush be active and thus sending minimum 1 tick */

    return 1;
  }

} /* utimer_expires_in */


/*=============================================================================

                             UTIMER API FUNCTIONS

=============================================================================*/

#ifdef FEATURE_UTIMER_DEFER_SUPPORT
utimer_error_type utimer_drv_set_deferrable
( 
   utimer_ptr_type timer, 
   utimer_ptr_type timer_actual_addr, /* For debug purposes only */
   boolean         condition
)
{
   utimer_error_type status;
   
   /* Pointer to timer structure */
   utimer_struct_ptr_type         utimer_ptr;
   
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

   /* Critical section moved to utimer_qdi.c */
   //UTIMER_LOCK();

   if(UTE_SUCCESS != (status = utimer_get_timer_struct(timer, timer_actual_addr, &utimer_ptr)))
   {
      //UTIMER_FREE();
      return status;
   }
   
   if(condition)
      utimer_ptr->info.deferrable = UTIMER_DEFERRABLE_FLAG;
   else
      utimer_ptr->info.deferrable = 0;
   
   //UTIMER_FREE();
   return UTE_SUCCESS;
} /* utimer_drv_set_deferrable */
#endif /* #ifdef FEATURE_UTIMER_DEFER_SUPPORT */


/*=============================================================================

FUNCTION UTIMER_DRV_DEF_OSAL

DESCRIPTION
 Allocates a timer from utimer_buffer and initializes it for the client

DEPENDENCIES
  None

RETURN VALUE
  utimer_error_type

SIDE EFFECTS
  None

=============================================================================*/
utimer_error_type utimer_drv_def_osal
(
  utimer_ptr_type                  timer,
  /* Timer's Actual Address in its own Process Domain, for Logging purposes only */
  utimer_ptr_type                  timer_actual_addr,
  utimer_notify_type               cb_type,
  utimer_osal_notify_obj_ptr       sigs_func_ptr,
  utimer_osal_notify_data          sigs_mask_data,
  unsigned int                     process_idx
)
{
   uint32 i = 0;
   utimer_struct_ptr_type   utimer_ptr;
   
   /* ------------------------------------------ */
   
   /* moved to utimer_client.c */
   //if(timer == NULL || cb_type >= UTIMER_INVALID_NOTIFY_TYPE || (sigs_func_ptr == NULL && sigs_mask_data != 0))
   //   return UTE_INVALID_PARAMETERS;
   
   /* Critical section moved to utimer_qdi.c */
   //UTIMER_LOCK();
     
   if(UTE_SUCCESS == utimer_get_timer_struct(timer, timer_actual_addr, &utimer_ptr))
   {
      //UTIMER_FREE();
      //ERR_FATAL("Handle already in use: timer: 0x%x, Handle: 0x%x", timer, *timer, 0);
      return UTE_HANDLE_IN_USE;
   }
  
  /* Get timer structure that is free */
  for(i=0, utimer_ptr = &utimer_buffer[0]; i<UTIMER_BUFFER_SIZE && utimer_ptr->info.timer_state != UTIMER_DEFAULT_FLAG; i++, utimer_ptr++);
  
  if(i == UTIMER_BUFFER_SIZE)
  {
    //UTIMER_FREE();
    //ERR_FATAL("No utimer is free", 0, 0, 0);
    return UTE_NO_FREE_TIMER_STRUCT;
  }
  
  if(utimer_ptr) 
  {
    /* Initialize to zero */
    memset((void*)utimer_ptr, 0, sizeof(utimer_struct_type));
  
  
    utimer_ptr->info.timer_state   = UTIMER_DEF_FLAG;
    //utimer_ptr->start              = 0;       /* timer is defined at this timetick */
    utimer_ptr->info.callback_type = cb_type;
    utimer_ptr->utimer_client_ptr  = timer_actual_addr;
    utimer_ptr->cbfn.func1         = sigs_func_ptr;
    utimer_ptr->cbdt.data          = sigs_mask_data;
  
    utimer_ptr->info.process_idx   = process_idx;
    if(process_idx != utimer_curr_process_idx)
      utimer_ptr->info.remote_process = UTIMER_FROM_REMOTE_PROCESS;
  }

  *timer = UTIMER_INDEX_TO_HANDLE(i);
  
  //UTIMER_FREE();
  return UTE_SUCCESS;
} /* utimer_def_osal */


/*=============================================================================

FUNCTION UTIMER_DRV_SET_ABSOLUTE

DESCRIPTION
 Sets/Re-Sets a timer

DEPENDENCIES
  Timer should be already defined

RETURN VALUE
  utimer_error_type

SIDE EFFECTS
  May reprogram the timer interrupt

=============================================================================*/
utimer_error_type utimer_drv_set_absolute
(
  utimer_ptr_type                  timer,
/* Timer's Actual Address in its own Process Domain, for Logging purposes only */
  utimer_ptr_type                  timer_actual_addr,
  utimer_timetick_type             time,
  utimer_timetick_type             reload,
  utimer_unit_type                 unit
)
{
   utimer_error_type        status;
   utimer_struct_ptr_type   utimer_ptr;
   
   if(UTE_SUCCESS != (status = utimer_get_timer_struct(timer, timer_actual_addr, &utimer_ptr)))
   {
      return status;
   }
   
   ticks_now = utimer_timetick_get();
   
   /* update utimer structure */
   utimer_ptr->info.timer_state = UTIMER_SET_FLAG;
   utimer_ptr->info.unit = unit;
   utimer_ptr->start  = ticks_now;
   utimer_ptr->reload = reload;
   utimer_ptr->expiry = time;
   
   utimer_update_timer_interrupt(UTIMER_MVS_TIMER_SET, ticks_now);
   
   #ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
   utimer_sets[utimer_set_cnt].timer_to_be_set = utimer_ptr->utimer_client_ptr;
   utimer_sets[utimer_set_cnt].timer_int       = utimer_ptr;
   utimer_sets[utimer_set_cnt].ts              = utimer_ptr->start;
   utimer_sets[utimer_set_cnt].expiry          = utimer_ptr->expiry;
   utimer_set_cnt = (utimer_set_cnt + 1) % MAX_UTIMER_SET;
   utimer_set_cnt_global++;
  
   /* This is a dummy statement to remove compiler warnings */
   utimer_sets[utimer_set_cnt].ts = utimer_sets[utimer_set_cnt].ts;
   #endif /* FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES */
  
   //UTIMER_FREE();
   return UTE_SUCCESS;
} /* utimer_drv_set_absolute */

/*=============================================================================

FUNCTION TIMER_DRV_GET_64

DESCRIPTION
  Gets the number of time units before the timer expires.

DEPENDENCIES
  None

RETURN VALUE
  Number of units before timer expires in the unit specified. If the unit 
is ticks, it will return value in 19MHz domain.

SIDE EFFECTS
  None

=============================================================================*/
utimer_timetick_type utimer_drv_get_64
(
  /* Timer to get */
  utimer_ptr_type                 timer,
  
  /* Timer's Actual Address in its own Process Domain, for Logging purposes only */
  utimer_ptr_type                 timer_actual_addr,
  
  /* Unit to measure "ticks before timer expires" in. */
  utimer_unit_type                unit
)
{
   /* Ticks until timer expires */
   utimer_timetick_type            ticks = 0;
   
   /* Pointer to timer structure */
   utimer_struct_ptr_type         utimer_ptr;
   
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Critical section moved to utimer_qdi.c */
  //UTIMER_LOCK();

  /* This API LOG should be before any possible early exit of the function */
  //TIMER_API_LOG(TIMER_API_LOG_TIMER_GET, timer, timetick_get_tmp64());
  
   if(UTE_SUCCESS != utimer_get_timer_struct(timer, timer_actual_addr, &utimer_ptr))
   {
      //UTIMER_FREE();
      return 0;
   }
  
  if ( utimer_ptr->info.timer_state == UTIMER_SET_FLAG)
  {
    /* Timer is active. Ensure non-zero tick count */
    ticks_now = utimer_timetick_get();
    ticks = utimer_expires_in( utimer_ptr, &ticks_now );
    if ( ticks == 0 )
    {
      ticks = 1;
    }
  }

  //UTIMER_FREE();

  /* If timer is active then ensure that return value is non-zero */
  if ( ticks != 0 )
  {
    ticks = utimer_timetick_conv_from_ticks( ticks, unit);
    if ( ticks == 0 )
    {
      ticks = 1;
    }
  }

  return ticks;
} /* utimer_drv_get_64 */

/*=============================================================================

FUNCTION UTIMER_DRV_CLR_64

DESCRIPTION
 Clears a timer

DEPENDENCIES
  Timer should be already defined

RETURN VALUE
  utimer_error_type

SIDE EFFECTS
  May reprogram the timer interrupt

=============================================================================*/
utimer_error_type utimer_drv_clr_64
(
  utimer_ptr_type                timer,
  /* Timer's Actual Address in its own Process Domain, for Logging purposes only */
  utimer_ptr_type                timer_actual_addr,
  utimer_unit_type               unit,
  utimer_timetick_type          *rem_time
)
{
   utimer_error_type        status = UTE_SUCCESS;
   utimer_struct_ptr_type   utimer_ptr = NULL;
   utimer_timetick_type     ticks;

   /*---------------------------------------------*/
   
   /* moved to utimer_client.c */
   //if(timer == NULL)
   //   return UTE_INVALID_PARAMETERS;
   
   /* Critical section moved to utimer_qdi.c */
   //UTIMER_LOCK();

   *rem_time = 0;
   status = utimer_get_timer_struct(timer, timer_actual_addr, &utimer_ptr);
   
   if(status == UTE_SUCCESS && utimer_ptr->info.timer_state != UTIMER_SET_FLAG)
   {
      //UTIMER_FREE();
      status = UTE_TIMER_ALREADY_IN_SAME_STATE;
   }
   
   ticks_now = utimer_timetick_get();
   
   if(status == UTE_SUCCESS)
   {
       ticks = utimer_expires_in( utimer_ptr, &ticks_now );
       
       utimer_ptr->info.timer_state = UTIMER_CLEARED_FLAG;
       //utimer_ptr->start            = ticks_now;
       utimer_ptr->expiry           = 0;
       
       utimer_update_timer_interrupt(UTIMER_MVS_TIMER_CLEARED, ticks_now);
       
       //UTIMER_FREE();
       
       *rem_time = utimer_timetick_conv_from_ticks( ticks, unit );
          
       /* Make sure to return atleast 1 if we are clearing an active timer */
       if(*rem_time == 0 && ticks != 0)
          *rem_time = 1;
   }
   
   #ifdef FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES
   utimer_clr_log[utimer_clr_cnt].timer_to_be_clr = timer;
   utimer_clr_log[utimer_clr_cnt].timer_int       = utimer_ptr;
   utimer_clr_log[utimer_clr_cnt].ts              = ticks_now;
   utimer_clr_log[utimer_clr_cnt].err_returned    = status;
   utimer_clr_cnt = (utimer_clr_cnt + 1) % MAX_UTIMER_CLR;
  
   /* This is a dummy statement to remove compiler warnings */
   utimer_clr_log[utimer_clr_cnt].ts = utimer_clr_log[utimer_clr_cnt].ts;
   #endif /* FEATURE_ENABLE_UTIMER_DEBUG_VARIABLES */
   
   return status;
} /* utimer_drv_clr_64 */


/*=============================================================================

FUNCTION UTIMER_DRV_UNDEF

DESCRIPTION
  Undefines a timer. Free up the timer to be used by another client

DEPENDENCIES
  Timer should be already defined

RETURN VALUE
  utimer_error_type

SIDE EFFECTS
  May reprogram the timer interrupt

=============================================================================*/
utimer_error_type utimer_drv_undef
(
  /* Timer to Undefine */
  utimer_ptr_type                  timer,
  /* Timer's Actual Address in its own Process Domain, for Logging purposes only */
  utimer_ptr_type                  timer_actual_addr
)
{
   utimer_error_type        status;
   utimer_struct_ptr_type   utimer_ptr;
   utimer_state_struct_type prev_state;
   
   /*---------------------------------------------------*/

   /* moved to utimer_client.c */
   //if(timer == NULL)
   //   return UTE_INVALID_PARAMETERS;
   
   /* Critical section moved to utimer_qdi.c */
   //UTIMER_LOCK();
   
   if(UTE_SUCCESS != (status = utimer_get_timer_struct(timer, timer_actual_addr, &utimer_ptr)))
   {
      //UTIMER_FREE();
      return status;
   }
   
   ticks_now = utimer_timetick_get();
   
   prev_state = utimer_ptr->info.timer_state;
   
   /* Intentionally not memsetting to 0. Just update what is required */
   utimer_ptr->info.timer_state = UTIMER_DEFAULT_FLAG;
   //utimer_ptr->start  = ticks_now;
   utimer_ptr->expiry = 0;
   
   /* Reprogram hw if the undef'd timer is an active one */
   if(prev_state == UTIMER_SET_FLAG)
      utimer_update_timer_interrupt(UTIMER_MVS_TIMER_UNDEFINED, ticks_now);
   
   *timer = UTIMER_INVALID_HANDLE;
   
   //UTIMER_FREE();
   return UTE_SUCCESS;
} /* utimer_drv_undef */

/*=============================================================================

FUNCTION UTIMER_NOTIFY_FIRST_NON_DEFER_UTIMER_EXPIRY_CHANGE

DESCRIPTION
  Calls the client's callbacks to notify the changed first non-deferrable timer 
  expiry

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

=============================================================================*/

static void utimer_notify_first_non_defer_utimer_expiry_change(void)
{
   int i;
   utimer_notify_type          notify_type;
   utimer_osal_notify_obj_ptr  notify_obj; 
   utimer_osal_notify_data     notify_mask;
   uint32                      process_idx;

   for(i=0; i<utimer_non_defer_expiry_notify_clients_cnt; i++)
   {
   
    notify_type = utimer_non_defer_expiry_notify_clients[i].notify_type;
    notify_obj  = utimer_non_defer_expiry_notify_clients[i].notify_obj;
    notify_mask = utimer_non_defer_expiry_notify_clients[i].notify_mask;
    process_idx = utimer_non_defer_expiry_notify_clients[i].process_idx;

    if(notify_obj == NULL)         
      ERR_FATAL("Notify_obj NULL means corruption happened; i=%d", i, 0, 0);     
	 
    /* If there are signals to be set, and a task / signal object to set the signals to,
            then set those signals in that task's task control block or the passed signal object */
	if (process_idx == utimer_curr_process_idx )
	{
		if ( notify_type == UTIMER_NATIVE_OS_SIGNAL_TYPE )
		{
		   qurt_anysignal_set((qurt_anysignal_t *)notify_obj, notify_mask);
		}
     	else if ( notify_type == UTIMER_REX_TCB_SIGNAL_TYPE )
     	{
		  #ifdef FEATURE_TIMER_REX_IS_AVAILABLE
		   (void) rex_set_sigs( notify_obj, notify_mask );
		  #else
		   ERR_FATAL("Signal type cannot be rex type as its not supported on this proc",0,0,0);
		  #endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
		}
		else if ( notify_type == UTIMER_FUNC1_CB_TYPE )
		{
         /*notify_mask contains client data provided while registeration*/
         ((utimer_t1_cb_type)notify_obj)(notify_mask);
 
		}
		else
		{
		  ERR_FATAL("Invalid Notify type 0x%x, may be corruption",notify_type,0,0);
		}
	}
	else
	{
      if(utimer_remote_callback_handler == NULL)
        {
          ERR_FATAL("No Shim Cb func. Call utimer_set_remote_process_timer_handler()", 0, 0, 0);
          return;
      	}
	  else
	  	{
          utimer_remote_callback_handler(process_idx, UTIMER_NON_DEFER_NOTIFICATION_MOCK_PTR, notify_type, 
                          notify_obj, notify_mask, 0);          
	  	}
	}
   } /* for() */
     
   return;
} /* utimer_notify_first_non_defer_utimer_expiry_change */


/*=============================================================================

FUNCTION UTIMER_DRV_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY

DESCRIPTION
  Gets time to the next occurring non-deferrable timer expiry value. 

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of ticks in 19MHz domain until the next timer expiry in a
 timer group which is non-deferrable.

SIDE EFFECTS
  None

=============================================================================*/

utimer_timetick_type utimer_drv_get_first_non_deferrable_timer_expiry( 
                                           utimer_non_defer_data_ptr utimer_data_ptr )
{
  utimer_timetick_type                   future_expiry;
  utimer_struct_ptr_type                 ptimer;
  uint32                                 i;
  uint8                                  index = 0xFF;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  UTIMER_LOCK();

  /* Assume the worst - no timers on list matching our criteria set to expire */
   for(future_expiry = utimer_max_expiry, i=0, ptimer = &utimer_buffer[0]; i<UTIMER_BUFFER_SIZE; i++, ptimer++)
   {
      if(ptimer->info.timer_state == UTIMER_SET_FLAG && ptimer->info.deferrable == 0 && ptimer->expiry < future_expiry)
      {
        future_expiry = ptimer->expiry;
        index = i;
      }
   }
   
   if(utimer_data_ptr != NULL)
   {
     if(index != 0xFF)
      {
        utimer_data_ptr->cb_type = utimer_buffer[index].info.callback_type;
        utimer_data_ptr->sigs_func_addr = utimer_buffer[index].cbfn.func1;
        utimer_data_ptr->sigs_mask_data = utimer_buffer[index].cbdt.data;
      }
     else
      { /*When non-deferrable timer is not present*/
        utimer_data_ptr->cb_type = UTIMER_INVALID_NOTIFY_TYPE;
        utimer_data_ptr->sigs_func_addr = NULL;
        utimer_data_ptr->sigs_mask_data = 0;
      }
    }

  UTIMER_FREE();

  return future_expiry;
} /* utimer_drv_get_first_non_deferrable_timer_expiry */

#ifdef FEATURE_UTIMER_DEFER_SUPPORT
/*=============================================================================

FUNCTION UTIMER_DRV_DEFER_MATCH_INTERRUPT_64

DESCRIPTION
  Sets timer match interrupt to the next occurring non-deferrable timer expiry 
  value. Complimentary function is timer_undefer_match_interrupt_64(). 

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
utimer_timetick_type -- Number of ticks in 19MHz domain 
until the next timer expiry in a timer group that is 
non-deferrable. 

SIDE EFFECTS
  Adjusts match value and prevents any subsequent timer calls from updating the
  match value, until timer_undefer_match_interrupt_64() is called.

=============================================================================*/
utimer_timetick_type utimer_drv_defer_match_interrupt_64( void )
{
  utimer_timetick_type                   future_expiry;
  utimer_timetick_type                   tt_ret;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  UTIMER_LOCK();

  /* From this point on timer interrupt  won’t be adjusted by timer set/clr apis */
  utimers.defer_cnt++;

  /* Get the current time */
  ticks_now = utimer_timetick_get();

  future_expiry = utimer_drv_get_first_non_deferrable_timer_expiry(NULL);

  /* If future_expiry is in past */
  if (future_expiry  < ticks_now )
  {
    UTIMER_FREE();
    return 0;
  }
  
  /* Return the difference between now and the above future expiry */
  tt_ret = (future_expiry - ticks_now);
  
  if(utimers.defer_cnt>1)
  {
    UTIMER_FREE();
    return tt_ret;
  }  
  
  /* Set the timer interrupt to occur at the proper point in the future*/
  utimer_set_next_interrupt( utimer_max_expiry, ticks_now, FALSE, UTIMER_MVS_DEFER_MATCH_INT );

  UTIMER_FREE();

  /* Return the difference between now and the above future expiry */
  return tt_ret;
} /* utimer_drv_defer_match_interrupt_64 */


/*=============================================================================

FUNCTION UTIMER_DRV_UNDEFER_MATCH_INTERRUPT_64

DESCRIPTION
  Set the timer ISR to fire at the earliest expiring timer in the active
  list.  Meant to serve as a the complimentary function to
  timer_drv_defer_match_interrupt()

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  None

SIDE EFFECTS
  Adjusts the match value.

=============================================================================*/
void utimer_drv_undefer_match_interrupt_64( void )
{
  /* Time of first expiring timer */
  utimer_timetick_type                   first;

  uint32                                 i;
  
  utimer_struct_ptr_type                 ptimer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  UTIMER_LOCK();

  utimers.defer_cnt--;
  
  if(utimers.defer_cnt < 0)
  {
     /* Enable after testing that there are no issues in CRM */
     //ERR_FATAL("utimers.defer_cnt should never become -ve. Became %d ", utimers.defer_cnt, 0, 0);
  }
  
  if(utimers.defer_cnt)
  {
     UTIMER_FREE();
     return;
  }
  
  ticks_now = utimer_timetick_get();

  /* Assume the worst - no timers on list matching our criteria set to expire */
  for(first = utimer_max_expiry, i=0, ptimer = &utimer_buffer[0]; i<UTIMER_BUFFER_SIZE; i++, ptimer++)
     if(ptimer->info.timer_state == UTIMER_SET_FLAG && ptimer->expiry < first)
     {
       first = ptimer->expiry;
     }

  /* If the first expiring timer is in the past ... */
  if ( first  < ticks_now  )
  {
    /* Set the timer for "as soon as possible" (eg, "now") */
    first = ticks_now;
  }
   
  /* Set the next interrupt match value. The function is called after power 
     collapse and the hardware might not contain right values . Hence force 
     MATCH_VAL register to be updated. */
  utimer_set_next_interrupt( first, ticks_now, TRUE, UTIMER_MVS_UNDEFER_MATCH_INT );

  UTIMER_FREE();
  return;
} /* timer_drv_undefer_match_interrupt_64 */
#endif /* #ifdef FEATURE_UTIMER_DEFER_SUPPORT */


/*=============================================================================

                             INIT FUNCTIONS

=============================================================================*/

/*=============================================================================

FUNCTION UTIMER_TASK

DESCRIPTION
  Initializes the UTimer Task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void utimer_task
(
  /* Unused parameter - required to match rex_task_func_type prototype */
  void *                           ignored    /*lint -esym(715,ignored) */
)
{
  /* Signals returned from rex wait procedure */
  utimer_osal_notify_data         sigs;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifndef FEATURE_UTIMER_USE_QURT_SYSCLOCK
  if(DAL_SUCCESS != DalInterruptController_RegisterISR(timers.dal_int_handle,
                                           TIMER_INTERRUPT_ID,
                                           (DALISR)timer_task_isr,
                                           NULL,
                                           DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER))
  {
      ERR_FATAL("ISR registration failed!", 0, 0, 0);
      return; 
  }
  #endif /*FEATURE_UTIMER_USE_QURT_SYSCLOCK*/

 /* Signal the task in order to prime the timer processing mechanism */
 (void) qurt_anysignal_set(&utimer_q6_signal, UTIMER_BLAST_SIG_MASK);


 /* Since not using RCINIT to start utimer_task */
//#if defined(FEATURE_RCINIT)
//  rcinit_handshake_startup();
//#endif

  //time_timetick_init();
  for(;;)
  {

    sigs = qurt_anysignal_wait (&utimer_q6_signal, UTIMER_BLAST_SIG_MASK);
    if (sigs == UTIMER_BLAST_SIG_MASK) 
    {
      /* Clear the timer expiry signal */
      qurt_anysignal_clear (&utimer_q6_signal, UTIMER_BLAST_SIG_MASK);

      /* Process expiring timer(s) */
      utimer_process_active_timers();
    }
  }
} /* utimer_task */


