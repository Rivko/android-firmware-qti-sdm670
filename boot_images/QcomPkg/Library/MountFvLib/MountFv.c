/** @file
  This file includes methods for loading and decompression of FV

  Copyright (c) 2017, Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  Portions copyright (c) 2004 - 2008, Intel Corporation. All rights reserved.
  
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php
  
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
 
**/

/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 12/01/17   kpa      Update to use ReadFromPartitionWithAttrib instead 
                     of ReadFromFvPartition
 09/08/17   kpa      Initial Revision
=============================================================================*/


/*=========================================================================
      Include Files
==========================================================================*/
#include <Uefi.h>

#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/ArmLib.h>
#include <Library/PcdLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ExtractGuidedSectionLib.h>
#include <Protocol/FirmwareVolumeBlock.h>
#include <Guid/FileSystemVolumeLabelInfo.h>
#include <Guid/LzmaDecompress.h>
#include "QcomLib.h"
#include <Library/GuidedFvLib.h>
#include <Library/QcomBdsLib.h>
#include <MemRegionInfo.h>
#include <Library/UefiCfgLib.h>
#include <Library/UefiLib.h>
#include <Library/MountFvUtils.h>



/**
  This function mounts a FV (firmware volume) passed in as an argument.  
  It locates the partition containing the FV and loads the data. if needed it is
  decompressed and then mounted.

  @param[in]  FvPartitionName            String name of the FV to be mounted
  @param[out] None
  
  @retval  EFI_SUCCESS          The Firmware volume is found and successfully mounted.
  @retval  EFI_NOT_FOUND        Firmware volume not found.
  @retval  EFI_OUT_OF_RESOURCES There were not enough resources to allocate the 
                                output data buffer or complete the operations.
  @retval  EFI_ACCESS_DENIED    The firmware volume containing the searched 
                                firmware file is configured to disallow reads.
**/
EFI_STATUS 
MountFvPartition (
  IN  CONST CHAR8 *FvPartitionName
)
{
  EFI_STATUS    Status = EFI_NOT_FOUND;
  VOID  *ReadBuffer = NULL;
  UINT32  Attrib = 0;
  UINTN  ImageSize = 0;
  CHAR16  *PartitionLabel = NULL;  
  
  if (FvPartitionName == NULL)
  {
    AsciiPrint(" Error: MountFvPartition requires Fv name.\r\n");
  } 
  
  if ( sizeof(FvPartitionName) >= MAX_PARTI_NAME_LEN )
  {
    AsciiPrint(" Error: Partition name too big.\r\n");
  }
  
   AsciiPrint(" --- ReadFromPartitionWithAttrib --- \r\n");
  
  PartitionLabel = AllocatePool(MAX_PARTI_NAME_LEN);
  AsciiStrToUnicodeStr(FvPartitionName, PartitionLabel);
  
  Attrib = BLK_IO_SEL_MATCH_PARTITION_LABEL;
  Attrib |= BLK_IO_SEL_STRING_CASE_INSENSITIVE;

  Status = ReadFromPartitionWithAttrib (PartitionLabel, &ReadBuffer, &ImageSize, Attrib);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN, "Failed to read from specified partition"));
    return Status;
  }

  Status = ProcessFvImageFile (ReadBuffer);
  if (EFI_ERROR(Status))
  {
    DEBUG((EFI_D_WARN, "Could not process FV in buffer\n"));
    return Status;
  }

  DEBUG((EFI_D_ERROR, "Successfully loaded FV\n"));
  return Status;
}
