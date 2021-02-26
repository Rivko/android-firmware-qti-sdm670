/** @file MenuApp.c
   
  Application to launch configurable menu. The menu cfg file can be
  Passed in as param.
  
  Copyright 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     -----------------------------------------------------------
  07/01/2013   yg      Initial version
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include <Library/QcomUtilsLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/LoadedImage.h>


/**
  Entry point for Menu App

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS           Erase successfully.
  @retval EFI_DEVICE_ERROR      Failed to program
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory for 
                                the buffers
**/

EFI_STATUS
EFIAPI
MenuAppMain ( IN EFI_HANDLE         ImageHandle,
              IN EFI_SYSTEM_TABLE   *SystemTable)
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINTN                       ErrorCode = 0;
  UINTN                       Argc;
  CHAR8                     **Argv, **TempArgv;

  Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);

  if (EFI_ERROR(Status))
    return Status;

  TempArgv = Argv;

  /* Check if the app was launched by shell and the Cmd str is actually the
   * Cmd app name itself, ie it would be "cmd" or "cmd.efi", then again process
   * to split, this is because Ebl doesn't pass the name of app, but shell does
   * other methods could yeild mixed results (like BDS launching apps via Menu, etc) */
  if (Argc &&
      ((AsciiStriCmp (Argv[0], "menu") == 0) ||
       (AsciiStriCmp (Argv[0], "menu.efi") == 0)))
  {
    --Argc;
    ++Argv;
  }

  /* Now both cmdStr and ArgStr have proper values. If no argument, then we can launch
   * BDS Menu by default */
  if (Argc == 0)
    ErrorCode = LaunchMenu ("BDS_Menu.cfg");
  else
    ErrorCode = LaunchMenu (Argv[0]);

  if (TempArgv)
    FreePool (TempArgv);

  if (ErrorCode != 0)
    AsciiPrint ("Error Code: %d\r\n", ErrorCode);

  AsciiPrint ("\r\nPress any key to exit...\r\n");
  ReadAnyKey (NULL, 0);

  return Status;
}

