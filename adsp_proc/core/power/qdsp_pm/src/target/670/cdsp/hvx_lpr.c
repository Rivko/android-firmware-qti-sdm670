/*==============================================================================
  FILE:         hvx_lpr.c

  OVERVIEW:     This file provides the LPR definition for programming 
                the HVX power modes

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/qdsp_pm/src/target/670/cdsp/hvx_lpr.c#1 $
$DateTime: 2017/08/24 10:26:03 $
==============================================================================*/
#include "comdef.h"
#include "sysmon_hvx_qdsppm.h"
#include "DDIVCS.h"
#include "adsppm.h"

#ifdef CDSP_HVX_LPR
extern DalDeviceHandle gVcsHvxHandle;
extern boolean gHvxTurnedOffRequested;
#endif
/*==============================================================================
                              EXTERNAL LPR FUNCTIONS
 =============================================================================*/
/**
 * HVXLPR_offEnter
 *
 * @brief Enter function for HVX On mode
 */
void HVXLPR_offEnter(uint64 wakeupTick)
{
#ifdef CDSP_HVX_LPR
  //Call Sysmon API to save
  sysmon_qdsppm_hvx_state_change(SYSMON_HVX_STATE_DISABLE);
  //Turn-off HVX if it is ON
  DalVCS_DisableHVX(&gVcsHvxHandle);
#endif
  return;
}

/**
 * HVXLPR_offExit
 *
 * @brief Exit function for HVX off mode
 */
void HVXLPR_offExit(uint64 wakeupTick)
{
#ifdef CDSP_HVX_LPR
  if(gHvxTurnedOffRequested == FALSE)
  {
    //Call Sysmon API to restore
    sysmon_qdsppm_hvx_state_change(SYSMON_HVX_STATE_ACTIVE);
    //Turn-on HVX if it was turned-off in Enter 
    DalVCS_EnableHVX(&gVcsHvxHandle);
#ifdef  HVX_SPL_INST
    char buf1[128];
    __asm__ volatile("vmem(%0) = v0" :: "r" (buf1));
                  
#endif	

  }
#endif

  return;
}

