/** @file
*
*  Copyright (C) 2014, Qualcomm Technologies, Inc. All Rights Reserved
*  Portions Copyright (c) 2011-2013, ARM Limited. All rights reserved.
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
 10/24/14   bh      Initial branch from ArmPkg with edits

=============================================================================*/

#include "QcomBdsInternal.h"

BOOLEAN
HasFilePathEfiExtension (
  IN CHAR16* FilePath
  )
{
  return (StrCmp (FilePath + (StrSize (FilePath) / sizeof (CHAR16)) - 5, L".EFI") == 0) ||
         (StrCmp (FilePath + (StrSize (FilePath) / sizeof (CHAR16)) - 5, L".efi") == 0);
}

// Return the last non end-type Device Path Node from a Device Path
EFI_DEVICE_PATH*
GetLastDevicePathNode (
  IN EFI_DEVICE_PATH*  DevicePath
  )
{
  EFI_DEVICE_PATH*     PrevDevicePathNode;

  PrevDevicePathNode = DevicePath;
  while (!IsDevicePathEndType (DevicePath)) {
    PrevDevicePathNode = DevicePath;
    DevicePath = NextDevicePathNode (DevicePath);
  }

  return PrevDevicePathNode;
}

EFI_STATUS
BdsLaunchBootOption (
  IN CHAR16* BootOption
  )
{
  EFI_STATUS          Status;
  BDS_LOAD_OPTION     *BdsLoadOption;

  Status = BootOptionFromLoadOptionVariable (BootOption, &BdsLoadOption);
  if (!EFI_ERROR(Status) && BdsLoadOption!=NULL) {
    Status = BootOptionStart (BdsLoadOption);
    FreePool (BdsLoadOption);

    if (!EFI_ERROR(Status)) {
      Status = EFI_SUCCESS;
    } else {
      Status = EFI_NOT_STARTED;
    }
  } else {
    Status = EFI_NOT_FOUND;
  }
  return Status;
}

UINTN
GetUnalignedDevicePathSize (
  IN EFI_DEVICE_PATH* DevicePath
  )
{
  UINTN Size;
  EFI_DEVICE_PATH* AlignedDevicePath;

  if ((UINTN)DevicePath & 0x1) {
    AlignedDevicePath = DuplicateDevicePath (DevicePath);
    Size = GetDevicePathSize (AlignedDevicePath);
    FreePool (AlignedDevicePath);
  } else {
    Size = GetDevicePathSize (DevicePath);
  }
  return Size;
}

EFI_DEVICE_PATH*
GetAlignedDevicePath (
  IN EFI_DEVICE_PATH* DevicePath
  )
{
  if ((UINTN)DevicePath & 0x1) {
    return DuplicateDevicePath (DevicePath);
  } else {
    return DevicePath;
  }
}

