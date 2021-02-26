/** @file SPMILibNull.c
  
  Stub functions for SPMILib

  Copyright (c) 2015, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
06/20/16    kpa     Added SpmiBusCfg_Init
05/08/15    plc     Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFISPMI.h>
#include "SpmiBus.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 

Spmi_Result SpmiBus_ReadLong(uint32 slaveId,
                             Spmi_AccessPriority priority,
                             uint32 address,
                             uint8* data,
                             uint32 len,
                             uint32* bytesRead)
{
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBus_ReadModifyWriteLongByte(uint32 slaveId,
                                            Spmi_AccessPriority priority,
                                            uint32 address,
                                            uint32 data,
                                            uint32 mask,
                                            uint8 *byteWritten)
{
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBus_Command(uint32 slaveId, Spmi_AccessPriority priority, Spmi_Command cmd)
{	
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBus_Init()
{
    return SPMI_SUCCESS;
}

/**
 * This API will only be called from SPMIRuntimeLib, so we should just set
 * whatever address is sent by the caller. This will always occur in Runtime
 */
Spmi_Result SpmiBus_OsSetCoreBaseAddress(
        uint32 uCoreIndex,
        UINTN uAddress)
{
    return SPMI_BUS_SUCCESS;
}

/**
 * This API will only be called from SPMIRuntimeLib, so we should just return
 * the Physical Address. Note that the call will usually occur in boottime
 */
Spmi_Result SpmiBus_OsGetCoreBaseAddress(
        uint32 uCoreIndex,
        UINTN *puAddress)
{
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
    return SPMI_BUS_SUCCESS;
}

Spmi_Result SpmiBus_WriteLong(uint32 slaveId,
                              Spmi_AccessPriority priority,
                              uint32 address,
                              uint8* data,
                              uint32 len)
{
    return SPMI_BUS_SUCCESS;
}


Spmi_Result SpmiBusCfg_Init(boolean initHw)
{
    return SPMI_BUS_SUCCESS;
}   
   
