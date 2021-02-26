/** @file StackCanary.c
   
  Stack Canary Test

  Copyright (c) 2017, , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/07/14   vk      Initial Revision

=============================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/BaseMemoryLib.h>

#define BUFFER_SIZE 5
#define WRITE_BYTES 10

VOID KillStack (CHAR8 *P)
{
  STATIC UINTN Count = 2;
  CHAR8 Buffer[BUFFER_SIZE];

  CopyMem (Buffer, P, WRITE_BYTES);
  while (Count--)
  {
    KillStack ("HelloAgain");
  }
  return;
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
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status = EFI_COMPROMISED_DATA;

  TEST_START("StackCanary");

  AsciiPrint("Stack Canary Test,__stack_chk_fail error means StackCanary Test has passed.\n");
  KillStack ("HelloWorld");

  TestStatus("StackCanary", Status);
  TEST_STOP("StackCanary");
  return Status;
}