/*=============================================================================

                  T I M E R   S E R V I C E   S U B S Y S T E M

GENERAL DESCRIPTION
  Implements time-keeping functions using the slow clock.

  Slow Clock Frequency          Granularity     Timer Range
    30.000kHz (lower limit)      33.3 us         39.7 hrs
    32.768kHz (nominal freq)     30.5 us         36.4 hrs
    60.000kHz (upper limit)      16.6 us         19.8 hrs


REGIONAL FUNCTIONS 

  timer_init()
    Initializes the timer subsytem.  Call only from time_init( ).


EXTERNALIZED FUNCTIONS

  timer_drv_def(timer, group, tcb, sigs, func, data)
    Defines and initializes a timer.
    Timers may also be staticly declared via the timer_drv_def() macro

  timer_def2(timer, group, func, data)
    Defines and initializes a timer, with the clock callback semantics.
    Timers may also be staticly declared via the TIMER_DEF2() macro

  timer_drv_set(timer, ticks, reload, unit)
    Sets an inactive timer to expire after a given period of time, or changes
    an active timer to expire after a given period of time.
    Optionally, specifies the timer to repeatly expire with a given period.

  timer_get(timer, unit)
    Get number of ticks before timer expires

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

    timer_drv_def() must be called, or the timer_drv_def( ) macro used, for each
      timer, before other timer functions are used with that timer.

    A valid sclk estimate is needed before timers can be set.


Copyright (c) 2003 - 2018 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/services/time/src/timer.c#5 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
22/02/17   abh     Initialize malloc memory for chunks to avoid corruption.
10/28/15   abh     Optimization changes in timer client chunks traversal logic.
10/19/15   abh     Added debug code changes for better logging.
04/03/14   psu     Update diag macros using optimized versions.
02/12/14   jb      Marked inline functions as extern for LLVM
06/20/13   ab      Added Handles code and error returns to Timer Apis.
04/26/11   din     Fixed compiler warnings.
03/11/11   ab      Removed compiler warnings.
03/10/11   ab      Fix a corner case when a small expiry timer is inserted 
                   immediately after a large expiry timer.
11/12/10   din     Added function to get time to next occuring non-deferrable 
                   timer expiry.
08/25/10   din     Removed call to timetick_get_sclk_offset in timer_expires_in.
04/27/10   din     Fixed compiler warnings.
12/18/09   din     Adding Timer List debugging feature.
12/10/09   din     Timer expiry now caters to changing sclk_delta
09/30/09   sri     Enhanced QXDM strings on timer_group NULL
05/22/09   din     Added missing refernce of zero in timer_drv_defer_match_interrupt 
05/22/09   din     Moved implementation of timer_set_next_interrupt to 
                   timetick_set_next_interrupt.
05/22/09   din     Removed FEATURE_TIMER_TASK. Removed Function timer_isr.
01/16/09   pbi     Moved MIN_REARM_SCLK definition to timetick.c
01/07/09   pbi     sclk_delta removed from timers_type
01/07/09   pbi     Removed timer_set_sclk_offset
03/25/09   din     In timer_expire(), cached callback functions from timer 
                   structure before first callback is made.
03/04/09   din     timer_drv_defer_match_interrupt returns zero when expiry of first 
                   non deferrable timers is in past.
02/23/09   din     Consoldation of time.
01/21/09   din     Forced timer_undefer_match_interrupt() to update MATCH_VAL
                   register. Updated timer_process_active_timers() to prevent
                   overflow in timer expiry past the active timer list zero.
01/16/09   din     Added missing reset of last_set_time in timer_restart().
01/13/09   din     Removed timer_delay_expiry_excluding_groups() and
                   timer_undelay_expiry().
01/12/09   din     Added deferrable attribute to timer_group. Also added 
                   lightweight API's to enable/ disable timer groups. API's
                   timer_drv_defer_match_interrupt(), timer_undefer_match_interrupt
                   & timer_group_set_deferrable().
01/05/09   din     Consolidation of time sources. Added 
                   FEATURE_TIME_UMTS_TARGETS. timer_set_next_interrupt was
           made uniform acroos different targets. Also Ensured that 
           timer_get() doesnot return 0.
12/23/08   tev     Remove timer_v.h inclusion.  No longer required now that
                   timer_v.h is obsolete.
12/15/08   tev     Explicitly include newly created private header file
                   timer_v.h
11/20/08   din     Always set set_time when match_value is updated in 
                   timer_set_next_interrupt(). Every attempt to set is recorded 
                   in last_set_time.
11/14/08   tev     Amended control for timer_restart to
                   defined(FEATURE_TIME_POWER_COLLAPSE) &&
                   !defined(FEATURE_TIME_REMOVE_TIMER_RESTART)
11/10/08   tev     Tentatively restored timer_restart under control of
                   FEATURE_TIME_POWER_COLLAPSE symbol definition.
10/17/08   din     Fixed initialization of do_reprogram_isr
10/16/08   din     Added lightweight mechanism to enable and disable timer groups 
                   during sleep. For use by the sleep subsystem only! APIs are
                   timer_delay_expiry_excluding_groups(), and timer_undelay_expiry()
                   (renamed timer_restart().) 
09/19/08   trc     timer_bin_find_first_exceeding() returns NULL upon failure
06/10/08   trc     Upon re-enabling a timer group, only nudge the active timer
                   list zero point as far back as the earliest expired timer
11/13/07   jhs     Removed timer_always_on_group.
08/31/07   jhs     Support for AMSS RESTART
05/29/07   jhs     Added in support to rewrite MATCH when writing match over a
                   tick boundary if FEATURE_TIME_REQUIRES_ODD_WRITE is defined.
04/19/07   gfr     Support for timer tracing.
01/12/07   trc     Increase MIN_REARM_SCLK from 6 to 9 slow clock cycles
12/04/06   trc     Bump up the # of timer bins from 64 to 128
10/11/06   trc     Reset match_value and set_value when sclk_delta is set
                   coming out of power collapse since they are no longer valid.
08/19/06   gb      Changed FEATURE_POWER_COLLAPSE to FEATURE_TIME_POWER_COLLAPSE. Made
                   changes for modem power collapse code.
08/15/06   jhs     Clear out match_value and set_value when sclk_delta is set
                   because they are no longer valid.
04/10/06   ajn     Prevent rounding nudge from creating a negative runtime.
04/07/06   gfr     Remove unneeded header file (msm_drv.h)
03/01/06   kap     Only unbin when timer->bin field is non-null
02/22/06   ajn     Add offset btw modem & MPM sclk counter for power collapse
02/10/06   kap/ajn Added Timer Optimization Changes (double linking/binning)
12/20/05   ejv     Added T_QSC60X0.
11/03/05   ajn     Zero-initialize the timer.cache fields in timer_define()
10/27/05   ajn     Always set set_time in timer_set_next_interrupt().
08/12/05   jhs     Moved apps processor to its own caching assembly function
                   for writing to MATCH_VAL.
08/05/05   ajn     MATCH_VAL written from caching assembly function.
07/29/05   ajn     Repeat MATCH_VAL writes until the write latches.
07/22/05   ajn     Don't write new MATCH_VAL until last write has latched.
07/06/05   ajn     Added timer_set_modem_app_sclk_offset (7500 APPS)
06/24/05   ajn     Include partial 7500 support
06/09/05   ajn     Corrected non-timer_task initialization.
05/19/05   ajn     Timer op's don't signal task or call process_active_timers.
05/09/05   ajn     Don't write to TIMETICK_MATCH_VAL if value is unchanged.
                   Fixed O(N^2) list insertion in timer_group_enable().
05/06/05   ajn     Cache time/reload values to avoid recalculating if unchanged
03/29/05   ajn     Replaced timer_ptr with timer_ptr_type, for L4 project.
03/16/05   ajn     Update next timer expiry interrupt w/o processing timers
                   when timers are removed from the active list.
03/16/05   ajn     Add ms rounding to following expiry in repeating timers.
12/06/04   lcc     In timer_process_active_timers, added reading of timetick
                   count to make sure it's non-zero before existing.  This is
                   needed for QUASar's timetick hw problem.
10/20/04   ajn     Moved timer expiry out of ISR context into a Timer task.
09/28/04   ajn     Catch use of timers w/o call to timer_define().
09/27/04   ajn     Force timer in the NULL timer group into an actual group.
08/30/04   ajn     Added "get next interrupt" and "interrupt early" functions
                   for TCXO shutdown.
07/06/04   ajn     timer_define() ERR's (not ERR_FATAL's) on active timer.
06/23/04   ajn     Corrected re-entrancy when processing active timers.
01/29/04   ajn     Corrected possible NULL pointer dereference.
01/09/04   ajn     Optimized the expiry-time advance of auto-reloading timers.
11/14/03   ajn     Added sclk timestamps for profiling timer_isr latency.
11/11/03   ajn     Added checks to detect timer_drv_def( ) calls on active timers
10/03/03   ajn     Call added to process_active_timers() in group_disable, to
                   ensure next active timer's interrupt occurs at proper time.
08/11/03   ajn     Added timer_pause/timer_resume for rex_set_timer replacement
08/08/03   ajn     Updated for coding standards
08/06/03   ajn     Added timer_def2(), for clk_reg() replacement
07/23/03   ajn     Moved _{to,from}_sclk functions to timetick.c
07/17/03   ajn     File created.

=============================================================================*/



/*=============================================================================


  *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE *** NOTE ***

  
  All comparisons in this module are done with respect to a moving origin,
  or "zero" point, to handle wrap-around of the 32-bit slow clock counter.
  
  For example, the conditional statement "if ( now >= expiry )" is wrong,
  since "now" could be 0xFFFFFF00, and "expiry" could be "0x00000100", ...
  "0x200" counts in the "future".
  
  The correct comparison would be "if ( now - zero >= expiry - zero )".
  This compares the current time (measured from some point in the past) to
  the expiry time (measured with respect to the same point in the past).
  If the "zero" origin was "0xFFFFFE00", with the above values, "now - zero"
  would be "0x100" and "expiry - zero" would be "0x300", making the entire
  expression FALSE; the expiry point has not been reached.

  If the expression "now - zero >= expiry - zero" returns TRUE, then it is
  safe to subtract "now - expiry" to determine how much time has passed since
  the expiry point.  If "now - zero >= expiry - zero" returns FALSE, the
  reverse subtraction is valid: "expiry - now" indicates how much time
  remains until the expiry point.

=============================================================================*/


/*=============================================================================

                           INCLUDE FILES

=============================================================================*/

#include "comdef.h"
#include "msg.h"
#include "timer_v.h"
#include "err.h"
#include "assert.h"
#include "stdint.h"
#include "DALSys.h"

#ifndef FEATURE_TIMER_USE_QURT_SYSCLOCK
#include "DDIInterruptController.h"
#include "DALStdErr.h"
#include "DALDeviceId.h"
#include "DalDevice.h"
#endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/
#ifdef FEATURE_TIME_QDSP6
#include "qurt.h"
#include "qurt_signal2.h"
#endif /*FEATURE_TIME_QDSP6*/

#if defined(FEATURE_TIMER_APQ8064)
#define TRAMP_SLP_TICK_ISR TRAMP_GP_TIMER_IRQ
#endif

#include <stringl/stringl.h>
#if defined(FEATURE_RCINIT)
#include "rcevt.h"
#include "rcinit.h"
#endif
#include "queue.h"
#include "time_timetick.h"
#include "timer_slaves.h"
#include "qurt_pimutex.h"
#include "qurt_cycles.h"


/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

/*-----------------------------------------------------------------------------
  Pool of timer bins
-----------------------------------------------------------------------------*/
#ifdef FEATURE_USE_TIMER_BIN
timer_bin_type              timer_bin_pool[ TIMER_BIN_NUM ];
#endif

static timers_type                timers=
{
  /* Faux timer group contain active timers & list of timer groups */
  {
    0,
	
    /* Active timers */
    {
      /* Slow clock "zero" base count for active timers list */
      0,

      /* Linked list of active timers */
      NULL
    },

    /* Disabled flag for the Faux Active Timer group.
       This group is always enabled.  When timers are disabled, they are moved
       off of this list */
    FALSE,

    /* Deferrable Flag */
    FALSE,

    /* Linked list of timer groups which have been disabled at some time. */
    &timers.active,

    /* Process index */
    0xFF,

	/* Group index */
    0xFF	 
  },

  /* Timestamp when call to _set_next_interrupt updates match_value */
  0,

  /* Timestamp of last attempt to update match value */
  0,

  /* Value passed to _set_next_interrupt */
  0,

  /* Value written to match count */
  0,

  /* Timestamp when timer isr occurred at */
  0,

  /* timers_process_active_timers is not executing */
  FALSE,

  /* Allow timer_isr to be set */
  TRUE,
  
  /* Active Timers count */
  0,
  
  /* Paused timers count */
  0,

  #ifdef FEATURE_USE_TIMER_BIN
  /* Active timer bin list */
  {
    /* First bin in active bin list */
    NULL,

    /* Last bin in active bin list */
    NULL,

    /* Number of bins in list */
    0
  },
  

  /* Free timer bin list */
  {
    /* First bin in free bin list */
    NULL,

    /* Last bin in free bin list */
    NULL,

    /* Number of bins in list */
    0
  }
  #endif /*endif FEATURE_USE_TIMER_BIN*/
  /* First Non-Deferrable Timer Pointer */
/* Timer Internal Pointer */
  NULL,
  
  NULL,
};


static timer_group_t *timer_internal_groups[TIMER_MAX_GROUPS] = {NULL};
static uint32 timer_next_free_group_idx = 0;
static uint32 timer_internal_groups_allocated = 0;
/* Buffer of timer_internal_type */
static timer_ptr_internal_type timer_buffer_ptr = NULL;

typedef struct timer_client_buffer_s
{
  timer_client_ptr_type           buffer;     /**< Chunk of memory for 
                                                               client timers */
  uint32                          buffer_idx; /** Index to next timer, probably free */

  uint16                          num_timers; /**< Number of timers in the chunk */
  
  uint16                          allocated_cnt; /**< Count of allocated timers in the chunk. */
} timers_client_buffer_type;
 
/* Timer client Chunks */
timers_client_buffer_type timer_client_chunks[TIMER_CLIENT_CHUNKS_MAX];
uint32 timer_client_chunks_allocated = 0;
uint32 timer_client_curr_chunk_idx = 0;


/* Count of total number of free client timers in in all the chunks */
uint32 timer_free_client_timers_cnt = 0;

/* Static array of client timers for the 1st chunk */
static timer_client_type timer_client_buffer[TIMER_CLIENT_BUFFER_INIT_ARRAY_SIZE][MAX_TIMER_BUFFER_NUMBER];

/* Global vars for storing timer chunk sizes */
uint32 timer_client_chunk_total_num_chunks = 0;

/* Queue to store free timer internal strcuture */
static q_type                     timer_free_q;

/* Indicated if timer_free_q in initialized */ 
static uint8 timer_q_init_status = 0;

/*handle for timetick*/
static DalDeviceHandle *hTimerHandle = NULL;

/* Global variable that has recent tick value */
time_timetick_type     ticks_now = 0;

/* Variables required for Handling timers of remote process */
static timer_process_cb_handler timer_remote_callback_handler = NULL;
static uint32 timer_curr_process_idx = 0;

/* Notification functions for indicating non-defer timer expiry changes */
typedef struct
{
  timer_notify_type         notify_type;
  time_osal_notify_obj_ptr  notify_obj;
  time_osal_notify_data     notify_mask;
  uint32                    process_idx;
} timer_non_defer_notification_client_type;

static timer_non_defer_notification_client_type timer_non_defer_expiry_notify_clients[TIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT];
static uint32 timer_non_defer_expiry_notify_clients_cnt = 0;

/*-----------------------------------------------------------------------------
  Timer trace data
-----------------------------------------------------------------------------*/

/* Stores number of timers present in timer_buffer */
static uint32 timer_count_buffer_allocated = 0;

#ifdef FEATURE_TIME_QDSP6

/*registration id obtained for timer signal registered with BLAST. This
 id will be used by timetick to program the match values for timers*/
static int timer_blast_sig_registration_id = -1;

/*Timer signal registered with BLAST. BLAST will set this signal to
 TIMER_QURT_SIG_MASK when the match value == timetick value*/
qurt_anysignal_t        timer_q6_signal;

/* Pointer to the mutex object for protecting critical-section of timer 
   code*/
qurt_mutex_t timer_qemu_mutex;

/*MACRO for locking/unlocking the qmutex*/
#define ATS_INTLOCK()  \
  qurt_pimutex_lock(&timer_qemu_mutex); \
  
#define ATS_INTFREE()  \
  qurt_pimutex_unlock(&timer_qemu_mutex); \

#define ATS_INTLOCK_SAV(X) \
  qurt_pimutex_lock(&timer_qemu_mutex); \
  
#define ATS_INTFREE_SAV(X)   \
  qurt_pimutex_unlock(&timer_qemu_mutex); \



#define TIMER_MAX_EXPIRY QURT_SYSCLOCK_MAX_DURATION_TICKS

static void timer_get_timetick_handle
(
  void
);

#ifdef FEATURE_TIMER_USE_QURT_SYSCLOCK
/*this function will be called in Q6 for updating the timer
 variables*/
static void timer_update_isr_time
(
  time_timetick_type* now
);
#endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

static void timer_init_qurt_signal
(
  void
);

#endif /*FEATURE_TIME_QDSP6*/

timer_error_type timer_get_clnt_int_timers(
timer_ptr_type timer,
uint32 process_idx,
timer_client_ptr_type * clnt_timer,
timer_ptr_internal_type * int_timer);

/* Function to create new client timer chunk */
timer_error_type timer_create_new_chunk(uint32 max_timers);

/*=============================================================================

                           TIMER DEBUG DATA

=============================================================================*/

/*-----------------------------------------------------------------------------
  Helper variables for the script
-----------------------------------------------------------------------------*/
/* Version information for the timer module */
ats_timer_version_type ats_timer_version = { 
		ATS_TIMER_VERSION_PERIPHERAL_ID,
		ATS_TIMER_MAJOR_VERSION,
		ATS_TIMER_MINOR_VERSION
		};

/*-----------------------------------------------------------------------------
  Active list check function prototype. (Enable FEATURE_TIMER_LIST_DEBUG to enable this.)
-----------------------------------------------------------------------------*/
static void timer_active_list_check(void);

/*-----------------------------------------------------------------------------
  Timer Trace Log
-----------------------------------------------------------------------------*/

#ifdef FEATURE_TIMER_TRACE

/* Declare the log structure */
timer_trace_type timer_trace;

#define TIMER_TRACE(event,ts,caller)  timer_trace_event(event, ts, caller)

#else /* FEATURE_TIMER_TRACE */

#define TIMER_TRACE(event,ts,caller)

#endif /* FEATURE_TIMER_TRACE */

/*-----------------------------------------------------------------------------
  Timer API Log
-----------------------------------------------------------------------------*/
#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES

timer_api_log_type timer_api_log[MAX_TIMER_API_CALLS];
static int timer_api_log_cnt = 0;

static void timer_api_log_func
(
  timer_api_type     event,
  timer_ptr_type     timer,
  timer_ptr_type     timer_actual_addr,
  time_timetick_type timetick,
  timer_error_type   ret_val,
  uint32             process_idx
);

#define TIMER_API_LOG_ENTRY(api,timer,timer_actual_addr,timetick,process_idx)    \
	    timer_api_log_func(api,timer,timer_actual_addr,timetick,TE_MAX,process_idx)    \

#define TIMER_API_LOG_EXIT(ret_val)    \
	    timer_api_log_func(TIMER_API_MAX,NULL,NULL,0,ret_val,0)    \

#else /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES */

#define TIMER_API_LOG_ENTRY(api,timer,timer_actual_addr,timetick,process_idx)
#define TIMER_API_LOG_EXIT(ret_val)

#endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES */

#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
/* Timer processing while loop cnt */
static uint32 timer_while_cnt = 0;

/* These values are set high to avoid the annoying F3 messages and an 
  * error fatal in case cb processing time is high.  1 sec = 19200000 ticks. 
  */
static uint64 timer_processing_time_warning_threshold = 19200;

#ifndef TIMER_ERROR_FATAL_THRESHOLD_ZEROED_OUT
static uint64 timer_processing_time_err_fatal_threshold=38400000;
#else
static uint64 timer_processing_time_err_fatal_threshold=0;
#endif


/*-----------------------------------------------------------------------------
  Record Match Val
-----------------------------------------------------------------------------*/
timer_match_value_record_type record_match_val[MAX_MATCH_REC];
static int record_match_val_counter = 0;

/*-----------------------------------------------------------------------------
  Timer Set log
-----------------------------------------------------------------------------*/
timer_set_type timer_sets[MAX_TIMER_SET];
static int timer_set_cnt = 0;
static int timer_set_cnt_global = 0;

/*-----------------------------------------------------------------------------
  Timer Clear log
-----------------------------------------------------------------------------*/
timer_clr_log_type timer_clr_log[MAX_TIMER_CLR_LOG];
static uint32 timer_clr_log_cnt = 0;

/*-----------------------------------------------------------------------------
  Timer Expired Debug log
-----------------------------------------------------------------------------*/
timer_expired_debug_type timer_expired_debug[MAX_TIMER_EXPIRED_DEBUG];
static uint32 timer_expired_debug_cnt = 0;

/*-----------------------------------------------------------------------------
  Timer expiry log (including the processing time)
-----------------------------------------------------------------------------*/
timer_expired_type timers_expired[MAX_TIMERS_EXPIRED];
static int timers_expired_cnt = 0;
int timers_expired_cnt_global = 0;

/*-----------------------------------------------------------------------------
  Timer remote expiry log
-----------------------------------------------------------------------------*/
/* For Remote process Timers */
timer_expired_type timers_remote_expired[MAX_TIMER_REMOTE_EXPIRED];
static int timer_remote_expired_cnt = 0;
int timer_remote_expired_cnt_global = 0;

/*-----------------------------------------------------------------------------
  Timer Defer Undefer log
-----------------------------------------------------------------------------*/
timer_defer_undefer_reference_count_log_type timer_defer_undefer_reference_count_log[MAX_TIMER_REFERNCE_COUNT_LOG];
static int timer_defer_undefer_reference_count_log_cnt = 0;

#endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES */

static boolean timer_slave_processing_enabled = FALSE; 

//extern timer_latency_profile_type timer_lat_profile;

/*=============================================================================

                             MACRO DEFINITIONS

=============================================================================*/

#define INDEX_TO_HANDLE(x)   ( (x) +1 )
#define HANDLE_TO_INDEX(x)   ( (x) -1 )
#define TIMER_HANDLE_CONSTANT 0xC3
#define GROUP_HANDLE_CONSTANT 0xC3C3

extern inline uint32 timer_index_to_handle(uint32 chunk_idx, uint32 timer_idx)
{
   uint32 handle;
   uint8 cid, tid1, tid0;
   
   cid = chunk_idx & 0xFF;
   tid1 = (timer_idx >> 8) & 0xFF;
   tid0 = timer_idx & 0xFF;
   
   handle = (uint32)(((cid ^ tid1 ^ tid0 ^ TIMER_HANDLE_CONSTANT) & 0xFF) << 24) | (uint32)((cid) << 16) | (uint32)((tid1) << 8) | (uint32)(tid0);
   return handle;
}
#define TIMER_INDEX_TO_HANDLE(cid, tid)   ( timer_index_to_handle(cid, tid) )

extern inline boolean timer_handle_to_index(uint32 handle, uint32 *chunk_idx, uint32 *timer_idx)
{
   uint8 temp, cid, tid1, tid0;
   
   if(chunk_idx == NULL || timer_idx == NULL)
      return FALSE;
   
   temp = (handle >> 24) & 0xFF;
   cid  = (handle >> 16) & 0xFF;
   tid1 = (handle >> 8) & 0xFF;
   tid0 = handle & 0xFF;
   
   if((cid ^ tid1 ^ tid0 ^ TIMER_HANDLE_CONSTANT) != temp)
      return FALSE;

   *chunk_idx = cid;
   *timer_idx = handle & 0xFFFF;
   return TRUE;
}
#define TIMER_HANDLE_TO_INDEX(h, cid_ptr, tid_ptr) (timer_handle_to_index(h, cid_ptr, tid_ptr))


static inline uint32 group_index_to_handle(uint32 group_idx)
{
   uint32 handle;
   handle = ((group_idx & 0xFFFF) ^ GROUP_HANDLE_CONSTANT)  << 16 | (group_idx & 0xFFFF);
   return handle;
}
#define GROUP_INDEX_TO_HANDLE(gid)   ( group_index_to_handle(gid) )

static inline boolean group_handle_to_index(uint32 handle, uint32 *group_idx)
{
   uint16 temp, gid;
   
   if(group_idx == NULL)
      return FALSE;
   
   temp = (handle >> 16) & 0xFFFF;
   gid  = handle & 0xFFFF;
   
   if((gid ^ GROUP_HANDLE_CONSTANT) != temp)
      return FALSE;

   *group_idx = gid;
   return TRUE;
}
#define GROUP_HANDLE_TO_INDEX(h, gid_ptr) (group_handle_to_index(h, gid_ptr))

/*=============================================================================

                             PROFILING DATA

=============================================================================*/


/*=============================================================================

                             FUNCTION DEFINITIONS

=============================================================================*/
/*=============================================================================

FUNCTION TIMER_GET_TIMETICK_HANDLE

DESCRIPTION
 This function is called from timer_init to pass timetick handle to timer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void timer_get_timetick_handle(void)
{

#ifdef FEATURE_TIMER_SLAVE1
  /* We want slave task to return the handle since slave needs the ptr too */
  timer_slaves_get_timetick_handle((void**)&hTimerHandle);
#else /* FEATURE_TIMER_SLAVE1 */
  DalTimetick_Attach("SystemTimer", &hTimerHandle);
  if(hTimerHandle == NULL)
  {
    ERR_FATAL("hTimerHandle == NULL", 0, 0, 0);
    return;
  }
#endif /* FEATURE_TIMER_SLAVE1 */

  /* Enable system timer */
  DalTimetick_Enable(hTimerHandle, 1);
  DalTimetick_InitTimetick64(hTimerHandle);
}



timers_type*  timer_get_active_list(void)
{
  return &timers;
}

#ifdef FEATURE_TIMER_TRACE
/*=============================================================================

FUNCTION TIMER_TRACE_EVENT

DESCRIPTION
  This function saved a timer trace event packet.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

static void timer_trace_event
(
  timer_trace_event_type event,
  uint64 ts,
  timer_match_interrupt_setter_type caller
)
{
  /* Pointer to the trace structure to fill in */
  uint32 index;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Get the next trace structure */
  index = timer_trace.index;

  /* Update the timer trace fields */
  timer_trace.events[index].event     = event;
  timer_trace.events[index].ts        = ts;
  timer_trace.events[index].mv        = timers.match_value;
  timer_trace.events[index].mv_setter = caller;

  /* Increment the index */
  TIMER_INC_MOD(timer_trace.index, MAX_TIMER_TRACE);

} /* timer_trace_event */

#endif /* FEATURE_TIMER_TRACE */

#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
/*=============================================================================

FUNCTION TIMER_API_LOG_FUNC

DESCRIPTION
  This function logs all of the api calls along with
  the thread ID.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

static void timer_api_log_func
(
  timer_api_type     event,
  timer_ptr_type     timer,
  timer_ptr_type     timer_actual_addr,
  time_timetick_type timetick,
  timer_error_type   ret_val,
  uint32             process_idx
  
)
{
  timer_client_ptr_type clnt_timer;
  timer_ptr_internal_type int_timer;

  if (event != TIMER_API_MAX)
  {
    /* Record Event Type */
    timer_api_log[timer_api_log_cnt].api  = event;
	
    /* Record ptr to TCB */
    timer_api_log[timer_api_log_cnt].tid  = qurt_thread_get_id();

    /* Record timetick value */
    timer_api_log[timer_api_log_cnt].ts   = timetick;
  
    /* Record ptr to timer */
    timer_api_log[timer_api_log_cnt].timer = timer_actual_addr;

	if(timer!=NULL)
	{
	  timer_get_clnt_int_timers(timer,
	  	                        process_idx,
	  	                        &clnt_timer,
	  	                        &int_timer);
      timer_api_log[timer_api_log_cnt].clnt_timer = clnt_timer;
      timer_api_log[timer_api_log_cnt].int_timer = int_timer;
	}

	timer_api_log[timer_api_log_cnt].ret_val = TE_MAX;
  
	/* Enable this FEATURE in timer scons to capture the API exit time as well */
	#ifdef FEATURE_TIMER_API_LOG_PROFILING
	  timer_api_log[timer_api_log_cnt].exit_ts = 0;
	#endif /* FEATURE_TIMER_API_LOG_PROFILING */


  }
  else
  {
    timer_api_log[timer_api_log_cnt].ret_val = ret_val;

    /* Enable this FEATURE in timer scons to capture the API exit time as well */
    #ifdef FEATURE_TIMER_API_LOG_PROFILING
	 timer_api_log[timer_api_log_cnt].exit_ts = timer_get_timetick();
    #endif /* FEATURE_TIMER_API_LOG_PROFILING */

    TIMER_INC_MOD(timer_api_log_cnt, MAX_TIMER_API_CALLS);
  }

  
} /* timer_api_log_func */
#endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES */



/*=============================================================================

FUNCTION TIMER_NOTIFY_FIRST_NON_DEFER_TIMER_EXPIRY_CHANGE

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

static void timer_notify_first_non_defer_timer_expiry_change(void)
{
   int i;
   timer_notify_type         notify_type;
   time_osal_notify_obj_ptr  notify_obj;
   time_osal_notify_data     notify_mask;
   uint32                    process_idx;

   for(i=0; i<timer_non_defer_expiry_notify_clients_cnt; i++)
   {
   
    notify_type = timer_non_defer_expiry_notify_clients[i].notify_type;
    notify_obj = timer_non_defer_expiry_notify_clients[i].notify_obj;
    notify_mask = timer_non_defer_expiry_notify_clients[i].notify_mask;
    process_idx = timer_non_defer_expiry_notify_clients[i].process_idx;

    if(notify_obj == NULL)         
      ERR_FATAL("Notify_obj NULL means corruption happened; i=%d", i, 0, 0);     
	 
    /* If there are signals to be set, and a task / signal object to set the signals to,
            then set those signals in that task's task control block or the passed signal object */
	if (process_idx == timer_curr_process_idx )
	{
		if ( notify_type == TIMER_NATIVE_OS_SIGNAL_TYPE )
		{
		  #ifdef FEATURE_TIME_QDSP6
		   qurt_anysignal_set((qurt_anysignal_t *)notify_obj, notify_mask);
		  #else
		   ERR_FATAL("Signal type cannot be NATIVE OS type",0,0,0);
		  #endif /* FEATURE_TIME_QDSP6 */
		}
     	else if ( notify_type == TIMER_REX_TCB_SIGNAL_TYPE )
     	{
		  #ifdef FEATURE_TIMER_REX_IS_AVAILABLE
		   (void) rex_set_sigs( notify_obj, notify_mask );
		  #else
		   ERR_FATAL("Signal type cannot be rex type",0,0,0);
		  #endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
		}
		else if ( notify_type == TIMER_NATIVE_OS_SIGNAL2_TYPE )
		{
		  #ifdef FEATURE_TIME_QDSP6
		   qurt_signal2_set((qurt_signal2_t *)notify_obj, notify_mask);
		  #else
		   ERR_FATAL("Signal type cannot be NATIVE OS type",0,0,0);
		  #endif /* FEATURE_TIME_QDSP6 */
		}
		else if ( notify_type == TIMER_DALSYS_EVENT_SIGNAL_TYPE )
		{
		  timer_dalsys_sig_set(notify_obj);
		}
		else
		{
		  ERR_FATAL("Invalid Notify type 0x%x, may be corruption",notify_type,0,0);
		}
	}
	else
	{
      if(timer_remote_callback_handler == NULL)
        {
          ERR_FATAL("No Shim Cb func. Call timer_set_remote_process_timer_handler()", 0, 0, 0);
          return;
      	}
	  else
	  	{
          timer_remote_callback_handler(process_idx, TIMER_NON_DEFER_NOTIFICATION_MOCK_PTR, notify_type, 
                          notify_obj, notify_mask, 0);          
	  	}
	}
   } /* for() */
     
   return;
} /* timer_notify_first_non_defer_timer_expiry_change */


/*=============================================================================

FUNCTION TIMER_GET_FIRST_ACTIVE_NON_DEFERRABLE_TIMER

DESCRIPTION
  This function returns the first non-deferrable timer on the active
  timer list.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the pointer to the first non-deferrable internal timer.
  If there are no non-deferrable timers on the active list, the return NULL. 

SIDE EFFECTS
  None.

=============================================================================*/

static void timer_update_first_active_non_deferrable_timer(void)
{
  timer_ptr_internal_type ptimer;
  time_timetick_type p1_expiry = TIMER_MAX_EXPIRY;
  time_timetick_type old_expiry = TIMER_MAX_EXPIRY;
  time_timetick_type new_expiry = TIMER_MAX_EXPIRY;

  if(timers.first_non_deferrable)
  	old_expiry = timers.first_non_deferrable->expiry;  

  for( ptimer = timers.active.list.first; ptimer != NULL; ptimer = ptimer->next )
  {
    /* If timer belongs to a group, which is non-deferrable */
    if (timer_internal_groups[ptimer->info.group_idx]->deferrable == FALSE )
    {
      /* Found the first non-deferrable timer on the active list */
      p1_expiry = ptimer->expiry;
      break;
    }
  }

  /* If there was a non-deferrable timer on the active list, then check the expiry value */
  if (p1_expiry == TIMER_MAX_EXPIRY)
  {
    /* There are no active non-deferrable timers */
    timers.first_non_deferrable = NULL;
  }
  else
  {
    /* First non-deferrable timer is on the active list */
    timers.first_non_deferrable = ptimer;
	new_expiry = p1_expiry;
  }

  if(old_expiry != new_expiry)
  	timer_notify_first_non_defer_timer_expiry_change();
  
}/* timer_update_first_active_non_deferrable_timer */

#if 0 /* This function is not being used and hence generating an unused
 function warning. Hence, not compiling this function*/
/*=============================================================================

FUNCTION TIMER_CONV_TO_SCLK_FREQ

DESCRIPTION
  This function returns 32kHz timetick value based on 19MHz timetick value.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 32kHz timetick value. 

SIDE EFFECTS
  None.

=============================================================================*/

static inline timetick_type timer_conv_to_sclk_freq(uint64 time_in)
{
  return (timetick_type)(time_in/XO_TO_SCLK_CONV_FACTOR);
} /* timer_conv_to_sclk_freq */


/*=============================================================================

FUNCTION TIMER_CONV_TO_XO_FREQ

DESCRIPTION
  This function returns 19MHz timetick value based on 32kHz timetick value.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 19MHz timetick value. 

SIDE EFFECTS
  None.

=============================================================================*/

static inline uint64 timer_conv_to_xo_freq(timetick_type time_in)
{
  return ((uint64)time_in)*((uint64)XO_TO_SCLK_CONV_FACTOR);
} /* timer_conv_to_xo_freq */

#endif /*end #if 0*/
/*=============================================================================

FUNCTION TIMER_TASK_ISR

DESCRIPTION
  This function detects a timer interrupt, and forwards it to the timer task
  for processing.

DEPENDENCIES
  Must only be called from tramp.

RETURN VALUE
  NULL pointer (No return value).

SIDE EFFECTS
  Uninstalls the timer ISR.
  Signals the timer task to process the expiring timer, which can have
  numerous side effects, depending on the expiring timer.

=============================================================================*/
static const void * timer_task_isr( void* input )
{
  /* Timer sclk time-stamp values */
  #ifdef TIMER_DEBUG_LATE_TIMER_ISR
  time_timetick_type                   set_time, set_value;

  /* Latency in ISR handling */
  time_timetick_type                   latency_sclk;
  #endif /*TIMER_DEBUG_LATE_TIMER_ISR*/

  time_timetick_type                   now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Disable interrupt since QTMR irq remains high */
  /* We need to disable the interrupt before we signal in order to avoid race cond'n */
  DalTimetick_Enable(hTimerHandle, 0);

  /* Get the timetick, before signalling timer task */
  DalTimetick_GetTimetick64(hTimerHandle, &now);

  #ifdef FEATURE_TIME_QDSP6
  /* Signal the timer task of the timer interrupt event */
  (void) qurt_anysignal_set(&timer_q6_signal, TIMER_QURT_SIG_MASK);
  #else /*FEATURE_TIME_QDSP6*/
  /* Signal the timer task of the timer interrupt event */
  (void) rex_set_sigs( timer_tcb_ptr, TIMER_EXPIRY_SIG );
  #endif /*FEATURE_TIME_QDSP6*/

  ATS_INTLOCK( );

  /* Save a trace packet */
  TIMER_TRACE(TIMER_TRACE_ISR, now, TIMER_MVS_TIMER_UNDEFINED);
  
  /* Record when timer isr actually was handled */
  timers.isr_time = now;

  /* Capture timer set values, for messaging outside of ATS_INTLOCK. */
  #ifdef TIMER_DEBUG_LATE_TIMER_ISR
  set_time  = timers.set_time;
  set_value = timers.set_value;
  #endif /*TIMER_DEBUG_LATE_TIMER_ISR*/

  ATS_INTFREE();

  #ifdef TIMER_DEBUG_LATE_TIMER_ISR
  if ( now - set_time >= set_value - set_time)
  {
    /* Determine the difference between when the interrupt was programmed to
       occur at, and when the interrupt was actually handled. */
    latency_sclk = now - set_value;

    if ( latency_sclk > TIMER_SCLK_LATENCY )
    {
      /* Timer interrupt was handled over btw 5 (38MHz) to 10ms (19MHz) late.
         (Timers may expire much later than 5-10ms, if they are in a disabled
         group, or if multiple timers expire due to the same interrupt.
         This is only a measure of the timer isr latency.) */

      MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Late Timer ISR: ST=%d SV=%d IT-SV=%d",
                set_time, set_value, latency_sclk);
    }
  }
  else
  {
    MSG_3(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Early Timer ISR: ST=%d SV=%d SV-IT=%d",
             set_time, set_value, set_value - now);
  }
  #endif /*TIMER_DEBUG_LATE_TIMER_ISR*/

  return NULL;
} /* timer_task_isr */


/*=============================================================================

FUNCTION TIMER_SET_NEXT_INTERRUPT

DESCRIPTION
  Program the SLEEP_XTAL_TIMETICK to generate an interrupt at the given value

DEPENDENCIES
  Internal timer_ function.
  Must be called from ATS_INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt when slow clock counter reaches given value

=============================================================================*/

static void timer_set_next_interrupt
(
  /* Slow clock count at which next interrupt will occur */
  time_timetick_type                   match_count,

  /* Current slow clock count */
  time_timetick_type                   ticks_now,

  /* If TRUE, forces MATCH_VAL register to be updated */
  boolean                         force,

  uint8                           caller
)
{
  /* temp timetick variable */
  time_timetick_type                   ts;

   /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Don't re-write the same value to the MATCH_VAL register. 
    Only if the timer_undefer_reference_count is zero, it means
    that NPA is done with its job and we can start programming the interrupt.*/
  if ((force == TRUE || ( match_count != timers.set_value  &&  
         match_count != timers.match_value )))       
  {
    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    record_match_val[record_match_val_counter].mv_supplied = match_count;
    record_match_val[record_match_val_counter].int_timer = timers.active.list.first;
    record_match_val[record_match_val_counter].mv_set = MAGIC_INITIALIZER_64;
    record_match_val[record_match_val_counter].ts = ticks_now;
    record_match_val[record_match_val_counter].post_set_ts = MAGIC_INITIALIZER_64;
    record_match_val[record_match_val_counter].mv_setter = caller;
    #endif

    /* If timer is being programmed in context of lower priority thread may
	   lead to preemption of thread leading timer in disabled state.
	   So setting Dummy Signal to timer task when timer being programmed
	   so that thread priority can be bump by qurt kernel because now timer
	   task would be waiting for mutex held by lower priority task leading to 
	   bumping up lower task priority by qurt kernel.*/
    if(caller != TIMER_MVS_TIMER_PROCESSED)
       qurt_anysignal_set(&timer_q6_signal, TIMER_DUMMY_MASK);	

    /* disable interrupts */
    DalTimetick_Enable(hTimerHandle, 0);

    /* Featurize since this function is called in case of sysclock and without */  
    #ifndef FEATURE_TIMER_USE_QURT_SYSCLOCK
    DalTimetick_SetNextInterrupt64(hTimerHandle,
                                   (DalTimetickTime64Type)match_count,
                                   (DalTimetickTime64Type)ticks_now,
                                   &timers.match_value);

    #else /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

    #ifdef USE_QURT_DIRECT
    timers.match_value = qurt_sysclock_alarm_create (timer_blast_sig_registration_id, ticks_now, match_count);
    #else  /* USE_QURT_DIRECT */
    timers.match_value = timetick_set_next_interrupt_tmp64(match_count, ticks_now);
    #endif /* USE_QURT_DIRECT */

    #endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

	ts = timetick_get_tmp64();

    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    record_match_val[record_match_val_counter].mv_set = timers.match_value;
    record_match_val[record_match_val_counter].post_set_ts = ts;
	TIMER_INC_MOD(record_match_val_counter,MAX_MATCH_REC);
    /* This is a dummy statement to remove compiler warning */
    record_match_val[record_match_val_counter].ts = \
                  record_match_val[record_match_val_counter].ts;
    #endif


    /* Save a trace packet with the match value */
    TIMER_TRACE( TIMER_TRACE_MATCH_VALUE_PROG, ts, caller );
    
    /* Record when this value was actually written */
    timers.set_time    = ticks_now ;
    
    /* Record when next timer should expire at */
    timers.set_value = match_count;    
  }

  /* Re-enable interrupts */
  DalTimetick_Enable(hTimerHandle, 1);

  /* Record when this match value was logically written, even if we don't
     bother to write the value if it is unchanged. */
  timers.last_set_time = ticks_now ;

} /* timer_set_next_interrupt */



/*=============================================================================

FUNCTION TIMER_UPDATE_TIMER_INTERRUPT

DESCRIPTION
  If the head of the timer list is modified, update the timer match value.
  Will cause an interrupt to be generated immediately if there is an
  expired timer.

DEPENDENCIES
  Internal timer_ function.
  Must be called from ATS_INTLOCKED context.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt when slow clock counter reaches the first timer's expiry point

=============================================================================*/

static void timer_update_timer_interrupt
(
  uint8 caller,

  time_timetick_type now
)
{

  /* Time of first expiring timer */
  time_timetick_type                   first;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */


  /* NOTE: This function has many early returns. */

  /* Skip updating the interrupts if not allowed */
  if ( FALSE == timers.do_reprogram_isr )
  {
    return;
  }

  /* If a timer is being altered inside "timer_process_active_timers" ... */
  if ( timers.processing )
  {
    /* ... don't bother updating the timer match value!  Caller will. */
    return;
  }



  /* The following two tests deal with timers.set_value, which is the time
     of the Already Programmed timer interrupt, which may have no relation to
     the first timer on the active timer list. */


  /* Get the current time */
  //DalTimetick_GetTimetick64(hTimerHandle, &now);


  /* If the timer interrupt has already occurred ... */
//  if ( timers.set_value - timers.set_time <= now - timers.set_time )
//  {
//    /* ... let the Timer ISR/Task update the next timer match value */
//    return;
//  }


  /* If the timer interrupt is just a little in the future ... */
//  if ( timers.set_value - now <= timetick_get_min_rearm_sclk_tmp64() )
//  {
//    /* ... we can't stop it from been generated.  Don't try to. */
//    return;
//  }

  
  /* The remaining tests deal with the first timer (if any) on the active
     timer list. */


  /* Are there timers on the timer list? */
  if ( timers.active.list.first != NULL )
  {
    /* Get the time of the first expiring timer */
    first = timers.active.list.first->expiry;

    /* If the first expiring timer matches the timer.set_value, ... */
    if ( first == timers.set_value )
    {
      /* ... then it is already properly programmed in TIMETICK_MATCH */
      return;
    }

    /* If the first expiring timer is still in the past ... */
    if ( first  <= now  )
    {
      /* Set the timer for "as soon as possible" (eg, "now") */
      first = now;
    }
  }
  else
  {
    /* If no timers, set "first" to as far in the future as possible */
    //first = timers.active.list.zero - 1;
    // in Badger, the max duration is controlled by macro QURT_SYSCLOCK_MAX_DURATION_TICKS
    first = TIMER_MAX_EXPIRY;
  }


  /* Set the next interrupt match value (unless it is unchanged) */
  timer_set_next_interrupt( first, now, FALSE, caller);

} /* timer_update_timer_interrupt */



/*=============================================================================

FUNCTION TIMER_INSERT_BETWEEN

DESCRIPTION
  Places 'timer' before 'ptr1' and after 'ptr2' in 'list'.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void timer_insert_between
(
  /* List to insert timer into */
  timer_list_ptr                  list,

  /* Timer to insert into list */
  timer_ptr_internal_type         timer, 

  /* Timer to insert after */
  timer_ptr_internal_type         ptr1,

  /* Timer to insert before */
  timer_ptr_internal_type         ptr2
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update our timer's prev/next ptrs to point at correct timer structs */

  timer->prev = ptr1; 
  timer->next = ptr2;


  /* Update surrounding prev/next ptrs (if necessary) to point to our 
     newly inserted timer */

  if( ptr1 != NULL )
  {
    ptr1->next = timer;
  }
  else
  {
    list->first = timer; /* We have a new start of list, update first ptr */
  } 

  if( ptr2 != NULL )
  {
    ptr2->prev = timer;
  }
  else
  {
    list->last = timer; /* We have a new end of list, update last ptr */
  }
 
} /* timer_insert_between */



/*=============================================================================

FUNCTION TIMER_INSERT

DESCRIPTION
  Inserts a timer into the sorted list of timers.
  Timers are sorted according to increasing expiration times

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_insert
(
  /* List timer is to be inserted into */
  timer_list_ptr                  list,

  /* Timer to be inserted into list of active timers */
  timer_ptr_internal_type         timer,

  time_timetick_type              now
)
{
  /* Pointer to a timer. Used for walking list of timers */
  timer_ptr_internal_type         ptr;
    
  #ifdef FEATURE_USE_TIMER_BIN

  /* Pointer to a timer. Determines where on the list to start looking */
  timer_ptr_internal_type         start;

  /* bin timetick bounds */
  time_timetick_type                   bin_lower, bin_upper;
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
    
  /* First set the list field of the timer */
  timer->list = list;

#if FEATURE_USE_TIMER_BIN
  /* if this timer is to be inserted on the active list */
  if( list == &timers.active.list )
  {

    /* If this timer is going on the active list we can use binning 
       to help us cut down on the list search time */

     /* Obtain the first timer from the bin exceeding this timer's expiry */
    start = timer_bin_find_first_exceeding( timer ); 
     
  }
  else
  {
    /* If this is not an active timer, do not optimize the search,
       just search from the start of the list */
    start = list->first;
  }

  if(start != NULL && start->bin != NULL)
  {
     bin_lower = start->bin->lower;
     bin_upper = start->bin->upper;
     
     /* Ignore the bin's first timer to insert new timer into timers list
        if the 'now is in between bin's upper and lower boundary' and first  
        timer of the bin is before now. In such case, Search from start of
        timers list to insert new timer. */
     if(now > bin_lower && now < bin_upper && start->expiry < now) 
        start = NULL;
  }
  
  /* Initialize the start position */
  if( start != NULL )
  {
    ptr = start;
  }
  else
  {
    ptr = list->first;
  }

#endif

  /* if this timer is to be inserted on the active list */
  if( list == &timers.active.list )
  {
    /* Increment the number of timers */
    timers.active_timers_count++; 
  }
  
  /* Search for appropriate list location to insert timer */
  ptr = list->first;
  while ( ptr != NULL &&  ptr->expiry <= timer->expiry )
  {
    ptr = ptr->next;
  }

  /* Insert the timer into the list */
  timer_insert_between( list, timer, ptr ? ptr->prev : list->last, ptr ); 

#if FEATURE_USE_TIMER_BIN
  /* Only bin this timer if we are inserting into the active list */
  if( list == &timers.active.list )
  {
    /* Insert thie timer into the appropriate timer list bin */
    timer_bin_insert( timer_bin_find_create(timer), timer );
  }
#endif 

  /* Update the first non-deferrable timer */
  if (timer_internal_groups[timer->info.group_idx]->deferrable == 0 &&
      (timers.first_non_deferrable == NULL ||
       timer->expiry < timers.first_non_deferrable->expiry))
  {
    /* Set this timer as the first non-deferrable
       (regardless of if it's p0 or not) */
    timers.first_non_deferrable = timer;

	/* Notify the clients who are interested in non deferrable timer change */
	timer_notify_first_non_defer_timer_expiry_change();
  }

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timer_sets[timer_set_cnt].ext_timer = timer->timer_ext->timer_client_ptr;
  timer_sets[timer_set_cnt].int_timer = timer;
  timer_sets[timer_set_cnt].clnt_timer = timer->timer_ext;
  timer_sets[timer_set_cnt].ts = now;
  timer_sets[timer_set_cnt].expiry = timer->expiry;
  timer_sets[timer_set_cnt].tid = qurt_thread_get_id();
  #ifdef FEATURE_USE_TIMER_BIN
    timer_sets[timer_set_cnt].bin = timer->bin;
  #endif
  TIMER_INC_MOD(timer_set_cnt,MAX_TIMER_SET);
  timer_set_cnt_global++;
  
  /* This is a dummy statement to remove compiler warnings */
  timer_sets[timer_set_cnt].ts = timer_sets[timer_set_cnt].ts;
  #endif 

  /* Throw an error fatal if timer list is corrupted */
  timer_active_list_check();

} /* timer_insert */



/*=============================================================================

FUNCTION TIMER_INSERT_LIST

DESCRIPTION
  Inserts a sorted list of timers into the sorted list of timers.
  Timers are sorted according to increasing expiration times

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_insert_list
(
  /* List timers are to be inserted into */
  timer_list_ptr                  dst_list,

  /* Linked list of timers to be inserted into the first list */
  timer_ptr_internal_type         src_list


)
{
  /* A timer from the second list, to be inserted in the first */
  timer_ptr_internal_type         timer;

  time_timetick_type              now;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Repeat for all timers on the source list */
  while ( src_list != NULL )
  {
    /* Retrieve a timer from the start of the source list */
    timer    = src_list;
    src_list = src_list->next;
    DalTimetick_GetTimetick64(hTimerHandle, &now);
    timer_insert( dst_list, timer, now );
  }

} /* timer_insert_list */



/*=============================================================================

FUNCTION TIMER_CONCATENATE_LISTS

DESCRIPTION
  Concatenates the src_list and the dst_list with the combined list taking on
  the identity of the dst_list.  src_list is cleared.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_concatenate_lists
(
  /* Linked list of timers to be inserted into the first list */
  timer_list_ptr                  src_list,

  /* List timers are to be inserted into */
  timer_list_ptr                  dst_list
)
{
  /* timer iteration pointer */
  timer_ptr_internal_type         timer;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if(src_list == NULL )
  {
     ERR_FATAL("src_list == NULL", 0, 0, 0);
     return;
  }

  /* Check if the source list is empty */
  if( src_list->first == NULL )
  {
    return;
  }

  /* Stitch the two lists together */

  /* Is the destination list empty? */
  if( dst_list->first != NULL )
  { 
    /* No, so update the head's previous pointer */
    dst_list->first->prev = src_list->last;

    /* Update the source list's last next pointer */
    src_list->last->next = dst_list->first; 

  }
  else
  {
    /* Yes, so update the destination list's last pointer */
    dst_list->last = src_list->last;
  }

  /* Update the destination lists's first pointer */
  dst_list->first = src_list->first;
 
  /* Now, for each of the src list items, update their structures */
  timer = src_list->first;
  while( timer != NULL )
  {
    /* Update the list membership */
    timer->list = dst_list;

    /* If this is the last of the src list, we are done */
    if( timer == src_list->last )
    {
      break;
    }

    /* Proceed to next timer */
    timer = timer->next; 
  }

  /* Finally update the source list */
  src_list->first = NULL;
  src_list->last  = NULL;
    
} /* timer_concatenate_lists */



/*=============================================================================

FUNCTION TIMER_UNLINK

DESCRIPTION
  Simply unlinks a timer from its current list of timers.
  Updates the associated list first/last ptrs

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_unlink
(
  /* Timer to remove from its current list */
  timer_ptr_internal_type        timer
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( timer->prev != NULL )
  {
    timer->prev->next = timer->next;
  }
  else
  {
    /* This was the head, so update first ptr */
    timer->list->first = timer->next;
  }

  if( timer->next != NULL )
  {
    timer->next->prev = timer->prev;
  }
  else
  {
    /* This was the tail, so updated last ptr */
    timer->list->last = timer->prev;
  }

} /* timer_unlink */

/*=============================================================================

FUNCTION TIMER_GET_INTERNAL_TIMER

DESCRIPTION
  This function returns pointer to internal timer.

DEPENDENCIES
  None.

RETURN VALUE
  Returns relevant pointer to internal timer structure. 
  If handle is invalid returns NULL.

SIDE EFFECTS
  This function should be called inside an intlock or a mutex.

=============================================================================*/

timer_ptr_internal_type timer_get_internal_timer
(
  timer_client_ptr_type   timer
)
{
  int32 temp_index;
  timer_ptr_internal_type temp_int = NULL;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(timer == NULL)
     return temp_int;
  
  temp_index = HANDLE_TO_INDEX( timer->timer_int_handle );

  if ( temp_index == TIMER_BUFFER_INVALID_HANDLE )
  {
    return temp_int;
  }

  if ( temp_index > MIN_TIMER_BUFFER_NUMBER
       && temp_index < MAX_TIMER_BUFFER_NUMBER )
  {
    temp_int = &(timer_buffer_ptr[temp_index]);    
  }
  return temp_int;
} /* timer_get_internal_timer */

/*=============================================================================

FUNCTION TIMER_FREE_INTERNAL_TIMER

DESCRIPTION
  1. Puts the timer back in free q
  
DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void timer_free_internal_timer
(
  timer_ptr_internal_type timer  
)
{
  /* Temporary index */
  int32                          temp_index;

   if ( timer->info.node_state == NODE_IS_FREE )
   { 
     ERR_FATAL ( "Removing int_timer which is not active",0,0,0);
     return;
   }
   
   /* Save index value in a temp var */
   temp_index = timer->index;

   /* Clear expired timer buffer */
   memset( timer, 0, sizeof( timer_internal_type ));

   /* Restore timer index*/
   timer->index = temp_index ;

   /* Note: Dont have reference to external structures from internal functions */
   //if ( ext_timer != NULL )
   //{
   //  ext_timer->timer_int_handle = TIMER_BUFFER_INVALID_HANDLE ;
   //}
   //else
   //{
   //  ERR_FATAL ( " Timer must have an external timer ", 0, 0, 0);
   //  return;
   //}

   timer->info.node_state = NODE_IS_FREE;
  
   /* Link timer_buffer element back to timer_free_q */
   (void) q_link( timer, &(timer->link) );

   /* Enqueue node in timer_free_q */
   q_put( &timer_free_q, &(timer->link) );   

   timer_count_buffer_allocated = MAX_TIMER_BUFFER_NUMBER - q_cnt(&timer_free_q);
} /* timer_free_internal_timer */


/*=============================================================================

FUNCTION TIMER_REMOVE

DESCRIPTION
  Removes a timer from the list of timers.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_remove
(
  /* Timer to be removed from list of active timers */
  timer_ptr_internal_type         timer
)
{

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( timer == NULL || timer->list == NULL )
  {
    return;
  }

  if( timer->list == &timers.active.list )
  {
    /* Decrement the number of timers if timer belongs to active list */
    timers.active_timers_count--; 
  }

 #if FEATURE_USE_TIMER_BIN
  /* if this timer is to be removed from the active list */
  if( timer->bin != NULL )
  {
    /* Unbin this timer */
    timer_bin_remove( timer );
  }
#endif

  /* Fix up links/list around this node */

  if ( timer->prev )
  {
    timer->prev->next = timer->next;
  }
  else
  {
    timer->list->first = timer->next;
  }

  if ( timer->next )
  {
    timer->next->prev = timer->prev;
  }                                 
  else
  {
    timer->list->last = timer->prev;
  }


  timer->next = NULL;
  timer->prev = NULL;
  timer->list = NULL;

  /* Update the first non-deferrable timer */
  if (timers.first_non_deferrable == timer)
  {
    /* Since the first non-deferrable timer was removed,
       we need to get the next non-deferrable timer */
    timer_update_first_active_non_deferrable_timer();
  }

  /* Throw an error fatal if timer list is corrupted */
  timer_active_list_check();

} /* timer_remove */



/*=============================================================================

FUNCTION TIMER_REMOVE_HEAD

DESCRIPTION
  Removes the head of the specified timer list.

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_remove_head
(
  /* List to chop head off of */
  timer_list_ptr                  list
)
{
  /* Head of the timer list */
  timer_ptr_internal_type         head = list->first;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if( list == &timers.active.list )
  {
    /* Decrement the number of timers if timer belongs to active list */
    timers.active_timers_count--; 
  }

  /* New front of list */
  list->first = head->next;

  /* Fix up links/list around this node */
  if( list->first != NULL )
  {
    /* Update next node's prev ptr */
    list->first->prev = NULL;
  }
  else
  {
    /* Empty list */
    list->last = NULL;
  }


  /* NULL out deleted node's link fields */
  head->next = NULL;
  head->list = NULL;

  /* Update the first non-deferrable timer */
  if (timers.first_non_deferrable == head)
  {
    /* Since the first non-deferrable timer was removed,
       we need to get the next non-deferrable timer */
    timer_update_first_active_non_deferrable_timer();
  }

  /* Throw an error fatal if timer list is corrupted */
  timer_active_list_check();

} /* timer_remove_head */



/*=============================================================================

FUNCTION TIMER_APPEND

DESCRIPTION
  Appends the list with a timer
  The appended timer's list member is updated

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_append
(
  /* List to add timer to */
  timer_list_ptr                  list,

  /* Timer to add to list */
  timer_ptr_internal_type         timer
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Update timer's links */
  timer->prev = list->last;
  timer->next = NULL;

  /* Update the old tail to point to this new timer */
  if( list->last != NULL )
  {
    list->last->next = timer;
  }
  else
  {
    /* Empty list, have a new head */
    list->first = timer;
  } 
  
  /* Always have a new tail */ 
  list->last = timer;
 
  /* We have a new list for timer */ 
  timer->list = list;

} /* timer_append */

#if 0    /* Currently no where being used, if required can enable again */
/*=============================================================================

FUNCTION TIMER_IN_LIST

DESCRIPTION
  Returns whether the timer actually exists in the specified list

DEPENDENCIES
  Internal timer_ function.
  Must be called from inside ATS_INTLOCK.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static boolean timer_in_list
(
  /* Timer under test */
  timer_ptr_internal_type         timer,

  /* List timer may be a member of */
  timer_list_ptr                  list
)
{
  /* Used to walk timer list */
  timer_ptr_internal_type         ptr; 

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Do the search */
  for( ptr = list->first; ptr != NULL; ptr = ptr->next )
  {
    if( ptr == timer )
    {
      /* Timer is in list */
      return TRUE;
    }
  }

  /* Timer was not in list */
  return FALSE;

} /* timer_in_list */
#endif /* #if 0*/

/*=============================================================================
FUNCTION TIMER_SET_REMOTE_PROCESS_CB_HANDLER

Sets the API that needs to be called when a remote process timer expires.
We will provide to this callback function, the timer callback details
=============================================================================*/
void timer_drv_set_remote_process_cb_handler(timer_process_cb_handler shim_cb_func)
{
   timer_remote_callback_handler = shim_cb_func;
} /* timer_drv_set_remote_process_cb_handler */

/*=============================================================================
FUNCTION TIMER_DRV_SET_LOCAL_PROCESS_IDX

Local Process Index needs to be set by timer shim layer so that remote timers
can be differentiated from local process timers
=============================================================================*/
void timer_drv_set_local_process_idx(uint32 local_process_index)
{
   timer_curr_process_idx = local_process_index;
} /* timer_drv_set_local_process_idx */

/*=============================================================================

FUNCTION TIMER_REMOTE_EXPIRE

DESCRIPTION
  Calls Shim layer callback function which which will push client timer's callback
   details into that process pipe/queue according to its implementation.

DEPENDENCIES
  Internal timer_function()

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/
static void timer_remote_expire
(
  /* Expiring timer to be processed */
  timer_client_ptr_type           timer,

  /* Duration timer ran for. */
  time_timetick_type              run_time_ms,
  
  time_timetick_type*             time_now
)
{
  /*to logs when the cb processing starts*/
  time_timetick_type cb_processing_started = 0;

  /*to log when the cb processing ends*/
  time_timetick_type cb_processing_ended = 0;

  /*to log the processing time*/
  time_timetick_type cb_processing_time = MAGIC_INITIALIZER_64;
  
  /*fn address*/
  timer_t1_cb_type  fn_address_of_timer = timer->cbfn.func1;
    
  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_post_cmd_params_type slave_cmd;
  memset(&slave_cmd, 0, sizeof(timer_slave_post_cmd_params_type));
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timers_remote_expired[timer_remote_expired_cnt].timer_expired	= timer->timer_client_ptr;
  timers_remote_expired[timer_remote_expired_cnt].info			= timer->info;
  timers_remote_expired[timer_remote_expired_cnt].sigs_func_obj	= timer->cbfn.sigs_obj;
  timers_remote_expired[timer_remote_expired_cnt].processing_started	= *time_now;
  timers_remote_expired[timer_remote_expired_cnt].processing_time		= MAGIC_INITIALIZER_64;
  #endif

  /*log the time when processing started*/
  cb_processing_started = *time_now;

  /* Call Shim callback function Initially which should be 
     called back for expiring other process related timers */
  if(timer_remote_callback_handler == NULL)
  {
     ERR_FATAL("No Shim Cb func. Call timer_set_remote_process_timer_handler()", 0, 0, 0);
     return;
  }
  
  if ( timer->info.callback_type == TIMER_NATIVE_OS_SIGNAL_TYPE ||
       timer->info.callback_type == TIMER_NATIVE_OS_SIGNAL2_TYPE ||
       timer->info.callback_type == TIMER_REX_TCB_SIGNAL_TYPE ||
       timer->info.callback_type == TIMER_FUNC1_CB_TYPE ||
       timer->info.callback_type == TIMER_FUNC2_CB_TYPE)
  {
  /* Call Shim layer callback which will push callback details into process pipe/queue */
  timer_remote_callback_handler(timer->info.process_idx, timer->timer_client_ptr, timer->info.callback_type, 
                          timer->cbfn.sigs_obj, timer->cbdt.data, run_time_ms);
  }
  else if( timer->info.callback_type != TIMER_NO_NOTIFY_TYPE)
  {
     ERR_FATAL("callback type seems to be corrupted timer: 0x%x, callback_type: %d, func obj: 0x%x", 
                                                                        timer->timer_client_ptr, 
                                                                        timer->info.callback_type, 
                                                                        fn_address_of_timer);
  }
  
  /*log the cb ended time*/
  DalTimetick_GetTimetick64(hTimerHandle, &cb_processing_ended);
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

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES

  timers_remote_expired[timer_remote_expired_cnt].processing_time = cb_processing_time;
  TIMER_INC_MOD(timer_remote_expired_cnt,MAX_TIMER_REMOTE_EXPIRED);
  timer_remote_expired_cnt_global++;

  /* generate a warning or error fatal out if the processing time > threshold*/
  if(cb_processing_time > timer_processing_time_warning_threshold)
  {
    MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"callback fn = 0x%x took %d ticks, max = %d ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_processing_time_warning_threshold);          
  }
  

  if(timer_processing_time_err_fatal_threshold != 0 && 
     cb_processing_time > timer_processing_time_err_fatal_threshold)
  {
    ERR_FATAL("callback fn = 0x%x took %d ticks, err thre = %d ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_processing_time_err_fatal_threshold);          
  }

  #endif /*(defined FEATURE_ENABLE_TIMER_DEBUG_VARIABLES)*/
} /* timer_remote_expire */


/*=============================================================================

FUNCTION TIMER_EXPIRE

DESCRIPTION
  Processes an expiring timer, according to the timer's definition.

DEPENDENCIES
  Internal timer_ function()
  Must be called from inside ATS_INTLOCK & TASKLOCK (or interrupt) contexts

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

static void timer_expire
(
  /* Expiring timer to be processed */
  timer_client_ptr_type           timer,

  /* Duration timer ran for. */
  time_timetick_type              run_time_ms,
  
  time_timetick_type*             time_now
)
{
  /* Timer 'type 1' calllback function */
  timer_t1_cb_type                cb1;

  /* Timer 'type 2' calllback function */
  timer_t2_cb_type                cb2;

  /*to logs when the cb processing starts*/
  time_timetick_type cb_processing_started = 0;

  /*to log when the cb processing ends*/
  time_timetick_type cb_processing_ended = 0;

  /*to log the processing time*/
  time_timetick_type cb_processing_time = MAGIC_INITIALIZER_64;
  
  /*fn address*/
  uint32  fn_address_of_timer = 0;
    
  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_post_cmd_params_type slave_cmd;
  memset(&slave_cmd, 0, sizeof(timer_slave_post_cmd_params_type));
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timers_expired[timers_expired_cnt].timer_expired	= timer->timer_client_ptr;
  timers_expired[timers_expired_cnt].info			= timer->info;
  timers_expired[timers_expired_cnt].sigs_func_obj	= timer->cbfn.sigs_obj;
  timers_expired[timers_expired_cnt].processing_started	= *time_now;
  timers_expired[timers_expired_cnt].processing_time		= MAGIC_INITIALIZER_64;
  #endif  

  /* Cache information from timer structure since it could get deleted any
  time after the first callback is made */
  cb1 = timer->cbfn.func1;
  cb2 = timer->cbfn.func2;
  fn_address_of_timer = (uint32)cb1;

  /*log the time when processing started*/
  cb_processing_started = *time_now;



  /* If signal object is native os object, call native os function to set sigs */

  if ( timer->info.callback_type == TIMER_NATIVE_OS_SIGNAL_TYPE )
  {
    qurt_anysignal_set((qurt_anysignal_t *)timer->cbfn.sigs_obj, timer->cbdt.sigs);
  }
  else if (timer->info.callback_type == TIMER_NATIVE_OS_SIGNAL2_TYPE) 
  {
    qurt_signal2_set((qurt_signal2_t *)timer->cbfn.sigs_obj, timer->cbdt.sigs);
  }
  
  /* If there are signals to be set, and a task to set the signals to,
     then set those signals in that task's task control block */
  else if( timer->info.callback_type == TIMER_REX_TCB_SIGNAL_TYPE )
  {
#ifdef FEATURE_TIMER_REX_IS_AVAILABLE
    rex_set_sigs((rex_tcb_type *)timer->cbfn.sigs_obj, timer->cbdt.sigs);
#else
    ERR_FATAL("On adsp, signal cannot be of rex type. Cb type: %d, sigs_obj: 0x%x, sigs: 0x%x", timer->info.callback_type, timer->cbfn.sigs_obj, timer->cbdt.sigs);
#endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
  }
  else if ( timer->info.callback_type == TIMER_DALSYS_EVENT_SIGNAL_TYPE )  
  {      
    timer_dalsys_sig_set(timer->cbfn.sigs_obj);  
  }  

  /* If the timer was defined with a type-1 style function callback,
     call the function, or post the function as an asynchronous call */

  else if ( timer->info.callback_type == TIMER_FUNC1_CB_TYPE)
  {
    fn_address_of_timer = (uint32)cb1;
    
    #ifdef FEATURE_TIMER_SLAVE1
    slave_cmd.timer_ext = timer->timer_client_ptr;
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

  else if ( timer->info.callback_type == TIMER_FUNC2_CB_TYPE)
  {
    fn_address_of_timer = (uint32)cb2;

    /* Call the callback function */
    #ifdef FEATURE_TIMER_SLAVE1
    slave_cmd.timer_ext = timer->timer_client_ptr;
    slave_cmd.cb_type   = TIMER_SLAVE_CB_TYPE2;
    slave_cmd.cb2       = cb2;
    slave_cmd.run_time  = (time_timetick_type)run_time_ms;
    slave_cmd.data      = timer->cbdt.data;
    slave_cmd.timer_slave_task_num = timer->info.slave_task_idx;
    timer_slave_post_cmd(&slave_cmd);
    #else
    cb2( (time_timetick_type) run_time_ms, timer->cbdt.data);
    #endif
  }
  else if( timer->info.callback_type != TIMER_NO_NOTIFY_TYPE)
  {
     ERR_FATAL("callback type seems to be corrupted timer: 0x%x, callback_type: %d, func obj: 0x%x", 
                                                                        timer->timer_client_ptr, 
                                                                        timer->info.callback_type, 
                                                                        fn_address_of_timer);
  }

  /* log the cb ended time */
  DalTimetick_GetTimetick64(hTimerHandle, &cb_processing_ended);
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

  /*generate a warning or error fatal out if the processing time > threshold*/
  #if (defined FEATURE_ENABLE_TIMER_DEBUG_VARIABLES)
  timers_expired[timers_expired_cnt].processing_time = cb_processing_time;
  TIMER_INC_MOD(timers_expired_cnt,MAX_TIMERS_EXPIRED);
  
  if(cb_processing_time > timer_processing_time_warning_threshold)
  {
     MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR, "callback fn = 0x%x took %d ticks, max = %d ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_processing_time_warning_threshold);
  }
  

  if(timer_processing_time_err_fatal_threshold != 0 && 
     cb_processing_time > timer_processing_time_err_fatal_threshold)
  {
    ERR_FATAL("callback fn = 0x%x took %d ticks, err thre = %d ticks",
               fn_address_of_timer,
               cb_processing_time,
               timer_processing_time_err_fatal_threshold);
  }

  #endif /*(defined FEATURE_ENABLE_TIMER_DEBUG_VARIABLES)*/
} /* timer_expire */


/*=============================================================================

FUNCTION TIMER_PROCESS_ACTIVE_TIMERS

DESCRIPTION
  Process timers, decrementing elapsed time from each one.

DEPENDENCIES
  Internal timer_ function()
  Must be called from inside ATS_INTLOCK & TASKLOCK (or interrupt) contexts, to
  prevent a large delay between reading the current timetick count, and
  writing the new timetick match value.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

void timer_process_active_timers( void )
{
  /* Timer being processed */
  timer_ptr_internal_type              timer;

  /* Length of time the timer ran for */
  time_timetick_type                   run_time;
  
    /* Duration timer ran for, in milliseconds */
  time_timetick_type                   run_time_ms = 0;

  /* Duration timer ran for, in milliseconds */
  time_timetick_type                   run_time_sclk;


  /* Minimum advance required for reloading timer */
  time_timetick_type                   min_advance;

  /* Temporary value to compute the new expiry point */
  time_timetick_type                   new_expiry;

  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  /* Temporary value to compute the new expiry point */
  time_timetick_type                   local_timer_expiry;
  #endif

  /* Client timer ptr */
  timer_client_ptr_type                clnt_timer;
  
  /* Temp client timer object */
  timer_client_type                    temp_clnt_timer;

  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Lock interrupts while testing & manipulating the active timer list */
  ATS_INTLOCK();
  
  /* Record the fact that timers are being processed, to prevent re-entry
     into this function, and unnecessary modification of TIMETICK_MATCH. */
  timers.processing = TRUE;

  /* Get current slow clock count */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  /* Save a trace packet */
  TIMER_TRACE(TIMER_TRACE_PROCESS_START,ticks_now,TIMER_MVS_TIMER_UNDEFINED);

  /*record the time at which the blast signal came*/
  #ifdef FEATURE_TIMER_USE_QURT_SYSCLOCK
  timer_update_isr_time(&ticks_now);
  #endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

  /* Check to determine if the timer at head of the active timer list has
     past its expiry point.  If it has, remove it, expire the timer, and
     repeat with the new timer at the head of active timer list.
       
     Note: a timer's expiry can change timers.active.list.zero, so no local
     variable should hold a calculated value involving it from one iteration
     to the next. */

#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timer_while_cnt = 0;
#endif 

  /* Imp note: Donot fire future timers. This gives problem if users are setting timers 
     near to 1920 ticks which will get impacted due to software delays too.
     Firing little later is always fine */
  while ( timer = timers.active.list.first,
          timer != NULL
            &&  timer->expiry
                 <= ticks_now + TIMER_OS_EARLY_TOL)
  {
    /* Get Client Timer obj */
    clnt_timer = timer->timer_ext;
    if(clnt_timer->timer_state != TIMER_SET_FLAG && clnt_timer->timer_state != TIMER_RESUMED_FLAG)
       ERR_FATAL("Timer corruption. state: 0x%x", clnt_timer->timer_state, 0, 0);
    
#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    timer_while_cnt++;
    timer_expired_debug[timer_expired_debug_cnt].whileloopcnt= timer_while_cnt;
    timer_expired_debug[timer_expired_debug_cnt].ext_timer	 = clnt_timer->timer_client_ptr;
    timer_expired_debug[timer_expired_debug_cnt].clnt_timer  = clnt_timer;
    timer_expired_debug[timer_expired_debug_cnt].int_timer	 = timer;
    timer_expired_debug[timer_expired_debug_cnt].ts 		 = ticks_now;
    timer_expired_debug[timer_expired_debug_cnt].start		 = timer->start;
    timer_expired_debug[timer_expired_debug_cnt].expiry 	 = timer->expiry;
    timer_expired_debug[timer_expired_debug_cnt].info		 = clnt_timer->info;
    timer_expired_debug[timer_expired_debug_cnt].deferrable	 = timer_internal_groups[clnt_timer->info.group_idx]->deferrable;	
    timer_expired_debug[timer_expired_debug_cnt].sigs_func   = clnt_timer->cbfn.sigs_obj;
    timer_expired_debug[timer_expired_debug_cnt].data_mask   = (uint32)clnt_timer->cbdt.data;
      
    TIMER_INC_MOD(timer_expired_debug_cnt,MAX_TIMER_EXPIRED_DEBUG);

#endif /*FEATURE_ENABLE_TIMER_DEBUG_VARIABLES*/

	#ifdef FEATURE_USE_TIMER_BIN
	if( timer->bin != NULL ) {
		/* Unbin this timer */
		timer_bin_remove( timer );
	}
	#endif

    /* Remove expiring timer from active timer list */
    timer_remove_head( &timers.active.list );
      
    /* Compute length of time the timer ran for.  This may be longer than
       programmed due to sleep or other delays in expiry processing. */
    run_time = ticks_now - timer->start;

    /* Compute time (in milliseconds) timer has been running for.
       This may be larger than expected if the timer expires during sleep,
       or otherwise has its expiry processing delayed */
    run_time_ms = timetick_cvt_from_sclk_tmp64( run_time, T_MSEC);

    #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    local_timer_expiry = timer->expiry;
    #endif

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
          timer->expiry = TIMER_MAX_EXPIRY; 
          
        }

      /* Record the new start time for the next expiry */
      timer->start = ticks_now;
      
        /* If a timer is a reloading timer, the millisecond conversion will have
           a rounding error.  We can adjust the start time to account for the
           residual.  This is done before signalling the timer expiry, since
           the timer expiry might alter the timer data */
      if ( clnt_timer->info.callback_type == TIMER_FUNC2_CB_TYPE)
      {
        if ( timer->list != NULL )
        {
          /* Timer is still on the list.  It is a reloading timer. */

          /* Difference btw run_time & run_time_sclk is rounding error (+/-.5ms) */
          run_time_sclk = timetick_cvt_to_sclk_tmp64( run_time_ms, T_MSEC);

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
      }
      
      /* Insert timer back in active list */
      timer_insert( &timers.active.list, timer, ticks_now );

      clnt_timer->timer_state      = TIMER_SET_FLAG; 
      clnt_timer->timer_state_at   = ticks_now; 
    }
    else
    {
      /* Free internal timer object */
      timer_free_internal_timer(timer);

      clnt_timer->timer_state      = TIMER_EXPIRED_FLAG; 
      clnt_timer->timer_state_at   = ticks_now; 
      clnt_timer->timer_int_handle = TIMER_BUFFER_INVALID_HANDLE;
    }
    
      /* After intfree() call, timer may get deleted or cleared. Therefore, backup func, sigs details before releasing lock */
      memscpy(&temp_clnt_timer, sizeof(temp_clnt_timer), clnt_timer, sizeof(timer_client_type));
      
      /* We've finished manipulating the active timer list.  Unlock
       interrupts for the duration of the timer's expiry processing */
      ATS_INTFREE();
	  
      if(temp_clnt_timer.info.remote_process == TIMER_FROM_REMOTE_PROCESS)
      {
         #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
	     timers_remote_expired[timer_remote_expired_cnt].expiry_value =  local_timer_expiry;
		 #endif /*FEATURE_ENABLE_TIMER_DEBUG_VARIABLES*/
		 
         /* Expire timer */
         timer_remote_expire(&temp_clnt_timer, run_time_ms, &ticks_now);
      }
      else
      {
         #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
	     timers_expired[timers_expired_cnt].expiry_value =  local_timer_expiry;
		 #endif /*FEATURE_ENABLE_TIMER_DEBUG_VARIABLES*/
        /* Expire timer */
        timer_expire( &temp_clnt_timer, run_time_ms, &ticks_now );
      }

    /* NOTE: The above call may run user code, which can do anything ...
       including installing and uninstall other timers.  This can cause
       the value of timers.active.list.zero to change at this point.
       We must use the new value in the next iteration, so any local
       variable which holds computations involving that value must
       be considered invalid after the above call.  */


    /* Lock interrupts in preparation of the next iteration of the loop,
       which tests & manipulates the active timer list */
    ATS_INTLOCK();

    /* Get current slow clock count */
    DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  } /* while timers on timer.active.list are expiring */
  
#ifndef FEATURE_TIME_QDSP6

  #ifndef TIMETICK_INT_EDGE_TRIGGERED

  /* Install the timer expiry ISR */
  tramp_set_isr( TRAMP_SLP_TICK_ISR, timer_task_isr );

  /* Ensure interrupt source has deasserted before continuing.
     This must occur before the timer_set_next_interrupt(), to prevent
     accidently clearing the interrupt for the next timer. */
  tramp_block_till_deasserted( TRAMP_SLP_TICK_ISR, 30 );

  #endif /* !TIMETICK_INT_EDGE_TRIGGERED */
#endif

  /* Timers that expire at and before "ticks_now" have been processed.
     Set interrupt for when next timer expires. */

  if ( timers.active.list.first != NULL )
  {
    /* The first timer on the active list is the timer next to expire */
    timer_set_next_interrupt( timers.active.list.first->expiry, 
                              ticks_now, TRUE, TIMER_MVS_TIMER_PROCESSED );
  }
  else
  {
    /* There is no next timer.  Set interrupt to a long, long time from now,
       about ~1.5 days.  This will be reset when a new timer is defined. */
    //timer_set_next_interrupt( ticks_now - 1, ticks_now, TRUE,TIMER_MVS_TIMER_PROCESSED );
    // in Badger, the max duration is controlled by macro QURT_SYSCLOCK_MAX_DURATION_TICKS
    timer_set_next_interrupt( QURT_SYSCLOCK_MAX_DURATION_TICKS, ticks_now, TRUE, TIMER_MVS_TIMER_PROCESSED );
  }

  /* Timer processing has completed */
  timers.processing = FALSE;

  /* Save a trace packet */
  TIMER_TRACE(TIMER_TRACE_PROCESS_END,timetick_get_tmp64(),TIMER_MVS_TIMER_UNDEFINED);

  /* We've finished manipulating the active timer list.  */
  ATS_INTFREE();
  
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

} /* timer_process_active_timers */



/*=============================================================================

FUNCTION TIMER_TASK

DESCRIPTION
  Initializes the Timer Task

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_task
(
  /* Unused parameter - required to match rex_task_func_type prototype */
  dword                           ignored    /*lint -esym(715,ignored) */
)
{
  /* Signals returned from rex wait procedure */
  time_osal_notify_data           sigs;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  #ifndef FEATURE_TIMER_USE_QURT_SYSCLOCK
  if(DAL_SUCCESS != DalInterruptController_RegisterISR(timers.dal_int_handle,
                                           TIMER_INTERRUPT_ID,
                                           (DALISR)timer_task_isr,
                                           NULL,
                                           DALINTRCTRL_ENABLE_LEVEL_HIGH_TRIGGER))
  {
      ERR_FATAL("ISR registration failed!", 0, 0, 0);
      return; 
  }
  #endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

 /* Signal the task in order to prime the timer processing mechanism */
 (void) qurt_anysignal_set(&timer_q6_signal, TIMER_QURT_SIG_MASK);


#if defined(FEATURE_RCINIT)
  rcinit_handshake_startup();
#endif

  //time_timetick_init();

  for(;;)
  {

    sigs = qurt_anysignal_wait (&timer_q6_signal, TIMER_QURT_TASK_SIGNAL_MASK);
    if (sigs & TIMER_QURT_SIG_MASK) 
    {
      /* Clear the timer expiry signal */
      qurt_anysignal_clear (&timer_q6_signal, TIMER_QURT_SIG_MASK);


      /* Process expiring timer(s) */
      timer_process_active_timers();
    }
    if ( sigs & TIMER_ALLOCATE_CHUNK_SIG )
    {
      /* Clear the timer expiry signal */
      qurt_anysignal_clear (&timer_q6_signal, TIMER_ALLOCATE_CHUNK_SIG);
	  
      /* Allocate a new client timer chunk.
            * This is done to keep the chunk allocation inside timer task only.
            * Here we are allocating the chunk beforehand.
            */
      (void)timer_create_new_chunk(MAX_TIMER_BUFFER_NUMBER);
    }
    if ( sigs & TIMER_DUMMY_MASK )
    {
      /* Clear the timer expiry signal */
      qurt_anysignal_clear (&timer_q6_signal, TIMER_DUMMY_MASK);

	  ATS_INTLOCK();
      /*Dummy signal is being set to bump up priority of lower priority tasks
        performing timer activity. timer task would be waiting for ATS Mutex
		acquired by lower priority task leading to bumping up priority of lower 
		priority task by qurt kernel to avoid lower priority to be preempt by
		higher priority task*/
      ATS_INTFREE();
    }
  }
} /* timer_task */


/* If FEATURE_TIMER_TASK is defined, then the time tick interrupt is
** serviced in the task context.  The timer task should be started
** before enabling the interrupt so that the task is running when an
** interrupt occurs.
** */

void timer_task_init(void) /* Time services */
{
   
  time_timetick_init();
  timer_init_qurt_signal();
  timer_init();
  RCINIT_INFO handle = rcinit_lookup("timer");

  if (RCINIT_NULL != handle)
  {
    rcinit_initfn_spawn_task(handle, timer_task);
  }
}


/*=============================================================================

FUNCTION TIMER_INIT                                                    REGIONAL

DESCRIPTION
  Initializes the 32kHz slow clock based Timer Subsytem

DEPENDENCIES
  Must only be called from time_init()

RETURN VALUE
  None

SIDE EFFECTS
  May install an ISR for the 32-bit slow clock match register

=============================================================================*/

void timer_init( void )
{
  #ifdef FEATURE_USE_TIMER_BIN
  /* Loop index for timer bin initialization */
  int                             i;
  
  /* Timer bin */
  timer_bin_ptr                   bin;
  #endif

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  #ifndef FEATURE_TIMER_USE_QURT_SYSCLOCK
  /* Dal Result */
  DALResult                       eRes;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  eRes = DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER, &timers.dal_int_handle);
  if( eRes != DAL_SUCCESS || timers.dal_int_handle == NULL )
  {
    /* Error out, we should not start up timer task if we cannot set interrupts through DAL */
    ERR_FATAL("timer_init: Couldn't obtain DalInterrupt Handle",0,0,0);
  }
  #endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/
  
  /* Re-populate the ATS timer version structure, so that compiler does not optimizes it. */
  ats_timer_version.peripheral = ATS_TIMER_VERSION_PERIPHERAL_ID;
  ats_timer_version.major_ver = ATS_TIMER_MAJOR_VERSION;
  ats_timer_version.minor_ver = ATS_TIMER_MINOR_VERSION;
  
  /*initialize the mutex here*/
  qurt_pimutex_init ( &timer_qemu_mutex );

  //time_timetick_enable(); /* TODO: This should be fine right? */
  timer_get_timetick_handle();

  /* Get current slow clock count */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_INIT, NULL, NULL, ticks_now, TIMER_DUMMY_PROCESS_INDEX);
  TIMER_API_LOG_EXIT(TE_SUCCESS);

  /* Initialize the timer_bin_pool and free list */
#ifdef FEATURE_USE_TIMER_BIN
  for(i = 0; i < TIMER_BIN_NUM; i++)
  {
    bin = &timer_bin_pool[i]; 

    /* init the bin's data */
    bin->first    = NULL;
    bin->last     = NULL;
    bin->lower    = 0;
    bin->upper    = 0;
    bin->count    = 0;

    /* push it on the free list */
    timer_bin_list_push( &timers.bins_free, bin);
  }
#endif

  /* Initialize the list of active timers to contain no timers */
  timers.active.list.first = NULL;
  timers.active.list.last  = NULL;

  /* Initialize timers.set_value, set_time values & last_set_time for tests 
     in set_next_interrupt() and update_timer_interrupt */
  timers.set_value         = ticks_now - 2;
  timers.set_time          = ticks_now - 1;
  timers.last_set_time     = ticks_now - 1;
  timer_v_init();  
#ifndef FEATURE_TIME_QDSP6

  #ifdef TIMETICK_INT_EDGE_TRIGGERED
  
  /* Setup edge-sensitive interrupt, instead of having the interrupt
     asserted for 30us */
  TIMETICK_INT_EDGE_TRIGGERED_INIT();

  #endif /* TIMETICK_INT_EDGE_TRIGGERED */

#endif

  #ifdef FEATURE_TIMER_SLAVE1
  timer_slave_processing_enabled = TRUE;
  #else
  timer_slave_processing_enabled = FALSE;
  #endif
} /* timer_init */


/*=============================================================================

FUNCTION TIMER_DRV_GROUP_DISABLE

DESCRIPTION
  Disables a timer group.  Timers in the timer group will continue to count
  down, but they will not expire until the group is enabled.

DEPENDENCIES
  Timer group must not be left disabled for more than 2^32 slow clock counts.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_drv_group_disable
(
  /* Timer group to be disabled */
  timer_group_ptr                 group
)
{
  timer_group_t_ptr               timer_group;
  uint32                          group_idx;
  
  /* Pointer to timers, for walking through lists of timers */
  timer_ptr_internal_type         ptr;

  time_timetick_type              now=0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(group == NULL || *group == GROUP_INVALID_HANDLE)
    return TE_INVALID_GROUP_HANDLE;

  ATS_INTLOCK();

  now = timetick_get_tmp64();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_GROUP_DISABLE,NULL,NULL,now,TIMER_DUMMY_PROCESS_INDEX);

  /* Disable the timer group, if it is not already disabled */
  
  if(!GROUP_HANDLE_TO_INDEX(*group, &group_idx))
  {
     TIMER_API_LOG_EXIT(TE_INVALID_GROUP_HANDLE);
     ATS_INTFREE();
     return TE_INVALID_GROUP_HANDLE;
  }
  
  timer_group = timer_internal_groups[group_idx];

  if ( !timer_group->disabled )
  {
    /* Mark timer-group as disabled, and record zero-basis */
    timer_group->disabled  = TRUE;

    /* Move given group's timers from active timer list to group timer list */
    ptr = timers.active.list.first;

    while (ptr != NULL)
    {
      /* Remember the next element on the active list, we are going
         to nuke it */
      timer_ptr_internal_type next = ptr->next;

      if ( ptr->info.group_idx == group_idx )
      {
        /* This is a timer in the given timer group */

        #ifdef FEATURE_USE_TIMER_BIN
        /* if it's binned, remove it from its bin */
        if( ptr->bin != NULL ) 
        {
            timer_bin_remove( ptr );
        }
        #endif
       
        /* Unlink timer from active list */
        timer_unlink( ptr );
               
        /* Once the timer has been removed from the active
         timer list, we need to decrement the num of
         timers in the active list*/
        /* Decrement the number of timers  */
        timers.active_timers_count--; 
       
        /* Append to group timer list */
        timer_append( &timer_group->list, ptr );
      }

      /* Walk to next timer node */
      ptr = next;
    }

    DalTimetick_GetTimetick64(hTimerHandle, &now);

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_GROUP_DISABLED, now);
  }

  /* Update the first non-deferrable timer in the active list */
  timer_update_first_active_non_deferrable_timer();

  TIMER_API_LOG_EXIT(TE_SUCCESS);

  ATS_INTFREE();
  return TE_SUCCESS;
} /* timer_drv_group_disable */



/*=============================================================================

FUNCTION TIMER_DRV_GROUP_ENABLE

DESCRIPTION
  Enables a timer group.  Timers in the timer group which expired during the
  diabled period will have their expiry processing executed.

DEPENDENCIES
  Timer group must not be left disabled for more than 2^32 slow clock counts.

RETURN VALUE
  None

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_drv_group_enable
(
  /* Timer group to be enabled */
  timer_group_ptr                 group
)
{
  uint32                          group_idx;
  timer_group_t_ptr               timer_group;
  
  /* Pointer to a timer in the list of timers */
  timer_ptr_internal_type         timer;


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(group == NULL || *group == GROUP_INVALID_HANDLE)
    return TE_INVALID_GROUP_HANDLE;

  ATS_INTLOCK();

  /* Get current clock tick */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_GROUP_ENABLE,NULL,NULL,ticks_now,TIMER_DUMMY_PROCESS_INDEX);
  
  if(!GROUP_HANDLE_TO_INDEX(*group, &group_idx))
  {
     TIMER_API_LOG_EXIT(TE_INVALID_GROUP_HANDLE);
     ATS_INTFREE();
     return TE_INVALID_GROUP_HANDLE;
  }
  
  timer_group = timer_internal_groups[group_idx];

  /* Enable the timer group, if it is disabled */

  if ( timer_group->disabled )
  {
    /* Walk through timers in timer group list.
       Stop when we get to a timer after the head of the active list */

    timer = timer_group->list.first;
    
    while( timer != NULL )
    { 
      if( timer->expiry  >= ticks_now )
      {
        break; 
      }
      /* Increment the number of expired timers  */
      timers.active_timers_count++; 
      timer = timer->next;
    }

    /* Break timer list into two parts at this point */
    if( timer != NULL )
    {
      timer_group->list.last = timer->prev;

      if( timer->prev == NULL )
      {
        timer_group->list.first = NULL;
      }
      else
      {
        timer->prev->next = NULL;
      }

      timer->prev = NULL;
    }
        
    /* Expired timers are in list pointed to by "timer_group->list" */
    /* Non-expired timers are in linked list pointed to by "timer" */

    /* Updates timer_num in timer_insert for non-expired timers */
    /* Move non-expired timers into active timer list */
    timer_insert_list( &timers.active.list, timer );

    /* If there are any expired timers ... */
    if ( timer_group->list.first != NULL ) {

      /* Move them to the front of the special expired list */
      timer_concatenate_lists( &timer_group->list, &timers.active.list ); 
   }

    /* Mark timer-group as enabled */
    timer_group->disabled = FALSE;

    DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_GROUP_ENABLED, ticks_now);
  }
  
  /* Update the first non-deferrable timer in the active list */
  timer_update_first_active_non_deferrable_timer();

  TIMER_API_LOG_EXIT(TE_SUCCESS);  
  ATS_INTFREE();
  return TE_SUCCESS;
} /* timer_drv_group_enable */

/*=============================================================================

FUNCTION TIMER_CREATE_NEW_GROUP

DESCRIPTION
  Creates new internal group and attaches handle to client group variable

DEPENDENCIES
  Should be called from an INTLOCK'd context

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/
static timer_error_type timer_create_new_group(timer_group_ptr group, 
                                               timer_group_ptr group_actual_addr, /* Only for debugging log purposes */
                                               uint32 *pgroup_idx, 
                                               uint32 process_idx)
{
  uint32 group_idx;
  uint32 local_free_group_idx;

  if ( timer_internal_groups_allocated >= TIMER_MAX_GROUPS ||
  	   timer_next_free_group_idx >= TIMER_MAX_GROUPS )
   {
      ERR_FATAL("Maximum possible groups %d are over", TIMER_MAX_GROUPS, 0, 0);
      return TE_FAIL;
   }  

  group_idx = timer_next_free_group_idx;
  if(timer_internal_groups[group_idx] != NULL)
  {
     ERR_FATAL("Incorrect free group index : %d", group_idx, 0, 0);
     return TE_NO_FREE_GROUP;
  }
  
  /* Allocate memory and initialize to zero */
  timer_internal_groups[group_idx] = (timer_group_t *) qurt_calloc(sizeof(timer_group_t), 1);
  if(timer_internal_groups[group_idx] == NULL)
  {
     MSG_1(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Couldnt alloc memory of %d bytes", sizeof(timer_group_t));
     return TE_MALLOC_FAILED;
  }

  timer_internal_groups_allocated++;
  timer_internal_groups[group_idx]->ext_self = group_actual_addr;
  timer_internal_groups[group_idx]->process_idx = process_idx;
  timer_internal_groups[group_idx]->group_idx = group_idx;
  *group = GROUP_INDEX_TO_HANDLE(group_idx);
  *pgroup_idx = group_idx;

  /* Update the next free group index */
  local_free_group_idx = (timer_next_free_group_idx + 1) % TIMER_MAX_GROUPS;
  while ( local_free_group_idx != timer_next_free_group_idx )
  	{
  	  if (timer_internal_groups[local_free_group_idx] == NULL )
  	  	{
  	  	 break;
  	  	}

	  local_free_group_idx = (local_free_group_idx + 1)%TIMER_MAX_GROUPS;
  	}

  timer_next_free_group_idx = local_free_group_idx;

  /*Placing newly created group onto the list*/
  timer_internal_groups[group_idx]->next = timers.active.next;
  timers.active.next = timer_internal_groups[group_idx];
  
  return TE_SUCCESS;
} /* timer_create_new_group */


/*=============================================================================

FUNCTION TIMER_DELETE_GROUP

DESCRIPTION
  Deletes an internal group and frees the memory associated with it.
  
DEPENDENCIES
  Should be called from an INTLOCK'd context

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/
static void timer_delete_group(uint8 group_idx )
{
  /* Free the memory for the group */
  qurt_free(timer_internal_groups[group_idx]);

  /* Mark the group pointer as NULL */
  timer_internal_groups[group_idx] = NULL;

  /* Update the next free group index & total allocated groups*/
  timer_next_free_group_idx = group_idx;
  timer_internal_groups_allocated--;

} /* timer_delete_group */

/*=============================================================================

FUNCTION TIMER_GROUP_SET_DEFERRABLE

DESCRIPTION
  Set timer group deferrable status. Timer groups default to being non-deferrable.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

=============================================================================*/

timer_error_type timer_drv_group_set_deferrable( timer_group_ptr group, 
                                                 timer_group_ptr group_actual_addr, /* For debug purposes only */
                                                 boolean condition,
                                                 uint32 process_idx)
{
  /* Set the property of this timer group to indicate whether or not it's 
  deferrable, based on the argument passed in*/
  timer_group_t_ptr group_ptr;
  uint32 group_idx;
  timer_error_type status = TE_SUCCESS;
  
  if( group == NULL )
    return TE_INVALID_PARAMETERS;
    
    
  ATS_INTLOCK();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_GROUP_SET_DEFERRABLE,NULL,NULL,timetick_get_tmp64(),process_idx);
  
  
  if(!GROUP_HANDLE_TO_INDEX(*group, &group_idx))
  {   /* Group is not present, Create it first */
     if((status = timer_create_new_group(group, group_actual_addr, &group_idx, process_idx)) != TE_SUCCESS)
     {
        TIMER_API_LOG_EXIT(status);
        ATS_INTFREE();
        return status;
     }
  }
  
  group_ptr = timer_internal_groups[group_idx];
  group_ptr->deferrable = condition;

  /* Update the first non-deferrable timer in the active list */
  timer_update_first_active_non_deferrable_timer();  

  TIMER_API_LOG_EXIT(status);
  ATS_INTFREE();
  return TE_SUCCESS;
} /* timer_drv_group_set_deferrable */


/*=============================================================================

FUNCTION TIMER_DRV_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY

DESCRIPTION
  Gets the first non-deferrable timer's expiry value.

DEPENDENCIES
  None.

RETURN VALUE
  Absolute expiry of first occuring non-deferrable timer.
  Returns TIMER_MAX_EXPIRY in case there is no non-deferrable timer present.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_drv_get_first_non_deferrable_timer_expiry( 
                                      timer_non_defer_data_ptr timer_data_ptr )
{
  /* Expiry for the 1st non-deferrable timer */
  time_timetick_type              future_expiry = TIMER_MAX_EXPIRY;
  timer_ptr_internal_type         ptimer;  

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  /* Get the current time */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY,NULL,NULL,ticks_now,TIMER_DUMMY_PROCESS_INDEX);

  /* Spin thru the active timer list */
  for( ptimer = timers.active.list.first; ptimer != NULL; ptimer = ptimer->next )
  {
    /* if timer belongs to a group, which is non-deferrable */
    if (timer_internal_groups[ptimer->info.group_idx]->deferrable == FALSE )
    {
      /* Take note of the 'future' timer's expiry and bail */
      future_expiry = ptimer->expiry;
      break;
    }
  }

  if(timer_data_ptr != NULL)
  {
    if(future_expiry == TIMER_MAX_EXPIRY)
     {
       timer_data_ptr->cb_type = TIMER_INVALID_NOTIFY_TYPE;
       timer_data_ptr->sigs_func_addr = NULL;
       timer_data_ptr->sigs_mask_data = 0;
     }
    else
     {
       timer_data_ptr->cb_type = ptimer->timer_ext->info.callback_type;
       timer_data_ptr->sigs_func_addr = ptimer->timer_ext->cbfn.func1;
       timer_data_ptr->sigs_mask_data = ptimer->timer_ext->cbdt.data; 
     }
  }
  
  TIMER_API_LOG_EXIT(TE_SUCCESS);
  
  ATS_INTFREE();
  return future_expiry;

} /* timer_drv_get_first_non_deferrable_timer_expiry */


/*=============================================================================

FUNCTION TIMER_DRV_DEFER_MATCH_INTERRUPT

DESCRIPTION
  Sets timer match interrupt to the next occurring non-deferrable timer expiry 
  value. Complimentary function is timer_undefer_match_interrupt(). 

DEPENDENCIES
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
time_timetick_type -- Number of ticks in 19MHz domain 
until the next timer expiry in a timer group that is 
non-deferrable. 

SIDE EFFECTS
  Adjusts match value and prevents any subsequent timer calls from updating the
  match value, until timer_undefer_match_interrupt() is called.

=============================================================================*/

time_timetick_type timer_drv_defer_match_interrupt_64( void )
{
  time_timetick_type                   future_expiry;
  timer_ptr_internal_type              ptimer;
  time_timetick_type                   tt_ret;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  /* Get the current time */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].ts = ticks_now;
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].function_called= TIMER_MVS_DEFER_MATCH_INT;
#endif	

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_DEFER_MATCH_INTERRUPT,NULL,NULL,ticks_now,TIMER_DUMMY_PROCESS_INDEX);

  /* Assume the worst - furthest point in future ~119 years */
  //future_expiry = -1;
  // in Badger, the max duration is controlled by macro QURT_SYSCLOCK_MAX_DURATION_TICKS
  future_expiry = QURT_SYSCLOCK_MAX_DURATION_TICKS;
  /* Spin thru the active timer list */
  for( ptimer = timers.active.list.first; ptimer != NULL; ptimer = ptimer->next )
  {
    /* if timer belongs to a group, which is non-deferrable */
    if (timer_internal_groups[ptimer->info.group_idx]->deferrable == FALSE )
    {
      /* Take note of the 'future' timer's expiry and bail */
      future_expiry = ptimer->expiry;
      break;
    }
  }

  /* If future_expiry is in past */
  if (future_expiry  < ticks_now )
  {
    TIMER_API_LOG_EXIT(TE_FAIL);

    /* Log the return value */
#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].ts_aux = 0;	
	TIMER_INC_MOD(timer_defer_undefer_reference_count_log_cnt,MAX_TIMER_REFERNCE_COUNT_LOG);
#endif	
	
    ATS_INTFREE();
    return 0;
  }
  
  /* Set the timer interrupt to occur at the proper point in the future*/
  timer_set_next_interrupt( future_expiry, ticks_now, FALSE,TIMER_MVS_DEFER_MATCH_INT );

  /* From this point on timer interrupt  won’t be adjusted */
  timers.do_reprogram_isr = FALSE;

  /* Compute the difference between now and the above future expiry */
  tt_ret = (future_expiry - ticks_now);

  /* Log the return value */
#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].ts_aux = tt_ret; 
  TIMER_INC_MOD(timer_defer_undefer_reference_count_log_cnt,MAX_TIMER_REFERNCE_COUNT_LOG);
#endif	

  TIMER_API_LOG_EXIT(TE_SUCCESS);

  ATS_INTFREE();

  /* Return the difference between now and the above future expiry */
  return tt_ret;
} /* timer_drv_defer_match_interrupt_64 */


/*=============================================================================

FUNCTION TIMER_DRV_UNDEFER_MATCH_INTERRUPT

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
void timer_drv_undefer_match_interrupt( void )
{
  /* Current slow clock count */
  time_timetick_type                   now;

  time_timetick_type                   tmp_ticks;

  /* Time of first expiring timer */
  time_timetick_type                   first;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Log timetick before entering the critical section */
  DalTimetick_GetTimetick64(hTimerHandle, &tmp_ticks);

  ATS_INTLOCK();

  DalTimetick_GetTimetick64(hTimerHandle, &now);

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_UNDEFER_MATCH_INTERRUPT,NULL,NULL,now,TIMER_DUMMY_PROCESS_INDEX);

#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].ts = now;
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].ts_aux = tmp_ticks;
  timer_defer_undefer_reference_count_log[timer_defer_undefer_reference_count_log_cnt].function_called = TIMER_MVS_UNDEFER_MATCH_INT;
  TIMER_INC_MOD(timer_defer_undefer_reference_count_log_cnt,MAX_TIMER_REFERNCE_COUNT_LOG);
#endif

    
  /* Are there timers on the timer list? */
  if ( timers.active.list.first != NULL )
  {
    /* Get the time of the first expiring timer */
    first = timers.active.list.first->expiry;
  }
  else
  {
    /* If no timers, set "first" to as far in the future as possible */
    //first = timers.active.list.zero - 1;
    // in Badger, the max duration is controlled by macro QURT_SYSCLOCK_MAX_DURATION_TICKS
    first = QURT_SYSCLOCK_MAX_DURATION_TICKS;
  }

  /* If the first expiring timer is in the past ... */
  if ( first  < now  )
  {
    /* Set the timer for "as soon as possible" (eg, "now") */
    first = now;
  }
  
   
  /* Set the next interrupt match value. The function is called after power 
     collapse and the hardware might not contain right values . Hence force 
     MATCH_VAL register to be updated. */
  timer_set_next_interrupt( first, now, TRUE,TIMER_MVS_UNDEFER_MATCH_INT );

  /* Allow timer isr programming again */
  timers.do_reprogram_isr = TRUE;

  TIMER_API_LOG_EXIT(TE_SUCCESS);

  ATS_INTFREE();

  } /* timer_drv_undefer_match_interrupt */


/*=============================================================================

FUNCTION TIMER_RESTART

DESCRIPTION
 This function is called after waking up. The timetick counter values 
 are readjusted to sync up with the base timetick counter when waking up from
 power collapse. The match value has to be adjusted accordingly.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  Adjusts the match value


=============================================================================*/

void timer_drv_restart(void)
{
  //TIMER_API_LOG(TIMER_API_LOG_TIMER_RESTART, NULL, timetick_get_tmp64());
  /* Calling timer_undefer_match_interrupt */
  timer_drv_undefer_match_interrupt();
}

/*=============================================================================
FUNCTION TIMER_INIT_BUFFER_QUEUE

DESCRIPTION
  Initialize buffer queue where free timer buffers are kept. 

DEPENDENCIES
  Must be called from timer_drv_def or timer_def2 before the first timer is 
  defined. It is not called from timer_init as some timers are defined 
  before call to timer_init.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static void timer_init_buffer_queue(void)
{
  /* local Iterator */
  uint32 i;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*Get memory from heap*/
  timer_buffer_ptr = (timer_ptr_internal_type)
                     qurt_malloc( sizeof(timer_internal_type) * MAX_TIMER_BUFFER_NUMBER );

  if(NULL == timer_buffer_ptr) 
  {
    ERR_FATAL("qurt_malloc returned NULL",0,0,0);
    return;
  }
  
  /*memset the allocated memory so that we do not have to memset it at timer_drv_set*/
  memset( timer_buffer_ptr, 0, sizeof(timer_internal_type) * MAX_TIMER_BUFFER_NUMBER );
  
  /* Initialize the timer_free_q */
  q_init( &timer_free_q );
  
  for ( i = 0 ; i < MAX_TIMER_BUFFER_NUMBER ; i++ )
  {
    timer_buffer_ptr[i].info.node_state = NODE_IS_FREE;
    timer_buffer_ptr[i].index            = i;
  
    /* Link the handles to the timer_free_q */
    q_link( &timer_buffer_ptr[i], &timer_buffer_ptr[i].link );
    
    /* Enqueue handles */
    q_put( &timer_free_q, &timer_buffer_ptr[i].link );

  }
} /* timer_init_buffer_queue */

/*=============================================================================
FUNCTION TIMER_CREATE_NEW_CHUNK

DESCRIPTION
  Creates new memory chunk for more timers

DEPENDENCIES
  Should be called from Intlock'd context

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
timer_error_type timer_create_new_chunk(uint32 max_timers)
{
   uint32 i;
   uint32 num_timers = max_timers;
   timer_client_ptr_type ptr = NULL;

   ATS_INTLOCK();

	/*
	* This is to check if some other task already allocated a new chunk,
	* while timer task didn't got a chance to handle the signal & call this
	* function from timer tasks context.
	*/
	if( timer_free_client_timers_cnt > TIMERS_MIN_CLIENT_TIMERS_FOR_NEW_CHUNK ) {
		/* There is no need to create a new chunk. */
		ATS_INTFREE();
		return TE_SUCCESS;
	}   
   
   if(timer_client_chunks_allocated == TIMER_CLIENT_CHUNKS_MAX)
   {
      ERR_FATAL("Maximum possible chunks %d are over", TIMER_CLIENT_CHUNKS_MAX, 0, 0);
	  ATS_INTFREE();
      return TE_FAIL;
   }
   
   do{
      /* Get memory from heap */
      ptr = (timer_client_ptr_type)
                        qurt_malloc( sizeof(timer_client_type) * num_timers );

      if(ptr == NULL)
      {
         num_timers = num_timers/2;
      }
      else
      {
         break;
      }
   }while(num_timers != 0);
   
   if(ptr == NULL)
   {
      MSG_2(MSG_SSID_ATS, MSG_LEGACY_HIGH,"Couldnt malloc %d bytes for %d client timers", sizeof(timer_client_type) * num_timers, num_timers);
      //ERR_FATAL("Couldnt malloc %d bytes for client timers", sizeof(timer_client_type) * num_timers, 0, 0);
      ATS_INTFREE();
      return TE_NO_FREE_CLIENT_TIMER;
   }

   /* Memset the timer client chunks memory */
   memset( ptr, 0x0, sizeof(timer_client_type) * num_timers );
   
   timer_client_chunks[timer_client_chunks_allocated].buffer     = ptr;
   timer_client_chunks[timer_client_chunks_allocated].buffer_idx = 0;
   timer_client_chunks[timer_client_chunks_allocated].num_timers = num_timers;
   timer_client_chunks[timer_client_chunks_allocated].allocated_cnt = 0;

   /* Update the count for total number of free client timers */
   timer_free_client_timers_cnt += num_timers;
     
   ptr = &timer_client_chunks[timer_client_chunks_allocated].buffer[0];
   for(i = 0; i < num_timers; ptr++, i++)
   {
      ptr->timer_state      = TIMER_DEFAULT_FLAG;
      /* For now checking only timer_state for knowing if a node free or not
         and to paint chunk memory with speed */
      ptr->info.node_state  = NODE_IS_FREE;   
   }
   
   /* Increment allocated chunks count */
   timer_client_chunks_allocated++;
   ATS_INTFREE();
   return TE_SUCCESS;
} /* timer_create_new_chunk */

/*=============================================================================
FUNCTION TIMER_INIT_CLIENT_TIMERS

DESCRIPTION
  Initialize client timer buffer memory and set them to default flag. 

DEPENDENCIES
  Must be called from timer_define before the first timer is defined.
  It is not called from timer_init as some timers are defined before
  call to timer_init.

RETURN VALUE
  None

SIDE EFFECTS
  None
=============================================================================*/
void timer_init_client_timers(void)
{
	/* Local variable for iterating loop */
	uint32 i = 0, j = 0;

	/* Temp client timer pointer */
	timer_client_ptr_type ptr = NULL;

	/* Split the statically allocated client timer buffer into equally divided
	 * chunks each of size MAX_TIMER_BUFFER_NUMBER.
	 */
	for (i = 0;
		 i < TIMER_CLIENT_BUFFER_INIT_ARRAY_SIZE && timer_client_chunks_allocated < TIMER_CLIENT_CHUNKS_MAX;
		 i++ ) {
		/* Update the timer client chunk pointer to point to the static buffer of client timers */
		timer_client_chunks[timer_client_chunks_allocated].buffer = &timer_client_buffer[i][0];
	
		/* Buffer index pointing to the 1st free client timer from this chunk */
		timer_client_chunks[timer_client_chunks_allocated].buffer_idx = 0;
	
		/* Total remaining statically allocated client timers in this chunk */
		timer_client_chunks[timer_client_chunks_allocated].num_timers = MAX_TIMER_BUFFER_NUMBER;
	
		/* There are no allocated timers in this chunk, update it as 0. */
		timer_client_chunks[timer_client_chunks_allocated].allocated_cnt = 0;
	
		/* Update the count for total number of free client timers */
		timer_free_client_timers_cnt += timer_client_chunks[timer_client_chunks_allocated].num_timers;

		/* Initialize the static client timer buffers with default values */	  
		ptr = &timer_client_chunks[timer_client_chunks_allocated].buffer[0];

		for(j = 0; j < MAX_TIMER_BUFFER_NUMBER; ptr++, j++) {

			/* Update the client timers state to TIMER_DEFAULT_FLAG */
			ptr->timer_state 	 = TIMER_DEFAULT_FLAG;
		
			/* For now checking only timer_state for knowing if a node
			 * is free or not and to paint chunk memory with speed.
			 */
			ptr->info.node_state  = NODE_IS_FREE;	 
		}

		/* Increment the allocated chunks index */
		timer_client_chunks_allocated++;
		
	}

	/* Assuming that this init function is called before any
	 * other chunks are inited, assign the current chunk to
	 * be used as the 0th chunk allocated statically. 
	 */
	timer_client_curr_chunk_idx = 0;		 
	
	/* Assign the values to the global variables for timer client chunks */
	timer_client_chunk_total_num_chunks = TIMER_CLIENT_CHUNKS_MAX;

	return;
} /* timer_init_client_timers */


/*=============================================================================
FUNCTION TIMER_GET_NEW_CLIENT_TIMER

DESCRIPTION
  Get New client timer from timer_client_chunks. 

DEPENDENCIES
  Need to be called from internal function

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
timer_client_ptr_type timer_get_new_client_timer(timer_error_type *status, uint32 *handle)
{
   timer_client_ptr_type clnt_timer = NULL;
   uint32 i;
   uint32 chunk_idx = 0, timer_idx = 0;
   uint32 num_timers, found_timer = 0;

   /* Before procedding, check if there are any free client timers available or not */
   if (timer_client_chunks_allocated == TIMER_CLIENT_CHUNKS_MAX && timer_free_client_timers_cnt == 0)
   	{
   	  ERR_FATAL("Maximum possible chunks %d are over", TIMER_CLIENT_CHUNKS_MAX, 0, 0);
      return NULL;
   	}
   
   /* Input validation */   
   if(status == NULL || handle == NULL)
   {
      MSG(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Got Null inputs");
      return NULL;
   }

   /* Check if we need to create a new chunk before procedding */
   if (timer_free_client_timers_cnt == 0)
   	{
   	  /* Create a new chunk since we dont have any free client timers */
      *status = timer_create_new_chunk(MAX_TIMER_BUFFER_NUMBER);
      if(*status != TE_SUCCESS)
      {
         MSG(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Couldnt create new memory chunk");
         return NULL;
      }
   	}
   
   *status = TE_FAIL;
   *handle = 0;
 
   chunk_idx = timer_client_curr_chunk_idx;
   found_timer = 0;
   
	do {
		if(timer_client_chunks[chunk_idx].allocated_cnt == timer_client_chunks[chunk_idx].num_timers) {
			/* Number of allocated timers in this chunk is equal to the total timers
			 * i.e. there are no free timers in this current chunk, no Need to traverse it.
			 */      
			chunk_idx = (chunk_idx + 1) % timer_client_chunks_allocated;
			continue;
		}
		else {
			/* Traverse the chunk starting from buffer_idx to find a free client timer */
		  
			/* Initialize few variables before starting to traverse the chunk */
			timer_idx = timer_client_chunks[chunk_idx].buffer_idx;
			num_timers = timer_client_chunks[chunk_idx].num_timers;
			i = timer_idx;
		  
			do {
				if(timer_client_chunks[chunk_idx].buffer[i].timer_state == TIMER_DEFAULT_FLAG) {
					/* Found a free timer from the chunk, update variables & break out of this inner loop */
					timer_client_curr_chunk_idx = chunk_idx;
					timer_client_chunks[chunk_idx].buffer_idx = timer_idx = i;
					found_timer = 1;
					break;
				}
		  
				/* Increment the index */
				i++;

				/* Correction for overflow */
				if(i == num_timers) {
					i = 0;
				}
		  
			}while(i != timer_idx);
		  
			/* If timer was found, break out of this loop as well */
			if (found_timer == 1) {
				break;
			}
			else {
				/* Getting here would mean that we didn't find any free 
				 * client timers in the current chunk, though we started
				 * the search only after checking for allocated_cnt, and
				 * we are supposed to successfully find a free client timer.
				 */
				ERR_FATAL("Corruption in the chunk, allocated_cnt = %d, num_timers=%d, timer_index=%d",
					timer_client_chunks[chunk_idx].allocated_cnt,num_timers,timer_idx);
			}
		}
		
		chunk_idx = (chunk_idx + 1) % timer_client_chunks_allocated;
	}while(chunk_idx != timer_client_curr_chunk_idx);
   
   /* Get Client Timer */
   clnt_timer = &timer_client_chunks[chunk_idx].buffer[timer_idx];
   timer_client_chunks[chunk_idx].buffer_idx = (timer_client_chunks[chunk_idx].buffer_idx + 1) % timer_client_chunks[chunk_idx].num_timers;
   
   timer_client_chunks[chunk_idx].allocated_cnt++;

   /* Decrement the count of free client timers */
   timer_free_client_timers_cnt--;
   
   /* memset to zero before giving for new client */
   memset(clnt_timer, 0, sizeof(timer_client_type));
   clnt_timer->info.node_state = NODE_IS_NOT_FREE;
   clnt_timer->timer_state = TIMER_DEFAULT_FLAG;

   *handle = TIMER_INDEX_TO_HANDLE(chunk_idx, timer_idx);
   *status = TE_SUCCESS;

   /* Check to see if the number of free client timers in the 
       * current chunk is less than threshold, if so then allocate 
       * another chunk in advance.
       * Do this only if all the timer chunks are not used up, to
       * avoid hitting ERR_FATAL in cases when there are
       * TIMERS_MIN_CLIENT_TIMERS_FOR_NEW_CHUNK or less
       * free timers still available.       
       */
   if ((timer_client_chunks_allocated < TIMER_CLIENT_CHUNKS_MAX) &&
   	   (timer_free_client_timers_cnt <= TIMERS_MIN_CLIENT_TIMERS_FOR_NEW_CHUNK ))   	
   {
     /* Signal in order to prime timer task to allocate a new chunk */
     (void) qurt_anysignal_set(&timer_q6_signal, TIMER_ALLOCATE_CHUNK_SIG);
   }
   
   return clnt_timer;
} /* timer_get_new_client_timer */


/*=============================================================================

FUNCTION TIMER_GET_CLIENT_TIMER

DESCRIPTION
  This function returns pointer to client timer.

DEPENDENCIES
  Should be called from Intlock'd context.. 

RETURN VALUE
  Returns relevant pointer to client timer structure. 
  If handle is invalid returns NULL.

SIDE EFFECTS
  This function should be called inside an intlock or a mutex.

=============================================================================*/
timer_client_ptr_type timer_get_client_timer
(
   timer_ptr_type    timer, 
   uint32            process_idx,
   timer_error_type  *status
)
{
  //int32 temp_index;
  uint32 cid, tid;
  timer_client_ptr_type temp_clnt = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(timer == NULL || status == NULL)
  {
     if(status)
       *status = TE_INVALID_PARAMETERS;
     return NULL;
  }  
  
  *status = TE_INVALID_TIMER_HANDLE;
  
  if(!TIMER_HANDLE_TO_INDEX(*timer, &cid, &tid))
  {
     return NULL;
  }

  if(cid >= timer_client_chunks_allocated)
     return NULL;
     
  if(tid >= timer_client_chunks[cid].num_timers)
     return NULL;

  temp_clnt = &timer_client_chunks[cid].buffer[tid];
  
  if(temp_clnt->timer_state < TIMER_DEFAULT_FLAG || temp_clnt->timer_state > TIMER_UNDEF_FLAG)
  {
     ERR_FATAL("Timer got corrupted. timer_state 0x%x, cid: 0x%x, tid: 0x%x ", temp_clnt->timer_state, cid, tid);
  }
  
  /* Only if the client timer is in use, otherwise it may be stale Handle */
  if(temp_clnt->info.node_state == NODE_IS_NOT_FREE 
     && temp_clnt->timer_state != TIMER_DEFAULT_FLAG
     && temp_clnt->info.process_idx == process_idx)
  {
     *status = TE_SUCCESS;
  }
  else
  {
     temp_clnt = NULL;
  }
  
  return temp_clnt;
} /* timer_get_client_timer */


/*=============================================================================
FUNCTION TIMER_FREE_CLIENT_TIMER

DESCRIPTION
  Free up the client timer. 

DEPENDENCIES
  Should be called from Intlock'd Context
  
RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
timer_error_type timer_free_client_timer(timer_ptr_type timer)
{
   uint32 cid = 0;
   uint32 tid = 0;
   timer_client_ptr_type clnt_timer = NULL;
   timers_client_buffer_type *chunk_ptr = NULL;
   
   if(!TIMER_HANDLE_TO_INDEX(*timer, &cid, &tid))
   {
      return TE_INVALID_TIMER_HANDLE;
   }
   
   chunk_ptr = &timer_client_chunks[cid];
   clnt_timer = &chunk_ptr->buffer[tid];
  
   /* Intentionally not memsetting to zero as other details will be useful for debugging. */
   clnt_timer->timer_state            = TIMER_DEFAULT_FLAG;
   clnt_timer->info.node_state        = NODE_IS_FREE;
   
   /* If there are no previous free timers, keep buffer_idx as this tid */
   if(chunk_ptr->num_timers == chunk_ptr->allocated_cnt)
      chunk_ptr->buffer_idx = tid;

   /* Decrement the allocated client timer count for this chunk */
   chunk_ptr->allocated_cnt--;

   /* Increment the count of free client timers */
   timer_free_client_timers_cnt++;

   return TE_SUCCESS;
} /* timer_free_client_timer */


/*=============================================================================

FUNCTION TIMER_GET_CLNT_INT_TIMERS

DESCRIPTION
  Gets Client, Internal timers from Timer Handle. 
  If internal timer is not required, NULL can be passed as input

DEPENDENCIES
  Should be called from Intlock'd context.

RETURN VALUE
  None

SIDE EFFECTS


=============================================================================*/
timer_error_type timer_get_clnt_int_timers(timer_ptr_type timer,
                                           uint32 process_idx,
                                           timer_client_ptr_type *clnt_timer, 
                                           timer_ptr_internal_type *int_timer)
{
  timer_error_type status = TE_SUCCESS;
  
  if(timer == NULL || clnt_timer == NULL)
    return TE_INVALID_PARAMETERS;
  
  *clnt_timer = NULL;
  if(int_timer)
     *int_timer = NULL;
  
  if(*timer == TIMER_INVALID_HANDLE)
    return TE_INVALID_TIMER_HANDLE;
  
  *clnt_timer = timer_get_client_timer(timer, process_idx, &status);
  if(status == TE_SUCCESS && int_timer != NULL)
  {
     *int_timer = timer_get_internal_timer ( *clnt_timer );
     if ( *int_timer == NULL )
        return TE_TIMER_NOT_ACTIVE;
  }
  return status;
} /* timer_get_clnt_int_timers */


/*=============================================================================

FUNCTION TIMER_DEFINE

DESCRIPTION
  Defines a timer structure, initializing members to NULL, and associates the
  timer with the given group.

DEPENDENCIES
  Timer must not already be defined and on a timer list (active).

RETURN VALUE
  None

SIDE EFFECTS
  May link timer group into linked list of timer groups

=============================================================================*/

static timer_client_ptr_type timer_define
(
  /* Timer to set */
  timer_ptr_type                  timer,
  
  /* Timer's Actuall Address in its own Process Domain, for debug purposes only */
  timer_ptr_type                  timer_actual_addr,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group,
  
 /* Group Actual Addr. For debug purposes only */
  timer_group_ptr                 group_actual_addr,
  
  /* Process Index */
  uint32                          process_idx,
  
  /* Returns status of this function */
  timer_error_type                *status
)
{
  timer_client_ptr_type clnt_timer = NULL;
  timer_group_t* group_ptr;
  
  uint32 group_idx = 0;
  uint32 handle;
  
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if(timer == NULL || status == NULL)
     return NULL;
  
	*status = TE_FAIL;

  /* Protect against simultaneous updates of linked lists */
  ATS_INTLOCK();
  
  if(*timer != TIMER_INVALID_HANDLE && NULL != (clnt_timer = timer_get_client_timer(timer, process_idx, status)))
  {
     if(clnt_timer->timer_client_ptr == timer_actual_addr)
     {
        ATS_INTFREE();
#ifdef FEATURE_TIMER_ERR_FATAL_FOR_DEFINED_TIMERS
        ERR_FATAL("Handle already in use: timer: 0x%x, Handle: 0x%x", timer, *timer, 0);
#else
        MSG_2(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Handle already in use: timer: 0x%x, Handle: 0x%x", timer, *timer);
#endif
        *status = TE_HANDLE_IN_USE;
        return NULL;
     }
  }
  
  /* Initialize the timer buffer if it is not initialized */
  if(!timer_q_init_status)
  {
    timer_init_client_timers();
    timer_init_buffer_queue();
    timer_q_init_status = 1;
  }
  
  if(!GROUP_HANDLE_TO_INDEX(*group, &group_idx))
  {  /* Group is not created previously, create it first */
      if((*status = timer_create_new_group(group, group_actual_addr, &group_idx, process_idx)) != TE_SUCCESS)
      {
         ATS_INTFREE();
         return NULL;
      }
  }
  
  group_ptr = timer_internal_groups[group_idx];
  
  /* If this timer group is not yet in the linked list of timer groups,
     add it to the list. */
  if( group_ptr->next == NULL )
  {
    /* As now groups are sequential array, we can remove pointers to next group.
       For now keeping it, later need to remove from other places too */
    group_ptr->next = timers.active.next;
    timers.active.next = group_ptr;
  }

   /* Get the new timer from client timers buffer */
   /* This function should memset to zero and return */
   clnt_timer = timer_get_new_client_timer(status, &handle);
   if(clnt_timer == NULL)
   {
      ATS_INTFREE();
      return NULL;
   }

  /* Initialize timer structure */
  clnt_timer->timer_client_ptr  = timer_actual_addr;
  clnt_timer->timer_int_handle  = TIMER_BUFFER_INVALID_HANDLE;
  clnt_timer->info.group_idx    = group_idx;
  clnt_timer->timer_state       = TIMER_DEF_FLAG;
  //clnt_timer->timer_state_at    = ticks_now;
  clnt_timer->timer_state_at = 0;  /* Get timetick call takes 10usecs (950 pcycles) around. Therefore for now setting zero */

  /* Fill Process details in the timer */
  clnt_timer->info.process_idx    = process_idx;
  if(process_idx != timer_curr_process_idx)
    clnt_timer->info.remote_process = TIMER_FROM_REMOTE_PROCESS;
  
  /* For now not using timer_state in info structure. See if it will be useful */
  //clnt_timer->info.timer_state = (TIMER_DEF_FLAG - TIMER_DEFAULT_FLAG) & 0xF;
  #ifdef FEATURE_TIMER_SLAVE1
     /*assign the slave task here*/
     clnt_timer->info.slave_task_idx = timer_assign_slave_task_to_timer();
  #endif
  
  *timer = handle;
  *status = TE_SUCCESS;
  
  /* End of critical section */
  ATS_INTFREE();  
  return clnt_timer;
} /* timer_define */

boolean timer_is_notify_type_valid(timer_notify_type notify_type)
{
  if(notify_type != TIMER_NATIVE_OS_SIGNAL_TYPE &&
     notify_type != TIMER_NATIVE_OS_SIGNAL2_TYPE &&
   	 notify_type != TIMER_REX_TCB_SIGNAL_TYPE &&
   	 notify_type != TIMER_DALSYS_EVENT_SIGNAL_TYPE &&
  	 notify_type != TIMER_FUNC1_CB_TYPE &&
  	 notify_type != TIMER_FUNC2_CB_TYPE &&
  	 notify_type != TIMER_NO_NOTIFY_TYPE)
     return FALSE;
  else
     return TRUE;
}

/*=============================================================================

FUNCTION timer_drv_def

DESCRIPTION
  Defines and initializes a timer. Signal and function notifications are 
  exclusive. If both are provided at the same time, error will be returned.

  The timer expires when:\n
  - sigs is non-NULL. The signals are set to the task given by tcb.
  - func is non-NULL. The callback function with the argument data
    is called from the timer tasks.

DEPENDENCIES
  None

RETURN VALUE
  timer_error_type: Possible errors are
  TE_SUCCESS
  TE_FAIL
  TE_NO_FREE_GROUP
  TE_NO_FREE_CLIENT_TIMER
  TE_HANDLE_IN_USE
  TE_INVALID_PARAMETERS
  TE_MALLOC_FAILED
  

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_drv_def_osal
(
  /* Timer to set */
  timer_ptr_type                  timer,
  
  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,

  /* Group timer will belong to (NULL = the "NULL" timer group) */
  timer_group_ptr                 group,
  
  /* Group Actual Addr. For Debug purposes only */
  timer_group_ptr                 group_actual_addr,

  /*ats timer callback type*/
  timer_notify_type               cb_type,

  /* Task to signal and/or task to queue APC call for */
  time_osal_notify_obj_ptr        sigs_func_addr,

  /* Task signals to set to the tcb task when timer expires */
  time_osal_notify_data           sigs_mask_data,

  /* Process Index */
  unsigned int                    process_idx
)
{
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  timer_error_type status = TE_SUCCESS;
  timer_client_ptr_type clnt_timer = NULL;
  
  if(timer == NULL)
    return TE_INVALID_PARAMETERS;
  
  if(cb_type >= TIMER_INVALID_NOTIFY_TYPE)
    return TE_INVALID_PARAMETERS;
  
  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_DEF_OSAL,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);
  
  if ( sigs_mask_data != 0  &&  sigs_func_addr == NULL )
  {
    TIMER_API_LOG_EXIT(TE_INVALID_PARAMETERS);
    /* Todo: Can print msgs for error conditions from timer apis? */
    //ERR_FATAL("Can't set signals to NULL futex. T=%x S=%x", timer, sigs, 0);    
    return TE_INVALID_PARAMETERS;
  }
  
  ATS_INTLOCK();
  
  /* Allocate new client timer object and provide handle in timer variable */
  clnt_timer = timer_define( timer, timer_actual_addr, group, group_actual_addr, process_idx, &status );
  if(clnt_timer == NULL)
  {
     TIMER_API_LOG_EXIT(status);
     ATS_INTFREE();
     return status;
  }
  
  if((cb_type == TIMER_NATIVE_OS_SIGNAL_TYPE || cb_type == TIMER_NATIVE_OS_SIGNAL2_TYPE) && sigs_mask_data == 0)
    cb_type = TIMER_NO_NOTIFY_TYPE;
    
  if((cb_type == TIMER_FUNC1_CB_TYPE || cb_type == TIMER_FUNC2_CB_TYPE) && sigs_func_addr == 0)
    cb_type = TIMER_NO_NOTIFY_TYPE;
  
  /* Copy data to timer structure */
  if(cb_type != TIMER_NO_NOTIFY_TYPE)
  {
     clnt_timer->cbfn.sigs_obj      = sigs_func_addr;
     clnt_timer->cbdt.sigs          = sigs_mask_data;
  }
  clnt_timer->info.callback_type = cb_type;         

  TIMER_API_LOG_EXIT(status);
  ATS_INTFREE();
  return status;
} /* timer_drv_def_osal */


/*=============================================================================

FUNCTION TIMER_DEF2

DESCRIPTION
  Defines a 'clock callback' style timer.

  When the timer expires:
    if func(time_ms, data) is called from interrupt context,

DEPENDENCIES
  None

RETURN VALUE
  timer_error_type: Possible errors are
  TE_SUCCESS
  TE_FAIL
  TE_NO_FREE_GROUP
  TE_NO_FREE_CLIENT_TIMER
  TE_HANDLE_IN_USE
  TE_INVALID_PARAMETERS
  TE_MALLOC_FAILED

SIDE EFFECTS
  None

=============================================================================*/
timer_error_type timer_drv_def2
(
  /* Timer to set */
  timer_ptr_type                  timer,
  /* Timer's Actuall Address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,
  
  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group,

  /* Group actual addr. For logging purposes only */
  timer_group_ptr                 group_actual_addr,

  /* Process Index */
  uint32                          process_idx
)
{
   timer_error_type status = TE_SUCCESS;
   
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(timer == NULL)
    return TE_INVALID_PARAMETERS;
  
  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_DEF2,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);
  
  ATS_INTLOCK();

  /* Initialize new timer structure */
  (void) timer_define( timer, timer_actual_addr, group, group_actual_addr, process_idx, &status );

  TIMER_API_LOG_EXIT(status);
  ATS_INTFREE();
  return status;
} /* timer_drv_def2 */


/*=============================================================================

FUNCTION timer_drv_reg

DESCRIPTION
  Initializes a 'clock callback' style timer.

  When the timer expires:
    if func(time_ms, data) is called from interrupt context,

DEPENDENCIES
  Timer must have been defined using timer_def2() or TIMER_DEF2()

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_drv_reg
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,  

  /* Function to call at timer expiry */
  timer_t2_cb_type                func,

  /* Arbitrary data for func(time_ms, data) */
  timer_cb_data_type              data,

  /* Time (in ms) until first timer expiry */
  timetick_type                   time_ms,

  /* Period (in ms) between repeated expiries (0 = not periodic) */
  timetick_type                   reload_ms,
  /* process index */
  unsigned int                    process_idx
)
{
   timer_client_ptr_type clnt_timer = NULL;
   timer_error_type status;
   
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  
  ATS_INTLOCK();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_REG,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);
  
  status = timer_get_clnt_int_timers(timer, process_idx, &clnt_timer, NULL);
  if(clnt_timer == NULL)
  {
     TIMER_API_LOG_EXIT(status);
     ATS_INTFREE();
     return status;
  }

  /* Copy function & function data to timer structure */
  if(func != NULL)
  {
     clnt_timer->cbfn.func2          = func;
     clnt_timer->cbdt.data           = data;
     clnt_timer->info.callback_type  = TIMER_FUNC2_CB_TYPE;
  }
 
  /* Start the timer */
  status = timer_drv_set_64( timer, timer_actual_addr, time_ms, reload_ms, T_MSEC, process_idx );

  TIMER_API_LOG_EXIT(status);

  ATS_INTFREE();  

  return status;
} /* timer_drv_reg */


/*=============================================================================

FUNCTION TIMER_SET_GET_CMD_BUF

DESCRIPTION
  Gets a command buffer for a free timer and fills it out with timer info.

DEPENDENCIES
  Needs to be called from Intlock context

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

static inline timer_error_type timer_set_get_cmd_buf
(
  timer_ptr_type                   timer,
  unsigned int                     process_idx,
  timer_client_ptr_type           *clnt_timer_in,
  timer_ptr_internal_type         *free_timer_in
)
{
#ifdef FEATURE_TIMER_DETECT_MEM_LEAKS 
  uint32 iter = 0;
#endif /*FEATURE_TIMER_DETECT_MEM_LEAKS*/
  int32  temp_index;
  /* Internal timer ptr */
  timer_ptr_internal_type free_timer = NULL;
  /* Client timer ptr */
  timer_client_ptr_type   clnt_timer = NULL;
  /* error status */
  timer_error_type        status = TE_SUCCESS;
  
  /* ---------------------------------------------- */
  *clnt_timer_in = NULL;
  *free_timer_in = NULL;
     
  status = timer_get_clnt_int_timers(timer, process_idx, &clnt_timer, &free_timer);
  if(clnt_timer == NULL)
  {
     return status;
  }
  
  if ( free_timer != NULL ) 
  {
     /* Remove from active list */
     timer_remove(free_timer);   
	 
     if(clnt_timer->timer_state == TIMER_PAUSED_FLAG)
        timers.paused_timers_count--;

     clnt_timer->timer_int_handle = TIMER_BUFFER_INVALID_HANDLE;
     
     /* Clear the internal timer from previous values */
     temp_index                   = free_timer->index;
     memset( free_timer, 0, sizeof( timer_internal_type ));
     free_timer->index            = temp_index ;
  }
  else
  {
     free_timer = (timer_ptr_internal_type) q_get( &timer_free_q );
     if(free_timer == NULL)
     {
        //ASSERT ( free_timer != NULL );
        *free_timer_in = NULL;
        return TE_NO_FREE_INTERNAL_TIMER;
     }
     /* increase the count of timers in timer_buffer */
     timer_count_buffer_allocated = MAX_TIMER_BUFFER_NUMBER - q_cnt(&timer_free_q);
  }

  
  /* Todo: Do we require this still? Need to modify the code according to new changes */
#ifdef FEATURE_TIMER_DETECT_MEM_LEAKS 
  
  /* Iterate through timer_buffer to see if we have an active timer
     with an external memory and different handle. This is an error */
  for(iter = 0; iter < MAX_TIMER_BUFFER_NUMBER ; iter++ ) 
  {
    if( timer_buffer_ptr[iter].timer_ext == clnt_timer 
        && timer_buffer_ptr[iter].info.node_state == NODE_IS_NOT_FREE ) 
    {
       ERR_FATAL("Trying to set the same timer with different handle mem=0x%x, handle=0x%x",timer, iter+1,0);      
    }
  }
#endif /* FEATURE_TIMER_DETECT_MEM_LEAKS */

  temp_index                   = free_timer->index; 
  free_timer->info.node_state  = NODE_IS_NOT_FREE;  
  /* Most fields are 0's or NULL's */
  /* Copy data to Internal timer structure */
  free_timer->info.group_idx   = clnt_timer->info.group_idx;
  free_timer->info.process_idx = clnt_timer->info.process_idx;
  free_timer->info.remote_process = clnt_timer->info.remote_process;
  free_timer->timer_ext      = clnt_timer;
  free_timer->info.slave_task_idx = clnt_timer->info.slave_task_idx;
  free_timer->thread_id        = qurt_thread_get_id();
  
  clnt_timer->timer_int_handle = INDEX_TO_HANDLE( temp_index ); 
  clnt_timer->timer_state      = TIMER_SET_FLAG;
  clnt_timer->timer_state_at   = ticks_now;
  
  *clnt_timer_in = clnt_timer;
  *free_timer_in = free_timer;
  
  return TE_SUCCESS;
} /* timer_set_get_cmd_buf */


/*=============================================================================

FUNCTION TIMER_DRV_SET_64

DESCRIPTION
Sets an inactive timer to expire after a given period of time, or changes an
active timer to expire after a given period of time. Optionally, specifies
the timer to repeatly expire with a given period.  If the timer 
is to be set in units of ticks, it will be done in the 19MHz 
domain.

DEPENDENCIES
  None

RETURN VALUE
  timer_error_type. Possible return values are
  TE_INVALID_PARAMETERS
  TE_INVALID_TIMER_HANDLE
  TE_NO_FREE_INTERNAL_TIMER
  TE_SUCCESS

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_drv_set_64
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,    

  /* Time (in units below) until first timer expiry */
  time_timetick_type              time,

  /* Period (in units) between repeated expiries (0 = not periodic) */
  time_timetick_type              reload,

  /* Unit to measure "ticks" in. */
  timer_unit_type                     unit,
  /* process index */
  unsigned int                    process_idx
)
{
   timer_error_type status;
   /* Client timer ptr */
   timer_client_ptr_type         clnt_timer = NULL;
   /* Local internal timer ptr */
   timer_ptr_internal_type       free_timer = NULL;
   
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();
  
  ticks_now = timetick_get_tmp64();
  
  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_SET,timer,timer_actual_addr,ticks_now,process_idx);

  status = timer_set_get_cmd_buf(timer, process_idx, &clnt_timer, &free_timer);
  if(clnt_timer == NULL || free_timer == NULL) 
  {
     TIMER_API_LOG_EXIT(status);
     ATS_INTFREE();
     return status;
  }

  /* Defensive programming. Clearing the signal that will
   be set when the timer expires.*/
   if ( clnt_timer->info.callback_type == TIMER_NATIVE_OS_SIGNAL_TYPE )
   {
     qurt_anysignal_clear( (qurt_anysignal_t *) clnt_timer->cbfn.sigs_obj, clnt_timer->cbdt.sigs );
   }
   else if(clnt_timer->info.callback_type == TIMER_NATIVE_OS_SIGNAL2_TYPE) 
   {
     qurt_signal2_clear( (qurt_signal2_t *) clnt_timer->cbfn.sigs_obj, clnt_timer->cbdt.sigs );
   }
   else if(clnt_timer->info.callback_type == TIMER_REX_TCB_SIGNAL_TYPE)
   {
#ifdef FEATURE_TIMER_REX_IS_AVAILABLE
    rex_clear_sigs((rex_tcb_type *)clnt_timer->cbfn.sigs_obj, clnt_timer->cbdt.sigs);
#else
    ERR_FATAL("On adsp, signal cannot be of rex type. timer: 0x%x", clnt_timer, 0, 0);
#endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
   }

    /* ... convert given duration into slow clocks, and save in cache */
  free_timer->cache.duration = time;
  //free_timer->cache.duration_sclk = timetick_cvt_to_sclk_tmp64(time, unit );
  DalTimetick_CvtToTimetick64(hTimerHandle, time, unit, &free_timer->cache.duration_sclk);
  /* Retrieve converted time duration from cache */
  time = free_timer->cache.duration_sclk;

   /* Convert and cache given reload interval into slow clocks */
  free_timer->cache.reload      = reload;
  //free_timer->cache.reload_sclk = timetick_cvt_to_sclk_tmp64(reload, unit );
  DalTimetick_CvtToTimetick64(hTimerHandle, reload, unit, &free_timer->cache.reload_sclk);
  /* Retrieve converted reload interval from cache */
  reload = free_timer->cache.reload_sclk;

  /* Ensure cached unit value is correct */
  free_timer->cache.unit = unit;

  /* Determine when timer should expire, and set reload */
  free_timer->start  = ticks_now;
  free_timer->expiry = free_timer->start + time;
  free_timer->reload = reload;
  
  /* Insert timer in either active timer list, or group timer list if it
     belongs to a disabled timer group */
  if ( timer_internal_groups[clnt_timer->info.group_idx]->disabled )
  {
    /* Timer belongs to a disabled group - add it to that group's list */
    timer_insert( &timer_internal_groups[clnt_timer->info.group_idx]->list, free_timer, free_timer->start);    
  }
  else
  {
    /* Timer is not a member of a disabled group - add to active list */
    timer_insert( &timers.active.list, free_timer, free_timer->start );

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_TIMER_SET, free_timer->start);
  }
  
  clnt_timer->info.expiry_type = TIMER_RELATIVE_EXPIRY;


  TIMER_API_LOG_EXIT(TE_SUCCESS);

  ATS_INTFREE();
  return TE_SUCCESS;
} /* timer_drv_set_64 */

/*=============================================================================

FUNCTION TIMER_SET_ABSOLUTE

DESCRIPTION
  Sets an inactive timer to expire at a given time, or changes
  an active timer to expire at a given time.

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS always currently

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_drv_set_absolute
(
  /* Timer to set */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,    

  /* Time (in XOs) until first timer expiry */
  time_timetick_type              time,
  /* process index */
  unsigned int                    process_idx

)
{
  timer_error_type              status = TE_SUCCESS;
  /* Client Timer */
  timer_client_ptr_type         clnt_timer = NULL;
  /* Local internal timer ptr */
  timer_ptr_internal_type       free_timer = NULL;
  /* Timeticks */
  time_timetick_type            preintlock_val = 0;
  time_timetick_type            postintlock_val = 0;
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  /* Specific to set_hard function... check to see if we pass expiry when acq lock */  
  DalTimetick_GetTimetick64(hTimerHandle, &preintlock_val);
  
  ATS_INTLOCK();
  
  ticks_now = preintlock_val;
  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_SET_ABSOLUTE,timer,timer_actual_addr,ticks_now,process_idx);
  

  status = timer_set_get_cmd_buf(timer, process_idx, &clnt_timer, &free_timer);
  if(clnt_timer == NULL || free_timer == NULL)
  {
     TIMER_API_LOG_EXIT(status);
     ATS_INTFREE();
     return status;
  }

  /* Defensive programming. Clearing the signal that will
  be set when the timer expires.*/
  if ( clnt_timer->info.callback_type == TIMER_NATIVE_OS_SIGNAL_TYPE )
   {
     qurt_anysignal_clear( (qurt_anysignal_t *)clnt_timer->cbfn.sigs_obj, clnt_timer->cbdt.sigs );
   }
   else if(clnt_timer->info.callback_type == TIMER_NATIVE_OS_SIGNAL2_TYPE ) 
   {
     qurt_signal2_clear((qurt_signal2_t *)clnt_timer->cbfn.sigs_obj, clnt_timer->cbdt.sigs );
   }
   else if(clnt_timer->info.callback_type == TIMER_REX_TCB_SIGNAL_TYPE)
   {
#ifdef FEATURE_TIMER_REX_IS_AVAILABLE
    rex_clear_sigs((rex_tcb_type *)clnt_timer->cbfn.sigs_obj, clnt_timer->cbdt.sigs);
#else
    ERR_FATAL("On adsp, signal cannot be of rex type. timer: 0x%x", clnt_timer, 0, 0);
#endif /* FEATURE_TIMER_REX_IS_AVAILABLE */
   }
  
  /* Get time just before calculations */
  DalTimetick_GetTimetick64(hTimerHandle, &postintlock_val);
  
  if( postintlock_val - preintlock_val  > time - preintlock_val )
  {
    /* We will want to expire this timer right away */
    /* ... convert given duration into slow clocks, and save in cache */
    free_timer->cache.duration      = 0;
    free_timer->cache.duration_sclk = 0;
    
    /* No reload allowed for this */
    free_timer->cache.reload      = 0;
    free_timer->cache.reload_sclk = 0;
    
    /* Ensure cached unit value is correct */
    free_timer->cache.unit = T_TICK;
    
    /* Determine when timer should expire, and set reload */
    
    free_timer->start  = postintlock_val;
    free_timer->expiry = postintlock_val;
    free_timer->reload = 0;    
  }
  else
  {  
   /* ... convert given duration into slow clocks, and save in cache */
   free_timer->cache.duration      = time - postintlock_val;
   free_timer->cache.duration_sclk = free_timer->cache.duration;
  
       /* No reload allowed for this */
   free_timer->cache.reload      = 0;
   free_timer->cache.reload_sclk = 0; /* Is this okay? */
  
   /* Ensure cached unit value is correct */
   free_timer->cache.unit = T_TICK;
  
   /* Determine when timer should expire, and set reload */
  
   free_timer->start  = postintlock_val;
   free_timer->expiry = time;
   free_timer->reload = 0;   
  }
  
    /* Insert timer in either active timer list, or group timer list if it
     belongs to a disabled timer group */
  
  if ( timer_internal_groups[clnt_timer->info.group_idx]->disabled )
  {
    /* Timer belongs to a disabled group - add it to that group's list */
    timer_insert( &timer_internal_groups[clnt_timer->info.group_idx]->list, free_timer, free_timer->start );
  }
  else
  {
    /* Timer is not a member of a disabled group - add to active list */
    timer_insert( &timers.active.list, free_timer, free_timer->start );
  
    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_TIMER_SET, free_timer->start);
  }

  clnt_timer->info.expiry_type = TIMER_ABSOLUTE_EXPIRY;

  TIMER_API_LOG_EXIT(TE_SUCCESS);
  ATS_INTFREE();
  return TE_SUCCESS;
} /* timer_drv_set_absolute */


/*=============================================================================

FUNCTION TIMER_EXPIRES_IN

DESCRIPTION
  Get number of ticks before timer expires

DEPENDENCIES
  Timer must be active (on a timer list)
  Internal function, called from ATS_INTLOCK context.

RETURN VALUE
  Number of ticks before timer expires.
  Zero is returned for inactive timers, and timers that have expired or
  are expiring.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_expires_in
(
  /* Timer to get */
  timer_ptr_internal_type         timer,

  /* Current slow clock tick count */
  time_timetick_type*                   now
)
{
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  /* Determine if the timer has expired or not */

  if ( timer->expiry   >=  (*now)  )
  {
    /* Timer has not expired - compute time till expiry. */

    return ( timer->expiry - (*now) );
  }
  else
  {
    /* Timer has expired - return zero */

    return 0;
  }

} /* timer_expires_in */


/*=============================================================================

FUNCTION TIMER_DRV_CLR_64
  Stops an active timer. Returns amount of time remaining before expiry
  in terms of specified unit.
  
DESCRIPTION
  Stops an active timer

DEPENDENCIES
  None

RETURN VALUE
  timer_error_type. Possible return values are:
  TE_INVALID_PARAMETERS
  TE_INVALID_TIMER_HANDLE
  TE_TIMER_ALREADY_IN_SAME_STATE
  TE_SUCCESS

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_drv_clr_64
(
  /* Timer to stop */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,

  /* Unit to measure "ticks before expiry" in.  Use T_NONE if not required */
  timer_unit_type                 unit,
  /* process index */
  unsigned int                    process_idx,
  
  time_timetick_type             *rem_time
)
{
  timer_error_type                status = TE_SUCCESS;
  
  timer_client_ptr_type           clnt_timer = NULL;
  
  /* Ticks until timer expires */
  time_timetick_type              ticks=0;

  time_timetick_type              ticks_now = 0;  

  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int = NULL;

  timer_internal_type             timer_internal_copy;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(rem_time != NULL)
     *rem_time = 0;
	 
  if(*timer == TIMER_INVALID_HANDLE)
     return TE_INVALID_PARAMETERS;

  ATS_INTLOCK();

  ticks_now = timetick_get_tmp64();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_CLR,timer,timer_actual_addr,ticks_now,process_idx);

  status = timer_get_clnt_int_timers(timer, process_idx, &clnt_timer, &timer_int);

  /* Timer clr logging */
  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    timer_clr_log[timer_clr_log_cnt].ext_timer 		= timer_actual_addr;
    timer_clr_log[timer_clr_log_cnt].int_timer 		= timer_int;
    timer_clr_log[timer_clr_log_cnt].ts 			= ticks_now;
    timer_clr_log[timer_clr_log_cnt].ret_val 		= status;
    if (clnt_timer != NULL)
      timer_clr_log[timer_clr_log_cnt].timer_state 	= clnt_timer->timer_state;
    else
    	timer_clr_log[timer_clr_log_cnt].timer_state= TIMER_UNDEF_FLAG;
    timer_clr_log[timer_clr_log_cnt].tid			= qurt_thread_get_id();
  #endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES */


  
  if(timer_int == NULL || clnt_timer == NULL)
  {
     TIMER_API_LOG_EXIT(status);
	 
     #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
       TIMER_INC_MOD(timer_clr_log_cnt,MAX_TIMER_CLR_LOG);
     #endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES */
	 
     ATS_INTFREE();
     if(rem_time)
        *rem_time = 0;
     if(status == TE_TIMER_NOT_ACTIVE)
        return TE_TIMER_ALREADY_IN_SAME_STATE;
     return status;
  }

  if ( timer_int->list != NULL )
  {
    /* Timer is active - record remaining time and remove timer */

    //ticks = timer_expires_in( timer_int );
    memscpy(&timer_internal_copy, sizeof(timer_internal_copy), timer_int, sizeof(timer_internal_type));
    timer_remove( timer_int);
    timer_free_internal_timer(timer_int);
    
    DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_TIMER_CLEARED, ticks_now);

    ticks = timer_expires_in( &timer_internal_copy, &ticks_now );

	status = TE_SUCCESS;
  }
  /*if the timer was paused, then it was removed from the active list
   or the disabled list, but it was not returned back to the free 
   queue. Hence, if a paused timer was cleared, return it back
   to the free queue*/
  else if(clnt_timer->timer_state == TIMER_PAUSED_FLAG) 
  {
    timer_free_internal_timer(timer_int);      
    timers.paused_timers_count--;
	status = TE_SUCCESS;
  }
  else
  {
    /* Timer was not active */
    ticks = 0;
	status = TE_TIMER_NOT_ACTIVE;
  }

  clnt_timer->timer_state = TIMER_CLEARED_FLAG;
  //clnt_timer->timer_state_at = ticks_now;
  clnt_timer->timer_state_at = 0;  /* Get timetick call takes 10usecs around. Therefore for now setting zero */
  clnt_timer->timer_int_handle = TIMER_BUFFER_INVALID_HANDLE;

  /* Timer clr logging */
  #ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
    timer_clr_log[timer_clr_log_cnt].ret_val = status;
	TIMER_INC_MOD(timer_clr_log_cnt,MAX_TIMER_CLR_LOG);
  #endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES */



  TIMER_API_LOG_EXIT(TE_SUCCESS);  
  ATS_INTFREE();
  
  if(rem_time)
     *rem_time = timetick_cvt_from_sclk_tmp64( ticks, unit );
  return TE_SUCCESS;
} /* timer_drv_clr_64 */


/*=============================================================================

FUNCTION TIMER_DRV_UNDEF

DESCRIPTION
  Stops an active timer, Frees the timer memory for the same

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS if success
  TE_TIMER_ALREADY_IN_SAME_STATE if timer handle is invalid or undefined previously
  

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_drv_undef
(
  /* Timer to stop */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,

  /* process index */
  unsigned int                    process_idx
)
{
timer_error_type				status = TE_SUCCESS;
  
  timer_client_ptr_type           clnt_timer = NULL;

  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int = NULL;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_UNDEF, timer, timer_actual_addr, timetick_get_tmp64(), process_idx);
  
  status = timer_get_clnt_int_timers(timer, process_idx, &clnt_timer, &timer_int);
  if(clnt_timer == NULL)
  {
    TIMER_API_LOG_EXIT(TE_TIMER_ALREADY_IN_SAME_STATE);
    ATS_INTFREE();
    return TE_TIMER_ALREADY_IN_SAME_STATE;
  }

  /* Remove from active list if active, Free the internal timer */
  if ( timer_int && timer_int->list != NULL )
  {
    /* Timer is active - record remaining time and remove timer */
    
    timer_remove( timer_int);
    timer_free_internal_timer(timer_int);

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt(TIMER_MVS_TIMER_UNDEFINED, timetick_get_tmp64());
  }
  /*if the timer was paused, then it was removed from the active list
   or the disabled list, but it was not returned back to the free 
   queue. Hence, if a paused timer was cleared, return it back
   to the free queue*/
  else if(timer_int && clnt_timer->timer_state == TIMER_PAUSED_FLAG) 
  {
    timer_free_internal_timer(timer_int);      
    timers.paused_timers_count--;
  }


  clnt_timer->timer_state = TIMER_UNDEF_FLAG;
  //clnt_timer->timer_state_at = timetick_get_tmp64();
  clnt_timer->timer_state_at = 0;  /* Get timetick call takes 10usecs around. Therefore for now setting zero */
  clnt_timer->timer_int_handle = TIMER_BUFFER_INVALID_HANDLE;

  /* Free the client timer */
  status = timer_free_client_timer(timer);

  *timer = TIMER_INVALID_HANDLE;

  TIMER_API_LOG_EXIT(status);  
  ATS_INTFREE();
  return status;
} /* timer_undef */


/*=============================================================================

FUNCTION TIMER_DRV_DELETE_PROCESS_TIMERS

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
timer_error_type timer_drv_delete_process_timers(uint32 process_idx)
{
   timer_client_ptr_type clnt_timer = NULL;
   timer_ptr_internal_type timer_int = NULL;
   timers_client_buffer_type *chunk_ptr = NULL;
   uint32 tid = 0;
   uint32 chunk_idx = 0;
   uint32 num_timers = 0;
   
   ATS_INTLOCK();
   
   for(chunk_idx = 0; chunk_idx < timer_client_chunks_allocated; chunk_idx++)
   {
      chunk_ptr = &timer_client_chunks[chunk_idx];
      clnt_timer = &timer_client_chunks[chunk_idx].buffer[0];
      num_timers = timer_client_chunks[chunk_idx].num_timers;
      
      for(tid=0; tid < num_timers; tid++, clnt_timer++)
      {
         if( (clnt_timer->info.process_idx == process_idx) &&
		 	 (clnt_timer->info.node_state != NODE_IS_FREE))
         {
            timer_int = timer_get_internal_timer ( clnt_timer );
            if ( timer_int != NULL )
            {
              /* Remove from active list if active, Free the internal timer */
              if ( timer_int->list != NULL )
              {
                /* Timer is active - record remaining time and remove timer */
                
                timer_remove( timer_int);
                timer_free_internal_timer(timer_int);
              }
              else if(clnt_timer->timer_state == TIMER_PAUSED_FLAG) 
              {
                timer_free_internal_timer(timer_int);      
                timers.paused_timers_count--;
              }
            }
            clnt_timer->timer_int_handle = TIMER_BUFFER_INVALID_HANDLE;
            clnt_timer->info.node_state  = NODE_IS_FREE;
            clnt_timer->timer_state      = TIMER_DEFAULT_FLAG;
            //clnt_timer->timer_state_at = timetick_get_tmp64();
            clnt_timer->timer_state_at = 0;  /* Get timetick call takes 10usecs around. Therefore for now setting zero */

            /* If there are no previous free timers, keep buffer_idx as this tid */
            if(chunk_ptr->allocated_cnt == chunk_ptr->num_timers)
            {
               chunk_ptr->buffer_idx = tid;
            }

            /* Decrement the allocated count for this chunk */
            chunk_ptr->allocated_cnt--;
		 
            /* Increment the count of free client timers */
            timer_free_client_timers_cnt++;
			
         } /* if */
      } /* for tid */
   } /* for chunk_idx*/

   /* Active timer list has changed - ensure next timer event is correct */
   timer_update_timer_interrupt(TIMER_MVS_TIMER_UNDEFINED, timetick_get_tmp64());
   
   ATS_INTFREE();
   return TE_SUCCESS;
} /* timer_drv_delete_process_timers */


/*=============================================================================

FUNCTION TIMER_DRV_DELETE_PROCESS_GROUPS

DESCRIPTION
  Deletes groups related to a process

DEPENDENCIES
  None

RETURN VALUE
  TE_SUCCESS if success

SIDE EFFECTS
  None

=============================================================================*/
timer_error_type timer_drv_delete_process_groups(uint32 process_idx)
{
   timer_group_t_ptr group = NULL;
   timer_group_t_ptr prev_group = &timers.active;  
   timer_group_t_ptr group_to_free = NULL;  

   //ERR_FATAL("Intentionally doing it",0,0,0);

   group = prev_group->next;
   
   ATS_INTLOCK();
   do
   {
	 if ( group->process_idx == process_idx )
	 { 
       if (prev_group != NULL)
       	{
       	  prev_group->next = group->next;
       	}

       group_to_free = group;
	   group = group->next;
   
	   /* delete & free the internal timer group belonging to this PD */
	   timer_delete_group(group_to_free->group_idx);

	   continue;
	 }

	 /* Update the prev_group & group pointers */
     prev_group = group;
	 group = group->next;
	 
   }while ( group != &timers.active );



   ATS_INTFREE();
   return TE_SUCCESS;
} /* timer_drv_delete_process_groups */


/*=============================================================================

FUNCTION TIMER_DRV_PAUSE

DESCRIPTION
  Pauses an active timer.

DEPENDENCIES
  Should only be called for a timer that is running.

RETURN VALUE
  timer_error_type. Possible return values can be
  TE_INVALID_PARAMETERS
  TE_INVALID_TIMER_HANDLE
  TE_TIMER_NOT_ACTIVE
  TE_TIMER_ALREADY_IN_SAME_STATE
  TE_SUCCESS

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_drv_pause
(
  /* Timer to pause */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,
  
  /* process index */
  unsigned int                    process_idx
)
{
  timer_error_type                status = TE_SUCCESS;
  /* Client timer */
  timer_client_ptr_type           clnt_timer = NULL;
  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int = NULL;
  /* now */
  time_timetick_type              now=0;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ATS_INTLOCK();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_PAUSE,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);
  
  status = timer_get_clnt_int_timers(timer, process_idx, &clnt_timer, &timer_int);
  if(clnt_timer == NULL || timer_int == NULL) 
  { /* Here timer_int is required, if its present, clnt_timer is also present */
     TIMER_API_LOG_EXIT(status);
     ATS_INTFREE();
     return status;
  }
  
  /* If already Paused */
  if(clnt_timer->timer_state == TIMER_PAUSED_FLAG) 
  {
     TIMER_API_LOG_EXIT(TE_TIMER_ALREADY_IN_SAME_STATE);
     ATS_INTFREE();
     return TE_TIMER_ALREADY_IN_SAME_STATE;
  }

  if ( timer_int->list != NULL )
  {
    /* Timer is active - remove it, and record remaining time */
   
    timer_remove( timer_int);

    DalTimetick_GetTimetick64(hTimerHandle, &now);

    /* Active timer list has changed - ensure next timer event is correct */
    timer_update_timer_interrupt( TIMER_MVS_TIMER_PAUSED, now);

    timer_int->expiry = timer_expires_in( timer_int, &now );

    timers.paused_timers_count++;
  }

  clnt_timer->timer_state    = TIMER_PAUSED_FLAG;
  clnt_timer->timer_state_at = now;

  TIMER_API_LOG_EXIT(TE_SUCCESS);  
  ATS_INTFREE();
  return TE_SUCCESS;
} /* timer_drv_pause */


/*=============================================================================

FUNCTION TIMER_DRV_RESUME

DESCRIPTION
  Resumes a previously paused timer.

  If a timer was paused 5 seconds before its expiry, and 30 seconds later
  timer_resume( ) is called, the timer will expire 5 seconds after that point.

DEPENDENCIES
  Must only be called for a timer that has been paused.

RETURN VALUE
  timer_error_type. Possible return values are
  TE_INVALID_PARAMETERS
  TE_INVALID_TIMER_HANDLE
  TE_TIMER_NOT_ACTIVE
  TE_TIMER_ALREADY_IN_SAME_STATE
  TE_SUCCESS

SIDE EFFECTS
  May generate signals to a task, which can cause task switches.
  May queue an Asynchronous Procedure Call.

=============================================================================*/

timer_error_type timer_drv_resume
(
  /* Timer to resume */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,

  /* Process Idx */
  uint32                          process_idx
)
{
  timer_error_type                status = TE_SUCCESS;
  /* Client timer ptr */
  timer_client_ptr_type           clnt_timer = NULL;
  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int = NULL;
  /* Ticks until timer expires */
  time_timetick_type              start = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ATS_INTLOCK();
  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_RESUME,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);

  status = timer_get_clnt_int_timers(timer, process_idx, &clnt_timer, &timer_int);
  if(clnt_timer == NULL || timer_int == NULL) 
  { /* Here timer_int is required, if its present, clnt_timer is also present */
     TIMER_API_LOG_EXIT(status);
     ATS_INTFREE();
     return status;
  }
  
  if(clnt_timer->timer_state != TIMER_PAUSED_FLAG)
  {
     TIMER_API_LOG_EXIT(TE_TIMER_ALREADY_IN_SAME_STATE);
     ATS_INTFREE();
     return TE_TIMER_ALREADY_IN_SAME_STATE;
  }
  
  if ( timer_int->list == NULL )
  {
    /* Determine when timer should expire */

    DalTimetick_GetTimetick64(hTimerHandle, &start);
    timer_int->expiry += start;

    /* Insert timer in either active timer list, or group timer list if it
      belongs to a disabled timer group */

    if ( timer_internal_groups[clnt_timer->info.group_idx]->disabled == 1)
    {
      /* Timer belongs to a disabled group - add it to that group's list */
      timer_insert( &timer_internal_groups[clnt_timer->info.group_idx]->list, timer_int, start );
    }
    else
    {
      /* Timer is not a member of a disabled group - add to active list */
      timer_insert( &timers.active.list, timer_int, start);

      /* Active timer list has changed - ensure next timer event is correct */
      timer_update_timer_interrupt(TIMER_MVS_TIMER_RESUMED, start);
    }

    timers.paused_timers_count--;
  }
  
  clnt_timer->timer_state    = TIMER_RESUMED_FLAG;
  //clnt_timer->timer_state_at = timetick_get_tmp64();
  clnt_timer->timer_state_at = 0;  /* Get timetick call takes 10usecs around. Therefore for now setting zero */

  TIMER_API_LOG_EXIT(TE_SUCCESS);
  ATS_INTFREE();
  return TE_SUCCESS;
} /* timer_drv_resume */


/*=============================================================================

FUNCTION TIMER_DRV_CLR_OSAL_THREAD_TIMERS

DESCRIPTION
  Stops active timers belonging to the given task thread

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/

void timer_drv_clr_osal_thread_timers
(
  /* Timers belonging to this task will be cleared (stopped). */
  time_osal_thread_id                    thread_id
)
{
  /* Pointer to a timer group.  Used for walking list of timer groups */
  timer_group_t                  *group = NULL;
  
  timer_client_ptr_type           clnt_timer = NULL;

  /* Pointer to a timer.  Used for walking list of timers */
  timer_ptr_internal_type         timer = NULL;

  /* Pointer to the next timer.  Used for walking list of timers */
  timer_ptr_internal_type         next = NULL;

  time_timetick_type              now=0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_CLR_OSAL_THREAD_TIMERS,NULL,NULL,timetick_get_tmp64(),TIMER_DUMMY_PROCESS_INDEX);

  /* Walk list of timer groups, starting with the faux active group */
  group = &timers.active;

  do
  {
    /* Walk the list of timers in this group, and
       remove timers which belong to given task */

    for( timer = group->list.first;  timer != NULL;  timer = next)
    {
      /* Save next timer in list, in case this timer's next field in NULL'd */
      next = timer->next;

      if ( timer->thread_id == thread_id)
      {
        if ( timer->timer_ext != NULL )
           clnt_timer = timer->timer_ext;
        else
        {
          //ERR_FATAL (" Timer must have an ext timer ",0,0,0);
          TIMER_API_LOG_EXIT(TE_FAIL);
          MSG(MSG_SSID_ATS, MSG_LEGACY_ERROR," Timer must have an ext timer ");
          ATS_INTFREE();
          return;
        }

        /* Remove the timer from the list */
        timer_remove( timer );
        timer_free_internal_timer(timer);

        clnt_timer->timer_state       = TIMER_CLEARED_FLAG;
        //clnt_timer->timer_state_at    = timetick_get_tmp64();
        clnt_timer->timer_state_at    = 0;  /* Get timetick call takes 10usecs around. Therefore for now setting zero */
        clnt_timer->timer_int_handle  = TIMER_BUFFER_INVALID_HANDLE;
      }
    }
    /* Determine the next timer group in the chain ... stopping when we
       return to the start of the list with the faux timer group */
    group = group->next;
  }while ( group != &timers.active );

  DalTimetick_GetTimetick64(hTimerHandle, &now);
  
  /* Active timer list may have changed - ensure next timer event is correct */
  timer_update_timer_interrupt(TIMER_MVS_TIMER_CLEAR_TASK_TIMERS, now );

  TIMER_API_LOG_EXIT(TE_SUCCESS);
  ATS_INTFREE();
  return;
} /* timer_drv_clr_osal_thread_timers */


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

time_timetick_type timer_drv_get_64
(
  /* Timer to get */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr, 

  /* Unit to measure "ticks before timer expires" in. */
  timer_unit_type                     unit,
  /* process index */
  unsigned int                    process_idx
  
)
{
  /* Ticks until timer expires */
  time_timetick_type              ticks = 0;
  /* Client timer ptr */
  timer_client_ptr_type           clnt_timer = NULL;
  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int = NULL;
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  /* This API LOG should be before any possible early exit of the function */
  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_GET,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);
  
  (void)timer_get_clnt_int_timers(timer, process_idx, &clnt_timer, &timer_int);
  if( clnt_timer == NULL || timer_int == NULL)
  {
     TIMER_API_LOG_EXIT(TE_FAIL);
     ATS_INTFREE();
     return 0;
  }

  if ( timer_int->list != NULL )
  {
    /* Timer is active. Ensure non-zero tick count */
    DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);
    ticks = timer_expires_in( timer_int, &ticks_now );
    if ( ticks == 0 )
    {
      ticks = 1;
    }
  }
  else
  {
    /* Timer is not active */
    ticks = 0;
  }

  ATS_INTFREE();

  /* If timer is active then ensure that return value is non-zero */
  if ( ticks != 0 )
  {
    ticks = timetick_cvt_from_sclk_tmp64( ticks, unit);
    if ( ticks == 0 )
    {
      ticks = 1;
    }
  }

  TIMER_API_LOG_EXIT(TE_SUCCESS);
  return ticks;
} /* timer_drv_get_64 */


/*=============================================================================

FUNCTION TIMER_DRV_GET_SCLK_TILL_EXPIRY_64

DESCRIPTION
Returns ticks till first active timer's expiry.
Ticks returned by this api are always target specific.
On targets with sclk, it will be sclk ticks while on
targets with XO clk, XO ticks are provided 

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  Number of target ticks

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_drv_get_sclk_till_expiry_64( void )
{
  time_timetick_type                   tt_ret = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  /* Get the current time */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_GET_SCLK_TILL_EXPIRY,NULL,NULL,ticks_now,TIMER_DUMMY_PROCESS_INDEX);

  if ( timers.set_value - timers.set_time > ticks_now - timers.set_time )
  {  
    /* Compute & return the time until the timer is set to expire */
    tt_ret = timers.set_value - ticks_now;
  }
  else
  {  
    /* The next timer interrupt is already pending! */
    tt_ret = 0;
  }

  TIMER_API_LOG_EXIT(TE_SUCCESS);

  ATS_INTFREE();

  return tt_ret;
} /* timer_drv_get_sclk_till_expiry_64 */


/*=============================================================================

FUNCTION TIMER_DRV_SET_WAKEUP_64

DESCRIPTION
  Sets the timer hw to expire in advance of the next expiring 
  timer expiry point to allow for TCXO to turn on.

DEPENDENCIES
  This function is provided for the exclusive use of SLEEP, for TCXO shutdown.
  Must be called from an ATS_INTLOCK'd context.

RETURN VALUE
  timer_error_type
  TE_SUCCESS always currently

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_drv_set_wakeup_64
(
  /* Wakeup the CPU in the given number of sclk's. */
  time_timetick_type                   wakeup_sclks
)
{
  /* Current slow clock count */
  time_timetick_type                   ticks_now = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Get the current time2 */
  DalTimetick_GetTimetick64(hTimerHandle, &ticks_now);

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_SET_WAKEUP,NULL,NULL,ticks_now,TIMER_DUMMY_PROCESS_INDEX);
  /* Program the timer */
  timer_set_next_interrupt( ticks_now + wakeup_sclks, ticks_now, FALSE, 
                            TIMER_MVS_SET_WAKEUP );

  TIMER_API_LOG_EXIT(TE_SUCCESS);
  return TE_SUCCESS;
}
/* timer_drv_set_wakeup_64 */

/*=============================================================================

FUNCTION TIMER_GET_TIMER_PARAMETER

DESCRIPTION
  Get requested timer parameter value or state.
  param_val2 is not always mandatory.

DEPENDENCIES
  
RETURN VALUE

SIDE EFFECTS
  None

=============================================================================*/
timer_error_type timer_get_timer_parameter(
                           timer_ptr_type timer,
                           unsigned int   process_idx,
                           timer_param_type param,
                           uint64 *param_val1, 
                           uint64 *param_val2)
{
  timer_error_type                status = TE_SUCCESS;
  /* Client timer ptr */
  timer_client_ptr_type           clnt_timer = NULL;
  /* Pointer to internal timer */
  timer_ptr_internal_type         timer_int = NULL;
  
  if(timer == NULL || param_val1 == NULL || param > TIMER_MAX_PARAMS)
     return TE_INVALID_PARAMETERS;
	 
  *param_val1 = 0;
  if(param_val2 != NULL)
     *param_val2 = 0;
  
  if(*timer == TIMER_INVALID_HANDLE)
     return TE_INVALID_TIMER_HANDLE;

  ATS_INTLOCK();
  
  status = timer_get_clnt_int_timers(timer, process_idx, &clnt_timer, &timer_int);
  if(clnt_timer == NULL)
  {
    ATS_INTFREE();
    return status;
  }
  
  
  status = TE_SUCCESS;
  switch(param)
  {
     case TIMER_IS_ACTIVE: 
     {
        if(timer_int && timer_int->list != NULL)
           *param_val1 = (uint64)TRUE;
        else
           *param_val1 = (uint64)FALSE;
        break;
     }
     
     case TIMER_IS_PERIODIC:
     {
        *param_val1 = (uint64)FALSE;
        if(timer_int && timer_int->reload > 0)
        {
            *param_val1 = (uint64)TRUE;
            if(param_val2 != NULL)
               *param_val2 = timer_int->reload;
        }
        break;
     }
     
     case TIMER_EXPIRES_AT_64:
     {
        if(timer_int) 
        {
           *param_val1 = timer_int->expiry;
        }
        break;
     }
     
     case TIMER_GET_START_64:
     {
        if(timer_int) 
        {
           *param_val1 = timer_int->start;
        }
        break;
     }
     
     case TIMER_GET_GROUP:
     {
        if(clnt_timer) 
        {
          *param_val1 = GROUP_INDEX_TO_HANDLE(clnt_timer->info.group_idx);
		}
        break;
     }
     
     case TIMER_GET_EXPIRY_TYPE:
     {
        if(clnt_timer) 
        {	 
          *param_val1 = clnt_timer->info.expiry_type;
		}
        break;
     }
     default: status = TE_INVALID_PARAMETERS;
  }
  
  ATS_INTFREE();
  return status;
} /* timer_get_timer_parameter */

/*=============================================================================

FUNCTION TIMER_DRV_IS_ACTIVE

DESCRIPTION
 This is function which queries whether a timer is active or not.        
 A timer which has been set to expire at a certain time - even if the   
 group it is a member of is disabled - is considered to be active.      
 A timer which is explicitly paused is not active, until it has been    
 resumed.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if the timer is active.FALSE if the timer is not active.

SIDE EFFECTS
  None

=============================================================================*/
boolean timer_drv_is_active
(
  timer_ptr_type  timer,
  timer_ptr_type  timer_actual_addr,
  unsigned int    process_idx
)
{
  uint64                         timer_active = (uint64)FALSE;
  timer_error_type               status;

  ATS_INTLOCK();
  
  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_IS_ACTIVE,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);

  status = timer_get_timer_parameter(timer, process_idx, TIMER_IS_ACTIVE, &timer_active, NULL);

  TIMER_API_LOG_EXIT(status);

  ATS_INTFREE(); 
  return (boolean)timer_active;
} /* timer_is_active */

/*=============================================================================

FUNCTION TIMER_DRV_IS_PERIODIC_TIMER

DESCRIPTION
 Returns TRUE if specified timer is a periodic timer.

DEPENDENCIES
  None

RETURN VALUE
  boolean - TRUE if the timer is periodic. FALSE if the timer is not active.

SIDE EFFECTS
  None

=============================================================================*/
timer_error_type timer_drv_is_periodic_timer
(
  timer_ptr_type      timer,
  timer_ptr_type      timer_actual_addr,
  unsigned int        process_idx,
  boolean            *is_periodic,
  time_timetick_type *period
)
{
  timer_error_type                status = TE_SUCCESS;
  uint64                          is_periodic_64 = (uint64)FALSE;
  uint64                          periodic_temp = 0;
  
  if(is_periodic == NULL)
     return TE_INVALID_PARAMETERS;
  
  *is_periodic = FALSE;

  ATS_INTLOCK();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_IS_PERIODIC,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);  

  status = timer_get_timer_parameter(timer, process_idx, TIMER_IS_PERIODIC, &is_periodic_64, (uint64 *)&periodic_temp);
  *is_periodic = (boolean)is_periodic_64;
  
  if(period)
     *period = (time_timetick_type)periodic_temp;

  TIMER_API_LOG_EXIT(status);

  ATS_INTFREE();
  return status;
} /* timer_drv_is_periodic_timer */


/*=============================================================================

FUNCTION TIMER_DRV_EXPIRES_AT_64

DESCRIPTION
 This returns the XO counter value when the timer    
 will expire at.

DEPENDENCIES
  Timer must be running for the returned value to be meaningful.

RETURN VALUE
  time_timetick_type - XO counter value corresponding to timer expiry point.

SIDE EFFECTS
  None

=============================================================================*/
time_timetick_type  timer_drv_expires_at_64
(
  timer_ptr_type  timer,
  timer_ptr_type  timer_actual_addr,
  unsigned int    process_idx
)
{
  time_timetick_type              expires_in=0;
  timer_error_type                status = TE_SUCCESS;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_EXPIRES_AT,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);

  status = timer_get_timer_parameter(timer, process_idx, TIMER_EXPIRES_AT_64, (uint64 *)&expires_in, NULL);

  TIMER_API_LOG_EXIT(status);

  ATS_INTFREE();

  return expires_in;
} /* timer_drv_expires_at_64 */


/*=============================================================================

FUNCTION TIMER_DRV_GET_START_64

DESCRIPTION
  Returns the time tick count when the timer started. 

DEPENDENCIES
  Timer must be running for the returned value to be meaningful.

RETURN VALUE
  time_timetick_type - TCXO counter value corresponding to timer start point.

SIDE EFFECTS
  None

=============================================================================*/

time_timetick_type timer_drv_get_start_64
(
  timer_ptr_type  timer,
  timer_ptr_type  timer_actual_addr,
  /* process index */
  unsigned int    process_idx
)
{
  time_timetick_type              timer_start=0;
  timer_error_type                status = TE_SUCCESS;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ATS_INTLOCK();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_GET_START,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);

  status = timer_get_timer_parameter(timer, process_idx, TIMER_GET_START_64, (uint64 *)&timer_start, NULL);

  TIMER_API_LOG_EXIT(status);

  ATS_INTFREE();

  return timer_start;
} /* timer_drv_get_start_64 */


/*=============================================================================

FUNCTION TIMER_DRV_GET_GROUP

DESCRIPTION
 Gets group handle

DEPENDENCIES
 Timer should be existing
 
RETURN VALUE
  Group Handle

SIDE EFFECTS
  None

=============================================================================*/

timer_error_type timer_drv_get_group
(
  timer_ptr_type  timer,
  timer_ptr_type  timer_actual_addr,
  unsigned int    process_idx,
  timer_group_type *group
)
{
   timer_error_type              status = TE_SUCCESS;
   uint64                        timer_group = 0;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  if(group == NULL)
     return TE_INVALID_PARAMETERS;

  ATS_INTLOCK();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_GET_GROUP,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);
  
  status = timer_get_timer_parameter(timer, process_idx, TIMER_GET_GROUP, &timer_group, NULL);
  *group = (timer_group_type) timer_group;

  TIMER_API_LOG_EXIT(status);

  ATS_INTFREE();
  
  return status;
} /* timer_drv_get_group */

/* =============================================================================

FUNCTION TIMER_DRV_GET_EXPIRY_TYPE

DESCRIPTION
 Returns the Expiry type of timer

DEPENDENCIES
  Timer must be set atleast once

RETURN VALUE

SIDE EFFECTS
  None

============================================================================= */

timer_error_type timer_drv_get_expiry_type
(
  timer_ptr_type  timer,
  timer_ptr_type  timer_actual_addr,
  unsigned int    process_idx,
  timer_expiry_type *expiry_type
)
{
   timer_error_type              status = TE_SUCCESS;
   uint64                        expiry_type_64 = (uint64)TIMER_RELATIVE_EXPIRY;

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
 
  if(expiry_type == NULL)
     return TE_INVALID_PARAMETERS;

  ATS_INTLOCK();

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_GET_EXPIRY_TYPE,timer,timer_actual_addr,timetick_get_tmp64(),process_idx);

  status = timer_get_timer_parameter(timer, process_idx, TIMER_GET_EXPIRY_TYPE, &expiry_type_64, NULL);
  *expiry_type = (timer_expiry_type) expiry_type_64;

  TIMER_API_LOG_EXIT(status);

  ATS_INTFREE();
  
  return status;
} /* timer_drv_get_expiry_type */


/*=============================================================================

FUNCTION TIMER_ACTIVE_LIST_CHECK

DESCRIPTION
  Throws an error fatal if timer list is corrupted

DEPENDENCIES
  FEATURE_TIMER_TRACE and FEATURE_TIMER_LIST_DEBUG should be turned on.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void timer_active_list_check(void)
{

#if defined( FEATURE_TIMER_TRACE ) && defined( FEATURE_TIMER_LIST_DEBUG )

	/* Pointer to traverse timer list */
	timer_ptr_internal_type 		 ptimer1, ptimer2;

	/* Number of timers in the list starting from first timer */
	uint32 forward_traversal_count = 0;

	/* Number of timers in the list starting from last timer */
	uint32 reverse_traversal_count = 0;

	/* First & last timer respectively in the active list */
	timer_ptr_internal_type 		 first, last;

	/* timetick for logging this functions entry & exit */
	static time_timetick_type		 timer_list_check_entry = 0;
	static time_timetick_type		 timer_list_check_last_exit = 0;  

	boolean timer_top_of_list = TRUE;

	/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
	timer_list_check_entry = timetick_get_tmp64();

	/* Return if active list is NULL */
	if ( timers.active.list.first == NULL ) {
		return;
	}

	/* Spin thru the active timer list */
	ptimer1 = timers.active.list.first->next;

	if (ptimer1 != NULL) {
		ptimer2 = ptimer1->next;
	}
	else {
		ptimer2 = NULL;
	}

	/* Checks for Cirular loop in timers' list 
	 * Start with 2 pointers ( ptimer1	and ptimer2 ). 
	 * Advance ptimers1 with one node and ptimer2 with two nodes
	 * At any point if ptimer1 == ptimer2 then you have a loop, 
	 * otherwise ptimer2 will reach NULL in n/2 iterations. 
	 */

	while ( ptimer1 != NULL && ptimer2 != NULL ) {
		if (ptimer1 == ptimer2 ) {
			ERR_FATAL(" Timer List Corrupted, ptimer1=0x%x ", ptimer1,0,0 );
		}

		ptimer1 = ptimer1->next;
		ptimer2 = ptimer2->next;

		if ( ptimer2 !=NULL ) {
			ptimer2 = ptimer2->next; 
		}
		else {
			break;
		}
	}

	first = timers.active.list.first;
	last = timers.active.list.last;

	while( first != NULL ) {
		/* To catch the timers that have not been expired though the expiry time
		 * has been crossed. A timer is expired if expiry - start of that timer is
		 * less than now - start. But if such a timer still exists in timers list,
		 * it means it been fired and thus need to error fatal 
		 */
		if( timers.processing == FALSE && (first->expiry < timer_list_check_entry) ) {
			if(timer_top_of_list == FALSE) {
				ERR_FATAL(" timer active list corrupted due to timers not being expired, timer=0x%x ",first,0,0); 
			}
		}
		else {
			/* Starting timers that have crossed expiry are over */
			timer_top_of_list = FALSE;
		}

		/* Check if the links of the doubly linked list are proper starting from the
		 * first timer and also count the number of timer 
		 */
		if( first->next != NULL ) {
			if( first != first->next->prev ) {
				ERR_FATAL(" timer active list corrupted, ptr1=0x%x ptr1->next->prev=0x%x",first,first->next->prev,0); 
			}
			/* Checking for expiries of the timers to be in sorted order for some corruption issues */
			if(first->expiry > first->next->expiry) {
				ERR_FATAL("Timer active list expiry order corrupted, timer=0x%x",first,0,0); 
			}
  		}

		/* Update the pointers for traversal */
		first = first->next;
		forward_traversal_count++;
	}

	/* Count the number of timers in the list starting from last timer */
	while( last != NULL ) {
		last = last->prev;
		reverse_traversal_count++;
	}

	/* List is corrupted if first_num is not same as last_num or is 
	 * not same as actual number of timers supposed to be present 
	 * at given point of time.
	 */
	if (forward_traversal_count != reverse_traversal_count) {
		ERR_FATAL(" timer active list corrupted, fwd_traversal=0x%x reverse_traversal=0x%x",
		forward_traversal_count,reverse_traversal_count,0 );
	}

	timer_list_check_last_exit = timer_list_check_entry ;

#endif /* FEATURE_TIMER_TRACE && FEATURE_TIMER_LIST_DEBUG */

} /*  timer_active_list_check */

/*=============================================================================

FUNCTION TIMER_GET_BLAST_SIG_REGISTRATION_ID

DESCRIPTION
  This function returns the registration id that will be used to signal the 
  expiry of the first timer.

DEPENDENCIES
  This function is applicable only for BLAST.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
int timer_get_blast_sig_registration_id
(
  void
)
{
  //TIMER_API_LOG(TIMER_API_LOG_TIMER_GET_BLAST_SIG_REGISTRATION_ID, NULL, timetick_get_tmp64());
  return timer_blast_sig_registration_id;
}


/*=============================================================================

FUNCTION TIMER_INIT_BLAST_SIGNAL

DESCRIPTION
  This function initializes BLAST signal related to timer expiry. When
  the match value == timetick value, BLAST will set this signal. 

  This function also initializes the mutex that will be used in crit-section
  portions of timer code.

DEPENDENCIES
  This function is applicable only for BLAST.

RETURN VALUE
  None

SIDE EFFECTS
  None

=============================================================================*/
static void timer_init_qurt_signal
(
  void
)
{
  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_INIT_QURT_SIGNAL,NULL,NULL,timetick_get_tmp64(),TIMER_DUMMY_PROCESS_INDEX);

  /*initialize the signal mask that we want to wait on*/
  qurt_anysignal_init (&timer_q6_signal);

  #ifdef FEATURE_TIMER_USE_QURT_SYSCLOCK

  timer_blast_sig_registration_id = 
        qurt_sysclock_register (&timer_q6_signal, TIMER_QURT_SIG_MASK);

  if (timer_blast_sig_registration_id < 0) 
  {
    TIMER_API_LOG_EXIT(TE_FAIL);
    ERR_FATAL("sclk registration failed",0,0,0);    
  }

  /*pass on the blast registration id to the timetick layer*/
  time_timetick_update_blast_signal_id((uint32)timer_blast_sig_registration_id);

  #endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

  TIMER_API_LOG_EXIT(TE_SUCCESS);

}

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
timer_error_type timer_drv_register_for_non_defer_expiry_change
(
  timer_notify_type        notify_type,
  time_osal_notify_obj_ptr notify_obj,
  time_osal_notify_data    notify_mask,
  uint32                   process_idx
)
{
   int idx=0;
   
   if(!timer_is_notify_type_valid(notify_type))
      return TE_INVALID_PARAMETERS;
   
   /* This registration is allowed only for signal type notifications */
   if(notify_type == TIMER_FUNC1_CB_TYPE || 
      notify_type == TIMER_FUNC2_CB_TYPE ||
      notify_type == TIMER_NO_NOTIFY_TYPE)   
      return TE_INVALID_PARAMETERS;

   if(notify_obj == NULL)   
      return TE_INVALID_PARAMETERS;
      
   ATS_INTLOCK();
   TIMER_API_LOG_ENTRY(TIMER_API_TIMER_REGISTER_FOR_NON_DEFER_EXPIRY,NULL,NULL,timetick_get_tmp64(),TIMER_DUMMY_PROCESS_INDEX);
   
   if(timer_non_defer_expiry_notify_clients_cnt == TIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT)
   {
      TIMER_API_LOG_EXIT(TE_FAIL);
      ATS_INTFREE();
      return TE_FAIL;
   }

   idx = timer_non_defer_expiry_notify_clients_cnt++;
   timer_non_defer_expiry_notify_clients[idx].notify_type = notify_type;
   timer_non_defer_expiry_notify_clients[idx].notify_obj = notify_obj;
   timer_non_defer_expiry_notify_clients[idx].notify_mask = notify_mask;
   timer_non_defer_expiry_notify_clients[idx].process_idx = process_idx;

   TIMER_API_LOG_EXIT(TE_SUCCESS);
   ATS_INTFREE();
   
   return TE_SUCCESS;  
} /* timer_register_for_non_defer_expiry_change */

/*=============================================================================

FUNCTION TIMER_UPDATE_ISR_TIME

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
#ifdef FEATURE_TIMER_USE_QURT_SYSCLOCK
void timer_update_isr_time
(
  time_timetick_type* now
)
{
  /* Timer sclk time-stamp values */
  time_timetick_type                   set_time, set_value;

  /* Latency in ISR handling */
  time_timetick_type                   latency_sclk;

  
  timers.isr_time = *now;

  TIMER_API_LOG_ENTRY(TIMER_API_TIMER_UPDATE_ISR_TIMER,NULL,NULL,*now,TIMER_DUMMY_PROCESS_INDEX); 

  /* Save a trace packet */
  TIMER_TRACE(TIMER_TRACE_ISR,now,TIMER_MVS_TIMER_UNDEFINED);  

  /* Capture timer set values, for messaging outside of ATS_INTLOCK. */
  set_time  = timers.set_time;
  set_value = timers.set_value;
  
  if ( *now - set_time >= set_value - set_time)
  {
    /* Determine the difference between when the interrupt was programmed to
       occur at, and when the interrupt was actually handled. */
    latency_sclk = *now - set_value;

    if ( latency_sclk > TIMER_SCLK_LATENCY )
    {
      /* Timer interrupt was handled over btw 5 (60kHz) to 10ms (30kHz) late.
         (Timers may expire much later than 5-10ms, if they are in a disabled
         group, or if multiple timers expire due to the same interrupt.
         This is only a measure of the timer isr latency.) */

      MSG_3(MSG_SSID_ATS, MSG_LEGACY_ERROR,"Late Timer ISR: ST=%d SV=%d IT-SV=%d",
                set_time, set_value, latency_sclk);
    }
  }
  TIMER_API_LOG_EXIT(TE_SUCCESS);
}
#endif /*FEATURE_TIMER_USE_QURT_SYSCLOCK*/

