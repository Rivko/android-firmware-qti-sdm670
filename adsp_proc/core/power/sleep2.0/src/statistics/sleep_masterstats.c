/*==============================================================================
  FILE:         sleep_masterstats.c
  
  OVERVIEW:     Contains internal,external variables and functions for the implementation 
                of CX collapse mode statistics

  DEPENDENCIES: None

                Copyright (c) 2017-2019 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/sleep2.0/src/statistics/sleep_masterstats.c#2 $
$DateTime: 2019/03/24 20:02:03 $
==============================================================================*/
#include <stdint.h>
#include "sleep_log.h"
#include "CoreVerify.h"
#include "smem.h" 
#include "smem_v.h"
#include "sleep_masterstats.h"

/*==============================================================================
                             GLOBAL VARIABLE
 ==============================================================================*/
volatile sleep_masterstats *g_masterSleepStats = NULL;

/*=======================================================================
 *                             GLOBAL FUNCTIONS
 *======================================================================*/
/**
 * sleepMasterstats_init
 */
void sleepMasterstats_init(void)
{
	
 /* Parameters structure for smem_alloc_ex and smem_get_addr_ex */	
  smem_alloc_params_type smem_params;
  
  smem_params.remote_host = SMEM_APPS;
  smem_params.smem_type   = SLEEP_SMEM_ID;
  smem_params.size        = sizeof(sleep_masterstats);
  smem_params.buffer      = NULL;
  smem_params.flags       = SMEM_ALLOC_FLAG_PARTITION_ONLY;

  /* Requests a pointer to a buffer in smem. Upon success, params.buffer points to the allocation in smem and return SMEM_STATUS_SUCCESS.*/
  CORE_VERIFY( SMEM_STATUS_SUCCESS	== smem_alloc_ex(&smem_params) );
  
  g_masterSleepStats = (sleep_masterstats*)smem_params.buffer;
  CORE_VERIFY_PTR(g_masterSleepStats);
  
  /* Master stats version ID to maitain compatibility with APPS */
  g_masterSleepStats->VersionId = MASTERSTATS_VERSIONID; 
  
  sleepLog_printf(SLEEP_LOG_LEVEL_INFO,0,"CX collapse stats shared Memory allocated succesfully");

  return;
}	

/**
 * sleepMasterstats_updatePowerdownStats
 */
void sleepMasterstats_updatePowerdownStats(uint64 timestamp) 
{
  g_masterSleepStats->SysSleep_last_entered_at  = timestamp;
	
  return;
}

/**
 * sleepMasterstats_updateWakeupStats
 */
void sleepMasterstats_updateWakeupStats(uint64 timestamp) 
{
	/* Updating the mode stats to the smem location */
    CORE_VERIFY(timestamp >= g_masterSleepStats->SysSleep_last_entered_at);
    
    g_masterSleepStats->SysSleep_last_exited_at         = timestamp;
    g_masterSleepStats->SysSleep_accumulated_duration  += (timestamp - g_masterSleepStats->SysSleep_last_entered_at);
    g_masterSleepStats->SysSleep_Count++;
	
    sleepLog_printf(SLEEP_LOG_LEVEL_INFO, 7,
                                 "Cx collapse mode sleep stats: "
				 "SysSleep_Count :%u "
				 "SysSleep_last_entered_at :0x%llx "
				 "SysSleep_last_exited_at : 0x%llx "
				 "SysSleep_accumulated_duration :0x%llx ",
				 g_masterSleepStats->SysSleep_Count,
				 ULOG64_DATA(g_masterSleepStats->SysSleep_last_entered_at),
				 ULOG64_DATA(g_masterSleepStats->SysSleep_last_exited_at),
				 ULOG64_DATA(g_masterSleepStats->SysSleep_accumulated_duration) );
  
  return;	
}
