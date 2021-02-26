/** @file SPMIDxe.c

  SPMI UEFI Driver code

  Copyright (c) 2012,2014,2015 Qualcomm Technologies, Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 09/20/12   unr     Added ReadModifyWriteLongByte API
 07/05/12   unr     Implemented a wrapper layer
 06/27/12   unr     Initial revision of UEFI SPMI Driver

=============================================================================*/

#include <Uefi.h>

/**
  SPMI PROTOCOL interfaces
 */
#include <Protocol/EFISPMI.h>

/**
  EFI interfaces
 */
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>

/**
  SPMI library interfaces
 */
#include "SpmiBus.h"

/**
  SPMI_READLONG implementation
 */
Spmi_Result
EfiSpmiBus_ReadLong (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    UINT32 uRegisterAddress,
    UINT8 *pucData,
    UINT32 uDataLen,
    UINT32 *puTotalBytesRead)
{
    UINT32 TotalBytesRead = 0;
    Spmi_Result result;
    EFI_TPL PrevTPL;

    if (SpmiProtocolHandle == NULL)
    {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

    result = SpmiBus_ReadLong(
        (uint32) uSlaveId,
        eAccessPriority,
        (uint32) uRegisterAddress, 
        (uint8 *) pucData,
        (uint32) uDataLen, 
        (uint32 *) (&TotalBytesRead));

    gBS->RestoreTPL(PrevTPL);

    if (puTotalBytesRead != NULL)
    {
        *puTotalBytesRead = TotalBytesRead;
    }

    return result;
}

/**
  SPMI_WRITELONG implementation
 */
Spmi_Result
EfiSpmiBus_WriteLong (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    UINT32 uRegisterAddress,
    UINT8 *pucData,
    UINT32 uDataLen)
{
    Spmi_Result result;
    EFI_TPL PrevTPL;

    if (SpmiProtocolHandle == NULL)
    {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

    result = SpmiBus_WriteLong(
        (uint32) uSlaveId,
        eAccessPriority,
        (uint32) uRegisterAddress,
        (uint8 *) pucData, 
        (uint32) uDataLen);

    gBS->RestoreTPL(PrevTPL);

    return result;
}

/**
  SPMI_READMODIFYWRITELONGBYTE implementation
 */
Spmi_Result
EfiSpmiBus_ReadModifyWriteLongByte (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    UINT32 uRegisterAddress,
    UINT32 uWriteData, 
    UINT32 uMask,
	UINT8 *pucDataWritten
    )
{
    Spmi_Result result;
    EFI_TPL PrevTPL;

    if (SpmiProtocolHandle == NULL)
    {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

    result = SpmiBus_ReadModifyWriteLongByte(
        (uint32) uSlaveId,
        eAccessPriority,
        (uint32) uRegisterAddress,
        (uint32) uWriteData, 
        (uint32) uMask,
        (uint8 *) pucDataWritten);

    gBS->RestoreTPL(PrevTPL);

    return result;
}

/**
  SPMI_COMMAND implementation
 */
Spmi_Result
EfiSpmiBus_Command (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    Spmi_Command eSpmiCommand)
{
    Spmi_Result result;
    EFI_TPL PrevTPL;

    if (SpmiProtocolHandle == NULL)
    {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

    result = SpmiBus_Command(
        (uint32) uSlaveId,
        eAccessPriority,
        eSpmiCommand); 

    gBS->RestoreTPL(PrevTPL);

    return result;
}

/**
  SPMI_READLONGDEBUG implementation
 */
Spmi_Result
EfiSpmiBus_ReadLongDebug (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    UINT32 uRegisterAddress,
    UINT8 *pucData,
    UINT32 uDataLen)
{
    Spmi_Result result;
    EFI_TPL PrevTPL;

    if (SpmiProtocolHandle == NULL)
    {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

    result = SpmiBus_ReadLongDebug(
        (uint32) uSlaveId,
        eAccessPriority,
        (uint32) uRegisterAddress,
        (uint8 *) pucData, 
        (uint32) uDataLen);

    gBS->RestoreTPL(PrevTPL);

    return result;
}

/**
  SPMI_WRITELONGDEBUG implementation
 */
Spmi_Result
EfiSpmiBus_WriteLongDebug (
    EFI_QCOM_SPMI_PROTOCOL *SpmiProtocolHandle,
    UINT32 uSlaveId,
    Spmi_AccessPriority eAccessPriority,
    UINT32 uRegisterAddress,
    UINT8 *pucData,
    UINT32 uDataLen)
{
    Spmi_Result result;
    EFI_TPL PrevTPL;

    if (SpmiProtocolHandle == NULL)
    {
        return SPMI_FAILURE_INVALID_PARAMETER;
    }

    PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);

    result = SpmiBus_WriteLongDebug(
        (uint32) uSlaveId,
        eAccessPriority,
        (uint32) uRegisterAddress,
        (uint8 *) pucData, 
        (uint32) uDataLen);

    gBS->RestoreTPL(PrevTPL);

    return result;
}


/**
  SPMI UEFI Protocol implementation
 */
EFI_QCOM_SPMI_PROTOCOL SPMIProtocolImplementation =
{
    SPMI_DXE_REVISION,
    EfiSpmiBus_ReadLong,
    EfiSpmiBus_WriteLong,
    EfiSpmiBus_ReadModifyWriteLongByte,
    EfiSpmiBus_Command,
    EfiSpmiBus_ReadLongDebug,
    EfiSpmiBus_WriteLongDebug
};

/**
  SPMI DXE Driver Entry Point
 */
EFI_STATUS
SPMIProtocolInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable)
{
    /* If we fail bus init, we do not install the protocol */
    if (SPMI_SUCCESS != SpmiBus_Init())
    {
        return EFI_DEVICE_ERROR;
    }

    return gBS->InstallMultipleProtocolInterfaces( &ImageHandle,
                                                   &gQcomSPMIProtocolGuid,
                                                   &SPMIProtocolImplementation,
                                                   NULL);
}
