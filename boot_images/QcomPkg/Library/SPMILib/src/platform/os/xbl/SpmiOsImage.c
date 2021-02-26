/**
 * @file:  SpmiOsImage.c
 * 
 * Copyright (c) 2013-2016 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $DateTime: 2017/09/18 09:26:13 $
 * $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SPMILib/src/platform/os/xbl/SpmiOsImage.c#1 $
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 11/30/16 Remove QcomBaseLib dependency
 * 01/20/15 Multiple bus support
 * 11/3/14  Automatic channel assignment
 * 10/1/13  Initial Version
 */
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/TimerLib.h>

#include "SpmiOs.h"
#include "SpmiBus_Os.h"
#include "SpmiLogs.h"
#include "SpmiBusInternal.h"
#include "PmicArb.h"
#include "SpmiOsTarget.h"

//******************************************************************************
// Macros / Definitions / Constants
//******************************************************************************

/**
 * Static Function Declarations and Definitions
 */
extern UINTN EFIAPI MicroSecondDelay (IN  UINTN MicroSeconds);


//******************************************************************************
// Global Data
//******************************************************************************

extern SpmiInfo * pSpmiBlockInfo;
static UINTN gpuPmicArbSpmiVirtualAddress = 0;

/**
 * Gets the base address of the PMIC Arbiter on this EE (PMIC_ARB_BASE)
 */
Spmi_Result SpmiOs_GetPmicArbBaseAddr(void** addr)
{
    uintptr_t addrInt;
    
    if (gpuPmicArbSpmiVirtualAddress == 0)
    {
        SpmiBus_OsGetCoreBaseAddress(0, &addrInt);
    }
    else
    {
        addrInt = gpuPmicArbSpmiVirtualAddress;
    }

    *addr = (void *) addrInt;
    
    return SPMI_SUCCESS;
}

//******************************************************************************
// Implemented API Functions
//******************************************************************************

Spmi_Result SpmiOs_Init(SpmiInfo **ppSpmiInfo)
{
   Spmi_Result rslt;
   if((rslt = SpmiOs_GetPmicArbBaseAddr((void**)&pSpmiBlockInfo->devices[0].baseAddrs)) != SPMI_SUCCESS) {
       return rslt;
   }
   *ppSpmiInfo = pSpmiBlockInfo;
   return SPMI_SUCCESS;
}

boolean SpmiOs_HandleTransactionError(SpmiDevInfo *pDev, Spmi_Result* rslt, PmicArbCmd cmd, 
                                      uint8 slaveId, uint16 address, uint8 tries)
{
    boolean rtn = FALSE;

    if(pDev->dynamicChannelMode && (*rslt == SPMI_FAILURE_CHANNEL_NOT_FOUND))
    {
        Spmi_Result cfgRslt;
        SpmiBusCfg_InternalChnlCfg chanCfg;
        uint8 periph = (uint8)REG_ADDR_TO_PERIPH_ID( address );
        
        chanCfg.slaveId = slaveId;
        chanCfg.periphId = periph;
        chanCfg.periphOwner = pSpmiBlockInfo->owner;
        chanCfg.irqOwnerFlag = FALSE;

        if((cfgRslt = SpmiBusCfg_ConfigureChannel( pDev, &chanCfg, TRUE )) != SPMI_SUCCESS) {
            *rslt = cfgRslt;
        }
        else if((cfgRslt = PmicArb_RemoveCacheEntry( pDev, chanCfg.channel )) != SPMI_SUCCESS) {
            *rslt = cfgRslt;
        }
        else if((cfgRslt = PmicArb_AddCacheEntry( pDev, chanCfg.channel, slaveId, periph )) != SPMI_SUCCESS) {
            *rslt = cfgRslt;
        }
        else 
        {
            rtn = TRUE;
        }
    }

    SPMI_LOG_VERBOSE( "OS Error %d (%s)", *rslt, rtn ? "retry" : "done" );
    return rtn;
}

void SpmiOs_Wait(uint32 us)
{
    MicroSecondDelay (us);
}

Spmi_Result SpmiOs_Malloc(uint32 size, void** buf)
{
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

SpmiOs_ClkStatus SpmiOs_GetSerialClkState(SpmiDevInfo *pDev)
{
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

SpmiOs_ClkStatus SpmiOs_GetAhbClkState(SpmiDevInfo *pDev)
{
    return SPMI_CLK_STATUS_NOT_SUPPORTED;
}

uint64 SpmiOs_GetTimeTick() 
{
    uint64 Ticks = 0;
    return (GetTimeInNanoSecond (Ticks)/1000);
}

/**
 * This API will only be called from SPMIRuntimeLib, so we should just return
 * the Physical Address. Note that the call will usually occur in boottime
 */
Spmi_Result SpmiBus_OsGetCoreBaseAddress(
        uint32 uCoreIndex,
        UINTN *puAddress)
{
    if (puAddress == NULL)
    {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    switch(uCoreIndex)
    {
        case 0:
            *puAddress = PcdGet32(SPMIBasePhysical0);
            break;
            
        default:
            return SPMI_FAILURE_INVALID_BUS_ID;
    }
    
    return SPMI_BUS_SUCCESS;
}

/**
 * This API will only be called from SPMIRuntimeLib, so we should just set
 * whatever address is sent by the caller. This will always occur in Runtime
 */
Spmi_Result SpmiBus_OsSetCoreBaseAddress(
        uint32 uCoreIndex,
        UINTN uAddress)
{
    /* Use case is only defined in runtime */ 
    switch(uCoreIndex)
    {
        case 0:
            gpuPmicArbSpmiVirtualAddress = uAddress;
            break;
                        
        default:
            return SPMI_FAILURE_INVALID_BUS_ID;
    }
    
    return SPMI_BUS_SUCCESS;
}

/**
 * This API will return SPMI core memory map size. 
 * This API is called from SPMIRuntimeLib.
 * 
 */
Spmi_Result SpmiBus_OsGetCoreMemoryMapSize(
        uint32 uCoreIndex,
        uint32 *memMapSize)
{
    if (memMapSize == NULL)
    {
        return SPMI_FAILURE_INVALID_PARAMETER;
    } 

    switch(uCoreIndex)
    {
        case 0:
            *memMapSize = PcdGet32(SPMIMemoryMapSize);
            break;
                        
        default:
            return SPMI_FAILURE_INVALID_BUS_ID;
    }
    
    return SPMI_BUS_SUCCESS;
}

