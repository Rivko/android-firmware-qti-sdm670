/** @file CrashDumpFlagApp.c
   
  Application to set flags to enable/disable offline crash dump feature.
  
  Copyright 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     -----------------------------------------------------------
  06/25/15     bh      Change guid name
  03/22/2013   niting  Initial revision
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/OfflineCrashDump.h>

STATIC UINT32 AbnormalResetOccurred = 0xFFFFFFFF;
STATIC UINT32 OfflineMemoryDumpCapable = 0xFFFFFFFF;
STATIC UINT8 OfflineMemoryDumpUseCapability = 0xFF;
STATIC UINT8 EnableLegacySBLDump = 0xFF;
STATIC BOOLEAN UpdateRequired = FALSE;
STATIC EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE *OfflineCrashDumpConfigTable = NULL;

VOID
EFIAPI
PrintOfflineCrashDumpValuesApp ( VOID )
{
  UINT32 MemoryCaptureMode = 0;
  UINT32 AbnormalResetOccurred = 0;
  BOOLEAN DLOADCookieSet = FALSE;
  BOOLEAN MemoryDumpEnabled = FALSE;
  EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE *OfflineCrashDumpConfigTable = NULL;

  EFI_STATUS Status = EFI_SUCCESS;
  UINT8 Value;
  UINTN Size = sizeof(Value);

  AsciiPrint("************Printing Offline Crash Dump Values**************\r\n");
  Status = EfiGetSystemConfigurationTable(&gEfiOfflineCrashDumpConfigTableGuid,
                                          (VOID**)&OfflineCrashDumpConfigTable);

  if (Status == EFI_NOT_FOUND)
  {
    AsciiPrint("  Offline Crash Dump Config Table does not exist in System Configuration Table\r\n");
    Status = EFI_SUCCESS;
  }
  else if (Status != EFI_SUCCESS)
  {
    AsciiPrint("ERROR: Could not successfully get item from System Configuration Table\r\n");
    return;
  }

  if (OfflineCrashDumpConfigTable == NULL)
  {
    AsciiPrint("  Offline Crash Dump Configuration Table: Uninitialized\r\n");
  }
  else
  {
    AsciiPrint("  Offline Crash Dump Configuration Table:\r\n");
    AsciiPrint("   Version: 0x%08x\r\n", OfflineCrashDumpConfigTable->Version);
    AsciiPrint("   AbnormalResetOccurred: 0x%08x\r\n", OfflineCrashDumpConfigTable->AbnormalResetOccurred);
    AsciiPrint("   OfflineMemoryDumpCapable: 0x%08x\r\n", OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable);
  }

  /* Print OfflineMemoryDumpUseCapability */
  Status = gRT->GetVariable(OFFLINE_MEMORY_DUMP_USE_CAPABILITY_VARNAME,
                            &gOSAVendorGuid,
                            NULL,
                            &Size,
                            &Value);
  if (Status != EFI_SUCCESS)
    AsciiPrint("  Could not retrieve UEFI variable: OfflineMemoryDumpUseCapability, Status = 0x%08x\r\n", Status);
  else
    AsciiPrint("  OfflineMemoryDumpUseCapability: 0x%02x\r\n", Value);

  /* Print EnableLegacySBLDump */
  Size = sizeof(Value);
  Status = gRT->GetVariable(ENABLE_LEGACY_SBL_DUMP_VARNAME,
                            &gQcomTokenSpaceGuid,
                            NULL,
                            &Size,
                            &Value);
  if (Status != EFI_SUCCESS)
    AsciiPrint("  Could not retrieve UEFI variable: EnableLegacySBLDump, Status = 0x%08x\r\n", Status);
  else
    AsciiPrint("  EnableLegacySBLDump: 0x%02x\r\n", Value);

  AsciiPrint("  Offline Crash Dump Values:\r\n");
  Status = GetMemoryCaptureMode(&MemoryCaptureMode);
  if (Status == EFI_SUCCESS)
  {
    AsciiPrint("    MemoryCaptureMode: 0x%08x\r\n", MemoryCaptureMode);
  }

  Status = GetAbnormalResetOccurred(&AbnormalResetOccurred);
  if (Status == EFI_SUCCESS)
  {
    AsciiPrint("    AbnormalResetOccurred: 0x%08x\r\n", AbnormalResetOccurred);
  }

  DLOADCookieSet = IsDLOADCookieSet();
  AsciiPrint("    Is DLOADCookieSet?: %a\r\n", DLOADCookieSet ? "Set" : "Cleared" );

  MemoryDumpEnabled = IsMemoryDumpEnabled();
  AsciiPrint("    Is Memory Dump Enabled?: %a\r\n", MemoryDumpEnabled ? "Yes" : "No" );
  AsciiPrint("************************************************************\r\n");
}

EFI_STATUS
EFIAPI
VerifyValues( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;
  PrintOfflineCrashDumpValuesApp();
  return Status;
}

EFI_STATUS
EFIAPI
ProcessValues( VOID )
{
  EFI_STATUS Status = EFI_SUCCESS;

  /* Process arguments and update values */
  if (OfflineCrashDumpConfigTable == NULL)
  {
    /* Initialize Offline Crash Dump Config Table */
    OfflineCrashDumpConfigTable = AllocateRuntimePool(sizeof(EFI_OFFLINE_CRASHDUMP_CONFIG_TABLE));
    if (OfflineCrashDumpConfigTable == NULL)
      return EFI_OUT_OF_RESOURCES;

    OfflineCrashDumpConfigTable->Version = OFFLINE_CRASHDUMP_CONFIG_TABLE_VERSION;
    OfflineCrashDumpConfigTable->AbnormalResetOccurred = 0;
    OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable = 0;

    AsciiPrint("Initializing Offline Crash Dump Configuration Table:\r\n");
    AsciiPrint (" Version: 0x%08x\r\n", OfflineCrashDumpConfigTable->Version);
    AsciiPrint (" AbnormalResetOccurred: 0x%08x\r\n", OfflineCrashDumpConfigTable->AbnormalResetOccurred);
    AsciiPrint (" OfflineMemoryDumpCapable: 0x%08x\r\n", OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable);

    Status = gBS->InstallConfigurationTable(&gEfiOfflineCrashDumpConfigTableGuid,
                                            (VOID*)OfflineCrashDumpConfigTable);
    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: Failed to install Offline Crash Dump Configuration Table:\r\n");
      return Status;
    }
  }

  if (UpdateRequired)
  {
    if ((AbnormalResetOccurred != 0xFFFFFFFF) && 
        (OfflineCrashDumpConfigTable->AbnormalResetOccurred != AbnormalResetOccurred))
    {
      OfflineCrashDumpConfigTable->AbnormalResetOccurred = AbnormalResetOccurred;
      AsciiPrint("Updated AbnormalResetOccurred in Offline Crash Dump Configuration Table:0x%08x\r\n", 
                 OfflineCrashDumpConfigTable->AbnormalResetOccurred );      
    }

    if ((OfflineMemoryDumpCapable != 0xFFFFFFFF) && 
        (OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable != OfflineMemoryDumpCapable))
    {
      OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable = OfflineMemoryDumpCapable;
      AsciiPrint("Updated OfflineMemoryDumpCapable in Offline Crash Dump Configuration Table:0x%08x\r\n", 
                 OfflineCrashDumpConfigTable->OfflineMemoryDumpCapable );      
    }
  }

  if (OfflineMemoryDumpUseCapability != 0xFF)
  {
    Status = gRT->SetVariable(OFFLINE_MEMORY_DUMP_USE_CAPABILITY_VARNAME,
                              &gOSAVendorGuid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS |
                              EFI_VARIABLE_RUNTIME_ACCESS |
                              EFI_VARIABLE_NON_VOLATILE,
                              sizeof(OfflineMemoryDumpUseCapability),
                              &OfflineMemoryDumpUseCapability);
    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: Failed to set OfflineMemoryDumpUseCapability UEFI variable\r\n");
      return Status;
    }

    AsciiPrint("Set OfflineMemoryDumpUseCapability: 0x%02x\r\n", OfflineMemoryDumpUseCapability);
  }

  if (EnableLegacySBLDump != 0xFF)
  {
    Status = gRT->SetVariable(ENABLE_LEGACY_SBL_DUMP_VARNAME,
                              &gQcomTokenSpaceGuid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS |
                              EFI_VARIABLE_NON_VOLATILE,
                              sizeof(EnableLegacySBLDump),
                              &EnableLegacySBLDump);
    if (Status != EFI_SUCCESS)
    {
      AsciiPrint("ERROR: Failed to set EnableLegacySBLDump UEFI variable\r\n");
      return Status;
    }

    AsciiPrint("Set EnableLegacySBLDump: 0x%02x\r\n", EnableLegacySBLDump);
  }

  return Status;
}

EFI_STATUS
EFIAPI
DeleteValue( CHAR8* ValueStr )
{
  EFI_STATUS Status = EFI_SUCCESS;

  if (AsciiStriCmp ((CHAR8*)ValueStr, "OfflineMemoryDumpUseCapability") == 0)
  {
    Status = gRT->SetVariable(OFFLINE_MEMORY_DUMP_USE_CAPABILITY_VARNAME,
                              &gOSAVendorGuid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS |
                              EFI_VARIABLE_RUNTIME_ACCESS |
                              EFI_VARIABLE_NON_VOLATILE,
                              0,
                              &OfflineMemoryDumpUseCapability);
  }
  else if (AsciiStriCmp ((CHAR8*)ValueStr, "EnableLegacySBLDump") == 0)
  {
    Status = gRT->SetVariable(ENABLE_LEGACY_SBL_DUMP_VARNAME,
                              &gQcomTokenSpaceGuid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS |
                              EFI_VARIABLE_NON_VOLATILE,
                              0,
                              &EnableLegacySBLDump);
  }
  else
  {
    Status = EFI_UNSUPPORTED;
  }

  if (Status != EFI_SUCCESS)
    AsciiPrint("ERROR: Failed to delete %a UEFI variable, Status = 0x%08x\r\n", ValueStr, Status);
  else
    AsciiPrint("Successfully deleted %a UEFI variable\r\n", ValueStr);

  return Status;
}

/* In this function process the cmdstr to compare what command we would
 * like to run and get argstr needed for that command */
EFI_STATUS
ProcessCommand (UINTN Argc, CHAR8** Argv)
{
  EFI_STATUS Status = EFI_SUCCESS;
  CHAR8 *CmdStr, *ArgStr;
  UINT32 i = 0;

  Status = EfiGetSystemConfigurationTable(&gEfiOfflineCrashDumpConfigTableGuid,
                                          (VOID**)&OfflineCrashDumpConfigTable);
  if (Status == EFI_NOT_FOUND)
  {
    AsciiPrint ("Offline Crash Dump Config Table does not exist in System Configuration Table\r\n");
    Status = EFI_SUCCESS;
  }
  else if (Status != EFI_SUCCESS)
  {
    AsciiPrint ("ERROR: Could not successfully get item from System Configuration Table\r\n");
    return Status;
  }

  while (i < Argc)
  {
    CmdStr = Argv[i];
    i++;

    /* Check arguments of 1 parameter */
    if (AsciiStriCmp ((CHAR8*)CmdStr, "-Verify") == 0)
    {
      Status = VerifyValues();
      if (Status != EFI_SUCCESS)
        return Status;
      else
        continue;
    }
    else if (AsciiStriCmp ((CHAR8*)CmdStr, "-SetDLOADCookie") == 0)
    {
      SetDLOADCookie();
      continue;
    }
    else if (AsciiStriCmp ((CHAR8*)CmdStr, "-ClearDLOADCookie") == 0)
    {
      ClearDLOADCookie();
      continue;
    }

    else if (AsciiStriCmp ((CHAR8*)CmdStr, "-Reboot") == 0)
    {
      gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, StrSize(FORCE_CRASH_STRING), FORCE_CRASH_STRING);
      continue;
    }

    if (i == Argc)
      break;

    ArgStr = Argv[i];
    i++;

#if 0
    AsciiPrint ("Processing Cmd: %a", CmdStr);
    if (ArgStr)
       AsciiPrint (", Arg: %a", ArgStr);
    AsciiPrint ("\r\n");
#endif

    /* Check arguments of 2 parameters */
    if (AsciiStriCmp ((CHAR8*)CmdStr, "-AbnormalResetOccurred") == 0)
    {
      AbnormalResetOccurred = AsciiStrHexToUintn(ArgStr);
      UpdateRequired = TRUE;
    }
    else if (AsciiStriCmp ((CHAR8*)CmdStr, "-AbnormalResetOccurredCookie") == 0)
    {
      UINT32 AbnormalResetOccurredCookie = AsciiStrHexToUintn(ArgStr);
      SetAbnormalResetOccurred(AbnormalResetOccurredCookie);
    }
    else if (AsciiStriCmp ((CHAR8*)CmdStr, "-MemoryCaptureModeCookie") == 0)
    {
      UINT32 MemoryCaptureModeCookie = AsciiStrHexToUintn(ArgStr);
      SetMemoryCaptureMode(MemoryCaptureModeCookie);
    }
    else if (AsciiStriCmp ((CHAR8*)CmdStr, "-OfflineMemoryDumpCapable") == 0)
    {
      OfflineMemoryDumpCapable = AsciiStrHexToUintn(ArgStr);
      UpdateRequired = TRUE;
    }
    else if (AsciiStriCmp ((CHAR8*)CmdStr, "-OfflineMemoryDumpUseCapability") == 0)
    {
      OfflineMemoryDumpUseCapability = (UINT8)(AsciiStrHexToUintn(ArgStr) & 0xFF);
    }
    else if (AsciiStriCmp ((CHAR8*)CmdStr, "-EnableLegacySBLDump") == 0)
    {
      EnableLegacySBLDump = (UINT8)(AsciiStrHexToUintn(ArgStr) & 0xFF);
    }
    else if (AsciiStriCmp ((CHAR8*)CmdStr, "-del") == 0)
    {
      Status = DeleteValue(ArgStr);
    }
    else
    {
      AsciiPrint ("Unable to find matching argument: %a\r\n", (CHAR8*)CmdStr);
      Status = EFI_INVALID_PARAMETER;
      break;
    }

    Status = ProcessValues();
  }

  /* Return success */
  return Status;
}

VOID
EFIAPI
PrintUsage ( VOID )
{
  AsciiPrint ("No command line args specified... \r\n");
  AsciiPrint ("Usage: CrashDumpFlagApp <Options>\r\n");
  AsciiPrint ("Options:\r\n");
  AsciiPrint (" -AbnormalResetOccurred <value>\r\n");
  AsciiPrint (" -OfflineMemoryDumpCapable <value>\r\n");
  AsciiPrint (" -OfflineMemoryDumpUseCapability <value>\r\n");
  AsciiPrint (" -AbnormalResetOccurredCookie <value>\r\n");
  AsciiPrint (" -MemoryCaptureModeCookie <value>\r\n");
  AsciiPrint (" -EnableLegacySBLDump <value>\r\n");
  AsciiPrint (" -SetDLOADCookie\r\n");
  AsciiPrint (" -ClearDLOADCookie\r\n");
  AsciiPrint (" -Verify\r\n");
  AsciiPrint (" -Reboot\r\n");
  AsciiPrint (" -del OfflineMemoryDumpUseCapability\r\n");
  AsciiPrint (" -del EnableLegacySBLDump\r\n");
  AsciiPrint ("Note: AbnormalResetOccurred and OfflineMemoryDumpCapable will be updated individually to System "
              "Configuration Table. If they do not exist or the Configuration Table entry does "
              "not exist, then it will initialize the entry\r\n");
  AsciiPrint ("Note: -del OfflineMemoryDumpUseCapability will delete the OfflineMemoryDumpUseCapability UEFI variable\r\n");
  AsciiPrint ("Note: -del EnableLegacySBLDump will delete the EnableLegacySBLDump UEFI variable\r\n");
  AsciiPrint ("Example (Enable SBL Offline Crash Dump): CrashDumpFlagApp \"-SetDLOADCookie -MemoryCaptureModeCookie 0x2\"\r\n");
  AsciiPrint ("Example (Enable Legacy UEFI Offline Crash Dump): CrashDumpFlagApp \"-SetDLOADCookie -MemoryCaptureModeCookie 0x1\"\r\n");
  AsciiPrint ("Example (Disable both Legacy UEFI and SBL Offline Crash Dump): CrashDumpFlagApp \"-ClearDLOADCookie -MemoryCaptureModeCookie 0x0\"\r\n");
  AsciiPrint ("Example (prints all values): CrashDumpFlagApp -Verify\r\n");
  AsciiPrint ("Example (simulates crash): CrashDumpFlagApp -Reboot\r\n");
}

/**
  Entry point for application

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS           Success.
  @retval EFI_INVALID_PARAMETER Invalid parameter supplied
  @retval EFI_OUT_OF_RESOURCES  Failed to allocate memory for the buffers
**/

EFI_STATUS
EFIAPI
CrashDumpFlagAppMain ( IN EFI_HANDLE         ImageHandle,
                       IN EFI_SYSTEM_TABLE   *SystemTable)
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINTN                       Argc;
  CHAR8                     **Argv, **TempArgv;

  Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);

  if ((Argc == 0) || (EFI_ERROR(Status)))
  {
    PrintUsage();
    Status = EFI_SUCCESS;
    goto Exit;
  }

  TempArgv = Argv;

  /* Check if the app was launched by shell and the Cmd str is actually the
   * Cmd app name itself, ie it would be "cmd" or "cmd.efi", then again process
   * to split, this is because Ebl doesn't pass the name of app, but shell does
   * other methods could yeild mixed results (like BDS launching apps via Menu, etc) */
  if ((AsciiStriCmp (Argv[0], "CrashDumpFlagApp") == 0) ||
      (AsciiStriCmp (Argv[0], "CrashDumpFlagApp.efi") == 0))
  {
    --Argc;
    if (Argc == 0)
    {
      FreePool (Argv);
      PrintUsage();
      Status = EFI_SUCCESS;
      goto Exit;
    }
    ++Argv;
  }

  /* Now both cmdStr and ArgStr have proper values, do string compare and
   * implement each command, Argument processing will be done as per the
   * command needs */
  Status = ProcessCommand (Argc, Argv);

  FreePool (TempArgv);

  if (Status != EFI_SUCCESS)
    AsciiPrint ("ERROR: Status = 0x%08x\r\n", Status);

Exit:
  AsciiPrint ("\r\nPress any key to exit...\r\n");
  ReadAnyKey (NULL, 0);

  return Status;
}

