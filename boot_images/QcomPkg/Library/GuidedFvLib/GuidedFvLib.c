/** @file
  This file has logic to launch app from Guided FV

  Copyright (c) 2016, 2017 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/08/17   bd      Support for authenticated FV loading
 10/31/16   bh      Connect FV protocol in load file function
 04/19/16   bh      Add cmd line arg support, read file from guided FV
 03/29/16   bh      Initial revision

=============================================================================*/

#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/GuidedFvLib.h>
#include <Library/QcomUtilsLib.h>
#include <Library/QcomLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/FileInfo.h>
#include <Library/DxeServicesTableLib.h>

#include "GuidedFvLibInternal.h"
extern EFI_GUID gEfiFileSystemVolumeLabelInfoIdGuid;

/* Function that locates and parses FV2 handles looking for 
   matching Guided FV. "FilePath" contains file device path */ 
STATIC EFI_STATUS GetAppFileDevPath (
    EFI_GUID                      *FvGuid,
    CHAR16                        *AppName,
    OUT EFI_DEVICE_PATH_PROTOCOL **FilePath)
{
  EFI_STATUS                Status;
  UINTN                     i;
  BOOLEAN                   FoundGuid = FALSE;
  EFI_DEVICE_PATH_PROTOCOL *FvDevicePath = NULL;
  EFI_GUID                 *TempGuid = NULL;
  EFI_HANDLE               *Fv2Handles = NULL;
  UINTN                     Fv2HandleCount;
  
  Status = ConnectByProtocol(&gEfiFirmwareVolume2ProtocolGuid);
  if (Status != EFI_SUCCESS && Status != EFI_NOT_FOUND)
  {
    DEBUG((EFI_D_ERROR, "Connect on FV2 handles failed!\n"));
    Status = EFI_NOT_FOUND;
    goto ErrorExit;
  }

  Status = gBS->LocateHandleBuffer (ByProtocol,
                           &gEfiFirmwareVolume2ProtocolGuid,
                           NULL,
                           &Fv2HandleCount,
                           &Fv2Handles);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Could not locate FV2 handles\n"));
    Status = EFI_NOT_FOUND;
    goto ErrorExit;
  }
  
  //Search through handles looking for matching guided FV
  for (i = 0; i < Fv2HandleCount; i++)
  {
    Status = gBS->HandleProtocol(
                  Fv2Handles[i],
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &FvDevicePath);
    if (Status == EFI_UNSUPPORTED)
      continue;

    FoundGuid = FALSE;
    //Loop through device path nodes to match FV Guid
    while (!IsDevicePathEndType (FvDevicePath))
    {
      if (DevicePathType (FvDevicePath) == MEDIA_DEVICE_PATH &&
          DevicePathSubType (FvDevicePath) == MEDIA_PIWG_FW_VOL_DP)
      {
        TempGuid = &((MEDIA_FW_VOL_DEVICE_PATH *) FvDevicePath)->FvName;
        if (CompareGuid(TempGuid, FvGuid))
        {
          FoundGuid = TRUE;
          break;
        }
      }
      FvDevicePath = NextDevicePathNode (FvDevicePath);
    }

    if (FoundGuid)
      break;
  }
  
  //Create file-path device path if Guided FV is matched
  if (FoundGuid)
  {
    *FilePath = FileDevicePath(Fv2Handles[i], AppName);
    if (*FilePath == NULL)
    {
      DEBUG((EFI_D_ERROR, "Could not create file-path device path\n"));
      Status = EFI_NOT_FOUND;
      goto ErrorExit;
    }
    Status = EFI_SUCCESS;
  }

ErrorExit:
  if (Fv2Handles)
    FreePool(Fv2Handles);
  return Status;
}

/* Launches an app from a guided FV. Depends on FV being
   accessible as a simple file system

   FvGuid     Guid of FV to launch app from
   AppName    Ascii name of app to launch
   Args       Ascii command line args to pass to app */
EFI_STATUS LaunchAppFromGuidedFv(EFI_GUID *FvGuid,
                                 CHAR8    *AppName,
                                 CHAR8    *Args OPTIONAL)
{
  EFI_STATUS                 Status;
  EFI_HANDLE                 ImageHandle;
  EFI_DEVICE_PATH_PROTOCOL  *FilePath = NULL;
  EFI_LOADED_IMAGE_PROTOCOL *ImageInfo;
  CHAR16                    *AppNameStr = NULL;
  CHAR16                    *ArgsStr = NULL;

  if (!FvGuid || !AppName)
  {
    return EFI_INVALID_PARAMETER;
  }

  AppNameStr = AllocatePool(2 * AsciiStrSize(AppName));
  if (AppNameStr == NULL)
    return EFI_OUT_OF_RESOURCES;

  AsciiStrToUnicodeStr(AppName, AppNameStr);

  Status = GetAppFileDevPath(
               FvGuid,
               AppNameStr,
               &FilePath);

  if (EFI_ERROR(Status) || !FilePath)
  {
    DEBUG((EFI_D_ERROR, "\nLaunchAppFromGuidedFv: Could not find required guided FV\n"));
    return EFI_NOT_FOUND;
  }

  Status = gBS->LoadImage (TRUE, gImageHandle, FilePath, NULL, 0, &ImageHandle);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "\nLaunchAppFromGuidedFv: Load image from guided FV failed, %r\n", Status));
    return Status;
  }

  if (Args)
  {
    ArgsStr = AllocatePool(2 * AsciiStrSize(Args));
    if (ArgsStr == NULL)
      return EFI_OUT_OF_RESOURCES;
    
    AsciiStrToUnicodeStr (Args, ArgsStr);

    Status = gBS->HandleProtocol(ImageHandle,
                                 &gEfiLoadedImageProtocolGuid,
                                 (VOID **) &ImageInfo);
    if (ImageInfo == NULL)
      DEBUG((EFI_D_WARN, "Could not pass arguments\n"));
    else
    {
      ImageInfo->LoadOptionsSize = (UINT32) StrSize (ArgsStr);
      ImageInfo->LoadOptions = AllocatePool(ImageInfo->LoadOptionsSize);
      if (ImageInfo->LoadOptions != NULL)
        StrCpy(ImageInfo->LoadOptions, ArgsStr);
      else
        DEBUG((EFI_D_WARN, "Could not pass arguments\n"));
    }
  }

  Status = gBS->StartImage(ImageHandle, NULL, NULL);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Start image failed\n"));
    return Status;
  }

  return EFI_SUCCESS;
}

#define GUID_STR_LEN StrLen(L"xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx")

INTN
CompareFvGuidToVolumeLabel (EFI_GUID * FvGuid, CHAR16* VolumeLabel)
{
  CHAR16    *pLabel = NULL;
  EFI_GUID   FsFvGuid;
  EFI_STATUS Status;
  UINTN      i;

  pLabel = VolumeLabel;

  if ((FvGuid == NULL) || (VolumeLabel == NULL))
    return -1;
  
  if (StrnCmp(L"Firmware Volume: ", pLabel, StrLen(L"Firmware Volume: ")))
    return -1;
  
  for (i = 0; i < StrLen(L"Firmware Volume: "); i++)
  {
    pLabel++;
  }

  if (StrLen(pLabel) < GUID_STR_LEN)
    return -1;

  Status = StrToGuid(pLabel, &FsFvGuid);
  if (EFI_ERROR(Status))
    return -1;

  return (CompareGuid(FvGuid, &FsFvGuid) == TRUE) ? 0 : -1;
}


/* Reads a file into buffer from a specified guided FV. 
   Buffer is allocated within function and FileSize contains
   size read in bytes. If FvGuid is NULL, it searches in all
   loaded FVs.

   FvGuid     Guid of FV to launch app from
   FileName   File to be read 
   FileBuffer Buffer to to be read into
   FileSize   Size in bytes of file read */
EFI_STATUS
LoadFileFromGuidedFv (EFI_GUID   *FvGuid,
                      CHAR8      *FileName,
                      OUT UINT8 **FileBuffer,
                      OUT UINTN  *FileSize)
{
  EFI_STATUS                       Status;
  EFI_HANDLE                      *Fv2Handles = NULL;
  UINTN                            Fv2HandleCount;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FsProtocol = NULL;
  EFI_FILE_PROTOCOL               *FsVolume = NULL;
  EFI_FILE_HANDLE                  FileHandle;
  EFI_FILE_INFO                   *FileInfo = NULL;
  EFI_FILE_SYSTEM_VOLUME_LABEL    *VolumeLabelInfo = NULL;
  CHAR16                          *FileNameStr = NULL;
  UINTN                            FileInfoSize = 0;
  UINTN                            VolumeLabelSize = 0;
  UINTN                            i;

  FileInfoSize = sizeof(*FileInfo);

  if (!FileBuffer || !FileSize || !FileName)
    return EFI_INVALID_PARAMETER;

  Status = ConnectByProtocol(&gEfiFirmwareVolume2ProtocolGuid);
  if (Status != EFI_SUCCESS && Status != EFI_NOT_FOUND)
  {
    DEBUG((EFI_D_ERROR, "Connect on FV2 handles failed!\n"));
    Status = EFI_NOT_FOUND;
    goto ErrorExit;
  }
  
  Status = gBS->LocateHandleBuffer (ByProtocol,
                           &gEfiFirmwareVolume2ProtocolGuid,
                           NULL,
                           &Fv2HandleCount,
                           &Fv2Handles);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Could not locate FV2 handles\n"));
    return Status;
  }

  for (i = 0; i < Fv2HandleCount; i++)
  {
    FsVolume = NULL;
    FileHandle = NULL;
    FileInfo = NULL;

    Status = gBS->HandleProtocol(
                  Fv2Handles[i],
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID **) &FsProtocol);

    if (EFI_ERROR(Status ))
      continue;
    
    Status = FsProtocol->OpenVolume(FsProtocol, &FsVolume);
    if (EFI_ERROR(Status) || !FsVolume)
      continue;

    Status = FsVolume->GetInfo(FsVolume,
                               &gEfiFileSystemVolumeLabelInfoIdGuid,
                               &VolumeLabelSize,
                               VolumeLabelInfo);

    if (Status == EFI_BUFFER_TOO_SMALL)
    {
      VolumeLabelInfo = AllocatePool(VolumeLabelSize);
      if (VolumeLabelInfo == NULL)
      {
        Status = EFI_OUT_OF_RESOURCES;
        FsVolume->Close(FsVolume);
        continue;
      }
      Status = FsVolume->GetInfo(FsVolume,
                                 &gEfiFileSystemVolumeLabelInfoIdGuid,
                                 &VolumeLabelSize,
                                 VolumeLabelInfo);
    }

    if (EFI_ERROR(Status))
    {
      FsVolume->Close(FsVolume);
      continue;
    }

    if (FvGuid)
    {
      if (CompareFvGuidToVolumeLabel(FvGuid, VolumeLabelInfo->VolumeLabel))
      {
        Status = EFI_NOT_FOUND;
        continue;
      }
    }
    
    FileNameStr = AllocatePool(2 * AsciiStrSize(FileName));
    if (FileNameStr == NULL)
    {
      return EFI_OUT_OF_RESOURCES;
    }

    AsciiStrToUnicodeStr(FileName, FileNameStr);

    Status = FsVolume->Open(FsVolume,
                            &FileHandle,
                            FileNameStr,
                            EFI_FILE_MODE_READ,
                            0);

    if (EFI_ERROR(Status) || !FileHandle)
    {
      if (FvGuid)
        goto ErrorExit;
      else
        continue;
    }

    Status = FileHandle->GetInfo(FileHandle,
                                 &gEfiFileInfoGuid,
                                 &FileInfoSize,
                                 FileInfo);

    if (Status == EFI_BUFFER_TOO_SMALL)
    {
      FileInfo = AllocatePool(FileInfoSize);
      if (!FileInfo)
      {
        Status = EFI_OUT_OF_RESOURCES;
        if (FvGuid)
          goto ErrorExit;
        else
          continue;
      }
      Status = FileHandle->GetInfo(FileHandle,
                                   &gEfiFileInfoGuid,
                                   &FileInfoSize,
                                   FileInfo);
    }

    if (EFI_ERROR(Status))
    {
      if (FvGuid)
        goto ErrorExit;
      else
        continue;
    }

    if (FileInfo  == NULL) 
    {
      Status = EFI_OUT_OF_RESOURCES;
      if (FvGuid)
        goto ErrorExit;
      else
        continue;
    }

    *FileSize = FileInfo->FileSize;
    *FileBuffer = AllocatePool(*FileSize);
    if (*FileBuffer == NULL)
    {
      Status = EFI_OUT_OF_RESOURCES;
      if (FvGuid)
        goto ErrorExit;
      else
        continue;
    }
    
    Status = FileHandle->Read(FileHandle,
                              FileSize,
                              *FileBuffer);
    if (EFI_ERROR(Status))
    {
      if (FvGuid)
        goto ErrorExit;
      else
        continue;
    }

    break;

  }
  
  return Status;

ErrorExit:
  if (FsVolume)
    FsVolume->Close(FsVolume);
  return Status;
}

/* Checks if the FV matching "FvGuid" has been mounted */
BOOLEAN
GuidedFvIsMounted(EFI_GUID *FvGuid)
{
  EFI_STATUS                Status = EFI_SUCCESS;
  EFI_DEVICE_PATH_PROTOCOL *FvDevicePath = NULL;
  EFI_HANDLE               *Fv2Handles = NULL;
  UINTN                     Fv2HandleCount;
  EFI_GUID                 *TempGuid;
  UINTN                     i;

  if (FvGuid == NULL)
    return FALSE;

  Status = gBS->LocateHandleBuffer (ByProtocol,
                                    &gEfiFirmwareVolume2ProtocolGuid,
                                    NULL,
                                    &Fv2HandleCount,
                                    &Fv2Handles);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_ERROR, "Could not locate FV2 handles\n"));
    return FALSE;
  }

  for (i = 0; i < Fv2HandleCount; i++)
  {
    Status = gBS->HandleProtocol(
                  Fv2Handles[i],
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &FvDevicePath);

    if (Status == EFI_UNSUPPORTED)
      continue;

    //Loop through device path nodes to match FV Guid
    while (!IsDevicePathEndType (FvDevicePath))
    {
      if (DevicePathType (FvDevicePath) == MEDIA_DEVICE_PATH &&
          DevicePathSubType (FvDevicePath) == MEDIA_PIWG_FW_VOL_DP)
      {
        TempGuid = &((MEDIA_FW_VOL_DEVICE_PATH *) FvDevicePath)->FvName;
        if (CompareGuid(TempGuid, FvGuid))
          return TRUE;
      }
      FvDevicePath = NextDevicePathNode (FvDevicePath);
    }
  }

  return FALSE;
}

/* Load and authenticate the FV */
EFI_STATUS LoadAndAuthenticateFV(EFI_GUID *FvGuid, CHAR16* PartitionLabel)
{
  EFI_STATUS      Status;
  VOID            *ReadBuffer = NULL;
  VOID            *FvBase = NULL;
  UINTN           ImageSize = 0;
  
  if (FvGuid == NULL)
  {
    DEBUG((EFI_D_ERROR, "FvGuid is not provided\n"));
    return EFI_INVALID_PARAMETER;
  }
  
  if (GuidedFvIsMounted(FvGuid))
  {
    DEBUG((EFI_D_WARN, "FV already mounted\n"));
    return EFI_SUCCESS;
  }
  
  
  Status = ReadFromPartition(PartitionLabel, &ReadBuffer, &ImageSize);
  if ((EFI_ERROR(Status)) || (ReadBuffer == NULL))
  {
    DEBUG((EFI_D_WARN, "Failed to mount FV from partition\n"));
    return Status;
  }
  
  Status = AuthenticateElf(ReadBuffer, ImageSize, &FvBase);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN, "Authentication of FV failed\n"));
    return Status;    
  }  
  
  Status = ProcessFvImageFile (FvBase);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN, "%a :Failed to mount FV from partition\n",__func__));
  }
  
  /* Free the temporary buffer
     ProcessFvImageFile has allocated another buffer for uncompressed data, which will be used now */
  if(ReadBuffer != NULL)
  {
    /* return type VOID */
    FreePages(ReadBuffer, ImageSize/EFI_PAGE_SIZE);  
  }
  
  return Status;
}  
