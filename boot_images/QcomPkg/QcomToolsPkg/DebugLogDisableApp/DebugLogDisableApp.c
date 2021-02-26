/** @file DebugLogDisableApp.c
   
  Tests for disabling debug logs.

  Copyright (c) 2011, Qualcomm Technologies Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 10/09/11   dp      Initial Revision

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/TestInterface.h>

/**
  Entry point for the application

  @param[in] ImageHandle    Image handle 
  @param[in] SystemTable    Pointer to the System Table
  
  @retval EFI_SUCCESS       Execution successful
  @retval other             Error occurred

**/
EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32 cookie_addr, cookie_magic_value;
  UINT32 read_value;
  TEST_START("DebugLogDisableApp");

  cookie_addr = PcdGet32(PcdDebugLogOverrideAddress);
  cookie_magic_value = PcdGet32(PcdDebugLogOverrideMagic);
  
  AsciiPrint("Cookie Address: 0x%x, Magic: 0x%x\n", cookie_addr, cookie_magic_value);

  read_value = *((UINT32 *)cookie_addr);
  if (read_value != cookie_magic_value)
  {
    AsciiPrint("Cookie Address does not have magic value. Value = 0x%x\n", 
      read_value);
    *((UINT32 *)cookie_addr) = cookie_magic_value;
    AsciiPrint("DebugLogDisable cookie value set. Force Reset\n");
    SystemTable->RuntimeServices->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);   
  }
  else
  {
    AsciiPrint("DebugLogDisable cookie already set. Exiting!\n");
  }

  TestStatus("DebugLogDisableApp", Status);
  TEST_STOP("DebugLogDisableApp");
  return Status;
}
