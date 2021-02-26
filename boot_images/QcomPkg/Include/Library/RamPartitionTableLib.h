/** @file RamPartitionTable.h
  
  RAM parition table interface

  Copyright (c) 2014,2016 by Qualcomm Technologies, Inc. 
  All Rights Reserved.
 
**/

/*=======================================================================
                        Edit History

when       who     what, where, why
--------   ----    ---------------------------------------------------
12/14/14   vk      Support non-contiguos DDR
10/30/14   vk      Initital Version
========================================================================*/

#include <Uefi.h>
#include <MemRegionInfo.h>

#define MAX_SUPPORTED_PARTITON_ENTRIES 4

/**
  Read RAM partition table and cache entries

**/

EFI_STATUS
InitRamPartitionTableLib (VOID);

/**
  Get Major version from RAM partition table

  @param  pRamPartitionTable     pointer to RAM partition table 
  @param  Version                Major version on output

  @retval EFI_SUCCESS            Successfully retrieves memory base and size
  @retval EFI_NOT_SUPPORTED      RAM parition table is not available
  @retval EFI_INVALID_PARAMETER  The RAM partition table is invalid
**/

EFI_STATUS
GetRamPartitionVersion (VOID *pRamPartitionTable, UINT32 *Version);

/**
  Using the RAM partition table from SMEM, get the DDR memory base and size

  @param  pMemoryBase            Pointer to the memory base
  @param  DDRMemorySize          Pointer to the memory size in bytes

  @retval EFI_SUCCESS            Successfully retrieves memory base and size
  @retval EFI_INVALID_PARAMETER  The RAM partition table is invalid
**/
EFI_STATUS
GetPartitionEntryByAddr (IN UINT64 Address, IN OUT MemRegionInfo *TableEntry);

/**
  Get a RAM partition table entries

  @param  EntryCount             On input, number of available Table buffer
                                 On output, number of entries in the table            
  @param  Table                  Caller allocated, pointer to return table entries

  @retval EFI_SUCCESS            Successfully retrieved partition table entries
  @retval EFI_NOT_SUPPORTED      RAM parition table is not available
  @retval EFI_INVALID_PARAMETER  TableEntry is NULL
  @retval EFI_BUFFER_TOO_SMALL   Not enough buffer to return all available entries
**/

EFI_STATUS
GetRamPartitions (IN OUT UINTN *EntryCount, IN OUT  MemRegionInfo *Table);

/**
  Get Highest Bank bit from RAM partition table

  @param  HighBankBit           Return Highest Bank bit value

  @retval EFI_SUCCESS           Retrieved Highest Bank Bit from RAM parition
  @retval EFI_NOT_SUPPORTED     RAM parition table is not available
  @retval EFI_INVALID_PARAMETER Invalid parameter
**/

EFI_STATUS
GetHighestBankBit (IN OUT UINT8 *HighBankBit);


/**
  Return total available DDR memroy
 
  @param  MemoryCapacity       Return memory capacity
 
  @retval EFI_SUCCESS          Returned memory capacity
  @retval EFI_NOT_SUPPORTED     RAM parition table is not available
  @retval EFI_INVALID_PARAMETER Invalid parameter
**/

EFI_STATUS
GetTotalPhysicalMemory(IN OUT UINTN *MemoryCapacity);

/**
  Return lowest physical start address of DDR memory
 
  @param  StartAddress       Return start address
 
  @retval EFI_SUCCESS          Returned start address
  @retval EFI_NOT_SUPPORTED     RAM parition table is not available
  @retval EFI_INVALID_PARAMETER Invalid parameter
**/
EFI_STATUS
GetLowestPhysicalStartAddress(UINTN *StartAddress);