#ifndef __OSAL_H__
#define __OSAL_H__
/***********************************************************************/
 /*!
        @file osal.h
        @brief This interface abstracts all diag operations at the OS level.

*/
 /*
 * Copyright (C) 2008-2014 QUALCOMM Technologies, Inc.
 *
 * This interface abstracts all diag operations at the OS level.
 * Areas currently abstracted are synchronization, inter-process communication,
 * timers and  thread creation. This file abstracts these functions
 * at the highlest level possible to ease porting between OSs.
 *
 **********************************************************************/

/*===========================================================================


                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/services/osal/inc/osal.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-01-28    ph    Added start signal to be used for handshake with osal
                   in the new task created using osal_create_thread().
2013-10-03    ph   OSAL APIs updated to use ATS timers instead of qurt timers.
2013-06-10    sm   Corrected comments for osal_get_remaining_time()
2011-09-28    is   Support non-deferrable Diag timers
2010-05-14    JV   Doxygenated.
2009-12-03    JV   New APIs osal_thread_exit and osal_thread_join
2009-12-03    JV   New API, osal_set_task_name() to set the current task name.
2009-09-10    JV   Moved the definition of struct osal_mutex_arg_t to an
                   internal header file.
2009-09-09    JV   osal_lock_mutex() and osal_inlock_mutex() are no more
                   function calls in the osal for rex. They map to INTLOCK()
                   and INTFREE()
2009-06-01    JV   Moved the osal_thread_attr_t structure to the types.h file.
2009-04-20    JV   Obsoleted the osal_set_msg_pri() API. Instead doing this
                   in the osal_create_thread(). Passing the pointer to priority
                   array to osal_create_thread().
2009-04-20    JV   Modified signatures of osal_thread_set_pri() and
                   osal_thread_get_pri()
2009-03-04    is   Updated and modified to support additional APIs
2008-08-28    JV   Modified
2008-04-10    eb   Create
===========================================================================*/

/*!
@mainpage Operating System abstraction layer

This interface abstracts all diag operations at the OS level. Areas currently
abstracted are synchronization, inter-process communication, timers and thread
operations. Certain REX specific APIs related to interrupts and the watchdog are
also abstracted here. Certain APIs are no-oped for certain OSes as you will find
in the comments. This file abstracts these functions at the highlest level
possible to ease porting between OSs.
This header file osal.h is common to all 4 abstraction layers, REX, WM, qube/L4
and qube/BLAST.

*/

#include "osal_os.h"

/* Return values */
#define OSAL_SUCCESS 0
#define OSAL_FAILURE 1
#define OSAL_NULL_PTR_RECVD 2

/*Start signal reserved for TASK_START */
#define OSAL_START_TASK_SIG             0x00008000

/* Synchronization (Critical Sections) */
/*===========================================================================
FUNCTION - OSAL_INIT_CRIT_SECT */

/*!
  @brief Initializes a critical section

  @param[in] crit_sect - Pointer to a critical section object

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/* ===========================================================================*/
  int osal_init_crit_sect(osal_crit_sect_t *crit_sect);

/*===========================================================================
FUNCTION - OSAL_DELETE_CRIT_SECT */

/*!
  @brief Deletes a critical section

  @param[in] crit_sect - Pointer to a critical section object

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */

/*===========================================================================*/
  int osal_delete_crit_sect(osal_crit_sect_t *crit_sect);

/*===========================================================================
  FUNCTION   OSAL_ENTER_CRIT_SECT */

/*!
  @brief Locks a critical section

  @param[in] crit_sect - Pointer to a critical section object

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_enter_crit_sect(osal_crit_sect_t *crit_sect);

/*===========================================================================
  FUNCTION   OSAL_EXIT_CRIT_SECT */

/*!
  @brief Unlocks a critical section

  @param[in] crit_sect - Pointer to critical section object

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/* ===========================================================================*/
  int osal_exit_crit_sect(osal_crit_sect_t *crit_sect);

  /* Synchronization (Mutexes) */
/*===========================================================================
  FUNCTION   OSAL_INIT_MUTEX */

/*!
  @brief Initializes a mutex

  @param[in] mutex - Address of Mutex object. Mutex will be initialized after the call

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_init_mutex(osal_mutex_arg_t *mutex);

/*===========================================================================
  FUNCTION   OSAL_DELETE_MUTEX */

/*!
  @brief Deletes a mutex

  @param[in] mutex - Address of Mutex object

  @dependencies None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_delete_mutex(osal_mutex_arg_t *mutex);

/* Signaling */
/*===========================================================================
  FUNCTION   OSAL_SET_SIGS */

/*!
  @brief
       Sets a signal in the task TCB for systems running on rex and BLAST. In the case of
       Qube, sends a message to the thread whose TCB is passed in.

  @param[in]  tcb_ptr - tcb pointer of the task/thread for which to set the signal
  @param[in]  sig     - the signals/messages to be set
  @param[out] sig_ptr - Address of signal object. The sigs prior to the set
                          action will be filled into this.

  @dependencies
       Every task or thread using this OSAL needs to maintain an array of integers
       containing priorities of each signal/msg it uses. Needs to call
       set_msg_priority() with the pointer to that array before the call
       to create the task. The set_msg_priority() is no-oped in rex.

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_set_sigs(osal_tcb_t *tcb_ptr, osal_sigs_t sig, osal_sigs_t *sig_ptr);

/*===========================================================================
  FUNCTION   OSAL_RESET_SIGS */

/*!
  @brief
       Clears the specified signals or messages for the TCB

  @param[in]  tcb_ptr - tcb pointer of the task/thread for which to reset the signal
  @param[in]  sig     - the signals/messages to be reset or cleared
  @param[out] sig_ptr - Address of signal object. The sigs prior to the reset
                          action will be filled into this.

  @dependencies
       Every task or thread using this OSAL needs to maintain an array of integers
       containing priorities of each signal/msg it uses. Needs to call
       set_msg_priority() with the pointer to that array before the call
       to create the task. The set_msg_priority() is no-oped in rex.

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */

/*===========================================================================*/
  int osal_reset_sigs(osal_tcb_t *tcb_ptr, osal_sigs_t sig, osal_sigs_t *sig_ptr);

/*===========================================================================
  FUNCTION   OSAL_GET_SIGS */

/*!
  @brief Gets the currently asserted signals/msgs for the task/thread

  @param[in]  tcb_ptr - tcb pointer of the task/thread whose signal to get
  @param[out] sig_ptr - Address of signal object. The current sigs/messages
                          for a task will be filled here

  @dependencies
       Every task or thread using this OSAL needs to maintain an array of integers
       containing priorities of each signal/msg it uses. Needs to call
       set_msg_priority() with the pointer to that array before the call
       to create the task. The set_msg_priority() is no-oped in rex.

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_get_sigs(osal_tcb_t *tcb_ptr, osal_sigs_t *sig_ptr);

/*  Timers */
/*===========================================================================
  FUNCTION   OSAL_CREATE_TIMER */

/*!
@brief Creates a timer in the default timer group which is defferable.

  @param[out] timer - Address of a valid timer object
  @param[in]  tcb_ptr - tcb of task/thread to set the signal for
  @param[in]  sig -    Signal/message to be set when the timer expires

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*=========================================================================== */
  int osal_create_timer(osal_timer_t *timer, osal_tcb_t *tcb_ptr, osal_sigs_t sig);

/*===========================================================================
  FUNCTION   OSAL_CREATE_TIMER_EX */

/*!
    @brief
       Extended version of timer-creation function. Allows the
       specification of a callback function and an argument to this
       function. The callback is called when the timer expires. The
       callback happens in the context of the task that calls
       osal_create_timer_ex.

       This timer will be created in the default timer group
       which is defferable.

  @param[out] timer - Address of a valid timer object
  @param[in] tcb_ptr -    Signal/message to be set when the timer expires
  @param[in] timer_cb_ptr - pointer to timer callback
  @param[in] cb_param - argument passed to timer callback

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_create_timer_ex(osal_timer_t *timer, osal_tcb_t *tcb_ptr,
                           osal_timer_cb_type timer_cb_ptr, osal_timer_param_type cb_param);


/*===========================================================================
  FUNCTION   OSAL_CREATE_DIAG_TIMER */

/*!
  @brief
    Creates a timer in the default diag timer group (diag_timer_group),
    which is non-defferable.

  @param[out] timer - Address of a valid timer object
  @param[in]  tcb_ptr - tcb of task/thread to set the signal for
  @param[in]  sig -    Signal/message to be set when the timer expires

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None
 =========================================================================== */
  int osal_create_diag_timer(osal_timer_t *timer, osal_tcb_t *tcb_ptr, osal_sigs_t sig);

/*===========================================================================
  FUNCTION   OSAL_CREATE_DIAG_TIMER_EX */

/*!
  @brief
       Extended version of timer-creation function. Allows the
       specification of a callback function and an argument to this
       function. The callback is called when the timer expires. The
       callback happens in the context of the task that calls
       create_timer_ex.

       This timer will be created in the diag timer group
       (diag_timer_group), which is made non-defferable.

  @param[out] timer - Address of a valid timer object
  @param[in]  tcb_ptr -    Signal/message to be set when the timer expires
  @param[in]  timer_cb_ptr - pointer to timer callback
  @param[in]  cb_param - argument passed to timer callback

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_create_diag_timer_ex(osal_timer_t *timer, osal_tcb_t *tcb_ptr,
                           osal_timer_cb_type timer_cb_ptr, osal_timer_param_type cb_param);

/*===========================================================================
  FUNCTION   OSAL_SET_TIMER */

/*!
  @brief Starts a timer

  @param[in] timer - Address of the timer object to start. The timer handle is filled in
  @param[in] timer_cnt - Duration for the timer in ms

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_set_timer(osal_timer_t *timer, osal_timer_cnt_t timer_cnt);

/*===========================================================================
  FUNCTION   OSAL_RESET_TIMER */

/*!
  @brief
       Re-sets a timer

  @param[in] timer - Address of the timer object to reset.

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_reset_timer(osal_timer_t *timer);

/*===========================================================================
  FUNCTION   OSAL_TIMED_WAIT */

/*!
  @brief
       Suspends a task and sets a timer. When either the timer or one of the
       signals is set and the task is the highest ready task, the task resumes.

  @param[in] tcb_ptr - task associated with the timer
  @param[in] sig - sigs to wait on
  @param[in] timer - pointer to timer to be set and wait on
  @param[in] timer_cnt - time to wait

  @dependencies
       None

  @return
        returns the signal that was set.

  @sideeffects
       Causes a task swap if the signals are not already set for the calling task.*/

/*===========================================================================*/
  osal_sigs_t osal_timed_wait(osal_tcb_t *tcb_ptr, osal_sigs_t sig,
                              osal_timer_t *timer, osal_timer_cnt_t timer_cnt);

/*===========================================================================
  FUNCTION   OSAL_DELETE_TIMER */

/*!
  @brief
       Deletes or clears a timer

  @param[in] timer - Address of the timer object to clear


  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_delete_timer(osal_timer_t *timer);

/*===========================================================================
  FUNCTION   OSAL_GET_REMAINING_TIME */

/*!
  @brief
       Returns the current remaining count of a timer. 0 is returned if the timer
       is not on the list.

  @param[in] timer_ptr - pointer to the timer to get

  @dependencies
       None

  @return
  <ul>
  <li> time count
  <li> 0
  </ul>

  @sideeffects None */
/*===========================================================================*/
  osal_timer_cnt_t osal_get_remaining_time(osal_timer_t *timer_ptr);

/* Thread Manipulation  */
/*===========================================================================
  FUNCTION   OSAL_CREATE_THREAD */

/*!
  @brief
       Creates a thread/task. Also sets the start signal to the new task created.
       It is the responsibility of the caller to call the API osal_handshake_start()
       where it waits on the start signal before it can continue processing.

       The signal OSAL_START_TASK_SIG(0x00008000) is reserved as start up signal
       for any new task that is created through this API and clients are advised not
       to use this signal for other purposes to avoid conflicts.



  @param[in] tcb_ptr - Valid tcb for new task/thread
  @param[in] attr - Thread attributes object
  @param[in] qube_msg_q_pri - array to pass in the priorities of msg queues for qube.
                   Ignore for all other OSes.

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_create_thread(osal_tcb_t *tcb_ptr, osal_thread_attr_t *attr, int *qube_msg_q_pri);

/*===========================================================================
  FUNCTION   OSAL_DELETE_THREAD */

/*!
  @brief
       Exits the current calling thread/task.  Calling osal_delete_thread() for an application's
       primary thread/task causes the application to terminate.

  @param[in] tcb_ptr - Valid tcb for the task/thread

  @dependencies
       None

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 1 OSAL_FAILURE
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_delete_thread(osal_tcb_t *tcb_ptr);

/*===========================================================================
  FUNCTION   OSAL_HANDSHAKE_START */
/*!
  @brief
       waits indefinitely on the start signal that indicates
       the task is ready to execute.

  @param[in] tcb_ptr - Valid tcb for the task/thread

  @dependencies
       Prior to this call, a new task should have been created using
       osal_create_thread(). The same tcb_ptr should be provided here
       while calling osal_handshake_start() as an argument so that the
       signal object(created earlier in osal_create_thread() )information
       is obtained from the tcb_ptr.

  @return One of the following error codes
  <ul>
  <li> 0 OSAL_SUCCESS
  <li> 2 OSAL_NULL_PTR_RECVD
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_handshake_start(osal_tcb_t *tcb_ptr);

/* Self Reference */
/*==========================================================================
  FUNCTION   OSAL_THREAD_SELF */

/*!
  @brief
       Gets a pointer to the TCB of the currently running task/thread

  @param None

  @dependencies
       None

  @return
  <ul>
  <li> pointer to qurt_tcb_t object.
  <li> NULL
  </ul>

  @sideeffects None */
/*==========================================================================*/
  osal_tcb_t *osal_thread_self(void);
/*===========================================================================
  FUNCTION   OSAL_SET_TASK_NAME */

/*!
  @brief
       Set the name for the current task/thread

  @param[in] name - Pointer to the string

  @dependencies
       None

  @return None

  @sideeffects None */
/*==========================================================================*/
  void osal_set_task_name(void *name);
/*===========================================================================
  FUNCTION   OSAL_THREAD_EXIT */

/*!
  @brief
    The thread_exit() function terminates the calling thread, making its exit
    status available to any waiting threads.

  @param[in] status - exit status

  @dependencies
       None

  @return None

  @sideeffects None */
/*==========================================================================*/
  void osal_thread_exit(int status);
/*===========================================================================
  FUNCTION   OSAL_THREAD_JOIN */

/*!
  @brief
    The thread_join() function shall suspend execution of the calling thread
    until the target thread terminates unless the target thread has already
    terminated.

  @param[in] tcb pointer - tcb pointer of the thread
  @param[out] status - address to fill in the status

  @dependencies
       None

  @return None

  @sideeffects None */
/*==========================================================================*/
  void osal_thread_join(osal_tcb_t *tcb_ptr, int *status);

/*===========================================================================
  FUNCTION   OSAL_THREAD_GET_PRI */

/*!
  @brief
       Gets the priority of the currently thread

  @param None

  @dependencies
       None

  @return Priority of the thread

  @sideeffects None */
/*==========================================================================*/
  osal_priority_type osal_thread_get_pri(void);

/*===========================================================================
FUNCTION   OSAL_THREAD_SET_PRI */

/*!
  @brief
       Sets the priority of the currently running thread

  @param[in] prio - priority to be set to

  @dependencies None

  @return The priority before it was changed.

  @sideeffects None */
/*==========================================================================*/
  osal_priority_type osal_thread_set_pri(osal_priority_type prio);

/*===========================================================================
  FUNCTION   OSAL_THREAD_WAIT */

/*!
  @brief
       Waits on specified signals/messages

  @param[in] tcb_ptr - tcb of task that the signals are associated with
  @param[in] sig - signals/messages to wait on

  @dependencies None

  @return Signals/messages asserted

  @sideeffects None */
/*===========================================================================*/
  osal_sigs_t osal_thread_wait(osal_tcb_t *tcb_ptr, osal_sigs_t sig);

/* Certain rex specific APIs related to interrupts and the watchdog */
/*===========================================================================
  FUNCTION   OSAL_AUTODOG_ENABLE */

/*!
  @beief
       Performs initialization needed to set up the auto dog service for a task.
       When auto-dog is on, the task does not need to wake up periodically to
       kick the dog when making a blocking call. The OS takes care of notifying
       Dog that the task is blocked and that there is no need to monitor the
       task until further notice. When the task is scheduled again, Dog is
       notified that the task is ready to be monitored again.

       WM, BLAST on aDSP, qube:  Since there's no dog services these platforms,
       this function does nothing and returns 0.

  @param None

  @dependencies None

  @return Returns the previous state of the autodog_enabled field in the TCB.

  @sideeffects None */
/*===========================================================================*/
  int osal_autodog_enable(int dog_report_val);

/*===========================================================================
FUNCTION   OSAL_AUTODOG_DISABLE */

/*!
  @brief
       Disables the auto dog service for a task.
       WM, BLAST on aDSP, qube:  Since there's no dog services these platforms,
       this function does nothing and returns 0.

  @param None

  @dependencies None

  @return Returns the previous state of the autodog_enabled field in the TCB.

  @sideeffects None */
/*===========================================================================*/
  int osal_autodog_disable( void );

/*===========================================================================
FUNCTION    OSAL_INTS_ARE_LOCKED */

/*!
  @brief
       Gets the interrupts Lock status
       For WM, BLAST on aDSP and qube currently does nothing and always returns 0

  @param None

  @dependencies None

  @return One of the following
  <ul>
  <li> dword: 0 - interrupts are currently Disabled in HW
  <li> otherwise  - interrupts are currently Enabled in HW
  </ul>

  @sideeffects None */
/*===========================================================================*/
  int osal_ints_are_locked( void );

/*===========================================================================
FUNCTION    OS_IS_IN_IRQ_MODE */

/*!
  @brief
       Checks to see if the current mode is IRQ mode or System mode
       and FALSE otherwise. Only for REX OS.

  @param None

  @dependencies None

  @return One of the following
  <ul>
  <li> TRUE - if the current mode is IRQ mode or System mode
  <li> FALSE - otherwise
  </ul>

  @sideeffects None */
/*===========================================================================*/
  boolean os_is_in_irq_mode( void );

#endif /* not __OSAL_H__ */

