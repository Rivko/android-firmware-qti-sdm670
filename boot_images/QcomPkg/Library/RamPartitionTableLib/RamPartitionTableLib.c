/*
 * @file RamPartitionTableLib.c
 *
 * @brief RamPartitionTableLib functions
 *
 * Copyright (c) 2014-2017 by Qualcomm Technologies, Inc. 
 * All Rights Reserved.
 *
 */
/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    --------------------------------------------------- 
01/13/17   ltg     Add GetLowestPhysicalStartAddress function
11/16/15   vk      Zero init globals
10/14/15   vk      Add NULL pointer checks
09/18/15   bh      Add support for v1 ram partition table for STI
04/09/15   vk      Set address as 64 bit 
08/04/15   vk      Fix for BaseTargetPkg
05/21/15   vk      Move to v2 table
04/17/15   vk      Use PcdMaxMem to allocate from below 4 GB
12/14/14   vk      Handle non-contiguous memory region
11/06/14   vk      Fix pointer check
10/30/14   vk      Initital Version
========================================================================*/
#include <Uefi.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/RamPartitionTableLib.h>
#include <com_dtypes.h>
#include <Library/smem.h>
#include <ram_partition.h>
#include <MemRegionInfo.h>

#ifdef ENABLE_DEP_64
#define DEFAULT_MEM_REGION_ATTRIBUTE ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK_XN
#else
#define DEFAULT_MEM_REGION_ATTRIBUTE ARM_MEMORY_REGION_ATTRIBUTE_WRITE_BACK
#endif

STATIC MemRegionInfo mRamPartitionTable [MAX_SUPPORTED_PARTITON_ENTRIES]; 
STATIC UINTN         mRamPartitionTableEntryCount = 0;

EFI_STATUS
GetRamPartitionVersion (IN VOID *pRamPartitionTable, IN OUT UINT32 *Version)
{
  /* v0 and v1 have same header info */
  usable_ram_part_table_t pRamPartTable;
  pRamPartTable = (usable_ram_part_table_t) pRamPartitionTable;

  /* First, make sure the RAM partition table is valid */
  if( pRamPartTable->magic1 == RAM_PART_MAGIC1 &&
      pRamPartTable->magic2 == RAM_PART_MAGIC2 )
  {
     *Version = pRamPartTable->version;
     return EFI_SUCCESS;
  }
  else
  {
    return EFI_NOT_FOUND;
  }
}

#define UNSUPPORTED_BELOW_VER 1

STATIC EFI_STATUS
GetRamPartitionTable (IN OUT VOID **pRamPartitionTable, IN OUT UINT32 *Version)
{ 
  EFI_STATUS Status;
  UINT32 RamPartitionBuffSz = 0;

  /* Get the RAM partition table */
  *pRamPartitionTable = smem_get_addr(SMEM_USABLE_RAM_PARTITION_TABLE, (uint32*)&RamPartitionBuffSz);
  if (*pRamPartitionTable == NULL)
  {
    /*NOTE: We should be here only if SMEM is not initialized (virtio, SmemNullLib)*/
    DEBUG ((EFI_D_ERROR, "WARNING: Unable to read memory partition table from SMEM\n"));
    return EFI_NOT_READY;
  }

  Status  = GetRamPartitionVersion(*pRamPartitionTable, Version);
  if (Status != EFI_SUCCESS)  /* Invalid RAM Partition, return early */
   return Status; 

  if (*Version < UNSUPPORTED_BELOW_VER)
  {
    DEBUG ((EFI_D_WARN, "WARNING: Using deprecated RAM partition table !\n"));
    CpuDeadLoop();
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetTotalPhysicalMemory(UINTN *MemoryCapacity)
{
  EFI_STATUS Status;
  MemRegionInfo EntryList[MAX_SUPPORTED_PARTITON_ENTRIES];
  UINTN EntryCount = MAX_SUPPORTED_PARTITON_ENTRIES;
  UINT32 Index = 0;

  if (MemoryCapacity == NULL) 
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetRamPartitions (&EntryCount, &EntryList[0]);
  if (Status != EFI_SUCCESS)
  {
    return Status;
  }

  *MemoryCapacity = 0;
  for (Index = 0; Index < EntryCount; Index++) 
  {
    *MemoryCapacity += EntryList[Index].MemSize;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetLowestPhysicalStartAddress(UINTN *StartAddress)
{
  EFI_STATUS Status;
  MemRegionInfo EntryList[MAX_SUPPORTED_PARTITON_ENTRIES];
  UINTN EntryCount = MAX_SUPPORTED_PARTITON_ENTRIES;
  UINT32 Index = 0;

  if (StartAddress == NULL) 
  {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetRamPartitions (&EntryCount, &EntryList[0]);
  if (Status != EFI_SUCCESS)
  {
    return Status;
  }

  *StartAddress = 0xFFFFFFFF;
  for (Index = 0; Index < EntryCount; Index++) 
  {
    if (EntryList[Index].MemBase < *StartAddress){
      *StartAddress = EntryList[Index].MemBase;      
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetRamPartitions(UINTN *EntryCount, MemRegionInfo *EntryTable)
{
  UINTN i = 0;

  if (EntryCount == NULL)
    return EFI_INVALID_PARAMETER;

  if ((*EntryCount > 0) && (EntryTable == NULL))
    return EFI_INVALID_PARAMETER;

  if (mRamPartitionTableEntryCount > *EntryCount)
  {
    *EntryCount = mRamPartitionTableEntryCount;
    return EFI_BUFFER_TOO_SMALL;
  }

  for( i = 0; i < mRamPartitionTableEntryCount; i++ )
  {
    AsciiStrCpy (EntryTable[i].Name, mRamPartitionTable[i].Name);

    EntryTable[i].MemBase           = mRamPartitionTable[i].MemBase;
    EntryTable[i].MemSize           = mRamPartitionTable[i].MemSize;
    EntryTable[i].BuildHobOption    = mRamPartitionTable[i].BuildHobOption;
    EntryTable[i].ResourceType      = mRamPartitionTable[i].ResourceType;
    EntryTable[i].ResourceAttribute = mRamPartitionTable[i].ResourceAttribute;
    EntryTable[i].MemoryType        = mRamPartitionTable[i].MemoryType;
    EntryTable[i].CacheAttributes   = mRamPartitionTable[i].CacheAttributes;
  }

  *EntryCount = mRamPartitionTableEntryCount;

  return EFI_SUCCESS;
}

EFI_STATUS
GetPartitionEntryByAddr (IN UINT64 Address, IN OUT MemRegionInfo *FirstBank)
{
  UINTN  i = 0;
  UINT64 MemEndAddr = 0;

  if (FirstBank == NULL)
    return EFI_INVALID_PARAMETER;

  for (i = 0; i < mRamPartitionTableEntryCount; i++ )
  {
    MemEndAddr = mRamPartitionTable[i].MemBase + mRamPartitionTable[i].MemSize;
    if ((Address >= mRamPartitionTable[i].MemBase) && (Address <= MemEndAddr))
    {
      *FirstBank = mRamPartitionTable[i];
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
GetHighestBankBit (IN OUT UINT8 *HighBankBit)
{
  EFI_STATUS Status;
  VOID       *pRamPartitionTable = NULL;
  usable_ram_part_table_t pRamPartTable;
  UINT32     i, Version = 0;

  if (HighBankBit == NULL)
    return EFI_INVALID_PARAMETER;

  Status = GetRamPartitionTable (&pRamPartitionTable, &Version);
  if (Status != EFI_SUCCESS)
    return Status;

  if (pRamPartitionTable == NULL)
    return EFI_NOT_FOUND;

  pRamPartTable = (usable_ram_part_table_t) pRamPartitionTable;
  for (i = 0; i < pRamPartTable->num_partitions; i++)
  {
    if ((pRamPartTable->ram_part_entry[i].partition_type == RAM_PARTITION_SYS_MEMORY) &&
        (pRamPartTable->ram_part_entry[i].partition_category == RAM_PARTITION_SDRAM))
    {
      *HighBankBit = pRamPartTable->ram_part_entry[i].highest_bank_bit;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
HandlePartitionVer1(
  IN VOID *pRamPartitionTable
  )
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 i = 0;
  usable_ram_part_table_t_v1 pRamPartTablev1;
  pRamPartTablev1 = (usable_ram_part_table_t_v1) pRamPartitionTable;

  if (pRamPartitionTable == NULL)
    return EFI_INVALID_PARAMETER;

   /* Parse the DDR partition table, and fill in table for only DDR entries */
  for( i = 0; i < pRamPartTablev1->num_partitions; i++ )
  {
    if ((pRamPartTablev1->ram_part_entry_v1[i].partition_type == RAM_PARTITION_SYS_MEMORY) &&
        (pRamPartTablev1->ram_part_entry_v1[i].partition_category == RAM_PARTITION_SDRAM))
    {
      UINT64 StartAddr = pRamPartTablev1->ram_part_entry_v1[i].start_address;
      UINT64 Length = pRamPartTablev1->ram_part_entry_v1[i].length;
      UINT64 EndAddr = StartAddr + Length;
      
      /* Handle boundary condition for UINT32 rollover */
      if (EndAddr == 0x100000000ULL) {
        Length = Length - 0x200000;
        EndAddr = StartAddr + Length;
      }

      if (FixedPcdGet64 (PcdMaxMemory) != 0)
      {
        if (EndAddr > FixedPcdGet64 (PcdMaxMemory)) 
          continue;
      }

      AsciiStrCpy (mRamPartitionTable[mRamPartitionTableEntryCount].Name, "RAM Partition");

      mRamPartitionTable[mRamPartitionTableEntryCount].MemBase           = StartAddr;
      mRamPartitionTable[mRamPartitionTableEntryCount].MemSize           = Length;
      mRamPartitionTable[mRamPartitionTableEntryCount].BuildHobOption    = AddMem;
      mRamPartitionTable[mRamPartitionTableEntryCount].ResourceType      = EFI_RESOURCE_SYSTEM_MEMORY;
      mRamPartitionTable[i].ResourceAttribute = SYSTEM_MEMORY_RESOURCE_ATTR_SETTINGS_CAPABILITIES;
      mRamPartitionTable[mRamPartitionTableEntryCount].MemoryType        = EfiConventionalMemory;
      mRamPartitionTable[i].CacheAttributes   = DEFAULT_MEM_REGION_ATTRIBUTE;
      mRamPartitionTableEntryCount++;

      ASSERT (mRamPartitionTableEntryCount <= MAX_SUPPORTED_PARTITON_ENTRIES);

    }
  }

  return Status;
}

EFI_STATUS
InitRamPartitionTableLib (VOID)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINT32 i, Version = 0;
  VOID   *pRamPartitionTable = NULL;
  usable_ram_part_table_t pRamPartTable;
  smem_init();

  SetMem (mRamPartitionTable, sizeof (mRamPartitionTable), 0);
  mRamPartitionTableEntryCount = 0;

  Status = GetRamPartitionTable (&pRamPartitionTable, &Version);
  if (Status == EFI_NOT_READY)
  {
    /* NOTE: Get here only on presilicon, if RAM partition is not available, and using NULL Libs */
    mRamPartitionTable[0].MemBase           = FixedPcdGet64(PcdMemoryBase);
    mRamPartitionTable[0].MemSize           = FixedPcdGet64(PcdMemorySize);
    mRamPartitionTable[0].BuildHobOption    = AddMem;
    mRamPartitionTable[0].ResourceType      = EFI_RESOURCE_SYSTEM_MEMORY;
    mRamPartitionTable[0].ResourceAttribute = SYSTEM_MEMORY_RESOURCE_ATTR_SETTINGS_CAPABILITIES;
    mRamPartitionTable[0].MemoryType        = EfiConventionalMemory;
    mRamPartitionTable[0].CacheAttributes   = DEFAULT_MEM_REGION_ATTRIBUTE;
    mRamPartitionTableEntryCount++;
    return EFI_SUCCESS;
  }

  if (Status != EFI_SUCCESS)
    return Status;

  if (pRamPartitionTable == NULL)
    return EFI_INVALID_PARAMETER;

  if (Version == 1)
  {
    Status = HandlePartitionVer1(pRamPartitionTable);
    return Status;
  }

  pRamPartTable = (usable_ram_part_table_t) pRamPartitionTable;
  if (pRamPartTable== NULL)
    return EFI_INVALID_PARAMETER;

  /* Parse the DDR partition table, and fill in table for only DDR entries */
  for( i = 0; i < pRamPartTable->num_partitions; i++ )
  {
    if ((pRamPartTable->ram_part_entry[i].partition_type == RAM_PARTITION_SYS_MEMORY) &&
        (pRamPartTable->ram_part_entry[i].partition_category == RAM_PARTITION_SDRAM))
    {
      UINT64 StartAddr = pRamPartTable->ram_part_entry[i].start_address;
      UINT64 Length = pRamPartTable->ram_part_entry[i].available_length;
      UINT64 EndAddr = StartAddr + Length;
      
      if (FixedPcdGet64 (PcdMaxMemory) != 0)
      {
        if (EndAddr > FixedPcdGet64 (PcdMaxMemory)) 
          continue;
      }

      AsciiStrCpy (mRamPartitionTable[mRamPartitionTableEntryCount].Name, "RAM Partition");

      mRamPartitionTable[mRamPartitionTableEntryCount].MemBase           = StartAddr;
      mRamPartitionTable[mRamPartitionTableEntryCount].MemSize           = Length;
      mRamPartitionTable[mRamPartitionTableEntryCount].BuildHobOption    = AddMem;
      mRamPartitionTable[mRamPartitionTableEntryCount].ResourceType      = EFI_RESOURCE_SYSTEM_MEMORY;
      mRamPartitionTable[mRamPartitionTableEntryCount].ResourceAttribute = SYSTEM_MEMORY_RESOURCE_ATTR_SETTINGS_CAPABILITIES;
      mRamPartitionTable[mRamPartitionTableEntryCount].MemoryType        = EfiConventionalMemory; 
      mRamPartitionTable[mRamPartitionTableEntryCount].CacheAttributes   = DEFAULT_MEM_REGION_ATTRIBUTE;
      mRamPartitionTableEntryCount++;

      ASSERT (mRamPartitionTableEntryCount <= MAX_SUPPORTED_PARTITON_ENTRIES);

    }
  }

  return EFI_SUCCESS;
}
