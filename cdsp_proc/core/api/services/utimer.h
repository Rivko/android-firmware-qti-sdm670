#ifndef UTIMER_H
#define UTIMER_H

/**
@file timer.h
@brief
Implements various (U)Timer APIs for micro(U)image.

INITIALIZATION AND SEQUENCING REQUIREMENTS

utimer_init() must be called once before any other timer functions.

utimer_def() must be called, for each timer, 
before other timer functions are used with that timer.

utimer_undef() must be called to free up the timer memory if the timer is not
required any more

A valid timetick estimate is needed before timers can be set.

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

                 U T I M E R   S E R V I C E   S U B S Y S T E M

                     P U B L I C   A P I S   A N D   D A T A

GENERAL DESCRIPTION
  Implements the utimer functions


REGIONAL FUNCTIONS

  utimer_init()
    Initializes the utimer subsytem.  Call through rcinit.

EXTERNALIZED FUNCTIONS

  //Todo: Change arguments
  utimer_def_osal(timer, group, tcb, sigs, func, data)
    Defines and initializes a timer.

  utimer_set(timer, set duration, reload duration, unit)
    Sets an inactive timer to expire after a given period of time, or changes
    an active timer to expire after a given period of time.
    Optionally, specifies the timer to repeatly expire with a given period.

  utimer_get(timer, unit)
    Get number of time units before utimer expires

  // Todo: Not available
  //timer_pause(timer)
  //  Suspends an active timer

  //timer_resume(timer)
  //  Resumes a previously "paused" active timer.

  utimer_clr(timer, unit)
    Stops an active timer. Returns 0 if its already inactive
    
  utimer_undef(timer)
    Stops an active timer, relinquishes the handle and frees the timer

  //Todo: Need to remove these apis as they are not supported for utimers
  //timer_clr_task_timers(tcb)
    //Stops all active timers which belong to the specified task.

  //timer_group_enable(timer_group)
    //Enables a timer group.  Timers in the timer group which expired during the
    //diabled period will have their expiry processing executed.

  //timer_group_disable(timer_group)
    //Disables a timer group.  Timers in the timer group will continue to count
    //down, but they will not expire.


INITIALIZATION AND SEQUENCING REQUIREMENTS

    utimer_init() must be called once before any other timer functions.

    utimer_def() must be called, for each timer, before other timer
    functions are used with that timer.

    A valid timetick estimate is needed before timers can be set.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1.c4/api/services/utimer.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/07/13   ab      created this file.

=============================================================================*/
#include "comdef.h"
#include "utimer_osal.h"



/*=============================================================================

                           ENUMERATIONS

=============================================================================*/

/* Todo: Many of errors may not be valid for utimers and can be removed or kept also, can be removed at apis and other places */
/* Error Returns Enums of UTimer APIs */
typedef enum
{
  UTE_SUCCESS = 0,
  UTE_FAIL,
  /* Need to make "utimer_client" as dependency if some module is calling
     into utimer creation apis before utimer module gets initialized */
  UTE_TIMER_MODULE_NOT_INITIALIZED, /* Error returned if timer creation apis are called before utimer system is initialized */
  UTE_HANDLE_IN_USE,              /* Error returned if timer is being defined upon a valid timer */
  UTE_INVALID_TIMER_HANDLE,       /* Error returned if timer is invalid */
  UTE_INVALID_GROUP_HANDLE,       /* Error returned if group is invalid */
  UTE_INVALID_PARAMETERS,         /* Error returned if input parameters for an api are invalid */
  UTE_INVALID_DURATION,           /* Error returned if timer is invalid duration */
  UTE_INVALID_STATE_FOR_OP,       /* Error returned if timer is in invalid state for requested operation */
  UTE_MALLOC_FAILED,              /* Error returned if free client timers are over */
  UTE_NO_FREE_GROUP,              /* Error returned if free client timers are over */
  UTE_NO_FREE_TIMER_STRUCT,       /* Error returned if free client timers are over */
  UTE_NO_FREE_INTERNAL_TIMER,     /* Error returned if free internal timers are over */
  UTE_TIMER_NOT_ACTIVE,           /* Error returned if timer is not active but an operation expects it */
  UTE_TIMER_ALREADY_IN_SAME_STATE,/* Error returned if timer is already in state where an operation is being tried */
}
utimer_error_type;

/* Notification type */
typedef enum
{
  UTIMER_NO_NOTIFY_TYPE,          /* Use this if, no notification is required */
  UTIMER_NATIVE_OS_SIGNAL_TYPE,   /* In Qurt, required for qurt_anysignal_t object */
  //UTIMER_NATIVE_OS_SIGNAL2_TYPE,  /* QURT SIGNAL2 is not supported in Uimage. If Qurt Supports it enable code which has this enum */
  UTIMER_REX_TCB_SIGNAL_TYPE,     /* For Rex only, rex_tcb_type is required */
  UTIMER_FUNC1_CB_TYPE,           /* timer_t1_cb_type type of function pointer is expected */
  UTIMER_FUNC2_CB_TYPE,           /* timer_t2_cb_type type of function pointer is expected */
  UTIMER_INVALID_NOTIFY_TYPE
} utimer_notify_type;

typedef enum 
{
  UT_TICK,     /**< -- Return time in Ticks */
  UT_USEC,     /**< -- Return time in Microseconds */
  UT_MSEC,     /**< -- Return time in Milliseconds */
  UT_SEC,      /**< -- Return time in Seconds */
  UT_MIN,      /**< -- Return time in Minutes */
  UT_HOUR,     /**< -- Return time in Hours */
  UT_NONE=UT_TICK  /**< -- Identifier to use if no paticular return type is needed */ 
} utimer_unit_type;

/* Imp Todo: If utimer_unit_type is not available, will need to define utimer_unit_type */

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/
/*Strcture to have data of first non-deferrable timer*/
typedef struct {
     utimer_notify_type               cb_type;
     utimer_osal_notify_obj_ptr       sigs_func_addr;
     utimer_osal_notify_data          sigs_mask_data;
} utimer_non_defer_data;
 
typedef utimer_non_defer_data* utimer_non_defer_data_ptr; 

/* UTimer Data Type. This is a handle to internal timer structure */
typedef unsigned int              utimer_type;

/** UTimer Pointer Data Type */
typedef utimer_type *             utimer_ptr_type;

/** Timer 'type 1' callback arbitrary data type.
*/
typedef unsigned long             utimer_cb_data_type;

/** Timer 'type 1' callback function. 
The callback function shouldn't take much stack and much time as well. 
Callbacks are called with a worker thread of 2KB stack.
*/
typedef void (*utimer_t1_cb_type)
(
  utimer_cb_data_type              data  
    /**< Arbitrary data given to UTIMER_FUNC1_CB_TYPE type timer to pass back to callback 
         function. */
);


/** Timer 'type 2' callback function (for clock callback style interface)
The callback function shouldn't take much stack and much time as well. 
Callbacks are called with a worker thread of 2KB stack.
*/
typedef void (*utimer_t2_cb_type)
(
  int32                           time_ms, 
    /**< Time elapsed from timer setting (start). */
  utimer_cb_data_type              data     
    /**< Arbitrary data given to UTIMER_FUNC2_CB_TYPE type timer to pass back to callback 
         function. */
);

/*==============================================================================

                        REGIONAL FUNCTION DECLARATIONS

=============================================================================*/

/** 
Initializes the UTimer subsytem.

@return
None.
 
@dependencies  
None.
 
@sideeffects 
This function registers with Qurt for Signal when Interrupt occurs
*/
void utimer_init( void );

/*==============================================================================

                            FUNCTION DECLARATIONS

=============================================================================*/

/**
Defines and initializes a utimer. Signals are to be native kernel signal type objects
Signal and function notifications are exclusive. If both are provided at the same time,
error will be returned.

When the timer expires if:\n
- cb_type is Signal type. The mask is set to sigs_obj.
- cb_type is Func type. The callback function with the argument data
  is called from the utimer tasks.

@note1hang
- A defined timer shouldn't be re-defined. UTE_HANDLE_IN_USE error will be returned in such case
- Signal or function notifications are "Exclusive".
- Client should call utimer_undef() once a timer is not required
- The callback function shouldn't take much stack and much time as well. Callbacks are called 
  with a worker thread of 2KB stack.

@param[in] utimer          Timer to set.
@param[in] cb_type         Refer to comments on utimer_notify_type
@param[in] sigs_func_ptr   signal or function ptr depending on cb_type
@param[in] sigs_mask_data  signal mask for signal obj or data for callback
                           function that needs to be returned
@return
  timer_error_type: Possible return values are
  UTE_SUCCESS
  UTE_FAIL
  UTE_NO_FREE_GROUP
  UTE_NO_FREE_CLIENT_TIMER
  UTE_HANDLE_IN_USE
  UTE_INVALID_PARAMETERS
  UTE_MALLOC_FAILED

@dependencies  
If reusing a previously defined timer, call timer_undef() before calling timer_def.
*/
utimer_error_type utimer_def_osal
(
  utimer_ptr_type                  timer,
  utimer_notify_type               cb_type,
  utimer_osal_notify_obj_ptr       sigs_func_ptr,
  utimer_osal_notify_data          sigs_mask_data
);

/**
Note: Deferrable option is not enabled currently in utimers
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
utimer_error_type utimer_set_deferrable   /* Todo: Need to enable this api when deferrable timers are needed */
( 
  utimer_ptr_type                  timer, 
  boolean                          condition 
);

/**
Sets an inactive utimer to expire at a fixed time, or changes an
active utimer to expire at that given time. The timer is to be set
in units of ticks, it is in the 19MHz domain.

@param[in] utimer  UTimer to set.
@param[in] time    Time in ticks specified as the exact timer expiry.

@return
utimer_error_type. Possible return values are
UTE_INVALID_PARAMETERS
UTE_INVALID_TIMER_HANDLE
UTE_NO_FREE_INTERNAL_TIMER
UTE_SUCCESS
  
@dependencies  
Timer should be defined using one of the utimer def apis

@sideeffects 
This function may generate signals to a task, which can cause task switches. \n
This function may queue an APC.
*/
utimer_error_type utimer_set_absolute
(
  utimer_ptr_type                  timer,
  utimer_timetick_type             time
);

/**
Sets an inactive utimer to expire after a given period of time, or changes an
active utimer to expire after a given period of time. Optionally, specifies
the timer to repeatly expire with a given period.  If the timer 
is to be set in units of ticks, it will be done in the 19MHz domain. 

@param[in] utimer  UTimer to set.
@param[in] time    Time in units specified until first timer expiry.
@param[in] reload  Period in units specified between repeated 
                   expiries (0 = not periodic).
@param[in] unit    Please refer comments of utimer_unit_type

@return
utimer_error_type. Possible return values are
UTE_INVALID_PARAMETERS
UTE_INVALID_TIMER_HANDLE
UTE_NO_FREE_INTERNAL_TIMER
UTE_SUCCESS
  
@dependencies  
Timer should be defined using one of the utimer def apis

@sideeffects 
This function may generate signals to a task, which can cause task switches. \n
This function may queue an APC.
*/
utimer_error_type utimer_set_64
(
  utimer_ptr_type                  timer,
  utimer_timetick_type             time,
  utimer_timetick_type             reload,
  utimer_unit_type                  unit
);

/**
Gets the number of units before the utimer expires. If the unit 
is ticks, it will return value in 19MHz domain. 

- Zero is returned for inactive utimers and utimers that have expired or are
expiring. 
- Fractional values that might truncate to zero, depending on the unit
passed as a parameter, are rounded to 1.

@param[in] utimer Timer to get.
@param[in] unit   Unit to use to measure the ticks before the timer expires.

@return
Number of units before timer expires. \n

@dependencies  
None.
*/
utimer_timetick_type utimer_get_64
(
  utimer_ptr_type                  timer,
  utimer_unit_type                  unit
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
Min. 1 will be returned for an active timer before clearing 

@note1hang
- If the timer_clr api is called at the moment of timer being expired
there can be chances that timer gets expired first and then utimer_clr
will simply return 0 as timer got expired but callback/signal calling can be
running on parallel thread. Therefore, it is always advised to check if there
is sufficient time(ex: 1msec or few ticks) before calling this api.
timer_get_64() can be used to check how much time is still left for the expiry.

@dependencies  
None.
*/
utimer_timetick_type utimer_clr_64
(
  utimer_ptr_type                 timer,
  utimer_unit_type                 unit
);

/**
utimer_stop

Same as utimer_clr_64 which stops an active timer and also with an error return.

@param[in] timer Timer to be stopped.
@param[in] unit  Unit to use to measure the ticks before expiry.
@param[in] rem_time  Amount of time remaining before expiry. 
                 Time will be in specified unit. This can be NULL.
                 If the unit is TU_TICK, ticks will be from XO clk
                   
@return
Status of the operation. Possible return values are:
UTE_INVALID_PARAMETERS
UTE_INVALID_TIMER_HANDLE
UTE_TIMER_ALREADY_IN_SAME_STATE
UTE_SUCCESS
  
@dependencies  
None.

@Note
If the utimer_stop api is called at the moment of timer being expired
there can be chances that timer gets expired first and then utimer_stop
will simply return 0 as timer got expired. Therefore, it is always
advised to check if there is sufficient time(ex: 1msec or few ticks)
before calling this api. utimer_get() can be used to check how much
time is still left for the expiry.
*/
utimer_error_type utimer_stop
(
  utimer_ptr_type                timer,
  utimer_unit_type                unit,
  utimer_timetick_type          *rem_time
);

/**
utimer_undef

Stops the timer and deallocates all the timer memory
for the given timer handle.

@param[in] timer Timer to be stopped.

@return
  UTE_SUCCESS if success
  UTE_TIMER_ALREADY_IN_SAME_STATE if timer handle is invalid or undefined previously

@dependencies
  None
*/

utimer_error_type utimer_undef
(
  /* Timer to Undefine */
  utimer_ptr_type                  timer
);


/* APIs for Sleep task */

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
  /*Structure to filled with first non-deferrable utimer data*/
  utimer_non_defer_data_ptr utimer_data_ptr
);

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
utimer_timetick_type utimer_defer_match_interrupt_64( void ) ;

/** 
Sets the timer ISR to fire at the earliest expiring timer in the active list.
This function is meant to serve as the complementary function to 
timer_defer_match_interrupt().

@dependencies  
This function must be called with interrupts locked.

@sideeffects 
This function adjusts the match value.
*/
void utimer_undefer_match_interrupt_64( void );

/**
FUNCTION UTIMER_REGISTER_FOR_NON_DEFER_EXPIRY_CHANGE

DESCRIPTION
Registers for first non-deferrable utimer expiry change notification.
Only signal type notifications are allowed, for others, error will be 
returned.

DEPENDENCIES
  Should be called from main image(non-uimage) only
  Input objects should be initialized by client

RETURN VALUE
UTE_INVALID_PARAMETERS   If passed sig obj addr is NULL or invalid notify type
UTE_SUCCESS :            If the operation completed successfully
UTE_FAIL :               If the operation completed with failure

SIDE EFFECTS
  None
*/
utimer_error_type utimer_register_for_non_defer_expiry_change
(
  utimer_notify_type			notify_type,
  utimer_osal_notify_obj_ptr 	notify_obj,
  utimer_osal_notify_data 	    notify_mask
);

#endif /* UTIMER_H */

