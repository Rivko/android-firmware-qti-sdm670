/** @file MountFvUtils.h
   
  Contains Qualcomm MountFvUtils library functions.

  Copyright (c) 2017, Qualcomm Technologies, Inc. All rights reserved. 
  Portions Copyright (c) 2006 - 2010, Intel Corporation. All rights reserved.
   
  This program and the accompanying materials
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
 12/01/17   kpa     Add ReadFromPartitionWithAttrib, ReadFromPartition
 09/08/17   kpa     Initial Revision

=============================================================================*/
#ifndef __MOUNTFVUTILS_H__
#define __MOUNTFVUTILS_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include "com_dtypes.h"

#define EXTRACT_SECTION_UI_SIZE(SectionUi) \
  ((UINTN) ((SectionUi->CommonHeader.Size[0] & 0xFF) | \
           ((SectionUi->CommonHeader.Size[1] << 8) & 0xFF00)| \
           ((SectionUi->CommonHeader.Size[2] << 16) & 0xFF0000)))

#define MAX_PARTI_NAME_LEN 16

/**
  Read a FV (firmware volume) from a partition passed in
  as an argument.  
  
  @param[in]  PartitionLabel    Label of the partition from where FV gets loaded
  @param[out] ReadBuffer        Buffer cointaining loaded FV
              ImageSize         Size of data read
              Attrib            Attributes of partitioned to be opened
  
  @retval  EFI_SUCCESS          Partition found and FV is loaded.
  @retval  EFI_NOT_FOUND        Firmware volume not found.
  @retval  EFI_OUT_OF_RESOURCES There were not enough resources to allocate the 
                                output data buffer or complete the operations.
**/   
EFI_STATUS ReadFromPartitionWithAttrib (
    CHAR16* PartitionLabel,
    VOID **ReadBuffer,
    UINTN *ImageSize,
    UINT32 Attrib
    );

/**
  Read the entire partition into memory.
  Update memory location in user given pointer

  @param[in]   PartitionLabel  Partition name as in partition.xml
  
  @param[out]  ReadBuffer      Pointer to hold address of data buffer. 
                               Memory for buffer is allocated inside this API.
                               And data from flash will be copied into that buffer
                       
  @param[out]  ImageSize       Number of bytes read.
  
  @retval  EFI_SUCCESS          The image is found and data and size is returned.
  @retval  EFI_NOT_FOUND        The image specified by PartitionLable cannot be found.
  @retval  EFI_OUT_OF_RESOURCES There were not enough resources to allocate the buffer
  */
EFI_STATUS
ReadFromPartition(
    CHAR16        *PartitionLabel,
    VOID          **ReadBuffer,
    UINTN         *ImageSize
    );

    
#endif /* __MOUNTFVUTILS_H__ */
