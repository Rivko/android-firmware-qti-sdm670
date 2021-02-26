/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       R E X   A R M   P O R T

GENERAL DESCRIPTION
   This is the ARM Port of REX (Real Time Executive).
   It is a simple, efficient real-time executive for the ARM7TDMI processors.
   It supports multi-tasking according to a priority  scheduling algorithm,
   and provides a small set of services for task suspension and reactivation.
   All REX services follow the APCS conventions for function call/return and
   parameter passing.

 TASKS

   REX treats tasks as independently dispatchable entities, each having
   its own stack, signal flags and priority which collectively constitute
   the context of the task. Each task has an associated data structure,
   called the task context block (TCB), which REX uses for keeping track of
   the task's context. REX maintains a list of tasks in which all task TCBs
   are linked in order of decreasing task priority. The first TCB in the
   task list belongs to the highest priority task, while the last TCB be-
   longs to the lowest priority task (i.e., the kernel task whose priority
   is zero).

   REX allows an arbitrary number of tasks to be created dynamically at
   any time during execution. Realistically, however, REX's performance
   deteriorates slightly with each added task, so care is called for in
   keeping the number of tasks to a minimum. Ultimately, the task load
   which REX can support depends on the processor chosen, the processor
   clock speed and the interrupt characteristics of the particular system.

 TASK STACKS

   As mentioned above, each task has its own stack which is used when-
   ever the task is running. When the task is suspended (for example, to
   run another task or to service an interrupt), the task's registers
   are pushed onto the task's stack, and then the top of stack pointer
   is saved in the task's TCB. Later, when the task is selected for
   running again, the top of stack pointer is restored from the task's TCB,
   the task's registers are popped off it's stack, and task execution
   resumes where it left off. The process of task switching is virtually
   invisible to tasks.

 TASK SIGNALS

   Each task has associated with it 32 general purpose signal flags which
   are kept as part of the task's context in the task's TCB. These signals
   are used to notify the task that some kind of event has occurred. A
   task's signals may be set or cleared by any task, and by any interrupt
   handler.

   A task can poll the current state of its signals at any time. Moreover,
   a task can also suspend its own execution pending the setting of one or
   more of its signals. So, for example, Task A may suspend waiting for
   signal 1 to be set. At some later time, Task B  or an interrupt handler
   can wake up Task A by setting signal 1 for A. Once awakened, Task A is
   again eligible for execution, although it will actually run only if it
   is the highest priority task that is ready to run.

   Notice that task signals are treated as a 32 bit wide mask. This means
   that all 32 signals for a task may be set or cleared in a single op-
   eration and, similarly, a task may suspend on up to 32 different signals
   at the same time, waking up when any one of them is set.

    The important points to keep in mind about signals are:

     1. 32 signals are defined on a per task basis. For purposes of setting,
        clearing and waiting, signals are treated as 32 bit masks.

     2. Signals are general purpose flags whose meanings may be defined
        any way the user sees fit.

     3. A task may optionally suspend execution pending the setting of
        particular signal(s), presumably by another task or an interrupt
        handler. The emphasis here should be on the word 'optionally'
        since setting a signal for a task that is not suspended on the
        signal does not directly affect the execution state of that task.


 TASK PRIORITIES AND SCHEDULING

   Each task has associated with it a priority that is stored in its TCB.
   Priorities range from 1 to 2^32-1, with 1 being the lowest priority and
   2^32-1 being the highest. REX reserves priority value zero for use with
   the kernel task.

   When scheduling, REX ALWAYS chooses the highest priority ready task,
   i.e., the highest priority task that is not suspended waiting on sig-
   nal(s). REX will continue to execute this task until it voluntarily
   suspends, or until an interrupt occurs which reactivates a higher pri-
   ority task by setting signals for the task. In the case where all user
   defined tasks suspend, the kernel task, defined by REX, will execute.

   REX also provides a mechanism by which a task can raise or lower its
   own priority dynamically.

===========================================================================

  INTERRUPTS

    When an interrupt occurs, the corresponding interrupt handler must
    immediately save the register context of the interrupted entity,
    which may be a task or a lower priority interrupt.

===========================================================================

  TIMERS

    Timer-related functions are now in rextime.c.

===========================================================================

  CRITICAL SECTIONS

    Critical sections are used to provide mutual exclusion between tasks,
    while allowing other un-involved tasks to still be scheduled as well
    as allowing interrupts to happen.  A critical section variable is
    declared globally, and must be initialized via a function call before
    it can be used.  Tasks can henceforth "enter" and "leave" the critical
    section.

    If a task attempts to enter a critical section while it is being held
    by a different task, the attempting task will be blocked until the
    critical section becomes available.

    REX keep a list of waiting tasks attached to the critical section
    variable. The list itself is constructed by linking pointers that appear
    within the TCBs of the tasks themselves.

    When a task releases a critical section, REX will select the highest
    priority waiting task to schedule.

=============================================================================

===========================================================================

  ASYNCHRONOUS PROCEDURE CALLS

    Asynchronous Procedure Calls provide a means to postpone the execution
    of a function until the REX is executing in the designated context.
    Queueing an APC for a task causes the APC to be executed the next time
    the task is given CPU.  APC's cannot be queued for the currently
    running task.

=============================================================================

EXTERNALIZED FUNCTIONS
  rex_init_crit_sect
     Initilalzes a critical section, setting elements to 0, NULL, etc.

  rex_enter_crit_sect
     Acquires the lock on a critical section, or suspends the calling task
     until the critical section is available.

  rex_leave_crit_sect
     Releases the lock on a critical section and grants it to a waiting task
     (if there is one waiting).

  rex_clr_sigs
     Clears a specified mask of signals for a specified task.

  rex_set_sigs
    Sets a specified mask of signals for a specified task. If the specified
    task is waiting on any of the signals set, it will be again be eligible
    for execution.

  rex_get_sigs
    Returns the state of the signals for a specified task.

  rex_wait
    Suspends the calling task until at least one signal of a specified
    signal mask is set for the task. If any signal of the signal mask is
    already set prior to calling rex_wait, an immediate return is made
    from the function and the calling task continues to execute.

  rex_def_task
    Creates a new task using a specified TCB, stack, priority, entry point
    and task parameter. Task will start running only if it is the highest
    priority ready task.

  rex_def_task_ext
    An extended version of rex_def_task that adds two parameters to the
    argument list.  The first is a string for the task name, the second
    is a boolean variable that indicates if the task should be started in
    "suspended" mode.  If that option is chosen, the task will not actually
    run until a rex_resume_task() call is issued.

  rex_suspend_task
    Suspends a REX task.

  rex_resume_task
    Allows a suspended task to be scheduled.

  rex_kill_task
    Kill a task by yanking the TCB from the list of active tasks.  If the
    same TCB is used in a future call to rex_def_task, the task will be
    started from the beginning, in other words, no state information from
    the previous existence will still be around.

  rex_self
    Returns a pointer to the TCB of the calling task.
    to obtain the address of its own TCB.

  rex_set_pri
    Changes the priority of the calling task to a specified priority.

  rex_task_pri
    Sets the priority of the task chosen to a determined value.

  rex_get_pri
    Returns the current priority of the calling task.

  rex_init
    Initializes REX and fires up a user startup task using a specified TCB,
    stack, priority, entry point and startup parameter.

  rex_set_interrupt_vector
    Given the id v of an installable vector and a pointer to the routine to
    be called when the interrupts occurs, this function installs the routine.

  rex_enable_interrupt
    Enables the interrupt associated with id v.

  rex_disable_interrupt
    Disables the interrupt associated with id v.

  rex_get_version_number
   Returns a REX version number. The version number returned is a
   decimal number in which the least significant two digits indicate
   the minor version number and the remaining digits (at most two)
   indicate the major version number. For instance, if the value
   returned is 307, the major version number is 3 and the minor version
   number is 7.

  rex_clr_profile
    Clears stored profiling information. Clears the numticks and
    rex_IRQnumticks fields in each TCB in the task list. The rex_pnumticks
    does not need to be cleared because it would be in sync with
    rex_time_tick2.

  rex_init_profile
    Performs initialization necessary for software profiling.

  rex_enable_profile
    Enables software profiling.

  rex_disable_profile
    Disables software profiling.

  rex_get_profile
    Retrieves software profiling information. Traverses rex_task_list and
    gets the numticks field from each TCB in the task list. In addition,
    gets rex_IRQnumticks as well.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  rex_init must be called to initialize the operating system.

Copyright (c) 1998,1999 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Copyright (c) 2000,2001 by Qualcomm Technologies Incorporated.  All Rights Reserved.
Copyright (c) 2002-2008 by Qualcomm Technologies Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/kernel/rex/src/rex.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/08   cz      Fixed a priority inverion issue in crit_sect..
06/11/08   cz      Merged changes needed for QSC1100.
07/13/07   cz      Added support for DAC based memory protection feature from umr.
07/25/06    pc     Added support for FEATURE_CPU_BASED_FLOW_CONTROL.
03/07/06    ck     Initialized dpc_param flags.
10/14/05    ck     Merged in the fix to not lock interrupts during a DPC call
                   Cleaned up REX_INTLOCK to match with REX_INTFREE
09/21/05    cr     Added PC logging functionality as an overloaded
                   functionality of stack checked builds. This requires a
                   value in r10 stack limit register that will always result
                   in overflow function being called.
03/09/05    ck     Merged in the fix to not lock interrupts when APC call is
                   executed.
11/17/04    gr     Fixed a bug that was preventing rex_kill_task_ext from
                   releasing critical sections correctly.
11/11/04    rp     Added Progressive boot support. New functions
                   rex_get_best_task and rex_resume_task_from_abort.
08/11/04    is     Externalized rex_kernel_tcb for REX profiling.
06/23/04    gr     Added IPC support in rex_kill_task_ext.
06/03/04   gfr     Rex intlock profiling support.
05/12/04    gr     Fixed a bug in the interaction of critical sections and
                   APCs.
05/12/04    gr     Changed REX_ASSERT to ASSERT.
04/16/04    gr     Minor modification to rex_set_best_task to make it more
                   efficient.
03/04/04    gr     Added IPC support.
11/25/03    gr     Deleted the debug versions of the Rex memory allocation
                   functions.
10/28/03    gr     Made rex_nest_depth global, since it is used elsewhere in
                   DMSS.
09/22/03   ksu     A task can now enter more than one critical section.
07/02/03    gr     Modified rex_apc_prolog to handle user mode.
06/20/03    gr     Modifications to get FEATURE_REX_PROFILE to work. Added
                   functions to disable and enable task switching.
04/15/03    cr     Fixed critical section bug in case where there is only a
                   single task on the critical section wait list.
02/28/03   eam     Masked off unused bits from SPSR in rex_apc_prolog, to
                   ensure Jazelle bit is not set during execution of an APC.
09/04/02    gr     Added the rex_def_task_ext2 function that allows the user
                   to specify a dog report value. rex_autodog_enable() and
                   rex_autodog_disable() now return the previous state.
07/16/02    gr     Updated REX version number.
07/16/02    gr     Renamed rex_autodog_init to rex_autodog_enable. Added
                   function rex_autodog_disable.
07/15/02    gr     Moved timer-related functions to rextime.c.
07/10/02    gr     Minor change to suppress a warning in rex_decrement_timers.
07/03/02    gr     Added support for pausing the monitoring of a task by Dog if
                   the task is blocked by a call to rex_wait or a call to
                   rex_enter_crit_sect. A task must call rex_autodog_init in
                   order to use this functionality. Monitoring by Dog resumes
                   when the task is made runnable.
06/04/02    gr     Fixed a comment.
05/29/02    gr     Fixed a featurization problem.
05/23/02    gr     Updated critical section code to allow a task to seize a
                   critical section more than once. Added code to initialize
                   the err_num field in the tcb. Added a function to allow
                   access to the internals of REX.
03/29/02   plm     Initialized tcb.leds field for rex_idle_task and rex_dpc_task
                   in rex_init() to support TIMETEST.
03/27/02    gr     Removed all references to rex_defer_sched, which is no
                   longer used.
03/26/02    gr     Fixed a bug in REX timer management.
03/21/02    gr     Cosmetic changes. Updated some comments, fixed indentation
                   problems and updated the copyright notice.
03/20/02    dc     Added support for FEATURE_SOFTWARE_PROFILE.
03/13/02    gr     Added function rex_get_version_number.
10/02/01    gr     Added support for memory debug functionality. Fixed a
                   stack-size bug in rex_def_task. Added assertions to catch
                   calls to the REX memory allocation functions from interrupt
                   context.
08/21/01    gr     Added function rex_exit.
08/15/01    gr     New APC implementation that is simpler than the old one and
                   is not dependent on the exact sequence of actions in
                   IRQ_Handler. Support for system signals.
06/06/01    gr     Modifications to enable the use of the extended timer
                   interface without turning on dynamic memory support.
12/19/00   jct     Merged MSM5000, MSM5200, MSP1000 versions of REX
06/08/00   rmd     Added MSM5000_IRAM_FWD #ifndefs around the functions
                   that will be copy into Internal RAM.
02/02/00   jct     Integrated preemptive interrupt changes
02/02/00   tim     Support for PIC.
1/xx/00    tim     Added priority inversion prevention to critical sections.
12/16/99   tim     Introduction of feature whereby a task starts out suspended,
                   also added task name to TCB.
12/07/99   tim     Introduction of critical sections construct for providing
                   mutual exclusion between tasks.
11/30/99   tim     Added rex_kill_task() function, changed rex_task_preamble
                   so that it will invoke rex_kill_task() if a task returns.
07/07/99   jct     Integrated Windows NT support
05/19/99    ms     Changes in rex_report_stack_overflow.
04/09/99    ms     Lint cleanup.
03/31/99    ms     Fixed rex_set_sigs to correctly compare priorities.
03/19/99    ms     Defined a new function rex_report_stack_overflow.
02/19/99    ms     Now running in Supervisor mode.
                   Changed interface of rex_init and rex_def_task. The stack
                   pointers in their parameter lists are void *.
                   Using a new type rex_stack_size_type for stack sizes.
02/12/99    ms     Added support for TIMETEST port.
02/05/99    sk     Reduced Kernel Task stack size to 40 words.
02/04/99    ms     Support for FEATURE_SI_STACK_WM.
12/02/98    ms     rex_task_list and rex_timer_list changed to headnodes.
11/30/98   jkl     Added new functions. Revised for code standards.
11/04/98   jkl     Revised for code standard (rex types)
09/10/98   jkl     Adapted for new context frame (Delta Rex)
05/27/98   jbh     Created module based on rex.asm 1.7
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#define REX_C

//#include "customer.h"
#include <stdlib.h>
#include "rex.h"

//#include <stringl/stringl.h>
#include <string.h>

#include "assert.h"

#ifndef ASSERT
#define ASSERT assert
#endif

#ifdef FEATURE_REX_TIMESLICING
  #include "DalDevice.h"
  #include "DDIInterruptController.h"
#endif

#ifdef FEATURE_REX_STAT
extern void rex_stat_init( void );
#endif
extern void rex_sched_internal(void);
extern byte *rex_init_stack(byte *, rex_task_func_type, rex_task_func_param_type);

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------
** Defines
**--------------------------------------------------------------------*/
#define REX_SLEEP_SIG 0x80000000

/*----------------------------------------------------------------------
** Kernel stack size in bytes.
**--------------------------------------------------------------------*/
#ifndef REX_KERNEL_STACK_SIZE
#define REX_KERNEL_STACK_SIZE 128
#endif

/*----------------------------------------------------------------------
** The interrupt stack pointer (not static - the scheduler is not
**                              in this module)
**--------------------------------------------------------------------*/
rex_stack_word_type *  rex_int_stack;

/*-----------------------------------------------------------------------
** The current task pointer (not static - the scheduler is not
**                              in this module)
**---------------------------------------------------------------------*/
rex_tcb_type  * rex_curr_task;


/*-----------------------------------------------------------------------
** The highest priority ready task (not static - the scheduler is not
**                              in this module)
** Notes: rex_best_task is set to the desired task to swap in prior
**  to making calls to rex_sched().  However, in some cases, such as
**  while a TASKLOCK is in effect or while ISRs are being processed,
**  rex_sched() will not be allowed to actually make the swap until
**  sometime later.  As such, rex_best_task could be written to several
**  times before rex_sched() happens. So, in all places when making
**  the decision to update rex_best_task, the decision needs to be
**  based on a comparison with what is already in rex_best_task, NOT
**  with rex_curr_task.  rex_best_task should be updated only when a
**  new task is found of higher priority than what rex_best_task has
**  already been set to.
**---------------------------------------------------------------------*/
rex_tcb_type  * rex_best_task;
uint32   rex_first_task_scheduled = 0;
/*-----------------------------------------------------------------------
   The task that will be scheduled next.This is normally the bext task
   except when a page fault happens when tasks are locked. In this case
   we cannot schedule the best task. The aborted task will be scheduled
   in this case

**---------------------------------------------------------------------*/
rex_tcb_type  * rex_task_to_schedule;
/*----------------------------------------------------------------------
** The task_list head node.
**--------------------------------------------------------------------*/
rex_tcb_type rex_task_list;


/* --------------------------------------------------------------------
** Count of REX Tasks
** -------------------------------------------------------------------- */
int rex_num_tasks = 0;

/*----------------------------------------------------------------------
** The kernel task which is the last member on the task list
**--------------------------------------------------------------------*/
static rex_tcb_type rex_kernel_tcb;

REX_DECLARE_STACK(rex_kernel_stack, REX_KERNEL_STACK_SIZE);


/*---------------------------------------------------------------------
** External declarations
**--------------------------------------------------------------------*/
extern void rex_arch_init(void);

extern void dog_monitor_pause( int dog_report_val );
extern void dog_monitor_resume( int dog_report_val );
#ifdef FEATURE_REX_TLS
extern void rex_tls_init(void);
#endif
extern void rex_invoke_tls_destructors(rex_tcb_type *tcb);


/*---------------------------------------------------------------------
** The interrupt nesting level - 0 means a task is being executed, 1
** means a single interrupt, 2 means a nested interrupt, etc.
**--------------------------------------------------------------------*/
word rex_int_nest_level;


/*---------------------------------------------------------------------
** Variables for supporting TASKLOCK/TASKFREE, macros that turn off
** context switching (but not interrupts) when mutual exlcusion is
** needed.
**--------------------------------------------------------------------*/

int rex_sched_allow = TRUE;    /* turns sched on/off */

unsigned int rex_nest_depth = 0;  /* supports nesting of TASKLOCK/
                                   * FREE.
                                   */

static int rex_init_complete = 0; /* Set to 1 after rex_init */

rex_err_handler_type rex_err_fatal_handler = NULL;

#if defined(FEATURE_REX_TIMESLICING) && defined(FEATURE_REX_TLS)
  rex_tls_key_t rex_slices_key;

  DalDeviceHandle                         *p_heartbeat_handle;
  DalDeviceHandle                         *p_interrupt_handle;

  #define REX_TIMESLICE_LENGTH_TICKS      0x2EE00              // 10ms @ 19.2MHz (192,000 ticks)
#endif


/* --------------------------------------------------------------------
** Rex version number. A decimal number of the form yxx or yyxx, where
** y denotes the major version number and x the minor version number.
** -------------------------------------------------------------------- */
#define REX_VERSION_NUMBER ((unsigned long) 403)

/* --------------------------------------------------------------------
** REX Task List Manipulation Macros
**   Allows remapping of task manipulation functions in other operation
**   environments.
** -------------------------------------------------------------------- */
#define REX_TASK_LIST_FRONT() (&rex_task_list)
#define REX_TASK_LIST_NEXT( tcb_ptr ) ((rex_tcb_type *) tcb_ptr->link.next_ptr )
#define REX_TASK_LIST_PREV( tcb_ptr ) ((rex_tcb_type *) tcb_ptr->link.prev_ptr )
#define REX_TASK_LIST_POP( tcb_ptr )\
   tcb_ptr->link.next_ptr->link.prev_ptr = tcb_ptr->link.prev_ptr;\
   tcb_ptr->link.prev_ptr->link.next_ptr = tcb_ptr->link.next_ptr;


/*===========================================================================
* MACRO REX_TASK_RUNNABLE
*
* Macro that captures testing of three conditions as to whether a task is
* runnable or not. See block comment in REX_SET_BEST_TASK for more details.
*
* Pass in ptr to TCB to be tested.  Interrupts must be locked prior to
* calling.
*
*==========================================================================*/
   /* ------------------------------------------------
   ** REX, REX++
   ** ------------------------------------------------ */

      /* REX
      **    A task is runnable if it is:
      **       NOT suspended
      **                              AND
      **       NOT waiting on a crit sect that is in the non signalled state
      **                              OR
      **       Has a signal set on which it is waiting
      */
#ifdef FEATURE_REX_TIMESLICING						
    #define REX_TASK_RUNNABLE(tcb)  ((tcb->suspended == FALSE) && \
                                       (tcb->wait      == 0)     && \
                                       (tcb->cs_link.next_ptr == NULL) && \
                                       (tcb->slices != 0))
#else
    #define REX_TASK_RUNNABLE(tcb)  ((tcb->suspended == FALSE) && \
                                       (tcb->wait      == 0)     && \
                                       (tcb->cs_link.next_ptr == NULL))
#endif


/* ==========================================================================
MACRO REX_PAUSE_DOG_MONITOR
DESCRIPTION
   Tells Dog to stop monitoring a specific task. Does nothing if auto-dog
   is not turned on for the task.

============================================================================= */
#define REX_PAUSE_DOG_MONITOR( tcb_ptr ) \
{ \
   if ( ( tcb_ptr->autodog_enabled ) && ( tcb_ptr->dog_report_val >= 0 ) )\
   { \
      /*dog_monitor_pause( tcb_ptr->dog_report_val );*/ \
   } \
}


/* ==========================================================================
MACRO REX_RESUME_DOG_MONITOR
DESCRIPTION
   Tells Dog to resume monitoring a specific task. Does nothing if auto-dog
   is not turned on for the task.

============================================================================= */
#define REX_RESUME_DOG_MONITOR( tcb_ptr ) \
{ \
   if ( ( tcb_ptr->autodog_enabled ) && ( tcb_ptr->dog_report_val >= 0 ) )\
   { \
      /*dog_monitor_resume( tcb_ptr->dog_report_val );*/ \
   } \
}

#ifdef FEATURE_REX_USER_HEAP

uint32 rex_user_heap_initialized = 0;

extern char Image$$AMSS_HEAP_SEG$$Base;
extern void amss_malloc_init(void *start_heap, void *end_heap);


void rex_user_heap_init(void)
{
   if (!rex_user_heap_initialized)
   {
      // TODO:: Heap base will be randomized before call to amss_malloc_init()
      amss_malloc_init((void*)&Image$$AMSS_HEAP_SEG$$Base, (void*)(&Image$$AMSS_HEAP_SEG$$Base+USER_HEAP_SIZE-1));
      
      rex_user_heap_initialized = 1;
   }

}

#endif /*FEATURE_REX_USER_HEAP*/

/* Adding this as ASSERT() is no longer allowed for error checking purposes by KW */
void REXC_ERR_FATAL(const char* err_msg, unsigned int line_num)
{
   abort();
}

static void rex_update_stack_usage(rex_tcb_type *tcb)
{
    unsigned top_addr    = (unsigned)tcb->stack_limit;
    unsigned bottom_addr = (unsigned)tcb->sp;

    /*
     * binary-search the stack top address has been modified.
     */
    while(bottom_addr > top_addr)
    {
        unsigned mid_addr = 0xfffffffc & ((top_addr + bottom_addr)/2); /* must be word-aligned */

        /*
         * check if the stack address has been modified.
         */
        if(*(unsigned*)mid_addr != REX_STACK_FILL_MAGIC)
        {
            /*
             * it has been modified. Update the deepest stack address.
             */
            bottom_addr = mid_addr;
        }
        else
        {
            top_addr = mid_addr + sizeof(unsigned);
        }
    }

    tcb->max_stack_usage = ((unsigned)tcb->stack_limit + tcb->stack_size) - bottom_addr;
}

void rex_sched(void)
{
   rex_tcb_type *temp_task;
   
   /* Move current task to end of its priority chain to enforce round robin scheduling of same priority tasks.
      This for loop will get the last task of the chain.  Then we will determine if we need to place the current
      task at the end.
   */
   for(temp_task = rex_curr_task; temp_task->link.next_ptr && (temp_task->link.next_ptr->pri == rex_curr_task->pri); temp_task = temp_task->link.next_ptr);

   if(temp_task != rex_curr_task)
   {
      /* Place the current task at the end of the priority chain */
      rex_curr_task->link.prev_ptr->link.next_ptr = rex_curr_task->link.next_ptr;
      rex_curr_task->link.next_ptr->link.prev_ptr = rex_curr_task->link.prev_ptr;
      rex_curr_task->link.next_ptr = temp_task->link.next_ptr;
      rex_curr_task->link.prev_ptr = temp_task;
      temp_task->link.next_ptr = rex_curr_task;
      if(rex_curr_task->link.next_ptr)
      {
         rex_curr_task->link.next_ptr->link.prev_ptr = rex_curr_task;
      }
   }
#if defined(FEATURE_REX_TIMESLICING) && defined(FEATURE_REX_TLS)
   /* Current task is being scheduled out, restore slices */
   rex_tls_getspecific(rex_slices_key, (rex_tls_value_t)&rex_curr_task->slices);
#endif

   /*
   ** Update the max stack usage of the current task before switching
   */
   rex_update_stack_usage(rex_curr_task);

   /*
   ** Enable stack guard for the next task
   */
   aop_mpu_enable_psp_stack_guard(TRUE, (unsigned)rex_best_task->stack_limit - AOP_MPU_STACK_GUARD_SIZE);
   
   rex_sched_internal();
}

#if defined(FEATURE_REX_TIMESLICING) && defined(FEATURE_REX_TLS)
/* Timeslicing ISR.  Runs every time the heartbeat timer IRQ fires and schedules out the current
   thread for another thread of same priority or higher if the current thread has exhausted its
   slices. */
void rex_timeslice_isr(void)
{   
   rex_tcb_type *rex_best_sav = rex_best_task;
   DalTimetickTime64Type matchval;

   REX_INTLOCK();
   
   /* We've ran out of slices, attempt to schedule next task of same priority.
      If the current task is the only thread of this priority it will be rescheduled. */
   if(--rex_curr_task->slices == 0)
   {
      /* Our current thread would be in the front of the chain, and now ignored by the following
         set of best_task because slices are now 0x0 */
      rex_set_best_task( REX_TASK_LIST_FRONT() );
      
      /* There is a higher priority task ready than our old best (most likely current) task.  Reschedule.
         Note: This runs in ISR context so scheduling will not occur until the ISR returns. */
      if(rex_best_task->pri >= rex_best_sav->pri)
      {
         rex_sched();
      }
      else
      {
         /* Saved best task is better than new best task, most likely current thread is only thread of its priority.
            Restore slices. */
         rex_best_task = rex_best_sav;
         rex_tls_getspecific(rex_slices_key, (rex_tls_value_t)&rex_curr_task->slices);
      }
   }
   
   /* ReProgram Heartbeat Timer with TVAL = REX_TIMESLICE_LENGTH_TICKS */
   DalTimetick_SetNextInterrupt64(p_heartbeat_handle,
                                   (DalTimetickTime64Type)REX_TIMESLICE_LENGTH_TICKS,
                                   (DalTimetickTime64Type)0x0 /*ignored*/,
                                   &matchval);
   
   REX_INTFREE();
}

/* Initialize timeslicing in the system.  Program the QTIMER frame 0 with the timeslice time
   and register for the timer interrupt.  
*/
void rex_timeslice_init(void)
{
   DALResult err = DAL_SUCCESS;
   DalTimetickTime64Type matchval;
   
   
   /* Init and enable QTIMER Frame 0 */ 
   DalTimetick_Attach("HeartbeatTimer", &p_heartbeat_handle);
  
   if (p_heartbeat_handle == NULL)
   {
      REXC_ERR_FATAL("Heartbeat handle failed",__LINE__);
   } 

   /* Enable Heartbeat Timer */
   DalTimetick_Enable(p_heartbeat_handle, 1);
   DalTimetick_InitTimetick64(p_heartbeat_handle);
   
   
   /* Register for QTIMER Frame 0 ISR & Provide rex_timeslice_isr as callback */
   err = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &p_interrupt_handle);
   if( err != DAL_SUCCESS || p_interrupt_handle == NULL )
   {
      /* Error out, we should not start up timer task if we cannot set interrupts through DAL */
      REXC_ERR_FATAL("timer_init: Could not obtain a Dal Interrupt Handle",__LINE__);
   }

   /* Install the timer expiry ISR, moved from timer_task to here */
   /* was DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER */
   err = DalInterruptController_RegisterISR(p_interrupt_handle,
                                            (DALInterruptID)0x0/*TODO*/,
                                            (DALISR)rex_timeslice_isr,
                                            NULL,
                                            DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER);
   if ( err != DAL_SUCCESS )
   {
      REXC_ERR_FATAL("Cannot register ISR",__LINE__);
   }
   
   /* Program Heartbeat Timer with CVAL = REX_TIMESLICE_LENGTH_TICKS */
   DalTimetick_SetNextInterrupt64(p_heartbeat_handle,
                                   (DalTimetickTime64Type)REX_TIMESLICE_LENGTH_TICKS,
                                   (DalTimetickTime64Type)0x0 /*ignored*/,
                                   &matchval);
}
#endif // FEATURE_REX_TIMESLICING && FEATURE_REX_TLS

void rex_register_err_handler(rex_err_handler_type handler)
{
   rex_err_fatal_handler = handler;
}

void rex_invoke_err_fatal_handler(void *arg)
{
   if (rex_err_fatal_handler == NULL)
   {
       REXC_ERR_FATAL("Fatal Error inovked with no Handler", __LINE__);
   }
   
   rex_err_fatal_handler(arg);
}

/*===========================================================================
FUNCTION REX_TRY_ENTER_CRIT_SECT
===========================================================================*/
boolean rex_try_enter_crit_sect(
  rex_crit_sect_type *crit_sect
)
{
  REX_INTLOCK ();

   /* mark the TCB with the critical section that we want */
   ASSERT( rex_curr_task->cs_sp <
           rex_curr_task->cs_stack + REX_CRIT_SECT_MAX - 1 );

   /* If this task already owns this critical section, just increment the
   ** lock count and return.
   */
   if ( crit_sect->lock_count > 0  &&
        rex_curr_task == crit_sect->owner)
   {
      ASSERT( crit_sect->lock_count < 0xff );
      crit_sect->lock_count++;
   }
   else
   {
      /* If it's unlocked, we simply lock it and proceed */
      if ( crit_sect->lock_count == 0 )
      {
         ++rex_curr_task->cs_sp;
         *rex_curr_task->cs_sp = crit_sect;

         crit_sect->lock_count = 1;
         crit_sect->owner      = rex_curr_task;

         crit_sect->tcb_link   = REX_CRIT_SECT_FLAG;

         /* If we bump the task's priority up to prevent priority inversion,
         ** we need to be able to restore it to its original priority once
         ** done.
         */
         crit_sect->orig_pri = rex_curr_task->pri;
      }
      /* not available, just return */
      else
      {
        REX_INTFREE ();
        return FALSE;
      }
   }

   REX_INTFREE ();
   return TRUE;
} /* END rex_try_enter_crit_sect */

/*===========================================================================
FUNCTION REX_SET_BEST_TASK

DESCRIPTION
  This function searches a portion of the TCB list to find the highest
  priority TCB which has is eligible for scheduling.  Since REX is
  pre-emptive, normally the highest priority non-blocked task will be
  running at any given time (except for a few situations like TASKLOCK).
  As such, normally one can start traversing the TCB list at the first task
  of lower priority than the one that was just blocked.  rex_set_best_task()
  accepts a pointer to a TCB as an argument, and will begin searching the
  list at the next TCB after the argument.  It will set rex_best_task to
  the first (and therefore highest priority) task it finds that is not
  blocked awaiting a resource.

  There are three conditions that need to be met for a task to be runnable.

  1. The task is not blocked on a rex_wait() function call.  This will be
     true if the task's TCB->wait variable is zero.

  2. The task is not blocked on a rex_crit_sect_enter() function call. This
     will be true if the task's TCB->cs_link.next_ptr is NULL.

  3. The task was not created in the suspended mode.  This will be true if the
     task's TCB->suspended flag is false.

  This function will call the macro REX_TASK_RUNNABLE to test the three
  conditions.

  Note that a given task can be blocked on only one of these constructs at
  a time.  For example, if the task comes up in the SUSPENDED mode, then
  it will never get the change to execute either rex_wait() or rex_enter_crit_
  section().  As such, at various places throughout REX, when a condition is
  met, there isn't a need to check for the other conditions before making a
  scheduling decision.  For example, in rex_set_sigs() a task that was blocked
  on a rex_wait() call is potentially woken up.  The code that does the wakeup
  needs only look at the tasks "sigs" and "wait" fields in the TCB to decide.
  The fact that the task was blocked on rex_wait() means that it could not
  be SUSPENDED nor could it be blocked on a critical section.


DEPENDENCIES
  This function is for *internal* REX use only, and furthermore, the calling
  function is required to have locked interrupts before making the call.

RETURN VALUE
  void

SIDE EFFECTS
  rex_best_task is modified.
===========================================================================*/
void rex_set_best_task(
  rex_tcb_type *start_tcb
)
{
   rex_tcb_type *candidate_task;

   ASSERT( start_tcb != NULL );

   candidate_task = start_tcb->link.next_ptr;

   ASSERT( candidate_task != NULL );

   /* find first runnable task */
   while ( REX_TASK_RUNNABLE(candidate_task) == FALSE)
   {
      candidate_task = candidate_task->link.next_ptr;
      ASSERT( candidate_task != NULL );
   }

   rex_best_task = candidate_task;
   return;
} /* END rex_set_best_task */


/*===========================================================================
FUNCTION REX_INIT_CRIT_SECT

DESCRIPTION
  Initializes a critical section.  Must be done prior to use.

DEPENDENCIES
  none

RETURN VALUE
  void

SIDE EFFECTS
  none
===========================================================================*/
void rex_init_crit_sect(
  rex_crit_sect_type *crit_sect
)
{
   crit_sect->lock_count = 0;
   crit_sect->tcb_link   = REX_CRIT_SECT_FLAG;
   crit_sect->orig_pri   = (rex_priority_type)-1;
   crit_sect->owner      = REX_CRIT_SECT_FLAG;
   crit_sect->magic      = REX_CRIT_SECT_MAGIC;
   return;
} /* END rex_init_crit_sect */


/*===========================================================================
FUNCTION REX_ENTER_CRIT_SECT

DESCRIPTION
  Acquires a critical section and proceeds.  If the critical section is
  locked by another task, puts the calling task onto a queue to be woken up
  later when the owning task releases it.

DEPENDENCIES
  critical section must have been initialized

RETURN VALUE
  void

SIDE EFFECTS
  The calling task will be suspended if the critical section is not
  available.
===========================================================================*/
void rex_enter_crit_sect(
  rex_crit_sect_type *crit_sect
)
{
   if (rex_curr_task)
   {
      //ASSERT( !TASKS_ARE_LOCKED( ) );
      //ASSERT( !rex_is_in_irq_mode( ) );
   
      REX_INTLOCK();
   
      /* mark the TCB with the critical section that we want */
      ASSERT( rex_curr_task->cs_sp <
              rex_curr_task->cs_stack + REX_CRIT_SECT_MAX - 1 );
      ASSERT(crit_sect->magic == REX_CRIT_SECT_MAGIC);
      /* If this task already owns this critical section, just increment the
      ** lock count and return.
      */
      if ( crit_sect->lock_count > 0  &&
           rex_curr_task == crit_sect->owner)
      {
         ASSERT( crit_sect->lock_count < 0xff );
         crit_sect->lock_count++;
      }
      else
      {
         ++rex_curr_task->cs_sp;
         *rex_curr_task->cs_sp = crit_sect;
   
         /* If it's unlocked, we simply lock it and proceed */
         if ( crit_sect->lock_count == 0 )
         {
            crit_sect->lock_count = 1;
            crit_sect->owner      = rex_curr_task;
   
            crit_sect->tcb_link   = REX_CRIT_SECT_FLAG;
   
            /* If we bump the task's priority up to prevent priority inversion,
            ** we need to be able to restore it to its original priority once
            ** done.
            */
            crit_sect->orig_pri = rex_curr_task->pri;
         }
         else
         {
            /* Add this TCB to the list attached to the crit section variable.
            ** Keep it simple, just put in on the front.
            */
   
            /* previous first element's prev_ptr needs to point to us */
            if (crit_sect->tcb_link != REX_CRIT_SECT_FLAG)
            {
               crit_sect->tcb_link->cs_link.prev_ptr = rex_curr_task;
            }
   
            /* set our next ptr to what the crit sect var previously pointed to */
            rex_curr_task->cs_link.next_ptr = crit_sect->tcb_link;
   
            /* we are the new head, so make crit sect var point to us, and set
            ** our prev_ptr to the flag value.
            */
            crit_sect->tcb_link = rex_curr_task;
            rex_curr_task->cs_link.prev_ptr = REX_CRIT_SECT_FLAG;
   
            /* If the requesting task is of higher priority than the task that
            ** holds the critical section, then to prevent priority inversion,
            ** we are going to temporarily bump the holding task's priority up
            ** to the priority of the requesting task long enough for it to
            ** release the crit section.
            */
            if ( rex_curr_task->pri > crit_sect->owner->pri )
            {
               /* besides raising priority of owner of crit_sect, the original priority
                 * of the critical sections in the owner thread's crit sect stack
                 * which are on top of this critical section will be bumped up to caller
                 * thread's priority. This has to be done to prevent "multiple
                 * critical section priority inversion issue". */
   
               rex_crit_sect_type **cs_sp_parser;
               rex_crit_sect_type *prev_crit_sect;
                  
               cs_sp_parser = crit_sect->owner->cs_sp;
               prev_crit_sect = *cs_sp_parser;
   
               /* while (not finding the crit_sect in the cs_stack) && (we are still in the cs_stack) */
               while ((prev_crit_sect != crit_sect) && (cs_sp_parser >= crit_sect->owner->cs_stack))                                                
               {
                  if ((NULL != prev_crit_sect ) && ((rex_crit_sect_type *)REX_CRIT_SECT_FLAG != prev_crit_sect))
                  {
                     if (prev_crit_sect->owner == crit_sect->owner)
                     {
                        prev_crit_sect->orig_pri = rex_curr_task->pri;
                     }
                  }
                  else
                  {
                     /* ASSERT, something wrong */
                     ASSERT("invalid crit_sect in cs_stack");
                  }                  
                     
                  /* get next crit_sect in the stack; */
                  prev_crit_sect = *(--cs_sp_parser);
               }
   
               /* we could not find the crit_sect in the cs_stack */
               if (cs_sp_parser < crit_sect->owner->cs_stack)
               {
                  /* ASSERT, something wrong */
                  ASSERT("Could not find crit_sect in cs_stack");
               }
               (void) rex_task_pri( crit_sect->owner, rex_curr_task->pri );
            }
   
            /* Tell Dog to stop monitoring this task.
            */
            REX_PAUSE_DOG_MONITOR( rex_curr_task );
   
            /* Schedule the best runnable task.
            */
            rex_set_best_task( REX_TASK_LIST_FRONT() );
            rex_sched();
         }
      }
   
      REX_INTFREE();
   }
   return;
} /* END rex_enter_crit_sect */


/*===========================================================================
FUNCTION REX_LEAVE_CRIT_SECT_INTERNALS

DESCRIPTION
  This function releases a critical section for a specified task.  Normally
  one would expect a calling task to release only a critical section that
  it owned.  However, suppose task A calls rex_kill_task() to kill task B.
  If task B is holding a critical region, the rex_kill_task() function needs
  to release that region, otherwise other tasks waiting on that region could
  be blocked forever.

  When rex_kill_task() calls this function, it does not wish to immediately
  invoke a possible rex_sched() call.  So a flag is provided as a parameter
  to specify whether rescheduling is allowed.

  An API version of this function, which only allows a task to release a
  critical region that it owns, follows.

DEPENDENCIES
  This function is for *internal* REX use only.

  The calling function must lock interrupts prior to calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  If the resched_allowed parameter is set to true, a task swap may occur.

===========================================================================*/
static void rex_leave_crit_sect_internals(
  rex_crit_sect_type *crit_sect,
  rex_tcb_type       *p_tcb,
  boolean             resched_allowed
)
{
   rex_tcb_type *index_ptr = NULL;
   rex_tcb_type *save_ptr  = NULL;
   rex_priority_type best_pri;
   ASSERT(crit_sect->magic == REX_CRIT_SECT_MAGIC);
   /* No INTLOCK - calling function is responsible */

   /* basic sanity check
   */
   if ( (p_tcb->cs_sp < p_tcb->cs_stack) ||
        (crit_sect->owner != p_tcb)      ||
        (p_tcb->cs_link.next_ptr != NULL) )
   {
      /* calling task is not holding this lock */
      ASSERT( 0 );
      return;
   }

   /* If this critical section has been taken more than once, just decrement
   ** the lock count and return.
   */
   if ( crit_sect->lock_count > 1 )
   {
      --crit_sect->lock_count;
      return;
   }

   if (*p_tcb->cs_sp != crit_sect)
   {
      /* Caller is trying to release critical sections in something other
      ** than LIFO order. We do not support this.
      */
      ASSERT( 0 );
      return;
   }

   /* mark that we are no longer holding/waiting-in this crit section */
   --p_tcb->cs_sp;

   /* if our priority got changed as part of priority-inversion-prevention,
   ** then change it back.
   */
   if ( crit_sect->orig_pri != p_tcb->pri )
   {
      (void) rex_task_pri( p_tcb, crit_sect->orig_pri );
   }

   /* If there are no waiting tasks, we can simply unlock and proceed */
   if ( crit_sect->tcb_link == REX_CRIT_SECT_FLAG )
   {
      crit_sect->lock_count = 0;
      crit_sect->owner      = REX_CRIT_SECT_FLAG;
      crit_sect->orig_pri   = (rex_priority_type)-1;
   }
   else
   {
      /* Find the highest priority task on the list attached to the crit
      ** sect variable and update the critical section variable and the
      ** task's TCB to indicate that this task now owns the critical section.
      ** Note that the list traversed is not the global TCB list that we
      ** normally traverse when looking for a task.
      */
      best_pri = 0;

      for( index_ptr = crit_sect->tcb_link;
           index_ptr != REX_CRIT_SECT_FLAG;
           index_ptr = index_ptr->cs_link.next_ptr
         )
      {
         if ( index_ptr->pri > best_pri )
         {
            best_pri = index_ptr->pri;
            save_ptr = index_ptr;
         }
      }

      /* Remove item from list */
      if ( save_ptr == NULL )
      {
         REXC_ERR_FATAL("rex_leave_crit_sect_internals() Failure: save_ptr == NULL", __LINE__);
      }

      /* If found task is the first on the list, fix up list head */
      if ( save_ptr->cs_link.prev_ptr == REX_CRIT_SECT_FLAG )
      {
         crit_sect->tcb_link = save_ptr->cs_link.next_ptr;
      }
      else  /* fix up previous element */
      {
         save_ptr->cs_link.prev_ptr->cs_link.next_ptr =
                   save_ptr->cs_link.next_ptr;
      }

      /* look to next element*/

      /* if found task is NOT the last item on the list */
      if ( save_ptr->cs_link.next_ptr != REX_CRIT_SECT_FLAG )
      {
         save_ptr->cs_link.next_ptr->cs_link.prev_ptr =
                   save_ptr->cs_link.prev_ptr;
      }

      /* Set this task's pointers to show that the task is not in the list .
      ** (Recall that the scheduler uses  these to determine if a task is
      ** blocked on a crit section or not )
      */
      save_ptr->cs_link.next_ptr = save_ptr->cs_link.prev_ptr = NULL;

      /* mark the crit section appropriately */
      crit_sect->owner    = save_ptr;
      crit_sect->orig_pri = save_ptr->pri;

      /* Tell Dog to resume monitoring the found task.
      */
      REX_RESUME_DOG_MONITOR( save_ptr );

      /* Determine if the found task should be swapped in or not */

      /* Always compare with REX_BEST_TASK, not REX_CURR_TASK! */
      if ( ( resched_allowed                    ) &&
           ( save_ptr->pri > rex_best_task->pri ) &&
           ( !save_ptr->suspended               )  )
      {
         rex_best_task = save_ptr;
         rex_sched();
      }

   }

   /* No INTFREE - calling function is responsible */
   return;
} /* END rex_leave_crit_sect_internals */


/*===========================================================================
FUNCTION REX_LEAVE_CRIT_SECT

DESCRIPTION
  Allows a task to release a critical section that it is holding.  This is
  the API version of rex_leave_crit_sect_internals.  It is the only version that
  should be called outside of REX.

DEPENDENCIES
  None

RETURN VALUE
  void

SIDE EFFECT
  If a higher priority task is waiting on the critical section, then when
  this task releases that critical section, the higher priority task will
  be swapped in.
===========================================================================*/
void rex_leave_crit_sect(
   rex_crit_sect_type *crit_sect
)
{
   if (rex_curr_task)
   {
      //ASSERT( !rex_is_in_irq_mode( ) );
   
      REX_INTLOCK();
   
      /* call "real" routine, tell it that it's OK to reschedule */
      rex_leave_crit_sect_internals( crit_sect, rex_curr_task, TRUE );
      REX_INTFREE();
   }
   return;
} /* END rex_leave_crit_sect */

/*===========================================================================
FUNCTION REX_DEL_CRIT_SECT

DESCRIPTION
  Deletes the critical section.  Implementation depends on underlying OS.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rex_del_crit_sect(
   rex_crit_sect_type *crit_sect
)
{
    ASSERT(crit_sect != NULL);
    ASSERT(crit_sect->owner == REX_CRIT_SECT_FLAG);
    ASSERT(crit_sect->tcb_link == REX_CRIT_SECT_FLAG);
    crit_sect->magic = 0;
}



/*===========================================================================
FUNCTION REX_CLR_SIGS

DESCRIPTION
  Clears the p_sigs from the tcb passed in.

DEPENDENCIES
  p_tcb is a valid task control block.

RETURN VALUE
  The previous signal mask.

SIDE EFFECTS
  The specified signals will be cleared for the specified task
===========================================================================*/
rex_sigs_type rex_clr_sigs(
   rex_tcb_type   *p_tcb,        /* tcb for which the signals will be cleared */
   rex_sigs_type   p_sigs        /* which signals to clear                    */
)
{
   rex_sigs_type prev_sigs = 0;  /* holds the signals prior to the clear */

   /*-------------------------------------------------------
   ** Save current signals for the return value and then
   ** clear the signals listed in p_sigs
   **-----------------------------------------------------*/
   REX_INTLOCK();

   p_sigs = ~p_sigs;

   prev_sigs  = p_tcb->sigs;
   p_tcb->sigs = p_tcb->sigs & p_sigs;

   REX_INTFREE();

   return prev_sigs;
} /* END rex_clr_sigs */


/*===========================================================================
FUNCTION REX_SET_SIGS

DESCRIPTION
  Sets the signals for a task. If the task is waiting for one or more of the
  set signals and it is of higher priority then the current task, a task
  swap is induced.

DEPENDENCIES
   p_tcb must be a valid task control block.

RETURN VALUE
  The sigs prior to the set action.

SIDE EFFECTS
  Changes a tasks signals which may induce a task swap.
===========================================================================*/
rex_sigs_type rex_set_sigs(
   rex_tcb_type   *p_tcb,          /* tcb for which the sigs will be set */
   rex_sigs_type   p_sigs          /* the sigs to set                    */
)
{
   rex_sigs_type prev_sigs = 0;    /* the sigs before setting */

   REX_INTLOCK();

   prev_sigs = p_tcb->sigs;

   p_tcb->sigs = p_tcb->sigs | p_sigs;

   /*-------------------------------------------------------
   ** Check and see if the p_tcb task was waiting for the
   ** sigs. If so, clear out the wait sigs and reschedule
   ** if the task is the new best task
   **-----------------------------------------------------*/

   if((p_tcb->wait & p_sigs) != 0)
   {
      p_tcb->wait = 0;

      /* Tell Dog to resume monitoring this task.
      */
      REX_RESUME_DOG_MONITOR( p_tcb );

      if ((p_tcb->pri > rex_best_task->pri) && REX_TASK_RUNNABLE(p_tcb))
      {
         rex_best_task = p_tcb;
         rex_sched();
      }
   }

   REX_INTFREE();

   return prev_sigs;
} /* END rex_set_sigs */


/*===========================================================================
FUNCTION REX_GET_SIGS

DESCRIPTION
  Returns the current signatures for a task.

DEPENDENCIES
  p_tcb is a valid task control block

RETURN VALUE
  The current sigs for a task.

SIDE EFFECTS
  None.
===========================================================================*/
rex_sigs_type rex_get_sigs(
  rex_tcb_type *p_tcb           /* tcb for which sigs will be returned */
)
{
   return p_tcb->sigs;
} /* END rex_get_sigs */


/*===========================================================================
FUNCTION REX_WAIT

DESCRIPTION
  Suspends a task until the one or more of the signals in the mask are set for
  the current task.

DEPENDENCIES
  None.

RETURN VALUE
  The signals that were set for the task.

SIDE EFFECTS
  Will cause a task swap if the signals for the task are not already set at
  the time of the call.
===========================================================================*/
rex_sigs_type rex_wait(
  rex_sigs_type p_sigs          /* sigs to wait on */
)
{
   rex_sigs_type sigs = 0;

   ASSERT( !rex_is_in_irq_mode( ) );
   ASSERT( !TASKS_ARE_LOCKED( ) );

   REX_INTERCEPT_WAIT( p_sigs );

   REX_INTLOCK();

   /*-------------------------------------------------------
   ** if no signals are set,  wait, and reschedule
   **-----------------------------------------------------*/
   if( (rex_curr_task->sigs & p_sigs) == 0 )
   {
      /*-------------------------------------------------------
      ** Set the sigs the task should wait upon
      **-----------------------------------------------------*/
      rex_curr_task->wait = p_sigs;

      /* Tell Dog to stop monitoring this task.
      */
      REX_PAUSE_DOG_MONITOR( rex_curr_task );

      /*-------------------------------------------------------
      ** Find the next best_task and schedule it.  We know
      ** that it will be a lower priority task than the one
      ** we just blocked, because a higher priority task that
      ** became ready would have already pre-empted the one
      ** we are blocking.
      **-----------------------------------------------------*/
      rex_set_best_task( REX_TASK_LIST_FRONT() );
      rex_sched();
   }
   /*-------------------------------------------------------
   ** return with the signal mask of the current task
   **-----------------------------------------------------*/
   sigs = rex_curr_task->sigs;
   REX_INTFREE();
   return sigs;
} /* END rex_wait */


/*===========================================================================
FUNCTION REX_IDLE_TASK

DESCRIPTION
  The lowest possible task priority (0) and runs only when no other
  task is ready to run.

DEPENDENCIES
  rex_def_task should spawn this task

RETURN VALUE
  None

SIDE EFFECTS
  this task never suspends, waits or returns.
===========================================================================*/
/*lint -e715 arg not referenced */
/*lint -e527 Unreachable */
void rex_idle_task(
   dword arg
)
{
   for(;;){}
} /* END rex_idle_task */

/*lint +e527 Unreachable */
/*lint +e715 arg not referenced */

/*===========================================================================
FUNCTION REX_TASK_PREAMBLE

DESCRIPTION
  Special Note: rex_task_preamble is an internal function that should be called
  only from rex service functions. It should not be called directly by user
  software.
  rex_task_preamble is a helper function which is the first thing to execute in
  every newly created task. Upon receiving control, rex_task_preamble merely
  calls the user-defined entry point for the new task, passing it the user-
  specified parameter for the new task.

DEPENDENCIES
  assumes that function rex_def_task has set up the correct
  context frame on the stack for the new task.
RETURN VALUE
  None. This function does not return.********
SIDE EFFECTS
  None
===========================================================================*/
void rex_task_preamble(
   void (*func_ptr)( dword arg ),
   dword arg
)
{
#if defined(FEATURE_REX_TIMESLICING) && defined(FEATURE_REX_TLS)
   /* Set default timeslice value for this thread.  It can be changed later */
   rex_tls_setspecific(rex_slices_key, (rex_tls_value_t)REX_DEFAULT_SLICES);
#endif
   
   func_ptr( arg );

   /* if we return, kill the task */
   rex_kill_task( rex_self() );

} /* END rex_task_preamble */


/*===========================================================================
FUNCTION REX_DEF_TASK/REX_DEF_TASK_EXT/REX_DEF_TASK_INTERNAL

DESCRIPTION
  Define a new rex task.

DEPENDENCIES
  p_tcb must be a valid tcb block.
  p_task must be a valid function pointer
  p_stack must be on an aligned boundary and the size must be the number
  of bytes in the stack

  Should only be called by other REX functions.

RETURN VALUE
  None.

SIDE EFFECTS
  Defines a new rex task that will be placed in the task list.
  Creates a new thread and an event for the new task.
  The task starts running only if it is the highest priority ready task.
===========================================================================*/
void rex_def_task_internal(
  rex_tcb_type            *p_tcb,             /* valid tcb for new task         */
  rex_stack_pointer_type   p_stack,            /* stack for new task             */
  rex_stack_size_type      p_stksiz,           /* stack size in bytes            */
  rex_priority_type        p_pri,              /* priority for new task          */
  rex_task_func_type       p_task,             /* task startup function          */
  rex_task_func_param_type p_param,            /* parameter for new task         */
  char                    *p_tskname,         /* A/N string for task name       */
  boolean                  p_susp,             /* is task initially suspended?   */
  void                    *p_parent,          /* opaque handle to container     */
  boolean                  p_dynamic,          /* stack/tcb alloc'd via dyna mem */
  int                      dog_report_val      /* Dog report value               */
)
{
   word                   index      = 0;
   byte                   *stack_ptr = NULL;

   if(rex_num_tasks == REX_MAX_TASKS)
   {
      REXC_ERR_FATAL("Trying to create more than REX_MAX_TASKS", __LINE__);
   }

   /* Memset the stack to magic value 0xDDDDDDDD to check stack usage */
   memset((void *)p_stack, 0xDD, p_stksiz);

   /*
    * Align to 32-byte boundary to support MPU.
    */
   if((unsigned long)p_stack % AOP_MPU_STACK_GUARD_SIZE)
   {
      unsigned long alignment = AOP_MPU_STACK_GUARD_SIZE - ((unsigned long)p_stack % AOP_MPU_STACK_GUARD_SIZE);

      p_stack   = (void*)(((unsigned long)p_stack + alignment));
      p_stksiz -= alignment;
   }

   /*
    * The top 32-byte of the stack is used as stack guard region by MPU to detect stack overflow.
    */
   p_stack   = (void*)((unsigned long)p_stack + AOP_MPU_STACK_GUARD_SIZE);
   p_stksiz -= AOP_MPU_STACK_GUARD_SIZE;

   /*-------------------------------------------------------
   ** Task stack pointer points to the bottom of allocated
   ** stack memory. p_stksiz is the number of 8-bit bytes.
   **-----------------------------------------------------*/
   stack_ptr = (byte *)((unsigned long)p_stack + (unsigned long)p_stksiz - sizeof(unsigned long) );

#if 0
   /* Make sure that stack_ptr and p_stack are both double-word
   ** aligned
   */
   temp_diff = (unsigned long) stack_ptr & 0x07;
   if (temp_diff)
   {
      stack_ptr = (byte *) ((unsigned long) stack_ptr - temp_diff);
      p_stksiz -= (rex_stack_size_type) temp_diff;
   }

   temp_diff = (unsigned long) p_stack & 0x07;
   if (temp_diff)
   {
      p_stack = (byte *) ((unsigned long)p_stack + (8 - temp_diff));
      p_stksiz -= (rex_stack_size_type) temp_diff;
   }
#endif

   /*-------------------------------------------------------
   ** Creates room for the context.
   ** sp points to the top of the context frame.
   **-----------------------------------------------------*/

   /*-------------------------------------------------------
   ** Defines the initial context.
   **-----------------------------------------------------*/
   stack_ptr = rex_init_stack(stack_ptr, p_task, p_param);

#if 0
   rex_context_frame_type *cf_ptr    = NULL;
   stack_ptr -= sizeof( rex_context_frame_type );
   cf_ptr              = (rex_context_frame_type*)stack_ptr;
	  cf_ptr->spsr.val    = PSR_Supervisor
#if defined(__thumb) || defined(__thumb_thumb__)
                                           | PSR_Thumb; // Needs to match the compile mode of this module
#else
   ;
#endif
   cf_ptr->r[0].task   = p_task;
   cf_ptr->r[1].arg    = p_param;
   cf_ptr->r[10].arg   = (unsigned long)p_stack;
   cf_ptr->lr.preamble = rex_task_preamble;
   cf_ptr->pc.preamble = rex_task_preamble;
#endif
   
   /* ------------------------------------------------------
   ** Initialize the task control block (TCB)
   ** ------------------------------------------------------ */
   p_tcb->sp               =      stack_ptr;
   p_tcb->stack_limit      =      p_stack;
   p_tcb->stack_size       =      p_stksiz;
   p_tcb->max_stack_usage  =      0;
   p_tcb->slices           =      0;
   p_tcb->sigs             =      0;
   p_tcb->wait             =      0;
   p_tcb->pri              =      p_pri;
   p_tcb->cs_link.next_ptr =      NULL;
   p_tcb->cs_link.prev_ptr =      NULL;
   p_tcb->cs_sp            =      p_tcb->cs_stack;   --p_tcb->cs_sp;
   p_tcb->suspended        =      p_susp;
   p_tcb->intlock_nest_level =    0;
   p_tcb->thread_id     =      (unsigned long)p_tcb;

   p_tcb->task_name[REX_TASK_NAME_LEN] = '\0';
   if (p_tskname != NULL) /* copy task name if one was supplied */
   {
      /* copy bytes until /0 received or enough chars have been copied */
      while ( p_tskname[index] && index < REX_TASK_NAME_LEN  )
      {
         p_tcb->task_name[index] = p_tskname[index];
         index++;
      }
      p_tcb->task_name[index] = '\0';
   }

   p_tcb->dog_report_val  = dog_report_val;
   p_tcb->autodog_enabled = FALSE;


   /*-------------------------------------------------------
   ** We are done if this is the idle task (kernel task) defined.
   **-------------------------------------------------------*/
   if (p_pri == 0) {
      return;
   }

   /*-------------------------------------------------------
   ** Find the first task whose priority is lower than the
   ** new task and insert the new task in front of the
   ** lower priority task.
   ** rex_idle_task (the kernel task) is at the end of the list
   ** with priority 0 and therefore no need to check for null ptrs.
   **-----------------------------------------------------*/
   REX_INTLOCK ();
   {
      rex_tcb_type *tcb_ptr;

      tcb_ptr = rex_task_list.link.next_ptr;

      /* The rex_kernel_tcb check is present in the following loop
      ** condition because the priority of the idle task is temporarily
      ** bumped up in rex_init to prevent scheduling. Tasks defined before
      ** the idle task's priority is reset to 0 can then cause tcb_ptr
      ** to go off the end of the list.
      */
      while ((tcb_ptr->pri > p_pri) && (tcb_ptr != &rex_kernel_tcb)) {
         tcb_ptr = tcb_ptr->link.next_ptr;
      }

      p_tcb->link.prev_ptr = tcb_ptr->link.prev_ptr;
      p_tcb->link.next_ptr = tcb_ptr;
      tcb_ptr->link.prev_ptr->link.next_ptr = p_tcb;
      tcb_ptr->link.prev_ptr = p_tcb;
   }

   /*---------------------------------------------------
   ** Make this task the best task if it is higher
   ** priority than the present best task.
   **---------------------------------------------------*/
   
   /* If this is the first task in the system (besides idle/stat), schedule it */
   if (!rex_first_task_scheduled && rex_is_initialized())
   {
      rex_best_task = p_tcb;
      rex_curr_task = rex_best_task;
      rex_num_tasks++;
      rex_start_task(rex_best_task);
   }

   /* Always compare with REX_BEST_TASK, not REX_CURR_TASK */
   if  ( (p_pri > rex_best_task->pri)  && (p_tcb->suspended == FALSE) )
   {
      rex_best_task = p_tcb;

      /* swap the task in */
      if (rex_is_initialized())
         rex_sched();
   }

   rex_num_tasks++;

   REX_INTFREE ();
   return;
} /* END rex_def_task_internal */


/* rex_def_task_ext takes two parameters in addition to the ones taken by
** rex_def_task. These allow the user to specify a name for the task and
** whether the task should be started in suspended mode or not. Note also
** that rex_def_task_ext treats stacks as arrays of characters and expects
** the stack size to be specified in units of bytes.
*/
void rex_def_task_ext(
   rex_tcb_type            *p_tcb,              /* valid tcb for new task       */
   rex_stack_pointer_type   p_stack,            /* stack for new task           */
   rex_stack_size_type      p_stksiz,           /* stack size in bytes          */
   rex_priority_type        p_pri,              /* priority for new task        */
   rex_task_func_type       p_task,             /* task startup function        */
   rex_task_func_param_type p_param,            /* parameter for new task       */
   char                    *p_tskname,          /* A/N string for task name     */
   boolean                  p_susp              /* is task initially suspended? */
)
{
   /* In this case, stacks are passed as arrays of bytes with the size
   ** indicating the number of bytes.  The adjustment of the stack pointer
   ** is to assure alignment.
   */
   void* adj_stack_ptr = NULL;

   ASSERT( !rex_is_in_irq_mode( ) );

   adj_stack_ptr = ((unsigned long)p_stack%4)?
      (void*)(((unsigned long)p_stack+(sizeof(long)) - (unsigned long)p_stack%4)):p_stack;

   rex_def_task_internal(
      p_tcb,
      (unsigned char*)adj_stack_ptr,
      p_stksiz - ((unsigned long)adj_stack_ptr - (unsigned long)p_stack),
      p_pri,
      p_task,
      p_param,
      p_tskname,
      p_susp,
      NULL,
      FALSE,
      -1
   );
   return;
} /* END rex_def_task_ext */


/* rex_def_task_ext2 takes one parameter in addition to the ones taken by
** rex_def_task_ext. This parameter allows the user to specify a dog report
** value for the task. Just as with rex_def_task_ext, the stack size should
** be specified in units of bytes.
*/
void rex_def_task_ext2(
   rex_tcb_type            *p_tcb,              /* valid tcb for new task       */
   rex_stack_pointer_type   p_stack,            /* stack for new task           */
   rex_stack_size_type      p_stksiz,           /* stack size in bytes          */
   rex_priority_type        p_pri,              /* priority for new task        */
   rex_task_func_type       p_task,             /* task startup function        */
   rex_task_func_param_type p_param,            /* parameter for new task       */
   char                    *p_tskname,          /* A/N string for task name     */
   boolean                  p_susp,             /* is task initially suspended? */
   int                      dog_report_val      /* Dog report value             */
)
{
   /* In this case, stacks are passed as arrays of bytes with the size
   ** indicating the number of bytes.  The adjustment of the stack pointer
   ** is to assure alignment.
   */
   void* adj_stack_ptr = NULL;

   ASSERT( !rex_is_in_irq_mode( ) );

   adj_stack_ptr = ((unsigned long)p_stack%4)?
      (void*)(((unsigned long)p_stack+(sizeof(long)) - (unsigned long)p_stack%4)):p_stack;

   rex_def_task_internal(
      p_tcb,
      (unsigned char*)adj_stack_ptr,
      p_stksiz - ((unsigned long)adj_stack_ptr - (unsigned long)p_stack),
      p_pri,
      p_task,
      p_param,
      p_tskname,
      p_susp,
      NULL,
      FALSE,
      dog_report_val
   );
   return;
} /* END rex_def_task_ext2 */

/* rex_def_task: original task definition function.
*/
void rex_def_task(
   rex_tcb_type            *p_tcb,              /* valid tcb for new task       */
   void *                   p_stack,            /* stack for new task           */
   rex_stack_size_type      p_stksiz,           /* stack size in units of
                                               rex_stack_word_type          */
   rex_priority_type        p_pri,              /* priority for new task        */
   void                     (*p_task)( dword ), /* task startup function        */
   rex_task_func_param_type p_param             /* parameter for new task       */
)
{
   /* In this case, stacks are passed as arrays of type rex_stack_word_type,
   ** with the size indicating the number of elements in the array. So we
   ** adjust here. The adjustment of the stack pointer is to assure
   ** alignment.
   */
   void* adj_stack_ptr = NULL;

   ASSERT( !rex_is_in_irq_mode( ) );

   adj_stack_ptr = ((unsigned long)p_stack%sizeof(rex_stack_word_type)) ?
      (void*)(((unsigned long)p_stack+(sizeof(rex_stack_word_type)) - (unsigned long)p_stack%sizeof(rex_stack_word_type))):p_stack;

   if (adj_stack_ptr != p_stack)
   {
      p_stksiz--;
   }

   rex_def_task_internal(
      p_tcb,
      (unsigned char*)adj_stack_ptr,
      p_stksiz*sizeof(rex_stack_word_type),
      p_pri,
      p_task,
      p_param,
      NULL,
      FALSE,
      NULL,
      FALSE,
      -1
   );
   return;
} /* END rex_def_task */


/*===========================================================================
FUNCTION REX_SUSPEND_TASK

DESCRIPTION
  Suspend a task so that it is not eligible for scheduling. If the task
  being suspended is the current task, then a context switch will occur.

  Note: If called to suspend the current task in IRQ mode, the suspension
  will not happen until the next time rex_best_task is set and rex_sched
  is called.

===========================================================================*/
void rex_suspend_task(
   rex_tcb_type *p_tcb
)
{
   p_tcb->suspended = TRUE;
   REX_INTLOCK();
      if ( ( p_tcb == rex_curr_task ) && !rex_is_in_irq_mode( ) )
      {
         rex_set_best_task( REX_TASK_LIST_FRONT() );
         rex_sched( );
      }
   REX_INTFREE();

   return;
} /* END rex_suspend_task */


/*===========================================================================
FUNCTION REX_RESUME_TASK

DESCRIPTION
  Make a suspended task eligible for scheduling.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  If the task being resumed is higher priority than the calling task and it
  is not blocked for some other reason, then a context switch will occur.

===========================================================================*/
void rex_resume_task(
  rex_tcb_type         *p_tcb
)
{
   REX_INTLOCK();

   /* basic sanity check to see if we should even be here or not */
   if (p_tcb->suspended == TRUE)
   {
      p_tcb->suspended = FALSE;

      if ((p_tcb->pri > rex_best_task->pri) && REX_TASK_RUNNABLE(p_tcb))
      {
         rex_best_task = p_tcb;
         rex_sched();
      }
   }

   REX_INTFREE();

   return;
} /* END rex_resume_task */


/*===========================================================================
FUNCTION REX_REMOVE_TASK

DESCRIPTION
  Function that implements the process of removing a TCB from
  the REX task list. Interrupts must be locked before calling this function.
===========================================================================*/
void rex_remove_task(
  rex_tcb_type *tcb_ptr   /* pointer to tcb */
)
{
   rex_tcb_type *prev_tcb_ptr;
   rex_tcb_type *next_tcb_ptr;

   prev_tcb_ptr = REX_TASK_LIST_PREV( tcb_ptr );
   next_tcb_ptr = REX_TASK_LIST_NEXT( tcb_ptr );

   if ( ( prev_tcb_ptr == NULL                 ||
      prev_tcb_ptr->pri != tcb_ptr->pri  ) &&
      next_tcb_ptr != NULL                 &&
      next_tcb_ptr->pri == tcb_ptr->pri     )
   {
      /* tcb_ptr is the priority representative for one or more other
      ** threads with the same priority. Once tcb_ptr is removed, the
      ** priority representative for these threads will be the thread
      ** that follows tcb_ptr in the thread list.
      */
      rex_tcb_type *temp_tcb_ptr = next_tcb_ptr;
      while ( temp_tcb_ptr->pri == tcb_ptr->pri )
      {
         temp_tcb_ptr->pri_rep_ptr = next_tcb_ptr;
         temp_tcb_ptr = REX_TASK_LIST_NEXT( temp_tcb_ptr );
      }
   }

   if ( (prev_tcb_ptr != NULL) && (next_tcb_ptr != NULL) ) 
   {
      REX_TASK_LIST_POP( tcb_ptr );
      
      tcb_ptr->link.prev_ptr = NULL;  // == prev_tcb_ptr
      tcb_ptr->link.next_ptr = NULL;  // == next_tcb_ptr
   }

   return;
} /* END rex_remove_task */


/*===========================================================================
FUNCTION REX_KILL_TASK

DESCRIPTION
  Kill a specified task by yanking its TCB from the active list.

DEPENDENCIES
  None.

RETURN VALUE
  Void in normal cases.
  If a thread calls rex_kill_task on itself, then this function will not
  return at all (as the thread goes away).

===========================================================================*/
void rex_kill_task_ext(
   rex_tcb_type *p_tcb,
   boolean       schedule_new_task
)
{

   REX_INTLOCK();
   
#ifdef FEATURE_REX_TLS
   /* invoke destructors for tls objects: cannot do within TASKLOCK because we require a mutex */
   rex_invoke_tls_destructors(p_tcb);
#endif
   
   TASKLOCK();

   /* Task is alive only if it is still linked into TCB list.
   */
   if ( (p_tcb->link.prev_ptr != NULL ) || (p_tcb->link.next_ptr != NULL) )
   {
      /* Remove TCB from the task list.
      */
      rex_remove_task( p_tcb );

#ifdef FEATURE_REX_TIME
      /* Remove REX timers associated with the task from the timer list.
      */
      rex_delete_task_timers( p_tcb );
#endif
      
      /* Tell Dog to stop monitoring this task.
      */
      REX_PAUSE_DOG_MONITOR( p_tcb );

      /* Check if we were holding or waiting on a critical section */
      while (p_tcb->cs_sp >= p_tcb->cs_stack)
      {
         if ( p_tcb->cs_link.next_ptr == NULL) /* holding crit section */
         {
            /* free the crit section, but don't call rex_sched() yet */
            rex_leave_crit_sect_internals( *p_tcb->cs_sp, p_tcb, FALSE);
         }
         else /* we were waiting on the list */
         {
            /* if item is first on the list, fix up list head */
            if (p_tcb->cs_link.prev_ptr == REX_CRIT_SECT_FLAG)
            {
               (*p_tcb->cs_sp)->tcb_link = p_tcb->cs_link.next_ptr;
            }
            else /* fix up previous item on list */
            {
               p_tcb->cs_link.prev_ptr->cs_link.next_ptr =
                  p_tcb->cs_link.next_ptr;
            }

            /* if item is NOT the last on the list */
            if (p_tcb->cs_link.next_ptr != REX_CRIT_SECT_FLAG)
            {
               p_tcb->cs_link.next_ptr->cs_link.prev_ptr =
                  p_tcb->cs_link.prev_ptr;
            }
            --p_tcb->cs_sp;
         }
      } /* END we needed to deal with crit section */

      rex_num_tasks--;

      if( schedule_new_task )
      {
         /* if the calling task is killing itself, and holding the task lock,
         ** then release the lock.
         */
         if (p_tcb == rex_curr_task)
         {
            /* If the task to be killed is holding the task lock, we need to
            ** release that lock.  It turns out that only if a task is killing
            ** itself is this situation possible. We check for this case in the
            ** next few lines of code.  To clarify, the other situation is where
            ** task A is calling rex_kill_task to kill task B.  We are guaranteed
            ** that task B does not hold the task lock, otherwise task A, which is
            ** issuing the rex_kill_task(), couldn't possibly be running.
            */
            if (rex_nest_depth > 0)
            {
               rex_nest_depth = 0;
               rex_sched_allow = TRUE;
            }
         } /* end-if task was killing itself */

         /* At this point, we've possibly given a critical section to a task that
         ** had been waiting, possibly released a TASKLOCK, and possibly had a
         ** calling task kill itself, all of which could require a scheduling change.
         ** It makes sense to search for a ready task from the top of the list.
         */
         rex_set_best_task( REX_TASK_LIST_FRONT() );
         rex_sched();
      } /* END needed to reschedule */
   } /* END TCB was still in active list */

   TASKFREE();
   REX_INTFREE();
   return;
} /* END rex_kill_task_ext */


/*===========================================================================
FUNCTION REX_KILL_TASK

DESCRIPTION
  Kill the current task by yanking its TCB from the active list.
  If a task calls rex_kill_task on itself, then this function will not
  return at all (as the task goes away).

===========================================================================*/
void rex_kill_task(
  rex_tcb_type *p_tcb
)
{
  rex_kill_task_ext( p_tcb, TRUE );
  return;
} /* END rex_kill_task */


/*===========================================================================
FUNCTION REX_SELF

DESCRIPTION
  Returns the task control block for the current task

DEPENDENCIES
  None.

RETURN VALUE
  The task control block for the current task

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type *rex_self( void )
{
   /*-------------------------------------------------------
   ** The currently running task is in rex_curr_task
   **-----------------------------------------------------*/
   return rex_curr_task;
} /* END rex_self */


/*===========================================================================
FUNCTION REX_SET_PRI

DESCRIPTION
  Sets the priority of the calling task.

DEPENDENCIES
  None.

RETURN VALUE
  The priority before it was changed.

SIDE EFFECTS
  The priority change will cause a change in the whole scheduling order for
  all of the tasks. This could result in an imediate task swap.
===========================================================================*/
rex_priority_type rex_set_pri(
   rex_priority_type p_pri       /* the new priority */
)
{
   /*-------------------------------------------------------
   ** A wrapper function that just calls rex_task_pri with
   ** the current task
   **-----------------------------------------------------*/
   return rex_task_pri(rex_curr_task, p_pri);
} /* END rex_set_pri */


/*===========================================================================
FUNCTION REX_TASK_PRI

DESCRIPTION
  Sets the priority of the passed in task.

DEPENDENCIES
  p_tcb must be a valid task control block

RETURN VALUE
  The priority before it was changed.

SIDE EFFECTS
  The priority change will cause a change in the whole scheduling order for
  all of the tasks. This could result in an imediate task swap.
===========================================================================*/
rex_priority_type rex_task_pri(
  rex_tcb_type       *p_tcb,    /* tcb to set priority on      */
  rex_priority_type  p_pri      /* the new priority            */
)
{
   rex_priority_type prev_pri   = p_tcb->pri;   /* the priority before the set */
   rex_tcb_type  *search_ptr;                /* temp pointer                */
   boolean comp = FALSE;                        /* Comparator */

   REX_INTLOCK();

   comp = (p_pri == p_tcb->pri);
   REX_INTFREE();

   /* Return if the priority is the same
   */
   if( comp )
   {
      return prev_pri;
   }

   REX_INTLOCK();

   /* Remove from the list and re insert in a new position
   ** based upon the new priority
   */
   p_tcb->link.next_ptr->link.prev_ptr = p_tcb->link.prev_ptr;
   p_tcb->link.prev_ptr->link.next_ptr = p_tcb->link.next_ptr;

   p_tcb->pri = p_pri;

   /* Find the task with the priority <= the new priority.
   ** Insert in front of this task
   ** rex_idle_task (the kernel task) is at the end of the list
   ** with priority 0 and therefore no need to check for null ptrs.
   */
   search_ptr = rex_task_list.link.next_ptr;
   while(search_ptr->pri > p_pri) {
      search_ptr = search_ptr->link.next_ptr;
   }

   p_tcb->link.prev_ptr = search_ptr->link.prev_ptr;
   p_tcb->link.next_ptr = search_ptr;
   search_ptr->link.prev_ptr->link.next_ptr = p_tcb;
   search_ptr->link.prev_ptr = p_tcb;

   /* If the task is runnable, and its priority is now higher than
   ** that of rex_best_task, then we can context switch it in.
   */
   if ( (p_pri > rex_best_task->pri) && ( REX_TASK_RUNNABLE(p_tcb) ) )
   {
      rex_best_task = p_tcb;
      rex_sched();
   }

   REX_INTFREE();
   return prev_pri;
} /* END rex_task_pri */


/*===========================================================================
FUNCTION REX_GET_PRI

DESCRIPTION
  Return the priority of the current task.

DEPENDENCIES
  None.

RETURN VALUE
  The priority of the current task.

SIDE EFFECTS
  None.
===========================================================================*/
rex_priority_type rex_get_pri( void )
{
  /*-------------------------------------------------------
  ** Just return the priority field of the current task
  **-----------------------------------------------------*/
  return rex_curr_task->pri;
} /* END rex_get_pri */

/*===========================================================================
FUNCTION REX_INIT

DESCRIPTION
  Initializes REX. It initializes the kernel task and calls the p_task main
  rex function.

DEPENDENCIES
  p_tcb must be a valid task control block.
  p_task must be a valid function pointer.

RETURN VALUE
  None.

SIDE EFFECTS
  When the initialization is done, this thread will wait until
  the exit event is triggered at which point it will clean up
  and return.
===========================================================================*/
void rex_init(
   void *              p_istack,           /* interrupt stack      */
   rex_stack_size_type p_istksiz,          /* interrupt stack size */
   rex_tcb_type        *p_tcb,             /* task control block   */
   void *              p_stack,            /* stack                */
   rex_stack_size_type p_stksiz,           /* stack size           */
   rex_priority_type   p_pri,              /* task priority        */
   void                (*p_task)( dword ), /* task function        */
   dword               p_param             /* task parameter       */
)
{

   if(rex_is_initialized())
   {
      return;
   }
   
   rex_arch_init();

#ifdef FEATURE_REX_TLS
   /* TLS Initialization */
   rex_tls_init();
   
   #ifdef FEATURE_REX_TIMESLICING   
   rex_tls_create(&rex_slices_key, NULL);
   #endif
#endif

   lock_ints ();

   /*-------------------------------------------------------
   ** Initialize the interrupt nest level at zero
   **-----------------------------------------------------*/
   rex_int_nest_level = 0;

   /*-------------------------------------------------------
   ** Initialize the task list.
   **-----------------------------------------------------*/
   rex_task_list.link.next_ptr  = &rex_kernel_tcb;
   rex_task_list.link.prev_ptr  = NULL;
   rex_kernel_tcb.link.next_ptr = NULL;
   rex_kernel_tcb.link.prev_ptr = &rex_task_list;

   REX_INTERCEPT_INIT_READY();

   /*-------------------------------------------------------
   ** rex_curr_task must be equal to rex_best_task to avoid
   ** scheduling at this moment
   **-----------------------------------------------------*/
   rex_curr_task = &rex_kernel_tcb;
   rex_best_task = &rex_kernel_tcb;


   rex_def_task_ext(
      &rex_kernel_tcb,                            /* tcb        */
      (unsigned char *) &rex_kernel_stack[0],     /* stack      */
      REX_KERNEL_STACK_SIZE,                      /* stack size */
      0,                                          /* priority   */
      rex_idle_task,                              /* function   */
      0,                                          /* arguments  */
      "REX Idle Task",                            /* task name  */
      FALSE                                       /* suspended  */
   );

   rex_kernel_tcb.pri = 0xFFFFFFFF;

   #ifdef FEATURE_REX_STAT
   rex_stat_init( );
   #endif

   rex_kernel_tcb.pri = 0;

   unlock_ints ();

   rex_init_complete = 1;

} /* END rex_init */


/*===========================================================================
FUNCTION REX_FIRST_TASK

DESCRIPTION
  Assign input iterator to first task on rex_task_list.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the first task on rex_task_list.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type * rex_first_task(
   rex_tcb_type** iter
)
{
   *iter = REX_TASK_LIST_FRONT();
   return *iter;
} /* END rex_first_task */


/*===========================================================================
FUNCTION REX_NEXT_TASK

DESCRIPTION
  Assign input iterator to the next task on rex_task_list.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the next task on rex_task_list.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type * rex_next_task(
   rex_tcb_type** iter
)
{
   *iter = REX_TASK_LIST_NEXT((*iter));
   return *iter;
} /* END rex_next_task */


/*===========================================================================
FUNCTION REX_PREV_TASK

DESCRIPTION
  Assign input iterator to the prev task on rex_task_list.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the prev task on rex_task_list.

SIDE EFFECTS
  None.
===========================================================================*/
rex_tcb_type * rex_prev_task(
   rex_tcb_type** iter
)
{
   *iter = REX_TASK_LIST_PREV((*iter));
   return *iter;
} /* END rex_prev_task */




/*===========================================================================
FUNCTION REX_ASSIGN_BEST_TASK

DESCRIPTION
  Sets the rex_best_task variable. The user must ensure that the input
  tcb is indeed the highest-priority runnable task.
  This function is for *internal* REX use only, and furthermore, the calling
  function is required to have locked interrupts before making the call.
===========================================================================*/
void rex_assign_best_task(
   rex_tcb_type *tcb_ptr
)
{
   if ( tcb_ptr != NULL )
   {
      rex_best_task = tcb_ptr;
   }
   return;
} /* END rex_assign_best_task */

/* ==================================================================
FUNCTION REX_SET_BEST_TASK_FROM_ALL

DESCRIPTION
   Searches the entire task list and sets the rex_best_task variable.

   NOTE: Interrupts must be locked when this function is called.
===================================================================== */
void rex_set_best_task_from_all( void )
{
   rex_set_best_task( REX_TASK_LIST_FRONT() );
   return;
} /* END rex_set_best_task_from_all */


/* ==================================================================
FUNCTION REX_GET_VERSION_NUMBER
DESCRIPTION
   Returns a REX version number. The version number returned is a
   decimal number in which the least significant two digits indicate
   the minor version number and the remaining digits (at most two)
   indicate the major version number. For instance, if the value
   returned is 307, the major version number is 3 and the minor version
   number is 7.
===================================================================== */
unsigned long rex_get_version_number(
   void
)
{
   return REX_VERSION_NUMBER;
} /* END rex_get_version_number */


/* ==========================================================================
FUNCTION REX_AUTODOG_ENABLE
DESCRIPTION
   Performs initialization needed to set up the auto dog service for a task.
   When auto-dog is on, the task does not need to wake up periodically to
   kick the dog when making a blocking REX call. REX takes care of notifying
   Dog that the task is blocked and that there is no need to monitor the
   task until further notice. When the task is scheduled again, Dog is
   notified that the task is ready to be monitored again.

   Returns the previous state of the autodog_enabled field in the TCB.
============================================================================= */
int rex_autodog_enable(
   int dog_report_val
)
{
   int prev_state;
   REX_INTLOCK();
      prev_state = rex_curr_task->autodog_enabled;
      rex_curr_task->dog_report_val  = dog_report_val;
      rex_curr_task->autodog_enabled = TRUE;
   REX_INTFREE();
   return prev_state;
} /* END rex_autodog_enable */


/* ==========================================================================
FUNCTION REX_AUTODOG_DISABLE
DESCRIPTION
   Disables the auto dog service for a task.

   Returns the previous state of the autodog_enabled field in the TCB.
============================================================================= */
int rex_autodog_disable( void )
{
   int prev_state;
   REX_INTLOCK();
      prev_state = rex_curr_task->autodog_enabled;
      rex_curr_task->autodog_enabled = FALSE;
   REX_INTFREE();

   return prev_state;
} /* END rex_autodog_disable */

/*==========================================================================
FUNCTION REX_GET_NUM_TASKS

DESCRIPTION
   Returns the number of currently dispatchable tasks, includes threads if
   FEATURE_REX_THREADS is on and threads are created, includes any task or
   thread regardless of its state (running, blocked, suspended, etc.)
============================================================================ */
int rex_get_num_tasks( void )
{
   return rex_num_tasks;
} /* END rex_get_num_tasks */


#ifdef FEATURE_REX_TIME
/*==========================================================================
FUNCTION REX_SLEEP

DESCRIPTION
  Blocks the current thread for a specified time. The thread becomes
  schedulable after the specified time has elapsed.  This is equivalent
  to creating a REX timer and calling rex_timed_wait on the timer with
  only the associated timer signal being waited upon
============================================================================ */
void rex_sleep(
   unsigned long ms
)
{
   boolean        sleeping = TRUE;
   boolean        sig_set  = FALSE;
   unsigned long  tmo      = ms;
   rex_timer_type sleep_timer;

   rex_def_timer(
      &sleep_timer,
      rex_self(),
      REX_SLEEP_SIG
   );

   while( sleeping )
   {
      (void) rex_timed_wait( REX_SLEEP_SIG, &sleep_timer, tmo );
      REX_INTLOCK();
         if( (tmo = rex_get_timer( &sleep_timer )) == 0 )
         {
            sleeping = FALSE;
         }
         else
         {
            sig_set = TRUE;
         }
      REX_INTFREE();
   }
   if( sig_set )
   {
      (void) rex_set_sigs( rex_self(), REX_SLEEP_SIG );
   }
   return;
} /* END rex_sleep */
#endif


/*===========================================================================
FUNCTION rex_task_lock

DESCRIPTION
  rex_task_lock prevents context switching from the point at which it is
  invoked until a matching rex_task_free is invoked.  While tasks are locked,
  the current task is guaranteed to not be replaced by a different task, even
  if the other task has a higher priority.  However, rex_task_lock does not
  block interrupts from occurring.  An ISR will pre-empt the task, but even
  if the ISR causes a higher priority task to become ready, control will
  still resume at the interrupted task.

  Note that a rex_wait() while a rex_task_lock is in effect will not work as
  normal, as the calling task will not be suspended.

  Also, rex_task_lock/rex_task_free have no effect if called from within an
  ISR.
===========================================================================*/
void rex_task_lock( void )
{
   if ( !rex_is_in_irq_mode( ) )
   {
      REX_INTLOCK();
      rex_sched_allow = FALSE;
      rex_nest_depth++;
      REX_INTFREE();
   }
} /* END rex_task_lock */

/*===========================================================================
FUNCTION rex_task_free

DESCRIPTION
  rex_task_free re-enables context switching after a rex_task_lock. Context
  switching is re-enabled if rex_task_free has been called as many times as
  rex_task_lock.

  rex_task_free has no effect if called from within an ISR.
===========================================================================*/
void rex_task_free( void )
{
   if ( !rex_is_in_irq_mode( ) )
   {
      REX_INTLOCK();
      if (rex_nest_depth > 0)
         rex_nest_depth--;
      if (rex_nest_depth == 0)
      {
         rex_sched_allow = TRUE;
         rex_sched();
      }
      REX_INTFREE();
   }
} /* END rex_task_free */

/*===========================================================================
FUNCTION rex_tasks_are_locked

DESCRIPTION
  Returns 1 if tasks are currently locked and 0 otherwise.
===========================================================================*/
int rex_tasks_are_locked( void )
{
  return !rex_sched_allow;
} /* END rex_tasks_are_locked */

/*===========================================================================
FUNCTION rex_is_initialized

DESCRIPTION
  Returns 1 if Rex has been initialized and 0 if not.
===========================================================================*/
int rex_is_initialized( void )
{
  return rex_init_complete;
} /* END rex_is_initialized */
