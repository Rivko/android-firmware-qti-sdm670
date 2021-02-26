/** @file Cache.c

  Copyright (c) 2010-2015 Qualcomm Technologies, Inc. 
  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential.

  Portions Copyright (c) 2008 - 2009, Apple Inc. All rights reserved.

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
 09/09/15   vk      Add MMU lib
 08/13/15   vk      Remove DBI header
 04/17/15   vk      Remove collapsing regions
 09/30/14   vk      Remove carveout and reinit cache
 08/01/14   nk      Increased Max Memory entry
 05/22/14   vk      Remove 2MB carveout
 05/19/14   vk      GetConfigValue64 support
 04/16/14   vk      Remove 1 MB alignment
 03/11/14   vk      Re-init cache with only AddPeripherals regions
 12/05/13   vk      Fix top of DDR value 
 11/26/13   vk      Do not add hob for page table, memory allocation adds hob
 11/15/13   vk      Add return value for MMU configuration
 10/31/13   vk      Update for absolute address for top of memory 
 10/10/13   vk      Update for absolute address in cfg
 10/09/13   niting  Added prodmode check for offline crash dump memory reservation
 06/11/13   niting  Get config item to reserve memory for crash dump
 05/17/13   vk      Initialize cache early 
 03/03/13   yg      Increase Max Mem regions
 02/01/13   vk      Branch from 8974 for common Sec
=============================================================================*/

#include <ProcessorBind.h>
#include <PiPei.h>
#include <Library/ArmMmuLib.h>
#include <Library/PrePiLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiCfgLib.h>
#include <Library/QcomTargetLib.h>
#include <Library/RamPartitionTableLib.h>

#include "UefiPlatCfg.h"

#define MAX_MEMORY_ENTRIES     (128)

EFI_STATUS
EFIAPI
InitCache (IN MemRegionInfo  *pMemRegions, IN UINTN  RegionsCnt)
{
  EFI_STATUS Status;
  STATIC ARM_MEMORY_REGION_DESCRIPTOR  MemoryTable[MAX_MEMORY_ENTRIES];
  ARM_MEMORY_REGION_ATTRIBUTES         CacheAttributes;
  VOID                                 *TranslationTableBase;
  UINTN                                TranslationTableSize;
  UINTN                                MemRgnCnt;
  UINTN                                MemoryTableIndex = 0;

  ZeroMem (MemoryTable, sizeof (MemoryTable));

  // Sanity check
  if ((RegionsCnt >= MAX_MEMORY_ENTRIES) || (pMemRegions == NULL))
    return EFI_INVALID_PARAMETER;

  if (FeaturePcdGet(PcdCacheEnable) == TRUE) 
    CacheAttributes = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;
  else 
    CacheAttributes = ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;
  
  for (MemRgnCnt = 0; MemRgnCnt < RegionsCnt; MemRgnCnt++)
  {
    // Skip entries which explicitly ask to be added as HOB only
    if (pMemRegions[MemRgnCnt].BuildHobOption == HobOnlyNoCacheSetting)
      continue;


    // Fill the new entry
    MemoryTable[MemoryTableIndex].PhysicalBase = pMemRegions[MemRgnCnt].MemBase;
    MemoryTable[MemoryTableIndex].VirtualBase  = pMemRegions[MemRgnCnt].MemBase;
    MemoryTable[MemoryTableIndex].Length       = pMemRegions[MemRgnCnt].MemSize;

    if (pMemRegions[MemRgnCnt].CacheAttributes == ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK)
      MemoryTable[MemoryTableIndex].Attributes   = CacheAttributes;
    else
      MemoryTable[MemoryTableIndex].Attributes   = pMemRegions[MemRgnCnt].CacheAttributes;
  
    MemoryTableIndex++;

    if (MemoryTableIndex >= MAX_MEMORY_ENTRIES)
      return EFI_INVALID_PARAMETER;
  }

  // End of Table
  MemoryTable[MemoryTableIndex].PhysicalBase = 0;
  MemoryTable[MemoryTableIndex].VirtualBase  = 0;
  MemoryTable[MemoryTableIndex].Length       = 0;
  MemoryTable[MemoryTableIndex].Attributes   = (ARM_MEMORY_REGION_ATTRIBUTES)0;
  
  Status = ArmConfigureMmu (MemoryTable, &TranslationTableBase, &TranslationTableSize);

  ASSERT (Status == EFI_SUCCESS);
  
  if (Status != EFI_SUCCESS)
    return Status;

  return EFI_SUCCESS;
}

EFI_STATUS
EarlyCacheInit (UINTN MemBase, UINTN MemSize)
{
  ARM_MEMORY_REGION_DESCRIPTOR         EarlyInitMemoryTable[2];
  UINTN                                EarlyInitMemoryTableIndex = 0;
  
  EarlyInitMemoryTable[EarlyInitMemoryTableIndex].PhysicalBase = MemBase;
  EarlyInitMemoryTable[EarlyInitMemoryTableIndex].VirtualBase  = MemBase;
  EarlyInitMemoryTable[EarlyInitMemoryTableIndex].Length       = MemSize;
  EarlyInitMemoryTable[EarlyInitMemoryTableIndex].Attributes   = ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;
  EarlyInitMemoryTableIndex++;

  // End of Table
  EarlyInitMemoryTable[EarlyInitMemoryTableIndex].PhysicalBase = 0;
  EarlyInitMemoryTable[EarlyInitMemoryTableIndex].VirtualBase  = 0;
  EarlyInitMemoryTable[EarlyInitMemoryTableIndex].Length       = 0;
  EarlyInitMemoryTable[EarlyInitMemoryTableIndex].Attributes   = (ARM_MEMORY_REGION_ATTRIBUTES)0;
  
  return ArmConfigureMmu (EarlyInitMemoryTable, NULL, NULL);
}
