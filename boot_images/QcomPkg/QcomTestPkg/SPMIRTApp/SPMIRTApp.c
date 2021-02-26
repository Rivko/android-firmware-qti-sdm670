/** @file SPMIRTApp.c
   
  SPMI App

  Copyright (c) 2012, 2015 , 2018, 2019 by Qualcomm Technologies, Inc. All Rights Reserved
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 07/13/15   al      Changing TPL_NOTIFY to TPL_CALLBACK
 08/02/12   unr     Initial revision of SPMI Runtime App

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>

#include "SpmiBus.h"

#define MAX_BUF_SIZE 4

/**
  Prints the error strings on console

  @param[in] error    Error code defined in SpmiBus_ResultType

  @retval none

 */
VOID SPMIErrorPrint(SpmiBus_ResultType error)
{
  switch (error)
  {
    case SPMI_BUS_FAILURE_INIT_FAILED:
      gST->ConOut->OutputString(gST->ConOut, L"INIT_FAILED \n\r");
      break;

    case SPMI_BUS_FAILURE_INVALID_PARAMETER:
      gST->ConOut->OutputString(gST->ConOut, L"INVALID_PARAMETER \n\r");
      break;

    case SPMI_BUS_FAILURE_GENERAL_FAILURE:
      gST->ConOut->OutputString(gST->ConOut, L"GENERAL_FAILURE \n\r");
      break;

    case SPMI_BUS_FAILURE_TRANSACTION_FAILED:
      gST->ConOut->OutputString(gST->ConOut, L"TRANSACTION_FAILED \n\r");
      break;

    case SPMI_BUS_FAILURE_TRANSACTION_DENIED:
      gST->ConOut->OutputString(gST->ConOut, L"TRANSACTION_DENIED \n\r");
      break;

    case SPMI_BUS_FAILURE_TRANSACTION_DROPPED:
      gST->ConOut->OutputString(gST->ConOut, L"TRANSACTION_DROPPED \n\r");
      break;

    default:
      break;
  }
}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

 **/

EFI_STATUS
EFIAPI
SPMIRTAppMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
  EFI_STATUS Status = EFI_SUCCESS;
  SpmiBus_ResultType result = SPMI_BUS_SUCCESS;
  SpmiBus_AccessPriorityType ePriority = SPMI_BUS_ACCESS_PRIORITY_LOW;
  EFI_TPL PrevTPL;

  UINT8 ucData[MAX_BUF_SIZE] = { 0, 0, 0, 0 };
  uint32 ReadNum;

  uint32 SlaveId = PcdGet32(SPMIPOnID);
  uint32 TypeReg = PcdGet32(SPMIPOnTypeReg);
  UINT32 TypeVal = PcdGet32(SPMIPOnType);
  uint32 WriteReg = PcdGet32(SPMIPOnWriteReg);

  TEST_START("SPMIRTApp");

  SpmiBus_Init();

  /* Read Type */
  PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);
  result = SpmiBus_ReadLong(SlaveId, ePriority, TypeReg, ucData, 2, &ReadNum);
  gBS->RestoreTPL(PrevTPL);
  if (SPMI_BUS_SUCCESS != result)
  {
    goto exit;
  }

  if((ucData[0] != (TypeVal & 0xFF)) ||
     (ucData[1] != ((TypeVal & 0xFF00) >> 8)))
  {
    gST->ConOut->OutputString(gST->ConOut, L"Type-Subtype verification failed \n\r");
    goto exit;
  }

  /* Write a register and read the value back */
  if (WriteReg != 0)
  {
    ucData[0] = 0x02;
    PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);
    result = SpmiBus_WriteLong(SlaveId, ePriority, WriteReg, &ucData[0], 1);
    gBS->RestoreTPL(PrevTPL);
    if (SPMI_BUS_SUCCESS != result)
    {
      goto exit;
    }

    ucData[1] = 0;
    PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);
    result = SpmiBus_ReadLong(SlaveId, ePriority, WriteReg, &ucData[1], 1, &ReadNum);
    gBS->RestoreTPL(PrevTPL);
    if (SPMI_BUS_SUCCESS != result)
    {
      goto exit;
    }

    if(ucData[0] != ucData[1])
    {
      gST->ConOut->OutputString(gST->ConOut, L"Data Written does not match Data Read \n\r");
      goto exit;
    }

    ucData[1] = 0xFC;
    ucData[2] = 0x1E;
    PrevTPL = gBS->RaiseTPL(TPL_CALLBACK);
    result = SpmiBus_ReadModifyWriteLongByte(SlaveId, ePriority, WriteReg, ucData[1], ucData[2], &ucData[3]);
    gBS->RestoreTPL(PrevTPL);
    if (SPMI_BUS_SUCCESS != result)
    {
      goto exit;
    }

    /*  Result     = (ReadData   & ~Mask)      | (WriteData & Mask) */
    /*  0x1D       = (0x3        & ~0x1E)      | (0xFC      & 0x1E) */
    if (ucData[3] != ((ucData[0] & ~ucData[2]) | (ucData[1] & ucData[2])))
    {
      gST->ConOut->OutputString(gST->ConOut, L"Read Modify Write data mismatch\n\r");
      goto exit;
    }

  }

exit:

  if (SPMI_BUS_SUCCESS != result)
  {
      SPMIErrorPrint(result);
      Status = EFI_DEVICE_ERROR;
  }

  TestStatus("SPMRTIApp", Status);
  TEST_STOP("SPMRTIApp");

  return Status;
}