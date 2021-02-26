/**
 @file AVSCoreSvc_PerfMon.cpp***********************

 @brief This file contains the implementation for ADSP performance monitoring.

 */

/*========================================================================
 $Header: //components/rel/avs.adsp/2.8.5/avcs/src/AdspCoreSvc_PerfMon.cpp#1 $

 Edit History

 when       who     what, where, why
 --------   ---     -------------------------------------------------------
 06/08/10   MWC     Created file.
 01/19/17   APM     Removed usage of QURT_PMUCFG register after hexagon V60
 ==========================================================================*/

/*-----------------------------------------------------------------------
 Copyright (c) 2010, 2017 Qualcomm Technologies, Incorporated.  All Rights Reserved.
 QUALCOMM Proprietary.
 -----------------------------------------------------------------------*/

/* =======================================================================
 INCLUDE FILES FOR MODULE
 ========================================================================== */
#include "AdspCoreSvc_PerfMon.h"
#include "AdspCoreSvc_InstanceStruct.h"
#include "qurt_elite_adsppm_wrapper.h"
#include <stringl/stringl.h>
#include "amssheap.h"
extern "C" {
#ifndef MDSPMODE
#include "sysmon_prof.h"
#endif
}
//#define VERBOSE_DEBUG
/*--------------------------------------------------------------*/
/* Macro definitions                                            */
/* -------------------------------------------------------------*/

/* -----------------------------------------------------------------------
 ** Constant / Define Declarations
 ** ----------------------------------------------------------------------- */
/* Minimum supported profiling period is 1 msec
 */
#define AVS_PROFILING_DURATION_MIN 1000
#define AVS_PROFILING_MAX_BATCH_DUR 100000
//query clock every 100ms only.
#define AVS_PROFILING_CLOCK_QUERY_MIN_INTERVAL 100000

/* the portion of the payload that is variable sized */
struct ExtendedPayload_t
{
   avcs_sw_thread_info_t SwThread[QURT_ELITE_MAX_SW_THREADS];
};

/* -----------------------------------------------------------------------
 ** Function prototypes
 ** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
 ** Static variables
 ** ----------------------------------------------------------------------- */

extern uint32_t avcsMemoryMapClient;


/******************************************************************************
 * FUNCTION: AdspCoreSvc_QueryInitialClocks
 * DESCRIPTION:
 *        queries value of clocks
 *        Tracks clock votes.
 *        with Multi-PD changes its no longer possible to rely on events.
 *        Events occur only due to votes from user-PD.
 *        but we need clock due to entire system.
 *        Also query by name doesnot work.
 *
 *****************************************************************************/
static void AdspCoreSvc_QueryClocks(AvsCoreSvc_Prof_t *pProf)
{
   npa_query_status status;
   npa_query_type query_result;
#ifdef VERBOSE_DEBUG
   MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Querying clocks");
#endif
   // Query the resource for the current state.
   if (NULL ==pProf->core_clk_handle)
   {
#ifndef SIM
      pProf->core_clk_handle = npa_create_query_handle("/clk/cpu");
#endif
#ifdef VERBOSE_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"NPA handle for CPU =%lu", pProf->core_clk_handle);
#endif
   }
   if (NULL ==pProf->snoc_clk_handle)
   {
#ifndef SIM
      pProf->snoc_clk_handle = npa_create_query_handle("/clk/snoc");
#endif
#ifdef VERBOSE_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"NPA handle for SNOC=%lu", pProf->snoc_clk_handle);
#endif
   }

   if (NULL ==pProf->bimc_clk_handle)
   {
#ifndef SIM
      pProf->bimc_clk_handle = npa_create_query_handle("/clk/bimc");
#endif
#ifdef VERBOSE_DEBUG
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"NPA handle for BIMC=%lu", pProf->bimc_clk_handle);
#endif
   }
   if (NULL !=pProf->core_clk_handle)
   {
      status = npa_query(pProf->core_clk_handle, NPA_QUERY_CURRENT_STATE, &query_result);
      if ( status == NPA_QUERY_SUCCESS )
      {
         // The value query_result.data.state has the result of the query.
         pProf->npa_adsp_core_clk = query_result.data.state;
#ifdef VERBOSE_DEBUG
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Querying CPU clocks. success clock=%lu", pProf->npa_adsp_core_clk);
#endif
      }
      else
      {
         pProf->npa_adsp_core_clk = 0;
#ifdef VERBOSE_DEBUG
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"Querying CPU clocks. failed.");
#endif
      }

   }
   if (NULL !=pProf->snoc_clk_handle)
   {
      status = npa_query(pProf->snoc_clk_handle, NPA_QUERY_CURRENT_STATE, &query_result);
      if ( status == NPA_QUERY_SUCCESS )
      {
         // The value query_result.data.state has the result of the query.
         pProf->npa_snoc_clk = query_result.data.state;
#ifdef VERBOSE_DEBUG
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO," Querying SNOC clocks. success clock=%lu", pProf->npa_snoc_clk);
#endif
      }
      else
      {
         pProf->npa_snoc_clk = 0;
#ifdef VERBOSE_DEBUG
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO," Querying SNOC clocks. failed.");
#endif
      }
   }
   if (NULL !=pProf->bimc_clk_handle)
   {
      status = npa_query(pProf->bimc_clk_handle, NPA_QUERY_CURRENT_STATE, &query_result);
      if ( status == NPA_QUERY_SUCCESS )
      {
         // The value query_result.data.state has the result of the query.
         pProf->npa_bimc_clk = query_result.data.state;
#ifdef VERBOSE_DEBUG
         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO," Querying BIMC clocks. success clock=%lu", pProf->npa_bimc_clk);
#endif
      }
      else
      {
         pProf->npa_bimc_clk = 0;
#ifdef VERBOSE_DEBUG
         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO," Querying BIMC clocks. failed.");
#endif
      }
   }   
}



static void AdspCoreSvc_DeregisterNpaCallbacks(AvsCoreSvc_Prof_t *pProf)
{
   if (NULL != pProf->core_clk_handle)
   {
      npa_destroy_query_handle(pProf->core_clk_handle);
      pProf->core_clk_handle = NULL;
   }
   if (NULL != pProf->snoc_clk_handle)
   {
      npa_destroy_query_handle(pProf->snoc_clk_handle);
      pProf->snoc_clk_handle = NULL;
   }
   if (NULL != pProf->bimc_clk_handle)
   {
      npa_destroy_query_handle(pProf->bimc_clk_handle);
      pProf->bimc_clk_handle = NULL;
   }
}

/* =======================================================================
 **                          Function Definitions
 ** ======================================================================= */

static uint32_t get_next_pmu_events_to_cfg(AvsCoreSvc_Prof_t *pProf)
{
   uint32_t ret = 0;
   avcs_pmu_event_set_t *set = &pProf->event_set_ptr[pProf->cfg_count_index];
   ret = ((uint32_t) set->evt[0]) | (((uint32_t) set->evt[1]) << 8)
                              		            | (((uint32_t) set->evt[2]) << 16) | (((uint32_t) set->evt[3]) << 24);
   pProf->cfg_count_index += 1;
   if (pProf->cfg_count_index == pProf->total_pmu_event_sets)
   {
      pProf->cfg_count_index = 0;
   }

   //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO,"PMU configured to count 0x%x",ret);

   return ret;
}

static avcs_pmu_event_set_t * get_next_pmu_events_to_read(AvsCoreSvc_Prof_t *pProf)
{
   avcs_pmu_event_set_t *set = &pProf->event_set_ptr[pProf->read_count_index];

   pProf->read_count_index += 1;
   if (pProf->read_count_index == pProf->total_pmu_event_sets)
   {
      pProf->read_count_index = 0;
   }
   return set;

}

static void copy_pmu_event_set(avcs_pmu_event_set_t *dst, avcs_pmu_event_set_t *src)
{
   dst->evt[0] = src->evt[0];
   dst->evt[1] = src->evt[1];
   dst->evt[2] = src->evt[2];
   dst->evt[3] = src->evt[3];
}

static uint32_t get_pmu_cfg_mask(uint32_t num_hw_theads)
{
   // Set PMU configuration register to measure events on all six threads
   // and events from both user mode and supervisor mode.
   /* [3:0] - thread mask - 1 bit per h/w thread
    * [8] - user/monitor modes - 0 to enable counting in all modes
    *                            1 to enable only user and guest mode counting
    * [9] - Tcycle/Pcycle - for events 0x10, 0x11, 0x18, 0x19, 0x4f, 0x9f
    *                          0 - counts in Thread cycles
    *                          1 - counts in Processor cycles (Counters increment every Pcycle and TID (mask from 3:0) is not used to qualify the
         event. This setting will show the total number of pcycles that one or more
         threads experienced the selected event. The six duration counters will not
         provide meaningful data if this bit is set and multiple threads are profiled
         simultaneously. ""This bit should only be set when profiling a single thread."")
    */
   uint32_t mask = 0;

   /* PMU Configuration register is removed after hexagon version 60. 
    * Modifying this register could lead to undesired results.
    */
   #if (__HEXAGON_ARCH__ <= 60)

     mask = (1 << num_hw_theads) - 1;
     //mask |= ((uint32_t) 0x1) << 9;
     //muthu's recommendation is to enable this flag always as tcycles dont make sense anymore.
     //however, based on the arch spec, "This bit should only be set when profiling a single thread". So keeping it zero.
   #endif

   //MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PMU mask 0x%X", mask);

   return mask;
}

static uint32_t thread_profiling(AvsCoreSvc_t *pMe,
      ExtendedPayload_t *pExtPayload)
{
   if (NULL == pExtPayload || pMe->prof.uMaxProfiledThreads == 0)
   {
      return 0;
   }

   uint32_t profiled_sw_threads = 0;
   uint64_t ullCycles[6]; //6 as per qurt API requirement

   if ( (AVS_PROFILING_PER_THREAD == pMe->prof.uProfilingLevel) ||
         (AVS_PROFILING_USER_SPECIFIED_PLUS_PER_THREAD == pMe->prof.uProfilingLevel))
   {
      qurt_sysenv_max_hthreads_t maxhwt;
      qurt_sysenv_get_max_hw_threads(&maxhwt);

      uint32_t num_list_elem = sizeof(qurt_elite_globalstate.aThreadContextBuf)
                              		            / sizeof(qurt_elite_globalstate.aThreadContextBuf[0]);

      // lock the mutex so the linked list doesn't morph during processing.
      qurt_elite_mutex_lock(&qurt_elite_globalstate.mutex);
      for (uint32_t count = 0; count < num_list_elem; count++)
      {
         uint32_t czThreadNamePrefix;
         if (QURT_ELITE_THREAD_RUNNING
               == (qurt_elite_globalstate.aThreadContextBuf[count].nThreadState))
         {
            // level 2 profiling - per SW thread
            if (profiled_sw_threads < pMe->prof.uMaxProfiledThreads)
            {
               /* Get and reset total number of thread cycles */
               qurt_profile_get_threadid_pcycles(
                     qurt_elite_globalstate.aThreadContextBuf[count].tid, ullCycles);
               qurt_profile_reset_threadid_pcycles(
                     qurt_elite_globalstate.aThreadContextBuf[count].tid);
               pExtPayload->SwThread[profiled_sw_threads].run_cycles = 0;

               for (unsigned int i = 0; i < maxhwt.max_hthreads; i++)
               {
                  pExtPayload->SwThread[profiled_sw_threads].run_cycles += ullCycles[i];
               }

               /* save thread ID and stack size */
               memscpy((char*) &czThreadNamePrefix, sizeof(czThreadNamePrefix),
                     &(qurt_elite_globalstate.aThreadContextBuf[count].name[0]), 4);
               pExtPayload->SwThread[profiled_sw_threads].thread_id = czThreadNamePrefix;
               pExtPayload->SwThread[profiled_sw_threads].stack_size =
                     qurt_elite_globalstate.aThreadContextBuf[count].nStackSize;

               /* estimate the free stack */
               uint32_t *pStackChecker =
                     (uint32_t*) (qurt_elite_globalstate.aThreadContextBuf[count].pStack);
               int nFreeStack = 0;
               for (nFreeStack = 0;
                     nFreeStack < qurt_elite_globalstate.aThreadContextBuf[count].nStackSize;
                     nFreeStack += QURT_ELITE_STACK_FILL_SPACING * 4, pStackChecker +=
                           QURT_ELITE_STACK_FILL_SPACING)
               {
                  if (QURT_ELITE_STACK_FILL_WORD != *pStackChecker)
                     break;
               }

               pExtPayload->SwThread[profiled_sw_threads].stack_free = nFreeStack;
#ifdef ADSPMODE //ToDo : short term fix to compile AVS.2.8 for 9x65 target. discuss with core team and figure out a way to remove this. 
               memheap_task_stats_type *thread_stats;

               /* Fetch the per thread heap usage*/
               if(ADSP_EOK != amssheap_get_task_stats(qurt_elite_globalstate.aThreadContextBuf[count].tid,(&thread_stats)))
               {
                  MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: Error fetching the heap statistics for thread id 0x%lx",qurt_elite_globalstate.aThreadContextBuf[count].tid);
               }
               else
               {
                  memheap_task_stats_type thread_heap_stats = (memheap_task_stats_type)(*(thread_stats));
                  pExtPayload->SwThread[profiled_sw_threads].curr_heap_usage = (thread_heap_stats).current_usage;
                  pExtPayload->SwThread[profiled_sw_threads].max_heap_usage = (thread_heap_stats).max_usage;

               }
#endif
               profiled_sw_threads++;
            }
         }
      }

      /* release the mutex. */
      qurt_elite_mutex_unlock(&qurt_elite_globalstate.mutex);

   }

   return profiled_sw_threads;
}

static void copy_adsppm_voting_info(avcs_component_adsppm_voting_info_t *src, avcs_component_adsppm_voting_info_t *dst)
{
   memscpy(dst, sizeof(avcs_component_adsppm_voting_info_t), src, sizeof(avcs_component_adsppm_voting_info_t));
}

static bool_t check_pmu_clobberred(AvsCoreSvc_Prof_t  *pProf)
{
   uint32_t temp1 = 0;
   uint32_t temp2 = qurt_pmu_get(QURT_PMUEVTCFG);
   uint32_t temp3 = 0;
   
   #if (__HEXAGON_ARCH__ <= 60)
     temp1 = qurt_pmu_get(QURT_PMUCFG);
   #endif

   bool_t ret = false;
   if (8 == pProf->num_pmu_counters) //8 PMU events
   {
      temp3 = qurt_pmu_get(QURT_PMUEVTCFG1);
   }

   if (pProf->periodCounter != 0)
   {
      if ( (pProf->prev_pmu_cfg != temp1) || (pProf->prev_pmu_evt_cfg != temp2) ||
            ( (pProf->prev_pmu_evt_cfg1 != temp3) && (8 == pProf->num_pmu_counters) )  )
      {
         MSG_6(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "PMU registers are clobberred: 0x%x -> 0x%lx,  0x%x -> 0x%lx,  0x%x -> 0x%lx",
               pProf->prev_pmu_cfg, temp1, pProf->prev_pmu_evt_cfg, temp2, pProf->prev_pmu_evt_cfg1, temp3);
         ret = true;
      }
   }

   return ret;
}

static void process_each_event(AvsCoreSvc_t *pMe,
      avcs_profiling_info_t *pPayload)
{

   //Query clocks only once periodic counter matches a multiple of queryClkInterval
   if (pMe->prof.periodCounter % pMe->prof.queryClkInterval  == 0)
   {
      AdspCoreSvc_QueryClocks(&pMe->prof);
   }

   pPayload->core_clock_freq = pMe->prof.npa_adsp_core_clk;
   pPayload->snoc_clock_freq = pMe->prof.npa_snoc_clk;
   pPayload->bimc_clock_freq = pMe->prof.npa_bimc_clk;

   pPayload->bus_clock_freq = 0;

   /* Wall clock elapsed */
   pPayload->wall_clock_ticks = qurt_sysclock_get_hw_ticks() - pMe->prof.uPrevWallClock;
   pMe->prof.uPrevWallClock += pPayload->wall_clock_ticks;
   /* Processor total and idle cycles */
   pPayload->proc_cycles = (uint32_t) (qurt_get_core_pcycles() - pMe->prof.ullPrevPCycles);
   pMe->prof.ullPrevPCycles += pPayload->proc_cycles;

   uint64_t ullCycles[6]; //6 as per qurt API requirement
   qurt_profile_get_idle_pcycles(ullCycles); //doesnt include idle cycles when all threads are in wait
   qurt_profile_reset_idle_pcycles();
   for (unsigned int i = 0; (i < pMe->prof.tot_num_hw_threads); i++)
   {
      pPayload->hw_idle_cycles[i] = (uint32_t) ullCycles[i];
   }

   /* heap consumption */
   pPayload->current_elite_heap_use = qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].curr_heap;
   pPayload->peak_elite_heap_use = qurt_elite_globalstate.avs_stats[QURT_ELITE_DEFAULT_HEAP_INDEX].peak_heap;
   /*heap consumption in non-elite*/
   pPayload->current_non_elite_heap_use = qurt_elite_globalstate.non_avs_stats.curr_heap;
   pPayload->peak_non_elite_heap_use = qurt_elite_globalstate.non_avs_stats.peak_heap;

   // traverse all threads and examine stack consumption and mcps.
   pPayload->profiled_sw_threads = thread_profiling(pMe, (ExtendedPayload_t*)(( (uint8_t*)pPayload)+
         sizeof(avcs_profiling_info_t)+
         sizeof(avcs_adsppm_voting_info_t)));

   if (pMe->prof.adsppmEventNeeded)
   {
      avcs_adsppm_voting_info_t *padsppm = (avcs_adsppm_voting_info_t*)(((uint8_t*)pPayload)+sizeof(avcs_profiling_info_t));
      uint32_t aggr_bw=0, aggr_mpps=0;
      qurt_elite_adsppm_wrapper_aggregate(&pMe->prof.overall, &pMe->prof.VOC, &aggr_bw, &aggr_mpps);
      copy_adsppm_voting_info(&pMe->prof.overall, &padsppm->overall);
      padsppm->adsppm_aggregated_bw = aggr_bw;
      padsppm->adsppm_aggregated_mpps = aggr_mpps;
   }

   if (pMe->prof.adsppmEventPerThreadNeeded)
   {
      //TODO: to implement this.
   }

   avcs_pmu_event_set_t *pPmuEventIdPtr = pPayload->pmu_event_sets;

   if (pMe->prof.total_pmu_event_sets != 0)
   {
      pPayload->pmu_clobbered = check_pmu_clobberred(&pMe->prof);

      if (8 == pMe->prof.num_pmu_counters) //8 PMU events
      {
         // update AXI bus accesses for read/write using PMU event counters.
         pPayload->pmu_event_counters[0] = qurt_pmu_get(QURT_PMUCNT0);
         pPayload->pmu_event_counters[1] = qurt_pmu_get(QURT_PMUCNT1);
         pPayload->pmu_event_counters[2] = qurt_pmu_get(QURT_PMUCNT2);
         pPayload->pmu_event_counters[3] = qurt_pmu_get(QURT_PMUCNT3);
         pPayload->pmu_event_counters[4] = qurt_pmu_get(QURT_PMUCNT4);
         pPayload->pmu_event_counters[5] = qurt_pmu_get(QURT_PMUCNT5);
         pPayload->pmu_event_counters[6] = qurt_pmu_get(QURT_PMUCNT6);
         pPayload->pmu_event_counters[7] = qurt_pmu_get(QURT_PMUCNT7);

         qurt_pmu_enable(0);

         qurt_pmu_set(QURT_PMUCNT0, 0x0);
         qurt_pmu_set(QURT_PMUCNT1, 0x0);
         qurt_pmu_set(QURT_PMUCNT2, 0x0);
         qurt_pmu_set(QURT_PMUCNT3, 0x0);
         qurt_pmu_set(QURT_PMUCNT4, 0x0);
         qurt_pmu_set(QURT_PMUCNT5, 0x0);
         qurt_pmu_set(QURT_PMUCNT6, 0x0);
         qurt_pmu_set(QURT_PMUCNT7, 0x0);

         //write the PMU event-id to the payload
         avcs_pmu_event_set_t *toread1 = get_next_pmu_events_to_read(&pMe->prof);
         copy_pmu_event_set(pPmuEventIdPtr, toread1);

#ifdef VERBOSE_DEBUG
         MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PMU events1: 0x%x -> %lu, 0x%x -> %lu, 0x%x -> %lu, 0x%x -> %lu",
               pPmuEventIdPtr->evt[0], pPayload->pmu_event_counters[0],
               pPmuEventIdPtr->evt[1], pPayload->pmu_event_counters[1],
               pPmuEventIdPtr->evt[2], pPayload->pmu_event_counters[2],
               pPmuEventIdPtr->evt[3], pPayload->pmu_event_counters[3]);
#endif

         pPmuEventIdPtr += 1;
         avcs_pmu_event_set_t *toread2 = get_next_pmu_events_to_read(&pMe->prof);
         copy_pmu_event_set(pPmuEventIdPtr, toread2);

#ifdef VERBOSE_DEBUG
         MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PMU events2: 0x%x -> %lu, 0x%x -> %lu, 0x%x -> %lu, 0x%x -> %lu",
               pPmuEventIdPtr->evt[0], pPayload->pmu_event_counters[4],
               pPmuEventIdPtr->evt[1], pPayload->pmu_event_counters[5],
               pPmuEventIdPtr->evt[2], pPayload->pmu_event_counters[6],
               pPmuEventIdPtr->evt[3], pPayload->pmu_event_counters[7]);
#endif

         /*Enable PMU event counters. These are enabled after free stack
           computation to avoid profiler overhead in PMU statistics*/
         pMe->prof.prev_pmu_cfg = get_pmu_cfg_mask(pMe->prof.tot_num_hw_threads);
         pMe->prof.prev_pmu_evt_cfg = get_next_pmu_events_to_cfg(&pMe->prof);
         pMe->prof.prev_pmu_evt_cfg1 = get_next_pmu_events_to_cfg(&pMe->prof);

         #if (__HEXAGON_ARCH__ <= 60)
           qurt_pmu_set(QURT_PMUCFG, pMe->prof.prev_pmu_cfg);
         #endif

         qurt_pmu_set(QURT_PMUEVTCFG, pMe->prof.prev_pmu_evt_cfg);

         qurt_pmu_set(QURT_PMUEVTCFG1, pMe->prof.prev_pmu_evt_cfg1);

         qurt_pmu_enable(1);
      }
      else if (4 == pMe->prof.num_pmu_counters)
      {
         // update AXI bus accesses for read/write using PMU event counters.
         pPayload->pmu_event_counters[0] = qurt_pmu_get(QURT_PMUCNT0);
         pPayload->pmu_event_counters[1] = qurt_pmu_get(QURT_PMUCNT1);
         pPayload->pmu_event_counters[2] = qurt_pmu_get(QURT_PMUCNT2);
         pPayload->pmu_event_counters[3] = qurt_pmu_get(QURT_PMUCNT3);
         pPayload->pmu_event_counters[4] = 0;
         pPayload->pmu_event_counters[5] = 0;
         pPayload->pmu_event_counters[6] = 0;
         pPayload->pmu_event_counters[7] = 0;

         qurt_pmu_enable(0);

         qurt_pmu_set(QURT_PMUCNT0, 0x0);
         qurt_pmu_set(QURT_PMUCNT1, 0x0);
         qurt_pmu_set(QURT_PMUCNT2, 0x0);
         qurt_pmu_set(QURT_PMUCNT3, 0x0);

         //write the PMU event-id to the payload
         copy_pmu_event_set(pPmuEventIdPtr, get_next_pmu_events_to_read(&pMe->prof));

#ifdef VERBOSE_DEBUG
         MSG_8(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "PMU events1: 0x%x -> %lu, 0x%x -> %lu, 0x%x -> %lu, 0x%x -> %lu",
               pPmuEventIdPtr->evt[0],pPayload->pmu_event_counters[0],
               pPmuEventIdPtr->evt[1],pPayload->pmu_event_counters[1],
               pPmuEventIdPtr->evt[2],pPayload->pmu_event_counters[2],
               pPmuEventIdPtr->evt[3],pPayload->pmu_event_counters[3]);
#endif

         /*Enable PMU event counters. These are enabled after free stack
           computation to avoid profiler overhead in PMU statistics*/
         pMe->prof.prev_pmu_cfg = get_pmu_cfg_mask(pMe->prof.tot_num_hw_threads);
         pMe->prof.prev_pmu_evt_cfg = get_next_pmu_events_to_cfg(&pMe->prof);

         #if (__HEXAGON_ARCH__ <= 60)
           qurt_pmu_set(QURT_PMUCFG, pMe->prof.prev_pmu_cfg);
         #endif

         qurt_pmu_set(QURT_PMUEVTCFG, pMe->prof.prev_pmu_evt_cfg);

         qurt_pmu_enable(1);
      }
   }
   else
   {
      memset( pPayload->pmu_event_counters,0,sizeof(pPayload->pmu_event_counters));
   }
}

static void enable_disable_adsppm_voting_events(AvsCoreSvc_t *pMe, bool_t is_enable)
{
   uint32_t opcode=is_enable?CVD_CMD_REGISTER_FOR_ADSPPM_VOTE_EVENT:CVD_CMD_DEREGISTER_FOR_ADSPPM_VOTE_EVENT;

   //send APR commands to voice

   uint16_t mvm_addr = 0;
   int32_t rc= __aprv2_cmd_local_dns_lookup(
         (char_t*)"qcom.audio.mvm", sizeof( "qcom.audio.mvm" ), &mvm_addr );

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVS CoreSvc: ADSMM vote event registration with voice: dns look up result %d, mvm_addr 0x%x ", rc, mvm_addr);

   if (!rc)
   {
      rc=elite_apr_if_alloc_send_cmd(pMe->aprHandle, pMe->aprAddr, 0, mvm_addr, 0,         0, opcode, NULL, 0);
      MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVS CoreSvc: sending register/deregister to voice svc for ADSP PM vote event %d ", rc);
   }

   if (is_enable)
   {
      qurt_elite_adsppm_wrapper_begin_profiling();
   }
   else
   {
      qurt_elite_adsppm_wrapper_end_profiling();
   }

   memset(&pMe->prof.overall, 0, sizeof(avcs_component_adsppm_voting_info_t));
   memset(&pMe->prof.VOC, 0, sizeof(avcs_component_adsppm_voting_info_t));

}

static uint16_t sizeof_one_event(AvsCoreSvc_t *pMe)
{
   return sizeof(avcs_profiling_info_t) +
         sizeof(avcs_adsppm_voting_info_t)*(pMe->prof.adsppmEventNeeded>0)+
         pMe->prof.uMaxProfiledThreads*sizeof(avcs_sw_thread_info_t);
}

void AdspCoreSvc_SetPeriodicProfilingEventsCmdHandler(AvsCoreSvc_t *pMe,
      elite_apr_packet_t* pPacket, uint16_t profiling_duration)
{
   ADSPResult result = ADSP_EOK;
   uint32_t profiling_level = AVS_PROFILING_OFF;
   uint32_t sampling_period = 0;
   avcs_cmd_set_periodic_profiling_events_t *pPayload = NULL;

   // if performance monitor is not enabled through QXDM, profiling level,
   // sampling period and shared buffer will be given through APR message.
   pPayload =  (avcs_cmd_set_periodic_profiling_events_t *) elite_apr_if_get_payload_ptr(pPacket);

   if (pPayload->avcs_profiling_minor_version != AVCS_API_VERSION_PERIODIC_PROFILING)
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVS CoreSvc: Unsupported profiler event version %u ",
            pPayload->avcs_profiling_minor_version);
      result = ADSP_EBADPARAM;
      elite_apr_if_end_cmd(pMe->aprHandle, pPacket, result);
      return;
   }

   profiling_level = pPayload->profiling_level;
   sampling_period = pPayload->sampling_period;

   switch (profiling_level)
   {
   /* if request to turn off profiling */
   case AVS_PROFILING_OFF:
   {
      /* if turning off while currently profiling */
      if (AVS_PROFILING_OFF != pMe->prof.uProfilingLevel)
      {
         if (0 != pMe->prof.profileTimer.qTimerObj)
         {
            (void) qurt_elite_timer_delete(&(pMe->prof.profileTimer));
         }

         qurt_profile_enable(0);

         pMe->prof.uProfilingLevel = AVS_PROFILING_OFF;

         // Diable PMU
         qurt_pmu_enable(0);

         if (pMe->prof.event_set_ptr)
         {
            qurt_elite_memory_free(pMe->prof.event_set_ptr);
            pMe->prof.event_set_ptr = NULL;
         }

         AdspCoreSvc_DeregisterNpaCallbacks(&pMe->prof);

         //unmap after turning off the profiling
         if (ADSP_FAILED(
               result = qurt_elite_memorymap_shm_mem_unmap(avcsMemoryMapClient,
                     pMe->prof.unMemMapHandle)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: PerfMon phy mem unmap failed  = %d!", result);
            break;
         }

         if (pMe->prof.adsppmEventNeeded || pMe->prof.adsppmEventPerThreadNeeded)
         {
            enable_disable_adsppm_voting_events(pMe, FALSE);
         }

         if (!pMe->prof.keep_sysmon_dcvs_enabled)
         {
            //MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AdspCoreSvc: Enabling sysmon/DCVS");
            //sysmon_enable(); //now we can enable sysmon
         }

         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AdspCoreSvc: Profiling ended");
      }

      /* else profiling is already off, do nothing. */
      result = ADSP_EOK;
      break;
   }
   /* request to turn on profiling */
   case AVS_PROFILING_SUMMARY:
   case AVS_PROFILING_PER_THREAD:
   case AVS_PROFILING_USER_SPECIFIED:
   case AVS_PROFILING_BW:
   case AVS_PROFILING_USER_SPECIFIED_PLUS_PER_THREAD:
   {
      /* if profiling is not already on */
      if (AVS_PROFILING_OFF == pMe->prof.uProfilingLevel)
      {
         if (AVS_PROFILING_DURATION_MIN > sampling_period)
         {
            result = ADSP_EBADPARAM;
            break;
         }

         //map the phy address pointer in the payload and get vaddr . return error if it fails
         qurt_elite_memorymap_shm_region_t tmpMemMapReg;
         tmpMemMapReg.shm_addr_lsw = pPayload->physical_address_lsw;
         tmpMemMapReg.shm_addr_msw = pPayload->physical_address_msw;
         tmpMemMapReg.mem_size = pPayload->size;

         if (pPayload->property_flag == SHARED_MEM_PROPERTY_PHYSICAL)
         {
            if ((ADSP_FAILED(
                  result = qurt_elite_memorymap_shm_mem_map(avcsMemoryMapClient,
                        &tmpMemMapReg,1,HMEM_CACHE_NONE_SHARED,
                        QURT_ELITE_MEMORYMAP_EBI1_POOL,&(pMe->prof.unMemMapHandle))))
                  || (ADSP_FAILED(
                        result = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(
                              avcsMemoryMapClient, pMe->prof.unMemMapHandle,
                              pPayload->physical_address_lsw,
                              pPayload->physical_address_msw,
                              &(pMe->prof.perfMonEvtPingVirtAddr)))))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVS CoreSvc: PerfMon phy mem map failed  = %d!", result);
               break;
            }

         }
         else
         {
            //mapping virtually
            if (ADSP_FAILED(
                  result = qurt_elite_memorymap_virtaddr_mem_map(avcsMemoryMapClient,&tmpMemMapReg,
                        1,HMEM_CACHE_NONE_SHARED,QURT_ELITE_MEMORYMAP_EBI1_POOL,&(pMe->prof.unMemMapHandle))))
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVS CoreSvc: PerfMon phy mem map failed  = %d!", result);
               break;
            }
            //get virt addr from the virt mapping
            if (ADSP_FAILED(
                  result = qurt_elite_memorymap_get_virtual_addr_from_shmm_handle(
                        avcsMemoryMapClient, pMe->prof.unMemMapHandle,
                        pPayload->physical_address_lsw, pPayload->physical_address_msw,
                        &(pMe->prof.perfMonEvtPingVirtAddr))))
            {
               MSG_2(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVS CoreSvc: PerfMon phy mem map failed  = %d! with address 0x%lx", result,pPayload->physical_address_lsw);
               break;
            }

         }

         pMe->prof.periodCounter = 0;
         pMe->prof.perfMonEvtPingPhyAddrLsw = pPayload->physical_address_lsw;
         pMe->prof.perfMonEvtPingPhyAddrMsw = pPayload->physical_address_msw;
         pMe->prof.perfMonEvtTotalSize = pPayload->size;
         pMe->prof.perfMonEvtPongVirtAddr = pMe->prof.perfMonEvtPingVirtAddr+pPayload->size/2;

         pMe->prof.adsppmEventNeeded = pPayload->adsppm_voting_info_needed;
         pMe->prof.adsppmEventPerThreadNeeded = pPayload->sw_thread_adsppm_voting_info_needed;
         pMe->prof.num_pmu_counters = pPayload->num_pmu_counters_to_use;

         avcs_pmu_pmu_events_needed_t *pSetPayload = (avcs_pmu_pmu_events_needed_t*) pMe->prof.perfMonEvtPingVirtAddr;

         MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVS CoreSvc: Number of PMU events sets %u. Num PMU counters %u ",pSetPayload->num_pmu_event_sets,pMe->prof.num_pmu_counters);

         pMe->prof.total_pmu_event_sets = pSetPayload->num_pmu_event_sets;
         pMe->prof.event_set_ptr = NULL;

         if (pMe->prof.total_pmu_event_sets != 0)
         {
            uint32_t sz = pMe->prof.total_pmu_event_sets * sizeof(avcs_pmu_event_set_t);
            pMe->prof.event_set_ptr = (avcs_pmu_event_set_t*) qurt_elite_memory_malloc(sz, QURT_ELITE_HEAP_DEFAULT);
            if (pMe->prof.event_set_ptr == NULL)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVS CoreSvc: Malloc failed for event set ptr. size = %lu",pMe->prof.total_pmu_event_sets);
               result = ADSP_ENOMEMORY;
               break;
            }
            //copy the given PMU event-ids
            memscpy(pMe->prof.event_set_ptr, sz, (uint8_t*) (pSetPayload + 1), sz);
         }

         pMe->prof.cfg_count_index = 0;
         pMe->prof.read_count_index = 0;

         pMe->prof.uProfilingLevel = profiling_level;

         if (pMe->prof.adsppmEventNeeded || pMe->prof.adsppmEventPerThreadNeeded)
         {
            enable_disable_adsppm_voting_events(pMe, TRUE);
         }
         pMe->prof.keep_sysmon_dcvs_enabled = pPayload->keep_sysmon_dcvs_enabled;

         /* Enable performance monitoring */
         qurt_profile_enable(1);

         /** Disable sysmon so that it doesn't interfere with our profiling. PMU registers are shared.*/
         if (!pMe->prof.keep_sysmon_dcvs_enabled)
         {
            //MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AdspCoreSvc: Disabling sysmon/DCVS");
            //sysmon_disable();
         }

         /* Reset states of qurt profiling timers */
         pMe->prof.uPrevWallClock = qurt_sysclock_get_hw_ticks();
         pMe->prof.ullPrevPCycles = qurt_get_core_pcycles();
         qurt_profile_reset_idle_pcycles();

         pMe->prof.perfMonEvtSrcAddr = elite_apr_if_get_dst_addr(pPacket);
         pMe->prof.perfMonEvtDstAddr = elite_apr_if_get_src_addr(pPacket);
         pMe->prof.perfMonEvtDstPort = elite_apr_if_get_src_port(pPacket);
         pMe->prof.uMaxProfiledThreads = 0;
         if (pPayload->max_profiled_sw_threads)
         {
            /* reset TCycle counters for each SW thread */
            // lock the mutex so the linked list doesn't morph during processing.
            qurt_elite_mutex_lock(&qurt_elite_globalstate.mutex);

            // reset pcycles in the thread list.

            const uint32_t num_list_elem = sizeof(qurt_elite_globalstate.aThreadContextBuf)
                                    		            / sizeof(qurt_elite_globalstate.aThreadContextBuf[0]);
            for (uint32_t index = 0; index < num_list_elem; index++)
            {
               if (QURT_ELITE_THREAD_RUNNING
                     == (qurt_elite_globalstate.aThreadContextBuf[index].nThreadState))
               {
                  qurt_profile_reset_threadid_pcycles(
                        qurt_elite_globalstate.aThreadContextBuf[index].tid);
               }
            }

            // release the mutex.
            qurt_elite_mutex_unlock(&qurt_elite_globalstate.mutex);

            uint32_t total_filled_size = sizeof(avcs_profiling_info_t) + sizeof(avcs_adsppm_voting_info_t);

            if (total_filled_size > pPayload->size/2)
            {
               pMe->prof.uMaxProfiledThreads = 0;
            }
            else
            {
               if (pPayload->max_profiled_sw_threads == (uint16_t)-1)
               {
                  pMe->prof.uMaxProfiledThreads = (pPayload->size/2 - total_filled_size) / sizeof(avcs_sw_thread_info_t);
               }
               else
               {
                  pMe->prof.uMaxProfiledThreads = pPayload->max_profiled_sw_threads;
               }
            }
         }

         /** decide totNumProfSetsInBatch based on memory available and on min batch duration. */
         pMe->prof.totNumProfSetsInBatch = 1;
         pMe->prof.numProfSetInBatch = 0;
         if (pPayload->enable_batch_event)
         {
            uint16_t size_of_one_prof_event = sizeof_one_event(pMe);
            pMe->prof.totNumProfSetsInBatch = (pPayload->size/2)/(size_of_one_prof_event);

            MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVS CoreSvc: total number of events in a batch due to mem constraints is %u",pMe->prof.totNumProfSetsInBatch);

            if (pMe->prof.totNumProfSetsInBatch == 0)
            {
               MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVS CoreSvc: Memory %lu is not enough for even one event",pPayload->size/2);
               result = ADSP_EBADPARAM;
               break;
            }
         }

         if (sampling_period <= AVS_PROFILING_MAX_BATCH_DUR)
         {
            uint32_t numSetsInBatch = AVS_PROFILING_MAX_BATCH_DUR/sampling_period;
            if (numSetsInBatch < pMe->prof.totNumProfSetsInBatch)
            {
               pMe->prof.totNumProfSetsInBatch=numSetsInBatch;
               MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVS CoreSvc: total number of events in a batch due to time constraints is %u",numSetsInBatch);
            }
         }

         MSG_1(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AVS CoreSvc: total number of events in a batch is %u",pMe->prof.totNumProfSetsInBatch);

         if (pMe->prof.total_pmu_event_sets != 0)
         {
            // Initialize PMU event counters to zero
            qurt_pmu_set(QURT_PMUCNT0, 0x0);
            qurt_pmu_set(QURT_PMUCNT1, 0x0);
            qurt_pmu_set(QURT_PMUCNT2, 0x0);
            qurt_pmu_set(QURT_PMUCNT3, 0x0);
 
            #if (__HEXAGON_ARCH__ <= 60)
              qurt_pmu_set(QURT_PMUCFG, get_pmu_cfg_mask(pMe->prof.tot_num_hw_threads));
            #endif

            qurt_pmu_set(QURT_PMUEVTCFG, get_next_pmu_events_to_cfg(&pMe->prof));

            if (8 == pMe->prof.num_pmu_counters)
            {
               // Initialize PMU event counters to zero
               qurt_pmu_set(QURT_PMUCNT4, 0x0);
               qurt_pmu_set(QURT_PMUCNT5, 0x0);
               qurt_pmu_set(QURT_PMUCNT6, 0x0);
               qurt_pmu_set(QURT_PMUCNT7, 0x0);
               qurt_pmu_set(QURT_PMUEVTCFG1, get_next_pmu_events_to_cfg(&pMe->prof));
            }
            // enable PMU
            qurt_pmu_enable(1);
         }

         /* set up timer */
         qurt_elite_timer_attr_t attr;
         memset(&attr,0,sizeof(qurt_elite_timer_attr_t));
         attr.is_deferrable_group = FALSE;
         result = qurt_elite_timer_create2(&(pMe->prof.profileTimer),
               QURT_ELITE_TIMER_PERIODIC, &attr, QURT_ELITE_TIMER_USER,
               &(pMe->avcsTimerSignal));

         qurt_elite_timer_periodic_start(&(pMe->prof.profileTimer), sampling_period);

         pMe->prof.queryClkInterval = sampling_period>AVS_PROFILING_CLOCK_QUERY_MIN_INTERVAL?1:(AVS_PROFILING_CLOCK_QUERY_MIN_INTERVAL/sampling_period);

         MSG(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AdspCoreSvc: Profiling started");
      }

      /* else if profiling already on, return error */
      else
      {
         result = ADSP_EALREADY;
      }

      break;
   }
   default:
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: Invalid profiling level");
      result = ADSP_EBADPARAM;
      break;
   }


   {
      if (ADSP_FAILED(result = elite_apr_if_end_cmd(pMe->aprHandle, pPacket, result)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: PerfMon request ACK failed  = %d!", result);
      }
   }
}

void AVS_AdsppmVoteDone(AvsCoreSvc_t *pMe, elite_apr_packet_t* pPacket)
{
   cvd_event_adsppm_vote_done_t *pPayload = (cvd_event_adsppm_vote_done_t*)elite_apr_if_get_payload_ptr(pPacket);
   avcs_component_adsppm_voting_info_t *pDest = &pMe->prof.VOC;
   cvd_event_adsppm_vote_done_t *pSrc = pPayload;

   pDest->mask |= pSrc->mask;

   if ( (pSrc->mask&CVD_COMPONENT_ADSPPM_VOTING_MASK__MIPS_CHANGED_MASK) == CVD_COMPONENT_ADSPPM_VOTING_MASK__MIPS_CHANGED_MASK)
   {
      pDest->mips_per_thread = pSrc->mips_per_thread;
      pDest->total_mips = pSrc->total_mips;
   }
   if ( (pSrc->mask&CVD_COMPONENT_ADSPPM_VOTING_MASK__ADSP_DDR_BW_CHANGED_MASK) == CVD_COMPONENT_ADSPPM_VOTING_MASK__ADSP_DDR_BW_CHANGED_MASK)
   {
      pDest->ib_adsp_ddr_Bps_lsw = pSrc->ib_adsp_ddr_Bps_lsw;
      pDest->ib_adsp_ddr_Bps_msw = pSrc->ib_adsp_ddr_Bps_msw;
      pDest->ab_adsp_ddr_Bps_lsw = pSrc->ab_adsp_ddr_Bps_lsw;
      pDest->ab_adsp_ddr_Bps_msw = pSrc->ab_adsp_ddr_Bps_msw;
   }

   //ideally we should look into src addr to determine if its voice. but since we dont have any alternative svc which do this, we are fine.
}

void AdspCoreSvc_GetCommonProfInfoCmdHandler(AvsCoreSvc_t *pMe, elite_apr_packet_t* pAprPacket)
{
   elite_apr_packet_t *pPacket = NULL;
   int32_t      rc = APR_EOK;

   memset(&pMe->prof, 0, sizeof(AvsCoreSvc_Prof_t));

   /* Allocate the return packet */
   if (APR_EOK != (rc = elite_apr_if_alloc_cmd_rsp(
         pMe->aprHandle,
         elite_apr_if_get_dst_addr( pAprPacket),
         elite_apr_if_get_dst_port( pAprPacket),
         elite_apr_if_get_src_addr( pAprPacket),
         elite_apr_if_get_src_port( pAprPacket),
         elite_apr_if_get_client_token( pAprPacket),
         AVCS_CMDRSP_GET_COMMON_PROF_INFO,
         sizeof(avcs_cmdrsp_get_common_prof_info_t),
         &pPacket )))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: GetCommonProfInfo Response packet allocation failed  = %ld!", rc);
      rc = ADSP_ENOMEMORY;
      goto GetCommonProfInfo_error_1;
   }
   if (NULL == pPacket)
   {
      MSG(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: GetCommonProfInfo Response packet allocation failed. pPacket is NULL");
      rc = ADSP_ENOMEMORY;
      goto GetCommonProfInfo_error_1;
   }

   /* populate payload */
   avcs_cmdrsp_get_common_prof_info_t *pPayload;
   void *pDummy;
   elite_apr_if_get_payload((&pDummy), pPacket);
   pPayload = (avcs_cmdrsp_get_common_prof_info_t *)pDummy;

   qurt_arch_version_t archver;
   qurt_sysenv_get_arch_version(&archver);
   pPayload->q6_arch_rev = archver.arch_version;
   pMe->prof.q6_arch_version = archver.arch_version;

   qurt_sysenv_max_hthreads_t maxhwt;
   qurt_sysenv_get_max_hw_threads(&maxhwt);
   pPayload->tot_num_hw_threads = maxhwt.max_hthreads;
   pMe->prof.tot_num_hw_threads = maxhwt.max_hthreads;

   pPayload->status = ADSP_EOK;

   /* send response */
   if (APR_EOK != (rc = elite_apr_if_async_send(pMe->aprHandle, pPacket)))
   {
      MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AvsCoreSvc: GetCommonProfInfo Response send failed  = %ld!", rc);
      rc = ADSP_EFAILED;
      goto GetCommonProfInfo_error_2;
   }
   /* free the original packet */
   (void)elite_apr_if_free( pMe->aprHandle, pAprPacket);

   MSG_2(MSG_SSID_QDSP6, DBG_HIGH_PRIO, "AdspCoreSvc_GetCommonProfInfoCmdHandler exit with arch 0x%lx, threads=%lu",
         archver.arch_version, maxhwt.max_hthreads);

   return;

   GetCommonProfInfo_error_2:
   (void) elite_apr_if_free(pMe->aprHandle, pPacket);
   GetCommonProfInfo_error_1:
   (void)elite_apr_if_end_cmd(pMe->aprHandle, pAprPacket, rc);
}

void AdspCoreSvc_ProcessPerfMon(AvsCoreSvc_t *pMe)
{
   ADSPResult result = ADSP_EOK;
   elite_apr_packet_t* pPacket = NULL;

   /* In case of an extra timer signal just after turning off profiling, simply exit. */
   if (AVS_PROFILING_OFF == pMe->prof.uProfilingLevel)
      return;

   avcs_profiling_info_t *pPayload = (avcs_profiling_info_t*)pMe->prof.perfMonEvtPingVirtAddr;
   pPayload = (avcs_profiling_info_t*)((char*)pPayload + pMe->prof.numProfSetInBatch*sizeof_one_event(pMe));

   process_each_event(pMe, pPayload);

   pMe->prof.numProfSetInBatch++;
   pMe->prof.periodCounter++; //never reset.

   //send the event if either memory is full or if max batch time is reached.
   if ( pMe->prof.totNumProfSetsInBatch == pMe->prof.numProfSetInBatch )
   {
      // need to send APR event for every profiling event
      result = elite_apr_if_alloc_event(pMe->aprHandle, pMe->prof.perfMonEvtSrcAddr,
            0, pMe->prof.perfMonEvtDstAddr, pMe->prof.perfMonEvtDstPort, 0,
            AVCS_EVENT_PERIODIC_PROFILING,
            sizeof(avcs_event_periodic_profiling_t), &pPacket);
      if (ADSP_FAILED(result) || (NULL == pPacket))
      {
         /* In case of failure to allocate event packet, just skip this update and continue */
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AVSCoreSvc: Could not allocate perfmon event packet = %d!", result);
         return;
      }

      avcs_event_periodic_profiling_t *pEvtPayload = (avcs_event_periodic_profiling_t*)elite_apr_if_get_payload_ptr(pPacket);
      uint64_t addr = (((uint64_t)pMe->prof.perfMonEvtPingPhyAddrMsw)<<32)|((uint64_t)pMe->prof.perfMonEvtPingPhyAddrLsw);
      if (pMe->prof.perfMonEvtPingVirtAddr > pMe->prof.perfMonEvtPongVirtAddr) //then ping buf is referring to second part.
      {
         addr += pMe->prof.perfMonEvtTotalSize/2;
      }
      pEvtPayload->physical_address_lsw = (uint32_t)addr;
      pEvtPayload->physical_address_msw = (uint32_t)(addr>>32);

      pEvtPayload->num_profiling_events_in_batch = pMe->prof.numProfSetInBatch;
      pEvtPayload->size_of_one_item = sizeof_one_event(pMe);

      elite_apr_if_set_payload_size(pPacket, sizeof(avcs_event_periodic_profiling_t));

      /* send the apr event */
      if (ADSP_FAILED(result = elite_apr_if_async_send(pMe->aprHandle, pPacket)))
      {
         MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: Failed to send perfmon event = %d!", result);
         if (ADSP_FAILED(result = elite_apr_if_free(pMe->aprHandle, pPacket)))
         {
            MSG_1(MSG_SSID_QDSP6, DBG_ERROR_PRIO, "AdspCoreSvc: Freeing Packet failed = %d!", result);
         }
      }

      pMe->prof.numProfSetInBatch = 0;
      //switch ping and pong after the event
      uint32_t temp = pMe->prof.perfMonEvtPingVirtAddr;
      pMe->prof.perfMonEvtPingVirtAddr = pMe->prof.perfMonEvtPongVirtAddr;
      pMe->prof.perfMonEvtPongVirtAddr = temp;
   }
}
