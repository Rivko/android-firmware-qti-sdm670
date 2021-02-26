/*==============================================================================
  FILE:         cache_lpr.c

  OVERVIEW:     This file provides the sleep LPR cache definition for the
                QDSP6 ALL WAIT low-power modes.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/src/lpr/cache_lpr.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "sleep_osi.h"
#include "q6_lpm_config.h"

/*===========================================================================
                              TCM LPR FUNCTIONS
 ==========================================================================*/
/** 
 * cacheLPR_TCMRetentionEnter 
 *  
 * @brief Configure TCM for Retention
 */
void cacheLPR_TCMRetentionEnter(uint64 wakeupTick)
{
  q6LPMConfig_setupTCMConfig(MEM_RET);
  return;
}

/** 
 * cacheLPR_TCMNonRetentionEnter 
 *  
 * @brief Configure TCM for Non-Retention
 */
void cacheLPR_TCMNonRetentionEnter(uint64 wakeupTick)
{
  /* Increase the CPU frequency to maximum at the current voltage, to
   * ensure TCM save/restore happens as fast as possible. */
  sleepOS_setFrequency();

  q6LPMConfig_setupTCMConfig(MEM_NORET);

  return;
}

/** 
 * cacheLPR_TCMNonRetentionExit
 *  
 * @brief Configure TCM for retention on exit
 */
void cacheLPR_TCMNonRetentionExit(void)
{
  q6LPMConfig_setupTCMConfig(MEM_RET);
  sleepOS_unsetFrequency();
}

/*===========================================================================
                              L2 LPR FUNCTIONS
 ==========================================================================*/
/** 
 * cacheLPR_l2RetentionEnter 
 *  
 * @brief Configure L2 for Retention
 */
void cacheLPR_L2RetentionEnter(uint64 wakeupTick)
{
  q6LPMConfig_setupL2Config(MEM_RET);
  return;
}

/** 
 * cacheLPR_l2NonRetentionEnter 
 *  
 * @brief Configure L2 for Non-Retention
 */
void cacheLPR_L2NonRetentionEnter(uint64 wakeupTick)
{
  q6LPMConfig_setupL2Config(MEM_NORET);
  return;
}

/** 
 * cacheLPR_L2NonRetentionExit
 *  
 * @brief Configure L2 for retention on exit
 */
void cacheLPR_L2NonRetentionExit(void)
{
  q6LPMConfig_setupL2Config(MEM_RET);
}

/** 
 * cacheLPR_initialize
 *  
 * @brief Sets a default configuraiton at init
 */
void cacheLPR_initialize(void)
{
  /* Set retention mode by default */
  q6LPMConfig_setupL2Config(MEM_RET);
  q6LPMConfig_setupTCMConfig(MEM_RET);
}

