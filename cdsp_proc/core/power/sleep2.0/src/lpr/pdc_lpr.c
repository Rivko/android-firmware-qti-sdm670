/*==============================================================================
  FILE:         pdc_lpr.c

  OVERVIEW:     This file provides the LPR definition for programming 
                the PDC power modes

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/lpr/pdc_lpr.c#2 $
$DateTime: 2019/05/07 04:03:54 $
==============================================================================*/
#include "comdef.h"
#include "sleep_target.h"
#include "sleep_statsi.h"
#include "pdc_seq.h"
#include "sleep_masterstats.h"

/*==============================================================================
                               EXTERNAL VARIABLES
 =============================================================================*/
/* If this flag is set, we will update CX collapse mode wakeup stats to smem */
 extern volatile boolean g_cxOffModeChosen;

/*==============================================================================
                              EXTERNAL LPR FUNCTIONS
 =============================================================================*/
/**
 * PDCLPR_CXRetEnter
 *
 * @brief Enter function for PDC CX retention mode
 */
void PDCLPR_CXRetEnter(uint64 wakeupTick)
{
  /* Hard coded value for now which matches PDC mode array */
  sleepTarget_setPdcLowPowerMode(PDC_MODE_CX_MIN_AOSS);
  return;
}

/**
 * PDCLPR_CXOffEnter
 *
 * @brief Enter function for PDC CX collapse mode
 */
void PDCLPR_CXOffEnter(uint64 wakeupTick)
{
  g_cxOffModeChosen = TRUE;
  sleepTarget_setPdcLowPowerMode(PDC_MODE_CX_COL_AOSS);
  return;
}

/**
 * PDCLPR_CXOffExit
 *
 * @brief Exit function for PDC CX collapse mode
 */
void PDCLPR_CXOffExit(uint64 wakeupTick)
{
  g_cxOffModeChosen = FALSE;
  return;
}

/*==============================================================================
                             PDC Profiling Functions
 =============================================================================*/
/**
 * PDCLPR_profileExit
 *
 * @brief Updates LPRM exit specific profiling data for the PDC.This time is the same regardless of cacheable or  
 *        uncacheable setting  and calls the mode Update function to store wakeupstats to smem 
 */
uint64 PDCLPR_profileExit(sleepStats_timestamps *timeStamps, 
                          sleep_lprm            *self,
                          boolean               runAsCacheable)
{
  profile_unit  *start = &timeStamps->pdc[STATS_PDC_PROFILE_UP_START];
  profile_unit  *end   = &timeStamps->pdc[STATS_PDC_PROFILE_UP_END];

  /* If they aren't valid, then most likely it has been bypassed due to a pending interrupt */
  if((0 == start->valid) || (0 == end->valid))
  {
    return 0;
  }

  CORE_VERIFY(end->timeStamp > start->timeStamp);

  if(g_cxOffModeChosen)
  {
    /* Updating the CX collapse mode wake up timestamps */
    sleepMasterstats_updateWakeupStats(end->timeStamp);
  }

  return (end->timeStamp - start->timeStamp);
}

/**
 * PDCLPR_profileEnter
 *
 * @brief Updates LPRM entry specific profiling data for the PDC.
 *        This time is the same regardless of cacheable or uncacheable setting
 *  
 * @note At the moment, this only tracks the HW time and not the SW entry function time 
 */
uint64 PDCLPR_profileEnter(sleepStats_timestamps *timeStamps, 
                           sleep_lprm            *self,
                           boolean               runAsCacheable)
{
  profile_unit  *start = &timeStamps->pdc[STATS_PDC_PROFILE_DN_START];
  profile_unit  *end   = &timeStamps->pdc[STATS_PDC_PROFILE_DN_END];

  /* If they aren't valid, then most likely it has been bypassed due to a pending interrupt */
  if((0 == start->valid) || (0 == end->valid))
  {
    return 0;
  }

  CORE_VERIFY(end->timeStamp > start->timeStamp);

  sleepStats_updateValue32(&self->mode_statistics.enter_lat, 
                           self->cacheable_mode_statistics.last_exit_end - 
                           self->cacheable_mode_statistics.last_exit_start);

  return (end->timeStamp - start->timeStamp);
}

