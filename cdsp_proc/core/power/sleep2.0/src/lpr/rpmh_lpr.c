/*==============================================================================
  FILE:         rpmh_lpr.c

  OVERVIEW:     This file provides the LPR definition for the RPMh
                low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2016-2019 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/lpr/rpmh_lpr.c#2 $
$DateTime: 2019/05/07 04:03:54 $
==============================================================================*/
#include "comdef.h"
#include "sleep_target.h"
#include "rpmh_client.h"

#ifdef SLEEP_IMAGE_SLPI
  #include "sleep_masterstats.h"
  #include "sleep_statsi.h"
#endif
/*==============================================================================
                               EXTERNAL VARIABLES
 =============================================================================*/  
#ifdef SLEEP_IMAGE_SLPI 
/* If this flag is set, we will update CX collapse mode wakeup stats to smem */
 extern volatile boolean g_cxOffModeChosen;
#endif

/*==============================================================================
                              INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * RPMhLPR_enter
 *
 * @brief Enter function for RPM synchronization low power mode.
 */
void RPMhLPR_enter(uint64 wakeupTick)
{
  /* Indicate that we are doing a deeper power mode */
  sleepTarget_setRPMhAssistedMode(TRUE);

#ifdef SLEEP_IMAGE_SLPI
  g_cxOffModeChosen = TRUE;
  /* In sensors, there is a secondary DRV that RPMh driver needs to know about */
  rpmh_enter_sleep(RSC_DRV_VIRTUAL_SENSORS);
#else
  /* Enable child RSC handshaking in non-SLPI images */
  sleepTarget_setRpmHandshake(TRUE);
#endif
  
  /* Let RPMh driver know the main DRV to sleep */
  rpmh_enter_sleep(SLEEP_RPMH_DRV_MAP);

  return;
}

/**
 * RPMhLPR_exit
 *
 * @brief Exit function for RPM synchronization low power mode.
 */
void RPMhLPR_exit(void)
{
  sleepTarget_setRPMhAssistedMode(FALSE);

  rpmh_exit_sleep(SLEEP_RPMH_DRV_MAP);

#ifdef SLEEP_IMAGE_SLPI
  g_cxOffModeChosen = FALSE;
  rpmh_exit_sleep(RSC_DRV_VIRTUAL_SENSORS);
#else
  /* Disable RPM handshaking */
  sleepTarget_setRpmHandshake(FALSE);
#endif

  return;
}

#ifdef SLEEP_IMAGE_SLPI
/*==============================================================================
                             Rpmh Profiling Function
 =============================================================================*/
 /**
 * RPMhLPR_MasterstatsProfileExit
 *
 * @brief Profile function to Update Cx collapse mode wakeup stats to shared memory
 */
void RPMhLPR_ProfileExit(sleepStats_timestamps *timeStamps, 
                          sleep_lprm            *self,
                          boolean               runAsCacheable)
{	
  rsc_profile_unit  *dn_end   = &timeStamps->rsc_child[STATS_RSC_PROFILE_DN_END];	
  rsc_profile_unit  *up_end = &timeStamps->rsc_child[STATS_RSC_PROFILE_UP_END];

  /* If they aren't valid, then most likely it has been bypassed due to a pending interrupt */
  if((0 == dn_end->valid) || (0 == up_end->valid))
  {
    return;
  }
  
  /* Updating the CX collapse wakeup mode stats */
  sleepMasterstats_updateWakeupStats(up_end->timeStamp);
}
#endif
