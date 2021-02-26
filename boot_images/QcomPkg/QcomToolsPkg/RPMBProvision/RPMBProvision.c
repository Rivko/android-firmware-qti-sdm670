/** @file RPMBProvision.c
   
  RPMB Provisioning Application. 
  
  Copyright (c) 2011-2013 Qualcomm Technologies, Inc. 
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
   
**/

/*=============================================================================
                              EDIT HISTORY


  when         who     what, where, why
  ----------   ---     -----------------------------------------------------------
  2013-05-09   niting  Make NoPrompt the default; add -Prompt
  2013-04-09   rsb     Return EFI_UNSUPPORTED if already provisioned
  2013-04-05   jt      Use QSEE command to do key provisioning 
  2013-03-25   yg      Add noprompt param
  2013-02-13   jt      Fixed klockwork warnings
  2013-01-17   jt      (Temporary) Do key provisioning entirely in UEFI 
  2012-09-14   yg      Adapt to new ReadAnyKey API
  2012-08-09   yg      Adapt to new GetAnyKey API
  2012-07-13   yg      Consolidate all errors and get confirmation for exit
  2012-03-16   jz      Get AppId from variable
  2012-03-06   jz      Renamed PcdTZSharedParamsBaseOffset to PcdTZScmCmdBufferBaseOffset
  2012-02-27   niting  Changed return code when RPMB is already provisioned
  2012-02-25   niting  Added change to initialize SCM physical buffer
  2012-02-15   niting  Fixed app to init SCM buffer 
  2011-12-19   yg      Initial revision.
 
=============================================================================*/

#include <Uefi.h>
#include <Library/RpmbLib.h>
#include <Library/UefiLib.h>
#include <Library/QcomLib.h>

#define QSEE_RPMB_ALREADY_PROVISIONED_ERROR 0xFFFFFF03

EFI_STATUS
AutoRPMBProvision (VOID)
{
  UINT32 Result = 0xFFFFFFFF;
  AsciiPrint ("Provision NoPrompt requested\r\n");

  if (ProvisionRpmbTestKey(&Result) != EFI_SUCCESS)
  {
    if (QSEE_RPMB_ALREADY_PROVISIONED_ERROR == Result)
    {
      AsciiPrint ("RPMB is already provisioned.\r\n");
      return EFI_UNSUPPORTED;
    }
    else
    {
      AsciiPrint ("RPMB provisioning Failed, Result = 0x%x.\r\n", Result);
      return EFI_DEVICE_ERROR;
    }
  }

  AsciiPrint ("RPMB provisioning Succeeded.\r\n");
  return EFI_SUCCESS;
}

EFI_STATUS
ManualRPMBProvision (VOID)
{
  UINTN            Code;
  EFI_INPUT_KEY    Key;
  UINT32           Result = 0xFFFFFFFF;

  AsciiPrint ("WARNING: This is a "
              "*PERMANENT LOCK* operation with key returned by TZ.\r\n"
              "If its a production phone, then this process should not "
              "be carried unless the debug fuses are blown already\r\n"
              "In doubt do NOT proceed. Are you sure to go ahead and "
              "Provision RPMB Now? (Vol UP = Yes, Any other key = No.\r\n");

  Code = 0;
  if (ReadAnyKey (&Key, 0) == EFI_SUCCESS)
  {
    Code = Key.ScanCode;
    if (Code == 0)
      Code = Key.UnicodeChar;
  }

  /* Print the code that we are basing our decision on */
 //  AsciiPrint ("%X\n", Code);

  if (Code != SCAN_UP)
  {
    AsciiPrint ("Not confirmed for RPMB provisioning, aborting...\r\n");
    return EFI_NO_RESPONSE;
  }

  AsciiPrint ("Got confirmation to provision\r\n");

  if (ProvisionRpmbTestKey(&Result) != EFI_SUCCESS)
  {
    if (QSEE_RPMB_ALREADY_PROVISIONED_ERROR == Result)
    {
      AsciiPrint ("RPMB is already provisioned.\r\n");
      return EFI_UNSUPPORTED;
    }
    else
    {
      AsciiPrint ("RPMB provisioning Failed, Result = 0x%x.\r\n", Result);
      return EFI_DEVICE_ERROR;
    }
  }

  AsciiPrint ("RPMB provisioning Succeeded.\r\n");
  return EFI_SUCCESS;
}

extern EFI_GUID gQcomTokenSpaceGuid;

/**
  Entry point for RPMB Provisioning. 

  Arguments to App: RPMBProvision -NoPrompt

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       Provisioned successfully.
  @retval EFI_DEVICE_ERROR  Failed to provision
**/
EFI_STATUS
EFIAPI
RPMBProvisionMain (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  UINTN            Argc;
  CHAR8            **Argv;
  UINTN            GetUserPrompt = FALSE;
  UINTN            i;

  Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);

  if (EFI_ERROR(Status))
    return Status;

  for (i = 0; i < Argc; ++i)
  {
    /* Check if the app was launched by shell and the Cmd str is actually the
     * Cmd app name itself, if yes then again process to split, this is because Ebl
     * doesn't pass the name of app, but shell does, other methods could yeild mixed
     * results (like BDS launching apps via Menu, etc) */
    if ((AsciiStriCmp (Argv[i], "RPMBProvision") == 0) ||
        (AsciiStriCmp (Argv[i], "RPMBProvision.efi") == 0))
    {
      continue;
    }

    if (AsciiStriCmp (Argv[i], "-NoPrompt") == 0)
    {
      GetUserPrompt = FALSE;
      continue;
    }
    else if (AsciiStriCmp (Argv[i], "-Prompt") == 0)
    {
      GetUserPrompt = TRUE;
      continue;
    }
  }

  if (GetUserPrompt)
    Status = ManualRPMBProvision ();
  else
    Status = AutoRPMBProvision ();

  if (GetUserPrompt)
  {
    AsciiPrint ("\r\nPress any key to exit...");
    ReadAnyKey (NULL, 0);
  }

  return Status;
}

