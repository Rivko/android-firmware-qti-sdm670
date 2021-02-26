/** @file SBITest.c
   
  SBI Test App

  Copyright (c) 2010-2011, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
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
 09/13/11   unr     Add Test Library Support
 09/10/11   unr     Zero out old values read and saved
 04/10/11   unr     Added Save and Restore functionality
 02/07/10   unr     Initial revision for UEFI SBI

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Library/SBIInterface.h>
#include <Library/SBITestInterface.h>
#include <Library/TestInterface.h>

SBI_RESULT_TYPE SBITest(void);

#define SBI_ASSERT( exp )   \
  if( !(exp) )              \
  {                         \
    sbi_test_err_cnt++;     \
  }

UINT32 sbi_test_err_cnt = 0;

VOID SBIErrorPrint(SBI_RESULT_TYPE error)
{
  switch (error)
  {

    case EFI_SBI_ERROR_INVALID_ADDRESS_WIDTH:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_INVALID_ADDRESS_WIDTH \n\r");
      break;

    case EFI_SBI_ERROR_INVALID_FREQ:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_INVALID_FREQ \n\r");
      break;

    case EFI_SBI_ERROR_INVALID_TRANSFER_MODE:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_INVALID_TRANSFER_MODE \n\r");
      break;

    case EFI_SBI_ERROR_INVALID_SLAVE_ID:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_INVALID_SLAVE_ID \n\r");
      break;

    case EFI_SBI_ERROR_INVALID_PORT:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_INVALID_PORT \n\r");
      break;

    case EFI_SBI_ERROR_PARAMETER_SIZE_MISMATCH:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_PARAMETER_SIZE_MISMATCH \n\r");
      break;

    case EFI_SBI_ERROR_ZERO_LENGTH_PARAMETER:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_ZERO_LENGTH_PARAMETER \n\r");
      break;

    case EFI_SBI_ERROR_NULL_POINTER_PASSED:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_NULL_POINTER_PASSED \n\r");
      break;

    case EFI_SBI_ERROR_VERIFY_BYTE_MISMATCH:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_VERIFY_BYTE_MISMATCH \n\r");
      break;

    case EFI_SBI_ERROR_INVALID_OPERATION:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_INVALID_OPERATION \n\r");
      break;

    case EFI_SBI_ERROR_READ_WRITE_FAILURE:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_READ_WRITE_FAILURE \n\r");
      break;

    case EFI_SBI_ERROR_DEVICE_NOT_POWERED:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR_DEVICE_NOT_POWERED \n\r");
      break;

    case EFI_SBI_ERROR:
      gST->ConOut->OutputString(gST->ConOut, L"Error Code - EFI_SBI_ERROR \n\r");
      break;

    default:
      break;
  }
}

VOID SBIPrintCtrlType(SBI_CONTROLLER_TYPE ctrl)
{
  switch (ctrl)
  {
    case EFI_SBI_CTLR_SBI:
      gST->ConOut->OutputString(gST->ConOut, L"Controller: EFI_SBI_CTLR_SBI \n\r");
      break;

    case EFI_SBI_CTLR_SSBI:
      gST->ConOut->OutputString(gST->ConOut, L"Controller: EFI_SBI_CTLR_SSBI \n\r");
      break;

    case EFI_SBI_CTLR_SSBI2:
      gST->ConOut->OutputString(gST->ConOut, L"Controller: EFI_SBI_CTLR_SSBI2 \n\r");
      break;

    case EFI_SBI_CTLR_PMIC_ARBITER:
      gST->ConOut->OutputString(gST->ConOut, L"Controller: EFI_SBI_CTLR_PMIC_ARBITER \n\r");
      break;

    case EFI_SBI_CTLR_PMIC_ARBITER_CMD:
      gST->ConOut->OutputString(gST->ConOut, L"Controller: EFI_SBI_CTLR_ARBITER_CMD \n\r");
      break;

    default:
      gST->ConOut->OutputString(gST->ConOut, L"Controller: Unknown \n\r");
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
SBITestMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
  SBI_RESULT_TYPE result;
  TEST_START("SBITest");
  result = SBITest(); 
  if (EFI_SBI_SUCCESS != result)
  {
    SBIErrorPrint(result);
    gST->ConOut->OutputString(gST->ConOut, L"SBI Test Failed \n\r");
    TestStatus("SBITest",EFI_DEVICE_ERROR);
    TEST_STOP("SBITest");
    return EFI_DEVICE_ERROR;
  }
  else
  {
    gST->ConOut->OutputString(gST->ConOut, L"SBI Test Passed \n\r");
    TestStatus("SBITest",EFI_SUCCESS);
    TEST_STOP("SBITest");
    return EFI_SUCCESS;
  }
}

SBI_RESULT_TYPE SBITest(void)
{
  EFI_STATUS EfiStatus = EFI_SUCCESS;

  SBI_RESULT_TYPE               SbiStatus = EFI_SBI_ERROR;
  SBI_CONTROLLER_TYPE           CtrlType;
  SBI_TEST_DESCRIPTOR           *TestDesc = NULL;
  SBI_TEST_TRANSFER_DESCRIPTOR  *TxDesc   = NULL;

  VOID *SbiHandle = NULL;

  UINT32  NumDevices  = 0;
  UINT32  DevIndex    = 0;
  UINT32  DescIndex   = 0;
  UINT8   DataRead    = 0;
  UINT8   DataSaved   = 0;

  //
  // Get number of devices to test
  //
  NumDevices = SBITestGetNumDevices();
  if (0 == NumDevices)
  {
    gST->ConOut->OutputString(gST->ConOut, L"SBI Test: No test devices found \n\r");
    return EFI_SBI_ERROR;
  }
  else
  {
    AsciiPrint("SBI Test: %d test devices found\n", NumDevices);
  }

  for (DevIndex = 0; DevIndex < NumDevices; DevIndex++)
  {
    AsciiPrint("SBI Test Device: %d\n", DevIndex);
    //
    // Get test descriptor
    //
    TestDesc = SBIGetTestDescriptor (DevIndex);
    if (NULL == TestDesc)
    {
      AsciiPrint("SBI Test Test Descriptor for device %d NULL\n", DevIndex);
      continue;
    }

    //
    // Initialize the device
    //
    EfiStatus = SBIInit((SBI_DEVICE) DevIndex, &SbiHandle);
    if (EFI_SUCCESS != EfiStatus)
    {
      if (EFI_DEVICE_ERROR == EfiStatus)
      {
        gST->ConOut->OutputString(gST->ConOut, L"SBI Test Driver Init failed \n\r");
      }

      if (EFI_INVALID_PARAMETER == EfiStatus)
      {
        gST->ConOut->OutputString(gST->ConOut, L"SBI Test Invalid Parameter to SBIInit\n\r");
      }

      return EFI_SBI_ERROR;
    }

    SbiStatus = SBIGetControllerType (SbiHandle, &CtrlType);
    if (EFI_SBI_SUCCESS != SbiStatus)
    {
      SBIErrorPrint(SbiStatus);
      gST->ConOut->OutputString(gST->ConOut, L"SBI Test SBIGetControllerType Failed\n\r");
      return SbiStatus;
    }

    SBIPrintCtrlType(CtrlType);

    sbi_test_err_cnt = 0;
    DataSaved = 0;
    DataRead  = 0;

    //
    // Execute the array of transfer descriptors. A write verify corresponds to
    // 2 entries in the transfer descriptor array - a write and then a read.
    //
    TxDesc = TestDesc->descriptor;
    for (DescIndex = 0; DescIndex < TestDesc->num_descriptors; DescIndex++)
    {
      if (TxDesc->type == SBI_TEST_SAVE)
      {
        SbiStatus = SBIReadByte(SbiHandle, TxDesc->addr, &DataSaved);
        SBI_ASSERT(SbiStatus == EFI_SBI_SUCCESS);
        AsciiPrint("SBI Test Save    [0x%04x]:0x%04x\n", TxDesc->addr, DataSaved);
      }

      if (TxDesc->type == SBI_TEST_WRITE)
      {
        AsciiPrint("SBI Test Write   [0x%04x]:0x%04x\n", TxDesc->addr, TxDesc->data);
        SbiStatus = SBIWriteByte(SbiHandle, TxDesc->addr, TxDesc->data);
        SBI_ASSERT(SbiStatus == EFI_SBI_SUCCESS);
      }

      if (TxDesc->type == SBI_TEST_READ)
      {
        SbiStatus = SBIReadByte(SbiHandle, TxDesc->addr, &DataRead);
        SBI_ASSERT(SbiStatus == EFI_SBI_SUCCESS);
        SBI_ASSERT(DataRead == TxDesc->data);
        AsciiPrint("SBI Test Read    [0x%04x]:0x%04x\n", TxDesc->addr, DataRead);
        if (DataRead != TxDesc->data)
        {
            AsciiPrint("SBI Test Read Failed [0x%04x]:Expected = 0x%04x:Actual = 0x%04x\n", TxDesc->addr, TxDesc->data, DataRead);
        }
      }

      if (TxDesc->type == SBI_TEST_RESTORE)
      {
        AsciiPrint("SBI Test Restore [0x%04x]:0x%04x\n", TxDesc->addr, DataSaved);
        SbiStatus = SBIWriteByte(SbiHandle, TxDesc->addr, DataSaved);
        SBI_ASSERT(SbiStatus == EFI_SBI_SUCCESS);
      }

      TxDesc++;
    }

    if(sbi_test_err_cnt > 0)
    {
      gST->ConOut->OutputString(gST->ConOut, L"SBI Test Read/Write Failed\n\r");
    }

    EfiStatus = SBIDeInit(SbiHandle);
    if (EFI_SUCCESS != EfiStatus)
    {
      gST->ConOut->OutputString(gST->ConOut, L"SBI Test Driver DeInit failed\n\r");
    }

    SbiHandle = NULL;
  }

  return SbiStatus;
}
