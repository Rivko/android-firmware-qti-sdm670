/** @file CapsuleUtil.c

  Capsule Test Application.

  Copyright (c) 2015 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


when         who     what, where, why
----------   ---     -----------------------------------------------------------
2015-03-04   jb      Initial revision.

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/EfiFileLib.h>
#include <Library/QcomLib.h>
#include <Protocol/EfiShell.h>

EFI_SHELL_PROTOCOL *mShellProtocol = NULL;

EFI_STATUS
EFIAPI
GetCapsuleHeaderArray (
  IN CHAR8 **FileNames,
  IN UINTN Files,
  OUT EFI_CAPSULE_HEADER ***CapsuleHeaderArray,
  OUT EFI_PHYSICAL_ADDRESS *ScatterGatherList)
{
  UINTN Index;
  EFI_STATUS Status;
  EFI_CAPSULE_HEADER **CapsuleHeaders = NULL;
  EFI_CAPSULE_BLOCK_DESCRIPTOR *Descriptors = NULL;
  CHAR16 *FileNameUnicode;

  if (Files == 0) {
    return EFI_SUCCESS;
  }

  if (NULL == mShellProtocol) {
    return EFI_PROTOCOL_ERROR;
  }

  CapsuleHeaders = AllocatePool (Files * sizeof(EFI_CAPSULE_HEADER *));
  if (NULL == CapsuleHeaders) {
    return EFI_OUT_OF_RESOURCES;
  }

  Descriptors = AllocatePool ((Files + 1) * sizeof(EFI_CAPSULE_BLOCK_DESCRIPTOR));
  if (NULL == Descriptors) {
    FreePool (CapsuleHeaders);
    return EFI_OUT_OF_RESOURCES;
  }

  for (Index = 0; Index < Files; Index++) {
    SHELL_FILE_HANDLE   File;
    VOID               *Buffer = NULL;
    UINTN               BufferSize;

    FileNameUnicode = (CHAR16 *)AllocatePool (((AsciiStrLen (FileNames[Index]) + 1) * sizeof (CHAR16)));
    if (NULL == FileNameUnicode) {
      AsciiPrint ("Failed to allocate\n");
      return EFI_SUCCESS;
    }
    AsciiStrToUnicodeStr (FileNames[Index], FileNameUnicode);

    Status = mShellProtocol->OpenFileByName(FileNameUnicode, &File, EFI_FILE_MODE_READ);
    FreePool(FileNameUnicode);
    if (NULL == File || EFI_ERROR(Status)) {
      AsciiPrint("Failed to open file\n");
      return EFI_DEVICE_ERROR;
    }

    // read file into buffer
    BufferSize = 0;
    Status = mShellProtocol->GetFileSize(File, &BufferSize);
    if (Status == EFI_SUCCESS) {
      Buffer = AllocatePool (BufferSize);
      Status = mShellProtocol->ReadFile(File, &BufferSize, Buffer);
    }
    if (EFI_ERROR (Status))
    {
      if( Buffer != NULL )
      {
        FreePool(Buffer);
        Buffer = NULL;
      }
      Status = mShellProtocol->CloseFile(File);
      AsciiPrint("Read file fail. \r\n");
      return Status;
    }

    CapsuleHeaders[Index] = (EFI_CAPSULE_HEADER *)Buffer;
    Descriptors[Index].Length = BufferSize;
    Descriptors[Index].Union.DataBlock = (EFI_PHYSICAL_ADDRESS)Buffer;

    Status = mShellProtocol->CloseFile(File);
  }

  Descriptors[Index].Length = 0;
  Descriptors[Index].Union.DataBlock = 0;
  if (NULL != CapsuleHeaderArray) {
    *CapsuleHeaderArray = CapsuleHeaders;
  } else {
    FreePool (CapsuleHeaders);
  }

  if (NULL != ScatterGatherList) {
    *ScatterGatherList = (EFI_PHYSICAL_ADDRESS)Descriptors;
  } else {
    FreePool(Descriptors);
  }
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
CapsuleUpdate (
  IN CHAR8 **FileNames,
  IN UINTN Files
  )
{
  EFI_STATUS Status;
  EFI_CAPSULE_HEADER **CapsuleHeaderArray;
  EFI_PHYSICAL_ADDRESS ScatterGatherList;

  Status = GetCapsuleHeaderArray(FileNames, Files, &CapsuleHeaderArray, &ScatterGatherList);
  if (EFI_ERROR(Status)) {
    AsciiPrint ("Failed to load capsules\r\n");
    return Status;
  }

  Status = gRT->UpdateCapsule(CapsuleHeaderArray, Files, ScatterGatherList);
  if (EFI_ERROR(Status)) {
    AsciiPrint ("Failed to update capsule - %r\r\n", Status);
  } else {
    AsciiPrint ("Update successful\r\n");
  }
  return Status;
}

EFI_STATUS
EFIAPI
CapsuleQuery (
  IN CHAR8 **FileNames,
  IN UINTN Files
  )
{
  EFI_STATUS Status;
  EFI_CAPSULE_HEADER **CapsuleHeaderArray;
  UINT64 MaximumCapsuleSize;
  EFI_RESET_TYPE ResetType;

  Status = GetCapsuleHeaderArray(FileNames, Files, &CapsuleHeaderArray, NULL);
  if (EFI_ERROR(Status)) {
    AsciiPrint ("Failed to load capsules\r\n");
    return Status;
  }

  Status = gRT->QueryCapsuleCapabilities(CapsuleHeaderArray, Files, &MaximumCapsuleSize, &ResetType);
  if (EFI_ERROR(Status)) {
    AsciiPrint ("Failed to query capsule capabilities - %r\r\n", Status);
  } else {
    AsciiPrint ("Query successful\r\n");
    AsciiPrint ("Max Size - %d\r\n", MaximumCapsuleSize);
    switch(ResetType) {
      case EfiResetCold:
        AsciiPrint ("Reset Type - Cold\r\n");
        break;

    case EfiResetWarm:
        AsciiPrint ("Reset Type - Warm\r\n");
        break;

    case EfiResetShutdown:
        AsciiPrint ("Reset Type - Shutdown\r\n");
        break;

    case EfiResetPlatformSpecific:
        AsciiPrint ("Reset Type - Platform Specific\r\n");
        break;
    }
  }
  return Status;
}

EFI_STATUS
EFIAPI
CapsuleTest (
  IN CHAR8 **FileNames,
  IN UINTN Files
  )
{
  EFI_STATUS Status;
  Status = CapsuleQuery (FileNames, Files);
  if (!EFI_ERROR(Status)) {
    Status = CapsuleUpdate (FileNames, Files);
  }
  return Status;
}

EFI_STATUS
EFIAPI
CapsuleUtilEntryPoint (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN  Argc;
  CHAR8  **Argv = NULL;

  Status = gBS->LocateProtocol(&gEfiShellProtocolGuid, NULL, (VOID**)&mShellProtocol);
  if (EFI_ERROR (Status)) {
    return EFI_ABORTED;
  }

  Status = GetCmdLineArgs (ImageHandle, &Argc, &Argv);
  if (EFI_ERROR (Status)) {
    AsciiPrint ("Failed to parse commmand line\n");
    return EFI_SUCCESS;
  }

  if (Argc < 2) {
    AsciiPrint ("Usage: CapsuleUtil <command> <file> <file>\n");
    return EFI_SUCCESS;
  }

  if ( 0 == AsciiStrCmp(Argv[1], "update")) {
    return CapsuleUpdate(&Argv[2], Argc - 2);
  } else if ( 0 == AsciiStrCmp(Argv[1], "query")) {
    return CapsuleQuery(&Argv[2], Argc - 2);
  } else if ( 0 == AsciiStrCmp(Argv[1], "test")) {
    return CapsuleTest(&Argv[2], Argc - 2);
  }
  /// todo: Add enhancement test were we publish content and check system table installation

  return EFI_SUCCESS;;

} /* CapsuleUtilEntryPoint */


