/*==============================================================================
  FILE:         sensors_lpi.c

  OVERVIEW:     This file provides the uSleep sensors LPI definition.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/lpi/ddr/sensors_lpi.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "uSleep_os.h"
#include "uSleep_os_trans.h"
#include "uSleep_util.h"
#include "uSleep_transi.h"

/*===========================================================================
                              GLOBAL FUNCTIONS
 ==========================================================================*/
/** 
 * sensorsLPI_enter 
 *  
 * @brief Function that will be called while entering the sensor LPI
 */
void sensorsLPI_enter(uint64 unused)
{
  /* Set main entry pointer to begin transition */
  uSleepOS_setIdleFunctionPtr(uSleepOS_mainEntry);

  return;
}

/** 
 * sensorsLPI_exit 
 *  
 * @brief Function that will be called while exiting from the sensors LPI
 */
void sensorsLPI_exit(void)
{
  return;
}

/** 
 * sensorsLPI_initialize 
 *  
 * @brief Function that will be called at power island init time
 */
void powerIslandLPI_initialize(void)
{
  /* Set initial uSleep transition times based on the original XML values. These will later be
   * adjusted as transitions happen. */
  uSleepTrans_initLatency();

  return;
}
