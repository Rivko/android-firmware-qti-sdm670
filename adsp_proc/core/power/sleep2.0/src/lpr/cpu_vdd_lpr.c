/*==============================================================================
  FILE:         cpu_vdd_lpr.c

  OVERVIEW:     This file provides the sleep LPR definition for the
                QDSP6 ALL WAIT low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/lpr/cpu_vdd_lpr.c#5 $
$DateTime: 2018/01/08 08:33:35 $
==============================================================================*/
#include "comdef.h"
#include "CoreTime.h"
#include "CoreVerify.h"
#include "sleepActivei.h"
#include "sleep_lpr.h"
#include "sleepi.h"
#include "sleep_log.h"
#include "sleep_target.h"
#include "sleep_statsi.h"
#include "qurt.h"
#include "q6_lpm_config.h"
#include "synthLPRM.h"
#include "DDIClock.h"
#include "DalDevice.h"
#include "sleep_masterstats.h"
#include "sleep_HALhwio.h"
#include "sleep_hwio.h"

/*==============================================================================
                               GLOBAL VARIABLES
  =============================================================================*/
/* Handle for communicating with the clk driver to turn off "things" before PC. */
static DalDeviceHandle *g_cpuClockHandle;

/* Time tracking for the external call */
static volatile uint64 g_cpuVddExtCallEnterTime  = 0;
static volatile uint64 g_cpuVddExtCallExitTime   = 0;

/* If this flag is set, we will update CX collapse mode sw entry time to smem */
volatile boolean g_cxOffModeChosen = FALSE;

/*==============================================================================
                INTERNAL FUNCTIONS FOR POWER CONFIG LPR'S MODES
 =============================================================================*/
/** 
 * powerConfigLPR_Q6LPMEnter
 *  
 * @brief Parameterized call to prepare core for different low power modes.
 * 
 * @param coreMode:   Mode to which RSC HW will be programmed to 
 *                    execute appropriate command sequence.
 *
 * @param wakeupTick: Absolute count of Q6 wakeup in ticks
 */
static void powerConfigLPR_Q6LPMEnter(rsc_low_power_mode_type coreMode,
                                      uint64                  wakeupTick)
{
  boolean synthHasUncacheableComponents;

#if (SLEEP_PROFILING_ID != SLEEP_PMI_ID)
  /* If there is a dedicated profiling interrupt and if the mode is fully cacheable, don't set a
   * enable the PMI wakeup interrupt  This relies on kernel to mark the exit of STM in cacheable modes */
  synthHasUncacheableComponents = (synthLPRM_isConfiguringModeFullyCacheable() == TRUE) ? FALSE : TRUE;
#else
  /* If there not a dedicated profiling interrupt always fire the PMI interrupt so profiling* 
   * data can be gathered for every mode */
  synthHasUncacheableComponents = TRUE;
#endif

  /* q6LPMConfig_setupModeConfig will only configure PMI if APCR is present and
   * the synth mode has uncacheable components */
  q6LPMConfig_setupModeConfig(coreMode, synthHasUncacheableComponents);

  return;
}

/*==============================================================================
                INTERNAL FUNCTIONS FOR CPU VDD LPR'S MODES
 =============================================================================*/
/** 
 * cpuVddLPR_powerCollapseEnter 
 *  
 * @brief Performs the appropriate power collapse depending on the presence of
 *        APCR.
 * 
 * @param coreMode: Mode to which RSC HW is programmed to execute appropriate
 *                  command sequence.
 */
static void cpuVddLPR_powerCollapseEnter(rsc_low_power_mode_type coreMode)
{
  if(sleepOS_isProcessorInSTM() == FALSE)
  {
    return;
  }

  g_cpuVddExtCallEnterTime = CoreTimetick_Get64();

  /* Call clock driver API to do any required HW workarounds before any power collapse */
  CORE_DAL_VERIFY(DalClock_ProcessorSleep(g_cpuClockHandle,
                                          CLOCK_SLEEP_MODE_POWER_COLLAPSE,
                                          0));

  g_cpuVddExtCallEnterTime = CoreTimetick_Get64() - g_cpuVddExtCallEnterTime;

   if(g_cxOffModeChosen)
   {
     /* Updating the CX collapse mode Entry timestamp in SMEM memory location */
     sleepMasterstats_updatePowerdownStats(CoreTimetick_Get64());
   }

  switch(coreMode)
  {
    case RSC_MODE_APCR_PLL_ON:
    case RSC_MODE_APCR_PLL_LPM:
#ifdef SLEEP_IMAGE_SLPI
    case RSC_MODE_APCR_PLL_LPM_NO_SS:
#endif
    {
      /* Perform APCR power collapse (L2 & TCM retention) */
      CORE_VERIFY(QURT_EOK == qurt_power_apcr_enter());
      break;
    }

    case RSC_MODE_PWRC:
    case RSC_MODE_PWRC_BHS:
    {
      /* Full PC kernel entry modes (L2 & TCM non-retention) */
      CORE_VERIFY(QURT_EOK == qurt_power_shutdown_enter(QURT_POWER_SHUTDOWN_TYPE_SAVETCM));
      break;
    }

    case RSC_MODE_L2_NRET:
    case RSC_MODE_L2_NRET_BHS:
    {
      /* L2 nonretention PC kernel entry modes (TCM retention mode) */
      CORE_VERIFY(QURT_EOK == qurt_power_shutdown_enter(QURT_POWER_SHUTDOWN_TYPE_L2NORET));
      break;
    }

    default:
    {
      CORE_LOG_VERIFY(0, sleepLog_printf(SLEEP_LOG_LEVEL_WARNING, 1,
                                         "Unsupported coreMode: 0x%x",
                                         coreMode));
    }
  }

  return;
}

/** 
 * cpuVddLPR_powerCollapseExit 
 *  
 * @brief Performs the appropriate exit from power collapse depending on the
 *        presence of APCR.
 */
static void cpuVddLPR_powerCollapseExit(void)
{
  /* If LPR Exit is called as part of Cached Synth mode then 
   * no additional stats related profiling is required */
  if(sleepOS_isProcessorInSTM() == FALSE)
  {
    return;
  }

  g_cpuVddExtCallExitTime = CoreTimetick_Get64();

  CORE_DAL_VERIFY(DalClock_ProcessorRestore(g_cpuClockHandle,
                                            CLOCK_SLEEP_MODE_POWER_COLLAPSE,
                                            0));

  g_cpuVddExtCallExitTime = CoreTimetick_Get64() - g_cpuVddExtCallExitTime;

  return;
}

/*==============================================================================
                 INTERNAL FUNCTIONS FOR POWER CONFIG LPR'S MODES
 =============================================================================*/
/** 
 * powerConfigLPR_initialize
 *  
 * @brief Initialize cpu vdd LPR
 */
void powerConfigLPR_initialize(void)
{
   /* Attaching a handle for clock */
   CORE_DAL_VERIFY(DAL_ClockDeviceAttach(DALDEVICEID_CLOCK, &g_cpuClockHandle));

  /* Common Q6SS register setup for all low power modes defined for
   * cpuVdd LPR. */
  q6LPMConfig_setupBase();

  return;
}

/**
 * powerConfigLPR_fullPCEnter
 *
 * @brief This function will be called while entering into the power collapse 
 *        mode with l2 non retention (flushed).
 *
 * @param wakeupTick: Time at which master is expected to wake up.
 */
void powerConfigLPR_fullPCEnter(uint64 wakeupTick)
{
  powerConfigLPR_Q6LPMEnter(RSC_MODE_L2_NRET_BHS, wakeupTick);
  return;
}

#ifdef SLEEP_IMAGE_SLPI
/**
 * powerConfigLPR_apcrLongSleepSetEnter
 *  
 * @brief Enter function for APCR (long sequence in Q6HPG) with Q6 PLL in LPM 
 *        and sleep set 
 * 
 * @note Function is specific to SLPI image
 *
 * @param wakeupTick: Time at which master is expected to wake up. 
 */
void powerConfigLPR_apcrLongSleepSetEnter(uint64 wakeupTick)
{
  powerConfigLPR_Q6LPMEnter(RSC_MODE_APCR_PLL_LPM, wakeupTick);
  return;
}
#endif

/**
 * powerConfigLPR_apcrLongEnter
 *  
 * @brief Enter function for APCR (long sequence in Q6HPG) with Q6 PLL in LPM 
 *  
 * @param wakeupTick: Time at which master is expected to wake up. 
 */
void powerConfigLPR_apcrLongEnter(uint64 wakeupTick)
{
#ifdef SLEEP_IMAGE_SLPI
  powerConfigLPR_Q6LPMEnter(RSC_MODE_APCR_PLL_LPM_NO_SS, wakeupTick);
#else
  powerConfigLPR_Q6LPMEnter(RSC_MODE_APCR_PLL_LPM, wakeupTick);
#endif
  return;
}

/** 
 * powerConfigLPR_apcrShortEnter 
 *  
 * @brief Enter function for APCR (short sequence in Q6HPG) 
 *  
 * @param wakeupTick: Time at which master is expected to wake up. 
 */
void powerConfigLPR_apcrShortEnter(uint64 wakeupTick)
{
  powerConfigLPR_Q6LPMEnter(RSC_MODE_APCR_PLL_ON, wakeupTick);
  return;
}

/** 
 * powerConfigLPR_apcrNoTimerEnter 
 *  
 * @brief Enter function for APCR (short sequence in Q6HPG) and PMI interrupt is disabled 
 *  
 */
void powerConfigLPR_apcrNoTimerEnter(uint64 wakeupTick)
{
  q6LPMConfig_setupModeConfig(RSC_MODE_APCR_PLL_ON, FALSE);
  return;
}


/*==============================================================================
                             CPU VDD Functions
 =============================================================================*/
/**
 * cpuVddLPR_fullPCEnter
 *
 * @brief Enter function for power collapse with L2 nonretention
 */
void cpuVddLPR_fullPCEnter(uint64 wakeupTick)
{
  cpuVddLPR_powerCollapseEnter(RSC_MODE_L2_NRET_BHS);
  return;
}

/** 
 * cpuVddLPR_exit 
 *  
 * @brief Exit function for power collapse with no retention
 */
void cpuVddLPR_exit(void)
{
  cpuVddLPR_powerCollapseExit();
  return;
}

/**
 * cpuVddLPR_apcrNoTimerExit
 *
 * @brief Enter function for power collapse with APCR
 */
void cpuVddLPR_apcrNoTimerExit(uint64 wakeupTick)
{
  /* Configuration for wakeup on sleep frame interrupt only */ 
  SLEEP_HWIO_OUT(L2VIC_WAKEUP_EN, DEFER_QTMR_FRAMES_CONFIG);

  /* Specific APCR mode doesn't matter here because correct APCR configuration has already
   * been setup in power config LPR */
  cpuVddLPR_powerCollapseEnter(RSC_MODE_APCR_PLL_ON);
  return;
}

/**
 * cpuVddLPR_apcrEnter
 *
 * @brief Enter function for power collapse with APCR
 */
void cpuVddLPR_apcrEnter(uint64 wakeupTick)
{
  /* Specific APCR mode doesn't matter here because correct APCR configuration has already
   * been setup in power config LPR */
  cpuVddLPR_powerCollapseEnter(RSC_MODE_APCR_PLL_LPM);
  return;
}

/**
 * cpuVddLPR_apcrNoTimerEnter
 *
 * @brief Enter function for power collapse with APCR
 */
void cpuVddLPR_apcrNoTimerEnter(uint64 wakeupTick)
{
  /* Configuration for wakeup on any frame interrupt */
  SLEEP_HWIO_OUT(L2VIC_WAKEUP_EN, UNDEFER_QTMR_FRAMES_CONFIG);	

  /* Specific APCR mode doesn't matter here because correct APCR configuration has already
   * been setup in power config LPR */
  cpuVddLPR_powerCollapseEnter(RSC_MODE_APCR_PLL_ON);
  return;
}

/*==============================================================================
                             CPU VDD Profiling Functions
 =============================================================================*/
/**
 * cpuVddLPR_profileEnter
 *
 * @brief Updates APCR LPRM specific profiling data
 */
uint64 cpuVddLPR_profileEnter(sleepStats_timestamps *timeStamps, 
                              sleep_lprm            *self,
                              boolean               runAsCacheable)
{
  uint64            start;
  rsc_profile_unit  *end = &timeStamps->rsc_child[STATS_RSC_PROFILE_DN_END];
  
  if(TRUE == runAsCacheable)
  {
    start = timeStamps->rsc_child[STATS_RSC_PROFILE_DN_START].timeStamp;
  }
  else 
  {
    /* CPU VDD is the last LPR before actual power collpase, simply use the start time of the
     * enter function */
    start = self->mode_statistics.last_enter_start;
  }

  CORE_VERIFY(end->timeStamp > start);
  return (end->timeStamp - start);
}

/**
 * cpuVddLPR_apcrProfileExit
 *
 * @brief Updates APCR LPRM specific profiling data
 */
uint64 cpuVddLPR_apcrProfileExit(sleepStats_timestamps *timeStamps, 
                                 sleep_lprm            *self,
                                 boolean               runAsCacheable)
{
  uint64            end;
  rsc_profile_unit  *start = &timeStamps->rsc_child[STATS_RSC_PROFILE_UP_START];
  
  if(start->valid)
  {
    if(TRUE == runAsCacheable)
    {
      end = timeStamps->rsc_child[STATS_RSC_PROFILE_UP_END].timeStamp;
    }
    else 
    {
      end = self->mode_statistics.last_exit_end;
    }

    CORE_VERIFY(end > start->timeStamp);
    return (end - start->timeStamp);
  }

  return 0;
}

/**
 * cpuVddLPR_fullPCProfileExit
 *
 * @brief Updates full pc LPRM specific profiling data
 */
uint64 cpuVddLPR_fullPCProfileExit(sleepStats_timestamps *timeStamps, 
                                   sleep_lprm            *self,
                                   boolean               runAsCacheable)
{
  rsc_profile_unit  *startSeq   = &timeStamps->rsc_child[STATS_RSC_PROFILE_UP_START];
  rsc_profile_unit  *altStart   = &timeStamps->rsc_parent[STATS_RSC_PROFILE_UP_END];
  uint64            end         = self->mode_statistics.last_exit_end; /* FUll PC is always uncacheable */

  /* Check the case where child initiates the wakeup. */
  /* Since full PC is uncacheable, any software trigger should not be able to happen, and the
   * issue as above shouldn't apply */
  if(startSeq->valid)
  {
    if((altStart->valid) && (altStart->timeStamp > startSeq->timeStamp))
    {
      return (end - altStart->timeStamp);
    }
    else
    {
      return (end - startSeq->timeStamp); 
    }
  }

  return 0;
}

