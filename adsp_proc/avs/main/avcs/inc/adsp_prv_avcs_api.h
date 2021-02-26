/*========================================================================
adsp_privatedefs.h

This file contains private ADSP APIs

Copyright (c) 2010 Qualcomm Technologies, Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/avs.adsp/2.8.5/avcs/inc/adsp_prv_avcs_api.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
01/02/15   RB      Created

========================================================================== */
/**
@file adsp_privatedefs_ext.h

@brief This file contains private ADSP APIs
 */

#ifndef _ADSP_PRV_AVCS_API_H_
#define _ADSP_PRV_AVCS_API_H_

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

typedef struct avcs_sw_thread_info_t avcs_sw_thread_info_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_sw_thread_info
@{ */
/* Per-software-thread profiling information that is part of the
  AVCS_EVENT_PERIODIC_PROFILING payload.
 */
struct avcs_sw_thread_info_t
{

	uint32_t              thread_id;
	/**< Unique identifier of a software thread.

           The ID comprises the first four ASCII characters of the thread name.
           It can be formatted as a string and displayed after appending the
           EOS character. */

	uint32_t              run_cycles;
	/**< Number of cycles this thread has been actively running on a hardware
           thread since the last measurement event. */

	uint32_t              stack_size;
	/**< Allocation size in bytes of this thread stack. */

	uint32_t              stack_free;
	/**< Estimated stack amount that has not been touched since thread
           inception. */

    uint32_t          curr_heap_usage;
   /**< Current Heap Utilized in bytes */

   uint32_t           max_heap_usage;
   /**< Maximum Heap Utilization in bytes */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_sw_thread_info */
/**
  Command to obtain common profiling info from the DSP.

  @apr_hdr_fields
    Opcode -- AVCS_CMDRSP_GET_COMMON_PROF_INFO

  @detdesc

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
 */
#define AVCS_CMD_GET_COMMON_PROF_INFO   0x0001291D

/** @endcond */

/**
    Returns a common profiling info for
    #AVCS_CMD_GET_COMMON_PROF_INFO command.

  @return
    None.

  @dependencies
  AVCS_CMD_GET_COMMON_PROF_INFO issued.
 */
#define AVCS_CMDRSP_GET_COMMON_PROF_INFO                           0x0001291E

typedef struct avcs_cmdrsp_get_common_prof_info_t avcs_cmdrsp_get_common_prof_info_t;

#include "adsp_begin_pack.h"

struct avcs_cmdrsp_get_common_prof_info_t
{
	uint32_t                  status;
	/**< Status message (error code).

             @values Refer to @xhyperref{Q5,[Q5]} */

	uint32_t              q6_arch_rev;
	/**< Architecture version of Q6. Value depends on REV hardware register (see Q6 system architecture document) */

	uint32_t              tot_num_hw_threads;
	/**< Number of hardware threads on the existing platform.

         @values 1 to 6 */
}
#include "adsp_end_pack.h"
;

/** @addtogroup core_cmd_profiling
@{ */
/**
  Requests that periodic profiling be provided at regular intervals.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS

  @msg_payload{avcs_cmd_set_periodic_profiling_events_t}
    @table{weak__avcs__cmd__get__periodic__profiling__events}

  @detdesc
    At requested intervals, the AVS writes profiling information to the
    provided memory location and sends the #AVCS_EVENT_PERIODIC_PROFILING
    event.
    @par
    The physical_address field is automatically mapped in the aDSP before
    profiling starts, and is unmapped after profiling is complete. There is no
    need to send memory map commands before this command.
    @par
    The size field indicates how much space is reserved. If the entire
    profiling data does not fit in this field, the profiler sends partial
    profiling information covering as many software threads as will fit.

	 the physical address shared serves as a ping-pong buffer. Ping and pong buffers
	 each have a size of half the total size.

	 The meaning of the physical address is different when this command is sent from
	 client to ADSP vs. when ADSP raised events to the client.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
 */
#define AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS   0x0001290A

/** Disables profiling. */
#define AVS_PROFILING_OFF                0

/** Enables profiling to collect summary information of MIPS/memory. */
#define AVS_PROFILING_SUMMARY            1

/** Enables profiling to collect summary information of MIPS/memory,
  per-software-thread MIPS, and stack consumption. AXI BW, AHB BW, MPPS,
  and cache miss ratios (L1 I$, L1D$, L2$) are also collected.
 */
#define AVS_PROFILING_PER_THREAD         2

/** Only user provided metrics will be collected. */
#define AVS_PROFILING_USER_SPECIFIED     3

/** special mode to debug BW related issues */
#define AVS_PROFILING_BW                 4

/** Only user provided metrics will be collected & user specified contains per thread info as well.
 */
#define AVS_PROFILING_USER_SPECIFIED_PLUS_PER_THREAD     5

/** Shared memory is virtually mapped. */
#define SHARED_MEM_PROPERTY_VIRTUAL   1

/** Shared memory is physically mapped. */
#define SHARED_MEM_PROPERTY_PHYSICAL  0

/** @} */ /* end_addtogroup core_cmd_profiling */

typedef struct avcs_cmd_set_periodic_profiling_events_t avcs_cmd_set_periodic_profiling_events_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_cmd_set_periodic_profiling_events
@{ */
/* Payload of the AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS command,
  which requests periodic events with profiling information.
 */
struct avcs_cmd_set_periodic_profiling_events_t
{
	uint16_t              profiling_level;
	/**< Level of profiling to be collected.

         @values
           - #AVS_PROFILING_OFF
           - #AVS_PROFILING_SUMMARY
           - #AVS_PROFILING_PER_THREAD
           - #AVS_PROFILING_USER_SPECIFIED
           - #AVS_PROFILING_BW
           - #AVS_PROFILING_USER_SPECIFIED_PLUS_PER_THREAD
           @tablebulletend */

	uint16_t              enable_batch_event;
	/**< one event contains multiple profiling data sets. */

	uint32_t              physical_address_lsw;
	/**< Lower 32 bits of the physical address of the buffer into which the AVS
         writes the profiling data for each event.

	 */
	uint32_t              physical_address_msw;
	/**< Upper 32 bits of the physical address of the buffer into which the AVS
         writes the profiling data for each event.

         The 64-bit address formed by physical_address_lsw and
         physical_address_msw must be 4&nbsp;KB aligned.

	 * In cmd from test-fmwk to ADSP: this memory contains avcs_pmu_pmu_events_needed_t following
	 *  which avcs_pmu_event_set_t indicate the events to be enabled in a round-robin fashion.
	 *  Test fmwk is responsible for making sure that totally multiple of 4 events are enabled.
	 *
	 * In event from ADSP to test-fmwk: this memory contains avcs_profiling_info_t.
	 * */

	uint16_t              size;
	/**< Number of bytes available in the shared buffer. This size must be in
         multiples of 4 KB pages.

         test framework and ADSP work on ping-pong buffers.
         The size include the size of ping and pong buffers.

         half of size must be sufficient for avcs_profiling_info_t (including its variable part) +
         avcs_adsppm_voting_info_t is needed + avcs_sw_thread_info_t of MAX software threads.

	 */
	uint16_t              max_profiled_sw_threads;
	/** max num of sw threads profiled. -1 => as much as memory is available.*/

	uint16_t              avcs_profiling_minor_version;
	/**< Minor version used for tracking the periodic profiling interface
        version. */

	uint8_t              adsppm_voting_info_needed;
	/**< whether ADSPPM event info is needed. */

	uint8_t              num_pmu_counters_to_use;
	/**< number of PMU counters to use */

	uint32_t              sampling_period;
	/**< Number of microseconds between the profiling measurements.

         @values @ge 100000 */

	uint8_t              property_flag;
	/** Indicates whether the shared memory provided is virtual (1) or physical
       (0). */

	uint8_t              keep_sysmon_dcvs_enabled;
	/** by default sysmon is disabled. set to true to keep it enabled.*/

	uint8_t              sw_thread_adsppm_voting_info_needed;
	uint8_t              reserved2;

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_weakgroup weak_avcs_cmd_set_periodic_profiling_events */

/* NOTE: Q8 = Hexagon QuRT RTOS User Guide */
/** @ingroup core_event_profiling
  In response to the #AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS command, provides
  statistics for the last measurement event. For detailed information on how to
  process this data, refer to @xhyperref{Q8,[Q8]}.

  @apr_hdr_fields
    Opcode -- AVCS_EVENT_PERIODIC_PROFILING

  @msg_payload{avcs_event_periodic_profiling_t}
    @table{weak__avcs__event__periodic__profiling}

  @par Profiling payload (avcs_profiling_info_t)
    @tablens{weak__avcs__event__periodic__profiling__t}

  @par Software thread information (avcs_sw_thread_info_t)
    @table{weak__avcs__sw__thread__info}

  @return
    None.

  @dependencies
    None.
 */
#define AVCS_EVENT_PERIODIC_PROFILING   0x0001290B

typedef struct avcs_event_periodic_profiling_t avcs_event_periodic_profiling_t;
#include "adsp_begin_pack.h"
struct avcs_event_periodic_profiling_t
{
	uint32_t              physical_address_lsw;
	/**< Lower 32 bits of the physical address of the buffer into which the AVS
        writes the profiling data for each event. avcs_profiling_info_t */

	uint32_t              physical_address_msw;
	/**< Upper 32 bits of the physical address of the buffer into which the AVS
        writes the profiling data for each event.
	 */

	uint16_t              num_profiling_events_in_batch;
	/** how many profiling events are contained in this buffer. Each is of struct avcs_profiling_info_t */

	uint16_t              size_of_one_item;
	/**< each profiling item will be of this size. this is for group events */
}
#include "adsp_end_pack.h"
;

/** contains the PMU event ids. */
typedef struct avcs_pmu_event_set_t avcs_pmu_event_set_t;
#include "adsp_begin_pack.h"
struct avcs_pmu_event_set_t
{
	uint8_t evt[4];
}
#include "adsp_end_pack.h"
;

/** contains the PMU event ids. the physical_address in avcs_cmd_set_periodic_profiling_events_t
 * points this this struct when sending command from test fmwk to DSP.*/
typedef struct avcs_pmu_pmu_events_needed_t avcs_pmu_pmu_events_needed_t;
#include "adsp_begin_pack.h"
struct avcs_pmu_pmu_events_needed_t
{
	uint32_t num_pmu_event_sets;
	/**< This is followed by array of avcs_pmu_event_set_t. */
}
#include "adsp_end_pack.h"
;

#define AVCS_COMPONENT_ADSPPM_VOTING_MASK__MIPS_CHANGED_MASK 0x1

#define AVCS_COMPONENT_ADSPPM_VOTING_MASK__ADSP_DDR_BW_CHANGED_MASK 0x2

typedef struct avcs_component_adsppm_voting_info_t avcs_component_adsppm_voting_info_t;
#include "adsp_begin_pack.h"
struct avcs_component_adsppm_voting_info_t
{
   uint32_t mask;
   /** mask indicating whether MIPs changed or ADSP-DDR BW or ADSP-LPM BW changed (1) or not (0).
    * See AVCS_COMPONENT_ADSPPM_VOTING_MASK*/
	uint16_t mips_per_thread;
	/** MIPs per thread */
	uint16_t total_mips;
	/** total MIPs*/

   uint32_t ib_adsp_ddr_Bps_lsw;
   /** LSW of ADSP-DDR instantaneous bandwidth (IB = BW voted to ADSPPM ) */
   uint32_t ib_adsp_ddr_Bps_msw;
   /** MSW of IB ADSP-DDR instantaneous bandwidth */
   uint32_t ab_adsp_ddr_Bps_lsw;
   /** LSW of ADSP-DDR average bandwidth (AB = IB * utilization factor = avg BW)*/
   uint32_t ab_adsp_ddr_Bps_msw;
   /** MSW of ADSP-DDR average bandwidth */

}
#include "adsp_end_pack.h"
;

typedef struct avcs_adsppm_voting_info_t avcs_adsppm_voting_info_t;
#include "adsp_begin_pack.h"
struct avcs_adsppm_voting_info_t
{
	avcs_component_adsppm_voting_info_t overall;
   uint32_t adsppm_aggregated_bw; //Mbps
   uint32_t adsppm_aggregated_mpps;
}
#include "adsp_end_pack.h"
;
/** @ingroup core_event_state
    Version of the periodic profiling interface. */
#define AVCS_API_VERSION_PERIODIC_PROFILING     0x2

typedef struct avcs_profiling_info_t avcs_profiling_info_t;

#include "adsp_begin_pack.h"

/** @weakgroup weak_avcs_profiling_info_t
@{ */
/* Profiling payload of the AVCS_EVENT_PERIODIC_PROFILING event,
    which provides a periodic update of profiling statistics from the AVS. */
/** The payload contains one instance of this structure, followed by avcs_adsppm_voting_info_t
 * and then followed by,
    profiled_sw_threads instances of %avcs_sw_thread_info_t
    (see <b>Software thread information</b> on the next page).

   the version of this struct is in AVCS_CMD_SET_PERIODIC_PROFILING_EVENTS
 */
struct avcs_profiling_info_t
{
	uint32_t              core_clock_freq;
	/**< Configured frequency in kHz of the aDSP core clock. */

	uint32_t              snoc_clock_freq;
	/**< Configured frequency in kHz of the SNOC bus clock. */

	uint32_t              bimc_clock_freq;
	/**< Configured frequency in kHz of the BIMC bus clock. */

	uint32_t              bus_clock_freq;
	/**< Configured frequency in kHz of the aDSP AHB bus clock. */

	uint32_t              wall_clock_ticks;
	/**< Number of ticks from the previous event.

         This is used to estimate the duration since the previous measurement
         event.*/

	uint32_t              proc_cycles;
	/**< Number of processor cycles that occurred since the previous
         measurement event. */

	uint32_t              current_elite_heap_use;
	/**< Current number of bytes allocated from the heap. */

	uint32_t              peak_elite_heap_use;
	/**< Peak heap consumption in bytes since bootup or the last
         #AVCS_CMD_RESET_PEAK_HEAP_USE. */

	uint32_t              current_non_elite_heap_use;
	/**< Current number of bytes allocated from the heap for non-Elite memory
         allocations. */

	uint32_t              peak_non_elite_heap_use;
	/**< Peak number of bytes allocated from the heap since bootup for
         non-Elite memory allocations. */

	uint32_t              hw_idle_cycles[6];
	/**< Per hardware thread, the number of idle cycles since the last
         measurement event. */

	uint32_t              profiled_sw_threads;
	/**< Number of profiled software threads.

         @values
         - 0 -- Software thread profiling is not present
         - > 0 -- Number of active software threads in the AVS @tablebulletend */

   uint32_t              pmu_clobbered;
   /** 1 -> PMU registered are clobbered. 0 -> not */

	avcs_pmu_event_set_t  pmu_event_sets[2];
	/** PMU event set (contains PMU event ids). total 8 PMU event IDs.*/

	uint32_t              pmu_event_counters[8];
	/**< PMU counter values. The corresponding event-ids are given by avcs_pmu_event_set_t.
	 * pmu_event_counters[0] -> pmu_event_sets[0].evt0,
	 * pmu_event_counters[1] -> pmu_event_sets[0].evt1,
	 * pmu_event_counters[2] -> pmu_event_sets[0].evt2,
	 * pmu_event_counters[3] -> pmu_event_sets[0].evt3,
	 * pmu_event_counters[4] -> pmu_event_sets[1].evt0,
	 *  ...
	 * pmu_event_counters[7] -> pmu_event_sets[1].evt3,
	 * If 8 PMU counters are not available, then the rest are set to zero.*/

}
#include "adsp_end_pack.h"
;


/** @ingroup core_cmd_mem_stats
Requests information on AVS memory consumption.

@apr_hdr_fields
  Opcode -- AVCS_CMD_GET_MEMORY_STATS

@msg_payload
  None.

@return
 #AVCS_CMDRSP_GET_MEMORY_STATS

@dependencies
  None.
  */
#define AVCS_CMD_GET_MEMORY_STATS                    0x00012907

 /* NOTE: Q9 = QuRT_Elite API Interface Specification (80-N2962-1) */
 /** @ingroup core_resp_mem_stats
In response to the #AVCS_CMD_GET_MEMORY_STATS command, returns statistics
related to QuRT_Elite memory allocation use. For more information on
QuRT_Elite, refer to @xhyperref{Q9,[Q9]}.

This event can be used to detect how fully the AVS heap is being used and
to track memory allocations/free counts for memory leaks.

@apr_hdr_fields
  Opcode -- AVCS_CMDRSP_GET_MEMORY_STATS

@msg_payload{avcs_cmd_rsp_get_memory_stats_t}
  @table{weak__avcs__cmd__rsp__get__memory__stats__t}

@return
  None.

@dependencies
  None.
  */
#define AVCS_CMDRSP_GET_MEMORY_STATS                  0x00012908

 typedef struct avcs_cmd_rsp_get_memory_stats_t avcs_cmd_rsp_get_memory_stats_t;

#include "adsp_begin_pack.h"

 /** @weakgroup weak_avcs_cmd_rsp_get_memory_stats_t
@{ */
 /* Payload of the AVCS_CMDRSP_GET_MEMORY_STATS command response, which
returns statistics related to QuRT_Elite memory allocation use.
  */
 struct avcs_cmd_rsp_get_memory_stats_t
 {

    uint32_t              num_elite_mallocs;
    /**< Number of Elite memory allocations that have occurred since bootup or
       since statistics were reset. */

    uint32_t              num_elite_frees;
    /**< Number of times Elite memory was freed since bootup or since
       statistics were reset. */

    uint32_t              current_elite_heap_use;
    /**< Current number of bytes allocated from the heap since statistics were
       reset for Elite. */

    uint32_t              peak_elite_heap_use;
    /**< Peak number of bytes allocated from the heap since bootup or since
       statistics were reset for Elite.*/

    uint32_t              num_non_elite_mallocs;
    /**< Number of non-Elite memory allocations that have occurred since bootup
       or since statistics were reset. */

    uint32_t              num_non_elite_frees;
    /**< Number of times non-Elite memory was freed since bootup or since
       statistics were reset. */

    uint32_t              current_non_elite_heap_use;
    /**< Current number of bytes allocated from the heap since statistics were
       reset for non-Elite. */

    uint32_t              peak_non_elite_heap_use;
    /**< Peak number of bytes allocated from the heap since bootup or since
       statistics were reset for non-Elite. */
    uint32_t              num_elite_nondefault_mallocs;
    /**< Number of Elite lpa and lpm heap memory allocations that have occurred since bootup or
           since statistics were reset. */

    uint32_t              num_elite_nondefault_frees;
    /**< Number of times Elite lpa  and lpm heap memory was freed since bootup or since
           statistics were reset. */

    uint32_t              current_elite_nondefault_heap_use;
    /**< Current number of bytes allocated from the lpa and lpm heap since statistics were
           reset for Elite. */

    uint32_t              peak_elite_nondefault_heap_use;
    /**< Peak number of bytes allocated from the lpa and lpm heap since bootup or since
           statistics were reset for Elite.*/
 }
#include "adsp_end_pack.h"
 ;
/** @} */ /* end_weakgroup weak_avcs_cmd_rsp_get_memory_stats_t */

/** @ingroup core_cmd_peak_heap_use
  Requests that the AVS reset the peak heap usage to zero, as reported in
  profiling events and acknowledgments.

  @apr_hdr_fields
    Opcode -- AVCS_CMD_RESET_PEAK_HEAP_USE

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
 */
#define AVCS_CMD_RESET_PEAK_HEAP_USE                  0x00012909

/**
   Registers with a service to send an event whenever ADSPPM voting is done.

  @apr_hdr_fields
    Opcode -- CVD_CMD_REGISTER_FOR_ADSPPM_VOTE_EVENT

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
 */
#define CVD_CMD_REGISTER_FOR_ADSPPM_VOTE_EVENT        0x0001291F

/**
   Deregisters with a service from sending an event whenever ADSPPM voting is done.

  @apr_hdr_fields
    Opcode -- CVD_CMD_DEREGISTER_FOR_ADSPPM_VOTE_EVENT

  @msg_payload
    None.

  @return
    APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

  @dependencies
    None.
 */
#define CVD_CMD_DEREGISTER_FOR_ADSPPM_VOTE_EVENT        0x00012920

/**
   Whenever ADSPPM voting is done for MIPs or Bandwidth
   a service sends this event as long as another service has registered with
   CVD_CMD_REGISTER_FOR_ADSPPM_VOTE_EVENT.Event stops when the other service
   calls CVD_CMD_DEREGISTER_FOR_ADSPPM_VOTE_EVENT.

  @apr_hdr_fields
    Opcode -- CVD_EVENT_ADSPPM_VOTE_DONE

  @msg_payload{avcs_event_adsppm_vote_done_t}

  @return
    None.

  @dependencies
    None.
 */

#define CVD_EVENT_ADSPPM_VOTE_DONE             0x00012921

#define CVD_COMPONENT_ADSPPM_VOTING_MASK__MIPS_CHANGED_MASK 0x1

#define CVD_COMPONENT_ADSPPM_VOTING_MASK__ADSP_DDR_BW_CHANGED_MASK 0x2


typedef struct cvd_event_adsppm_vote_done_t cvd_event_adsppm_vote_done_t;
#include "adsp_begin_pack.h"
struct cvd_event_adsppm_vote_done_t
{
   uint32_t mask;
   /** mask indicating whether MIPs changed or ADSP-DDR BW or ADSP-LPM BW changed (1) or not (0).
    * See CVD_COMPONENT_ADSPPM_VOTING_MASK*/
   uint16_t mips_per_thread;
   /** MIPs per thread */
   uint16_t total_mips;
   /** total MIPs*/

   uint32_t ib_adsp_ddr_Bps_lsw;
   /** LSW of ADSP-DDR instantaneous bandwidth (IB = BW voted to ADSPPM ) */
   uint32_t ib_adsp_ddr_Bps_msw;
   /** MSW of IB ADSP-DDR instantaneous bandwidth */
   uint32_t ab_adsp_ddr_Bps_lsw;
   /** LSW of ADSP-DDR average bandwidth (AB = IB * utilization factor = avg BW)*/
   uint32_t ab_adsp_ddr_Bps_msw;
   /** MSW of ADSP-DDR average bandwidth */
}
#include "adsp_end_pack.h"
;
/**
  This command is to lock cache lines with the designated total size.
  Note that there are 3 sub-opcodes in this command.
  To use this functionality, the first command issued to DSP must be AVCS_COMMAND_LOCK_LINES_INIT to alloc a buffer to lock lines with.
  During testing we can issue command AVCS_COMMAND_LOCK_LINES_EXEC of arbitrary size to DSP.
  When we are done with the testing, we should issue AVCS_COMMAND_LOCK_LINES_DEINIT to free the buffer we have just used.
  @apr_hdr_fields
    Opcode -- AVCS_COMMAND_LOCK_LINES

  @msg_payload{avcs_event_adsppm_vote_done_t}

  @return
    None.

  @dependencies
    None.
 */
#define AVCS_COMMAND_LOCK_LINES   0x00012922

#define AVCS_COMMAND_LOCK_LINES_INIT   0x1
#define AVCS_COMMAND_LOCK_LINES_EXEC   0x2
#define AVCS_COMMAND_LOCK_LINES_DEINIT 0x3

typedef struct avcs_command_lock_lines_t avcs_command_lock_lines_t;
#include "adsp_begin_pack.h"
struct avcs_command_lock_lines_t
{
   uint32_t sec_opcode;
   /** Secondary opcode denoting whether this command is to init, exec or deinit line locking
    *  For init, we will create a memory partition with the sizes designated
    *  For exec, we will lock the lines with the sizes designated
    *  For de-init, we will first unlock the whole partiiton, then we will delete the partition
    */
   uint32_t  total_size_bytes;
   /**< Total sizes of the current operation.
            @values
            -all -- total size will be locked with the current operation
                     must be 32-bit aligned. */

}
#include "adsp_end_pack.h"
;


/**
 *
 * This command removes all modules in AMDB, register static modules and then
 * add new module according to ACDB file if present.
 * If ACDB file is absent, all built-in modules will be loaded.
 *
 * this simulates the behavior at boot-up.
 *
  @apr_hdr_fields
    Opcode -- AVCS_AMDB_RESET

  @msg_payload{avcs_event_adsppm_vote_done_t}

  @return
    None.

  @dependencies
    None.
 */
#define    AVCS_AMDB_RESET 0x0001292B


#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_PRV_AVCS_API_H_ */
