#/**
 * @file:  SpmiOsImage.c
 * 
 * Copyright (c) 2013-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2018/02/16 04:48:18 $
 * $Header: //components/rel/aop.ho/1.1.c1/aop_proc/core/pmic/spmi/src/platform/os/aop/SpmiOsImage.c#1 $
 * $Change: 15485400 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 1/20/15  Multiple bus support
 * 11/3/14  Automatic channel assignment
 * 10/1/13  Initial Version
 */

#include "SpmiOs.h"
#include "SpmiInfo.h"
#include "SpmiOsLogs.h"
#include "SpmiOsTarget.h"
#include "SpmiLogs.h"
#include "time_service.h"
#include <string.h>
#include "SpmiBus.h"
#include "vrm_inc.h"
#include "rex.h"

//******************************************************************************
// Global Data
//******************************************************************************

extern SpmiInfo * pSpmiBlockInfo;

static rex_crit_sect_type spmiLock;

//ULogHandle spmiLogHandle;

//******************************************************************************
// Public API Functions
//******************************************************************************
__attribute__((section("pm_cram_reclaim_pool")))
Spmi_Result SpmiOs_Init(SpmiInfo **ppSpmiInfo)
{
   *ppSpmiInfo = pSpmiBlockInfo;

   rex_init_crit_sect(&spmiLock);

   return SPMI_SUCCESS;
}

boolean SpmiOs_HandleTransactionError(SpmiDevInfo *pDev, Spmi_Result* rslt, PmicArbCmd cmd, 
                                      uint8 slaveId, uint16 address, uint8 tries)
{
    return FALSE;
}

void SpmiOs_Wait(uint32 us)
{
    busywait( us );
}

Spmi_Result SpmiOs_Malloc(uint32 size, void** buf)
{
    *buf = malloc(size);
    if(*buf != NULL) {
        memset( *buf, 0, size );
        return SPMI_SUCCESS;
    }    
    
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

Spmi_Result SpmiOs_Free(void* buf)
{
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

Spmi_Result SpmiOs_RegisterISR(SpmiDevInfo *pDev, SpmiOs_IsrPtr isr)
{
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

uint64 SpmiOs_GetTimeTick(void)
{
    return time_service_now();
}

SpmiOs_ClkStatus SpmiOs_GetSerialClkState(SpmiDevInfo *pDev)
{
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

SpmiOs_ClkStatus SpmiOs_GetAhbClkState(SpmiDevInfo *pDev)
{
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

Spmi_Result SpmiOs_Lock()
{
    Spmi_Result result = SPMI_SUCCESS;

    rex_enter_crit_sect(&spmiLock);

    return result;
}

Spmi_Result SpmiOs_Unlock()
{
    Spmi_Result result = SPMI_SUCCESS;

    rex_leave_crit_sect(&spmiLock);

    return result;
}

boolean SpmiOs_UseHwInterface(PmicArbCmd cmd)
{
    boolean hwInterfaceFlag = FALSE;

    return hwInterfaceFlag; 
}

Spmi_Result SpmiOs_ExecuteCommand(PmicArbCmd cmd, Spmi_AccessPriority priority, uint8 vSlaveId, 
                                  uint16 address, uint8* data, uint32 dataLen, uint32* bytesTransacted)
{
   return SPMI_FAILURE_COMMAND_NOT_SUPPORTED;
}
