/*==============================================================================
  FILE:         sleep_os.c

  OVERVIEW:     This file provides the sleep task and OS-specific
                functionality for the processors with Qurt running on them.

  DEPENDENCIES: sleepOS_mainTask() is the entry point to the lowest-priority
                thread processing.

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/os/qurt/sleep_os.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "sleepi.h"
#include "rcinit.h"
#include "qurt.h"
#include "npa.h"
#include "DALDeviceId.h"
#include "DALStdErr.h"
#include "synthRegistry.h"
#include "timer.h"
#include "sleep_config.h"
#include "CoreIni.h"
#include "sleep_npa_scheduler.h"
#include "sleep_os.h"
#include "sleep_osi.h"
#include "sleep_target.h"
#include "sleep_solveri.h"
#include "sleep_stats.h"
#include "sleep_statsi.h"
#include "DDIClock.h"
#include "sleep_utils.h"
#include "sleepActive.h"
#include "sleepActivei.h"

#ifdef USLEEP_ISLAND_MODE_ENABLE
#include "uSleep.h"
#endif

/*==============================================================================
                          INTERNAL TYPE DEFINITIONS
 =============================================================================*/
/* Main sleep structure for various node and device handles used during the current sleep cycle */
typedef struct sleep_handles_s
{
  /* DAL Client of Clock driver for requesting cpu frequency during Sleep */
  DalDeviceHandle   *dal_clk_handle;

  /* Handle to vote on /npa/in_stm resource */
  npa_client_handle sleep_in_stm_handle;
  
  /* Query handle to the /clk/cpu resource */
  npa_query_handle  clk_cpu_query_handle;
}sleep_handles;

/* Main sleep state structure used for current sleep cycle data */
typedef struct sleep_state_s
{
  /* Flag to avoid unnecessary execution of Sleep exit path logic 
   *
   * This flag is useful in two scenarios:
   * 1. Sleep was bailed due to one of the many restrictions.
   * 2. Target(Q6) woke up from APCR based low power modes and by the time
   *    control comes to Sleep task, exit logic is already executed from
   *    IST context. We don't need to execute it again.
   *
   * Must be set to TRUE if we were to enter solver based low power modes
   * at idle time and to FALSE while executing exit logic.
   */
  boolean             execute_sleep_exit_path;

  /* Flag to indicate if processor is running in sleep context. */
  boolean             processor_in_stm;
}sleep_state;

/*==============================================================================
                           EXTERNAL GLOBAL VARIABLES
 =============================================================================*/
extern sleep_idle_entry_ptr_type                      g_sleepIdleEntryTable[];
extern volatile sleep_idle_entry_ptr_type * volatile  g_idleEntryPtr;
extern volatile sleep_PMI_handler_ptr_type            g_PMIFncPtr;
extern volatile sleep_profile_handler_ptr_type        g_profileFncPtr;
extern uint8                                          g_sleepTaskStack[];
extern uint8                                          g_sleepPMITaskStack[];

/*==============================================================================
                           FORWARD DECLARATIONS
 =============================================================================*/
static void sleepOS_performLPMExit(void);

void sleepHoldoff_registerRCinitCompletionEvent(void);
static void sleepOS_inStmNodeAvailableCb(void         *context,
                                         unsigned int eventType,
                                         void         *data,
                                         unsigned int dataSize );

/*==============================================================================
                           INTERNAL VARIABLES
 =============================================================================*/
/* Global sleep state that contains various pieces of data used in the current sleep cycle */
static sleep_state    g_sleepState = {0};

/* Various node and device handles */
static sleep_handles  g_sleepHandles;

/* Value that provides a mechanism to disable sleep completly, or only allow the
 * most basic (ATS default) low power mode.
 * 
 * Note that in production code, the default is to allow all low power modes.
 * 
 * If initially set to 0 from cold boot (before sleep init function), sleep will remain in
 * a 'hold off' mode, and will forever spin in a while(1) equivalent loop.
 * Changing after, at runtime, has no further effect.
 * 
 * If initially set to 1 from cold boot, normal operation is allowed
 * 
 * If initially set to 2 from cold boot, only ATS default (clock gate) will be allowed
 * 
 * If not initally 0 (i.e. not in 'hold off') and changed later during runtime, the change will
 * only be recognized upon a restriction change (i.e. ATS is triggered to run). 
 * Only changing between 1 (normal operation) and 2 (clock gate) is meaningful at runtime. Once
 * normal operation is entered, it will not go back to holdoff (setting to 0 is same as
 * setting 2). */ 
#ifdef FEATURE_DISABLE_SLEEP_MODES
volatile uint8 g_sleepAllowLowPowerModes = 0;
#else
volatile uint8 g_sleepAllowLowPowerModes = 1;
#endif

/*==============================================================================
                       INTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/**
 * sleepOS_inStmNodeAvailableCb
 *
 * @brief Obtains the client handle for /npa/in_stm node when it
 *        becomes available.
 */
static void sleepOS_inStmNodeAvailableCb(void         *context,
                                         unsigned int eventType,
                                         void         *data,
                                         unsigned int dataSize)
{
  CORE_VERIFY_PTR(g_sleepHandles.sleep_in_stm_handle =
                  npa_create_sync_client("/npa/in_stm", "sleep_in_stm_client",
                                         NPA_CLIENT_REQUIRED));
  return;
}

/**
 * sleepOS_clkCpuNodeAvailableCb
 *
 * @brief Creates a query handle to /clk/cpu when it becomes available.
 */
static void sleepOS_clkCpuNodeAvailableCb(void          *context,
                                          unsigned int  eventType,
                                          void          *data,
                                          unsigned int  dataSize )
{
  CORE_VERIFY_PTR(g_sleepHandles.clk_cpu_query_handle = npa_create_query_handle("/clk/cpu"));
  return;
}

/**
 * sleepOS_registerPMIHandler
 *
 * @brief As part interrupt registration with QuRT, this function creates 
 *        a thread that will execute function to handle the Power Management
 *        Interrupt (PMI).
 *
 * @note As per pthread specifics, must be called from thread which does not terminate
 *       before the newly created thread does i.e. usually from Sleep task entry 
 *       function.
 */
static void sleepOS_registerPMIHandler(void)
{
  qurt_thread_t       pmiHandlerThread;
  qurt_thread_attr_t  pmiHandlerAttr;

  /* Set default interrupt handlers */
  sleepOS_setPMIFunctionPTR(sleepOS_PMIPerformer);
  sleepOS_setProfileFunctionPTR(sleepOS_profilePerformer);

  /* Initialize attribute */
  qurt_thread_attr_init(&pmiHandlerAttr);

  /* setting up attributes for pmi handler thread */
  qurt_thread_attr_set_name(&pmiHandlerAttr, "PMI_Handler");
  qurt_thread_attr_set_tcb_partition(&pmiHandlerAttr, 1); /* locate in uImage */
  qurt_thread_attr_set_priority(&pmiHandlerAttr,      1);
  qurt_thread_attr_set_stack_size(&pmiHandlerAttr, SLEEP_PMI_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&pmiHandlerAttr, (void *)g_sleepPMITaskStack);
  
  /* Fill in the stack with a marker to try and find overflows */
  memset(g_sleepPMITaskStack, 0xF8, sizeof(uint8) * SLEEP_PMI_STACK_SIZE);

  /* Creating thread to run PMI handler */
  CORE_VERIFY(QURT_EOK == qurt_thread_create(&pmiHandlerThread, &pmiHandlerAttr,
                                             sleepOS_pmiHandler, NULL));

  return;
}

/**
 * sleepOS_qurtInit
 * 
 * @brief Initialize the sleep subsystem.
 */
static void sleepOS_qurtInit(void)
{
  /* Attaching Dal handle to clock driver for frequency requests */
  CORE_DAL_VERIFY(DAL_DeviceAttach(DALDEVICEID_CLOCK, &g_sleepHandles.dal_clk_handle));

  /* Initialize the logging subsystem. */
  sleepLog_initialize();
  sleepLog_setLevel(SLEEP_LOG_LEVEL_MAX);

  /* Initialize target-independent NPA nodes. */
  sleepNPA_initialize();

  /* Initialize LPR / LPRM tracking subsystem. */
  sleepLPR_initialize();

  /* Initialize statistical data system */
  sleepStats_init();

  /* Initialize target-dependent stuff (nodes, modes, etc.). */
  sleepTarget_initialize();

  /* Register for RCInit completion event */
  sleepHoldoff_registerRCinitCompletionEvent();

  /* Register interrupt handler for interrupt 0 (also known as Power 
   * Management Interrupt) */
  sleepOS_registerPMIHandler();

  npa_resource_available_cb("/npa/in_stm",
                            sleepOS_inStmNodeAvailableCb, NULL);
                             
  npa_resource_available_cb("/clk/cpu",
                            sleepOS_clkCpuNodeAvailableCb, NULL);

  /* Setup the solver configuration. Must be done after LPR init or wherever the
   * registry is initialized. */
  sleepSolver_init();

  return;
}

/**
 * sleepOS_waitForIdle 
 *
 * @brief Suspend Sleep until processor is idle
 *  
 * @note  This call prepares for power collapse and masks the 
 *        global interrupt if HW threads are all wait
 *
 * @return Enum type indicating if all hardware threads are idle and we entered STM 
 *         or if a return from the kernel call was forced and we are not in STM. 
 */
static idle_status sleepOS_waitForIdle(void)
{
  return (qurt_power_wait_for_idle() ? WAIT_FOR_IDLE_FORCED_EXIT : WAIT_FOR_IDLE_ENTERED_STM);
}

/**
 * sleepOS_performLPMEnter
 * 
 * @brief Function to Enter low-power mode selected by Full Active Time
 *        solver.
 *
 * @param context: Reference to the context from which it is being called. 
 *  
 * @return TRUE if sleep mode was entered 
 *         FALSE if any error or early exit occured 
 */
static boolean sleepOS_performLPMEnter(void *context)
{
  uint64                sleepStart;
  sleep_solver_output   *solverOutput;
  sleep_solver_input    *solverInput;
  uint64                backoffDeadline;
  sleepOS_IdleModeType  idleModeCfg = (sleepOS_IdleModeType)context;

  /* Execute common sleep entry checks */
  if(0 == (sleepStart = sleepOS_prepareForSleep(idleModeCfg)))
  {
    return FALSE;
  }

  /* Record the beginning of the idle time */
  sleepStats_putLprTimeData(sleepStart, SLEEP_STATS_TIME_SLEEP_IDLE_START_TYPE);

  /* Get last solver I/O */
  solverInput   = sleepActive_GetSolverInput();
  solverOutput  = sleepActive_GetSolverOutput();
  
  /* Setting a flag to indicate that after this point we will need to
   * perform exit logic */
  g_sleepState.execute_sleep_exit_path = TRUE;

  /* Calculate the backoff deadline, and pass that to the enter functions. */
  backoffDeadline = solverOutput->ref_deadline - *solverOutput->backoff_ptr;
 
  /* Sleep overhead time */
  sleepStart = CoreTimetick_Get64() - sleepStart;

  sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 1 + (2 * 2),
                  "Entering modes (Hard deadline: 0x%llx) "
                  "(Backoff deadline: 0x%llx) "
                  "(Sleep overhead: %d)",
                  ULOG64_DATA(solverOutput->ref_deadline),
                  ULOG64_DATA(backoffDeadline),
                  (uint32)sleepStart);

  /* Enter the low power mode */
  synthLPRM_enter(solverOutput->selected_synthLPRM, backoffDeadline, solverInput->fLUT);

  return TRUE;
}

/** 
 * sleepOS_performLPMExit 
 *  
 * @brief Function to execute Exit path while coming out of solver based 
 *        low power modes during idle time.
 *
 * Depending on the low power modes we enter, this could be called either
 * from Sleep or an IST context.
 *
 * @param context: Reference to context from which it is being called 
 *                 (unused for now - more for complying with prototype
 *                 of interrupt handler). But if we need to differentiate
 *                 this could be used.
 *
 * @return none
 */
static void sleepOS_performLPMExit(void)
{
  sleep_solver_output *solverOutput;
  sleep_solver_input  *solverInput;

  /* Verify exit path should be running */
  CORE_VERIFY(TRUE == g_sleepState.execute_sleep_exit_path);

  /* Perform any target specific exit requimrents before calling LPR exit
   * routines (such as syncing the current timer) */
  sleepTarget_performInitialLPRExit();

  /* Logging point of sleep code resuming from PC */
  sleepStats_putLprTimeData(CoreTimetick_Get64(), SLEEP_STATS_TIME_MSTR_RETURN_TYPE);

  sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 0, "Exiting modes");

  /* Setting the flag to FALSE to avoid any unnecessary execution 
   * of the exit path afterwards. */
  g_sleepState.execute_sleep_exit_path = FALSE;

   /* Get last solver I/O */
  solverInput   = sleepActive_GetSolverInput();
  solverOutput  = sleepActive_GetSolverOutput();

  /* Exit functions of low power modes */
  synthLPRM_exit(solverOutput->selected_synthLPRM, solverInput->fLUT);

  /* Sleep Exit STM */
  sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_PROFILING, 
                      SLEEP_EXIT_STM_NUM_ARGS,
                      SLEEP_EXIT_STM_STR, 
                      SLEEP_EXIT_STM);
  
  /* Exit STM and perform any final sleep cleanup */
  sleepOS_completeSleep(TRUE);
    
  sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_PROFILING, 
                      SLEEP_EXIT_IDLE_NUM_ARGS, 
                      SLEEP_EXIT_IDLE_STR, 
                      SLEEP_EXIT_IDLE);
  
  return;
}

/*==============================================================================
                       EXTERNAL FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * sleepOS_completeSleep
 */
void sleepOS_completeSleep(boolean exitStm)
{
  /* We are about to exit from Sleep context - set flag to false */
  g_sleepState.processor_in_stm = FALSE;

  /* Exit STM
   * All statistical data must be filled in prior to exiting STM or a race
   * condition will be created between processing in the sleep task and the
   * PMI exit handler.
   * In certain cases (like island exit) we want to perform everything EXCEPT actually
   * exiting STM, so check the given parameter */ 
  if(TRUE == exitStm)
  {
    /* Actual sleep exit STM time */ 
    sleepStats_putLprTimeData(CoreTimetick_Get64(), SLEEP_STATS_TIME_STM_EXIT_TYPE);
    qurt_power_exit();
  }
  else
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 0, " STM EXIT BYPASS");
  }
  
  if(NULL != g_sleepHandles.sleep_in_stm_handle)
  {
    /* Allow NPA to trigger Event callbacks */
    npa_complete_request(g_sleepHandles.sleep_in_stm_handle);
  }

  sleepTarget_enableWatchdog();

  return;
}

/*
 * sleepOS_prepareForSleep
 */
uint64 sleepOS_prepareForSleep(sleepOS_IdleModeType idleModeCfg)
{
  uint64 sleepStart;

  /* Wait for all other HW threads to go idle. If the override setting is 
   * disabled, this function will return and the system will already be in
   * STM. */
  if(WAIT_FOR_IDLE_FORCED_EXIT == sleepOS_waitForIdle())
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_DEBUG, 0,
                    "Sleep preparation function exiting "
                    "(Reason: Forced idle exit requested)");

    /* An exit was explicitly requested from the wait for idle call.  When kernel
     * returns, we will not be in STM in this case */
    return 0;
  }

  /* Mark start of this sleep cycle */
  sleepStart = CoreTimetick_Get64();
  sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_PROFILING, SLEEP_ENTER_IDLE_NUM_ARGS,
                      SLEEP_ENTER_IDLE_STR, 
                      SLEEP_ENTER_IDLE,
                      (uint32)idleModeCfg);

  /* Ensure that sleep mode function has not changed */
  if(FALSE == sleepOS_verifyIdleModeConfig(idleModeCfg))
  {
    sleepLog_QDSSEvent(SLEEP_EARLY_EXIT_STM_NUM_ARGS,
                       SLEEP_EARLY_EXIT_STM);

    sleepLog_printf(SLEEP_LOG_LEVEL_DEBUG, 0,
                    "Sleep cycle skipped "
                    "(Reason: Sleep function changed)");

    sleepStart = 0;
    qurt_power_exit();
  }
  else
  {
    if(SLEEP_OS_IDLE_MODE_LPI != idleModeCfg)
    {
      /* Set in stm state */
      g_sleepState.processor_in_stm = TRUE;

      if(NULL != g_sleepHandles.sleep_in_stm_handle)
      {
        /* Prevent NPA from triggering Event Callbacks */
        npa_issue_required_request(g_sleepHandles.sleep_in_stm_handle, 1);
      }
    }

    sleepTarget_disableWatchdog();
  }

  return sleepStart;
}

/* 
 * sleepOS_setFrequency 
 */
uint32 sleepOS_setFrequency(void)
{
  uint32  cpuFreqHz;
  uint32  cpuFreqKhz;
  boolean bRequired = TRUE;

  CORE_VERIFY_PTR(g_sleepHandles.dal_clk_handle);

  CORE_DAL_VERIFY(DalClock_SetCPUMaxFrequencyAtCurrentVoltage(
                   g_sleepHandles.dal_clk_handle,
                   bRequired,
                   &cpuFreqHz) );

  cpuFreqKhz = cpuFreqHz / 1000;
  
  sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 1, 
                  "Sleep CPU frequency set (%u Khz)", cpuFreqKhz);

  return cpuFreqKhz;
}

/*
 * sleepOS_unsetFrequency 
 */
void sleepOS_unsetFrequency(void)
{
  uint32  cpuFreqHz;
  boolean bRequired = FALSE;

  CORE_DAL_VERIFY(DalClock_SetCPUMaxFrequencyAtCurrentVoltage(
                   g_sleepHandles.dal_clk_handle,
                   bRequired,
                   &cpuFreqHz) );

  return;
}

/*
 * sleepOS_getFrequency
 */
uint32 sleepOS_getFrequency(void)
{
  npa_query_type   result;
  npa_query_status status;
  uint32           cpuFreqKHz = 115200; /* Default value if query handle to clock is not created */
  
  status = npa_query(g_sleepHandles.clk_cpu_query_handle, 
                     NPA_QUERY_CURRENT_STATE, &result);
                      
  if(status == NPA_QUERY_SUCCESS)
  {
    cpuFreqKHz = result.data.state;
    CORE_VERIFY(cpuFreqKHz);
  }

  return cpuFreqKHz;
}

/*
 * sleepOS_profilePerformer
 */
void sleepOS_profilePerformer(void)
{
#if (SLEEP_PROFILING_ID != SLEEP_PMI_ID)
  /* Signal ATS to perform profiling of the last PC */
  sleepActive_SetSignal(SLEEP_SIGNAL_CTL_PERFORM_PROFILE);
#endif
  return;
}

/*
 * sleepOS_PMIPerformer
 */
void sleepOS_PMIPerformer(void)
{
#if (SLEEP_PROFILING_ID == SLEEP_PMI_ID)
  sleep_solver_output *solverOutput;
  uint64              now = CoreTimetick_Get64();

  /* If exit path is required, this is uncacheable APCR and STM will be released there. */
  if(TRUE == g_sleepState.execute_sleep_exit_path)
  {
    /* Call LPR exit handler */
    sleepOS_performLPMExit();
  }
  /* If exit path is not required, there are two posibilities:
   *  1. Just exited a fully cacheable mode, in which case there is nothing to do since kernel
   *     will not have entered STM for the PMI handler
   *  2. Just exited full PC, in which case we need to release STM here because kernel will
   *     have entered STM to run the PMI handler. */
  else
  {
    /* Pointer will always be valid - structure elements may not be */
    solverOutput = sleepActive_GetSolverOutput();

    /* We should never be here if the solver doesn't have a valid mode */
    CORE_VERIFY_PTR(solverOutput->selected_synthLPRM);

    if(0 == solverOutput->selected_synthLPRM->uncacheable.num_component_modes)
    {
     /* In cacheable modes set STM exit time first thing which is as close in SW as we can get
       * to exit time.  This will be written in the perform exit function if uncacheable. */ 
      sleepStats_putLprTimeData(now, SLEEP_STATS_TIME_STM_EXIT_TYPE);
    }
  }
#else
  /* Call LPR exit handler */
  sleepOS_performLPMExit();
#endif

  /* PMI interrupt is currently shared with profilng interrupt (until HW provides a seperate one)
   * so always assert the control signal for processing */
  sleepActive_SetSignal(SLEEP_SIGNAL_CTL_PERFORM_PROFILE);

  return;
}

/*
 * sleepOS_offsetSleepDuration
 */
void sleepOS_offsetSleepDuration(uint64 offset)
{
  sleep_solver_output *solverOutput = sleepActive_GetSolverOutput();

  /* Adjust the sleep duration, so that the new duration gets passed
   * to the rest of the enter functions. */
  if(NULL != solverOutput)
  {
    solverOutput->selected_synthLPRM->wakeup_tick += offset;
    solverOutput->ref_deadline                    += offset;
  }
  
  return;
}

/*
 * sleepOS_getHardDeadline
 */
uint64 sleepOS_getHardDeadline(void)
{
  sleep_solver_output *solverOutput = sleepActive_GetSolverOutput();

  return (solverOutput->ref_deadline);
}

/*
 * sleepOS_verifyIdleModeConfig
 */
boolean sleepOS_verifyIdleModeConfig(sleepOS_IdleModeType idleMode)
{
  CORE_VERIFY(idleMode < SLEEP_OS_IDLE_MODE_NUM_STATES);
  return (*g_idleEntryPtr == g_sleepIdleEntryTable[idleMode])? TRUE : FALSE;
}

/*
 * sleepOS_configIdleMode
 */
void sleepOS_configIdleMode(sleepOS_IdleModeType newIdleMode)
{
  CORE_VERIFY(newIdleMode < SLEEP_OS_IDLE_MODE_NUM_STATES);
  
  /* Update sleep function pointer */
  g_idleEntryPtr = &g_sleepIdleEntryTable[newIdleMode];

  /* Mode specific action */
  if( SLEEP_OS_IDLE_MODE_OVERRIDE == newIdleMode )
  {
    /* No Sleep task intervention - Kernel performs all wait directly. */
    qurt_power_override_wait_for_idle(TRUE);
  }
  else
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 1, 
                    "Setting sleep function (Idle Mode: %d)",
                    newIdleMode);

    /* Sleep task needs to be scheduled. */
    qurt_power_override_wait_for_idle(FALSE);
  }

  return;
}

/*
 * sleepOS_setPMIFunctionPTR
 */
void sleepOS_setPMIFunctionPTR(sleep_PMI_handler_ptr_type newPtr)
{
  g_PMIFncPtr = newPtr;
  return;
}

/*
 * sleepOS_setProfileFunctionPTR
 */
void sleepOS_setProfileFunctionPTR(sleep_profile_handler_ptr_type newPtr)
{
#if (SLEEP_PROFILING_ID != SLEEP_PMI_ID)
  g_profileFncPtr = newPtr;
#endif
  return;
}


/*
 * sleepOS_getPMIFunctionPTR
 */
sleep_PMI_handler_ptr_type sleepOS_getPMIFunctionPTR(void)
{
  return g_PMIFncPtr;
}


/*
 * sleepOS_getProfileFunctionPTR
 */
sleep_profile_handler_ptr_type sleepOS_getProfileFunctionPTR(void)
{
#if (SLEEP_PROFILING_ID != SLEEP_PMI_ID)
  return g_profileFncPtr;
#else
  return NULL;
#endif
}

/*
 * sleepOS_holdoffNoMode
 */
uint32 sleepOS_holdoffNoMode(void)
{
  /* Due to some holdoff client, we cannot simply perform any low power modes.
   * Simply return. At some point, sleep function pointer would change when all
   * holdoff requests are released. Until then, we will keep coming here. */
  return 0;
}

/*
 * sleepOS_createConfigHandle
 */
CoreConfigHandle sleepOS_createConfigHandle(void)
{
#ifdef SLEEP_ENABLE_EFS
  char path[MAX_INI_PATH_LEN];

  /* Build the path to the sleep_config.ini file */
  snprintf(path, MAX_INI_PATH_LEN,
           "/nv/item_files/sleep/core0/sleep_config.ini");

  return(CoreIni_ConfigCreate(path));

#else
  return NULL;
#endif
}

/*
 * sleepOS_destroyConfigHandle
 */
void sleepOS_destroyConfigHandle(CoreConfigHandle config)
{
#ifdef SLEEP_ENABLE_EFS
  CoreIni_ConfigDestroy(config);
#endif
  return;
}

/*
 * sleepOS_readEFSConfig 
 */
void sleepOS_readEFSConfig(void)
{
#ifdef SLEEP_ENABLE_EFS
  uint8         sleepConfigValue  = 0;
  unsigned int  ret               = FALSE;

  /* Check to see if we should do any LPRMs at all. */
  ret = sleepConfig_readBool((char *)"all_lprms", 
                             NULL,
                             (char *)"disable", 
                             (boolean *)&sleepConfigValue);

  if(ret)
  {
    if(sleepConfigValue)
    {
      g_sleepAllowLowPowerModes = 0;
    }
  }

  if(g_sleepAllowLowPowerModes == 0)
  {
    sleepLog_printf(SLEEP_LOG_LEVEL_WARNING, 0,
                    "WARNING (message: \"All sleep modes are disabled"
                    " in the global config\")" );
  }
#endif
  return;
}

/*
 * sleepOS_performLPM
 */ 
uint32 sleepOS_performLPM(void)
{
  boolean sleepModeEntered;
  void    *context = (void *)(SLEEP_OS_IDLE_MODE_DEFAULT);

  sleepModeEntered = sleepOS_performLPMEnter(context);

  /* Check if we need to execute exit path from here. It might already 
   * have been executed from other context.
   *
   * @Note
   * We are relying on a fact that when we come out of low power mode after
   * idle solver based Sleep, only one HW thread is running any SW. For modes,
   * that trigger PMI from SPM, exit path must be executed from IST context.
   * This is currently ensured by QuRT. If it changes, we will have to insert
   * additional check so that we don't execute exit path from here if we
   * had entered in to any mode with PMI.
   */
  if(TRUE == g_sleepState.execute_sleep_exit_path)
  {
    sleepOS_performLPMExit();
  }
 
  return 0;
}

/*
 * sleepOS_isProcessorInSTM
 */
boolean sleepOS_isProcessorInSTM(void)
{
  return (g_sleepState.processor_in_stm);
}

/**
 * sleepOS_taskInit
 * 
 * @brief RCInit function that will create the main sleep task and initilize 
 *        all of the sleep data structures 
 */
void sleepOS_taskInit(void)
{
  qurt_thread_t       sleepHandlerThread;
  qurt_thread_attr_t  sleepHandlerAttr;
  uint32              locateInIsland = 0;

#ifdef USLEEP_ISLAND_MODE_ENABLE
  locateInIsland = 1;
#endif

  /* Initialize the sleep subsystem */
  sleepOS_qurtInit();

  /* Initialize main sleep thread which needs to be located in the island
   * section */ 
  qurt_thread_attr_init(&sleepHandlerAttr);

  /* setting up attributes for the sleep thread */
  qurt_thread_attr_set_name(&sleepHandlerAttr, "sleep");
  qurt_thread_attr_set_tcb_partition(&sleepHandlerAttr, locateInIsland); /* locate in uImage */
  qurt_thread_attr_set_priority(&sleepHandlerAttr, 253);

  /* Stack memory has to be statically allocated due to island section location requirement */
  qurt_thread_attr_set_stack_size(&sleepHandlerAttr, SLEEP_STACK_SIZE);
  qurt_thread_attr_set_stack_addr(&sleepHandlerAttr, (void *)g_sleepTaskStack);
  
  /* Fill in the stack with a marker to try and find overflows */
  memset(g_sleepTaskStack, 0xF8, sizeof(uint8) * SLEEP_STACK_SIZE);

  /* Create the main sleep task in island section memory */
  CORE_VERIFY(QURT_EOK == qurt_thread_create(&sleepHandlerThread, &sleepHandlerAttr,
                                             sleepOS_mainTask, NULL));

  return;
}

