/** @file

  Defines the interface of a RamDisk support Library.

  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

**/

#ifndef _RAMDISK_LIB_H_
#define _RAMDISK_LIB_H_

#include <Library/DebugLib.h>


#define RAMDISK_BLOCK_SIZE 512

/**
  Performs additional actions after a PE/COFF image has been loaded and relocated.

  If ImageContext is NULL, then ASSERT().

  @param  ImageContext  Pointer to the image context structure that describes the
                        PE/COFF image that has already been loaded and relocated.

**/
EFI_STATUS
EFIAPI
ValidateRamDisk (
  IN  UINT8 *ImageLocation,
  OUT UINTN *ImageSize
);

#endif
