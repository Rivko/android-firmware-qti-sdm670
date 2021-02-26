/** @file ResetReasonTest.c
   
  Reset reason tests

  Copyright (c) 2016, Qualcomm Technologies, Inc. All rights reserved.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/11/16   bh      Add test for platform specific EDL reset
 06/23/16   vk      Add test for clear
 01/28/16   vk      Initial Revision

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/QcomLib.h>

#include <Protocol/EFIResetReason.h>

extern EFI_GUID gResetFastbootModeGuid;
extern EFI_GUID gResetRecoveryModeGuid;
extern EFI_GUID gResetAlarmBootGuid;
extern EFI_GUID gResetDmVerifyLoggingGuid;
extern EFI_GUID gResetDmVerifyEnforcingGuid;
extern EFI_GUID gResetDmVerifyKeysclearGuid;
extern EFI_GUID gResetUnknownGuid;

STATIC EFI_STATUS
PrintCurrentResetReason (VOID)
{
  EFI_STATUS               Status;
  EFI_RESETREASON_PROTOCOL *pResetReasonProtocol = NULL;
  UINT32                   ResetReason = 0;
  CHAR16                   ResetReasonStr[128];
  UINT32                   ResetReasonStrLen = 128;

  SetMem (ResetReasonStr, ResetReasonStrLen*2, 0);

  Status = gBS->LocateProtocol(&gEfiResetReasonProtocolGuid, NULL, (VOID **) &pResetReasonProtocol);
  if ( EFI_ERROR (Status) || pResetReasonProtocol == NULL )
  {
    AsciiPrint ("ERROR: Locate ResetReasonProtocol  failed, Status = (0x%p)\r\n", Status);
    return Status;
  }
  
  Status = pResetReasonProtocol->GetResetReason ( pResetReasonProtocol, &ResetReason, ResetReasonStr, &ResetReasonStrLen);
  if (EFI_ERROR (Status))
    AsciiPrint ("ERROR: failed to get ResetReason, Status = (0x%p)\r\n", Status);
  else
    AsciiPrint ("Current Reset Reason Value : 0x%x \r\n", ResetReason);

  return Status;
}

STATIC EFI_STATUS
ClearResetReason (VOID)
{
  EFI_STATUS Status;
  EFI_RESETREASON_PROTOCOL *pResetReasonProtocol = NULL;
   
  Status = gBS->LocateProtocol(&gEfiResetReasonProtocolGuid, NULL, (VOID **) &pResetReasonProtocol);
  if ( EFI_ERROR (Status) || pResetReasonProtocol == NULL )
  {
    AsciiPrint ("ERROR: Locate ResetReasonProtocol  failed, Status = (0x%p)\r\n", Status);
    return Status;
  }
  
  PrintCurrentResetReason ();
 
  Status = pResetReasonProtocol->ClearResetReason ( pResetReasonProtocol );
    if (EFI_ERROR (Status))
      AsciiPrint ("ERROR: failed to clear ResetReason, %r\r\n", Status);
	
  PrintCurrentResetReason ();
  
  return Status;	
}

EFI_STATUS
EFIAPI
ResetReasonTestMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  UINTN      Argc;
  CHAR8      **Argv;
  
  #define MAX_RESET_REASON_STR 32
  CHAR16     ResetReasonStr[MAX_RESET_REASON_STR];
  UINT8      ResetReasonVal=0;
  STATIC     VOID* ResetData;
  UINTN      ResetDataSz = 0;
  UINT8      *pResetVal = NULL;
  UINT64     TempVal = 0;

  Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);
  if (Status != EFI_SUCCESS)
    return Status;

  PrintCurrentResetReason();

  SetMem (ResetReasonStr, sizeof (ResetReasonStr), 0);

  if (Argc == 2)
  {
    //AsciiPrint ("%a", Argv[0]);
    //AsciiPrint ("%a", Argv[1]);
    if (Argv[0] != NULL)
      AsciiStrToUnicodeStr (Argv[0], ResetReasonStr);

    if (Argv[1] != NULL)
    {
      TempVal = AsciiStrHexToUint64 (Argv[1]);
      ResetReasonVal = (TempVal & 0xFF);
    }
  }
  else 
  {
    AsciiPrint ("Usage: ResetReasonTest <ResetReasonString> <ResetReasonValue>\n");
    AsciiPrint ("eg: EBL> ResetReasonTest RESET_PARAM 0x55\n");
    return EFI_SUCCESS;
  }

  if (ResetReasonStr)
    Print (L"Reset Reason String: %s \n", ResetReasonStr);
	
  /* Processing for ResetReasonTest "clear 0" option */
  if (StrCmp (ResetReasonStr, L"clear") == 0)
  {
    AsciiPrint ("Testing CLEAR\n");
    return ClearResetReason();
  }	

  if (StrCmp (ResetReasonStr, STR_RESET_PARAM) == 0)
  {
    ResetDataSz = StrSize(ResetReasonStr) + sizeof (UINT8);
    Print (L"Reset Reason Value : 0x%x \n", ResetReasonVal);
  }

  if (StrCmp (ResetReasonStr, STR_RESET_PLAT_SPECIFIC_EDL) == 0)
  {
    Print (L"EDL Reset Requested\n");
    gRT->ResetSystem (EfiResetPlatformSpecific, EFI_NO_MAPPING, StrSize(STR_RESET_PLAT_SPECIFIC_EDL), STR_RESET_PLAT_SPECIFIC_EDL);
  }

  ResetData = AllocatePool (ResetDataSz);
  if (ResetData == NULL)
    return EFI_OUT_OF_RESOURCES;

  SetMem (ResetData, ResetDataSz, 0);

  StrCpy (ResetData, ResetReasonStr);

  pResetVal = (UINT8*)(ResetData + StrSize(STR_RESET_PARAM)); 
  
  *pResetVal = ResetReasonVal;
    
  /* Do a cold reset */
  //gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  gRT->ResetSystem (EfiResetCold, EFI_NO_MAPPING, ResetDataSz, ResetData);

  return EFI_SUCCESS;
}


