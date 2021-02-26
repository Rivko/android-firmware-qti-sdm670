/**
 * @file:  SpmiApi.c
 * 
 * Copyright (c) 2014 by Qualcomm Technologies Incorporated. All Rights Reserved.
 * 
 * $Change: 14402555 $ 
 * 
 *                              Edit History
 * Date     Description
 * -------  -------------------------------------------------------------------
 * 7/24/13  Initial Version
 */

#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFISPMI.h>
#include "SpmiBus.h"

//******************************************************************************
// Global Data
//******************************************************************************

static EFI_QCOM_SPMI_PROTOCOL *spmiIo = NULL;

//******************************************************************************
// Public API Functions
//******************************************************************************

Spmi_Result SpmiBus_Init()
{
    if(gBS->LocateProtocol( &gQcomSPMIProtocolGuid, NULL, (void**) &spmiIo ) != EFI_SUCCESS) {
        DEBUG( (EFI_D_ERROR, "Locate protocol failed\n") );
        return SPMI_FAILURE_INIT_FAILED;
    }

    if(spmiIo == NULL) {
        DEBUG( (EFI_D_ERROR, "Invalid SPMI protocol handle\n") );
        return SPMI_FAILURE_INIT_FAILED;
    }

    return SPMI_SUCCESS;
}

Spmi_Result SpmiBus_DeInit()
{
    return SPMI_SUCCESS;
}

Spmi_Result SpmiBus_ReadLong(uint32 slaveId,
                             Spmi_AccessPriority priority,
                             uint32 address,
                             uint8* data,
                             uint32 len,
                             uint32* bytesRead)
{
    if(spmiIo == NULL) {
        DEBUG((EFI_D_ERROR, "SPMI protocol handle is invalid\n"));
        return SPMI_FAILURE_NOT_INITIALIZED;
    }
    
    return spmiIo->ReadLong( spmiIo, slaveId, priority, address, data, len, bytesRead );
}

Spmi_Result SpmiBus_WriteLong(uint32 slaveId,
                              Spmi_AccessPriority priority,
                              uint32 address,
                              uint8* data,
                              uint32 len)
{
    if(spmiIo == NULL) {
        DEBUG((EFI_D_ERROR, "SPMI protocol handle is invalid\n"));
        return SPMI_FAILURE_NOT_INITIALIZED;
    }
    
    return spmiIo->WriteLong( spmiIo, slaveId, priority, address, data, len );
}

Spmi_Result SpmiBus_Command(uint32 slaveId, Spmi_AccessPriority priority, Spmi_Command cmd)
{
    if(spmiIo == NULL) {
        DEBUG((EFI_D_ERROR, "SPMI protocol handle is invalid\n"));
        return SPMI_FAILURE_NOT_INITIALIZED;
    }
    
    return spmiIo->Command( spmiIo, slaveId, priority, cmd );
}

Spmi_Result SpmiBus_RegisterIsr(uint8 slaveId,
                                uint8 periph,
                                uint8 mask,
                                const SpmiIsr isr,
                                const void* ctx,
                                SpmiToken* token)
{
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

Spmi_Result SpmiBus_UnregisterIsr(SpmiToken* token, uint8 mask)
{
    return SPMI_FAILURE_FUNCTIONALITY_NOT_SUPPORTED;
}

Spmi_Result SpmiBus_ReadModifyWriteLongByte(uint32 slaveId,
                                            Spmi_AccessPriority priority,
                                            uint32 address,
                                            uint32 data,
                                            uint32 mask,
                                            uint8 *byteWritten)
{
    if(spmiIo == NULL) {
        DEBUG((EFI_D_ERROR, "SPMI protocol handle is invalid\n"));
        return SPMI_FAILURE_NOT_INITIALIZED;
    }
    
    return spmiIo->ReadModifyWriteLongByte( spmiIo, slaveId, priority, address, data, mask, byteWritten );
}
