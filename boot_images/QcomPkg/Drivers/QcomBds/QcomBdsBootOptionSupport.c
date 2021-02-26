/** @file
*
*  Copyright (c) 2014, Qualcomm Technologies, Inc. All Rights Reserved
*  Portions Copyright (c) 2011, ARM Limited. All rights reserved.
*
*  This program and the accompanying materials
*  are licensed and made available under the terms and conditions of the BSD License
*  which accompanies this distribution.  The full text of the license may be found at
*  http://opensource.org/licenses/bsd-license.php
*
*  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
*  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
*
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 02/03/15   bh      Fix KW errors
 10/24/14   bh      Initial branch from ArmPkg with edits

=============================================================================*/

#include "QcomBdsInternal.h"

#include <Library/NetLib.h>

#include <Protocol/BlockIo.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/PxeBaseCode.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/SimpleNetwork.h>

#include <Guid/FileSystemInfo.h>

#define IS_DEVICE_PATH_NODE(node,type,subtype) (((node)->Type == (type)) && ((node)->SubType == (subtype)))

EFI_STATUS
BdsLoadOptionFileSystemList (
  IN OUT LIST_ENTRY* BdsLoadOptionList
  );

EFI_STATUS
BdsLoadOptionFileSystemCreateDevicePath (
  IN CHAR16*                    FileName,
  OUT EFI_DEVICE_PATH_PROTOCOL  **DevicePathNodes,
  OUT ARM_BDS_LOADER_TYPE       *BootType,
  OUT UINT32                    *Attributes
  );

EFI_STATUS
BdsLoadOptionFileSystemUpdateDevicePath (
  IN EFI_DEVICE_PATH            *OldDevicePath,
  IN CHAR16*                    FileName,
  OUT EFI_DEVICE_PATH_PROTOCOL  **NewDevicePath,
  OUT ARM_BDS_LOADER_TYPE       *BootType,
  OUT UINT32                    *Attributes
  );

BOOLEAN
BdsLoadOptionFileSystemIsSupported (
  IN  EFI_DEVICE_PATH           *DevicePath
  );

EFI_STATUS
BdsLoadOptionMemMapList (
  IN OUT LIST_ENTRY* BdsLoadOptionList
  );

EFI_STATUS
BdsLoadOptionMemMapCreateDevicePath (
  IN CHAR16*                    FileName,
  OUT EFI_DEVICE_PATH_PROTOCOL  **DevicePathNodes,
  OUT ARM_BDS_LOADER_TYPE       *BootType,
  OUT UINT32                    *Attributes
  );

EFI_STATUS
BdsLoadOptionMemMapUpdateDevicePath (
  IN EFI_DEVICE_PATH            *OldDevicePath,
  IN CHAR16*                    FileName,
  OUT EFI_DEVICE_PATH_PROTOCOL  **NewDevicePath,
  OUT ARM_BDS_LOADER_TYPE       *BootType,
  OUT UINT32                    *Attributes
  );

BOOLEAN
BdsLoadOptionMemMapIsSupported (
  IN  EFI_DEVICE_PATH           *DevicePath
  );

EFI_STATUS
BdsLoadOptionPxeList (
  IN OUT LIST_ENTRY* BdsLoadOptionList
  );

EFI_STATUS
BdsLoadOptionPxeCreateDevicePath (
  IN CHAR16*                    FileName,
  OUT EFI_DEVICE_PATH_PROTOCOL  **DevicePathNodes,
  OUT ARM_BDS_LOADER_TYPE       *BootType,
  OUT UINT32                    *Attributes
  );

EFI_STATUS
BdsLoadOptionPxeUpdateDevicePath (
  IN EFI_DEVICE_PATH            *OldDevicePath,
  IN CHAR16*                    FileName,
  OUT EFI_DEVICE_PATH_PROTOCOL  **NewDevicePath,
  OUT ARM_BDS_LOADER_TYPE       *BootType,
  OUT UINT32                    *Attributes
  );

BOOLEAN
BdsLoadOptionPxeIsSupported (
  IN  EFI_DEVICE_PATH           *DevicePath
  );

EFI_STATUS
BdsLoadOptionTftpList (
  IN OUT LIST_ENTRY* BdsLoadOptionList
  );

EFI_STATUS
BdsLoadOptionTftpCreateDevicePath (
  IN CHAR16*                    FileName,
  OUT EFI_DEVICE_PATH_PROTOCOL  **DevicePathNodes,
  OUT ARM_BDS_LOADER_TYPE       *BootType,
  OUT UINT32                    *Attributes
  );

EFI_STATUS
BdsLoadOptionTftpUpdateDevicePath (
  IN EFI_DEVICE_PATH            *OldDevicePath,
  IN CHAR16*                    FileName,
  OUT EFI_DEVICE_PATH_PROTOCOL  **NewDevicePath,
  OUT ARM_BDS_LOADER_TYPE       *BootType,
  OUT UINT32                    *Attributes
  );

BOOLEAN
BdsLoadOptionTftpIsSupported (
  IN  EFI_DEVICE_PATH           *DevicePath
  );

BDS_LOAD_OPTION_SUPPORT BdsLoadOptionSupportList[] = {
  {
    BDS_DEVICE_FILESYSTEM,
    BdsLoadOptionFileSystemList,
    BdsLoadOptionFileSystemIsSupported
  }
};

EFI_STATUS
BootDeviceListSupportedInit (
  IN OUT LIST_ENTRY *SupportedDeviceList
  )
{
  UINTN   Index;

  // Initialize list of supported devices
  InitializeListHead (SupportedDeviceList);

  for (Index = 0; Index < BDS_DEVICE_MAX; Index++) {
    BdsLoadOptionSupportList[Index].ListDevices (SupportedDeviceList);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
BootDeviceListSupportedFree (
  IN LIST_ENTRY *SupportedDeviceList,
  IN BDS_SUPPORTED_DEVICE *Except
  )
{
  LIST_ENTRY  *Entry;
  BDS_SUPPORTED_DEVICE* SupportedDevice;

  Entry = GetFirstNode (SupportedDeviceList);
  while (Entry != SupportedDeviceList) {
    SupportedDevice = SUPPORTED_BOOT_DEVICE_FROM_LINK(Entry);
    Entry = RemoveEntryList (Entry);
    if (SupportedDevice != Except) {
      FreePool (SupportedDevice);
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
BootDeviceGetDeviceSupport (
  IN  EFI_DEVICE_PATH           *DevicePath,
  OUT BDS_LOAD_OPTION_SUPPORT   **DeviceSupport
  )
{
  UINTN Index;

  // Find which supported device is the most appropriate
  for (Index = 0; Index < BDS_DEVICE_MAX; Index++) {
    if (BdsLoadOptionSupportList[Index].IsSupported (DevicePath)) {
      *DeviceSupport = &BdsLoadOptionSupportList[Index];
      return EFI_SUCCESS;
    }
  }

  return EFI_UNSUPPORTED;
}

EFI_STATUS
BdsLoadOptionFileSystemList (
  IN OUT LIST_ENTRY* BdsLoadOptionList
  )
{
  EFI_STATUS                        Status;
  UINTN                             HandleCount;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             Index;
  BDS_SUPPORTED_DEVICE              *SupportedDevice;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL*  FileProtocol;
  EFI_FILE_HANDLE                   Fs;
  UINTN                             Size;
  EFI_FILE_SYSTEM_INFO*             FsInfo;
  EFI_DEVICE_PATH_PROTOCOL*         DevicePathProtocol;

  // List all the Simple File System Protocols
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &HandleCount, &HandleBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePathProtocol);
    if (!EFI_ERROR(Status)) {
      // Allocate BDS Supported Device structure
      SupportedDevice = (BDS_SUPPORTED_DEVICE*)AllocatePool (sizeof(BDS_SUPPORTED_DEVICE));
      if (SupportedDevice == NULL)
        return EFI_OUT_OF_RESOURCES;
      FileProtocol = NULL;
      Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiSimpleFileSystemProtocolGuid, (VOID **)&FileProtocol);
      ASSERT_EFI_ERROR(Status);

      FileProtocol->OpenVolume (FileProtocol, &Fs);

      // Generate a Description from the file system
      Size = 0;
      FsInfo = NULL;
      Status = Fs->GetInfo (Fs, &gEfiFileSystemInfoGuid, &Size, FsInfo);
      if (Status == EFI_BUFFER_TOO_SMALL) {
        FsInfo = AllocatePool (Size);
        if (FsInfo == NULL)
          return EFI_OUT_OF_RESOURCES;
        Status = Fs->GetInfo (Fs, &gEfiFileSystemInfoGuid, &Size, FsInfo);
      }
      if (FsInfo == NULL)
        continue;
      UnicodeSPrint (SupportedDevice->Description,BOOT_DEVICE_DESCRIPTION_MAX,L"%s (%d MB)",FsInfo->VolumeLabel,(UINT32)(FsInfo->VolumeSize / (1024 * 1024)));
      FreePool(FsInfo);
      Fs->Close (Fs);

      SupportedDevice->DevicePathProtocol = DevicePathProtocol;
      SupportedDevice->Support = &BdsLoadOptionSupportList[BDS_DEVICE_FILESYSTEM];

      InsertTailList (BdsLoadOptionList,&SupportedDevice->Link);
    }
  }

  return EFI_SUCCESS;
}

BOOLEAN
BdsLoadOptionFileSystemIsSupported (
  IN  EFI_DEVICE_PATH           *DevicePath
  )
{
  EFI_DEVICE_PATH*  DevicePathNode;

  DevicePathNode = GetLastDevicePathNode (DevicePath);

  return IS_DEVICE_PATH_NODE(DevicePathNode,MEDIA_DEVICE_PATH,MEDIA_FILEPATH_DP);
}

