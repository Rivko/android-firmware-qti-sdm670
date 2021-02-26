/*==============================================================================
  FILE:         sleep_target.c

  OVERVIEW:     This file provides target-specific functionality for the 8960
                Modem Subsystem.

  DEPENDENCIES: None

                Copyright (c) 2011-2013 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
================================================================================
$Header: //components/rel/core.qdsp6/2.1/products/pd_stubs/src/pd_stubs.c#1 $
$DateTime: 2017/07/21 22:10:47 $
==============================================================================*/

/* ==================================================================
                     INCLUDE FILES
   ================================================================== */
#include "qurt.h"
#include "pd_mon_qurt.h"

/* ==================================================================
                     FUNCTIONS
   ================================================================== */

void pd_mon_audio_core(void)
{
    PD_MON_HANDLE hPdAudio;
    PD_MON_RESTART restart = PD_MON_RESTART_ALWAYS;
    hPdAudio=pd_mon_spawn(&restart, AUDIO_IMG_NAME);
    if (hPdAudio == 0){
        qurt_printf("%s: failed to spawn ... restart: %d \n", __func__, restart);
    }
    else{
        qurt_printf("%s: Protection Domain Launched %s\n", __func__, AUDIO_IMG_NAME);
    }
}
   
void pd_mon_sensors_core(void)
{
    PD_MON_HANDLE hPdSensor;
    PD_MON_RESTART restart = PD_MON_RESTART_ALWAYS;
    hPdSensor = pd_mon_spawn(&restart, SENSOR_IMG_NAME);
    if (hPdSensor == 0){
        qurt_printf("%s: failed to spawn ... restart: %d \n", __func__, restart);
    }
    else{
        qurt_printf("%s: Protection Domain Launched %s\n", __func__, SENSOR_IMG_NAME);
    }
}