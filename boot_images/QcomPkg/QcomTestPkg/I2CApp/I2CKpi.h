
/** @file I2CKpi.h

  I2C Kpi Header

                Copyright (c) 2017 Qualcomm Technologies Incorporated.
                              All Rights Reserved.
                     Qualcomm Confidential and Proprietary


**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 11/21/17   VG     Created
=============================================================================*/

#ifndef _I2C_KPI_H_
#define _I2C_KPI_H_

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/ArmArchTimer.h>

#define NUM_DEBUG_TRANSACTIONS 54

UINT64 starting_tick = 0, ending_tick = 0, total_ticks = 0, 
       uSeconds = 0,tput = 0, remainder = 0;       
UINT32 TimetickFreq = 0;

typedef enum {
     START = 0,         
     STOP,          
     WRITE,         
     READ,          
} BusDbgCmd;

typedef struct BusDbgInfo {
    uint32  cmd;
	uint8   sid;
	uint32  bus_Frequency_KHz;
    uint32  dataLen;
    uint64  startTime,EndTime,duration;
    uint64   speed;
	uint32  res;
} BusDbgInfo;

typedef struct BusDebugInfo
{    uint8 busidx ;
     BusDbgInfo busTrans[NUM_DEBUG_TRANSACTIONS];
} BusDebugInfo;

#endif /* _I2C_KPI_H_ */