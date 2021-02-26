#ifndef PROFPMON_H
#define PROFPMON_H
/*===========================================================================

DESCRIPTION
  Definitions of DIAG subsystem packets for configuring task profile,
  memory debug, PC profile, and code coverage 
 
Copyright (c) 2002-2006 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE
$Header: //components/rel/core.qdsp6/2.1/services/prof/inc/prof_pmon.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/07/02   jct     Integrated onto target
07/20/02    gr     Created

===========================================================================*/

/* ------------------------------------------------------------------------
** Constants
** ------------------------------------------------------------------------ */
#define QURT_ELITE_MAX_SW_THREADS 64
/** @ingroup core_event_profiling_2
  @brief Per-software-thread profiling information that is part of the
  acs_event_periodic_profiling_t and acs_event_periodic_profiling_3_t
  payload structures.
*/
struct acs_sw_thread_info
{
    uint32	thread_id;
      /**< Unique identifier of a software thread. The ID comprises the first
           four ASCII characters of the thread name. It can be formatted as a
           string and displayed after appending the EOS character. */

    uint32	run_cycles;
      /**< Number of cycles this thread has been actively running on a hardware
           thread since the last measurement event. */

    uint16	stack_size;
      /**< Allocation size in bytes of this thread stack. */

    uint16	stack_free;
      /**< Estimated stack amount that has not been touched since thread
           inception.
           @newpage */

};

/** @ingroup core_event_profiling_3
  @brief Profiling payload of the #ADSP_EVENT_PERIODIC_PROFILING_3 event,
  which provides a periodic update of profiling statistics from the aDSP. The
  payload contains one instance of this structure, followed by
  profiled_sw_threads instances of acs_sw_thread_info.
  */
struct acs_event_periodic_profiling_3_t
{

    uint32	core_clock_freq;
    /**< Configured frequency of the aDSP core clock, in kHz. */

    uint32	bus_clock_freq;
    /**< Configured frequency of the aDSP AHB bus clock in kHz. */
      
    uint32	wall_clock_ticks;
    /**< Number of ticks from the previous event. This is used to estimate the
         duration since the previous measurement event. Each tick is a cycle of
         the 32768 Hz sleep timer. */

    uint32	proc_cycles;
    /**< Number of processor cycles that occurred since the previous
         measurement event. */

    uint32	current_heap_use;
    /**< Current number of bytes allocated from the heap. */

    uint32	peak_heap_use;
    /**< Peak heap consumption in bytes since bootup or the last
         #ADSP_CMD_RESET_PEAK_HEAP_USE. */

    uint32	num_sw_threads;
    /**< Number of software threads in the aDSP. */

    uint32	tot_num_hw_threads;
    /**< Number of hardware threads on the existing platform.

         Supported values: 1 to 4 */

    uint32	hw_idle_cycles[4];
    /**< Per hardware thread, the number of idle cycles since the last
         measurement event. The array size of 4 accommodates all anticipated
         platforms to be supported */

    uint32	profiled_sw_threads;
    /**< Indicates the number of profiled software threads.

         Supported values:
         - 0 -- Software thread profiling is not present
         - > 0 -- Number of active software threads in the aDSP

         The payload for this event comprises one instance of
           acs_event_periodic_profiling_3_t, followed immediately by
         profiled_sw_threads number of instances of acs_sw_thread_info. */

    uint32	pmu_event_counters[4];
    /**< Counts the occurrence of configured PMU events.
         @newpage */
      
    uint32	pmu_mode;
    /**< Specifies the events that are measured in PMU mode in a given
         profiling period.
      
         Supported values:
         - 0 -- AXI BW
         - 1 -- AHB BW
         - 2 -- L1 cache events, MPPS
         - 3 -- L2 cache events */
      
    uint32	axi_bw_vote;
    /**< AXI bandwidth vote in bytes per second (refer to @xhyperref{Q6,[Q6]}).
         @newpage */

    /* NOTE: Q6 = Hexagon Multimedia: Elite Firmware Architecture Description */ 

};

/** @brief Log packet strucure of ACS performance monitor when enabled 
through QXDM. ADSP will send this info for every event.
*/
typedef PACK(struct )
{
  uint32	  version_info;
      /**< Version information for synchronization */  
      /**< between parser utility and event payload*/  
  uint32   profile_duration;
      /**< profiling duration(usec) between two    */
      /**< events                                  */
  struct acs_event_periodic_profiling_3_t event_payload;
      /**< profiler event payload                  */  
  struct acs_sw_thread_info sw_threads_info[QURT_ELITE_MAX_SW_THREADS];
      /**< informtion of profiled SW threads       */    
      /**< space allocated for maximum sw threads  */         
} acs_event_periodic_profiling_log_t;


/** @brief log record define for audio log driver  */
LOG_RECORD_DEFINE(LOG_AUDVOC_DATA_PATHS_PACKET_C)
  acs_event_periodic_profiling_log_t      log_header;
LOG_RECORD_END


/*============================================================================

PACKET   BLAST Performance Monitoring information

ID       LOG_BLAST_PM_C

DESCRIPTION
  Contains performance information collected from QDSP6 for the requested
  events.

  ver:        Version no. to handle future changes. For this format
              ver = 1

  app:        Field to distinguish multiple BLAST systems on the same
              target.
              0 - Modem SW
              1 - Modem FW
              2 - Multi Media

  rsvd:       2 bytes reseved for future use.

  counters[x]: Performance information collected from QDSP6 corresponding to
              the requested events.

  total_samples: The total number of core cycles elapsed so far.

  total_sclk_samples: The total number of sleep clock ticks elapsed so far.
              Sleep clock runs at fixed frequency of 32768 Hz.

  idle_samples[x]: A measure of the HW thread time used for running tasks.
              This field is set to how many pcycles each HW thread was
              sleeping.

	
  flags:      Same as LOG_QURT_TASKPROFILE_C
============================================================================*/
LOG_RECORD_DEFINE(LOG_BLAST_PM_C)

   uint8     ver;
   uint8     app;
   uint8     rsvd[2];
   uint32    counters[4];
   uint32    total_sclk_samples;
   uint64    total_samples;
   uint64    idle_samples[6];
   
LOG_RECORD_END


LOG_RECORD_DEFINE(LOG_QURT_PM_C)

   uint8     ver; 
   uint8     app;
   uint8     hw_thread_num;
   uint8     rsvd;
   uint8     events[4]; //4
   uint32    counters[4];
   uint32    timer_freq;
   uint32    core_freq;
   uint64    timer_ticks;
   uint64    core_pcycles;
   uint64    idle_pcycles[6];
   
LOG_RECORD_END

#endif /* PROFPMON_H */