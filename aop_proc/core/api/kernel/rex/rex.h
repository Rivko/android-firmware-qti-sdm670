#ifndef REX_H
#define REX_H
/*==========================================================================

                      R E X    H E A D E R    F I L E

DESCRIPTION
   API for the REX - Real Time Executive


Copyright (c) 1990-2009 by Qualcomm Technologies Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/api/kernel/rex/rex.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/04/09   ts      Added rextp_tick_base utcb member for rextp purposes
09/14/08   kcr     Donot check for autodog_enabled field during 
                   REX_RESUME_DOG_MONITOR
09/08/08   cz      Added magic number and remove init_count for crit sect
06/04/08   cz      Added KxMutex API for Native REX.
06/04/08   cz      Added rex_try_enter_crit_sect().
06/04/08   cz      Removed unused ig_mutex_p field from rex_crit_sect_type.
06/04/08   cz      Adding init_count field in rex_crit_sect_type to track the 
                   number of initializations.
06/04/08   cz      Adding next field in rex_crit_sect_type to track critical 
                   sections in system.
05/27/08   kcr     Make REX_CRIT_SECT_MAX configurable at build time.
09/28/07   kcr     Adding lock variable to tcb for thread-safe tcb updates on qdsp6.
07/19/07   cz      Adding REX signal extension support.
07/13/07   cz      Added support for DAC based memory protection feature from umr.
07/25/06    pc     Added support for FEATURE_CPU_BASED_FLOW_CONTROL.
11/03/05    ck     Unified Rex API for Rex and L4 builds.
07/27/05    ck     Externalized rex_intlocked which is used by REX_INTLOCK
                   and is now called from rextime module.
11/16/04    ck     Removed definitions for rex_suspend_task_from_abort(),
                   rex_task_pri_from_abort() and rex_sets_sig_from_abort
                   which were used for progressive boot.
11/15/04    rp     Added progressive boot support.
06/03/04   gfr     Rex intlock profiling support.
03/04/04    gr     Added IPC support.
11/26/03    gr     Deleted the prototypes of the debug versions of the Rex
                   memory allocation functions.
10/31/03   ajn     Changed timer implementation on MSM6500+ targets to use
                   slow clock based services/time/timer.*
10/28/03    gr     Added externs for variables that are used elsewhere in
                   DMSS.
09/23/03    gr     Added missing prototype.
09/22/03   ksu     A task can now enter more than one critical section.
09/10/03    gr     Added the macro INTS_ARE_LOCKED.
06/20/03    gr     Defined REX_INT_LOCK and REX_INT_FREE macros to distinguish
                   interrupt locking in Rex from external interrupt locking.
                   Redefined TASKLOCK and TASKFREE so that they do not refer
                   to global variables.
11/15/02   jct     Added rex_create_thread, rex_terminate_thread
09/04/02    gr     Added the rex_def_task_ext2 function that allows the user
                   to specify a dog report value. rex_autodog_enable() and
                   rex_autodog_disable() now return the previous state.
07/16/02    gr     Renamed rex_autodog_init to rex_autodog_enable. Added
                   prototype for rex_autodog_disable.
07/11/02    gr     Added prototypes of timer list initialization function and
                   an APC-related function. Removed prototypes of obsolete
                   functions.
07/03/02    gr     Added support for pausing the monitoring of a task by Dog.
                   A new field in rex_tcb_type and the prototype of a new
                   function comprise the changes.
05/29/02    gr     Moved some macro definitions around to get REX to build
                   correctly off target.
05/23/02    gr     Renamed "locked" field in rex_crit_sect_type to lock_count.
                   Added err_num field to rex_tcb_type. Added check to ensure
                   that preprocessor symbols do not get redefined.
05/02/02    gr     Removed #ifdef TG protection for inclusion of target.h.
03/28/02    gr     Corrected definition of TASKS_ARE_LOCKED macro.
03/27/02    gr     Simplified the TASKLOCK and TASKFREE macros.
03/21/02    gr     Cosmetic changes. Updated some comments and fixed
                   indentation problems.
03/20/02    dc     Added support for FEATURE_SOFTWARE_PROFILE.
03/13/02    gr     Added macro TASKS_ARE_LOCKED.
                   Added prototype of new function rex_get_version_number.
03/08/02   kar     Added LTK support
11/02/01    gr     Added support for memory debug functionality.
08/21/01    gr     Redefined REX_ASSERT to call rex_exit.
08/15/01    gr     Changes required for new APC implementation. Support for
                   system signals.
06/06/01    gr     Modifications to enable the use of the extended timer
                   interface without turning on dynamic memory support.
01/22/01    mt     Additions for BSD to DS Sock net functionality.
12/19/00   jct     Merged MSM5000, MSM5200, MSP1000 versions of REX
08/18/00   bgc     Added r8-r11 to the task context for task switches.
11/17/00   crr     Changed banked registers from  r0-r7, r12 to r0-r7,r8-r12
                   to support nested, preemptive interrupts.
10/25/00    mt     Add new members to rex_tcb_type to support wait for
                   multiple sync events.
09/20/00    gr     Changes to support processes
09/14/00   jct     Modularization of new REX features and extended feature set
08/10/00    gr     Changes to support sync events. Added a field to rex_tcb_type
                   to support context-dependent intlock status when running on
                   NT.
06/15/00   jct     First ZREX additions
03/15/00    gr     Changed REX_TASK_NAME_LEN from 11 to 31.
                   Added extra field to rex_timer_type structure, protected by
                   FEATURE_REX_EXTENDED_TIMER.
                   Added prototypes for new functions rex_decrement_timers,
                   rex_tick_is_enabled, rex_enable_tick and rex_disable_tick.
01/15/00   tim     Added priority inversion prevention to critical sections.
12/16/99   tim     Introduction of feature that allows tasks to be started in
                   a paused state, plus addition of task name to TCB.
12/07/99   tim     Introduction of critical sections construct for providing
                   mutual exclusion between tasks.
11/30/99   tim     New implementation of TASKLOCK/TASKFREE, new function
                   rex_kill_task().
08/24/99   jct     Integrated Windows NT support
08/23/99   jeb     Removed cntrl-L white space character
08/16/99   jct     Added TASKLOCK/FREE
06/16/99   jct     Added inclusion of rexnt.h for T_WINNT targets
02/19/99    ms     Modified context frame to exploit running in Supervisor mode.
                   Changed interface of rex_init and rex_def_task. The stack
                   pointers in their parameter lists are void *.
                   Defined a new type rex_stack_size_type for stack sizes.
                   Added prototype for DBGBreak.
02/12/99    ms     Added support for TIMETEST port.
02/05/99    sk     Reduced kernel stack size to 40 words.
02/04/99    ms     Support for FEATURE_SI_STACK_WM.
12/16/98    ms     Removed 80186 support.
12/10/98   jkl     Include r12 in the context frame.
11/03/98   jct     Compressed some typedefs
10/26/98   jkl     Merged T_ARM with 80186 code, cleaned up file
09/15/98   udp     Replaced _ARM_ --> T_ARM
09/15/98   jct     Removed inclusion of types.h (was commented out anyway)
07/10/98   jct     Revised for coding standard, removed unused code
01/01/98   bwj     Created
02/01/96   rdh     Added rex_task_pri for time-critical task response speedup.
03/16/93   jah     Added extern of rex_int_level so that tasks could see the
                   current level of interrupt nesting.
02/23/93   jah/gb  Added TIMETEST code for task performance analysis using
                   an 8-bit buffered output.
04/23/92   ip      Changed signals type to rex_sigs_type (still one word).
04/22/92   ip      Initial porting of file from Brassboard to DMSS.

===========================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "comdef.h"
#include "rextls.h"
#include "rex_syscalls.h"
#include "aop_mpu.h"


/*===========================================================================

                           DATA DECLARATIONS

===========================================================================*/

/* ----------------------------------------------------------------------
** REX Data Types
** - These are sized based on the target
**   - Stack word, size of a word declared for stack variables
**   - Priority word, holds REX task priorities
**   - Time Count word, holds timer count downs
**   - Signal word, holds signals for a task
** ---------------------------------------------------------------------- */
typedef dword           rex_stack_word_type;
typedef dword           rex_stack_size_type;
typedef unsigned char   *rex_stack_pointer_type;
typedef dword           rex_priority_type;
typedef dword           rex_timer_cnt_type;
typedef dword           rex_sigs_type;
typedef dword           rex_vect_type;

/* Maximum number of tasks allowed in the system */
#define REX_MAX_TASKS   15

typedef uint32          rex_task_func_param_type;
typedef void (*rex_task_func_type)( dword );
   /* Function pointer type for REX tasks.
   ** The start-up function for a REX task must be of this type.
   */
typedef void (*rex_isr_func_type)( int irq );
typedef void (*rex_interrupt_func)(void);
   /* Type for IRQ and FIQ interrupt vectors.
   */

/* --------------------------------------------------------------------
 ** Asynchronous Procedure Calls
 **   Call the function with the given signature in a named context
 ** -------------------------------------------------------------------- */
typedef void (*rex_apc_func_type)( unsigned long param );
/* Signature of functions which can be called as APCs
 */
typedef rex_apc_func_type rex_timer_cb_type;
/* Type for REX timer callback functions.
 */
 
typedef void (*rex_err_handler_type)(void *);

extern void rex_task_intlock (void);
extern void rex_task_intunlock (void);

/* --------------------------------------------------------------------
** Deferred Procedure Calls
**   Provides context in which to run ISR level functions at task context.
**   DPC's are the highest priority task in the system
** -------------------------------------------------------------------- */
#define REX_DPC_NUM_RING_ITEMS 0x100
   /* Maximum number of items in the DPC ring buffer.
   */
#define REX_DPC_RING_MASK      0x0FF
   /* Used to perform modulo operation.
   */
#define REX_DPC_STACK_SIZE     1024
   /* Number of bytes in the stack for the DPC task.
   */
#define REX_DPC_SIG 0x01
   /* Signal on which the DPC task waits.
   */
typedef boolean (rex_dpc_func_type)(
   unsigned long param1,
   unsigned long param2,
   unsigned long param3
);
   /* DPC Function Type
   **   Signature of function which can be called as a DPC
   */
typedef struct {
   rex_dpc_func_type *dpc_ptr;
   unsigned long      dpc_param1;
   unsigned long      dpc_param2;
   unsigned long      dpc_param3;
} rex_dpc_item_type;
/* Type for items on DPC ring buffer.
*/

/* --------------------------------------------------------------------
** Inter-Process Communication
** -------------------------------------------------------------------- */
#define REX_IPC_MQ_SIZE 320
typedef struct
{
   uint32                    node_id;
   uint32                    service_id;
   rex_sigs_type             receive_signal;
   uint32                    msg_wanted;
   struct rex_tcb_struct*    tcb_ptr;
   struct pq_msg_queue_type* mq_ptr;
   uint8                     mq[REX_IPC_MQ_SIZE];
} rex_ipc_info_type;

/* ----------------------------------------------------------------------
** Time test variable
** ---------------------------------------------------------------------- */
extern word rex_led_buf;

extern byte  rex_profile_enabled;
extern dword rex_IRQnumticks;
extern word  rex_pnumticks;
extern dword rex_time_tick2;

typedef struct {
  byte  name[4];
  dword tick;
} sw_profile_type;

/* The maximum number of tasks for which profiling will be done.
*/
#ifndef REX_MAX_PROFILE_ENTRY
   #define REX_MAX_PROFILE_ENTRY 32
#endif


/* ----------------------------------------------------------------------
** Rex timer name length
** ---------------------------------------------------------------------- */
#ifndef REX_TIMER_NAME_LENGTH
   #define REX_TIMER_NAME_LENGTH 32
#endif

/* ----------------------------------------------------------------------
** REX TCB Link Type
** - struct with a next and a prev pointer, for use by TCBs and critical
**   sections
** ---------------------------------------------------------------------- */
typedef struct {
   struct rex_tcb_struct *next_ptr;
   struct rex_tcb_struct *prev_ptr;
} rex_tcb_link_type;

/* ----------------------------------------------------------------------
** REX Critical Section Type
** - structure contains a lock and a pointer to the first TCB waiting
**   on the lock.  TCBs then form the rest of the list with the cs_list
**   pointer in the TCB.
** ---------------------------------------------------------------------- */
typedef struct _rex_crit_sect_type{
  byte                   lock_count; /* > 0 if crit sect is taken      */
  struct rex_tcb_struct *owner;      /* TCB of task that owns the lock */
  struct rex_tcb_struct *tcb_link;   /* ptr to first TCB waiting */
  rex_priority_type      orig_pri;   /* For use with prio-inversion prevention */
  struct _rex_crit_sect_type *next;   /* for rex crit sect list */
  dword                  magic;       /* magic number is used to identify initilized obj */
} rex_crit_sect_type;

/*----------------------------------------------------------------------
** This magic number is set to "magic" field when initialization. 
**---------------------------------------------------------------------*/
#define REX_CRIT_SECT_MAGIC 0x1234ABCD

/*----------------------------------------------------------------------
** For the critical section TCB list, we need a flag value that is not
** NULL.  This means that the TCB is involved in a critical region operation.
**---------------------------------------------------------------------*/
#define REX_CRIT_SECT_FLAG ((rex_tcb_type *) 0x01)

/*----------------------------------------------------------------------
** Maximum number of critical sections that a task can simultaneously
** enter.  In general, this should be as small as possible to keep the
** TCB small, while large enough to handle anticipated nesting levels.
** If you think this nesting level is not sufficient, you can override 
** this default value by redefining it in custrex.h file in your 
** build environment.
**---------------------------------------------------------------------*/
#ifndef REX_CRIT_SECT_MAX 
#define REX_CRIT_SECT_MAX (16)
#endif


/* Size alloted for the name of a task
*/
#ifndef REX_TASK_NAME_LEN
   #define REX_TASK_NAME_LEN    11  /* choose so that this number +1 is
                                     * divisible by 4.
                                     */
#endif

/* ----------------------------------------------------------------------
** Advance translation of rex_timer_struct to timer_struct, since
** rex_tcb_struct might declare: struct rex_timer_struct *sync_timer_ptr;
** ---------------------------------------------------------------------- */
#define rex_timer_struct timer_struct

/* ----------------------------------------------------------------------
** REX Task Control Block
** - Describes a REX task
** - Fields should not be accessed directly
** - Order of fields should not be altered as there are dependencies in
**   the kernel on their ordering
** ---------------------------------------------------------------------- */
typedef struct rex_tcb_struct {
  void                       *sp;          /* Stack pointer   */
  void                       *stack_limit; /* Stack Limit     */
  unsigned long              slices;       /* Task slices     */

  rex_sigs_type              sigs;
  rex_sigs_type              wait;

  rex_priority_type          pri;          /* Task's priority */
   
  boolean                    suspended;

  unsigned long              intlock_nest_level; /* How many times intlock has been nested */
  
  /* whether task is suspended, this is runtime suspension, set by rex_suspend_task() */

  /* all fields *above* this line have offsets defined
   * in rexarm.h.  Be careful to keep both consistent.
   */

  rex_tcb_link_type          link;         /* for TCB list */

  rex_tcb_link_type          cs_link;       /* non-NULL when waiting for a
                                             * critical section
                                             */

  rex_crit_sect_type         *cs_stack[REX_CRIT_SECT_MAX]; /* crit sects TCB wants/has */
  rex_crit_sect_type         **cs_sp;        /* crit sects stack pointer */


  struct rex_tcb_struct      *pri_rep_ptr; /* priority representative */


  char                       task_name[REX_TASK_NAME_LEN + 1];

  unsigned long              thread_id;

  unsigned long              stack_size;
  unsigned long              max_stack_usage;

  int                        dog_report_val;    /* Dog Report Id          */
  int                        autodog_enabled;   /* Dog Report enabled?    */
#ifdef FEATURE_REX_TLS
  rex_tls_table_t            tls_table;
#endif
} rex_tcb_type;

/* ------------------------------------------------------------------------
** CPU Registers
** ------------------------------------------------------------------------ */
typedef PACKED union {
   unsigned long arg;
   unsigned long val;
   void          *addr;
   void         (*func)( void );
   void         (*task)( unsigned long );
   void         (*preamble)( void (*func)(unsigned long), unsigned long );
} rex_cpu_register_type;


/* ------------------------------------------------------------------------
** Context Frame
** ------------------------------------------------------------------------ */
typedef PACKED struct {
   rex_cpu_register_type spsr;
   rex_cpu_register_type r[13];  /* r0-r7,r8-r12 */
   rex_cpu_register_type lr;    /* r14        */
   rex_cpu_register_type pc;    /* r15        */
} rex_context_frame_type;

/* ------------------------------------------------------------------------
** Frame pushed on the IRQ stack in the IRQ handler
** ------------------------------------------------------------------------ */
typedef PACKED struct {
   rex_cpu_register_type spsr;
   rex_cpu_register_type r[6];  /* r0-r3,r10,r12               */
   rex_cpu_register_type lr;    /* IRQ mode r14 = SVC mode PC  */
} rex_saved_irq_frame_type;

#ifdef FEATURE_TIMERS_SUPPORTED
/* ----------------------------------------------------------------------
** REX Timers are being replaced by slow clock based timers, which:
**    - have a resolution of 30us instead of 5ms,
**    - don't require a periodic 5ms tick
**    - can wake the ARM up from sleep, if required
** - Timer fields must not be accessed directly
** ---------------------------------------------------------------------- */

  #define rex_timer_type timer_type
  #include "timer.h"

#else

  #define rex_timer_type int

#endif


typedef struct
{
   unsigned long sleep_time_samples;
   unsigned long total_time_samples;
}rex_sleep_report_type;
 
/* Boolean state variable indicating whether or not the rex is
 * currently involved a kernel intlock.
 */
extern unsigned long rex_intlocked;

/* ------------------------------------------------------------------------
** REX errors
** ------------------------------------------------------------------------ */
#define  REX_ERROR_MISALIGNED_DATA                          0x01
#define  REX_ERROR_DATA_ABORT                               0x02
#define  REX_ERROR_PREFETCH_ABORT                           0x03
#define  REX_ERROR_DOUBLE_ABORT_EXCEPTION                   0x04
#define  REX_ERROR_UNDEFINED_INSTR                          0x05
#define  REX_ERROR_INVALID_PARAMETER                        0x06
#define  REX_ERROR_USER_DEVICE_ACCESS                       0x07

/* ------------------------------------------------------------------------
** The maximum number of REX timers that can be in the system at one time.
** ------------------------------------------------------------------------ */
#ifndef REX_SYS_MAX_TIMER_IDS
   #define REX_SYS_MAX_TIMER_IDS 128
#endif

/* ------------------------------------------------------------------------
** Program Counter Dump array size.
** ------------------------------------------------------------------------ */
#ifndef PC_ARRAY_SIZE
   #define PC_ARRAY_SIZE 0x100000
#endif

/* ------------------------------------------------------------------------
** Default Number of Time Slices for a Thread.
** ------------------------------------------------------------------------ */
#define REX_DEFAULT_SLICES 5

/*----------------------------------------------------------------------
** The stack is filled with the "magic" value when initialization.
**---------------------------------------------------------------------*/
#define REX_STACK_FILL_MAGIC 0xDDDDDDDD

/*===========================================================================

                            MACRO DECLARATIONS

===========================================================================*/
   #define REX_INTERCEPT_INIT_READY()
   #define REX_INTERCEPT_DEF_TIMER( p_timer, p_tcb, p_sigs )
   #define REX_INTERCEPT_DEF_TIMER_EX( p_timer, p_tcb, p_sigs )
   #define REX_INTERCEPT_KILL_TASK( p_tcb )
   #define REX_INTERCEPT_KILL_TIMER( p_timer )
   #define REX_INTERCEPT_WAIT( p_sigs )

   #define REX_INTERCEPT_START_IDLE_TASK()
   #define REX_INTERCEPT_IDLE_TASK()
   #define REX_INTERCEPT_DEF_TASK_EXT_INIT_CONTEXT( p_tcb, p_task, p_param, p_pri )
   #define REX_INTERCEPT_SYS_WAIT( sigs )

   #define REX_ASSERT( xx_condition )

/*===========================================================================
REX_DECLARE_STACK

DESCRIPTION
   Declare a 32-byte-aligned byte array for task stack with 32-byte stack guard.

===========================================================================*/
#define REX_DECLARE_STACK(var_name, size_bytes) \
__attribute__((section("task_stacks"))) __attribute__((aligned(32))) \
uint8 var_name[(size_bytes) + AOP_MPU_STACK_GUARD_SIZE]

/*===========================================================================
MACRO REX_STACK_SIZE

DESCRIPTION
   Return the size of stack in rex_stack_word_type

===========================================================================*/
#define REX_STACK_SIZE(var_name)  (sizeof(var_name) / sizeof(rex_stack_word_type))

/*===========================================================================
MACRO TASKLOCK

DESCRIPTION
   Prevent task from being preempted by other tasks

===========================================================================*/
#ifndef TASKLOCK 
#define TASKLOCK( ) rex_task_lock( )
#endif

/*===========================================================================
MACRO TASKFREE

DESCRIPTION
   Allow task to be preempted by other tasks

===========================================================================*/
#ifndef TASKFREE
#define TASKFREE( ) rex_task_free( )
#endif

/*===========================================================================
MACRO TASKS_ARE_LOCKED

DESCRIPTION
   Returns TRUE if a TASKLOCK is in effect and FALSE otherwise.

===========================================================================*/
#define TASKS_ARE_LOCKED( ) rex_tasks_are_locked( )

/*===========================================================================
MACRO INTS_ARE_LOCKED

DESCRIPTION
  Returns 0 if interrupts are currently enabled and a non-zero value if
  interrupts are disabled. Note that this function checks only IRQ (does not
  check FIQ).
===========================================================================*/
#define INTS_ARE_LOCKED() rex_ints_are_locked( )

/*===========================================================================
MACROS REX_INTLOCK/REX_INTFREE

DESCRIPTION
   Locks/unlocks interrupts. If FEATURE_REX_PROFILE is defined, sets a
   variable to note that the interrupt locking/freeing was done from
   within Rex. This distinction is useful for profiling.

   NOTE: These macros are to be used internally in Rex.

===========================================================================*/
#define REX_INTLOCK( ) rex_task_intlock ()
#define REX_INTFREE( ) rex_task_intunlock ()

/*===========================================================================
MACROS REX_DISABLE_PREMPTION/REX_ENABLE_PREMPTION

DESCRIPTION
   Makes the code protected by these two calls run atomically. On native
   rex and on ARM L4 these are essentially INTLOCK/INTFREE. 
   On qdsp6 they will behave differently.

===========================================================================*/

#define REX_DISABLE_PREMPTION() INTLOCK()
#define REX_ENABLE_PREMPTION()  INTFREE()

/*===========================================================================
MACROS REX_ISR_LOCK/REX_ISR_UNLOCK

DESCRIPTION
   Piece of code that needs to be protected and can potentially run in
   ISR context needs to use these macros instead of INTLOCK/INTFREE.
   These macros map to INTLOCK/INTFREE on native rex and on ARM L4 but  
   On qdsp6 they will behave differently.

===========================================================================*/

#define REX_ISR_LOCK(my_global_lock_ptr)  INTLOCK()
#define REX_ISR_UNLOCK(my_global_lock_ptr) INTFREE()


/*===========================================================================

                           FUNCTION DECLARATIONS

===========================================================================*/

#ifdef __cplusplus
   extern "C"
   {
#endif

/*===========================================================================
FUNCTION REX_INIT

DESCRIPTION
  Initializes the REX subsystem

DEPENDENCIES
  p_tcb must be a valid task control block.
  p_task must be a valid function pointer.

RETURN VALUE
  None

SIDE EFFECTS
  When the initialization is done, this thread will wait until
  the exit event is triggered at which point it will clean up
  and return.
===========================================================================*/
void rex_init(
  void *                 p_istack,              /* interrupt stack      */
  rex_stack_size_type    p_istksiz,             /* interrupt stack size */
  rex_tcb_type           *p_tcb,                /* task control block   */
  void *                 p_stack,               /* stack                */
  rex_stack_size_type    p_stksiz,              /* stack size           */
  rex_priority_type      p_pri,                 /* task priority        */
  rex_task_func_type     p_task,                /* task function        */
  dword                  p_param                /* task parameter       */
);


/*===========================================================================
FUNCTION REX_TICK

DESCRIPTION
  If timers are enabled, calls rex_decrement_timers to decrement the timer
  value for all valid timers in the active timer list. Returns immediately
  if timers are disabled.

DEPENDENCIES
  Interrupts must be disabled.

RETURN VALUE
  None.

SIDE EFFECTS
  If a timer expires, rex_set_sigs is called and a task swap may be induced.
===========================================================================*/
void rex_tick(
  rex_timer_cnt_type p_ticks      /* number of rex ticks elapsed */
);



/*===========================================================================
FUNCTION REX_DEF_TIMER

DESCRIPTION
  Initializes a REX timer

DEPENDENCIES
  p_timer must be a valid timer
  tcb_ptr must be a valid task control block

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rex_def_timer(
   rex_timer_type *p_timer,      /* pointer to a valid timer structure */
   rex_tcb_type   *tcb_ptr,      /* tcb to associate with the timer    */
   rex_sigs_type  sigs           /* sigs to set upon timer expiration  */
);


/*===========================================================================
FUNCTION REX_CLR_TIMER

DESCRIPTION
  Clears a running timer

DEPENDENCIES
  None

RETURN VALUE
  The countdown of the timer (in ms).

SIDE EFFECTS
  None
===========================================================================*/
rex_timer_cnt_type rex_clr_timer(
  rex_timer_type *p_timer    /* pointer to timer to clear */
);


/*===========================================================================
FUNCTION REX_SET_TIMER

DESCRIPTION
  Sets a REX timer to expire after the specified interval (in ms).

DEPENDENCIES
  None

RETURN VALUE
  The last countdown of the timer

SIDE EFFECTS
  Could cause a call to set sigs and thus induce a task swap.
===========================================================================*/
rex_timer_cnt_type rex_set_timer(
  rex_timer_type       *p_timer,  /* pointer to timer to set */
  rex_timer_cnt_type   msecs      /* value in milliseconds   */
);


/*===========================================================================
FUNCTION REX_GET_TIMER

DESCRIPTION
  Get the current countdown of a REX timer

DEPENDENCIES
  p_timer must be a valid timer.

RETURN VALUE
  The countdown remaining (in ms).

SIDE EFFECTS
  None
===========================================================================*/
rex_timer_cnt_type rex_get_timer(
   rex_timer_type *p_timer   /* pointer to the timer to get */
);


/*===========================================================================
FUNCTION REX_PAUSE_TIMER

DESCRIPTION
  Pauses a timer.
===========================================================================*/
void rex_pause_timer(
  rex_timer_type *timer_ptr
);


/*===========================================================================
FUNCTION REX_RESUME_TIMER

DESCRIPTION
  Resumes a timer.
===========================================================================*/
void rex_resume_timer(
  rex_timer_type *timer_ptr
);


/*===========================================================================
FUNCTION REX_DEF_TASK

DESCRIPTION
  Creates a REX Task

DEPENDENCIES
  p_tcb must be a valid tcb block.
  p_task must be a valid function pointer.

  This function expects the TCB buffer which p_tcb points to was ZEROed out. 
  If the buffer was allocated from heap, please make sure clearing it to ZERO 
  by memset before calling this function.
  
RETURN VALUE
  None

SIDE EFFECTS
  Defines a new rex task that will be placed in the task list.
  Creates a new thread and an event for the new task.
  The task starts running only if it is the highest priority ready task.
===========================================================================*/
void rex_def_task(
  rex_tcb_type            *p_tcb,                 /* valid tcb for new task   */
  void *                   p_stack,                /* stack for new task       */
  rex_stack_size_type      p_stksiz,               /* stack size for new task  */
  rex_priority_type        p_pri,                  /* priority for new task    */
  rex_task_func_type       p_task,                 /* task startup function    */
  rex_task_func_param_type p_param                 /* parameter for new task   */
);


/*===========================================================================
FUNCTION REX_DEF_TASK_EXT

DESCRIPTION
  Creates a REX Task.  Adds two arguments past what rex_def_task() allows.
  First, accepts a string containing the task name.  Second, accepts a flag
  that indicates whether the task should be started in SUSPENDED mode or not.

  Expects the task stack to be an array of bytes, the stack size should be
  the number of bytes in the array, the stack pointer itself should be
  aligned on a 4 byte boundary.  When not aligned, REX will alter the base
  of the stack to the next 4 byte boundary thereby not using 1-3 bytes of
  memory.

DEPENDENCIES
  p_tcb must be a valid tcb block.
  p_task must be a valid function pointer.
  p_stack should be on a 4 byte aligned boundary

  This function expects the TCB buffer which p_tcb points to was ZEROed out. 
  If the buffer was allocated from heap, please make sure clearing it to ZERO 
  by memset before calling this function.

RETURN VALUE
  None

SIDE EFFECTS
  Defines a new rex task that will be placed in the task list.
  Creates a new thread and an event for the new task.
  The task starts running only if it is the highest priority ready task and
  it is not SUSPENDED.
===========================================================================*/
void rex_def_task_ext(
  rex_tcb_type            *p_tcb,                  /* valid tcb for new task   */
  rex_stack_pointer_type   p_stack,                /* stack for new task       */
  rex_stack_size_type      p_stksiz,               /* stack size for new task  */
  rex_priority_type        p_pri,                  /* priority for new task    */
  rex_task_func_type       p_task,                 /* task startup function    */
  rex_task_func_param_type p_param,                /* parameter for new task   */
  char                    *p_tskname,              /* A/N string for task name */
  boolean                  suspended               /* is task initially susp?  */
);


/*===========================================================================
FUNCTION REX_DEF_TASK_EXT2

DESCRIPTION
  Creates a REX Task.  Adds an argument past what rex_def_task_ext() allows.

DEPENDENCIES
  p_tcb must be a valid tcb block.
  p_task must be a valid function pointer.
  p_stack should be on a 4 byte aligned boundary

  This function expects the TCB buffer which p_tcb points to was ZEROed out. 
  If the buffer was allocated from heap, please make sure clearing it to ZERO 
  by memset before calling this function.
  
RETURN VALUE
  None

SIDE EFFECTS
  Defines a new rex task that will be placed in the task list.
  Creates a new thread and an event for the new task.
  The task starts running only if it is the highest priority ready task and
  it is not SUSPENDED.
===========================================================================*/
void rex_def_task_ext2(
  rex_tcb_type            *p_tcb,                  /* valid tcb for new task   */
  rex_stack_pointer_type   p_stack,                /* stack for new task       */
  rex_stack_size_type      p_stksiz,               /* stack size for new task  */
  rex_priority_type        p_pri,                  /* priority for new task    */
  rex_task_func_type       p_task,                 /* task startup function    */
  rex_task_func_param_type p_param,                /* parameter for new task   */
  char                    *p_tskname,              /* A/N string for task name */
  boolean                  suspended,              /* is task initially susp?  */
  int                      dog_report_val          /* Dog report value         */
);

/*===========================================================================
FUNCTION REX_DEF_TASK_INTERNAL

DESCRIPTION
  Creates a REX Task.  Adds three arguments past what rex_def_task_ex() allows.
  Accepts an opaque handle to a parent entity or container.  Useful in contexts
  where REX tasks are contained by other execution contexts.  Accepts a boolean
  argument indicating whether or not the TCB and stack space were allocated
  dynamically.  Useful in situations where recovering the memory from the task
  once it has died is useful. Accepts the dog_report id for the task. Useful
  for using the autodog functionality.

DEPENDENCIES
  p_tcb must be a valid tcb block.
  p_task must be a valid function pointer.
  p_stack must be on an aligned boundary and the size must be the number
  of bytes in the stack

  !!! Intended for use by Kernel level functions only !!!

RETURN VALUE
  None

SIDE EFFECTS
  Defines a new rex task that will be placed in the task list.
  Creates a new thread and an event for the new task.
  The task starts running only if it is the highest priority ready task and
  it is not SUSPENDED.
===========================================================================*/
void rex_def_task_internal(
  rex_tcb_type            *p_tcb,             /* valid tcb for new task         */
  rex_stack_pointer_type   p_stack,            /* stack for new task             */
  rex_stack_size_type      p_stksiz,           /* stack size for new task        */
  rex_priority_type        p_pri,              /* priority for new task          */
  rex_task_func_type       p_task,             /* task startup function          */
  rex_task_func_param_type p_param,            /* parameter for new task         */
  char                    *p_tskname,         /* A/N string for task name       */
  boolean                  p_susp,             /* is task initially suspended?   */
  void                    *p_parent,          /* opaque handle to container     */
  boolean                  p_dynamic,          /* stack/tcb alloc'd via dyna mem */
  int                      dog_report_val      /* Dog report value               */
);


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
);


/*===========================================================================
FUNCTION REX_RESUME_TASK

DESCRIPTION
  Resumes a REX task that was started in the SUSPENDED mode.

DEPENDENCIES
  p_tcb must be a valid tcb block.
  p_task must be a valid function pointer.

RETURN VALUE
  None

SIDE EFFECTS
  If the resumed task is of higher priority than the calling task, then a
  context switch will occur.
===========================================================================*/
void rex_resume_task(
  rex_tcb_type         *p_tcb                  /* task to be un-suspended  */
);


/*===========================================================================
FUNCTION REX_KILL_TASK

DESCRIPTION
  Kills a rex task by removing it from the list of active tasks.  If a task
  makes a call to rex_kill_task to kill itself, then after the TCB is
  removed from the list, a call to the scheduler is made to find a task to
  run in place of the killed task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void rex_kill_task(
  rex_tcb_type *p_tcb
);


/*===========================================================================
FUNCTION REX_INIT_CRIT_SECT

DESCRIPTION
  Initializes a critical section.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rex_init_crit_sect(
  rex_crit_sect_type  *crit_sect  /* critical section to initialize */
);


/*===========================================================================
FUNCTION REX_ENTER_CRIT_SECT

DESCRIPTION
  Acquire the lock on a critical section.  If it's not available, task
  suspends and is placed on a waiting list until the lock is free.

DEPENDENCIES
  Critical section variable must have been initialized with
  rex_init_crit_sect() call.

RETURN VALUE
  None

SIDE EFFECTS
  Task will block if lock is not available.  Highest priority non-blocked
  task will be swapped in.
===========================================================================*/
void rex_enter_crit_sect(
  rex_crit_sect_type *crit_sect
);


/*===========================================================================
FUNCTION REX_LEAVE_CRIT_SECT

DESCRIPTION
  Release the lock on a critical section.  If there is a list of tasks
  waiting on the critical section, choose the highest priority task and
  grant that task the lock.

DEPENDENCIES
  Calling task must be holding the critical section.

RETURN VALUE
  None.

SIDE EFFECTS
  If the highest priority task on the list of waiting tasks is higher
  priority than the calling task, then the calling task will be swapped out
  and the higher priority task will be swapped in.
===========================================================================*/
void rex_leave_crit_sect(
  rex_crit_sect_type *crit_sect
);

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
);

/*===========================================================================
FUNCTION REX_TASK_PRI

DESCRIPTION
  Set the priority of the specified task to the given priority

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The priority change will cause a change in the whole scheduling order for
  all of the tasks. This could result in an immediate task swap.
===========================================================================*/
rex_priority_type rex_task_pri(
  rex_tcb_type       *p_tcb,     /* tcb to set priority on */
  rex_priority_type  p_pri       /* the new priority       */
);


/*===========================================================================
FUNCTION REX_SET_PRI

DESCRIPTION
  Sets the current task's (i.e. the calling task's) priority to the specified
  value.

DEPENDENCIES
  None

RETURN VALUE
  Previous priority

SIDE EFFECTS
  Affects schedulability of system
===========================================================================*/
rex_priority_type rex_set_pri(
   rex_priority_type p_pri       /* the new priority */
);


/*===========================================================================
FUNCTION REX_GET_PRI

DESCRIPTION
  Get the priority of this task

DEPENDENCIES
  None

RETURN VALUE
  Current priority

SIDE EFFECTS
  None
===========================================================================*/
rex_priority_type rex_get_pri( void );


/*===========================================================================
FUNCTION REX_SELF

DESCRIPTION
  Gets a pointer to the TCB of the currently running task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
rex_tcb_type* rex_self( void );


/*===========================================================================
FUNCTION REX_WAIT

DESCRIPTION
  Waits on specified signals

DEPENDENCIES
  None

RETURN VALUE
  Signals asserted

SIDE EFFECTS
  Will cause a task swap if the signals for the task are not already set at
  the time of the call.
===========================================================================*/
rex_sigs_type rex_wait(
  rex_sigs_type p_sigs     /* sigs to wait on */
);


/*===========================================================================
FUNCTION REX_CLR_SIGS

DESCRIPTION
  Clears the specified signals for the specified TCB

DEPENDENCIES
  None

RETURN VALUE
  Signal mask after clearing

SIDE EFFECTS
  Affects schedulability of specified TCB
===========================================================================*/
rex_sigs_type rex_clr_sigs(
  rex_tcb_type *tcb_ptr,     /* tcb for which the signals will be cleared */
  rex_sigs_type sigs         /* which signals to clear                    */
);


/*===========================================================================
FUNCTION REX_SET_SIGS

DESCRIPTION
  Sets signals for specified TCB, causes REX to reschedule system

DEPENDENCIES
  None

RETURN VALUE
  Signals mask after setting, note, that if the set causes a higher priority
  task to run, and it clears the sigs, the mask returned may indicate
  that the signal was never set (meaning, it was set and processed)

SIDE EFFECTS
  A task swap might happen.
===========================================================================*/
rex_sigs_type rex_set_sigs(
  rex_tcb_type *tcb_ptr,     /* tcb for which the sigs will be set */
  rex_sigs_type sigs         /* the sigs to set                    */
);


/*===========================================================================
FUNCTION REX_GET_SIGS

DESCRIPTION
  Gets the currently asserted signals for the specified task

DEPENDENCIES
  None

RETURN VALUE
  Signal mask

SIDE EFFECTS
  None
===========================================================================*/
rex_sigs_type rex_get_sigs(
  rex_tcb_type *p_tcb        /* tcb for which sigs will be returned */
);

/*===========================================================================
FUNCTION REX_REGISTER_RECEIVE_SIGNAL

DESCRIPTION

PARAMETERS

RETURN VALUE

==========================================================================*/
void rex_register_receive_signal(
   rex_sigs_type signal
);

/*===========================================================================
FUNCTION REX_TIMED_WAIT

DESCRIPTION
  Performs a rex_set_timer followed by a rex_wait.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Causes a task swap if the signals are not already set for the calling task.
===========================================================================*/
rex_sigs_type rex_timed_wait(
  rex_sigs_type       p_sigs,         /* sigs to wait on          */
  rex_timer_type      *p_timer,       /* timer to set and wait on */
  rex_timer_cnt_type  p_cnt           /* timer to wait            */
);

/*===========================================================================
FUNCTION REX_INT_LOCK

DESCRIPTION
  Lock interrupt

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Affects schedulability of system and ability to service hardware
===========================================================================*/
dword rex_int_lock(void);


/*===========================================================================
FUNCTION  REX_INT_FREE

DESCRIPTION
  free interrupts

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Affects schedulability of system and ability to service hardware
===========================================================================*/
dword rex_int_free(void);

/*===========================================================================
FUNCTION REX_TRY_ENTER_CRIT_SECT

DESCRIPTION
  Acquire the lock on a critical section.  If it's not available, this
  function will just return without the object.

DEPENDENCIES
  Critical section object must have been initialized with
  rex_init_crit_sect() call.

RETURN VALUE
  TRUE if crit_sect is available
  FALSE if crit_sect is not available

SIDE EFFECTS
  None
===========================================================================*/
boolean rex_try_enter_crit_sect(
  rex_crit_sect_type *crit_sect
);

/*===========================================================================
FUNCTION REX_SET_INTERRUPT_VECTOR

DESCRIPTION
  Installs interrupt vector call

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Affects schedulability of system and ability to service hardware
===========================================================================*/
void rex_set_interrupt_vector(
    rex_vect_type v,
    rex_isr_func_type fnc_ptr
);


/*===========================================================================
FUNCTION REX_ENABLE_INTERRUPT

DESCRIPTION
  free interrupts

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Affects schedulability of system and ability to service hardware
===========================================================================*/
void rex_enable_interrupt( rex_vect_type v );


/*===========================================================================
FUNCTION  REX_DISABLE_INTERRUPT

DESCRIPTION
  Locks interrupts

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Affects schedulability of system and ability to service hardware
===========================================================================*/
void rex_disable_interrupt( rex_vect_type v );


/*===========================================================================
FUNCTION  REX_CLEAR_INTERRUPT

DESCRIPTION
  Clears interrupts

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Affects schedulability of system and ability to service hardware
===========================================================================*/
void rex_clear_interrupt( rex_vect_type v );

/*===========================================================================
FUNCTION  REX_CONFIGURE_INTERRUPT

DESCRIPTION
  Configures interrupts

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Affects schedulability of system and ability to service hardware
===========================================================================*/
void rex_configure_interrupt(rex_vect_type v, interrupt_config config);


/*===========================================================================
FUNCTION  REX_GET_INTERRUPT_POLARITY

DESCRIPTION
  Return interrupt polarity

DEPENDENCIES
  None

RETURN VALUE
  interrupt_config

SIDE EFFECTS
  None
===========================================================================*/
interrupt_config rex_get_interrupt_polarity(rex_vect_type v);

/*===========================================================================
FUNCTION  REX_SET_INTERRUPT_PRIORITY

DESCRIPTION
  Sets interrupts prority

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Affects schedulability of system and ability to service hardware
===========================================================================*/
void rex_set_interrupt_priority(rex_vect_type v, unsigned priority);

/*===========================================================================
FUNCTION REX_SCHED

DESCRIPTION
  Does the task swap. Is called within rex and should not be called from
  outside of rex. Rex_start_task is an entry point from rex_sched to restore
  context (called only by rex_init).

DEPENDENCIES
  Rex_sched assumes that the following global variables have reasonable
  values: rex_curr_task, rex_best_task.

RETURN VALUE
  None.

SIDE EFFECTS
  Rex_sched may set the current task pointer (rex_curr_task) equal to
  the best task pointer (rex_best_task) if they are not already the same.
  Also, rex_sched may cause a task swap to occur from the old current
  task to the new current task.
===========================================================================*/
void rex_sched( void );


/*===========================================================================
FUNCTION REX_START_TASK

DESCRIPTION
  Does the task initialization.It is called within REX and should not be
  called from outside of rex.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Starts the task by restoring its context
===========================================================================*/
void rex_start_task( rex_tcb_type *p_tcb );


/* ==================================================================
FUNCTION REX_IS_IN_IRQ_MODE

DESCRIPTION
   Returns TRUE if the current mode is IRQ mode and FALSE otherwise.
===================================================================== */
boolean rex_is_in_irq_mode( void );

/*===========================================================================
FUNCTION REX_REMOVE_TASK

DESCRIPTION
  Function that implements the process of removing a TCB from
  the REX task list.
===========================================================================*/
void rex_remove_task(
  rex_tcb_type *tcb_ptr   /* pointer to tcb */
);

/*===========================================================================
FUNCTION REX_GET_CPSR

DESCRIPTION
  Returns the current Processor Status Register
===========================================================================*/
unsigned long rex_get_cpsr( void );

/*===========================================================================
FUNCTION REX_VIRT_TO_PHYS

DESCRIPTION
  Returns the translated physical address or fault status.
===========================================================================*/
unsigned int rex_virt_to_phys( unsigned int virt_addr );

/* --------------------------------------------------------------------------
**
** Asynchronous Procedure Calls
**
** -------------------------------------------------------------------------- */

/*===========================================================================
FUNCTION REX_QUEUE_APC

DESCRIPTION
  Pushes an APC context frame onto the stack of the specified task.
  APC's are popped and executed the next time the task is runnable.
  The number of APC's is limited only by the available stack space.

  APC's are executed in the context of the target task before the task
  is resumed at is previously preempted location.

  APC's queued for the currently running task are rejected.

  This does NOT make the specified task runnable.

===========================================================================*/
boolean rex_queue_apc(
   rex_apc_func_type apc,
      /* Pointer to the function to call
      */
   rex_tcb_type *tcb_ptr,
      /* Task in whose context the APC should run
      */
   unsigned long param
      /* Parameter to pass to the apc
      */
);

/* ==========================================================================
FUNCTION REX_QUEUE_APC_INTERNAL
DESCRIPTION
   Performs the actual work of queueing an APC. This function is for internal
   REX use only. It must never be called from an ISR. The function exported
   to the user is rex_queue_apc, which causes rex_queue_apc_internal to be
   called in the context of the DPC task.
============================================================================= */
boolean rex_queue_apc_internal(
   unsigned long apc_handle,
   unsigned long tcb_handle,
   unsigned long param
);

/*===========================================================================

FUNCTION REX_APC_EPILOG

DESCRIPTION
  Performs processing needed after a rex_apc_call call. After the APC
  has been executed, there are two cases to consider:
  (a) A context switch is not needed.
      In this case, just pop the top context frame off the stack of the
      current thread and restore registers using this frame.

  (b) A context switch is needed.
      In this case, save the stack pointer of the current thread in its
      TCB. It is not necessary to save a context frame because one gets
      pushed when an APC is queued.

  NOTE 1: This function is for internal REX use only.

  NOTE 2: This function restores registers and may cause a context switch.

===========================================================================*/
void rex_apc_epilog( void );

/* --------------------------------------------------------------------------
**
** Deferred Procedure Calls
**
** -------------------------------------------------------------------------- */

/* ==================================================================
FUNCTION  REX_QUEUE_DPC
DESCRIPTION
   A function that executes in a context which is deferred from
   interrupt context.  This can only be called at interrupt context
   and otherwise causes a diagnostics to be logged and the requested
   call to be ignored.

   Operates much like an APC except is executed in the context of a
   REX owned thread.
===================================================================== */
boolean rex_queue_dpc(
   rex_dpc_func_type *dpc_ptr,
      /* Pointer to the function to call
      */
   unsigned long      param1,
      /* Parameter to pass to the dpc
      */
   unsigned long      param2,
      /* Parameter to pass to the dpc
      */
   unsigned long      param3
      /* Parameter to pass to the dpc
      */
);

/* --------------------------------------------------------------------------
**
** REX Extended Dynamic Timers
**
** -------------------------------------------------------------------------- */

/* ==================================================================
FUNCTION REX_DEF_TIMER_EX
DESCRIPTION
   Extended version of timer-definition function. Allows the
   specification of a callback function and an argument to this
   function. The callback is called when the timer expires. The
   callback happens in the context of the task that calls
   rex_def_timer_ex.
===================================================================== */
void rex_def_timer_ex(
   rex_timer_type    *timer_ptr,
      /* pointer to a valid timer structure */
   rex_timer_cb_type  timer_cb_ptr,
     /* pointer to timer callback           */
   unsigned long      cb_param
     /* argument passed to timer callback   */
);

/* The following extended timer functions require dynamic memory
*/
/* ==================================================================
FUNCTION REX_CREATE_TIMER_EX
DESCRIPTION
   Similar to rex_def_timer_ex, except that this function allocates
   the memory for the timer dynamically. A pointer to the timer is
   returned.
===================================================================== */
rex_timer_type *rex_create_timer_ex(
   rex_timer_cb_type  timer_cb_ptr,
     /* pointer to timer callback           */
   unsigned long      cb_param
     /* argument passed to timer callback   */
);

/* ==================================================================
FUNCTION REX_DELETE_TIMER_EX
DESCRIPTION
   Frees the memory for a dynamically allocated timer.
===================================================================== */
void rex_delete_timer_ex(
   rex_timer_type *timer_ptr
);

/* ------------------------------------------------------------------------
**
** REX Dynamic Memory Management
**
** ------------------------------------------------------------------------ */

/* ==================================================================
FUNCTION REX_MALLOC
DESCRIPTION
   Dynamically allocates a specified number of bytes and returns a
   pointer to this chunk of memory.
===================================================================== */
void *rex_malloc(
   unsigned long num_bytes
);

/* ==================================================================
FUNCTION REX_CALLOC
DESCRIPTION
   Dynamically allocates a specified number of blocks of the specified
   size and returns a pointer to this chunk of memory.
===================================================================== */
void *rex_calloc(
   unsigned long num_blocks,
   unsigned long block_size
);

/* ==================================================================
FUNCTION REX_REALLOC
DESCRIPTION
  Resizes the ptr block of memory to be size bytes while preserving the
  block's contents.  If the block is shortened, bytes are discarded off the
  end.  If the block is lengthened, the new bytes added are not initialized
  and will have garbage values.  If heap_ptr is NULL, the contents of ptr are
  unchanged and the function silently returns NULL.  If ptr is NULL, the
  functions behaves exactly like mem_malloc (see above).  If ptr is not NULL
  and size is 0, the function behaves exactly like mem_free (see below).  If
  the block cannot be resized, ptr is not NULL and size is not 0, then NULL
  is returned and the original block is left untouched.  If the ptr block IS
  successfully resized and the returned value is different from the ptr value
  passed in, the old block passed in must be considered deallocated and no
  longer useable (i.e. do NOT try to mem_free it!)  This function will ASSERT
  if it can detect a bad pointer or a pointer to a free block within the
  range of memory managed by heap_ptr.  However, it's not always possible to
  do this.  Passing in a ptr block that is outside the memory managed by
  heap_ptr will result in a silent return of NULL with the contents of ptr
  unchanged.  This function will call mem_malloc if it can't grow the block
  in place, so the allocater failed function WILL be called if the heap is
  out of room to grow the block.

  Returns a pointer to the beginning of the resized block of memory (which
  may be different than ptr) or NULL if the block cannot be resized.
===================================================================== */
void *rex_realloc(
   void         *ptr,
   unsigned long size
);

/* ==================================================================
FUNCTION REX_FREE
DESCRIPTION
   Frees a chunk of memory allocated using rex_malloc.
===================================================================== */
void rex_free(
   void *mem_ptr
);

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
);

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
void rex_set_best_task
(
  rex_tcb_type *start_tcb
);

/* ==================================================================
FUNCTION REX_SET_BEST_TASK_FROM_ALL

DESCRIPTION
   Searches the entire task list and sets the rex_best_task variable.

   NOTE: Interrupts must be locked when this function is called.

===================================================================== */
void rex_set_best_task_from_all( void );

/* --------------------------------------------------------------------------
**
** REX Profiling Support
**
** -------------------------------------------------------------------------- */

/* ==================================================================
FUNCTION REX_ENABLE_PROFILING

DESCRIPTION
   Turns on profiling.
===================================================================== */
void rex_enable_profiling( void );

/* ==================================================================
FUNCTION REX_DISABLE_PROFILING

DESCRIPTION
   Turns on profiling.
===================================================================== */
void rex_disable_profiling( void );

/* ==================================================================
FUNCTION REX_GET_TOTAL_TIME_SAMPLES

DESCRIPTION
   Returns the total number of time samples.
===================================================================== */
unsigned long rex_get_total_time_samples( void );

/* ==================================================================
FUNCTION REX_SET_TOTAL_TIME_SAMPLES

DESCRIPTION
   Sets the total number of time samples to the specified number.
===================================================================== */
void rex_set_total_time_samples( unsigned long );

/* ==================================================================
FUNCTION REX_GET_SLEEP_REPORT

DESCRIPTION
   Gets the number of sleep task time samples and the total number
   of time samples.
===================================================================== */
rex_sleep_report_type  rex_get_sleep_report( void );

/* ==================================================================
FUNCTION REX_ENABLE_SLEEP_REPORTING

DESCRIPTION
   Gets the number of sleep task time samples and the total number
   of time samples.
===================================================================== */
void  rex_enable_sleep_reporting(void);

/* ==================================================================
FUNCTION REX_DISABLE_SLEEP_REPORTING

DESCRIPTION
   Gets the number of sleep task time samples and the total number
   of time samples.
===================================================================== */
void  rex_disable_sleep_reporting(void);

/* --------------------------------------------------------------------------
**
** REX Program Counter Trace Support
**
** -------------------------------------------------------------------------- */

/* ==================================================================
FUNCTION REX_PCDUMP_INIT_REGISTERS

DESCRIPTION
   Initializes FIQ-mode registers used by the PC dumping section of
   the FIQ handler.
===================================================================== */
void rex_pcdump_init_registers(
   unsigned long start_address,
   unsigned long end_address,
   unsigned long current_address
);

/* ==================================================================
FUNCTION REX_ENABLE_PC_PROFILE

DESCRIPTION
   Turns on PC dumping.
===================================================================== */
void rex_enable_pc_profile( void );

/* ==================================================================
FUNCTION REX_DISABLE_PC_PROFILE

DESCRIPTION
   Turns off PC dumping.
===================================================================== */
void rex_disable_pc_profile( void );

/* ==================================================================
FUNCTION REX_CLEAR_PC_PROFILE

DESCRIPTION
   Turns off PC dumping.
===================================================================== */
void rex_clear_pc_profile( void );

/*===========================================================================
FUNCTION REX_KILL_TASK_EXT

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
);

/* ==================================================================
FUNCTION REX_DELETE_TASK_TIMERS

DESCRIPTION
   Removes all the timers that belong to a specified task from the
   REX timer list.
===================================================================== */
void rex_delete_task_timers(
   rex_tcb_type *tcb_ptr
);

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
);

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
);

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
);

/* ==================================================================
FUNCTION REX_SYS_GET_TIMER_LIST
DESCRIPTION
   Returns a list of the current rex timers, must pass in an array
   of REX_SYS_MAX_TIMER_IDS of rex_timer_types
===================================================================== */
void rex_sys_get_timer_list(
   rex_timer_type **timer_list
);

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
);

/*===========================================================================

FUNCTION REX_CLR_PROFILE

DESCRIPTION
  Clears stored profiling information. Clears the numticks and
  rex_IRQnumticks fields in each TCB in the task list. The rex_pnumticks
  does not need to be cleared because it would be in sync with rex_time_tick2.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_clr_profile( void );

/*===========================================================================

FUNCTION REX_INIT_PROFILE

DESCRIPTION
  Performs initialization necessary for software profiling. Sets the
  rex_time_tick2 variable to the address passed in.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_init_profile(
   dword rex_time_tick2_addr
);

/*===========================================================================

FUNCTION REX_ENABLE_PROFILE

DESCRIPTION
  Enables software profiling by setting rex_enable_profile, clearing
  rex_pnumticks and restarting the clock.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_enable_profile( void );

/*===========================================================================

FUNCTION REX_DISABLE_PROFILE

DESCRIPTION
  Disables software profiling by resetting rex_profile_enabled, clearing
  rex_pnumticks and stopping the clock.

DEPENDENCIES
  none.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_disable_profile( void );

/*===========================================================================

FUNCTION REX_GET_PROFILE

DESCRIPTION
  Retrieves software profiling information. Traverses rex_task_list and gets
  the numticks field from each TCB in the task list. In addition, gets
  rex_IRQnumticks as well.

DEPENDENCIES
  Assumes that the TCB of each task is in rex_task_list.

RETURN VALUE
  none.

SIDE EFFECTS
  none.
===========================================================================*/
void rex_get_profile(
   sw_profile_type *sw_profile,
   byte *num_task
);

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
);


/* ==========================================================================
FUNCTION REX_AUTODOG_DISABLE
DESCRIPTION
   Disables the auto dog service for a task.

   Returns the previous state of the autodog_enabled field in the TCB.
============================================================================= */
int rex_autodog_disable( void );


/*===========================================================================
FUNCTION REX_INIT_TIMER_LIST

DESCRIPTION
  Ininitializes the REX timer list by inserting the null timer after the head
  node.

===========================================================================*/
void rex_init_timer_list( void );

/*===========================================================================
FUNCTION  REX_CREATE_TASK
DESCRIPTION
   Similar to rex_def_task. REX allocates space for the tcb and
   the stack. Returns a handle to the thread or NULL on error.  Creates
   an independently dispatchable context.
===========================================================================*/
rex_tcb_type*
   /* TCB pointer on success, NULL otherwise
   */
rex_create_task(
   rex_stack_size_type  stack_size,
      /* Requested stack size
      */
   rex_priority_type    priority,
      /* Requested priority
      */
   rex_task_func_type   thread_ptr,
      /* Thread function
      */
   unsigned long        param,
      /* Parameter to pass to thread function when it is executed
      */
   char                 *thread_name,
      /* ASCII string name for the thread, can be NULL
      */
   boolean              suspended
      /* Indicated whether or not the thread should be started
      ** in the suspended state.  When TRUE, the thread will be
      ** created but only scheduable after a call to zrex_resume_thread,
      ** FALSE indicates the thread is immediately scheduable after
      ** creation
      */
);

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
);

/*==========================================================================
FUNCTION REX_GET_NUM_TASKS

DESCRIPTION
   Returns the number of currently dispatchable tasks, includes threads if
   FEATURE_REX_THREADS is on and threads are created, includes any task or
   thread regardless of its state (running, blocked, suspended, etc.)
============================================================================ */
int rex_get_num_tasks( void );

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
void rex_task_lock( void );

/*===========================================================================
FUNCTION rex_task_free

DESCRIPTION
  rex_task_free re-enables context switching after a rex_task_lock. Context
  switching is re-enabled if rex_task_free has been called as many times as
  rex_task_lock.

  rex_task_free has no effect if called from within an ISR.
===========================================================================*/
void rex_task_free( void );

/*===========================================================================
FUNCTION rex_tasks_are_locked

DESCRIPTION
  Returns 1 if tasks are currently locked and 0 otherwise.
===========================================================================*/
int rex_tasks_are_locked( void );

/*===========================================================================
FUNCTION rex_ints_are_locked

DESCRIPTION
  Returns 1 if interrupts are currently locked and 0 otherwise.
===========================================================================*/
int rex_ints_are_locked( void );

/*===========================================================================
FUNCTION rex_is_initialized

DESCRIPTION
  Returns 1 if Rex has been initialized and 0 if not.
===========================================================================*/
int rex_is_initialized( void );

/*===========================================================================
FUNCTION rex_set_dac

DESCRIPTION
  Sets the DAC variable in the rex TCB.

DEPENDENCIES
  Should be called after rex_init().

===========================================================================*/
void rex_set_dac( rex_tcb_type* tcb, unsigned int dac );

/*===========================================================================
FUNCTION rex_register_err_handler

DESCRIPTION
  Registers and err handler with rex.  Will be invoked by err syscall.

DEPENDENCIES
  None.

===========================================================================*/
void rex_register_err_handler(rex_err_handler_type handler);

/* Rex intlock stat profiling */
#ifdef FEATURE_REX_STAT
/*===========================================================================
FUNCTION rex_stat_enable/disable

DESCRIPTION
  Enabled or disables the rex int lock profiling code.  May be nested.
===========================================================================*/
void rex_stat_disable(void);
void rex_stat_enable(void);
#define REX_STAT_DISABLE() rex_stat_disable()
#define REX_STAT_ENABLE()  rex_stat_enable()
#else
#define REX_STAT_DISABLE()
#define REX_STAT_ENABLE()
#endif

/*==========================================================================
FUNCTION REX_SET_SMT_MASK

DESCRIPTION
  Binds the rex thread(specified by tcb) to the hardware threads 
  specified in hw_thread_bitmask.
  eg.if hw_thread_bitmask is
  0...010001
  the thread is bound to haredware thread 0 and 4.

============================================================================*/
void rex_set_smt_mask
(
   rex_tcb_type *p_tcb,
   unsigned int hw_thread_bitmask
);

#ifdef __cplusplus
   }
#endif

#endif /* END REX_H */
