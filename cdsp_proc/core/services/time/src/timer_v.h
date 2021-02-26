#ifndef TIMER_V_H
#define TIMER_V_H
/*=============================================================================

 T I M E R  S E R V I C E  I N T E R N A L

GENERAL DESCRIPTION
 Provides internal structures and function used by timer. 

EXTERNALIZED FUNCTIONS
  time_genoff_pre_init
    Pre -Initializes Generic offset
  
  time_genoff_post_init
    Completes initialization sequence of Generic offset

  time_genoff_add_base_subsys
    Adds a genoff instance as base_subsystem for another genoff instance

  time_genoff_attach_cb
    Attaches call back which are called during after set operation

  time_genoff_set_remote_genoff
    Updates genoff in other proc

  time_genoff_add_init_call
    Adds initialization sequence for the genoff in consideration

  time_genoff_updates_per_storage
    Updates persistent storage info

  time_genoff_get_pointer
    Returns the Pointer to different time bases

  time_genoff_set_generic_offset
    Sets Generic offset.

  time_genoff_get_offset
    Gets Generic offset



Copyright (c) 2009 - 2018 by QUALCOMM Technologies Incorporated.  All Rights Reserved.

=============================================================================*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ------------------------------------------------------------
10/19/15   abh     Added debug code changes for better logging.


=============================================================================*/



/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "timer.h"
#include "queue.h"

/*=============================================================================

                           DATA DEFINITIONS

=============================================================================*/

#define TIMER_FREQ_VALUE          19200000
#define TIMER_SCLK_LATENCY        TIMER_FREQ_VALUE/100

/*-----------------------------------------------------------------------------
  MACRO for ats timers versioning.
  -----------------------------------------------------------------------------*/
/* Peripheral ID Macro */
#if defined( FEATURE_ATS_PROC_ID_MPSS )
	/* Peripheral ID for MPSS */
	#define ATS_TIMER_VERSION_PERIPHERAL_ID		1
#elif defined( FEATURE_ATS_PROC_ID_ADSP )
	/* Peripheral ID for ADSP */
	#define ATS_TIMER_VERSION_PERIPHERAL_ID 	2
#elif defined( FEATURE_ATS_PROC_ID_WCNSS )
	/* Peripheral ID for WCNSS */
	#define ATS_TIMER_VERSION_PERIPHERAL_ID 	3
#else
	/* Peripheral ID for TN */
	#define ATS_TIMER_VERSION_PERIPHERAL_ID 	4
#endif

/* Macro's for major/minor version */
#define ATS_TIMER_MAJOR_VERSION		1
#define ATS_TIMER_MINOR_VERSION		1

/*-----------------------------------------------------------------------------
  Debug variable sizes
  IMP : These sizes should always be kept in powers of 2
  -----------------------------------------------------------------------------*/
#ifdef FEATURE_ATS_PROC_ID_ADSP
	/* Keeping lesser sizes of the debug variables for ADSP. */
	#define MAX_TIMER_TRACE              64
	#define MAX_TIMER_API_CALLS          64
	#define MAX_MATCH_REC                16
	#define MAX_TIMER_SET                128
	#define MAX_TIMER_CLR_LOG            64
	#define MAX_TIMER_EXPIRED_DEBUG      128
	#define MAX_TIMERS_EXPIRED			 64
	#define MAX_TIMER_REMOTE_EXPIRED     32
	#define MAX_TIMER_REFERNCE_COUNT_LOG 8
#else
	/* These debug variables sizes are kept to support minimal debug. */
	#define MAX_TIMER_TRACE 			 128
	#define MAX_TIMER_API_CALLS 		 128
	#define MAX_MATCH_REC				 32
	#define MAX_TIMER_SET				 256
	#define MAX_TIMER_CLR_LOG			 128
	#define MAX_TIMER_EXPIRED_DEBUG 	 256
	#define MAX_TIMERS_EXPIRED			 128
	#define MAX_TIMER_REMOTE_EXPIRED	 32
	#define MAX_TIMER_REFERNCE_COUNT_LOG 16
#endif

/*-----------------------------------------------------------------------------
  Signals used by the timer task.
-----------------------------------------------------------------------------*/

/* Signal for a timer expiring */
#define TIMER_EXPIRY_SIG			0x00000001

/* Signal mask to indicate timer task to allocate new client timer chunk */
#define TIMER_ALLOCATE_CHUNK_SIG	0x00000004                                   

/* Signal mask to indicate timer task for processing of timers */
#define TIMER_QURT_SIG_MASK			0x00000002

/* Dummy Signal mask  */
#define TIMER_DUMMY_MASK			0x00000008

/* OR'ed signals that timer task will wait on. */
#define TIMER_QURT_TASK_SIGNAL_MASK	(TIMER_QURT_SIG_MASK | TIMER_ALLOCATE_CHUNK_SIG | TIMER_DUMMY_MASK)


/* Interrupt ID Vector */
#define TIMER_INTERRUPT_ID			3 /*qtmr_qgic2_phy_irq[0]*/

#ifdef FEATURE_USE_TIMER_BIN
/*-----------------------------------------------------------------------------
  Timer Bin Configuration
   - Modify these values to adjust/improve binning performance
-----------------------------------------------------------------------------*/

/* This range is a result of the following calculation...
In general we can say that the oscillator has a +/- 100% swing in freq
Therefore lower bound on Latency(Ticks/ms) = 2*f/1000 
If 5ms is max then TIMER_SCLK_LATENCY = (2*f/1000)*5 = f/100 
*/
#define TIMER_BIN_RANGE           (TIMER_FREQ_VALUE/4)

/* Number of bins available. This must be large enough to accomodate all the
   timers in the system, or an err_fatal will be reached */ 
#define TIMER_BIN_NUM             128 

#endif /* FEATURE_USE_TIMER_BIN */

/*-----------------------------------------------------------------------------
  Macro's to define the sizes of various timer structures.
-----------------------------------------------------------------------------*/
/* 
 * TIMER_MAX_GROUPS : Number of timer internal groups, MAX is 256
 * TIMER_CLIENT_CHUNKS_MAX : Max client chunks, Max is 256
 * TIMERS_MIN_CLIENT_TIMERS_FOR_NEW_CHUNK : Threshold for number of total free client timers left to create a new chunk in timer task.
 * MAX_TIMER_BUFFER_NUMBER : Number of internal timer buffers
 * MIN_TIMER_BUFFER_NUMBER
 * TIMER_CLIENT_BUFFER_INIT_ARRAY_SIZE : Number of 2D array elements for the static allocation of client timers
 * TIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT : Number of non-defer notify clients
 */
#define TIMER_MAX_GROUPS 								64
#define MAX_TIMER_BUFFER_NUMBER							64
#define MIN_TIMER_BUFFER_NUMBER							-1
#define TIMER_CLIENT_CHUNKS_MAX							10
#define TIMER_CLIENT_BUFFER_INIT_ARRAY_SIZE				2
#define TIMERS_MIN_CLIENT_TIMERS_FOR_NEW_CHUNK			15
#define TIMER_NON_DEFER_EXPIRY_NOTIFY_CLIENTS_MAX_CNT	4

/* This is a mock pointer passed as the timer address to remote callback 
 * handler for handling the non-defer timer expiry notification changes.
 */
#define TIMER_NON_DEFER_NOTIFICATION_MOCK_PTR ((timer_ptr_type)0x88888888)

/* Timer early tolerance value in native ticks (100 usecs).
 * If a timers expiry falls under this many ticks in furutre,
 * it will be considered fit for expiry and will get processed.
 */
#define TIMER_OS_EARLY_TOL 1920

/* Invalid Handle */
#define TIMER_BUFFER_INVALID_HANDLE   -1

/* Invalid handle values for external group & timer variables */
#define GROUP_INVALID_HANDLE 0
#define TIMER_INVALID_HANDLE 0

/* The range (in timeticks) of each timer bin 
    - This must be (2^n - 1) to work (for bitwise optimizations)
    - 8191 = 0.25 sec bins */

#define MAGIC_INITIALIZER         0xDEADBEEF
#define MAGIC_INITIALIZER_64      0xDEADBEEFD00DFEEDuLL

/* Factor to divide 19.2MHz clock into 32kHz */
#define XO_TO_SCLK_CONV_FACTOR 586

/* Macro for incrementing counter's with range in powers of 2 */
#define TIMER_INC_MOD(x,s)         ( x = ( x + 1 ) & ( s - 1 ) )

/* Dummy process index for places where we just assign & not use */
#define TIMER_DUMMY_PROCESS_INDEX  0xFDFDFDFD

#define TIMER_FROM_REMOTE_PROCESS 3

/** @brief Pointer to the timer client structure. */
typedef struct timer_client_struct *timer_client_ptr_type;
/** @brief Pointer to the internal group structure. */
typedef struct timer_group_s *timer_group_t_ptr;
/** @brief Pointer to the timer list structure. */
typedef struct timer_struct_internal *timer_ptr_internal_type;

/** Pointer to timer list structure */
typedef struct timer_list_struct  *timer_list_ptr;
/** Pointer to timer bin structure */
typedef struct timer_bin_struct   *timer_bin_ptr;

/* Remote Process Callback Handler type */
typedef void (*timer_process_cb_handler)
(
  /* Index of the Process */
  uint32                          process_idx, 
  /* Timer pointer */
  timer_ptr_type                  timer,
  /* Callback type */
  timer_notify_type               cb_type,
  
  time_osal_notify_obj_ptr        sigs_func_addr,
  
  time_osal_notify_data           sigs_mask_data,
  
  time_timetick_type              run_time_ms
);

/*=============================================================================

                           ENUMERATIONS

=============================================================================*/

/* Enumerated type for Parameters */
typedef enum
{
  TIMER_IS_ACTIVE,
  TIMER_IS_PERIODIC,
  TIMER_EXPIRES_AT_64,
  TIMER_GET_START_64,
  TIMER_GET_GROUP,
  TIMER_GET_EXPIRY_TYPE,
  TIMER_MAX_PARAMS
} timer_param_type;

/** Timer Queue flags with magic number */
typedef enum timer_internal_queue
{
  NODE_IS_FREE = 0x3,
  NODE_IS_NOT_FREE = 0xC 
} timer_internal_q;

typedef enum
{
  TIMER_ABSOLUTE_EXPIRY,
  TIMER_RELATIVE_EXPIRY
}timer_expiry_type;

/** @brief Timer state structure type. */
typedef enum timer_state_struct
{
  TIMER_DEFAULT_FLAG = 0x0FEDCBA9,
  TIMER_DEF_FLAG,
  TIMER_SET_FLAG,
  TIMER_EXPIRED_FLAG,
  TIMER_CLEARED_FLAG,
  TIMER_PAUSED_FLAG,
  TIMER_RESUMED_FLAG,
  TIMER_UNDEF_FLAG,
} timer_state_struct_type;

/*This enum will be used to specify the event
 responsible for setting the match value*/
typedef enum
{
  TIMER_MVS_TIMER_PROCESSED = 0x1,
  TIMER_MVS_TIMER_SET,
  TIMER_MVS_DEFER_MATCH_INT,
  TIMER_MVS_UNDEFER_MATCH_INT,
  TIMER_MVS_GROUP_DISABLED,
  TIMER_MVS_GROUP_ENABLED,
  TIMER_MVS_TIMER_CLEARED,
  TIMER_MVS_TIMER_PAUSED,
  TIMER_MVS_TIMER_RESUMED,
  TIMER_MVS_TIMER_CLEAR_TASK_TIMERS,
  TIMER_MVS_SET_WAKEUP,
  TIMER_MVS_TIMER_UNDEFINED,
}timer_match_interrupt_setter_type;

#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
/* Timer API name enums */
typedef enum
{
  TIMER_API_TIMER_INIT = 0x1,
  TIMER_API_TIMER_DEF,
  TIMER_API_TIMER_DEF_OSAL,
  TIMER_API_TIMER_DEF2,
  TIMER_API_TIMER_SET,
  TIMER_API_TIMER_SET_GET,
  TIMER_API_TIMER_SET_ABSOLUTE,
  TIMER_API_TIMER_GET,
  TIMER_API_TIMER_GET_PARAMS,
  TIMER_API_TIMER_SET_PARAMS,
  TIMER_API_TIMER_CLR,
  TIMER_API_TIMER_STOP,
  TIMER_API_TIMER_UNDEF,
  TIMER_API_TIMER_CLR_TASK_TIMERS,
  TIMER_API_TIMER_CLR_OSAL_THREAD_TIMERS,
  TIMER_API_TIMER_GROUP_ENABLE,
  TIMER_API_TIMER_GROUP_DISABLE,
  TIMER_API_TIMER_GROUP_SET_DEFERRABLE,
  TIMER_API_TIMER_GET_FIRST_NON_DEFERRABLE_TIMER_EXPIRY,
  TIMER_API_TIMER_GET_TIME_TILL_FIRST_NON_DEFERRABLE_TIMER,
  TIMER_API_TIMER_DEFER_MATCH_INTERRUPT,
  TIMER_API_TIMER_UNDEFER_MATCH_INTERRUPT,
  TIMER_API_TIMER_RESTART,
  TIMER_API_TIMER_REG,
  TIMER_API_TIMER_GET_SCLK_TILL_EXPIRY,
  TIMER_API_TIMER_SET_WAKEUP,
  TIMER_API_TIMER_GET_QURT_SIG_REGISTRATION_ID,
  TIMER_API_TIMER_INIT_QURT_SIGNAL,
  TIMER_API_TIMER_REGISTER_FOR_NON_DEFER_EXPIRY,
  TIMER_API_TIMER_UPDATE_ISR_TIMER,
  TIMER_API_TIMER_EXPIRES_AT,
  TIMER_API_TIMER_GET_START,
  TIMER_API_TIMER_GET_GROUP,
  TIMER_API_TIMER_GET_EXPIRY_TYPE,
  TIMER_API_TIMER_PAUSE,
  TIMER_API_TIMER_RESUME,
  TIMER_API_TIMER_IS_ACTIVE,
  TIMER_API_TIMER_IS_PERIODIC,
  TIMER_API_NOTIFY_NON_DEFER_EXPIRY_CHANGE,
  TIMER_API_BIN_LIST_INSERT_BETWEEN,
  TIMER_API_BIN_CREATE_BETWEEN,
  TIMER_API_BIN_FIND_CREATE,
  TIMER_API_BIN_FIND_FIRST_EXCEEDING,
  TIMER_API_BIN_REMOVE,
  TIMER_API_TIMER_INSERT,
  TIMER_API_CONCATENATE_LISTS,
  TIMER_API_FREE_INTERNAL_TIMER,
  TIMER_API_REMOVE,
  TIMER_API_REMOTE_EXPIRE,
  TIMER_API_TIMER_EXPIRE,
  TIMER_API_PROCESS_ACTIVE_TIMERS,
  TIMER_API_TIMER_TASK,
  TIMER_API_CREATE_NEW_GROUP,
  TIMER_API_CREATE_NEW_CHUNK,
  TIMER_API_INIT_CLIENT_TIMERS,
  TIMER_API_GET_CLIENT_TIMER,
  TIMER_API_GET_NEW_CLIENT_TIMER,
  TIMER_API_GET_CLIENT_INT_TIMERS,
  TIMER_API_CLIENT_MEM_LEAK,
  TIMER_API_DEFINE,
  TIMER_API_ACTIVE_LIST_CHECK,
  TIMER_API_REGISTER_FOR_NON_DEFER_EXPIRY,
  TIMER_API_MAX = 0xFFFFFFFF
} timer_api_type;
#endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES */

/*=============================================================================

                           STRUCTURES

=============================================================================*/
/** @brief Timer list structure type. Values in this structure are for private 
use by timer.c only. */
typedef struct timer_list_struct
{
  timer_ptr_internal_type         first; /**< List of timers. */

  timer_ptr_internal_type         last;  /**< End of the timer list. */
}
timer_list_type;

typedef struct timer_group_s
{
  timer_group_ptr                 ext_self;   /**< Points to external timer group 
                                                   user variable */
  timer_list_type                 list;       /**< Timers in the disabled timer 
                                                   group. */
  boolean                         disabled;   /**< Disabled flag for the timer 
                                                   group. */
  boolean                         deferrable; /**< Deferrable flag for the 
                                                   timer group. Default = FALSE. */
  timer_group_t_ptr               next;       /**< Link to the next timer group. */

  uint8                          process_idx;	/**< Process index relating this group 
												 to the associated PD */
  uint8 						 group_idx;  /**< Group index to the internal groups */
} timer_group_t;

/** Timer cache structure type */

typedef struct
{
  /** Original set duration */
  time_timetick_type                   duration;

  /** Converted set duration */
  time_timetick_type                   duration_sclk;

  /** Original reload increment */
  time_timetick_type                   reload;

  /** Converted reload increment */
  time_timetick_type                   reload_sclk;

  /** Time units */
  timer_unit_type                     unit;
}
timer_cache_type;

/* Try to keep less than 32 bits in total */
typedef struct timer_properties_struct
{
  /* Nibble (4bits) wise: 7th: None or free 6th-5th: Group Idx, 4th: slave task idx, 3rd: Timer state,
     2nd: process idx, 1st-3rd bit: remote_process 1st: 2-1 bits: cb type, 1st 0th bit: expiry_type
     0th: Node state, being used or not */
  unsigned int group_idx:8;   /* 8 bits for group index: Limits to 0-255 groups */
  unsigned int slave_task_idx:4;   /* 4 bits for slave task num: 0-15 */
  unsigned int timer_state:4;   /* 4 bits for timer_state: 0-15 */ /* Currently not being used, later if required can use */
  unsigned int process_idx:4;   /* 4 bits for process index: 0-15 */
  unsigned int remote_process:2;   /* 2 bits: Indicates whether timer is from remote process */
  unsigned int callback_type:3;   /* 3 bits*/ /* values 0-7 */
  unsigned int expiry_type:1;   /* 1 bit: Relative(0)/Absolute(1) */
  unsigned int node_state:4;   /* 4 bits: Values are NODE_IS_FREE(0x3), NODE_IS_NOT_FREE(0xC) only */
}timer_properties_type;

/** @brief Timer Client Structure type. Contains client details like
     Group, Signals, callback functions, process details etc
     Values in this structure are for private use by timer.c only.*/
typedef struct timer_client_struct
{
  timer_state_struct_type         timer_state; 
  
  time_timetick_type              timer_state_at; 

  /**< Address of client timer */
  timer_ptr_type                  timer_client_ptr;

  /**< Handle to the internal timer. */
  int32                           timer_int_handle;

   union
   {
      /**< Signals Object */
      void                           *sigs_obj;
      /**< Asynchronous Procedure Call (APC) function to call when the timer 
         expires. */
      timer_t1_cb_type                func1; 
      /**< Clock callback style function to call when the timer expires. */
      timer_t2_cb_type                func2; 
  }cbfn;

  union
  {
     /**< Signal(s) to set in the task when the timer expires. */
     time_osal_notify_data           sigs;  
     /**< Data for the APC function when the timer expires. */
     timer_cb_data_type              data;
  }cbdt;

  timer_properties_type              info;
}timer_client_type;

/** Internal Timer struture referenced by external timer structure through handle */
typedef struct timer_struct_internal
{
  /* Link for the next queue */
  q_link_type                     link;
  
  /* Index of timer_buffer array */
  int32                           index;
  
  /* Backward pointer to client timer */
  timer_client_ptr_type           timer_ext;
  
  time_osal_thread_id             thread_id;

  /** The list the timer is on.  "list" will contain:
       "&active_timers" if the timer is active;
       "&timer->group->list" if the timer is active, but in a disabled group;
       "NULL" if the timer is not active. */
  timer_list_ptr                  list;

  /** Slow clock tick count timer expires at (list != NULL),
     or remaining ticks until expiry if timer is paused (list == NULL) */
  time_timetick_type              expiry;

  /** Reload offset when timer expires (0 = no reload at expiry) */
  time_timetick_type              reload;

  /** Slow clock tick count value when timer was set (started) */
  time_timetick_type              start;

  /** Durations and reload times, original units and converted to sclks */
  timer_cache_type                cache;

  /** Pointer to the next timer in the list (list != NULL) */
  timer_ptr_internal_type         next;

  /** Pointer to the previous timer in the list (list != NULL) */
  timer_ptr_internal_type         prev;

  #ifdef FEATURE_USE_TIMER_BIN
  /** Pointer to the bin for this timer */
  timer_bin_ptr                   bin;
  #endif

  timer_properties_type           info;
}timer_internal_type;

/*-----------------------------------------------------------------------------
  Timer Bin structure type
    Represents a single bin used for fast insertion into the timer list
-----------------------------------------------------------------------------*/
#ifdef FEATURE_USE_TIMER_BIN
typedef struct timer_bin_struct
{
  /* First timer in the bin */
  timer_ptr_internal_type         first;

  /* Last timer in the bin */
  timer_ptr_internal_type         last;

  /* Earliest expiry time in the bin */
  time_timetick_type                   lower;

  /* Latest expiry time in the bin */
  time_timetick_type                   upper;

  /* Number of timers in the bin */
  int                             count;

  /* Next bin pointer */
  timer_bin_ptr                   next_bin;

  /* Previous bin pointer */
  timer_bin_ptr                   prev_bin;
}
timer_bin_type;


/*-----------------------------------------------------------------------------
  Timer bin list type
-----------------------------------------------------------------------------*/

typedef struct timer_bin_list_struct
{
  /* First timer bin in list */
  timer_bin_ptr                   first;

  /* Last timer bin in list */
  timer_bin_ptr                   last;

  /* Number of timer bins */
  int                             count; 
}
timer_bin_list_type, *timer_bin_list_ptr;
#endif

/*-----------------------------------------------------------------------------
  Private timer data ...
-----------------------------------------------------------------------------*/

typedef struct
{
  /* Faux timer group, used to hold list of active timers, as well as a list
     of timer groups.
     Active timers will never "belong" to this group, ie timer->group will
     never be &timers.active, since this would cause them to forget their
     true timer group. */
  timer_group_t                        active;

  /* Slow clock timestamp of last write to SLEEP_XTAL_TIMETICK_MATCH */
  time_timetick_type                   set_time;

  /* Slow clock timestamp of last attempt to write SLEEP_XTAL_TIMETICK_MATCH */
  time_timetick_type                   last_set_time;
  
  /* Next timer expiration value */
  time_timetick_type                   set_value;

  /* Last value written to SLEEP_XTAL_TIMETICK_MATCH */
  time_timetick_type                   match_value;

  /* Slow clock timestamp when timer_isr is called */
  time_timetick_type                   isr_time;

  /* Flag to indicate if timers_process_active_timers() is executing */
  boolean                         processing;

  /* Flag to indicate if timer_isr can be updated */
  boolean                         do_reprogram_isr;

  /* Active timers count */
  uint32                          active_timers_count;
  /* Pause timers count */
  uint32                          paused_timers_count;

#ifdef FEATURE_USE_TIMER_BIN
  /* Active timer bin list */
  timer_bin_list_type             bins;

  /* Free timer bin list */
  timer_bin_list_type             bins_free;
#endif

  /* Pointer to the first non-deferrable timer */
  timer_ptr_internal_type         first_non_deferrable;
  
  /* DAL Device Handle */
  DalDeviceHandle*                dal_int_handle;
}
timers_type;

/*=============================================================================

                           TIMER DEBUG RELATED STRUCTURES

=============================================================================*/

/*-----------------------------------------------------------------------------
  Helper variables for the script
-----------------------------------------------------------------------------*/
/* Version information for the timer module */
typedef struct ats_timer_version_s
{
uint32 peripheral;
uint32 major_ver;
uint32 minor_ver;
} ats_timer_version_type;

/*-----------------------------------------------------------------------------
  Timer Trace Log
-----------------------------------------------------------------------------*/
#ifdef FEATURE_TIMER_TRACE

/* Type of timer events to log (24 bytes) */
typedef enum
{
  TIMER_TRACE_ISR,
  TIMER_TRACE_PROCESS_START,
  TIMER_TRACE_PROCESS_END,
  TIMER_TRACE_MATCH_VALUE_PROG,
} timer_trace_event_type;

/* A single timer event trace */
typedef struct
{
  timer_trace_event_type             event;
  timer_match_interrupt_setter_type  mv_setter;
  time_timetick_type                 mv;
  time_timetick_type                 ts;  

} timer_trace_data_type;

/* Structure of all timer log events */
typedef struct
{
  uint32                index;
  timer_trace_data_type events[MAX_TIMER_TRACE];
} timer_trace_type;

#endif /* FEATURE_TIMER_TRACE */

/*-----------------------------------------------------------------------------
  Timer API Log
-----------------------------------------------------------------------------*/

#ifdef FEATURE_ENABLE_TIMER_DEBUG_VARIABLES
/* 24 bytes */
typedef struct
{
  /* API type that was called */
  timer_api_type            api;
  /* Pointer to TCB */
  time_osal_thread_id		tid;
  /* timer_error_type */
  timer_error_type          ret_val;
  /* Timer Pointer */
  timer_ptr_type            timer;
  /* Timer client Pointer */
  timer_client_ptr_type     clnt_timer;
  /* Internal timer pointer */
  timer_ptr_internal_type   int_timer;  
  /* Time Stamp */
  time_timetick_type        ts;

  /* Enable this feature from scons to have API profiling */
  #ifdef FEATURE_TIMER_API_LOG_PROFILING
  /* Exit timetick */
  time_timetick_type        exit_ts;
  #endif /* FEATURE_TIMER_API_LOG_PROFILING */  

} timer_api_log_type;

/*-----------------------------------------------------------------------------
  Record Match Val
-----------------------------------------------------------------------------*/
/* 40 bytes */
typedef struct timer_match_value_record_s
{
  timer_match_interrupt_setter_type  mv_setter;
  timer_ptr_internal_type            int_timer;  
  time_timetick_type                 mv_supplied;  
  time_timetick_type                 mv_set;
  time_timetick_type                 post_set_ts;  
  time_timetick_type                 ts;
}timer_match_value_record_type;

/*-----------------------------------------------------------------------------
  Timer Set log
-----------------------------------------------------------------------------*/
typedef struct timer_sets_s
{
  timer_ptr_type 			ext_timer;
  timer_ptr_internal_type 	int_timer;
  timer_client_ptr_type     clnt_timer;
  time_timetick_type 		ts;
  time_timetick_type 		expiry;
  time_osal_thread_id		tid;

  #ifdef FEATURE_USE_TIMER_BIN  
  timer_bin_ptr             bin;
  #endif
}timer_set_type;

/*-----------------------------------------------------------------------------
  Timer Clear log
-----------------------------------------------------------------------------*/
typedef struct timer_clr_dbg_s
{
  timer_ptr_type            ext_timer;
  timer_ptr_internal_type   int_timer;
  time_timetick_type        ts;
  timer_error_type          ret_val;
  timer_state_struct_type   timer_state;
  time_osal_thread_id		tid;
}timer_clr_log_type;

/*-----------------------------------------------------------------------------
  Timer Expired Debug log
-----------------------------------------------------------------------------*/
typedef struct timer_expired_debug_struct
{
  uint32                    whileloopcnt;
  timer_ptr_type            ext_timer;
  timer_ptr_internal_type   int_timer;
  timer_client_ptr_type     clnt_timer;
  time_timetick_type        expiry;
  time_timetick_type        start;  
  time_timetick_type        ts;
  void *                    sigs_func;
  uint32                    data_mask;
  boolean                   deferrable;
  timer_properties_type     info;
}timer_expired_debug_type;

/*-----------------------------------------------------------------------------
  Timer remote expiry log
-----------------------------------------------------------------------------*/
typedef struct timer_expired_s
{
  timer_ptr_type          timer_expired;
  time_timetick_type      expiry_value; /* copy the expiry value of the timer */
  time_timetick_type      processing_started;
  time_timetick_type      processing_time;
  void *                  sigs_func_obj;
  timer_properties_type   info;
}timer_expired_type;

/*-----------------------------------------------------------------------------
  Timer Defer Undefer log
-----------------------------------------------------------------------------*/
typedef struct 
{
  timer_match_interrupt_setter_type function_called;
  time_timetick_type ts;
  
  /* ts_aux will either store the entry timetick in case of undefer call,
     * or it will have the return value in case of timer defer function call. */
  time_timetick_type ts_aux;
}timer_defer_undefer_reference_count_log_type;

#endif /* FEATURE_ENABLE_TIMER_DEBUG_VARIABLES */


/*=============================================================================

                           FUNCTION PROTOTYPES

=============================================================================*/
#ifdef FEATURE_USE_TIMER_BIN
void timer_bin_list_insert_between
(
  /* Bin list to insert bin into */
  timer_bin_list_ptr              list, 

  /* Bin to insert in bin list */
  timer_bin_ptr                   bin,

  /* Which node bin should be inserted after */
  timer_bin_ptr                   ptr1,

  /* Which node bin should be inserted before */
  timer_bin_ptr                   ptr2
);

void timer_bin_list_remove
(
  /* Bin list to removed bin from */
  timer_bin_list_ptr              list, 

  /* Bin to remove from bin list */
  timer_bin_ptr                   bin
);

timer_bin_ptr timer_bin_list_pop
(
  /* Bin list to remove the head bin from */
  timer_bin_list_ptr              list
);

void timer_bin_list_push
(
  /* Bin list to push bin onto */
  timer_bin_list_ptr              list, 

  /* Bin to push onto bin list */
  timer_bin_ptr                   bin
);

timer_bin_ptr timer_bin_create_between
(
  /* Bin to create new bin after */
  timer_bin_ptr                   bin1, 

  /* Bin to create new bin before */
  timer_bin_ptr                   bin2,

  /* Timer which will exist in the bin (used to define bin range) */
  timer_ptr_internal_type         timer
) ;

timer_bin_ptr timer_bin_find_create
( 
  /* Timer for which a bin is needed */
  timer_ptr_internal_type        timer
);

timer_ptr_internal_type timer_bin_find_first_exceeding
(
  /* Timer used to determine bin range */
  timer_ptr_internal_type         timer
);

void timer_bin_insert
(
  /* Bin to insert timer in */
  timer_bin_ptr                   bin,

  /* Timer to insert in bin */
  timer_ptr_internal_type         timer 
);

void timer_bin_remove
(
  /* Timer to remove from bin list */
  timer_ptr_internal_type         timer
);

#endif

timer_ptr_internal_type timer_get_internal_timer
(
  timer_client_ptr_type   timer
);

timers_type*  timer_get_active_list(void);
void timer_v_init(void);

/* ===================================================
              INTERNAL APIS
   =================================================== */

time_timetick_type timer_drv_get_sclk_till_expiry_64( void );

void timer_drv_clr_osal_thread_timers
(
  /* Timers belonging to this task will be cleared (stopped). */
  time_osal_thread_id                    thread_id
);

timer_error_type timer_drv_resume
(
  /* Timer to resume */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,

  /* Process Idx */
  uint32                          process_idx
);

timer_error_type timer_drv_pause
(
  /* Timer to pause */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,

  /* process index */
  unsigned int                    process_idx
);

timer_error_type timer_drv_undef
(
  /* Timer to stop */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,

  /* process index */
  unsigned int                    process_idx
);

boolean timer_drv_is_active
(
  timer_ptr_type  timer,
  timer_ptr_type  timer_actual_addr,
  unsigned int    process_idx
);

time_timetick_type timer_drv_defer_match_interrupt_64( void );

time_timetick_type timer_drv_get_first_non_deferrable_timer_expiry(timer_non_defer_data_ptr timer_data_ptr);

timer_error_type timer_drv_group_disable
(
  /* Timer group to be disabled */
  timer_group_ptr                 group
);

timer_error_type timer_drv_group_enable
(
  /* Timer group to be enabled */
  timer_group_ptr                 group
);

timer_error_type timer_drv_get_group
(
  timer_ptr_type  timer,
  timer_ptr_type  timer_actual_addr,
  unsigned int    process_idx,
  timer_group_type *group
);

timer_error_type timer_drv_get_expiry_type
(
  timer_ptr_type  timer,
  timer_ptr_type  timer_actual_addr,
  unsigned int    process_idx,
  timer_expiry_type *expiry_type
);

timer_error_type timer_drv_set_wakeup_64
(
  /* Wakeup the CPU in the given number of sclk's. */
  time_timetick_type                   wakeup_sclks
);

time_timetick_type timer_drv_get_start_64
(
  timer_ptr_type  timer,
  timer_ptr_type  timer_actual_addr,
  /* process index */
  unsigned int    process_idx
);

time_timetick_type  timer_drv_expires_at_64
(
  timer_ptr_type  timer,
  timer_ptr_type  timer_actual_addr,  
  unsigned int    process_idx
);

timer_error_type timer_drv_def2
(
  /* Timer to set */
  timer_ptr_type                  timer,  

  /* Timer's Actuall Address in its own Process Domain, for Logging purposes only */
  timer_ptr_type                  timer_actual_addr,

  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group,
  
  /* Group timer will belong to (NULL=the "NULL" timer group) */
  timer_group_ptr                 group_actual_addr,

  /* Process Index */
  uint32                          process_idx
);

timer_error_type timer_drv_group_set_deferrable( timer_group_ptr group, 
                                                 timer_group_ptr group_actual_addr, /* For debug purposes only */
                                                 boolean condition,
                                                 uint32 process_idx);

timer_error_type timer_drv_is_periodic_timer
(
  timer_ptr_type      timer,
  timer_ptr_type      timer_actual_addr,
  unsigned int        process_idx,
  boolean            *is_periodic,
  time_timetick_type *period
);

timer_error_type timer_drv_clr_64
(
  /* Timer to stop */
  timer_ptr_type                  timer,

  /* Timer's actual address in its own Process Domain, for logging purposes only */
  timer_ptr_type                  timer_actual_addr,  

  /* Unit to measure "ticks before expiry" in.  Use T_NONE if not required */
  timer_unit_type                     unit,
  /* process index */
  unsigned int                    process_idx,
  
  time_timetick_type             *rem_time
);

timer_error_type timer_drv_register_for_non_defer_expiry_change
(
  timer_notify_type 		notify_type,
  time_osal_notify_obj_ptr 	notify_obj,
  time_osal_notify_data 	notify_mask,
  uint32 					process_idx
);

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
);

void timer_drv_restart(void);

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
);

void timer_drv_undefer_match_interrupt( void );

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
);

timer_error_type timer_drv_def_osal
(
  /* Timer to set */
  timer_ptr_type                  timer,
  
  /* Timer's Actual Address in its own Process Domain, for Logging purposes only */
  timer_ptr_type                  timer_actual_addr,

  /* Group timer will belong to (NULL = the "NULL" timer group) */
  timer_group_ptr                 group,

  /* Group actual addr. For Debugging purposes only */
  timer_group_ptr                 group_actual_addr,
  
  /*signal type*/
  timer_notify_type               cb_type,

  /* Task to signal and/or task to queue APC call for */
  time_osal_notify_obj_ptr        sigs_func_addr,

  /* Task signals to set to the tcb task when timer expires */
  time_osal_notify_data           sigs_mask_data,

  /* Process Index */
  unsigned int                    process_idx
);

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
);

/* Internal Function */
timer_error_type timer_drv_delete_process_timers(uint32 process_idx);
timer_error_type timer_drv_delete_process_groups(uint32 process_idx);

/*=============================================================================
FUNCTION TIMER_SET_REMOTE_PROCESS_CB_HANDLER

Sets the API that needs to be called when a remote process timer expires.
We will provide to this callback function, the timer callback details
=============================================================================*/
void timer_drv_set_remote_process_cb_handler
(
   timer_process_cb_handler shim_cb_func
);

/*=============================================================================
FUNCTION TIMER_DRV_SET_LOCAL_PROCESS_IDX

Local Process Index needs to be set by timer shim layer so that remote timers
can be differentiated from local process timers
=============================================================================*/
void timer_drv_set_local_process_idx
(
   uint32 local_process_idx
);

#endif /* TIMER_V_H */
