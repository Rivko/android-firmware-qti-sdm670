/** @file SBIMiniTest.c
   
  SBI MiniTest App

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
 08/02/11   unr     Initial revision for UEFI SBI Mini

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Library/SBIMiniInterface.h>
#include <Library/SBITestInterface.h>
#include <Library/TestInterface.h>

BOOLEAN SBIMiniTest(void);

#define SBI_ASSERT( exp )   \
  if( !(exp) )              \
  {                         \
    sbi_test_err_cnt++;     \
  }

UINT32 sbi_test_err_cnt = 0;

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
SBIMiniTestMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
  BOOLEAN result;
  TEST_START("SBIMiniTest");
  result = SBIMiniTest(); 
  if (TRUE != result)
  {
    gST->ConOut->OutputString(gST->ConOut, L"SBI Test Failed \n\r");
    TestStatus("SBIMiniTest",EFI_DEVICE_ERROR);
    TEST_STOP("SBIMiniTest");
    return EFI_DEVICE_ERROR;
  }
  else
  {
    gST->ConOut->OutputString(gST->ConOut, L"SBI Test Passed \n\r");
    TestStatus("SBIMiniTest",EFI_SUCCESS);
    TEST_STOP("SBIMiniTest");
    return EFI_SUCCESS;
  }
}

BOOLEAN SBIMiniTest(void)
{
  BOOLEAN SbiStatus = FALSE;

  SBI_TEST_DESCRIPTOR           *TestDesc = NULL;
  SBI_TEST_TRANSFER_DESCRIPTOR  *TxDesc   = NULL;

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
    return FALSE;
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
    SBIMiniInit(DevIndex);

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
        SbiStatus = SBIMiniReadByte(DevIndex, TxDesc->addr, &DataSaved);
        SBI_ASSERT(SbiStatus == TRUE);
        AsciiPrint("SBI Test Save    [0x%04x]:0x%04x\n", TxDesc->addr, DataSaved);
      }

      if (TxDesc->type == SBI_TEST_WRITE)
      {
        AsciiPrint("SBI Test Write   [0x%04x]:0x%04x\n", TxDesc->addr, TxDesc->data);
        SbiStatus = SBIMiniWriteByte(DevIndex, TxDesc->addr, TxDesc->data);
        SBI_ASSERT(SbiStatus == TRUE);
      }

      if (TxDesc->type == SBI_TEST_READ)
      {
        SbiStatus = SBIMiniReadByte(DevIndex, TxDesc->addr, &DataRead);
        SBI_ASSERT(SbiStatus == TRUE);
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
        SbiStatus = SBIMiniWriteByte(DevIndex, TxDesc->addr, DataSaved);
        SBI_ASSERT(SbiStatus == TRUE);
      }

      TxDesc++;
    }

    if(sbi_test_err_cnt > 0)
    {
      gST->ConOut->OutputString(gST->ConOut, L"SBI Test Read/Write Failed\n\r");
    }
  }

  return SbiStatus;
}
