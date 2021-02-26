/*==============================================================================
  FILE:         sleep_target.c

  OVERVIEW:     This file provides target-specific functionality for the
                subsystem.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/target/sleep_target.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include <stdint.h>
#include "comdef.h"
#include "sleep_target.h"
#include "timer.h"
#include "CoreTime.h"
#include "CoreVerify.h"
#include "sleep_log.h"
#include "npa_resource.h"
#include "npa_remote_resource.h"
#include "sleep_lpri.h"
#include "DDIInterruptController.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "sleepi.h"
#include "synthTypes.h"
#include "synthLPR.h"
#include "synthLPI.h"
#include "sleep_stats_types.h"
#include "sleep_npa_scheduler.h"
#include "sleep_statsi.h"
#include "rsc.h"
#include "sleepActive.h"
#include "sleepActivei.h"
#include "sleep_solveri.h"
#include "q6_lpm_config.h"
#include "sleep_osi.h"
#include "pdc_seq.h"
#include "sleep_masterstats.h"

#ifdef USLEEP_ISLAND_MODE_ENABLE
#include "utimer.h"
#endif

/*==============================================================================
                               GLOBAL VARIABLES
 =============================================================================*/
#ifdef SLEEP_BRINGUP_DEBUG
  /* Values must be non-zero initial from rcinit */
  static volatile uint32              g_sleepDebugTimerRndMs = 0; /* Maximum random timer value in 1 ms multiple */
  static volatile time_timetick_type  g_sleepDebugTimerUs    = 0; /* Creates a period debug timer in uSec */

  /* Debug timer client*/
  timer_type          g_sleepDebugTimerClient;
#endif

/* Sleep Target configuration parameters */
sleep_target_config g_sleepTargetConfig = 
{
  .chip_family    = DALCHIPINFO_FAMILY_UNKNOWN,
  .chip_ver       = 0,
#ifdef SLEEP_IMAGE_SLPI
  .l2_mask        = 0xfc,
  .tcm_mask       = 0x01,
#else
  .l2_mask        = 0xFFFF,
  .tcm_mask       = 0x0000,
#endif
};

/* Variable for indication of entry to RPM assisted modes */
static boolean          g_synthLPRMisRPMhAssisted = FALSE;

/* Dependencies to wait on before registering any LPRs. */
static const char       *g_LPRDependencies[] = {SLEEP_LPR_NODE_NAME};

/* Dal data struct to hold DAL Interrupt Controller Handle */
static DalDeviceHandle  *g_DALInterruptCtrlHndl;

/* Handle to the DAL "WakeupTimer" timetick driver used to provide the local wakeup timer */
static DalDeviceHandle  *g_DALWakeTimerHandle;

/*==============================================================================
                           EXTERNAL REFERENCES
 =============================================================================*/
/* deferrable timer groups */
extern timer_group_type timer_null_group;

/* target-specific nodes that are implemented & initialized in this module */
extern sleep_synth_lpr SleepLPR_synth;
extern sleep_synth_lpi SleepLPI_synth;

extern npa_node_definition g_cpuVddNode;
extern npa_node_definition g_rpmNode;
extern npa_node_definition g_l2CacheNode;
extern npa_node_definition g_pdcNode;
extern npa_node_definition g_rscpNode;

/*==============================================================================
                           FORWARD DECLARATIONS
 =============================================================================*/
void sleepTarget_wakeupHandler(uint32 param);
void sleepTarget_aopWakeupHandler(uint32 param);
void sleepOS_profilingHandler(uint32 param);

static uint64 sleepTarget_enableWakeupTimer(uint64 wakeupTick);
static void sleepTarget_disableWakeupTimer(void);

/*==============================================================================
                          INTERNAL FUNCTION DECLARATIONS
 =============================================================================*/
#ifdef SLEEP_BRINGUP_DEBUG
/** 
 * sleepTarget_debugTimerCB 
 *  
 * @brief  ISR for debug timer
 */
static void sleepTarget_debugTimerCB(int data)
{
  uint32 rndVal;
  uint64 timerValue = g_sleepDebugTimerUs;

  if(g_sleepDebugTimerUs || g_sleepDebugTimerRndMs)
  {
    if(g_sleepDebugTimerRndMs)
    {
      rndVal      = rand();
      timerValue  = (rndVal % 1000);
      timerValue  = (timerValue * g_sleepDebugTimerRndMs) + 1; //1uS to X ms
    }

    timer_set_64(&g_sleepDebugTimerClient, timerValue, 0, T_USEC);
  }
  else
  {
    timer_clr_64(&g_sleepDebugTimerClient, T_USEC);
  }
  return;
}
#endif /* SLEEP_BRINGUP_DEBUG */

/** 
 * sleepTarget_registerLPRNode 
 *  
 * @brief  Helper function to register and publish the internally control LPR nodes
 */
static void sleepTarget_registerLPRNode(npa_node_definition *node, 
                                        npa_resource_state  initialState)
{
  uint32  cnt;

  /* Define the node */
  npa_define_node(node, &initialState, NULL);

  /* Publish node to any user PD's */
  for(cnt=0; cnt < node->resource_count; cnt++)
  {
    /* This call is only valid at init time, before any user PD's are started */
    npa_remote_publish_resource(NPA_ALL_PDS, node->resources[cnt].name);
  }

  return;
}

/** 
 * sleepTarget_registerLPRs 
 *  
 * @brief  Register the LPRs & nodes that are owned by sleep.
 */
static void sleepTarget_registerLPRs(void         *context,
                                     unsigned int eventType,
                                     void         *data,
                                     unsigned int dataSize)
{
  /* Register sleep owned LPR's that have their own NPA node drivers.
   * The LPR will be registered as part of the node init process. */
  sleepTarget_registerLPRNode(&g_cpuVddNode,   SLEEP_CPUVDD_LPR_ALLOW_ALL);
  sleepTarget_registerLPRNode(&g_rpmNode,      SLEEP_RPM_LPR_ALLOW_ALL);
  sleepTarget_registerLPRNode(&g_l2CacheNode,  SLEEP_L2_CACHE_LPR_ALLOW_ALL);
#ifndef SLEEP_IMAGE_SLPI
  /* SLPI has a hardware solver that will select deeper RPMh assisted modes */
  sleepTarget_registerLPRNode(&g_pdcNode,      SLEEP_PDC_LPR_ENABLE_ALL);
  sleepTarget_registerLPRNode(&g_rscpNode,     SLEEP_RSCP_LPR_ENABLE_ALL);
#endif

  /* Register & enable sleep owned NPA nodeless LPR's */
  sleepLPR_registerNodelessLPR("wakeup",      SLEEP_ALL_LPRMS);
  sleepLPR_registerNodelessLPR("cpu_config",  SLEEP_ALL_LPRMS);

  return;
}

/*==============================================================================
                          EXTERNAL FUNCTION DECLARATIONS
 =============================================================================*/
/**
 * sleepTarget_initialize
 */
void sleepTarget_initialize(void)
{
  uint32  timerIntN;
  uint32  islandMode  = 0;

#ifdef USLEEP_ISLAND_MODE_ENABLE
  islandMode = 1;
#endif

  /* Register a callback for when the necessary dependencies are available,
   * in order to register the nodes and LPRs that sleep owns. */
  npa_resources_available_cb(NPA_ARRAY(g_LPRDependencies),
                             sleepTarget_registerLPRs, NULL);

  /* set below groups as deferrable */
  timer_group_set_deferrable(&timer_null_group, TRUE);

  /* use qtimer frame that is available on all other targets */
  DalTimetick_Attach("WakeUpTimer", &g_DALWakeTimerHandle);
  CORE_VERIFY_PTR(g_DALWakeTimerHandle);

  DalTimetick_GetInterruptVectorNum(g_DALWakeTimerHandle, &timerIntN);

  /* Attaching to interrupt controller and registering interrupt handlers */
  CORE_VERIFY(DAL_DeviceAttach(DALDEVICEID_INTERRUPTCONTROLLER,
                               &g_DALInterruptCtrlHndl) == DAL_SUCCESS);

  CORE_VERIFY(DAL_SUCCESS == DalInterruptController_RegisterISR(
                              g_DALInterruptCtrlHndl,
                              timerIntN,
                              (DALIRQ)sleepTarget_wakeupHandler,
                              (const DALIRQCtx) 0,
                              DALINTRCTRL_ENABLE_CONFIG_EX(DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,
                                                           0, islandMode)));

#ifndef SLEEP_IMAGE_SLPI
  /* !! PDC timer wakeup HW bug workaround (QCTDD04481022)
   * Register for AOP IPC which will be sent everytime PDC wakes up */
  CORE_VERIFY(DAL_SUCCESS == DalInterruptController_RegisterISR(
                              g_DALInterruptCtrlHndl,
#ifdef SLEEP_IMAGE_CDSP
                              56,                                /* AOP second IPC */
#else
                              30,
#endif
                              (DALIRQ)sleepTarget_aopWakeupHandler,
                              (const DALIRQCtx) 0,
                              DALINTRCTRL_ENABLE_CONFIG_EX(DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,
                                                           0, 0)));
#endif //ndef SLEEP_IMAGE_SLPI

  /* Register for specific profiling interrupt if supported in target.
   * For now, only normal mode is supported - Note to disable it on island entry */
#if (SLEEP_PROFILING_ID != SLEEP_PMI_ID)
  CORE_VERIFY(DAL_SUCCESS == DalInterruptController_RegisterISR(
                              g_DALInterruptCtrlHndl,
                              SLEEP_PROFILING_ID,
                              (DALIRQ)sleepOS_profilingHandler,
                              (const DALIRQCtx) 0,
                              DALINTRCTRL_ENABLE_CONFIG_EX(DALINTRCTRL_ENABLE_RISING_EDGE_TRIGGER,
                                                           0, 0)));
#endif

  /* Initialize sleep NPA scheduler */
  sleepNPAScheduler_init();

  /* Initialize the SMEM memory to store subsystem stats for cx collapse */
  sleepMasterstats_init();

#ifdef SLEEP_BRINGUP_DEBUG
  if(g_sleepDebugTimerUs || g_sleepDebugTimerRndMs)
  {
    CORE_VERIFY(TE_SUCCESS == timer_def_osal(&g_sleepDebugTimerClient, 
                                             &timer_non_defer_group,
                                             TIMER_FUNC1_CB_TYPE, 
                                             sleepTarget_debugTimerCB, 0));

    timer_set_64(&g_sleepDebugTimerClient, (g_sleepDebugTimerUs | 1), 0, T_USEC);
  }
#endif

  return;
}

/**
 * sleepTarget_getCPUArchitecture
 */
void sleepTarget_getCPUArchitecture(DalChipInfoFamilyType   *chipFamily,
                                    DalChipInfoVersionType  *chipVer)
{
  static uint32 init = 0;

  if(init == 0)
  {
    DalDeviceHandle *hChipInfo;

    /* Get global chip architecture  */
    if( DAL_DeviceAttach(DALDEVICEID_CHIPINFO, &hChipInfo) == DAL_SUCCESS )
    {
      DalDevice_Open(hChipInfo, DAL_OPEN_SHARED);
      DalChipInfo_GetChipFamily(hChipInfo, &g_sleepTargetConfig.chip_family);
      DalChipInfo_GetChipVersion(hChipInfo, &g_sleepTargetConfig.chip_ver);
      DalDevice_Close(hChipInfo);
      init = 1;
    }
  }

  if(chipFamily != NULL)
    *chipFamily = g_sleepTargetConfig.chip_family;

  if(chipVer != NULL)
    *chipVer    = g_sleepTargetConfig.chip_ver;

  return;
}

/*
 * sleepTarget_initializeLPR
 */
void sleepTarget_initializeLPR(void)
{
  /* Register the pre-synthesized LPRMs here.  This must be done
   * before any of the component LPRMs are registered, so we do
   * it when the /sleep/lpr node is being initialized. */
  synthLPR_define( &SleepLPR_synth );
#ifdef USLEEP_ISLAND_MODE_ENABLE
  synthLPI_define( &SleepLPI_synth );
#endif

  return;
}

/*
 * sleepTarget_enableRpmhTimer 
 */
void sleepTarget_enableRpmhTimer(uint64 wakeupTick)
{
  /* Ensure that wakeup_tick is in the future */
  if(CoreTimetick_Get64() >= wakeupTick)
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 0,
                    " RPMh wakeup time already in past, skipping programming");

    /* Signal ATS to pickup the latest duration since the current one is expired already */
    sleepActive_SetSignal(SLEEP_SIGNAL_HARD_DURATION);
  }
  else
  {
    CORE_VERIFY(PDC_SEQ_SUCCESS == pdc_seq_set_wakeup_time(wakeupTick));
  }

  return;
}

/*
 * sleepTarget_enableSleepTimer 
 */
uint64 sleepTarget_enableSleepTimer(uint64 wakeupTick)
{
  uint64 matchTick;

  /* Ensure that wakeup_tick is in the future */
  if(CoreTimetick_Get64() >= wakeupTick)
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 0,
                    " Wakeup time already in past, skipping programming");

    /* This function should not be called in the idle timeline */
    CORE_VERIFY(FALSE == sleepOS_isProcessorInSTM());
    
    /* Signal ATS to pickup the latest duration since the current one is expired already */
    sleepActive_SetSignal(SLEEP_SIGNAL_HARD_DURATION);

    return 0;
  }

  /* Ensure any pending interrupt is cleared before programming a new value */
  CORE_VERIFY(DalTimetick_Enable(g_DALWakeTimerHandle, 0) == DAL_SUCCESS);

  /* Setting up a new match value */
  DalTimetick_SetNextInterrupt64(g_DALWakeTimerHandle,
                                 wakeupTick,
                                 0, &matchTick);

  /* Re-enable wakeup timer interrupt */
  CORE_VERIFY(DalTimetick_Enable(g_DALWakeTimerHandle, 1) == DAL_SUCCESS);

  sleepLog_printf(SLEEP_LOG_LEVEL_INFO, (1*2),
                  " Program QTMR (Match tick: 0x%llx)",
                  ULOG64_DATA(matchTick));

  return matchTick;
}

/*
 * sleepTarget_disableSleepTimer
 */
void sleepTarget_disableSleepTimer( void )
{
  /* Disables wakeup timer interrupt */
  CORE_VERIFY(DalTimetick_Enable(g_DALWakeTimerHandle, 0) == DAL_SUCCESS);

  return;
}

/*
 * sleepTarget_enableWatchdog 
 */
void sleepTarget_enableWatchdog(void)
{
  return;
}

/*
 * sleepTarget_disableWatchdog 
 */
void sleepTarget_disableWatchdog(void)
{
  return;
}

/*
 * sleepTarget_setRPMhAssistedMode
 */
void sleepTarget_setRPMhAssistedMode( boolean value )
{
  g_synthLPRMisRPMhAssisted = value;
  return;
}

/*
 * sleepTarget_getRPMhAssistedMode
 */
boolean sleepTarget_getRPMhAssistedMode( void )
{
  return(g_synthLPRMisRPMhAssisted);
}

/*
 * sleepTarget_getHardWakeupTime
 */
void sleepTarget_getHardWakeupTime(sleep_solver_deadlines *deadlines)
{
  uint64 npaDeadline;
  
  /* Retrieve the time (in ticks) of the next normal mode scheduled timer expiry. */
  deadlines->normal_timer.value = 
    (uint64)timer_get_first_non_deferrable_timer_expiry(&deadlines->normal_timer.owner.normal);
  
  /* Retreive the NPA deadline */
  npaDeadline = sleepNPAScheduler_getDeadline();

  /* Retrieve the island mode deadline */
#ifdef USLEEP_ISLAND_MODE_ENABLE
  deadlines->island_timer.value = 
    (uint64)utimer_get_first_non_deferrable_timer_expiry(&deadlines->island_timer.owner.island);
#else
  deadlines->island_timer.value = -1;
#endif

  sleepLog_printf(SLEEP_LOG_LEVEL_DEBUG, 4 + (2*3),
                  "Hard deadlines (Normal expiry: 0x%llx, Type: %u, Obj: 0x%x) "
                  "(Island expiry: 0x%llx, Type: %u, Obj: 0x%x) "
                  "(NPA Expiry: 0x%llx) ",
                  ULOG64_DATA(deadlines->normal_timer.value),
                  deadlines->normal_timer.owner.normal.cb_type,
                  (uint32)deadlines->normal_timer.owner.normal.sigs_func_addr,
                  ULOG64_DATA(deadlines->island_timer.value),
                  deadlines->island_timer.owner.island.cb_type,
                  (uint32)deadlines->island_timer.owner.island.sigs_func_addr,
                  ULOG64_DATA(npaDeadline) );

  /* Get minimum of actual timers */
  deadlines->overall_timer_min  = MIN(deadlines->normal_timer.value, deadlines->island_timer.value);

  /* Set first normal mode deadline to expire */
  deadlines->normal_min = MIN(deadlines->normal_timer.value, npaDeadline);

  /* Set the earliest wakeup time of all deadlines */
  deadlines->overall_min = MIN(deadlines->island_timer.value, deadlines->normal_min);

  return;
}

/*
 * sleepTarget_performInitialLPRExit
 */
void sleepTarget_performInitialLPRExit(void)
{
  /* No specific target actions are required for initial LPM exit */
  return;
}

/*
 * sleepTarget_setRpmHandshake
 */
rsc_result_t sleepTarget_setRpmHandshake(boolean performHandshake)
{
  q6LPMConfig_setRpmhHandshake(performHandshake);
  return RSC_SUCCESS;
}

/*
 * sleepTarget_setPdcLowPowerMode
 */
pdc_seq_result_t sleepTarget_setPdcLowPowerMode(pdc_low_power_mode_type mode)
{
  return(pdc_seq_set_lpm(mode));
}

/*
 * sleepTarget_setRscLowPowerMode
 */
rsc_result_t sleepTarget_setRscLowPowerMode(rsc_type type, rsc_low_power_mode_type coreMode)
{
  /* Configure the desired power mode for the given RSC */
  return(rsc_set_low_power_mode(type, coreMode));
}

/*
 * sleepTarget_getRscTimestamp
 */
void sleepTarget_getRscTimestamp(rsc_type         type,
                                 rsc_profile_unit timeStamp[RSC_PROFILE_NUMBER_OF_UNITS])
{
  rsc_profile_get_unit_data(type, timeStamp);
  return;
}

/*
 * sleepTarget_isInterruptPending
 */
boolean sleepTarget_isInterruptPending(uint32 intNumber)
{
  uint32 state = 0; 

  DalInterruptController_IsInterruptPending(g_DALInterruptCtrlHndl, intNumber, &state);
  return (0 != state) ? TRUE : FALSE;
}

/*
 * sleepTarget_setDefaultPowerMode
 */
void sleepTarget_setDefaultPowerMode(boolean enable)
{
  sleep_solver_output *solverOutput = sleepActive_GetSolverOutput();

  if(TRUE == enable)
  {
    /* Enable clock gating mode as the default.  If this changes, clock gating will need
     * to be added back to the cpu_vdd LPR */
    q6LPMConfig_setupModeConfig(RSC_MODE_CLK_GATE, TRUE);

    /* Enable kernel override directly to enter the default mode immediately at idle */
    sleepOS_configIdleMode(SLEEP_OS_IDLE_MODE_OVERRIDE);
  }
  else if(NULL != solverOutput->selected_synthLPRM)
  {
    /* Unconfigure clock gating mode which effectivly enables the currently selected and configured
     * low power mode. Note that the currently selected mode will never be clock gating
     * itself. */
    q6LPMConfig_setupModeConfig(RSC_MODE_CLK_GATE, FALSE);

    /* Allow ATS to correctly configure the sleep task for the current power mode. */
    sleepActive_configureIdleMode(solverOutput->selected_synthLPRM);
  }

  return;
}

