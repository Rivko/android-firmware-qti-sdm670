/** @file
  Implements Utility functions
   
  Copyright (c) 2013-2014, Qualcomm Technologies, Inc. All rights reserved.
  Portions Copyright (c) 2007, Intel Corporation<BR>
  Portions copyright (c) 2008-2009, Apple Inc. All rights reserved.

  All rights reserved. This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 09/02/14   vk      KW fixes
 07/30/13   vk      Add BSD License
 07/11/13   yg      Initial version
=============================================================================*/

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/EfiFileLib.h>
#include <Library/QcomUtilsLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/LoadedImage.h>

/**
  Start an EFI image (PE32+ with EFI defined entry point).

  Argv[0] - device name and path
  Argv[1] - "" string to pass into image being started

  fs1:\Temp\Fv.Fv "arg to pass" ; load an FV from the disk and pass the
                                ; ascii string arg to pass to the image
  fv0:\FV                       ; load an FV from an FV (not common)
  LoadFile0:                    ; load an FV via a PXE boot

  @param  Argc   Number of command arguments in Argv
  @param  Argv   Array of strings that represent the parsed command line.
                 Argv[0] is the App to launch

  @return EFI_SUCCESS

**/
EFI_STATUS
LaunchApp (IN UINTN  Argc, IN CHAR8  **Argv)
{
  EFI_STATUS                  Status;
  EFI_OPEN_FILE               *File;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  EFI_HANDLE                  ImageHandle;
  UINTN                       ExitDataSize;
  CHAR16                      *ExitData;
  VOID                        *Buffer;
  UINTN                       BufferSize;
  EFI_LOADED_IMAGE_PROTOCOL   *ImageInfo;

  ImageHandle = NULL;

  if (Argc < 1)
    return EFI_INVALID_PARAMETER;

  File = EfiOpen (Argv[0], EFI_FILE_MODE_READ, 0);
  if (File == NULL)
    return EFI_INVALID_PARAMETER;

  DevicePath = File->DevicePath;
  if (DevicePath != NULL) {
    // check for device path form: blk, fv, fs, and loadfile
    Status = gBS->LoadImage (FALSE, gImageHandle, DevicePath, NULL, 0, &ImageHandle);
  } else {
    // Check for buffer form: A0x12345678:0x1234 syntax.
    // Means load using buffer starting at 0x12345678 of size 0x1234.

    Status = EfiReadAllocatePool (File, &Buffer, &BufferSize);
    if (EFI_ERROR (Status)) {
      EfiClose (File);
      return Status;
    }
    if (Buffer == NULL)
    {
      AsciiPrint ("ERROR: Memory Allocation failed !\n");
      return EFI_OUT_OF_RESOURCES;
    }
    Status = gBS->LoadImage (FALSE, gImageHandle, DevicePath, Buffer, BufferSize, &ImageHandle);

    if (Buffer != NULL)
      FreePool (Buffer);
  }

  EfiClose (File);

  if (!EFI_ERROR (Status)) {
    if (Argc >= 2) {
      // Argv[1] onwards are strings that we pass directly to the EFI application
      // We don't pass Argv[0] to the EFI Application, just the args
      Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&ImageInfo);
      ASSERT_EFI_ERROR (Status);

      /* Need WideChar string as CmdLineArgs */
      ImageInfo->LoadOptionsSize = 2 * (UINT32)AsciiStrSize (Argv[1]);
      ImageInfo->LoadOptions     = AllocatePool (ImageInfo->LoadOptionsSize);
      if (ImageInfo->LoadOptions == NULL)
      {
        AsciiPrint ("ERROR: Memory Allocation failed !\n");
        return EFI_OUT_OF_RESOURCES;
      }
      AsciiStrToUnicodeStr (Argv[1], ImageInfo->LoadOptions);
    }

    // Transfer control to the EFI image we loaded with LoadImage()
    Status = gBS->StartImage (ImageHandle, &ExitDataSize, &ExitData);
  }

  return Status;
}


EFI_STATUS
LoadFromFV (IN   CHAR8*    Name, 
            OUT  UINT8**   FileBuffer,
            OUT  UINTN*    FileSize)
{
  EFI_STATUS                  Status;
  EFI_OPEN_FILE               *File;

  if (Name == NULL || FileBuffer == NULL || FileSize == NULL)
    return EFI_INVALID_PARAMETER;

  File = EfiOpen (Name, EFI_FILE_MODE_READ, EFI_SECTION_RAW);
  if (File == NULL)
    return EFI_LOAD_ERROR;
  
  Status = EfiReadAllocatePool (File, (VOID**)FileBuffer, FileSize);

  EfiClose (File);

  return Status;
}

