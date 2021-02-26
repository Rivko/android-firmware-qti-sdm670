/** @file UefiPlatCfg.c
  Pareser for UEFI platform configuration data.

  Copyright (c) 2012-2017 Qualcomm Technologies, Inc.  All Rights Reserved. 
  Qualcomm Technologies Proprietary and Confidential.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 
 --------   ---     -----------------------------------------------------------
 07/12/17   vk      Fix FD DEP
 09/09/16   vk      Add MMU lib
 06/28/16   bh      No longer assume config file is first in FV0
 02/18/16   bh      Update memory map parsing and non-FD bank parsing
 11/10/15   vk      Initialize globals explicitly for RAM load
 10/14/15   vk      Add NULL Check
 04/17/15   vk      Move to start initially from RAM partition entry with FD
 03/02/15   vk      Updates to parserlib
 10/18/14   jb      Allocate only memory map type
 10/30/14   vk      New cfg file
 04/10/14   niting  Fixed warnings
 11/26/13   vk      Update GetDDRInfo to use UINTN
 11/15/13   vk      Add ReInitCache return value check
 10/18/13   vk      Update MemRegionInfo struct members, Fix AllocMemNoFree
 10/16/13   vk      Change MemBaseOffset to MemBase, MemSize
 05/17/13   vk      Initialize cache early 
 05/14/13   vk      Add processing for write-through cache setting
 04/13/13   plc     Check fuse for number of CPUs, if indicated in CFG file
 03/14/13   vk      AllocPool to AllocPages - mark as EfiBootServiceData
 02/11/13   niting  Add support for HW chip version based config items
 02/06/13   vk      Add error check for debug message
 02/01/13   vk      Common Sec, text based cfg file params
 01/17/13   vk      Add ConfigTable
 01/17/13   vk      Add UefiCfgLib and lablels to memory table
 11/21/12   niting  Added uefiplat.cfg for memory map configuration
 11/19/12   niting  Initial revision.
 
=============================================================================*/

#include <PiPei.h>
#include <UefiPlatCfg.h>
#include <PrePiFvFileIo.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/PrePiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ArmMmuLib.h>

#include <Library/ParserLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiCfgLib.h>
#include <Include/UefiInfoBlk.h>
#include <Library/RamPartitionTableLib.h>

#define ROUND_TO_PAGE(x,y) (((x) + (y)) & (~(y)))

#define DEFAULT_MEMORY_REGIONS 64
#define UEFIPLATCFG_FILE "uefiplat.cfg"

/* Refer to TLMM_TLMM_HW_REVISION_NUMBER register definition */
#define HW_REVNUM_REG_PARTNUM_BMASK      0x0FFFF000
#define HW_REVNUM_REG_PARTNUM_SHIFT      12

/* Defined in LD script */
extern UINT64 EndOfText;

STATIC MemRegionInfo* mMemRegions = NULL;
STATIC UINTN mNumMemRegions = 0;
STATIC UINT64 mMemMapLow = 0, mMemMapHigh = 0;

STATIC ConfigPair* ConfigTable = NULL;
STATIC UINTN ConfigTableEntryCount = 0;
STATIC UINT8 *FreeBufferPtr, *EndPtr;
STATIC UINT32 *ChipIDReg = NULL;

EFI_STATUS
EFIAPI
InitCache (IN MemRegionInfo *pMemRegions, IN UINTN RegionsCnt);

UefiInfoBlkType* GetInfoBlock (VOID);

VOID
InitGlobals (VOID)
{
  mMemRegions = NULL;
  mNumMemRegions = 0;
  mMemMapLow = 0xFFFFFFFFFFFFFFFFULL;
  mMemMapHigh = 0;
  ConfigTable = NULL;
  ConfigTableEntryCount = 0;
  FreeBufferPtr = NULL;
  EndPtr = NULL;
  ChipIDReg = NULL;
}

/**
  Allocates and zeros a buffer.

  @param  AllocationSize        The number of bytes to allocate and zero.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID * EFIAPI
AllocateZeroPool (IN UINTN  AllocationSize)
{
  VOID  *Memory;

  Memory = AllocatePages (EFI_SIZE_TO_PAGES(AllocationSize));
  if (Memory != NULL) {
    ZeroMem (Memory, AllocationSize);
  }
  return Memory;
}

/* NOTE: We do not free this memory */
STATIC VOID*
AllocMemNoFree (UINTN Size)
{
  UINT8* AllocatedPtr;

  if (Size == 0)
    return NULL;

  if (Size >= EFI_PAGE_SIZE)
    return AllocateZeroPool (Size);

  if (FreeBufferPtr == NULL)
  {
    if ((FreeBufferPtr = AllocateZeroPool (EFI_PAGE_SIZE)) == NULL)
    {
      DEBUG((EFI_D_WARN, "MemoryAlloc failed\n"));
      return NULL;
    }
    EndPtr = FreeBufferPtr + EFI_PAGE_SIZE;
  }

  Size = (Size + 3) & (~3);
  if (FreeBufferPtr + Size > EndPtr)
  {
    if ((FreeBufferPtr = AllocateZeroPool (EFI_PAGE_SIZE)) == NULL)
    {
      DEBUG((EFI_D_WARN, "MemoryAlloc failed\n"));
      ASSERT (FreeBufferPtr != NULL);
      return NULL;
    }
    EndPtr = FreeBufferPtr + EFI_PAGE_SIZE;
  }

  AllocatedPtr = FreeBufferPtr;
  FreeBufferPtr += Size;
  return AllocatedPtr;
}

STATIC BOOLEAN
IsDigit (CHAR8* Ch)
{
  ASSERT (Ch != NULL);
  if (Ch == NULL)  /* For KW */
   return FALSE;

  return (BOOLEAN) ((*Ch >= '0') && (*Ch <= '9'));
}

STATIC BOOLEAN
IsHexDigit (CHAR8* Ch)
{
  ASSERT (Ch != NULL);
  if (Ch == NULL)  /* For KW */
   return FALSE;

  return (BOOLEAN) ((IsDigit(Ch)) ||
                    (*Ch >= 'A' && *Ch <= 'F') ||
                    (*Ch >= 'a' && *Ch <= 'f'));
}

STATIC BOOLEAN
IsHexDigitString (CHAR8* Ch)
{
   ASSERT (Ch != NULL); /* For KW */
  if (Ch == NULL)
   return FALSE;

  while (*Ch != '\0')
  {
    if (!IsHexDigit(Ch))
      return FALSE;
    Ch++;  
  }

  return TRUE;
}

STATIC BUILD_HOB_OPTION_TYPE
ParseBuildHobOption (CHAR8* BuildHobOptionStr)
{
  if (AsciiStrCmp (BuildHobOptionStr, "AddMem") == 0)
    return AddMem;

  else if (AsciiStrCmp (BuildHobOptionStr, "AddDev") == 0)
    return AddPeripheral;

  else if (AsciiStrCmp (BuildHobOptionStr, "NoHob") == 0)
    return NoBuildHob;

  else if (AsciiStrCmp (BuildHobOptionStr, "AllocOnly") == 0)
    return AllocOnly;

  DEBUG ((EFI_D_ERROR, "Invalid BuildHob option in config file !\n"));
  ASSERT (FALSE);

  return ErrorBuildHob; /* For KW */
}

STATIC UINTN
ParseResourceType (CHAR8* ResourceTypeStr)
{
  if (AsciiStrCmp (ResourceTypeStr, "SYS_MEM") == 0)
    return EFI_RESOURCE_SYSTEM_MEMORY;
  
  else if (AsciiStrCmp (ResourceTypeStr, "MMAP_IO") == 0)
    return EFI_RESOURCE_MEMORY_MAPPED_IO;
 
  else if (AsciiStrCmp (ResourceTypeStr, "IO") == 0)
    return EFI_RESOURCE_IO;

  else if (AsciiStrCmp (ResourceTypeStr, "FIRMWARE_DEV") == 0)
    return EFI_RESOURCE_FIRMWARE_DEVICE;

  else if (AsciiStrCmp (ResourceTypeStr, "MMAP_IO_PORT") == 0)
    return EFI_RESOURCE_MEMORY_MAPPED_IO_PORT;

  else if (AsciiStrCmp (ResourceTypeStr, "MEM_RES") == 0)
    return EFI_RESOURCE_MEMORY_RESERVED;

  else if (AsciiStrCmp (ResourceTypeStr, "IO_RES") == 0)
    return EFI_RESOURCE_IO_RESERVED;

  DEBUG ((EFI_D_ERROR, "Invalid ResourceType in config file !\n"));
  ASSERT (FALSE);

  return EFI_RESOURCE_MAX_MEMORY_TYPE; /* For KW */
}

STATIC UINTN
ParseResourceAttribute (CHAR8* ResourceAttribStr)
{
   /* NOTE: Since this returns bitwise OR, we check only 
    * common strings, if we don't understand the string,
    * return uint32, assuming it is valid. Decimal is not 
    * supported ! */

  if (AsciiStrCmp (ResourceAttribStr, "SYS_MEM_CAP") == 0)
  {
    /* SYSTEM_MEMORY_RESOURCE_ATTR_CAPABILITIES is a bitwise 
     * or of all system memory capabilities */
    return SYSTEM_MEMORY_RESOURCE_ATTR_SETTINGS_CAPABILITIES;
  }

  else if (AsciiStrCmp (ResourceAttribStr, "SYS_MEM_CAP_XN") == 0)
    return (SYSTEM_MEMORY_RESOURCE_ATTR_SETTINGS_CAPABILITIES | EFI_RESOURCE_ATTRIBUTE_EXECUTION_PROTECTED);

  else if (AsciiStrCmp (ResourceAttribStr, "UNCACHEABLE") == 0)
    return EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE;

  else if (AsciiStrCmp (ResourceAttribStr, "WRITE_COMBINEABLE") == 0)
    return EFI_RESOURCE_ATTRIBUTE_WRITE_COMBINEABLE;

  else if (AsciiStrCmp (ResourceAttribStr, "WRITE_THROUGH") == 0)
    return EFI_RESOURCE_ATTRIBUTE_WRITE_THROUGH_CACHEABLE;

  else if (AsciiStrCmp (ResourceAttribStr, "PRESENT") == 0)
    return EFI_RESOURCE_ATTRIBUTE_PRESENT;

  else if (AsciiStrCmp (ResourceAttribStr, "INITIALIZED") == 0)
    return EFI_RESOURCE_ATTRIBUTE_INITIALIZED;

  else if (AsciiStrCmp (ResourceAttribStr, "TESTED") == 0)
    return EFI_RESOURCE_ATTRIBUTE_TESTED;

  else if (AsciiStrCmp (ResourceAttribStr, "PROTECTED") == 0)
    return EFI_RESOURCE_ATTRIBUTE_READ_PROTECTED;

  else if (AsciiStrCmp (ResourceAttribStr, "WRITE_PROTECTED") == 0)
    return EFI_RESOURCE_ATTRIBUTE_WRITE_PROTECTED;

  else if (AsciiStrCmp (ResourceAttribStr, "EXECUTION_PROTECTED") == 0)
    return EFI_RESOURCE_ATTRIBUTE_EXECUTION_PROTECTED;


  else if ((AsciiStrLen (ResourceAttribStr) <= 10) && 
           ((AsciiStrnCmp (ResourceAttribStr, "0x", 2) == 0) ||
           (AsciiStrnCmp (ResourceAttribStr, "0X", 2) == 0))) 
  {
    /* 0x12345678 
     *|--| check first two chars "0x" or "0X"to determine if HEX 
     *and max length 10 for HEX uint32 */
    DEBUG ((EFI_D_WARN, "Warning: Unable to match string, using hex value for resource attribute !\n"));
    if (IsHexDigitString (ResourceAttribStr + 2))
      return AsciiStrHexToUintn(ResourceAttribStr);
  }

  /* Unable to recognize string, and is not hex, stop here */
  DEBUG ((EFI_D_ERROR, "Invalid Resource Attribute in config file !\n"));
  ASSERT (FALSE);

  return EFI_RESOURCE_ATTRIBUTE_EXECUTION_PROTECTED+1; /* For KW */
}

STATIC UINTN
ParseCacheAttribute (CHAR8* CacheAttribStr)
{
  if (AsciiStrCmp (CacheAttribStr, "NS_DEVICE") == 0)
    return ARM_MEMORY_REGION_ATTRIBUTE_NONSECURE_DEVICE;

  else if (AsciiStrCmp (CacheAttribStr, "UNCACHED_UNBUFFERED") == 0)
    return ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED;

  else if (AsciiStrCmp (CacheAttribStr, "UNCACHED_UNBUFFERED_XN") == 0)
    return ARM_MEMORY_REGION_ATTRIBUTE_UNCACHED_UNBUFFERED_XN;

  else if (AsciiStrCmp (CacheAttribStr, "WRITE_BACK_XN") == 0)
    return ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK_XN;

  else if (AsciiStrCmp (CacheAttribStr, "WRITE_THROUGH_XN") == 0)
    return ARM_MEMORY_REGION_ATTRIBUTE_WRITE_THROUGH_XN;

  else if (AsciiStrCmp (CacheAttribStr, "WRITE_BACK") == 0)
    return ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK;

  else if (AsciiStrCmp (CacheAttribStr, "WRITE_THROUGH") == 0)
    return ARM_MEMORY_REGION_ATTRIBUTE_WRITE_THROUGH;

  else if (AsciiStrCmp (CacheAttribStr, "NS_UNCACHED_UNBUFFERED") == 0)
    return ARM_MEMORY_REGION_ATTRIBUTE_NONSECURE_UNCACHED_UNBUFFERED;

  else if (AsciiStrCmp (CacheAttribStr, "NS_WRITE_BACK") == 0)
    return ARM_MEMORY_REGION_ATTRIBUTE_NONSECURE_WRITE_BACK;
  
  else if (AsciiStrCmp (CacheAttribStr, "NS_WRITE_THROUGH") == 0)
    return ARM_MEMORY_REGION_ATTRIBUTE_NONSECURE_WRITE_THROUGH;

  else if (AsciiStrCmp (CacheAttribStr, "DEVICE") == 0)
    return ARM_MEMORY_REGION_ATTRIBUTE_DEVICE;
  
  DEBUG ((EFI_D_ERROR, "Invalid Cache Attribute in config file !\n"));
  ASSERT (FALSE);

  return ARM_MEMORY_REGION_ATTRIBUTE_NONSECURE_DEVICE+1; /* This is invalid, we should no be here, for KW */
}


STATIC UINTN
ParseMemoryType (CHAR8* MemoryTypeStr)
{
  if (AsciiStrCmp (MemoryTypeStr, "Conv") == 0)
    return EfiConventionalMemory;

  else if (AsciiStrCmp (MemoryTypeStr, "Reserv") == 0)
    return EfiReservedMemoryType;

  else if (AsciiStrCmp (MemoryTypeStr, "MmIO") == 0)
    return EfiMemoryMappedIO;

  else if (AsciiStrCmp (MemoryTypeStr, "LdData") == 0)
    return EfiLoaderData;

  else if (AsciiStrCmp (MemoryTypeStr, "BsData") == 0)
    return EfiBootServicesData;

  else if (AsciiStrCmp (MemoryTypeStr, "BsCode") == 0)
    return EfiBootServicesCode;

  else if (AsciiStrCmp (MemoryTypeStr, "RtCode") == 0)
    return EfiRuntimeServicesCode;

  else if (AsciiStrCmp (MemoryTypeStr, "RtData") == 0)
    return EfiRuntimeServicesData;

  else if (AsciiStrCmp (MemoryTypeStr, "Unusable") == 0)
    return EfiUnusableMemory;

  else if (AsciiStrCmp (MemoryTypeStr, "ACPIReclaim") == 0)
    return EfiACPIReclaimMemory;

  else if (AsciiStrCmp (MemoryTypeStr, "ACPI_NVS") == 0)
    return EfiACPIMemoryNVS;

  else if (AsciiStrCmp (MemoryTypeStr, "MmIOPort") == 0)
    return EfiMemoryMappedIOPortSpace;

  else if (AsciiStrCmp (MemoryTypeStr, "PalCode") == 0)
    return EfiPalCode;

  else if (AsciiStrCmp (MemoryTypeStr, "MaxMem") == 0)
    return EfiMaxMemoryType;

  else if (AsciiStrCmp (MemoryTypeStr, "LdCode") == 0)
    return EfiLoaderCode;

  DEBUG ((EFI_D_ERROR, "Invalid Memory Type in config file !\n"));
  ASSERT (FALSE);

  return EfiMaxMemoryType+1; /* This is invalid, we should no be here, for KW */
}

#define MIN_SUPPORTED_VERSION 3

STATIC VOID EFIAPI
ProcessConfigTokens (UINT8* Section, UINT8* Key, UINT8* Value)
{
  CHAR8* mKey = (CHAR8*)Key;
  CHAR8* mValue = (CHAR8*)Value;
  UINTN MaxMemoryRegions = 0;

  if (AsciiStrCmp (mKey, "Version") == 0)
  {
    if (AsciiStrDecimalToUintn (mValue) < MIN_SUPPORTED_VERSION)
    {
      DEBUG ((EFI_D_ERROR, "Unsupported config file version!\n"));
      CpuDeadLoop();
      return; /* For KW */
    }
  }

  else if (AsciiStrCmp (mKey, "MaxMemoryRegions") == 0)
  {
    MaxMemoryRegions = AsciiStrDecimalToUintn (mValue);
    
    /* Allocate memory regions */
    if (mMemRegions == NULL)
    {
      UINTN AllocSz = 0;
      AllocSz = MaxMemoryRegions * sizeof(MemRegionInfo);
      mMemRegions = (MemRegionInfo*)AllocMemNoFree(AllocSz);

      if (mMemRegions == NULL)
      {
        DEBUG ((EFI_D_ERROR, "Unable to allocate memory for memory table!\n"));
        ASSERT (mMemRegions != NULL);
        CpuDeadLoop();
        return; /* For KW */
      }

      /* Initialize */
      SetMem (mMemRegions, AllocSz, 0);
      mNumMemRegions = 0;
    }
  }

  return;
}

STATIC VOID EFIAPI
ParseConfigParameters (UINT8* Section, UINT8* Key, UINT8* Value)
{
  STATIC UINTN MaxConfigParameters;

  CHAR8* mKey = (CHAR8*)Key;
  CHAR8* mValue = (CHAR8*)Value;

  /* Allocate only once */
  if (ConfigTable == NULL) 
  {
    if (AsciiStrCmp (mKey, "ConfigParameterCount") == 0)
    {
      UINTN AllocSz = 0;

      MaxConfigParameters = AsciiStrDecimalToUintn (mValue);
      DEBUG ((EFI_D_INFO, "MaxConfigParameters = %d\r\n", MaxConfigParameters));

      AllocSz = MaxConfigParameters * sizeof (ConfigPair); 
      ConfigTable = AllocMemNoFree (AllocSz);
      if (ConfigTable == NULL)
      {
        DEBUG ((EFI_D_ERROR, "Failed to allocate enough memory for config table \n"));
        ASSERT (ConfigTable != NULL);
        return; /* For KW*/
      }

      SetMem (ConfigTable, AllocSz, 0);
    }
  }
  else
  {
    /* Process only max number of entries provided in cfg file */
    if ((ConfigTableEntryCount > MaxConfigParameters) || (MaxConfigParameters == 0))
    {
      DEBUG ((EFI_D_ERROR, "Config file has more entries than specified max value\n"));
      ASSERT (MaxConfigParameters != 0);
      ASSERT (ConfigTableEntryCount < MaxConfigParameters);
      return; /* For KW */
    }

    ConfigTable[ConfigTableEntryCount].Key = AllocMemNoFree (AsciiStrLen (mKey)+1);
    ConfigTable[ConfigTableEntryCount].Value = AllocMemNoFree (AsciiStrLen (mValue)+1);

    AsciiStrCpy (ConfigTable[ConfigTableEntryCount].Key, mKey);
    AsciiStrCpy (ConfigTable[ConfigTableEntryCount].Value, mValue);

    ConfigTableEntryCount++;
  }

  return;
}

void ProcessMemoryMapTokens (UINT8* Section, UINTN SectionOffset, UINT8* TokenPtrArray[], UINTN MaxTokens)
{
  CHAR8 MemCfgName[MAX_MEM_LABEL_NAME];
  UINT64 MemCfgMemBase = 0;
  UINT64 MemCfgSize = 0;
  UINTN MemCfgBuildHobOption = 0;
  UINTN MemCfgResourceType = 0;
  UINTN MemCfgResourceAttribute = 0;
  UINTN MemCfgMemoryType = 0;
  UINTN MemCfgCacheAttributes = 0;

  if ((TokenPtrArray[0] == NULL) || (TokenPtrArray[1] == NULL) ||
      (TokenPtrArray[2] == NULL) || (TokenPtrArray[3] == NULL) ||
      (TokenPtrArray[4] == NULL) || (TokenPtrArray[5] == NULL) ||
      (TokenPtrArray[6] == NULL) || (TokenPtrArray[7] == NULL))
    return;

  AsciiStrCpy (MemCfgName, (CHAR8*)TokenPtrArray[2]);
  MemCfgMemBase           = AsciiStrHexToUint64((CHAR8*)TokenPtrArray[0]);
  MemCfgSize              = AsciiStrHexToUint64((CHAR8*)TokenPtrArray[1]);
  MemCfgBuildHobOption    = ParseBuildHobOption ((CHAR8*)TokenPtrArray[3]);
  MemCfgResourceType      = ParseResourceType ((CHAR8*)TokenPtrArray[4]);
  MemCfgResourceAttribute = ParseResourceAttribute ((CHAR8*)TokenPtrArray[5]);
  MemCfgMemoryType        = ParseMemoryType((CHAR8*)TokenPtrArray[6]);
  MemCfgCacheAttributes   = ParseCacheAttribute ((CHAR8*)TokenPtrArray[7]);

  AsciiStrCpy (mMemRegions[mNumMemRegions].Name, MemCfgName);
  mMemRegions[mNumMemRegions].MemBase           = MemCfgMemBase;
  mMemRegions[mNumMemRegions].MemSize           = MemCfgSize;
  mMemRegions[mNumMemRegions].BuildHobOption    = (BUILD_HOB_OPTION_TYPE)MemCfgBuildHobOption;
  mMemRegions[mNumMemRegions].ResourceType      = MemCfgResourceType;
  mMemRegions[mNumMemRegions].ResourceAttribute = MemCfgResourceAttribute;
  mMemRegions[mNumMemRegions].MemoryType        = (EFI_MEMORY_TYPE)MemCfgMemoryType;
  mMemRegions[mNumMemRegions].CacheAttributes   = (ARM_MEMORY_REGION_ATTRIBUTES)MemCfgCacheAttributes;
  mNumMemRegions++;
}

STATIC VOID GetCfgMemMapBounds(VOID)
{
  UINT64 Start, End;
  UINTN i = 0;

  for (i = 0; i < mNumMemRegions; i++)
  {
    Start = mMemRegions[i].MemBase;
    End = Start + mMemRegions[i].MemSize;
    if (Start < mMemMapLow)
    {
      mMemMapLow = Start;
    }

    if (End > mMemMapHigh)
    {
      mMemMapHigh = End;
    }
  }
}


EFI_STATUS
AddNonFdRegions (UINTN EntryCount, MemRegionInfo *EntryTable, UefiInfoBlkType *UefiInfoBlkPtr)
{
  UINT64 UefiFdBase = 0;
  UINT64 RegionEndAddr = 0;
  UINT64 DDRMemSize = 0;
  UINTN i = 0;
  
  UefiFdBase = FixedPcdGet32(PcdEmbeddedFdBaseAddress);

  for (i = 0; i < EntryCount; i++)
  {
    DDRMemSize += EntryTable[i].MemSize;

    RegionEndAddr = EntryTable[i].MemBase +  EntryTable[i].MemSize;

    /* Skip adding FD region */
    if ((UefiFdBase >= EntryTable[i].MemBase) && (UefiFdBase <= RegionEndAddr))
      continue;

    AsciiStrCpy (mMemRegions[mNumMemRegions].Name, EntryTable[i].Name);

    mMemRegions[mNumMemRegions].MemBase           = EntryTable[i].MemBase;
    mMemRegions[mNumMemRegions].MemSize           = EntryTable[i].MemSize;;
    mMemRegions[mNumMemRegions].BuildHobOption    = EntryTable[i].BuildHobOption;
    mMemRegions[mNumMemRegions].ResourceType      = EntryTable[i].ResourceType;
    mMemRegions[mNumMemRegions].ResourceAttribute = EntryTable[i].ResourceAttribute;
    mMemRegions[mNumMemRegions].MemoryType        = EntryTable[i].MemoryType;
    mMemRegions[mNumMemRegions].CacheAttributes   = EntryTable[i].CacheAttributes;
  
    mNumMemRegions++;
  }

  UefiInfoBlkPtr->DDRMemSize = DDRMemSize;

  return EFI_SUCCESS;
}

EFI_STATUS
AddFdRegionRemainder(UefiInfoBlkType *UefiInfoBlkPtr)
{
  MemRegionInfo FdRegion;
  UINT64 RegionEndAddr = 0;
  EFI_STATUS    Status;
  UINT64 UefiFdBase = FixedPcdGet64(PcdEmbeddedFdBaseAddress);

  Status = GetPartitionEntryByAddr (UefiFdBase, &FdRegion);
  if (EFI_ERROR(Status))
    return EFI_LOAD_ERROR;

  UefiInfoBlkPtr->FdMemBase = FdRegion.MemBase;
  RegionEndAddr = FdRegion.MemBase +  FdRegion.MemSize;
  
  //TODO: Make sure we set to BsData and switch to conventional later in DXE
  if (mMemMapLow > FdRegion.MemBase)
  {
    AsciiStrCpy (mMemRegions[mNumMemRegions].Name, FdRegion.Name);

    mMemRegions[mNumMemRegions].MemBase           = FdRegion.MemBase;
    mMemRegions[mNumMemRegions].MemSize           = (mMemMapLow - FdRegion.MemBase);
    mMemRegions[mNumMemRegions].BuildHobOption    = FdRegion.BuildHobOption;
    mMemRegions[mNumMemRegions].ResourceType      = FdRegion.ResourceType;
    mMemRegions[mNumMemRegions].ResourceAttribute = FdRegion.ResourceAttribute;
    mMemRegions[mNumMemRegions].MemoryType        = FdRegion.MemoryType;
    mMemRegions[mNumMemRegions].CacheAttributes   = FdRegion.CacheAttributes;
  
    mNumMemRegions++;
  }

  if (mMemMapHigh < RegionEndAddr)
  {
    AsciiStrCpy (mMemRegions[mNumMemRegions].Name, FdRegion.Name);

    mMemRegions[mNumMemRegions].MemBase           = mMemMapHigh;
    mMemRegions[mNumMemRegions].MemSize           = (RegionEndAddr - mMemMapHigh);
    mMemRegions[mNumMemRegions].BuildHobOption    = FdRegion.BuildHobOption;
    mMemRegions[mNumMemRegions].ResourceType      = FdRegion.ResourceType;
    mMemRegions[mNumMemRegions].ResourceAttribute = FdRegion.ResourceAttribute;
    mMemRegions[mNumMemRegions].MemoryType        = FdRegion.MemoryType;
    mMemRegions[mNumMemRegions].CacheAttributes   = FdRegion.CacheAttributes;
  
    mNumMemRegions++;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UpdateSystemMemoryRegions (VOID)
{
  EFI_STATUS Status;
  MemRegionInfo EntryList[MAX_SUPPORTED_PARTITON_ENTRIES];
  UINTN EntryCount = MAX_SUPPORTED_PARTITON_ENTRIES;
  UefiInfoBlkType* UefiInfoBlkPtr;

  UefiInfoBlkPtr = GetInfoBlock();


  Status = GetRamPartitions (&EntryCount, &EntryList[0]);
  if (Status != EFI_SUCCESS)
  {
    ASSERT (Status == EFI_SUCCESS);
    return Status;
  }
  
  Status = AddNonFdRegions (EntryCount, &EntryList[0], UefiInfoBlkPtr);
  if (Status != EFI_SUCCESS)
  {
    ASSERT (Status == EFI_SUCCESS);
    return Status;
  }

  Status = AddFdRegionRemainder(UefiInfoBlkPtr);
  if (Status != EFI_SUCCESS)
  {
    ASSERT (Status == EFI_SUCCESS);
    return Status;
  }

  return Status;
}

EFI_STATUS EFIAPI
InitCacheWithMemoryRegions (VOID)
{
  /* Initialize cache with new memory map */
  if (InitCache (mMemRegions, mNumMemRegions) != EFI_SUCCESS)
    return EFI_LOAD_ERROR;
  
  return EFI_SUCCESS;
}

EFI_STATUS EFIAPI
SetFdDep (VOID)
{
  EFI_STATUS    Status = EFI_UNSUPPORTED;
  MemRegionInfo MemRegion;
  UINT64        AddrEndOfText;
  UINT64        TextSegmentBase = 0, TextSegmentSize = 0;
  UINT64        NonTextSegmentBase = 0, NonTextSegmentSize = 0;

   /* Look for "UEFI FD" memory region in config file */
  Status = GetMemRegionInfoByName("UEFI FD", &MemRegion);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "ERROR: UEFI FD region not present in cfg file !\n"));
    return EFI_UNSUPPORTED;
  }

  AddrEndOfText = (UINT64)&EndOfText;
  TextSegmentBase = MemRegion.MemBase;
  TextSegmentSize = ROUND_TO_PAGE ( (AddrEndOfText - MemRegion.MemBase),  (SIZE_4KB-1));
  NonTextSegmentBase = ROUND_TO_PAGE ( AddrEndOfText, (SIZE_4KB-1));
  NonTextSegmentSize = MemRegion.MemSize - TextSegmentSize;

  ArmCleanInvalidateDataCache ();
  ArmInvalidateInstructionCache();

  ArmDisableMmu();
   
  /* Set RO for Sec text segment */
  Status = ArmSetMemoryRegionReadOnly (TextSegmentBase, TextSegmentSize);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "RO: 0x%016lx--0x%016lx [Failed]\n", TextSegmentBase, TextSegmentSize));
    return Status;
  }

  /* Set XN for Sec data segment */
  Status = ArmSetMemoryRegionNoExec (NonTextSegmentBase, NonTextSegmentSize);
  if (Status != EFI_SUCCESS)
  {
    DEBUG ((EFI_D_ERROR, "XN: 0x%016lx--0x%016lx [Failed]\n", NonTextSegmentBase, NonTextSegmentSize));
    return Status;
  }

  ArmEnableMmu();
 
  return Status;
}

STATIC VOID EFIAPI
ParseChipIDConfigParameters (UINT8* Section, UINTN SectionOffset, UINT8* TokenPtrArray[], UINTN MaxTokens)
{
  UINTN ChipIDConfigNum, ChipIDVal;
  UINTN Index = 0;     
  
  if ((TokenPtrArray[0] == NULL) || (TokenPtrArray[1] == NULL) || (TokenPtrArray[2] == NULL))
    return;
  
  if (ChipIDReg == NULL)
  {
    /* Find ChipID_RegAddr config item */
    for (Index = 0; Index < ConfigTableEntryCount; Index++)
        if (AsciiStrCmp(ConfigTable[Index].Key, "ChipID_RegAddr") == 0)
            break;

    if (Index < ConfigTableEntryCount)
    {
      ChipIDReg = (UINT32*)AsciiStrHexToUintn(ConfigTable[Index].Value);
      DEBUG ((EFI_D_INFO, "ChipID_RegAddr = 0x%08x\r\n", ChipIDReg));
    }
    else
    { 
      DEBUG ((EFI_D_WARN, "ChipID address uninitialized. Could not parse config item\r\n"));
      return;
    }
  }
  
  ChipIDConfigNum = AsciiStrHexToUintn((CHAR8*)TokenPtrArray[0]);
  ChipIDVal = *ChipIDReg;
  ChipIDVal = ((HW_REVNUM_REG_PARTNUM_BMASK & ChipIDVal) >> HW_REVNUM_REG_PARTNUM_SHIFT);
    
  /* Check if ChipID matches */
  if (ChipIDConfigNum == ChipIDVal)
  {
    /* Find existing config item */
    for (Index = 0; Index < ConfigTableEntryCount; Index++)
        if (AsciiStrCmp(ConfigTable[Index].Key, (CHAR8*)TokenPtrArray[1]) == 0)
            break;

    if (Index < ConfigTableEntryCount)
    {
        /* replace existing item */
        ConfigTable[Index].Value = AllocMemNoFree (AsciiStrLen ((CHAR8*)TokenPtrArray[2])+1);
        AsciiStrCpy (ConfigTable[Index].Value, (CHAR8*)TokenPtrArray[2]);           
    }
  }
}

EFI_STATUS
EFIAPI
LoadAndParsePlatformCfg ( VOID )
{
  EFI_STATUS Status;
  INTN       MemParserDesc = 0;
  UINT8      *CfgBuffer = NULL;
  UINTN      FileSize = 0;

  InitGlobals();

  Status = LoadFileFromFV0 (UEFIPLATCFG_FILE, &CfgBuffer, &FileSize);

  if (EFI_ERROR(Status) || (CfgBuffer == NULL) || (FileSize == 0) ) 
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }

  MemParserDesc = OpenParser (CfgBuffer, FileSize, NULL);
  if (MemParserDesc < 0)
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }

  /* Reset Global for RAM load */
  mMemRegions = NULL;
  mNumMemRegions = 0;
  if (EnumKeyValues (MemParserDesc, (UINT8*)"Config",  ProcessConfigTokens) < 0)
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }

  MemParserDesc = ReopenParser (MemParserDesc);
  if (MemParserDesc < 0)
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }

  if (EnumCommaDelimSectionItems (MemParserDesc, (UINT8*)"MemoryMap",  ProcessMemoryMapTokens) < 0)
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }

  GetCfgMemMapBounds();
  
  //UpdateSystemMemoryRegions();
  MemParserDesc = ReopenParser (MemParserDesc);
  if (MemParserDesc < 0)
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }


  if (EnumCommaDelimSectionItems (MemParserDesc, (UINT8*)"RegisterMap",  ProcessMemoryMapTokens) < 0)
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }

  MemParserDesc = ReopenParser (MemParserDesc);
  if (MemParserDesc < 0)
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }

  /* Reset for RAM load */
  ConfigTable = NULL;
  ConfigTableEntryCount = 0;
  if (EnumKeyValues (MemParserDesc, (UINT8*)"ConfigParameters", ParseConfigParameters) < 0)
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }
  
  MemParserDesc = ReopenParser (MemParserDesc);
  if (MemParserDesc < 0)
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }  
  
  if (EnumCommaDelimSectionItems (MemParserDesc, (UINT8*)"ChipIDConfig", ParseChipIDConfigParameters) < 0)
  {
    Status = EFI_LOAD_ERROR;
    goto ErrorExit;
  }

  CloseParser(MemParserDesc);

  return EFI_SUCCESS;

ErrorExit:
  DEBUG ((EFI_D_ERROR, "Failed LoadAndParsePlatformCfg\r\n"));
  CpuDeadLoop();
  return Status;
}


/* Gets the Memory Map that was parsed from the platform cfg file */
EFI_STATUS EFIAPI
GetMemRegionCfgInfo ( MemRegionInfo** MemoryRegions, UINTN* NumMemoryRegions )
{
  if ((MemoryRegions == NULL) || (NumMemoryRegions == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *MemoryRegions = mMemRegions;
  *NumMemoryRegions = mNumMemRegions;
  
  return EFI_SUCCESS;
}

/* Gets the configuration parameters parsed from config file */
EFI_STATUS EFIAPI
GetConfigTable (ConfigPair** ConfigTblPtr, UINTN* ConfigCnt)
{
  if ((ConfigTblPtr == NULL) || (ConfigCnt == NULL))
  {
    return EFI_INVALID_PARAMETER;
  }

  *ConfigTblPtr = ConfigTable;
  *ConfigCnt = ConfigTableEntryCount;
  
  return EFI_SUCCESS;
}
