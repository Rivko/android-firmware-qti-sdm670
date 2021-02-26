/** @file
  Common header file.
 
    Copyright (c) 2012, Qualcomm Technologies Inc. All rights reserved.
Portions Copyright (c) 2011, Intel Corporation. All rights 
reserved.<BR> This program and the accompanying materials 
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
 12/12/14   bh      Fix invalid GUID format
 07/26/12   niting  Added age field to CapsuleGuidTable
 03/13/12   niting  Added firmware update support.
 02/06/12   jdt     Initial revision

=============================================================================*/

#ifndef _CAPSULE_COMMON_HEADER_
#define _CAPSULE_COMMON_HEADER_

#include <PiPei.h>

//
// This capsule PEIM puts its private data at the start of the
// coalesced capsule. Here's the structure definition.
//
#define EFI_CAPSULE_PEIM_PRIVATE_DATA_SIGNATURE SIGNATURE_32 ('C', 'a', 'p', 'D')
#define MAX_SUPPORT_CAPSULE_NUM               50

typedef struct {
  UINT32  Signature;
  UINT32   CapsuleSize;
} EFI_CAPSULE_PEIM_PRIVATE_DATA;

#define CAPSULE_TEST_SIGNATURE SIGNATURE_32('T', 'E', 'S', 'T')

#if defined (MDE_CPU_IA32) || defined (MDE_CPU_X64)
typedef struct {
  EFI_PHYSICAL_ADDRESS  EntryPoint;
  EFI_PHYSICAL_ADDRESS  StackBufferBase;
  UINT64                StackBufferLength;
  EFI_PHYSICAL_ADDRESS  JumpBuffer;
  EFI_PHYSICAL_ADDRESS  BlockListAddr;
  EFI_PHYSICAL_ADDRESS  MemoryBase64Ptr;
  EFI_PHYSICAL_ADDRESS  MemorySize64Ptr;
} SWITCH_32_TO_64_CONTEXT;

typedef struct {
  UINT16                ReturnCs;
  EFI_PHYSICAL_ADDRESS  ReturnEntryPoint;
  UINT64                ReturnStatus;
  IA32_DESCRIPTOR       Gdtr;
} SWITCH_64_TO_32_CONTEXT;
#endif

/**
 * This struct describes the blob of Coalesced 
 *  Capsule data.
 *  
 *  PrivateData -- A Brief header consisting of
 *   a signature and the size of the coalesced
 *   capsules.
 *  
 *   CapsuleData - This is an array of bytes
 *    representing the coalesed capsules.
 *  
 */
typedef struct{

  EFI_CAPSULE_PEIM_PRIVATE_DATA PrivateData;
  UINT8                         CapsuleData[1];

}EFI_CAPSULE_BUFFER_HEADER;

/**
 * A CAPSULE_OFFSET_TABLE 
 *  is found at the end of
 *  the capsulebuffer. It gives
 *  offsets from the beginning of
 *  the first capsule to each
 *  capsule in the Capsule buffer.
 *  
 *  CapsuleCount - The number of
 *  offsets in CapsuleArray.
 *  
 *  CapsuleArray - The array of
 *  offsets.  
 *  
 */
typedef struct
{
  UINT64 CapsuleCount;
  UINT64 OffsetArray[1];

}CAPSULE_OFFSET_TABLE;

#define CAPSULE_UPDATE_TABLE_SIGNATURE           SIGNATURE_32('C','A','P','S')
#define CAPSULE_UPDATE_TABLE_REVISION            0x00010001
#define CAPSULE_UPDATE_TABLE_BLOCK_SIZE          512
#define CAPSULE_UPDATE_TABLE_BLOCK_SHIFT         9

// based on header entries in CAPSULE_GUID_TABLE
#define CAPSULE_GUID_ENTRIES_MAX                 20

#define CAPSULE_INVALID_GUID \
  { 0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00} }

/**
 * Capsule Guid Entry - stores the GUID and location of a capsule within
 * the partition.
 */
typedef struct {
  /// 
  /// Guid for the capsule. If set to CAPSULE_INVALID_GUID, then this capsule
  /// has been consumed and is no longer available for firmware update.
  /// 
  EFI_GUID            Guid;

  /// 
  /// Block offset from start of the partition to where the capsule 
  /// header/data is stored. Each capsule is stored on a block alignment.
  /// 
  UINT32              BlockOffset;

  /// 
  /// Size in bytes of the capsule header/data
  /// 
  UINT32              ImageSize;

}CAPSULE_GUID_ENTRY;

/**
 * Firmware Update Table - stores information of this table along with the block
 * offset of each Record Table referenced by this table 
 */
typedef struct {
  ///
  /// Signature to validate the start of this structure in memory.
  ///
  UINT32              Signature;

  ///
  /// Version number of this header structure.
  /// Currently set to CAPSULE_UPDATE_TABLE_REVISION.
  ///
  UINT32              Revision;

  ///
  /// Number of valid Capsule Guid Entries
  ///
  UINT32              GuidCount;

  ///
  /// Block size of partition. 
  /// Currently set to CAPSULE_UPDATE_TABLE_BLOCK_SIZE.
  ///
  UINT32              BlockSize;

  ///
  /// Total partition size to store capsules.
  ///
  UINT32              BlockCount;

  ///
  /// Number of available blocks in the partition for capsules.
  ///
  UINT32              FreeBlocks;

  ///
  /// Keeps track of how many boot cycles the table has remained
  /// the same. Initialized to 0 every time the table is updated.
  ///
  UINT8               Age;

  ///
  /// Reserved to account for padding to align with CapsuleGuidEntries;
  /// Currently set to 0x000000.
  ///
  UINT8               Reserved[3];

  ///
  /// Crc32 of the entire table.
  ///
  UINT32              Crc32;

  ///
  /// Entries for each Guid in the Capsule Guid Table.
  /// GuidCount in Header determines the number of valid entries
  /// in this array.
  ///
  CAPSULE_GUID_ENTRY CapsuleGuidEntries[CAPSULE_GUID_ENTRIES_MAX];

} CAPSULE_GUID_TABLE;

extern EFI_GUID gUpdateCapsulePartitionGuid;
extern EFI_BLOCK_IO_PROTOCOL *CapsuleBlkIo;
extern EFI_STATUS EFIAPI ValidateCapsuleGuidTable ( CAPSULE_GUID_TABLE *CapsuleGuidTable );
extern BOOLEAN EFIAPI UpdateCapsulePartitionExists ( VOID );

/**
  The function to coalesce a fragmented capsule in memory.

  @param PeiServices        General purpose services available to every PEIM.
  @param BlockListBuffer    Point to the buffer of Capsule Descriptor Variables.
  @param MemoryBase         Pointer to the base of a block of memory that we can walk
                            all over while trying to coalesce our buffers.
                            On output, this variable will hold the base address of
                            a coalesced capsule.
  @param MemorySize         Size of the memory region pointed to by MemoryBase.
                            On output, this variable will contain the size of the
                            coalesced capsule.

  @retval EFI_NOT_FOUND     if we can't determine the boot mode
                            if the boot mode is not flash-update
                            if we could not find the capsule descriptors

  @retval EFI_BUFFER_TOO_SMALL
                            if we could not coalesce the capsule in the memory
                            region provided to us

  @retval EFI_SUCCESS       if there's no capsule, or if we processed the
                            capsule successfully.
**/
EFI_STATUS
EFIAPI
CapsuleDataCoalesce (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN IN EFI_PHYSICAL_ADDRESS         *BlockListBuffer,
  IN OUT VOID                        **MemoryBase,
  IN OUT UINTN                       *MemorySize
  );

#endif
