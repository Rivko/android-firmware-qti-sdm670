/*==============================================================================
  FILE:         sleepActive.c
  
  OVERVIEW:     This file implements the Active Time solver thread and the 
                interfaces associated with it.
  
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/active/sleepActive.c#5 $
$DateTime: 2018/01/18 23:39:29 $
===============================================================================*/
#include "comdef.h"
#include "npa.h"
#include "rcinit.h"
#include "sleepActive.h"
#include "sleepActivei.h"
#include "sleep_log.h"
#include "sleep_solveri.h"
#include "sleep_target.h"
#include "qurt_signal.h"
#include "CoreVerify.h"
#include "sleep_osi.h"
#include "sleep_npa_scheduler.h"
#include "timer.h"
#include "sleep_npa.h"
#include "sleep_npai.h"
#include "rpmh_client.h"
#include "sleep_statsi.h"

/*==============================================================================
                                 CONSTANTS
  =============================================================================*/
/* Active Time Solver depends on being able to query 
 * following nodes for evaluating low-power modes */
static const char* sleep_active_deps[] = 
{
  SLEEP_MAX_DURATION_NODE_NAME,
  SLEEP_NORMAL_LATENCY_NODE_NAME,
#ifdef USLEEP_ISLAND_MODE_ENABLE
  SLEEP_ISLAND_LATENCY_NODE_NAME,
#endif
  "/clk/cpu"
};

/*==============================================================================
                              EXTERNAL VARIABLES
  =============================================================================*/
extern sleepStats_timestamps  g_wakeupTimestamps;
extern qurt_anysignal_t       g_pmiSignal;

/*==============================================================================
                              INTERNAL VARIABLES
  =============================================================================*/
/* Time a mode was last profiled */
static uint64                 g_lastProfileTime = 0;

/* ATS thread stack */
static uint8                g_sleepSolverTaskStack[SLEEPSOLVER_STACK_SIZE];

/* Signal Object used for communicating with Active Time solver */
static qurt_signal_t        g_sleepActiveSigObj;

/* State of Solver Inputs */
static sleep_solver_input   g_solverInput;

/* Last solver output selected by Active Time solver */
static sleep_solver_output  g_lastSolverOutput = {0};

/* Query handle for the current state of max_duation node */
static npa_query_handle     g_npaMaxDurationHandle;

/* Query handles for the current state of latency nodes */
static npa_query_handle     g_npaNormalLatencyHandle;

#ifdef USLEEP_ISLAND_MODE_ENABLE
static npa_query_handle     g_npaIslandLatencyHandle;
#endif

/* Event handle for changes in "/clk/cpu" state */
static npa_event_handle     g_npaFreqChangeEventHandle;

/* Threshold timer object. */
static timer_type           g_sleepActiveThresholdTimer;

/* Handle to the RPM LPR node */
static npa_client_handle    g_rpmhSyncHandle = NULL;

/*==============================================================================
                           STATIC FUNCTION DEFINITIONS
  =============================================================================*/
/** 
 * sleepActive_UpdateFreqLUT
 * 
 * @brief Update the Frequency LUT for statistics profiling 
 */
static void sleepActive_UpdateFreqLUT(void)
{
  g_solverInput.last_cpu_freq_khz = sleepOS_getFrequency();

  if( TRUE != sleepStats_getTableEntry(g_solverInput.last_cpu_freq_khz,
                                       &g_solverInput.fLUT))
  {
    /* If the function returns !TRUE, we need to allocate a new fLUT and 
     * realloc all of the cache & statistic arrays */
    g_solverInput.fLUT =
        sleepStats_addFrequencyTable(g_solverInput.fLUT,
                                     g_solverInput.last_cpu_freq_khz);
  }

  return;
}

/** 
 * sleepActive_threshTimerCB 
 *  
 * @brief Threshold timer callback function. It sets signals for durations
 *        so that ATS can re-solve low power modes with updated durations. 
 *
 * @param data: Callback context data (Unused - for prototype compliance)
 *
 * @note There seems a discrepancy about context type. Timer definition 
 *       expects context of time_osal_notify_data type whereas callback
 *       expects timer_cb_data_type. While both maps to 'unsigned long',
 *       it can lead to error.
 */
static void sleepActive_threshTimerCB(timer_cb_data_type data)
{
  /* Signal a hard duration change */
  sleepActive_SetSignal(SLEEP_SIGNAL_THRESHOLD_EXPIRED);
  return;
}

/** 
 * sleepActive_setThresholdTimer 
 *  
 * @brief Sets the threshold timer which will expire at the deadline provided
 *        by solver. It is expected to be called after solver has finished.
 *
 * @param output: Output object used while calling solver.
 */
static void sleepActive_setThresholdTimer(sleep_solver_output *output)
{
  if((output->threshold_deadline != 0) &&
     (output->threshold_deadline != output->ref_deadline))
  {
    /* We have valid threshold deadline and there is a possibility of 
     * different mode becoming optimal as duration reduces. */
    timer_set_absolute(&g_sleepActiveThresholdTimer,
                       output->threshold_deadline);

    sleepLog_printf(SLEEP_LOG_LEVEL_ATS, (1*2),
                    "Threshold timer set (Deadline: 0x%llx)",
                    ULOG64_DATA(output->threshold_deadline));
  }
  else
  {
    sleepActive_clearThreasholdTimer();
  }

  return;
}

/** 
 * sleepActive_TriggerSolver 
 *  
 * @brief Execute the Active Time solver and determine whether mode selection
 *        has changed
 *
 *        Performs the following operations:
 *        1. Execute solver based on last updated solver input
 *        2. If mode selection results in the same mode,  configure reentrant
 *           component modes.
 *        3. If mode selection results in a Cached Mode then unconfigure
 *           previously configured cached modes and configure the newly selected
 *           cached ones.
 *        4. If mode selection results in Uncached Mode then 
 *           unconfigure previous configured cached modes
 *        5. Calls sleepOS_ConfigIdleMode to set the appropriate entry 
 *           depending on whether the selected mode is cached or uncached
 * 
 */
static solver_status sleepActive_TriggerSolver(void)
{
  sleep_solver_output solverOutput;
  solver_status       solverStatus;
  uint32              backoffValue    = 0;
  sleep_synth_lprm    *lastSynthMode  = g_lastSolverOutput.selected_synthLPRM;
  
  /* Run the main solver to select a mode to enter */
  solverStatus = sleepSolver_solveMode(&g_solverInput, &solverOutput);

  sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_ATS, SLEEP_EXIT_SOLVER_NUM_ARGS,
                      SLEEP_EXIT_SOLVER_STR, SLEEP_EXIT_SOLVER);

  if(SOLVER_DEADLINE_IN_PAST == solverStatus)
  {
    /* Stop processing if wakeup time isn't valid */
    return solverStatus;
  }

  /* Update the last selected solver output data */
  g_lastSolverOutput = solverOutput;
  
  /* Set a threshold timer in case this selected mode becomes stale. */
  sleepActive_setThresholdTimer(&solverOutput);
    
  /* Set the appropriate backoff value used for entering the component modes below */
  if(NULL != solverOutput.backoff_ptr)
  {
    /* Get the normal mode backoff value */
    backoffValue = *solverOutput.backoff_ptr;
  }

  if(NULL != lastSynthMode)
  {
    /* If solver selected the same synth mode, we need to enter the reentrant component modes */
    if(lastSynthMode == solverOutput.selected_synthLPRM)
    {
      synthLPRM_cachedModeEnter(solverOutput.selected_synthLPRM,
                                g_solverInput.hard_deadlines.overall_min - backoffValue,
                                TRUE);
      return solverStatus;
    }

    /* Transition across Synth Modes if solver output has changed */
    if((TRUE == synthLPRM_isAttributeSet(lastSynthMode, SYNTH_MODE_ATTR_LPI, FALSE)) ||
       (lastSynthMode->cacheable.num_component_modes > 0))
    {
      sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 1, 
                      "Unconfiguring previous cached mode (name: \"%s\")",
                      lastSynthMode->name);

      /* Unconfigure previously configured Cached Synth Mode */
      synthLPRM_cachedModeExit(lastSynthMode);

      /* Unconfigure previously configured Cached Synth Mode */
      if(TRUE == synthLPRM_isAttributeSet(lastSynthMode, SYNTH_MODE_ATTR_LPI, TRUE))
      {
        /* Ensure the LPI entry function is cleared */
        sleepOS_setLPIEntryFunction(NULL);
      }
    }
  }

  /* Nothing further to do if solver was unable to select a mode. */
  if(SOLVER_SUCCESS != solverStatus)
  {
    return solverStatus;
  }

  CORE_VERIFY(solverOutput.selected_synthLPRM);

  if(solverOutput.selected_synthLPRM->cacheable.num_component_modes > 0)
  {
    /* If synth is cacheable, run its enter functions */
    sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 1,
                    "Configuring cacheable mode (Fully cacheable: %s)",
                    0 == solverOutput.selected_synthLPRM->uncacheable.num_component_modes ? "True" : "False");

    /* Enter the cacheable modes */
    synthLPRM_cachedModeEnter(solverOutput.selected_synthLPRM,
                              g_solverInput.hard_deadlines.overall_min - backoffValue,
                              FALSE);
  }
  
  return solverStatus;
}

/** 
 * sleepActive_ClearSignal 
 *  
 * @brief Clear a signal or mask of signals on Active Time solver thread
 *
 * Signals set on the Active Time solver are used to determine 
 * execution conditions that have changed. Signals should be cleared
 * as soon they are processed.
 * 
 * @param sig: signal or signal mask to be cleared on Active Time solver
 */
static SLEEP_INLINE void sleepActive_ClearSignal(uint32 sig)
{
  qurt_signal_clear(&g_sleepActiveSigObj, sig);
  return;
}

/** 
 * sleepActive_getCurrentSignal 
 *  
 * @brief Returns any currently pending signals 
 *
 * @param sigMask: Mask used to check for specific currently pending signals
 */
static uint32 sleepActive_getCurrentSignal(uint32 sigMask)
{
  uint32 sigs;

  sigs = qurt_signal_get(&g_sleepActiveSigObj);
  
  return(sigs & sigMask);
}

/** 
 * sleepActive_ProcessFreqChangeEvent 
 *  
 * @brief This Callback is triggered when "core/cpu" frequency state changes.
 *
 * @param See npa_callback type
 */
static void sleepActive_ProcessFreqChangeEvent(void         *context,
                                               unsigned int event,
                                               void         *eventData,
                                               unsigned int eventDataSize)
{
  if ( event == NPA_EVENT_CHANGE )
  {
    sleepActive_SetSignal(SLEEP_SIGNAL_CPU_FREQUENCY);
  }

  return;
}

/** 
 * sleepActive_ProcessDataSignals 
 *  
 * @brief Updates solver input based on set signals
 *
 * @param signals: Data signals that are set on Active Time Solver
 */
static void sleepActive_ProcessDataSignals(uint32 signals)
{
  uint32          queueState;
  npa_query_type  qres;
#ifdef USLEEP_ISLAND_MODE_ENABLE
  npa_query_type  islandQres;
#endif
  static boolean  lastRPMhQueueState = TRUE;

  if(signals & SLEEP_SIGNAL_NORMAL_LATENCY)
  {
    sleepActive_ClearSignal(SLEEP_SIGNAL_NORMAL_LATENCY);

    if(NULL != g_npaNormalLatencyHandle)
    {
      /* Get the latency budget from the latency node.  The latency node will
       * return the minimum latency budget being requested by all clients.
       * This lets us know how quickly we need to respond to an interrupt when
       * it occurs. */
      CORE_VERIFY(NPA_QUERY_SUCCESS == 
                  npa_query(g_npaNormalLatencyHandle, NPA_QUERY_CURRENT_STATE, &qres));

      g_solverInput.normal_latency_budget = qres.data.value;
    }
  }

  if(signals & SLEEP_SIGNAL_ISLAND_LATENCY)
  {
    sleepActive_ClearSignal(SLEEP_SIGNAL_ISLAND_LATENCY);

#ifdef USLEEP_ISLAND_MODE_ENABLE
    if(NULL != g_npaIslandLatencyHandle)
    {
      CORE_VERIFY(NPA_QUERY_SUCCESS == 
                  npa_query(g_npaIslandLatencyHandle, NPA_QUERY_CURRENT_STATE, &islandQres));

      g_solverInput.island_latency_budget = islandQres.data.value;
    }
#endif
  }

  if(signals & SLEEP_SIGNAL_SOFT_DURATION)
  {
    sleepActive_ClearSignal(SLEEP_SIGNAL_SOFT_DURATION);

    if(NULL != g_npaMaxDurationHandle)
    {
      /* Get the sleep duration from the max duration node.  This will return the
       * "soft" max duration.  Soft wakeups are those that are hinted at by other
       * subsystems.  Soft wakeups allow us to calculate which mode we will enter,
       * so we don't waste time entering a mode that we are going to have to wake
       * up from soon anyway. */
      CORE_VERIFY( NPA_QUERY_SUCCESS == 
                   npa_query(g_npaMaxDurationHandle, NPA_QUERY_CURRENT_STATE,
                             &qres));
      g_solverInput.soft_duration =  qres.data.value;
    }
  }

  if(signals & SLEEP_SIGNAL_CPU_FREQUENCY)
  {
    sleepActive_ClearSignal(SLEEP_SIGNAL_CPU_FREQUENCY);

    /* CPU Frequency changed.  Update the statistics frequency LUT */
    sleepActive_UpdateFreqLUT();
  }

  if(signals & (SLEEP_SIGNAL_HARD_DURATION | 
                SLEEP_SIGNAL_ISLAND_HARD_DURATION))
  {
    sleepActive_ClearSignal((SLEEP_SIGNAL_HARD_DURATION | 
                             SLEEP_SIGNAL_ISLAND_HARD_DURATION));

    /* First non deferrable timer expiry changed, threshold timer expired
     * or npa scheduler deadline changed - Sleep may have different hard 
     * duration. 
     *
     * Context switch could happen while calculating hard duration and we
     * may be past hard deadline - however in that case we should already
     * have another signal pending so it should be ok. */
    sleepTarget_getHardWakeupTime(&g_solverInput.hard_deadlines);
  }
  
  /* Enable/Disable the RPM LPR according to the queue status.  When the
   * NPA call is complete, ATS will be re-signaled with a registry change */
  if(signals & SLEEP_SIGNAL_RPM_QUEUE_CHANGE)
  {
    sleepActive_ClearSignal(SLEEP_SIGNAL_RPM_QUEUE_CHANGE);

    if(NULL != g_rpmhSyncHandle)
    {
      queueState = rpmh_cmdqs_empty();
      
	  sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 2,"SLEEP_SIGNAL_RPM_QUEUE_CHANGE : lastRPMhQueueState = %d, queueState = %d", lastRPMhQueueState, queueState);
      /* Only update if there is a state change */
      if(lastRPMhQueueState != queueState)
      {
        lastRPMhQueueState = queueState;

        if(FALSE == queueState)
        {
          /* If there are messages in the RPMh cmd queue, disable all RPMh power modes */
          npa_issue_required_request(g_rpmhSyncHandle, SLEEP_RPM_LPR_DISABLE_ALL);
        }
        else
        {
          /* If the queue is empty, allow all RPM power modes */
          npa_complete_request(g_rpmhSyncHandle);
        }
      }
    }
  }

  /* Nothing do do for these signals except rerun the solver */
  if(signals & (SLEEP_SIGNAL_REGISTRY             | 
                SLEEP_SIGNAL_THRESHOLD_EXPIRED    |
                SLEEP_SIGNAL_WAKE_LATENCY_CHANGED))
  {
    sleepActive_ClearSignal((SLEEP_SIGNAL_REGISTRY            | 
                             SLEEP_SIGNAL_THRESHOLD_EXPIRED   |
                             SLEEP_SIGNAL_WAKE_LATENCY_CHANGED));
  }
  
  sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 0, "Processed data signals");

  return;
}

/** 
 * sleepActive_ProcessControlSignals 
 *  
 * @brief Updates solver signal mask based on set signals
 *
 * @param signals: Control signals that are set on Active Time Solver
 *
 * @return If non-zero, the control signals to further wait on
 */
static uint32 sleepActive_ProcessControlSignals(uint32 signals)
{
  int     intPendingStatus  = 0;
  int     intEnableStatus   = 0;
  uint32  sigs;

  if(signals & SLEEP_SIGNAL_CTL_INIT_COMPLETE)
  {
    sleepActive_ClearSignal(SLEEP_SIGNAL_CTL_INIT_COMPLETE);

    /* Keep sleep in 'hold off' mode until RCinit is complete to avoid entering
     * any low power modes */
    sleepOS_configIdleMode(SLEEP_OS_IDLE_MODE_HOLD_OFF);

    sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 0, "ATS: boot complete");

    /* ATS init is done, wait for RCInit done */
    return SLEEP_SIGNAL_CTL_RCINIT_DONE;
  }

  /* Process updated Control signals, if any */
  if(signals & SLEEP_SIGNAL_CTL_RCINIT_DONE)
  {
    sleepActive_ClearSignal(SLEEP_SIGNAL_CTL_RCINIT_DONE);
    sleepOS_configIdleMode(SLEEP_OS_IDLE_MODE_DEFAULT);
    
    sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 0, "ATS: RCinit complete, allowing normal operation");

    /* Refresh any signal that became pending while rcinit was booting */
    return SLEEP_SIGNAL_MASK_STANDARD;
  }
  
  /* Few posibilities here:
   * 
   * HW will automatically clear pending, and disable when it is reconized.
   * 
   * 1. If ISR has asserted signal, then there is no issue and we can process it normally
   *  
   * 2. If int is disabled, then HW has gotten to it, and ISR may or may not have asserted the signal.
   *    We can process now and wait for ISR to finish
   * 
   * 3. If int is pending then HW has not gotten to it yet.
   *    We can process now and wait for ISR to run & finish.
   * 
   * 4. If int is not pending, enabled, and signal is not asserted, then there is nothing to 
   *    do - this is a normal data signal restriction change
   */ 
  if(0 == (signals & SLEEP_SIGNAL_CTL_PERFORM_PROFILE))
  {
    qurt_interrupt_get_status(SLEEP_PROFILING_ID, 0, &intPendingStatus);
    qurt_interrupt_get_status(SLEEP_PROFILING_ID, 1, &intEnableStatus);

    if((1 == intPendingStatus) || (0 == intEnableStatus))
    {
      /* If interrupt is pending, we know ISR has not run, or is in the middle of running.
       * Wait for the ISR to assert the signal to process it. */
      sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 0, "ATS: Waiting for profiling signal");
      return SLEEP_SIGNAL_CTL_PERFORM_PROFILE;
    }
  }

  /* Reread the perform signal here to close the window where sleepsolver has been preempted
   * after getting original signal set and PMI handler has asserted it, or it was bypass asserted
   * above. */
  if(sleepActive_getCurrentSignal(SLEEP_SIGNAL_CTL_PERFORM_PROFILE))
  {
    /* Record the time of last profile to ensure it's not duplicated */
    g_lastProfileTime = CoreTimetick_Get64();

    /* Perform all the profiling for the last power collapse cycle */
    sleepStats_logWakeup();

    do
    {
      qurt_interrupt_get_status(SLEEP_PROFILING_ID, 0, &intPendingStatus);
      sigs = qurt_anysignal_get(&g_pmiSignal);
    }while((1 == intPendingStatus) || (0 != sigs));

    sleepActive_ClearSignal(SLEEP_SIGNAL_CTL_PERFORM_PROFILE);

    /* Refresh any signal that became pending while profiling was processing */
    return SLEEP_SIGNAL_MASK_STANDARD;
  }

  /* Allow standard data signal processing */
  return 0;
}

/** 
 * sleepActive_WaitforSignal 
 *  
 * @brief Waits for any signal in the mask to assert, and returns the pending signals
 * 
 * @param sigMask: Mask used to control whether Active Time solver is
 */
static SLEEP_INLINE uint32 sleepActive_WaitforSignal(uint32 sigMask)
{
  return(qurt_signal_wait(&g_sleepActiveSigObj, sigMask, QURT_SIGNAL_ATTR_WAIT_ANY));
}

/**
 * sleepActive_rpmNodeAvailableCb
 *
 * @brief Obtains the client handle for "/core/cpu/rpm_sync" node when it
 *        becomes available.
 */
static void sleepActive_rpmNodeAvailableCb(void         *context,
                                           unsigned int eventType,
                                           void         *data,
                                           unsigned int dataSize)
{
  CORE_VERIFY_PTR(g_rpmhSyncHandle = npa_create_sync_client(SLEEP_RPM_LPR_NODE_NAME,
                                                            "/sleep/RPMhQueue",
                                                            NPA_CLIENT_REQUIRED));
  return;
}

/** 
 * sleepActive_ResourceAvailCB 
 *  
 * @brief Called when dependencies of Active Time Solver become available 
 */
static void sleepActive_ResourceAvailCB(void          *context, 
                                        unsigned int  event, 
                                        void          *resources,
                                        unsigned int  resourcesSize)
{
  /* Initialize Query Handles */
  CORE_VERIFY_PTR(g_npaMaxDurationHandle =
                  npa_create_query_handle(SLEEP_MAX_DURATION_NODE_NAME));

  CORE_VERIFY_PTR(g_npaNormalLatencyHandle =
                  npa_create_query_handle(SLEEP_NORMAL_LATENCY_NODE_NAME));

#ifdef USLEEP_ISLAND_MODE_ENABLE
  CORE_VERIFY_PTR(g_npaIslandLatencyHandle =
                  npa_create_query_handle(SLEEP_ISLAND_LATENCY_NODE_NAME));
#endif

  CORE_VERIFY_PTR(g_npaFreqChangeEventHandle =
                  npa_create_event_cb("/clk/cpu",
                                      "ATS Input",
                                      NPA_TRIGGER_CHANGE_EVENT,
                                      sleepActive_ProcessFreqChangeEvent,
                                      NULL));

  /* Trigger the inputs corresponding to the available nodes and signal
   * ATS boot as completed */
  sleepActive_SetSignal(SLEEP_SIGNAL_SOFT_DURATION      |
                        SLEEP_SIGNAL_NORMAL_LATENCY     |
                        SLEEP_SIGNAL_ISLAND_LATENCY     |
                        SLEEP_SIGNAL_CPU_FREQUENCY      |
                        SLEEP_SIGNAL_CTL_INIT_COMPLETE);

  return;
}

/*==============================================================================
                            GLOBAL FUNCTION DEFINITIONS
  =============================================================================*/
/* 
 * sleepActive_GetSolverOutput
 */
sleep_solver_output *sleepActive_GetSolverOutput(void)
{
  return(&g_lastSolverOutput);
}

/* 
 * sleepActive_GetSolverInput 
 */
sleep_solver_input *sleepActive_GetSolverInput(void)
{
  return(&g_solverInput);
}

/*
 * sleepActive_GetSolverHardDeadlines
 */
const sleep_solver_deadlines *sleepActive_GetSolverHardDeadlines(void)
{
  return(&g_solverInput.hard_deadlines);
}

/* 
 * sleepActive_SetSignal 
 */
void sleepActive_SetSignal(sleepActive_SignalType signals)
{
  qurt_signal_set(&g_sleepActiveSigObj, (uint32)signals);
}

/**
 * @brief Processing loop for Active Time Solver.
 *
 * Executes from the "sleepsolver" thread context
 */
static void sleepActive_Thread(void *unused)
{
  solver_status solverStatus;
  uint32        sigMask = SLEEP_SIGNAL_CTL_INIT_COMPLETE; /* Initially wait for ATS init complete */
  uint32        signals = 0;

  while(1)
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 1,
                    "ATS: waiting for signals: 0x%x", 
                    sigMask);

    signals = sleepActive_WaitforSignal(sigMask);

    sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_ATS, SLEEP_ATS_SIGNALS_NUM_ARGS, 
                        SLEEP_ATS_SIGNALS_STR, SLEEP_ATS_SIGNALS,
                        signals);
    
    /* Process and Clear Control signals, if any */
    sigMask = sleepActive_ProcessControlSignals(signals);

    /* Skip further processing of Data Signals if Control Signals
     * cause Active Time solver to be disabled. */
    if(0 != sigMask)
    {
      continue;
    }

    /* If control signals did not provide a specific mask, listen for all standard signals */
    sigMask = SLEEP_SIGNAL_MASK_STANDARD;
    
    /* Set default safe power mode to enter should the system go idle in the critical
     * section of processing any restriction changes */
    sleepTarget_setDefaultPowerMode(TRUE);

    /* Verify the profiling logic worked correctly between the solver and isr threads */
    //CORE_VERIFY(g_wakeupTimestamps.rsc_child[STATS_RSC_PROFILE_UP_END].timeStamp <= g_lastProfileTime);
    /* HW Workaround: Use time put in PMI handler since timestamp can be overwriten by RSCc
     * software sequence trigger */
    CORE_VERIFY(sleepStats_getLprTimeData(SLEEP_STATS_TIME_STM_EXIT_TYPE) <= g_lastProfileTime);

    do
    {
      /* Process updated input signals */
      sleepActive_ProcessDataSignals(signals);

      /* Check for any pending signals that could have been asserted while ATS is running
       * and update data now to avoid running solver more than necessary. */
      signals = sleepActive_getCurrentSignal(SLEEP_SIGNAL_MASK_ALL_DATA);

      if(0 == signals)
      {
        /* If no signals have been triggerd, invoke the solver with updated input.
         * Solver will configure the appropriate idle mode if required. */
        solverStatus = sleepActive_TriggerSolver();

        if(SOLVER_DEADLINE_IN_PAST == solverStatus)
        {
          /* Current deadline is in the past, listen for timer based events and any frequency
           * change as that could change LUT's */
          sigMask = SLEEP_SIGNAL_HARD_DURATION        | 
                    SLEEP_SIGNAL_SOFT_DURATION        | 
                    SLEEP_SIGNAL_ISLAND_HARD_DURATION |
                    SLEEP_SIGNAL_CPU_FREQUENCY;

          sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 0, 
                          "Wakeup deadline in the past, leaving default mode enabled");
          break;
        }
        else if(SOLVER_LATENCY_RESTRICTED == solverStatus)
        {
          /* Solver is explicitly latency restricted, listen for latency update events */
          sigMask = SLEEP_SIGNAL_ISLAND_LATENCY   | 
                    SLEEP_SIGNAL_NORMAL_LATENCY;

          sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 0, 
                          "Latency restricted, leaving default mode enabled");
          break;
        }
        else if(SOLVER_REGISTRY_DISABLED == solverStatus)
        {
          /* Registry is disabled */
          sigMask = SLEEP_SIGNAL_REGISTRY;

          sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 0, 
                          "Registry disabled, leaving default mode enabled");
          break;
        }

        /* Depending on solver behavior, certain signals may have been set, so
         * check for any pending ones before exiting processing loop. */
        signals = sleepActive_getCurrentSignal(SLEEP_SIGNAL_MASK_ALL_DATA);

        if((0 == signals) && (SOLVER_NO_MODE_CHOSEN == solverStatus))
        {
          /* Nothing further to do if solver was unable to select a mode. */
          sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 0, "No mode chosen, leaving default mode enabled");
        }
      }

      if(signals)
      {
        sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 1,
                        " ATS: solver bypass (New signals: 0x%x)",
                        signals);
      }
      else
      {
        /* Unconfigure the default mode which will reconfigure for the currently selected mode */
        sleepTarget_setDefaultPowerMode(FALSE);
      }
    }while(signals);

  } /* while (1) */
}

/*
 * sleepActive_setRPMQueueStatus
 */
void sleepActive_setRPMQueueStatus(boolean isEmpty)
{
  /* Change RPM LPR status if we are at active time, and have a state change from the
   * RPM driver */
  if(FALSE == sleepOS_isProcessorInSTM())
  {
    sleepActive_SetSignal(SLEEP_SIGNAL_RPM_QUEUE_CHANGE);
  }

  return;
}

/*
 * sleepActive_configureIdleMode
 */
void sleepActive_configureIdleMode(sleep_synth_lprm *synthLPRM)
{
  sleepOS_IdleModeType mode = SLEEP_OS_IDLE_MODE_DEFAULT;

  CORE_VERIFY_PTR(synthLPRM);

  /* Configure sleep task to run the correct function */
  if(TRUE == synthLPRM_isAttributeSet(synthLPRM, SYNTH_MODE_ATTR_LPI, TRUE))
  {
    /* Set OS Idle Mode for LPI mode
     * The LPI attribute takes priority over the cacheable attribute */
    mode = SLEEP_OS_IDLE_MODE_LPI;
  }
  else if(synthLPRM->uncacheable.num_component_modes == 0)
  {
    /* Configure OS Idle Mode for Cached LPR mode */
    mode = SLEEP_OS_IDLE_MODE_OVERRIDE;
  }

  sleepLog_printf(SLEEP_LOG_LEVEL_ATS, 1,
                  " ATS: config idle mode (Mode: %d)",
                  mode);

  sleepOS_configIdleMode(mode);

  return;
}

/*
 * sleepActive_clearThreasholdTimer
 */
void sleepActive_clearThreasholdTimer(void)
{
  uint64 remainingTime = timer_clr_64(&g_sleepActiveThresholdTimer, T_TICK);

  if(remainingTime)
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_ATS, (1*2),
                    "Stoping threshold timer (Remaining: 0x%llx)",
                    ULOG64_DATA(remainingTime));
  }

  return;
}

/** 
 * sleepActive_Init 
 *  
 * @brief Initializes the Active Time Solver 
 *        Required to be called as part of rcinit. 
 */
void sleepActive_Init(void)
{
  qurt_thread_t       solverHandlerThread;
  qurt_thread_attr_t  solverHandlerAttr;

  /* Setup Solver Input Defaults */
  g_solverInput.fLUT                    = 0;
  g_solverInput.normal_latency_budget   = 1;
  g_solverInput.island_latency_budget   = UINT32_MAX;
  g_solverInput.soft_duration           = UINT64_MAX;

  memset(&g_solverInput.hard_deadlines, -1, sizeof(g_solverInput.hard_deadlines));

  /* Initialize signal type */
  qurt_signal_init(&g_sleepActiveSigObj);

  /* Defining threshold timer. */
  CORE_VERIFY( TE_SUCCESS == 
               timer_def_osal(&g_sleepActiveThresholdTimer,  /* Timer obj */
                              &timer_null_group,             /* Timer group */
                              TIMER_FUNC1_CB_TYPE,           /* Notify type */
                              sleepActive_threshTimerCB,     /* Callback fcn */
                              (time_osal_notify_data)0));    /* Context */

  /* Register to receive signal when Hard Duration is updated */
  CORE_VERIFY(TE_SUCCESS == timer_register_for_non_defer_expiry_change(
                              TIMER_NATIVE_OS_SIGNAL_TYPE,
                              &g_sleepActiveSigObj,
                              SLEEP_SIGNAL_HARD_DURATION));

  /* Manually trigger Active time solver to qury hard duration */
  sleepActive_SetSignal(SLEEP_SIGNAL_HARD_DURATION); 
                  
  /* Register to Callback when resources becomes available */
  npa_resources_available_cb(NPA_ARRAY(sleep_active_deps),
                             sleepActive_ResourceAvailCB,
                             NULL);

  npa_resource_available_cb(SLEEP_RPM_LPR_NODE_NAME,
                            sleepActive_rpmNodeAvailableCb, NULL);

  /* Start the ATS thread */
  qurt_thread_attr_init(&solverHandlerAttr);

  /* setting up attributes for the solver thread */
  qurt_thread_attr_set_name(&solverHandlerAttr, "sleepsolver");
  qurt_thread_attr_set_priority(&solverHandlerAttr, 254);

  /* Stack memory has to be statically allocated due to island section location requirement */
  qurt_thread_attr_set_stack_size(&solverHandlerAttr, SLEEPSOLVER_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&solverHandlerAttr, (void *)g_sleepSolverTaskStack);
  
  /* Fill in the stack with a marker to try and find overflows */
  memset(g_sleepSolverTaskStack, 0xF8, sizeof(uint8) * SLEEPSOLVER_STACK_SIZE);

  /* Create the main sleep task in island section memory */
  CORE_VERIFY(QURT_EOK == qurt_thread_create(&solverHandlerThread, &solverHandlerAttr,
                                             sleepActive_Thread, NULL));

  return;
}

