//#ifndef REX_H
/* Circular #include: "rex.h" includes "timer.h", which includes "rex.h".
   Ensure first half of "rex.h" is included before "timer.h" processed. */
//#include "rex.h"
//#endif /* REX_H */

#ifndef TIMER_H
#define TIMER_H

/**
@file timer.h
@brief
Implements various Timer APIs for various clients usage.

Timer groups are a way of enabling and disabling groups of timers at the same time. This is
 mainly used by the sleep subsystem to disable unwanted timers during sleep. For this reason, one
 should be careful which group their timer is set in, to help conserve power. 
 
When timer's are active, they live on the active list. When deactivated, they are moved to their appropriate timer group.
When a timer group is re-enabled, they are spooled back onto the active list. Timers that were pending expire once,
 while recurring ones get bumped to the future.

INITIALIZATION AND SEQUENCING REQUIREMENTS

timer_init() must be called once before any other timer functions.

timer_def() must be called, for each timer, 
before other timer functions are used with that timer.

timer_undef() must be called to free up the timer memory if the timer is not
required any more

A valid sclk estimate is needed before timers can be set.

*/
/*=============================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The ATS_mainpage.dox file contains all file/group descriptions that
      are in the output PDF generated using Doxygen and Latex. To edit or 
      update any of the file/group text in the PDF, edit the 
      ATS_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the time_services group 
      description in the ATS_mainpage.dox file. 
=============================================================================*/

/*=============================================================================
  Copyright (c) 2003-2017 QUALCOMM Technologies Incorporated.
  All rights reserved.
  Qualcomm Confidential and Proprietary
===========================================================================*/
/*=============================================================================

                  T I M E R   S E R V I C E   S U B S Y S T E M

                     P U B L I C   A P I S   A N D   D A T A

GENERAL DESCRIPTION
  Implements the timer functions


REGIONAL FUNCTIONS

  timer_init()
    Initializes the timer subsytem.  Call only from time_init( ).

EXTERNALIZED FUNCTIONS

  timer_def(timer, group, tcb, sigs, func, data)
    Defines and initializes a timer.

  timer_def2(timer, group, func, data)
    Defines and initializes a timer, with the clock callback semantics.

  timer_set(timer, ticks, reload, unit)
    Sets an inactive timer to expire after a given period of time, or changes
    an active timer to expire after a given period of time.
    Optionally, specifies the timer to repeatly expire with a given period.

  timer_get(timer, unit)
    Get number of timer units before timer expires

  timer_pause(timer)
    Suspends an active timer

  timer_resume(timer)
    Resumes a previously "paused" active timer.

  timer_clr(timer, unit)
    Stops an active timer

  timer_clr_task_timers(tcb)
    Stops all active timers which belong to the specified task.

  timer_group_enable(timer_group)
    Enables a timer group.  Timers in the timer group which expired during the
    diabled period will have their expiry processing executed.

  timer_group_disable(timer_group)
    Disables a timer group.  Timers in the timer group will continue to count
    down, but they will not expire.


INITIALIZATION AND SEQUENCING REQUIREMENTS

    timer_init() must be called once before any other timer functions.

    timer_def() must be called, for each timer, before other timer
    functions are used with that timer.

    A valid sclk estimate is needed before timers can be set.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/services/timer.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/29/10   scd     (Tech Pubs) Edited Doxygen markup and comments.
11/12/10   din     Added function to get time to next occuring non-deferrable 
                   timer expiry.
10/27/10   EBR     Doxygenated File.
12/10/09   din     Added sclk_delta to timer_type. 
11/25/09   din     Added timer_[un]defer_match_interrupt()
05/22/09   din     Removed FEATURE_TIMER_TASK
01/07/09   pbi     Removed timer_set_sclk_offset
02/23/09   din     Consoldation of time.
01/13/09   din     Removed timer_delay_expiry_excluding_groups() and
                   timer_undelay_expiry().
01/12/09   din	   Added deferrable attribute to timer_group. Also added 
                   lightweight API's to enable/ disable timer groups. API's are
                   timer_defer_match_interrupt(), timer_undefer_match_interrupt()
                   & timer_group_set_deferrable().
12/23/08   tev     Restore timer_null_group to public timer.h.  Eliminate
                   need for timer_v.h.
12/15/08   tev     Removed timer_null_group to private timer_v.h header file.
11/14/08   tev     Amended control for timer_restart to
                   defined(FEATURE_TIME_POWER_COLLAPSE) &&
                   !defined(FEATURE_TIME_REMOVE_TIMER_RESTART)
11/10/08   tev     Tentatively restored timer_restart under control of
                   FEATURE_TIME_POWER_COLLAPSE symbol definition.
10/16/08    din    Added lightweight mechanism to enable and disable timer groups 
                   during sleep. For use by the sleep subsystem only! APIs are
                   timer_delay_expiry_excluding_groups(), and timer_undelay_expiry()
                   (renamed timer_restart().)  
11/13/07   jhs     Removed timer_always_on_group.
08/31/07   jhs     Support for AMSS RESTART
12/04/06   trc     extern "C" {} our API to avoid name mangling for C++ clients
08/19/06   gb      Changed FEATURE_POWER_COLLAPSE to FEATURE_TIME_POWER_COLLAPSE
02/22/06   ajn     SCLK offset now applies for both modem & apps processors.
02/10/06   kap/ajn Added Timer Optimization Changes (double linking/binning)
11/01/05   ajn     Added cache initialization to TIMER_DEF/TIMER_DEF2 macros.
10/10/05   ajn     Added TIMER_EXPIRES_AT() macro
07/06/05   ajn     Added TIMER_IS_ACTIVE() macro
07/06/05   ajn     Added timer_set_modem_app_sclk_offset (7500 APPS)
05/06/05   ajn     Cache time/reload values to avoid recalculating if unchanged
03/29/05   ajn     Replaced timer_ptr with timer_ptr_type, for L4 project.
10/20/04   ajn     Added a timer task, for timer expiry
09/28/04   ajn     Added "timer_null_group" for timers declared in NULL group
08/30/04   ajn     Added "get next interrupt" and "interrupt early" functions
                   for TCXO shutdown.
08/11/03   ajn     Added timer_pause/timer_resume for rex_set_timer replacement
08/08/03   ajn     Updated for coding standards
08/06/03   ajn     Added timer_def2(), for clk_reg() replacement
07/23/03   ajn     Moved _{to,from}_sclk functions to timetick.h
07/17/03   ajn     File created.

=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "time_timetick.h"
#include "time_osal_qurt.h"

#ifdef __cplusplus
/* Avoid name mangling for any C++ customers */
extern "C" {
#endif

/** @addtogroup time_services
@{ */

/*==============================================================================

                           DATA DECLARATIONS

=============================================================================*/

/* Error Returns Enums of Timer APIs */
typedef enum
{
  TE_SUCCESS = 0,
  TE_FAIL,
  /* Need to make "timer_client" as dependency if some module is calling
     into timer creation apis before timer module gets initialized */
  TE_TIMER_MODULE_NOT_INITIALIZED, /* Error returned if timer creation apis are called before timer system is initialized */
  TE_HANDLE_IN_USE,              /* Error returned if timer is being defined upon a valid timer */
  TE_INVALID_TIMER_HANDLE,       /* Error returned if timer is invalid */
  TE_INVALID_GROUP_HANDLE,       /* Error returned if group is invalid */
  TE_INVALID_PARAMETERS,         /* Error returned if input parameters for an api are invalid */
  TE_INVALID_DURATION,           /* Error returned if timer is invalid duration */
  TE_INVALID_STATE_FOR_OP,       /* Error returned if timer is in invalid state for requested operation */
  TE_MALLOC_FAILED,              /* Error returned if free client timers are over */
  TE_NO_FREE_GROUP,              /* Error returned if free client timers are over */
  TE_NO_FREE_CLIENT_TIMER,       /* Error returned if free client timers are over */
  TE_NO_FREE_INTERNAL_TIMER,     /* Error returned if free internal timers are over */
  TE_TIMER_NOT_ACTIVE,           /* Error returned if timer is not active but an operation expects it */
  TE_TIMER_ALREADY_IN_SAME_STATE,/* Error returned if timer is already in state where an operation is being tried */
  TE_MAX = 0xFFFFFFFF
}
timer_error_type;


/* Notification Callback type */
typedef enum
{
  TIMER_NO_NOTIFY_TYPE,          /* Use this if, no notification is required */
  TIMER_NATIVE_OS_SIGNAL_TYPE,   /* In Qurt, required for qurt_anysignal_t object */
  TIMER_NATIVE_OS_SIGNAL2_TYPE,  /* In Qurt, required for qurt_signal2_t object */
  TIMER_REX_TCB_SIGNAL_TYPE,     /* For Rex only, rex_tcb_type is required */
  TIMER_FUNC1_CB_TYPE,           /* timer_t1_cb_type type of function pointer is expected */
  TIMER_FUNC2_CB_TYPE,           /* timer_t2_cb_type type of function pointer is expected */
  TIMER_DALSYS_EVENT_SIGNAL_TYPE, /* For Dalsys Event objects, DALSYSEventHandle* obj addr is required */
  TIMER_INVALID_NOTIFY_TYPE
}
timer_notify_type;

/*Strcture to have data of first non-deferrable timer*/
typedef struct {
     timer_notify_type              cb_type;
     time_osal_notify_obj_ptr       sigs_func_addr;
     time_osal_notify_data          sigs_mask_data;
} timer_non_defer_data;
 
typedef timer_non_defer_data* timer_non_defer_data_ptr;

/* Timer Data Type. This contains the handle which represents internal timer object */
typedef unsigned int             timer_type;

/** Timer Pointer Data Type */
typedef timer_type *             timer_ptr_type;

/* Timer Group handle */
/* Note: Groups variables are to be created as Global variables. 
         Timer groups is a way of enabling and disabling groups of timers at the same time. 
         It is advised to create 1 or 2 groups at max per task, only if required. 
         If only a general timer is being created and if it doesn't require to be
         enabled/disabled as a group, can use "NULL" as group input which will put
         such timers into default timer_null_group         */
typedef unsigned int             timer_group_type;

/** @brief Group structure pointer typedef.
*/
typedef timer_group_type *       timer_group_ptr;

/* The NULL timer group, for timers not in any other group. "NULL" as input for group ptr should be provided */
extern timer_group_type          timer_null_group;

/* Global Non-deferrable group for various clients use */
/* Note: This group shouldn't be called with disable/enable group functions as
   this will be used by other clients too. */
extern timer_group_type          timer_non_defer_group;

/** Timer 'type 1' callback arbitrary data type.
*/
typedef unsigned long            timer_cb_data_type;

/** Timer 'type 1' callback function.
*/
typedef void (*timer_t1_cb_type)
(
  timer_cb_data_type              data  
    /**< Arbitrary data given to timer_def( ) to pass to this callback 
         function. */
);


/** Timer 'type 2' callback function (for clock callback style interface)
*/
typedef void (*timer_t2_cb_type)
(
  int32                           time_ms, 
    /**< Time elapsed from timer setting (start). */
  timer_cb_data_type              data     
    /**< Arbitrary data given to timer_def2( ) to pass to this callback 
         function. */
);

#ifndef FEATURE_L4
/* The L4 project has issues where timer_ptr is used other than as a typedef.
   For other projects, temporarily define timer_ptr a migration aid. */
//#define timer_ptr                 timer_ptr_type
#endif /* !FEATURE_L4 */

/* Dalsys Macros */
#define timer_dalsys_sig_init(h, obj) \
{ \
  if(DAL_SUCCESS != DALSYS_EventCreate(DALSYS_EVENT_ATTR_CLIENT_DEFAULT, \
                           h, \
                           obj)) \
      ERR_FATAL("timer dalsys sig obj failed", 0, 0, 0); \
\
} \

#define timer_dalsys_sig_set(h) \
   DALSYS_EventCtrl((DALSYSEventHandle)(*(DALSYSEventHandle *)h), DALSYS_EVENT_CTRL_TRIGGER)

#define timer_dalsys_sig_wait(h) \
   DALSYS_EventWait((DALSYSEventHandle)(*(DALSYSEventHandle *)h))

#define timer_dalsys_sig_reset(h) \
   DALSYS_EventCtrl((DALSYSEventHandle)(*(DALSYSEventHandle *)h), DALSYS_EVENT_CTRL_RESET)

/*==============================================================================

                        REGIONAL FUNCTION DECLARATIONS

=============================================================================*/

/** 
Initializes the Timer subsytem.

@return
None.
 
@dependencies  
None.
 
@sideeffects 
This function may install an ISR for the timer hw match register.
*/
void timer_init( void );


/** 
Initializes the Timer task.

@param[in] ignored Unused parameter required to match rex_task_func_type 
                   prototype.

@return
None.

@dependencies  
None.
*/
/*void timer_task
(
  dword                           ignored
);*/



/*==============================================================================

                            FUNCTION DECLARATIONS

=============================================================================*/

/**
Enables a timer group. Timers in the timer group that expired during the
diabled period have their expiry processing executed after this call.

@param[in] timer_group pointer to timer group to be enabled.

@return
timer_error_type

@dependencies  
None.

@sideeffects 
This function may generate signals to a task, which can cause task switches. \n
This function may queue an asynchronous procedure call.
*/
timer_error_type timer_group_enable
(
  timer_group_ptr                 timer_group
);


/**
Disables a timer group. Timers in the timer group continue to count
down, but they do not expire.

@param[in] timer_group Timer group to be disabled.

@return
timer_error_type
 
@dependencies
None.
*/
timer_error_type timer_group_disable
(
  timer_group_ptr                 timer_group
);


/**
Sets the timer group deferrable status. 

@note1hang Timer groups default to being non-deferrable.

@param[in] pgroup    Timer group to be undeferred.
@param[in] condition Boolean value: \n
                     TRUE -- Deferrable. \n
                     FALSE -- Otherwise.
@return
timer_error_type
 
@dependencies  
None.
*/
timer_error_type timer_group_set_deferrable
( 
  timer_group_ptr pgroup, 
  boolean condition 
);


/**
Gets the first non-deferrable timer's expiry value.


@return
Absolute expiry of first occuring non-deferrable timer.
Returns TIMER_MAX_EXPIRY in case there is no non-deferrable timer present.

@dependencies  
None. 
*/


time_timetick_type timer_get_first_non_deferrable_timer_expiry
(
 /*Structure to filled with first non-deferrable timer data*/
 timer_non_defer_data_ptr timer_data_ptr
);

/**
Registers for first non-deferrable timer expiry change notification.
Only signal type notifications are allowed, for others, error will be 
returned.

@param[in] notify_type       notification type from timer_notify_type. Only signal notify type
@param[in] notify_obj        notify object address that should be set on expiry change. Only signal obj addr.
@param[in] notify_mask       mask to be set to the signal obj addr provided

@dependencies Input objects should be initialized by client

@return
TE_INVALID_PARAMETERS   If passed sig obj addr is NULL or invalid notify type
TE_SUCCESS :            If the operation completed successfully
 
@dependencies  
None.
*/
timer_error_type timer_register_for_non_defer_expiry_change
(
  timer_notify_type        notify_type,
  time_osal_notify_obj_ptr notify_sig_obj,
  time_osal_notify_data    notify_sig_mask
);

/**
Gets time to the next occurring non-deferrable timer expiry value.

@return
Number of ticks in 19MHz domain until the next timer expiry in a
 timer group which is non-deferrable.

@dependencies  
Must be called with interrupts locked. 
*/


time_timetick_type timer_get_time_till_first_non_deferrable_timer_64( void ) ;

/**  
Sets timer match interrupt to the next occurring non-deferrable timer expiry
value. Complementary function is timer_undefer_match_interrupt(). 

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
time_timetick_type timer_defer_match_interrupt_64( void ) ;

/** 
Sets the timer ISR to fire at the earliest expiring timer in the active list.
This function is meant to serve as the complementary function to 
timer_defer_match_interrupt().

@dependencies  
This function must be called with interrupts locked.

@sideeffects 
This function adjusts the match value.
*/
void timer_undefer_match_interrupt( void );

 
/**
Defines and initializes a timer. Signals are to be native kernel signal type objects
Signal and function notifications are exclusive. If both are provided at the same time,
error will be returned.
                                  
The timer expires when:\n
- sigs is non-NULL. The signals are set to sigs_obj.
- func is non-NULL. The callback function with the argument data
  is called from the timer tasks.

@note1hang
- A defined timer shouldn't be re-defined. TE_HANDLE_IN_USE will be returned in such case
- Signal or function notifications are "Exclusive".
- Client should call timer_undef() once a timer is not required

@param[in] timer           Timer to set.
@param[in] group           Group timer belongs to (NULL=the "NULL" timer group). 
@param[in] cb_type         Refer to comments on timer_notify_type
@param[in] sigs_func_ptr   signal or function ptr depending on cb_type
@param[in] sigs_mask_data  signal mask for signal obj or data for callback
                           function that needs to be returned
@return
  timer_error_type: Possible return values are
  TE_SUCCESS
  TE_FAIL
  TE_NO_FREE_GROUP
  TE_NO_FREE_CLIENT_TIMER
  TE_HANDLE_IN_USE
  TE_INVALID_PARAMETERS
  TE_MALLOC_FAILED

@dependencies  
If reusing a previously defined timer, call timer_undef() before calling timer_def.
*/
timer_error_type timer_def_osal
(
  timer_ptr_type                  timer,
  timer_group_ptr                 group,
  timer_notify_type               cb_type,
  time_osal_notify_obj_ptr        sigs_func_ptr,
  time_osal_notify_data           sigs_mask_data
);


/**
Defines a clock callback-style timer. When the timer expires, the callback 
function with the arguments time_ms and data is called from the interrupt context. 


@note1hang
- A defined timer shouldn't be defined again without deleting thru timer_undef()
- Client should call timer_undef() once a timer is not required

@param[in] timer Timer to set.
@param[in] group Group timer belongs to (NULL=the NULL timer group).
  
@return
  timer_error_type: Possible return values are
  TE_SUCCESS
  TE_FAIL
  TE_NO_FREE_GROUP
  TE_NO_FREE_CLIENT_TIMER
  TE_HANDLE_IN_USE
  TE_INVALID_PARAMETERS
  TE_MALLOC_FAILED

@dependencies  
The timer must be initialized with timer_reg().
If reusing a previously defined timer, call timer_undef() before calling timer_def2.
*/
timer_error_type timer_def2
(
  timer_ptr_type                  timer,
  timer_group_ptr                 group
);


/**
Initializes a clock callback style timer.

@param[in] timer     Timer to set.
@param[in] func      Function to call at timer expiry.
@param[in] data      Arbitrary data for the callback function with the 
                     arguments time_ms and data.
@param[in] time_ms   Time in milliseconds until first timer expiry.
@param[in] reload_ms Period in milliseconds between repeated expiries (0=not periodic).
  
@return
timer_error_type: Possible return values are
TE_INVALID_PARAMETERS
TE_INVALID_TIMER_HANDLE
TE_NO_FREE_INTERNAL_TIMER
TE_SUCCESS

@dependencies
The timer must be initialized with timer_def2().
*/
timer_error_type timer_reg
(
  timer_ptr_type                  timer,
  timer_t2_cb_type                func,
  timer_cb_data_type              data,
  timetick_type                   time_ms,
  timetick_type                   reload_ms
);


/**
Sets an inactive timer to expire after a given period of time, or changes an
active timer to expire after a given period of time. Optionally, specifies
the timer to repeatly expire with a given period.  If the timer 
is to be set in units of ticks, it will be done in the 19MHz 
domain. 

@param[in] timer  Timer to set.
@param[in] time   Time in units specified until first timer expiry.
@param[in] reload Period in units specified between repeated 
                  expiries (0=not periodic).
@param[in] unit   Please refer comments of timer_unit_type

@return
timer_error_type. Possible return values are
TE_INVALID_PARAMETERS
TE_INVALID_TIMER_HANDLE
TE_NO_FREE_INTERNAL_TIMER
TE_SUCCESS
  
@dependencies  
None.

@sideeffects 
This function may generate signals to a task, which can cause task switches. \n
This function may queue an APC.
*/
timer_error_type timer_set_64
(
  timer_ptr_type                  timer,
  time_timetick_type              time,
  time_timetick_type              reload,
  timer_unit_type                 unit
);


/**
Sets timer to expire at absolute ticks (instead of relative expiry) 

@note
The ticks are assumed to be target specific. Targets with sclk, will be
assumed as ticks of sclk ticks while on targets with XO clk, ticks are 
assumed to be XO clk ticks

@return* 
timer_error_type. Possible return values are:
TE_INVALID_PARAMETERS
TE_INVALID_TIMER_HANDLE
TE_NO_FREE_INTERNAL_TIMER
TE_SUCCESS

@dependencies  
None.

@sideeffects 
This function adjusts the match value.
*/
timer_error_type timer_set_absolute
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Time until first timer expiry */
  time_timetick_type              time

);


/**
Gets the number of units before the timer expires. If the unit 
is ticks, it will return value in 19MHz domain. 

- Zero is returned for inactive timers and timers that have expired or are
expiring. 
- Fractional values that might truncate to zero, depending on the unit
passed as a parameter, are rounded to 1.

@param[in] timer Timer to get.
@param[in] unit  Unit to use to measure the ticks before the timer expires.

@return
Number of units before timer expires. \n

@dependencies  
None.
*/
time_timetick_type timer_get_64
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit
);


/**
Queries whether a timer is active. A timer that has been set to expire at a 
certain time is considered to be active even if the group it is a member of is 
disabled. A timer that is explicitly paused is not active until it has been 
resumed.

@param[in] timer Timer to be queried.

@return
TRUE  -- Timer is active. \n
FALSE -- Timer is not active.

@dependencies  
None.
*/
boolean timer_is_active
(
  timer_ptr_type  timer
);
#define TIMER_IS_ACTIVE(timer)  ( timer_is_active(timer) )


/**
Returns the XO counter value when the timer expires.

@param[in] timer Timer to be queried.

@return
XO counter value corresponding to the timer expiry point.
  
@dependencies  
The timer must be running for the returned value to be meaningful.

@sa
DalTimetick_GetTimetick64()
*/
time_timetick_type timer_expires_at_64
(
  timer_ptr_type  timer
);

#define TIMER_EXPIRES_AT_64(timer)  ( timer_expires_at_64(timer) )

/**
Returns the time tick count in 19MHz when the timer started.

@param[in] timer Timer to be queried.

@return
Clock counter value in the 19MHz domain corresponding to 
the timer start point. 
  
@dependencies  
The timer must be running for the returned value to be meaningful.
*/
time_timetick_type timer_get_start_64
(
  timer_ptr_type  timer
);

/**
Stops an active timer. Returns amount of time remaining before expiry
in terms of specified unit.

@param[in] timer Timer to be stopped.
@param[in] unit  Unit to use to measure the ticks before expiry.
                 if unit is tick, returns in 19MHz domain
                   
@return
Amount of time in specified unit remaining before expiry.
0 will be returned for an expired timer or invalid timer. 

@note1hang
- If the timer_clr api is called at the moment of timer being expired
there can be chances that timer gets expired first and then timer_clr
will simply return 0 as timer got expired but callback/signal calling can be
running on parallel thread. Therefore, it is always advised to check if there
is sufficient time(ex: 1msec or few ticks) before calling this api.
timer_get_64() can be used to check how much time is still left for the expiry.

@dependencies  
None.
*/
time_timetick_type timer_clr_64
(
  timer_ptr_type                  timer,
  timer_unit_type                 unit
);

/**
Same as timer_clr which stops an active timer and also with an error return.

@param[in] timer Timer to be stopped.
@param[in] unit  Unit to use to measure the ticks before expiry.
@param[in] rem_time  Amount of time remaining before expiry. 
                 Time will be in specified unit. This can be NULL.
                 If the unit is TU_TICK, ticks will be from XO clk
                   
@return
Status of the operation. Possible return values are:
TE_INVALID_PARAMETERS
TE_INVALID_TIMER_HANDLE
TE_TIMER_ALREADY_IN_SAME_STATE
TE_SUCCESS
  
@dependencies  
None.

@Note
If the timer_stop api is called at the moment of timer being expired
there can be chances that timer gets expired first and then timer_stop
will simply return 0 as timer got expired. Therefore, it is always
advised to check if there is sufficient time(ex: 1msec or few ticks)
before calling this api. timer_get() can be used to check how much
time is still left for the expiry.
*/
timer_error_type timer_stop
(
  timer_ptr_type                 timer,
  timer_unit_type                unit,
  time_timetick_type             *rem_time
);

/**
Removes timer from active list and deallocates all the timer memory
for the given timer handle.

@param[in] timer Timer to be stopped.

@return
  TE_SUCCESS if success
  TE_TIMER_ALREADY_IN_SAME_STATE if timer handle is invalid or undefined previously

@dependencies
  None
*/

timer_error_type timer_undef
(
  /* Timer to stop */
  timer_ptr_type                  timer
);

/**
Pauses an active timer.

@param[in] timer Timer to be paused.
 
@return
timer_error_type. Possible return values can be
TE_INVALID_PARAMETERS
TE_INVALID_TIMER_HANDLE
TE_TIMER_NOT_ACTIVE
TE_TIMER_ALREADY_IN_SAME_STATE
TE_SUCCESS

@dependencies  
This function is only to be called for a timer that is active.
*/
timer_error_type timer_pause
(
  timer_ptr_type                  timer
);


/**
Resumes a previously paused timer. When a timer is paused 5 seconds before its
expiry and timer_resume( ) is called 30 seconds later, the timer expires
5 seconds after timer_resume( ) was called.

@param[in] timer Timer to be resumed.

@return
timer_error_type. Possible return values are
TE_INVALID_PARAMETERS
TE_INVALID_TIMER_HANDLE
TE_TIMER_NOT_ACTIVE
TE_TIMER_ALREADY_IN_SAME_STATE
TE_SUCCESS
   
@dependencies  
This function must only be called for a timer that has been paused.

@sideeffects 
This function may generate signals to a task, which can cause task switches. \n
This function may queue an APC.
*/
timer_error_type timer_resume
(
  timer_ptr_type                  timer
);



/**
Stops active timers belonging to the given task.

@param[in] Thread ID Timers belonging to this thread are cleared (stopped).

@return
None.

@dependencies  
None.
*/
void timer_clr_osal_thread_timers
(
  time_osal_thread_id                    thread_id
);


/**
Returns the number of slow clocks in 19MHz domain until the 
expiry of the next timer to expire. 

@return
Number of ticks in the 19MHz domain until the next timer 
expiry. 

@dependencies  
This function is for the exclusive use of SLEEP for TCXO shutdown. \n
This function must be called with interrupts locked.
*/
time_timetick_type timer_get_sclk_till_expiry_64( void );

/**
Sets the timer hw to expire in advance of the next expiring 
timer expiry point to allow for TCXO to turn on.

@param[in] wakeup_sclks Wakes up the CPU in the given number of 
      slow clocks in 19MHz domain.

@return
timer_error_type

@dependencies  
This function is for the exclusive use of SLEEP for TXCO shutdown. \n
This function must be called with interrupts locked.
*/
timer_error_type timer_set_wakeup_64
(
  time_timetick_type                   wakeup_sclks
);

/**
Readjusts the timetick counter values to synchronize with the base timetick 
counter when waking up from a power collapse. The match value must be adjusted 
after a power collapse and this function performs that adjustment. This 
function is called after waking up. 

@return
None.

@dependencies  
None.

@sideeffects 
This function adjusts the match value.
*/
void timer_restart(void);

/**
 Suspends the client task for the specified time.
 If non_deferrable option is True, task will surely resumed at specified time
 If non_deferrable option is False, and if Proc goes into sleep at the time of
 specified timeout, task will resume only after Proc wakesup.

@return
None.

@dependencies  
 This function should only be called from the clients task context only.

@sideeffects
None.
*/
void timer_sleep
(
  time_timetick_type              timeout,
  timer_unit_type                 unit,
  boolean                         non_deferrable
);

/** @} */ /* end_addtogroup time_services */

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* TIMER_H */


