#ifndef UTIMER_V_H
#define UTIMER_V_H

/*=============================================================================

 U T I M E R  S E R V I C E  I N T E R N A L  H E A D E R

GENERAL DESCRIPTION
 Provides internal structures and functions used by utimers. 

Copyright (c) 2009 - 2017 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/services/utimers/src/utimer_v.h#2 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/074/13  ab      File created.

=============================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "utimer.h"

#ifdef FEATURE_UTIMER_QDSP6

/*Timer signal registered with BLAST. BLAST will set this signal to
 UTIMER_BLAST_SIG_MASK when the match value == timetick value*/
#define UTIMER_BLAST_SIG_MASK 0x2

#endif /* FEATURE_UTIMER_QDSP6 */

#define UTIMER_TASK_PRI 18
#define UTIMER_TASK_STACK_SIZE 1792

#define UTIMER_BUFFER_SIZE 16

#define XO_FREQ_VALUE                   19200000
#define UTIMER_FROM_REMOTE_PROCESS      3
#define UTIMER_DEFERRABLE_FLAG          5
#define UTIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT 1

/* This range is a result of the following calculation...
In general we can say that the oscillator has a +/- 100% swing in freq
Therefore lower bound on Latency(Ticks/ms) = 2*f/1000 
If 5ms is max then TIMER_SCLK_LATENCY = (2*f/1000)*5 = f/100 
*/
#define UTIMER_SCLK_LATENCY             XO_FREQ_VALUE/100

/*=============================================================================

                           ENUMERATIONS

=============================================================================*/

/** @brief Timer state structure type. */
typedef enum utimer_state_struct
{
  UTIMER_DEFAULT_FLAG = 0xAB,
  UTIMER_DEF_FLAG,
  UTIMER_SET_FLAG,
  UTIMER_EXPIRED_FLAG,
  UTIMER_CLEARED_FLAG,
  UTIMER_PAUSED_FLAG,
  UTIMER_RESUMED_FLAG,
  UTIMER_UNDEF_FLAG,
} utimer_state_struct_type;

/*This enum will be used to specify the event
 responsible for setting the match value*/
typedef enum
{
  UTIMER_MVS_TIMER_PROCESSED = 0x1,
  UTIMER_MVS_TIMER_SET,
  UTIMER_MVS_DEFER_MATCH_INT,
  UTIMER_MVS_UNDEFER_MATCH_INT,
  UTIMER_MVS_GROUP_DISABLED,
  UTIMER_MVS_GROUP_ENABLED,
  UTIMER_MVS_TIMER_CLEARED,
  UTIMER_MVS_TIMER_PAUSED,
  UTIMER_MVS_TIMER_RESUMED,
  UTIMER_MVS_TIMER_UNDEFINED,
  UTIMER_MVS_TIMER_CLEAR_TASK_TIMERS,
  UTIMER_MVS_DELETE_PROCESS_TIMERS,
  UTIMER_MVS_SET_WAKEUP,
}utimer_match_interrupt_setter_type;


/*=============================================================================

                           STRUCTURES

=============================================================================*/

/* Remote Process Callback Handler type */
typedef void (*utimer_process_cb_handler)
(
  /* Index of the Process */
  uint32                           process_idx, 
  /* Timer pointer */
  utimer_ptr_type                  timer,
  /* Callback type */
  utimer_notify_type               cb_type,
  
  utimer_osal_notify_obj_ptr       sigs_func_addr,
  
  utimer_osal_notify_data          sigs_mask_data,
  
  utimer_timetick_type             run_time_ms
);

/* Try to keep less than 32 bits in total */
/* utimers should have: State, notify_type, deferrable_type, process_index, units and more */
typedef struct utimer_properties_struct
{
  utimer_state_struct_type   timer_state:8;     /* 8 bits for timer_state: 0-255 */
//  unsigned int               index:4;         /* 4 bits for timers: Only 16 timers are possible */
  unsigned int               process_idx:4;     /* 4 bits for process index: 0-15 */
  unsigned int               remote_process:2;  /* 2 bits: Indicates whether timer is from remote process */
  utimer_notify_type         callback_type:3;   /* 3 bits*/ /* values 0-7 */
  utimer_unit_type           unit:3;            /* 3 bits */ /* values 0-7 */
  unsigned int               deferrable:3;      /* 3 bits */ /* values 0-7 */
  utimer_error_type          last_error:5;      /* 5 bits. Can reduce to 4 bits if required */
}utimer_properties_type;


/**  UTimer Structure
     Contains Callback details, process, expiry details etc
     Values in this structure are for private use by utimer.c only.*/
typedef struct utimer_struct
{
  utimer_properties_type          info;
  
  /**< Address of client timer */
  utimer_ptr_type                 utimer_client_ptr;

   union
   {
      /**< Signals Object */
      void                            *sigs_obj;
      /**< Asynchronous Procedure Call (APC) function to call when the timer 
         expires. */
      utimer_t1_cb_type                func1; 
      /**< Clock callback style function to call when the timer expires. */
      utimer_t2_cb_type                func2; 
  }cbfn;

  union
  {
     /**< Signal(s) to set in the task when the timer expires. */
     utimer_osal_notify_data           sigs;  
     /**< Data for the APC function when the timer expires. */
     utimer_cb_data_type               data;
  }cbdt;
  
  /** Clock tick count timer expires at (list != NULL),
     or remaining ticks until expiry if timer is paused (list == NULL) */
  utimer_timetick_type            expiry;

  /** Reload offset when timer expires (0 = no reload at expiry) */
  utimer_timetick_type            reload;

  /** Slow clock tick count value when timer was set (started) */
  utimer_timetick_type            start;
  
}utimer_struct_type, *utimer_struct_ptr_type;

/* Notification functions for indicating non-defer timer expiry changes */
typedef struct
{
  utimer_notify_type          notify_type;
  utimer_osal_notify_obj_ptr  notify_obj;
  utimer_osal_notify_data     notify_mask;
  uint32                      process_idx;
} utimer_non_defer_notification_client_type;

/*-----------------------------------------------------------------------------
  Private timer data ...
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Slow clock timestamp of last write to SLEEP_XTAL_TIMETICK_MATCH */
  utimer_timetick_type                   set_time;
  
  /* Slow clock timestamp of last attempt to write SLEEP_XTAL_TIMETICK_MATCH */
  utimer_timetick_type                   last_set_time;
  
  /* Last value written to SLEEP_XTAL_TIMETICK_MATCH */
  utimer_timetick_type                   match_value;

  /* Slow clock timestamp when timer_isr is called */
  utimer_timetick_type                   isr_time;

  /* Flag to indicate if timers_process_active_timers() is executing */
  boolean                                processing;

  /* Count of number of times defer function is called */
  int                                    defer_cnt;

  /* Active timers count */
  uint32                                 active_timers_count;
  
  /* Slow clock timestamp of first non_deferrable timer expiry */
  utimer_timetick_type                   first_non_def_timer_expiry;
  
  /* Pause timers count */
  //uint32                            paused_timers_count;

  /* DAL Device Handle */
  //DalDeviceHandle*                  dal_int_handle;
} utimers_type;


/*=============================================================================

                           INTERNAL APIS

=============================================================================*/

void utimer_update_timer_interrupt
(
  uint8                   caller,

  utimer_timetick_type    now
);

utimer_error_type utimer_drv_def_osal
(
  /* Timer to set */
  utimer_ptr_type                  timer,
  
  /* Timer's Actual Address in its own Process Domain, for Logging purposes only */
  utimer_ptr_type                  timer_actual_addr,

  /*signal type*/
  utimer_notify_type               cb_type,

  /* Task to signal and/or task to queue APC call for */
  utimer_osal_notify_obj_ptr       sigs_func_addr,

  /* Task signals to set to the tcb task when timer expires */
  utimer_osal_notify_data          sigs_mask_data,

  /* Process Index */
  unsigned int                     process_idx
);

utimer_error_type utimer_drv_set_deferrable( utimer_ptr_type timer, 
                                             utimer_ptr_type timer_actual_addr, /* For debug purposes only */
                                             boolean condition);

utimer_error_type utimer_drv_set_absolute
(
  /* Timer to set */
  utimer_ptr_type                  timer,

  /* Timer's Actual Address in its own Process Domain, for Logging purposes only */
  utimer_ptr_type                  timer_actual_addr,
  
  /* Time (in units below) until first timer expiry */
  utimer_timetick_type             time,

  /* Period (in units) between repeated expiries (0 = not periodic) */
  utimer_timetick_type             reload,

  /* Unit to measure "ticks" in. */
  utimer_unit_type                  unit
);

utimer_timetick_type utimer_drv_get_64
(
  /* Timer to get */
  utimer_ptr_type                 timer,
  
  /* Timer's Actual Address in its own Process Domain, for Logging purposes only */
  utimer_ptr_type                  timer_actual_addr,

  /* Unit to measure "ticks before timer expires" in. */
  utimer_unit_type                 unit
);

utimer_error_type utimer_drv_clr_64
(
  /* Timer to stop */
  utimer_ptr_type                 timer,  
  
  /* Timer's Actual Address in its own Process Domain, for Logging purposes only */
  utimer_ptr_type                 timer_actual_addr,

  /* Unit to measure "ticks before expiry" in.  Use T_NONE if not required */
  utimer_unit_type                 unit,
  
  utimer_timetick_type           *rem_time
);

utimer_error_type utimer_drv_undef
(
  /* Timer to stop */
  utimer_ptr_type                 timer,
    
  /* Timer's Actual Address in its own Process Domain, for Logging purposes only */
  utimer_ptr_type                 timer_actual_addr
);

utimer_timetick_type utimer_drv_get_first_non_deferrable_timer_expiry( utimer_non_defer_data_ptr utimer_data_ptr );

utimer_timetick_type utimer_drv_defer_match_interrupt_64( void );

void utimer_drv_undefer_match_interrupt_64( void );

/* Internal Function */
utimer_error_type utimer_drv_delete_process_timers(uint32 process_idx);

utimer_error_type utimer_drv_register_for_non_defer_expiry_change
(
  utimer_notify_type         notify_type,
  utimer_osal_notify_obj_ptr notify_obj,
  utimer_osal_notify_data    notify_mask,
  uint32                     process_idx
);

/*=============================================================================
FUNCTION UTIMER_SET_REMOTE_PROCESS_CB_HANDLER

Sets the API that needs to be called when a remote process timer expires.
We will provide to this callback function, the timer callback details
=============================================================================*/
void utimer_drv_set_remote_process_cb_handler
(
   utimer_process_cb_handler shim_cb_func
);

/*=============================================================================
FUNCTION UTIMER_DRV_SET_LOCAL_PROCESS_IDX

Local Process Index needs to be set by timer shim layer so that remote timers
can be differentiated from local process timers
=============================================================================*/
void utimer_drv_set_local_process_idx
(
   uint32 local_process_idx
);

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
);

#endif /* UTIMER_V_H */