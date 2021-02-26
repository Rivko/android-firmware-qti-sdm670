/** @file ResetTest.c
   
  Reset device specific number of times

  Copyright (c) 2015, Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/14/15   bh      Update to take argument for number of reboots
 09/02/15   vk      Initial Revision

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/EfiFileLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Protocol/BlockIo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/LoadedImage.h>

EFI_STATUS
EFIAPI
ResetTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  UINTN      Argc;
  CHAR8    **Argv;
  UINTN      NumReboots = 0;
  UINTN      RebootSize = 0;
  UINTN      BootCycles = 0;
  UINTN      VarSize = 0;

  RebootSize = sizeof(NumReboots);
  VarSize = sizeof(BootCycles);
  Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);

  if (Argc == 1)
  {
    NumReboots = (UINTN) AsciiStrDecimalToUintn(Argv[0]);
    if (NumReboots == 0)
    {
      AsciiPrint("Input error, please enter a valid integer > 0\n");
      return Status;
    }

    Status = gRT->SetVariable(L"ResetTestReqReboot", &gQcomTokenSpaceGuid,
                         (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                         sizeof(NumReboots), &NumReboots);

    Status = gRT->SetVariable(L"ResetTestNumIter", &gQcomTokenSpaceGuid,
                         (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                         sizeof(BootCycles), &BootCycles);

    AsciiPrint("Number of Reboots required has been set: %d\n", NumReboots);
    AsciiPrint("Number of iterations done has been reset\n");
  }

  Status = gRT->GetVariable(L"ResetTestReqReboot", 
                            &gQcomTokenSpaceGuid, 
                            NULL, 
                            &RebootSize, 
                            &NumReboots);
  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("ResetTestReqReboot value not found, setting default to 5\n");
    NumReboots = 5;
    Status = gRT->SetVariable(L"ResetTestReqReboot", &gQcomTokenSpaceGuid,
                         (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                         sizeof(NumReboots), &NumReboots);
  }

  AsciiPrint("Reboots required: %d\n", NumReboots);

  VarSize = sizeof(BootCycles);

  Status = gRT->GetVariable(L"ResetTestNumIter",
                            &gQcomTokenSpaceGuid,
                            NULL,
                            &VarSize,
                            &BootCycles);

  if (Status != EFI_SUCCESS)
  {
    AsciiPrint("ResetTestNumIter value was not found, setting default of 0\n");
    BootCycles = 0;
    Status = gRT->SetVariable(L"ResetTestNumIter",
                              &gQcomTokenSpaceGuid,
                              (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                              sizeof(VarSize), &BootCycles);
  }

  AsciiPrint("Reboots done: %d\n", BootCycles);
  if (BootCycles < NumReboots)
  {
    BootCycles++;

    Status = gRT->SetVariable (L"ResetTestNumIter", &gQcomTokenSpaceGuid,
                           (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE),
                           VarSize, &BootCycles);

    Status = AddDevPathToBootOrder (ImageHandle, NULL, L"Reset Test", NULL, 0, 0);
    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: Unable to add to boot order\n");
    }
  
    /* Do a cold reset */
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

  AsciiPrint("Required reboots met\n");
  RemoveFromBootOrder (L"Reset Test");

  AsciiPrint("RESET TEST SUCCEEDED\n");
  return EFI_SUCCESS;
}

