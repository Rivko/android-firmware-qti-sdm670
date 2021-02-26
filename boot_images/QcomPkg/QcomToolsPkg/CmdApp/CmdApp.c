/** @file CmdApp.c

  Application to act as an app specified as param. This is
  essentially multiple apps combined into one.

  Copyright 2012 - 2017 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     -----------------------------------------------------------
  09/08/17     kpa     Added support for MountFv tool
  09/04/17     gk      UEFI USB compliance test changes
  07/28/17     pm      Added support to set UsbHLOSPortRole value
  06/22/16     ck      Remove the option to use secondary port for the debugger
  06/15/16     bd      Remove offlinecrashdump library dependency from QcomLib  
  06/08/16     sj      Changed mass storage enter mode from app
  03/22/16     bh      Adding BootOrder creation for 64-bit BDS testing
  11/18/15     va      UEFI Factoy ship mode support 
  08/13/15     al      Adding Charging related
  05/28/15     rli     Add toggle for UEFI File Logging
  10/02/13     ck      Added SetBootUsbFirstDelayValue
  08/29/13     ml      Added support for SSUsbfn
  05/13/2013   niting  Add options to support memory dump
  05/12/2013   niting  Change HotKey Menu option
  05/09/2013   yg      Fix Argument issue
  05/02/2013   sb      Add ToggleUseUsbFnPrimPort
  04/03/2013   yg      Added Low Power Mode command
  03/20/2013   nk      Added XPU Error Fatal
  02/20/2013   niting  Added flag to toggle volatile boot options
  02/15/2013   niting  Added ability to keep display ON during charging
  02/14/2013   dy      Add ToggleDisplay
  02/14/2013   mliss   Add ToggleUseSynopsys
  02/07/2013   dy      Add ConfigRsense
  11/13/2012   dy      Add DisableChgApp
  09/11/2012   yg      Add DisableHotkeys
  08/22/2012   yg      Add Boot USBFirst
  08/22/2012   vk      Add ToggleEnableShell
  07/27/2012   yg      Convert DisableWP to toggle function
  07/24/2012   yg      Add Disable write protection cmd
  07/18/2012   yg      Fix memory corruption issue
  07/17/2012   vishalo Added DispInfo
  07/17/2012   yg      Handle Unicode and Ascii cases
  07/12/2012   yg      Initial version
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/EfiFileLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Protocol/BlockIo.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ParserLib.h>
#include <Protocol/LoadedImage.h>
#include "AppCmds.h"
#include <Library/GuidedFvLib.h>
#include <Library/OfflineCrashDump.h>


extern EFI_GUID gToolsFvGuid;
extern EFI_GUID gTestsFvGuid;

/* In this function process the cmdstr to compare what command we would
 * like to run and get argstr needed for that command */
INTN
ProcessCommand (UINTN Argc, CHAR8** Argv)
{
  CHAR8 *CmdStr, *ArgStr = NULL;

  CmdStr = Argv[0];
  --Argc;

  if (Argc)
    ArgStr = Argv[1];

#if 0
  AsciiPrint ("Processing Cmd: %a", CmdStr);
  if (ArgStr)
     AsciiPrint (", Arg: %a", ArgStr);
  AsciiPrint ("\r\n");
#endif

  if (AsciiStriCmp ((CHAR8*)CmdStr, "reboot") == 0)
    gST->RuntimeServices->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "shutdown") == 0)
    gST->RuntimeServices->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "massstorage") == 0)
    LaunchAppFromGuidedFv(&gToolsFvGuid, "UsbfnMsdApp", NULL);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "edl") == 0)
    EnterEDLMode ();

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "DispInfo") == 0)
    DispInfo ((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggleWriteProtect") == 0)
    ToggleWriteProtection ((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggleEnableShell") == 0)
    ToggleEnableShell ((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "BootUSBFirst") == 0)
    BootUSBFirst((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggleBDSHotKeyState") == 0)
    ToggleBDSHotKeyState((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggeClockLoggingState") == 0)
    ToggeClockLoggingState((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "DumpClockLoggingState") == 0)
    DumpClockLoggingState((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggleEnableUsbHostPrimPort") == 0)
    ToggleEnableUsbHostPrimPort ((CHAR8*) ArgStr);
  
  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggleEnableHsUartBaudRate") == 0)
    ToggleEnableHsUartBaudRate ((CHAR8*) ArgStr);
    
  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggleSSUsbfnForceSS") == 0)
    ToggleSSUsbfnForceSS ((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggleForceUsbDeviceModeCompliance") == 0)
	  ToggleForceUsbDeviceModeCompliance ((CHAR8*) ArgStr);


  else if (AsciiStriCmp((CHAR8 *)CmdStr, "SetBootUsbFirstDelayValue") == 0)
    SetBootUsbFirstDelayValue((CHAR8 *)ArgStr);

  else if (AsciiStriCmp((CHAR8 *)CmdStr, "SetUsbHLOSPortRole") == 0)
    SetUsbHLOSPortRole((CHAR8 *)ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggleDisplayDuringCharging") == 0)
    ToggleDisplayDuringCharging((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggleEnableVolatileBootOptions") == 0)
    ToggleEnableVolatileBootOptions((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggleEnableLegacySBLDump") == 0)
    ToggleEnableLegacySBLDump((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ToggleOfflineMemoryDumpUseCapability") == 0)
    ToggleOfflineMemoryDumpUseCapability((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "XpuErrFatal") == 0)
    XpuErrFatal((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "LowPowerMode") == 0)
    LowPowerMode((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "PrintChargerAppDbgMsg") == 0)
    PrintChargerAppDbgMsg((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "FileLoggingToggle") == 0)
    FileLoggingToggle((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "Add64BitBootOrder") == 0)
    Add64BitBootOrder((CHAR8*) ArgStr);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "ChargerTest") == 0)
      LaunchAppFromGuidedFv(&gTestsFvGuid, "ChargerTest", NULL);

  else if (AsciiStriCmp ((CHAR8*)CmdStr, "MountFv") == 0)
    MountFvCommand((CHAR8*) ArgStr);   

  else
    AsciiPrint ("Unable to find matching command: %a\r\n", (CHAR8*)CmdStr);
  
  //Sync UEFI variable table after changes
  FlushVariableServices();

  /* Return success */
  return 0;
}

/**
  Entry point for Cmd App

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS           Erase successfully.
  @retval EFI_DEVICE_ERROR      Failed to program
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory for
                                the buffers
**/

EFI_STATUS
EFIAPI
CmdAppMain ( IN EFI_HANDLE         ImageHandle,
             IN EFI_SYSTEM_TABLE   *SystemTable)
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINTN                       ErrorCode = 0;
  UINTN                       Argc;
  CHAR8                     **Argv, **TempArgv;

  Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);

  if (EFI_ERROR(Status) || (Argc == 0))
    return Status;

  TempArgv = Argv;

  /* Check if the app was launched by shell and the Cmd str is actually the
   * Cmd app name itself, ie it would be "cmd" or "cmd.efi", then again process
   * to split, this is because Ebl doesn't pass the name of app, but shell does
   * other methods could yeild mixed results (like BDS launching apps via Menu, etc) */
  if ((AsciiStriCmp (Argv[0], "cmd") == 0) ||
      (AsciiStriCmp (Argv[0], "cmd.efi") == 0))
  {
    --Argc;
    if (Argc == 0)
    {
      FreePool (Argv);
      AsciiPrint ("No Command to process... \r\n");
      return EFI_SUCCESS;
    }
    ++Argv;
  }

  /* Now both cmdStr and ArgStr have proper values, do string compare and
   * implement each command, Argument processing will be done as per the
   * command needs */
  ErrorCode = ProcessCommand (Argc, Argv);

  FreePool (TempArgv);

  if (ErrorCode != 0)
    AsciiPrint ("Error Code: %d\r\n", ErrorCode);

  AsciiPrint ("\r\nPress any key to exit...\r\n");
  ReadAnyKey (NULL, 0);

  return Status;
}

