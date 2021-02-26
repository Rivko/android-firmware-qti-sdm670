#ifndef _SLEEP_MASTER_STATS_H_
#define _SLEEP_MASTER_STATS_H_
/*==============================================================================
  FILE:         sleep_masterstats.h
  
  OVERVIEW:     Internal functions & types to gather and adjust
                CX collapse mode statistics
 
  DEPENDENCIES: None
                
                Copyright (c) 2017-2019 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/sleep2.0/inc/sleep_masterstats.h#1 $
$DateTime: 2019/05/07 04:03:54 $
==============================================================================*/
#include <stdint.h>
#include <stdlib.h>
#include "comdef.h"

/*==============================================================================
                           MACRO DEFINITIONS
 =============================================================================*/
/* Master stats version ID */ 
#define MASTERSTATS_VERSIONID 0x1

/*=======================================================================
 *                         TYPE DEFINITIONS
 *======================================================================*/
/*CX collapse mode statistics*/
typedef struct master_stats_t
{
  uint32 VersionId;                     /* Version Id to maitain compatibility with APPS */
  uint32 SysSleep_Count;                /* Number of times DRV has voted voted for AOSS Sleep + CX Collapse */
  uint64 SysSleep_last_entered_at;      /* LAST Timestamp the Master(DRV) voted for AOSS Sleep + CX Collapse */
  uint64 SysSleep_last_exited_at;       /*  LAST Timestamp the Master(DRV) voted for CX at MO */
  uint64 SysSleep_accumulated_duration; /* Shows how long the Master (DRV) has been in a sleep mode with CX collapse voted for since device booted up */

} sleep_masterstats;

/*=======================================================================
 *                             GLOBAL FUNCTIONS
 *======================================================================*/
/**
 * sleepMasterstats_init
 *
 * @brief Initilizes the SMEM memory to store Cx collapse stats
 */
void sleepMasterstats_init(void);			
				   
/**
 * sleepMasterstats_updatePowerdownStats
 *
 * @brief Updating the CX collapse mode Entry timestamp in SMEM memory location
 *
 * @param timestamp:  CX collapse Mode Software Entry timestamp
 */
void sleepMasterstats_updatePowerdownStats(uint64 timestamp);
					   
/**
 * sleepMasterstats_updateWakeupStats
 *
 * @brief Updating the CX collapse mode wakeup Stats in SMEM memory location
 * 
 * @param timestamp: CX collapse Mode Exit timestamp
 */
void sleepMasterstats_updateWakeupStats(uint64 timestamp);

#endif /*_SLEEP_MASTER_STATS_H_*/

