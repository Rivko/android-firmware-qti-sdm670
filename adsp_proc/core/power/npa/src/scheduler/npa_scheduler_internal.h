/*==============================================================================
@file npa_scheduler_internal.h

NPA Scheduled Request Function/Structure Declarations

Copyright (c) 2009-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

$Header: //components/rel/core.qdsp6/2.1/power/npa/src/scheduler/npa_scheduler_internal.h#1 $
============================================================================*/
#ifndef NPA_SCHEDULER_INTERNAL_H
#define NPA_SCHEDULER_INTERNAL_H

#include "npa_scheduler.h"
#include "npa_internal.h"
#include "npa_scheduler_config.h"
#include "npa_inlines.h"
#include "timer.h"
#include "sleep_lpr.h"

/*----------------------------------------------------------------------------
 * Enumerations/Constants
 * -------------------------------------------------------------------------*/
/* flag to turn on and off sleep lpr support */
extern int npa_sleep_lpr_supported;

/* NPA Schedulers Log name */
#define NPA_SCHEDULER_DEFAULT_LOG_NAME "NPA Scheduler Log"

/* Sleep LPR Resource name */
#define NPA_SCHEDULER_SLEEP_LPR_RESOURCE_NAME "/sleep/lpr"

/* Sleep's /core/cpu/wakeup Resource Name */
#define NPA_SCHEDULER_SLEEP_CORE_CPU_WAKEUP_RESOURCE_NAME "/core/cpu/wakeup"

/* Sleep's /core/cpu/wakeup Resource Name */
#define NPA_SCHEDULER_SLEEP_MAX_DURATION_RESOURCE_NAME "/sleep/max_duration"

/* Scheduled Request Client State */
typedef enum {
  NPA_SCHEDULED_CLIENT_INACTIVE = 0,/* No activity planned. Idle state. 
                                     * Only client can touch in this state
                                     */
  NPA_SCHEDULED_CLIENT_EXPECTING,  /* Client has been put into Scheduled
                                    * mode and is expecting an "issue".
                                    * Only client can touch.
                                    */
  NPA_SCHEDULED_CLIENT_LINKED,     /* In the link list. Must use lock
                                    * to change the link
                                    */
  NPA_SCHEDULED_CLIENT_UNLINKED,   /* Not in the link list.  Thread
                                    * which put in this state is only
                                    * one that can make changes
                                    */
  NPA_SCHEDULED_CLIENT_FORKING,    /* Request is being forked by the
                                    * sleep enter routine. It will be
                                    * put back in the list as LINKED 
                                    * to have the
                                    * notify occur on the timeout.
                                    */
  NPA_SCHEDULED_CLIENT_NOTIFY_EXPECTED, /* Request is forked and awaiting a notification.
                                         * It will be moved to INACTIVE
                                         * after the fork is complete.
                                         */
  NPA_SCHEDULED_CLIENT_IN_PROGRESS,/* being executed, only scheduler
                                    * can touch in this state
                                    */
  NPA_SCHEDULED_CLIENT_UNDEFINED   /* Unknown state */
} npa_scheduled_client_state;


/* Implied Complete State */
typedef enum {
  NPA_NO_IMPLIED_COMPLETE = 0,      /* There is no implied complete pending
                                     * or in progress
                                     */
  NPA_IMPLIED_COMPLETE_PENDING,     /* There is an Implied Complete pending
                                     * but it has not been started yet
                                     */
  NPA_IMPLIED_COMPLETE_IN_PROGRESS, /* There is an Implied Complete started
                                     * and waiting for completion.  This
                                     * must be finished before any other
                                     * requests (unschedule, sync, issue)
                                     * can happen.
                                     */
  NPA_IMPLIED_COMPLETE_EXECUTED     /* Implied complete was executed but
                                     * if the requests is ever issued again
                                     * and then has to be backed out (e.g.
                                     * two rude wakeups), then need be able
                                     * to back it out too.
                                     */
} npa_implied_complete_state;


/* npa_scheduler_next_timeout_states - defines whether npa scheduler
 * can setup a timeout or poke the scheduler if something is soon.
 * Only the scheduler can move to a lower state.
 * NTS_OPEN - Not running in scheduler task and before the first poke.
 *            Only the scheduler can set this state.
 * NTS_NO_POKE - Not running in teh scheduler task but a poke has occurred.
 *               Only setable by a non-scheduler task (i.e. not already in 
 *               MTS_NO_SETUP)
 * NTS_NO_SETUP - Running in the scheduler task but before it calculates
 *                the next timeout so no need to poke or setup a new time.
 *                Only the scheduler can set this state.
 * MUST BE KEPT TO 4 bits in size else change size of holding variable
 * in npa_scheduler.
 */
typedef enum {
  NPA_SCHEDULER_NTS_OPEN = 0,     /* ok to either schedule or poke */
  NPA_SCHDEDULER_NTS_NO_POKE = 1, /* no poking but ok to setup next timeout */
  NPA_SCHEDULER_NTS_NO_SETUP = 2, /* no poking or setting up next timeout. */
  NPA_SCHDULER_NTS_LAST = 0xF     /* last possible number */
} npa_scheduler_next_timeout_states;


/* Actions to the SCHEDULER task. 
 * Note events that can be sent must line up with indexes
 * in npa_scheduler.events
 */
typedef enum npa_scheduler_action_type
{
  /* Events must start with 0 and end before UNDEFINED and all be defined */
  NPA_SCHEDULED_ACTION_TIMEOUT = 0,    /* Timeout event has occurred */
  NPA_SCHEDULED_ACTION_TRIGGER = 1,    /* Run the scheduled events */
  NPA_SCHEDULER_ACTION_LPR_WAKEUP = 2, /* LPR Wakeup has occurred */
  NPA_SCHEDULED_ACTION_IMPLIED_COMPLETE = 3, /* Impplied complete is needed */
  NPA_SCHEDULED_ACTION_UNDEFINED = 4   /* Must be last */
} npa_scheduler_action_type;

/* Scheduler queue struct to store and pass around queues such as the
 * request queue, the forked queue, and the complete queue in the 
 * npa_scheduler_control structure. 
 */
typedef struct
{
  struct npa_client *head;
  struct npa_client *tail;
  unsigned int num_linked_clients;
  unsigned int high_watermark;
  CoreMutex *lock;
  unsigned int not_sorted;  /* non-zero : list is not sorted, insert at tail */
  uint64 sum_wake_work; /* time of postsleep needed */
} npa_scheduler_queue_struct;


/* main control information for the Scheduler */
typedef struct npa_scheduler_control
{
  const struct npa_scheduler_config *config_ptr; /* Target specific config data */

  /* NPA Scheduler specific client info pool */
  CorePoolType                data_pool;

  int thread_id; /* NPA Scheduler thread id */

  /* Timer interface */
  struct
  {
    uint64 current_timeout;
    uint64 timedout_at; /* time of last timeout_callback */
    timer_type timer;
    timer_group_type group;
    unsigned int deferred_match_count; /* times called
                                        * timer_defer_match_interrupt
                                        * minus times called
                                        * timer_undefer_match_interrupt
                                        */
  } timer;

  /* /clk/cpu request interface */
  npa_client_handle clk_cpu_client;

  /* Client handle to the "/protocol/rpm/polling_mode" resource */
  npa_client_handle rpm_polling_mode_client;

  /* core/cpu/busy query interface */
  npa_query_handle core_cpu_busy_query_handle;

  /* Fields used to execute implied_completes when deep sleep is delayed */

  /* core/cpu/busy event handle */
  npa_event_handle core_cpu_busy_event_handle;

  /* List of handles whose implied completes were deferred assuming
     deep sleep to be imminent */
  npa_list_header implied_completes_planB_list;
  
  /* Flags */
  unsigned timer_up;           /* !0 if timer subsystem is up */

  unsigned next_timeout_state; /* Use npa_scheduler_next_timeout_state */

  uint64 next_scheduler_time;  /* next NPA Scheduler task run time
                                * i.e. time scheduler must be running
                                */

  /* Sleep interface */
  struct
  {
    unsigned enter;                   /* !0 if in sleep LPR Enter function */
    unsigned lpr_state_on;            /* Set when Sleep LPR is available.
                                       * 0 if LPR off, 1 if on 
                                       */
    npa_client_handle lpr_client;     /* LPR client handle */
    npa_query_handle  cpu_wakeup_query_handle; /* Wakeup query handle */
    npa_query_handle  max_duration_query_handle; /* Max Duration query handle */
    uint64 duration;  /* duration pass by sleep */
    uint64 wakeup_time_ticks; /* calculated end sleep time */
    uint64 last_sleep_deadline; /* Last value sent to sleep */
  } sleep;
  
  /* Active Scheduled Requests Client list */
  npa_scheduler_queue_struct request_queue;

  /* Active Forked Scheduled Requests Client list - Unsorted */
  npa_scheduler_queue_struct forked_queue;

  /* Active/Pending Complete Request list
   * Used to handle any scheduled or implied completes
   */
  npa_scheduler_queue_struct complete_queue;
  
  /* Pending Implied Completes stack to do now
   * Used to control which requests have pending completes do now.
   * Only insert at the head.
   */
  npa_list_header implied_completes_now_list;
  
  /* Pending Implied Completes to execute on wakeup, rather than from within
     LPR enter */
  npa_list_header implied_completes_on_wakeup_list;

  /* Next awake callbacks registered with the scheduler */
  npa_list_header next_awake_callbacks;

  /* special lock when doing any work the scheduler
   * usually does or coordination on scheduler lists.
   * This currently includes unschedule/syncs/notify and destroys.
   */
  CoreMutex     *scheduler_work_lock;

  /* Scheduler log */
  ULogHandle     log;

  /* List of resources with latency overrides */
  npa_list_header overrides_list;

  /* NPA Scheduler Task Events
   * Index must match up with order defined in npa_scheduler_action_type.
   * The entry is used to send a DALSYS_Event to the NPA Scheduler thread
   * which enters npaScheduler_task() and has the knowledge of what message
   * calls which routines.
   */
  DALSYSEventHandle events_array[NPA_SCHEDULED_ACTION_UNDEFINED];

  struct
  {
    uint64 timer; /* dynamic adjustment used for timer related events */
    uint64 lpr;   /* dynamic adjustment used for lpr wakeup related events */
  } adjust;

  /* overall Scheduler statistics*/
  struct
  {
    unsigned long num_run_scheduler_requests; /* number of time
                                               * npa_run_scheduler_requests
                                               * has been called.
                                               */
    uint64 last_run_scheduler_request_time; /* last time run_scheduler_request
                                             * is called 
                                             */
    unsigned long sched_requests_handled; /* number times called 
                                           * npa_run_scheduled_request() 
                                           */
    unsigned long lpr_enter;  /* # called npa_scheduler_lpr_enter() */
    unsigned long lpr_exit;   /* # called npa_scheduler_lpr_exit()  */
    unsigned long forked;     /* # of times requests were forked. */
    unsigned long fork_short_circuits; /* number of times forked request 
                                        * was same as previous so
                                        * short circuited the request.
                                        */
    unsigned long on_time;    /* on time requests */
    unsigned long late;       /* late requests */
    unsigned long implied_completes; /* # implied completes ran */
    uint64      saved_request_work; /* work saved by forking in sleep as
                                     * opposed to handling request directly.
                                     */
    unsigned long forced_notifies; /* number of times a notify was forced from
                                    * inside an unschedule_client
                                    */

    unsigned long soft_wakes_used;      /* number of times the LPR chose the
                                         * soft wake time.
                                         */

    npa_duration run_requests_duration; /* duration of
                                         * npa_run_scheduler_requests in TICKS
                                         */

    npa_duration run_requests_late_duration; /* time npa_run_scheduler_requests
                                        * should have run and actually did run
                                        * late
                                        */

    npa_duration run_requests_early_duration; /* time npa_run_scheduler_requests
                                        * should have run and actually did run
                                        * early.
                                        */

    npa_duration run_requests_very_early_duration;/* time npa_run_scheduler_requests
                                          * should have run and actually did
                                          * run more than 1000 ticks early
                                          */

    npa_duration timeout_to_scheduler_duration; /* time between timer firing
                                                 * and NPA Scheduler task
                                                 * running.
                                                 */

    npa_duration lpr_entry_duration; /* duration of
                                      * npa_lpr_exit in TICKS
                                      */

    npa_duration lpr_exit_duration; /* duration of
                                     * npa_lpr_exit in TICKS
                                     */
    npa_duration timeout_callback_early; /* early Timer callback
                                          * vs current start time
                                          */
    npa_duration timeout_callback_late;  /* Timer callback
                                          * vs current start time
                                          */
    npa_duration nas_timer_adjustment; /* adjustment to sleep wake time based
                                        * upon timer needing to fire before request
                                        * finish time is done.
                                        */
  } stats;

  uint32 clk_cpu_floor; /* Q6 clk floor request for scheduler operations;
                         * value from config or derived dynamically
                         */

} npa_scheduler_control;

/* do this so we can do some white box testing */
extern npa_scheduler_control npa_scheduler;

/* Internal Scheduled Request structure */
typedef struct npa_scheduler_data
{
  /* Schedule Resource definitions */
  struct npa_client       *prev, *next;   /* Scheduled Linked list */
  npa_scheduled_client_state state;
  npa_implied_complete_state implied_state; /* Implied Complete state */
  uint64                request_time;   /* time request must be done */
  uint64                start_time;     /* time expected to start client */
  npa_resource_state      late_probability;
  npa_scheduler_callback  notification;   /* callback when done */
  npa_scheduled_notification_state notify_state; /* latest notify state */
  void *context;                          /* Callback Data */
  /* original issue_request that scheduler should call */
  void (*issue_request)( npa_client_handle client, int new_request );
  int new_request;  /* new_request to pass to issue_request above */
  npa_scheduler_queue_struct *my_queue; /* which queue this structure is on
                                         * else NULL.  Used by
                                         * npa_schduler_remove() to know which
                                         * queue the client is on.
                                         */
  /* Active latency numbers at time of last issue req.
   * Takes the default and then let's a resource query
   * override the value.
   */
  struct
  {
    uint32     request; /* default or NPA_QUERY_RESOURCE_REQUEST_LATENCY */
    uint32     fork;    /* default or NPA_QUERY_FORK_REQUEST_LATENCY */
    uint32     notify;  /* default or NPA_QUERY_NOTIFY_REQUEST_LATENCY */
    uint32     wake;    /* time needed post wake */
    uint32     request_adjustment; /* runtime determined additional latency */
  } active_latency;

  struct            /* stats for this client */
  {
    unsigned long on_time; /* times request was not late */
    unsigned long late;    /* times request was late */
    unsigned long long late_by_sum; /* sum of time between when late request 
                                      * finished and when it was requested.
                                     */
    unsigned long resource_handle_scheduling; /* resource doing scheduling */
    uint64   last_issue_request_time; /* last request was issued @ time */
    uint64   last_issue_fork_time; /* last request was forked @ time */
    uint64   last_notification_time;  /* last notification call @ time */
    uint64   last_implied_complete_time;  /* last implied complete call @ time */
    unsigned long forked;  /* times requests were forked by LPR */
    uint64        fork_short_circuits; /* times forked request was same so
                                        * short circuited the issue.
                                        */
    uint64        saved_request_work; /* work saved by forking in sleep as
                                       * opposed to handling request directly.
                                       */
    npa_duration fork_issue_request_duration; /* duration running issue request */
    npa_duration notify_issue_request_duration;
    npa_duration full_issue_request_duration;
  } stats;
} npa_scheduler_data;

/* Single struct for holding all the workloop queued data */
typedef struct npa_scheduler_action
{
  /* pointer to the function that should handle this action */
  void (*handler_fcn)(struct npa_scheduler_action *action);
  npa_scheduler_action_type  type; /* type of action */
  void *data; /* data to pass */
} npa_scheduler_action;

/* Next awake callbacks */
typedef struct npa_next_awake_callback
{
  npa_simple_callback func;
  void               *data;
} npa_next_awake_callback;

#define next_awake_callback_pool list_pool
CORE_STATIC_VERIFY( sizeof( npa_next_awake_callback ) <= sizeof( npa_list_elem ) );


/*======================================================================
  Internal Client API

  Note: The internal client type is created/destroyed via the standard
  client api. 
  ======================================================================*/

#ifndef ABS
#define  ABS( x ) ( ((x) < 0) ? -(x) : (x) )
#endif /* ABS */

/**
 * <!-- NPA_NEEDS_NOTIFY -->
 *
 * @brief Check to see if the client's resource has a notify request pending.
 *
 * @todo All requests are assumed to run to completion. Need to look up if a
 * request is actually forked and ready to be notified.
 */
#define NPA_NEEDS_NOTIFY(client) 0


/**
 * <!-- npa_init_scheduler -->
 *
 * @brief Initializes the data structures needed for Scheduled Requests to work.  
 */
void npa_init_scheduler(void);


/**
 * <!-- npa_destroy_scheduable_client -->
 *
 * @brief Delete a Schedulable client
 *
 * Unschedule any pending requests and removed/free schedulable request
 * data structures from the client.
 *
 * @param client : pointer to client.
 */
void npa_destroy_schedulable_client(npa_client_handle client);


/**
 * <!-- npa_scheduler_status_name -->
 * 
 * @brief Status to a string request
 *
 * Returns a pointer to a constant string of the text for the
 * npa_scheduled_notification_state.
 *
 * @param state : requested resource state
 *
 * @return Returns string ptr of the name.
 */
const char *npa_scheduler_status_name(
                   npa_scheduled_notification_state state);


/**
 * <!-- npa_scheduler_walk_list -->
 * 
 * @brief Debug check for a valid scheduled client list. 
 *
 * Walks through the scheduled client lists to verify that
 * it is consistent.  It makes sure all are in the state
 * of NPA_SCHEDULED_CLIENT_LINKED and that the count 
 * matches the links in the list.
 *
 * This is a blocking routine on each list.
 *  
 * @return Returns number of detected errors in all lists.
 */
uint32 npa_scheduler_walk_list(void);


/**
 * <!-- awake_until_init -->
 *
 * @brief Initialize the awake_until_node and busy_node and define the 
 * awake_until marker.
 *
 * Creates the needed resources, nodes, and data structures
 * to support the awake_until feature.
 */
void awake_until_init(void);


/**
 * <!-- npa_run_scheduler_forks -->
 *
 * @brief Run pending presleep fork requests
 *
 * Takes the current pending sorted scheduled request list and runs
 * any of the the requests that are forkable reasonably close
 * to the wake time.
 *
 * It will calculate a new timeout but only notify sleep on the time
 * but not change the timer's timout.
 *
 * THIS MUST BE RAN ONLY IN THE SLEEP THREAD.
 * It is assumed that it is not preemptable.
 *
 * @param wake_time : expected sleep exit time.
 */
void npa_run_scheduler_forks(uint64 wake_time);


/**
 * <!-- npa_scheduler_lpr_init --> 
 *
 * @brief Init function for the Scheduler LPR. 
 * 
 * Initialize the Scheduler LPR and then enables itself.
 *
*/
void npa_scheduler_lpr_init( void );


/**
 * <!-- npa_scheduler_status_name -->
 * 
 * @brief Status to a string request
 *
 * Returns a pointer to a constant string of the text for the
 * npa_scheduled_notification_state.
 *  
 * @param state : npa_scheduled_notification_state value
 * @param state : requested resource state
 *
 * @return Returns string ptr of the name.
 */
const char *npa_scheduler_status_name(
                   npa_scheduled_notification_state state);


/**
 * <!-- npa_scheduler_clear_client_stats -->
 * 
 * @brief Clears a clients statistics area.
 *
 * @param client : Client handle for the clear
 */
void npa_scheduler_clear_client_stats(npa_client_handle client);


/**
 * <!-- npa_scheduler_clear_control_stats-->
 * 
 * @brief Clears the request scheduler's statistics area.
 */
void npa_scheduler_clear_control_stats(void);


/**
 * <!-- npa_scheduler_signal_task -->
 *
 * @brief Send signal to the NPA Scheduler task
 *
 * @param sig : Signal to send to the NPA Scheduler task.
 */
void npa_scheduler_signal_task(npa_scheduler_action_type sig);


/**
 * <!-- npa_run_scheduler_requests -->
 *
 * @brief Run pending requests
 *
 * Takes the current pending sorted scheduled request list and runs
 * the requests quest starting at the head.  It will run until the list
 * is either empty or a request is npa_max_reschedule_delta in the future.
 *
 * It will calculate a new timeout and setup another timer
 * when it has reached either point.
 *
 * @param action : Information about signal causing the function to be called
 */
void npa_run_scheduler_requests(npa_scheduler_action *action);


/**
 * <!-- npa_schedule_timeout_callback -->
 *
 * @brief Handles Timer Callbacks
 * 
 * Triggers the Scheduler workloop with an action to process the 
 * schedule requests from the timer thread.
 *
 * @param data : Data passed by timer.  Defined when calling timer_def().
 */
void npa_scheduler_timeout_callback(timer_cb_data_type data);


/******************************************************************
  @brief npa_schedule_timeout_callback - Handles Timer Callbacks
  
  Triggers the Scheduler workloop with an action to process the 
  schedule requests from the timer thread.

  Inputs:
  @param data : Data passed by timer.  Defined when calling
                timer_def().
*/
void npa_scheduler_planBtimer_callback(timer_cb_data_type data);


/**
 * <!-- npa_scheduler_disable_timer -->
 *
 * @brief Cancel the Scheduler timer and notify sleep lpr of the change
 *
 * @note Should always be called in a thread safe environment.
 */
void npa_scheduler_disable_timer(void);


/**
 * <!-- npa_scheduler_handle_implied_complete -->
 *
 * @brief Issue the Implied Complete if needed
 *
 * Checks to see if an implied complete is pending on the client
 * and issues it if the system is busy
 *
 * @note This should only be run in the NPA Scheduler tread.
 *  
 * @param action : handle to action describing. (Not used)
 */
void npa_scheduler_handle_implied_complete(npa_scheduler_action *action);


/**
 * <!-- npa_scheduler_init_rctask -->
 *
 * @brief Initialize NPA Scheduler task
 *
 * Usually called by RCINIT or other startup initialization task.
 */
void npa_scheduler_init_rctask( void );


/**
 * <!-- npaScheduler task -->
 *
 * @brief Wait on the npa_scheduler.events_array and process npa_scheduler_action_types
 *
 * @param unused_param: Parameter received from Main Control task - ignored
 */
void npaScheduler_task(unsigned long int unused_param);


/**
 * <!-- npa_get_resource_override_latency -->
 * 
 * @brief Search for a resource name in npa_scheduler.overrides
 *
 * @param name : Name of resource to search for
 *
 * @return pointer to latencies if they exist else returns NULL
 */
npa_resource_latency_data 
  *npa_get_resource_override_latency(npa_list_header *list,
                                     const char *name);

#endif /* NPA_SCHEDULER_INTERNAL_H */
