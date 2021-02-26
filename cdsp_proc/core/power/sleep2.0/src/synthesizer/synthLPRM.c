/*==============================================================================
  FILE:         synthLPRM.c
  
  OVERVIEW:     Provides the synth lprm functions
 
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/synthesizer/synthLPRM.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "sleepi.h"
#include "sleep_statsi.h"
#include "sleep_lpri.h"
#include "synthLPRM.h"
#include "synthLPR.h"
#include "CoreAtomicOps.h"
#include "lookup_table_types.h"
#include "sleep_log.h"

#ifdef USLEEP_ISLAND_MODE_ENABLE
#include "synthLPI.h"
#endif

/*==============================================================================
                           INTERNAL VARIABLES
 =============================================================================*/
static sleep_synth_lprm *g_sleepCurrentlyConfiguringMode = NULL;

/*==============================================================================
                           FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * Helper function for synthLPRM_setupComponentModeParents
 */
static void setupComponentModeParents(sleep_synth_lprm *synthLPRM,
                                      sleep_lprm_group *group)
{
  uint32      i;
  uint32      newParentIdx;
  sleep_lprm  *sleepLPRM;
  void        *tmpPtr;
  uint32      numMode;
  sleep_lprm  **componentModes;

  CORE_VERIFY_PTR(group);

  numMode         = group->num_component_modes;
  componentModes = group->component_modes;

  for( i = 0; i < numMode; i++ )
  {
    sleepLPRM     = componentModes[i];
    newParentIdx  = sleepLPRM->synth_LPRM_parent_count++;

    CORE_VERIFY_PTR(tmpPtr = realloc(sleepLPRM->synth_LPRM_parent_array,
                                     sizeof(void *) * sleepLPRM->synth_LPRM_parent_count));

    sleepLPRM->synth_LPRM_parent_array = tmpPtr;

    /* Set the component mode's parent to this synthmode */
    sleepLPRM->synth_LPRM_parent_array[newParentIdx] = synthLPRM;
  }

  return;
}

/*
 * synthLPRM_setupComponentModeParents
 */
void synthLPRM_setupComponentModeParents(sleep_synth_lprm *synthLPRM)
{
  CORE_VERIFY_PTR(synthLPRM);

  setupComponentModeParents(synthLPRM, &synthLPRM->cacheable);
  setupComponentModeParents(synthLPRM, &synthLPRM->uncacheable);

  return;
}

/*
 * synthLPRM_initSynth
 */
void synthLPRM_initSynth(sleep_synth_lprm *synthLPRM)
{
  CORE_LOG_VERIFY_PTR( synthLPRM, sleepLog_printf(SLEEP_LOG_LEVEL_ERROR, 0,
                       "ERROR (message: \"NULL Synthesized LPRM\")" ) );

  /* Default is synthmode disabled */
  synthLPRM->mode_status = SLEEP_DISABLED;
  synthLPRM_setupComponentModeParents(synthLPRM);

  return;
}

/*
 * synthLPRM_getLatency
 */
uint32 synthLPRM_getLatency(sleep_synth_lprm  *synthLPRM,
                            uint32            mLUTidx)
{
  CORE_VERIFY_PTR(synthLPRM);
  CORE_VERIFY(mLUTidx < SLEEP_NUM_STATIC_FREQ);

  /* Since enter & exit latencies are not dynamically adjusted,
   * always return the appropriate seed value */
  return synthLPRM->seed_latency->enter_exit[mLUTidx];
}

/*
 * synthLPRM_getEnterLatency
 */
uint32 synthLPRM_getEnterLatency(sleep_synth_lprm  *synthLPRM,
                                 uint32            mLUTidx)
{
  CORE_VERIFY_PTR(synthLPRM);
  CORE_VERIFY(mLUTidx < SLEEP_NUM_STATIC_FREQ);

  /* Since enter latency is not dynamically adjusted,
   * always return the appropriate seed value */
  return synthLPRM->seed_latency->enter[mLUTidx];
}

/*
 * synthLPRM_getBackOffTime
 */
const uint32* synthLPRM_getBackOffTime(sleep_synth_lprm  *synthLPRM,
                                       sleep_fLUT_node   *fLUT)
{
  const uint32* backoffPtr = NULL;

#ifdef SLEEP_ENABLE_AUTO_SYNTH_BACKOFF_ADJUSTMENT
  uint32  dynIndex;

  CORE_VERIFY_PTR(synthLPRM);

  dynIndex = synthLPRM->dynamic_num;

  /* If the backoff adjustment feature is enabled, then get the backoff value
   * from the frequency dependant list */
  backoffPtr = &(fLUT->dynamic_data.synth.LPRM[dynIndex].backoff);

#else

  CORE_VERIFY_PTR(synthLPRM);
  /* If the backoff feature is disabled, then get the value from the initial
   * seed value(s) given */
  backoffPtr = &(synthLPRM->seed_latency->backoff[fLUT->mLUT_idx]);

#endif

  return backoffPtr;
}

/*
 * synthLPRM_isAttributeSet
 */
boolean synthLPRM_isAttributeSet(sleep_synth_lprm *synthLPRM, 
                                 uint32           attributes,
                                 boolean          requireAllSet)
{
  boolean rtnValue;
  
  if(TRUE == requireAllSet)
  {
    /* Check if all attributes are set */
    rtnValue = (attributes != (synthLPRM->attr & attributes)) ? FALSE : TRUE;
  }
  else
  {
    /* Check if any attribute is set */
    rtnValue = (0 == (synthLPRM->attr & attributes)) ? FALSE : TRUE;
  }

  return(rtnValue);
}

/*
 * synthLPRM_cachedModeEnter
 */
void synthLPRM_cachedModeEnter(sleep_synth_lprm *synthLPRM,
                               uint64            wakeupTick,
                               boolean           reenter)
{
  sleep_lprm  *component;
  uint32      nMode;
  uint32      numMode;
  uint32      isReentrant;
  sleep_lprm  **componentModes;
  uint32      LPRMTime;
  uint64      startTime = CoreTimetick_Get64();
  
  CORE_VERIFY_PTR(synthLPRM);
  CORE_VERIFY(TRUE != sleepOS_isProcessorInSTM());

  synthLPRM->wakeup_tick = wakeupTick;

  /* Set the current mode being entered */
  g_sleepCurrentlyConfiguringMode = synthLPRM;

  /* Iterating through each component mode */
  numMode         = synthLPRM->cacheable.num_component_modes;
  componentModes  = synthLPRM->cacheable.component_modes;

  for(nMode = 0; nMode < numMode; nMode++)
  {
    /* Calling the actual enter functions of the component modes */
    component   = componentModes[nMode];
    isReentrant = component->attributes & SLEEP_MODE_ATTR_MULTIPLE_ENTRY;

    /* Set component last entry time */
    component->cacheable_mode_statistics.last_enter_start = startTime;

    /* If reentrant is false, call every component mode's enter function.
     * Otherwise, only call component mode's enter function if its REENTRANT
     * attribute is set.  */
    if((NULL != component->enter_func) && 
       ((FALSE == reenter) || (0 != isReentrant)))
    {
      sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_PROFILING, 
                          SLEEP_ATS_ENTER_MODE_NUM_ARGS,
                          SLEEP_ATS_ENTER_MODE_STR, 
                          SLEEP_ATS_ENTER_MODE,
                          (FALSE != reenter) ? "True" : "False",
                          component->parent_LPR->resource_name,
                          component->mode_name);
      
      component->enter_func(wakeupTick);
    
      if(NULL == component->enter_profile_func) 
      {
        LPRMTime = (uint32)(CoreTimetick_Get64() - startTime);
        sleepStats_updateValue32(&component->cacheable_mode_statistics.enter_lat, LPRMTime);
      }
    }

    /* Set end of this entering this LPRM */
    component->cacheable_mode_statistics.last_enter_end = CoreTimetick_Get64();

    /* Get beginning of the next LPRM */
    startTime = CoreTimetick_Get64();
  }

  return;
}

/*
 * synthLPRM_enter
 */
void synthLPRM_enter(sleep_synth_lprm  *synthLPRM, 
                     uint64            wakeupTick, 
                     sleep_fLUT_node   *fLUT)
{
  sleep_lprm  *component;
  uint32      i;
  uint32      numMode;
  sleep_lprm  **componentModes;
  uint32      LPRMTime;
  uint64      startTime = CoreTimetick_Get64();
  
  CORE_VERIFY_PTR(synthLPRM);

  /* setup initial value */
  synthLPRM->wakeup_tick = wakeupTick;

  /* Set the current mode being entered */
  g_sleepCurrentlyConfiguringMode = synthLPRM;

  /* Iterating through each component mode */
  numMode         = synthLPRM->uncacheable.num_component_modes;
  componentModes  = synthLPRM->uncacheable.component_modes;
  for(i = 0; i < numMode; i++)
  {
    /* Calling the actual enter functions of the component modes */
    component = componentModes[i];

    /* set time of last entry */
    component->mode_statistics.last_enter_start = startTime;

    /* Logging mode entry message */
    sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_PROFILING, 
                        SLEEP_ENTER_MODE_NUM_ARGS,
                        SLEEP_ENTER_MODE_STR, SLEEP_ENTER_MODE, 
                        component->parent_LPR->resource_name,
                        component->mode_name);

    /* Call the enter function with the latest sleep duration value */
    if(NULL != component->enter_func)
    {
      component->enter_func(synthLPRM->wakeup_tick);

      if(NULL == component->enter_profile_func)
      {
        LPRMTime = (uint32)(CoreTimetick_Get64() - startTime);
        sleepStats_updateValue32(&component->mode_statistics.enter_lat, LPRMTime);
      }
    }

    /* Set end of this entering this LPRM */
    component->mode_statistics.last_enter_end = CoreTimetick_Get64();

    /* Get start of next LPRM */
    startTime = CoreTimetick_Get64();
  }

  return;
}

/*
 * synthLPRM_cachedModeExit
 */
void synthLPRM_cachedModeExit(sleep_synth_lprm *synthLPRM)
{
  int         nMode;
  sleep_lprm  *component;
  uint32      numMode;
  sleep_lprm  **componentModes;
  uint32      LPRMTime;
  uint64      startTime = CoreTimetick_Get64();
  
  CORE_VERIFY_PTR(synthLPRM);
  CORE_VERIFY(TRUE != sleepOS_isProcessorInSTM());

  /* Set the current mode being exited */
  g_sleepCurrentlyConfiguringMode = synthLPRM;

  numMode         = synthLPRM->cacheable.num_component_modes;
  componentModes  = synthLPRM->cacheable.component_modes;

  for(nMode = numMode - 1; nMode >= 0; nMode--)
  {
    /* Calling the exit functions of the component modes */
    component = componentModes[nMode];
    
    /* set time of last exit */
    component->cacheable_mode_statistics.last_exit_start = startTime;

    /* Call LPRM exit function */
    if(NULL != component->exit_func)
    {
      sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_PROFILING, 
                          SLEEP_ATS_EXIT_MODE_NUM_ARGS,
                          SLEEP_ATS_EXIT_MODE_STR, 
                          SLEEP_ATS_EXIT_MODE, 
                          component->parent_LPR->resource_name,
                          component->mode_name);

      component->exit_func();
    
      if(NULL == component->exit_profile_func)
      {
        LPRMTime = (uint32)(CoreTimetick_Get64() - startTime);
        sleepStats_updateValue32(&component->cacheable_mode_statistics.exit_lat, LPRMTime);
      }
    }

    component->cacheable_mode_statistics.last_exit_end = CoreTimetick_Get64();
    startTime = CoreTimetick_Get64();
  }

  return;
}

/*
 * synthLPRM_exit
 */
void synthLPRM_exit(sleep_synth_lprm *synthLPRM,
                    sleep_fLUT_node  *fLUT)
{
  int         nMode;
  sleep_lprm  *component;
  uint32      numMode;
  sleep_lprm  **componentModes;
  uint32      LPRMTime;
  uint64      startTime = CoreTimetick_Get64();
  
  CORE_VERIFY_PTR(synthLPRM);

  /* Set the current mode being exited */
  g_sleepCurrentlyConfiguringMode = synthLPRM;

  numMode         = synthLPRM->uncacheable.num_component_modes;
  componentModes  = synthLPRM->uncacheable.component_modes;

  for(nMode = numMode - 1; nMode >= 0; nMode--)
  {
    /* Calling the exit functions of the component modes */
    component = componentModes[nMode];

    component->mode_statistics.last_exit_start = startTime;

    sleepLog_QDSSPrintf(SLEEP_LOG_LEVEL_PROFILING, 
                        SLEEP_EXIT_MODE_NUM_ARGS,
                        SLEEP_EXIT_MODE_STR, SLEEP_EXIT_MODE, 
                        component->parent_LPR->resource_name,
                        component->mode_name);

    /* Call LPRM exit function */
    if(NULL != component->exit_func)
    {
      component->exit_func();

      /* Do statistic calculations */
      if(NULL == component->exit_profile_func)
      {
        LPRMTime = (uint32)(CoreTimetick_Get64() - startTime);
        sleepStats_updateValue32(&component->mode_statistics.exit_lat, LPRMTime);
      }
    }

    component->mode_statistics.last_exit_end = CoreTimetick_Get64();
    startTime = CoreTimetick_Get64();
  }

  return;
}

static void synthLPRM_runProfiling(sleep_lprm             *component,
                                   sleepStats_timestamps  *timeStamps,
                                   sleep_fLUT_node        *fLUT,
                                   boolean                cacheable)
{
  uint32                  latency;
  sleepStats_lprm_stats   *stats;

  /* Increase parent LPR count */
  if(TRUE == cacheable)
  {
    component->parent_LPR->cacheable_run_count++;
    stats = &component->cacheable_mode_statistics;
  }
  else
  {
    component->parent_LPR->run_count++;
    stats = &component->mode_statistics;
  }

  /* Call LPRM profiling enter function */
  if(NULL != component->enter_profile_func)
  {
    latency = component->enter_profile_func(timeStamps, component, cacheable);

    if(latency)
    {
      sleepStats_updateValue32(&stats->enter_lat, latency);
    }
  }

  /* Call LPRM profiling exit function */
  if(NULL != component->exit_profile_func)
  {
    latency = component->exit_profile_func(timeStamps, component, cacheable);

    /* Update LPRM cycles & exit time tracking */
    if(latency)
    {
      sleepStats_updateValue32(&stats->exit_lat, latency);
    }
  }

  return;
}

/*
 * synthLPRM_profile
 */
void synthLPRM_profile(sleep_synth_lprm       *synthLPRM,
                       sleepStats_timestamps  *timeStamps,
                       sleep_fLUT_node        *fLUT)
{
  uint32        nMode;
  sleep_lprm    *component;
  uint32        numMode;
  sleep_lprm    **componentModes;
  uint64        synthStart = 0;
  uint64        synthEnd   = 0;
  
  CORE_VERIFY_PTR(synthLPRM);
  CORE_VERIFY_PTR(timeStamps);
  CORE_VERIFY_PTR(fLUT);
  
  sleepLog_printf(SLEEP_LOG_LEVEL_PROFILING, 0, " Profiling components ");

  /* Start with cacheable components */
  numMode = synthLPRM->cacheable.num_component_modes;

  if(numMode)
  {
    componentModes = synthLPRM->cacheable.component_modes;

    /* Mark the time in synth mode as the triggering of the child RSC */
    synthStart = timeStamps->rsc_child[STATS_RSC_PROFILE_DN_START].timeStamp;
      synthEnd   = timeStamps->rsc_child[STATS_RSC_PROFILE_UP_END].timeStamp;

    for(nMode = 0; nMode < numMode; nMode++)
    {
      /* Calling the exit functions of the component modes */
      component = componentModes[nMode];
      synthLPRM_runProfiling(component, timeStamps, fLUT, TRUE);
    }
  }

  /* Next uncacheable components */
  numMode = synthLPRM->uncacheable.num_component_modes;

  if(numMode)
  {
    /* If there are any uncacheable modes, that means there was idle timeline, so mark the
     * start and end points of the synthmode as such */
    synthStart = sleepStats_getLprTimeData(SLEEP_STATS_TIME_SLEEP_IDLE_START_TYPE);
    synthEnd   = sleepStats_getLprTimeData(SLEEP_STATS_TIME_STM_EXIT_TYPE);
    
    componentModes  = synthLPRM->uncacheable.component_modes;
    for(nMode = 0; nMode < numMode; nMode++)
    {
      /* Calling the exit functions of the component modes */
      component = componentModes[nMode];
      synthLPRM_runProfiling(component, timeStamps, fLUT, FALSE);
    }
  }

  /* Update the synthmode cycles in the synthmode */
  CORE_VERIFY(synthStart <= synthEnd);
  synthLPRM->synth_stats.cycles++;
  synthLPRM->synth_stats.last_enter_timestamp = synthStart;
  synthLPRM->synth_stats.last_exit_timestamp  = synthEnd;
  synthLPRM->synth_stats.in_mode_time         += (synthEnd - synthStart);

  /* Record time in PC for any clients that care */
  sleepStats_updatePCTime(synthEnd - synthStart);

  return;
}

/* 
 * synthLPRM_isConfiguringModeFullyCacheable 
 */
boolean synthLPRM_isConfiguringModeFullyCacheable(void)
{
  if (NULL != g_sleepCurrentlyConfiguringMode)
  {
    return (0 == g_sleepCurrentlyConfiguringMode->uncacheable.num_component_modes) ? TRUE : FALSE;
  }
  
  return FALSE;
}

/*
 * synthLPRM_getConfiguringModeEnterExitLatency
 */
uint32 synthLPRM_getConfiguringModeEnterExitLatency(void)
{
  return g_sleepCurrentlyConfiguringMode->seed_latency->enter_exit[0];
}

/*
 * synthLPRM_update
 */
void synthLPRM_update(sleep_synth_lprm *synthLPRM,
                      sleep_lprm       *changedSleepLPRM )
{
  uint32       nComponentModes;
  sleep_status oldStatus;
  
  CORE_VERIFY_PTR(synthLPRM);
  CORE_VERIFY_PTR(changedSleepLPRM);

  oldStatus = synthLPRM->mode_status;

  /* Update the number of enabled modes */
  if( TRUE == changedSleepLPRM->mode_enabled )
    synthLPRM->num_enabled_component_modes++;
  else
    synthLPRM->num_enabled_component_modes--;

  /* Make sure the number of enabled component modes is not out of range. */
  nComponentModes = synthLPRM->cacheable.num_component_modes +
                    synthLPRM->uncacheable.num_component_modes;
  CORE_VERIFY( synthLPRM->num_enabled_component_modes >= 0 &&
               synthLPRM->num_enabled_component_modes <= nComponentModes );
  
  if(synthLPRM->num_enabled_component_modes == nComponentModes)
  {
    synthLPRM->mode_status = SLEEP_ENABLED;
  }
  else
  {
    synthLPRM->mode_status = SLEEP_DISABLED;
  }

  /* If we changed state, check for a parent and update it if we have one */
  if(synthLPRM->mode_status != oldStatus)
  {
#ifdef USLEEP_ISLAND_MODE_ENABLE
    if(0 != (synthLPRM->attr & SYNTH_MODE_ATTR_LPI))
    {
      synthLPI_modeUpdated(synthLPRM);
    }
    else
#endif
    {
      synthLPR_modeUpdated(synthLPRM);
    }
  }

  return;
}

