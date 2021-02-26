/*============================================================================
  @file sns_root_pd_init.c

  @brief
  PD init for Sensors Root PD.

  Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================*/

/*============================================================================
  INCLUDES
  ============================================================================*/
#include "pd_mon_qurt.h"
/*============================================================================
  Public   Functions
  ============================================================================*/
void sns_root_pd_init(void)
{
   PD_MON_RESTART restart = PD_MON_RESTART_ALWAYS;
   (void)pd_mon_spawn(&restart, SENSOR_IMG_NAME);
}

