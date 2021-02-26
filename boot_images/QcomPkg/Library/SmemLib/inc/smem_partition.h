/*=============================================================================

                           SMEM PARTITION

 Copyright (c)  2013 - 2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/inc/smem_partition.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/16   rv      SMEM Version 0x000C0000 changes.
04/20/15   rs      Increased max number of partitions 
10/09/13   bt      Add offset_mapped_un/cached to smem_partition_info_type.
06/25/13   pa      Added canary and padding size to allocation header.
                   Changed documentation and naming from 'upwards' to 
                   'uncached' and 'downwards' to 'cached' to avoid confusion.
                   Removed list references and smem_partition_item_info_type.
04/24/13   pa      Initial version.
===========================================================================*/

#ifndef SMEM_PARTITION_H
#define SMEM_PARTITION_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "smem.h"
#include "smem_internal.h"

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/
/** Partition Identifier: "$PRT" */
#define SMEM_PARTITION_HEADER_ID 0x54525024

#define SMEM_READ_SMEM_4(addr)       (*(uint32*)(addr))
#define SMEM_READ_SMEM_2(addr)       (*(uint16*)(addr))
#define SMEM_WRITE_SMEM_4(addr, val) ((*(uint32*)(addr)) = (uint32)(val))
#define SMEM_WRITE_SMEM_2(addr, val) ((*(uint16*)(addr)) = (uint16)(val))

/** Return the nearest multiple of y at or above x */
#define ROUND_UP(x,y)  ((((x) + (y) - 1) / (y)) * (y))

/** Number of bytes of padding based on cacheline sizes */
#define SMEM_PARTITION_ITEM_PADDING(size_cl) \
      (ROUND_UP(sizeof(smem_partition_allocation_header_type), (size_cl)) - \
                sizeof(smem_partition_allocation_header_type))

/** Maximum allowed number of partitions */
#define SMEM_MAX_PARTITIONS 20

/** Canary placed in the allocation header for each item */
#define SMEM_ALLOC_HDR_CANARY 0xa5a5

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/
/** This structure is the header for the SMEM partition, and sits at the base
 ** of every partition in SMEM. */
typedef struct 
{
  /** Identifier magic number */
  uint32 identifier;

  /** IDs for the 2 subsystems which have access to this partition.
   ** Order does not matter.
   ** Use uint16, rather than enum type, to ensure size. */
  uint16 host0;
  uint16 host1;

  /** Size of the partition, in bytes, not including the exclusion ranges */
  uint32 size;

  /** Offset of the byte following the last uncached allocation */
  uint32 offset_free_uncached;

  /** Offset of the byte following the last cached allocation */
  uint32 offset_free_cached;

  /** Reserved fields */
  uint32 reserved[3];

} smem_partition_header_type;

/** This structure is the header for each SMEM allocation in the 
 ** protected partition. */
typedef struct 
{
  /** Canary value.  Can be used to detect corruption.
   ** Set to 0xa5a5 */
  uint16 canary;

  /** SMEM item ID.
   ** Use uint16 here, rather than enum, to ensure size. */
  uint16 smem_type;

  /** Size of the allocated item in SMEM, in bytes. 
   ** Includes padding_data bytes. */
  uint32 size;

  /** Size of data padding, in bytes.  
   ** If the requested size was padded (rounded up), this is the
   ** amount it was rounded. */
  uint16 padding_data;

  /** Size of header padding, in bytes. */
  uint16 padding_header;

  /** Reserved fields */
  uint32 reserved[1];

} smem_partition_allocation_header_type;

/** This structure holds debug info for the SMEM partition code */
typedef struct
{
  /** Number of allocations from the uncached side that are known to this proc.
   ** This may be stale if we have not done a scan of SMEM since the remote
   ** endpoint last allocated an item. */
  uint32 num_alloc_uncached;

  /** Number of allocations from the cached side that are known to this proc.
   ** This may be stale if we have not done a scan of SMEM since the remote
   ** endpoint last allocated an item. */
  uint32 num_alloc_cached;

  /** Number of times this endpoint has detected one or more new SMEM items
   ** allocated by the remote endpoint and performed a scan */
  uint32 num_scan_uncached;

  /** Number of times this endpoint has detected one or more new SMEM items
   ** allocated by the remote endpoint and performed a scan */
  uint32 num_scan_cached;

} smem_partition_debug_info_type;

/** This structure describes the state of the SMEM partition.  
 ** This resides in local memory. */
typedef struct 
{
  /** Pointer to the header for the partition which sits at the base */
  smem_partition_header_type * header;

  /** Size of the partition, in bytes, not including the exclusion ranges */
  uint32 size;

  /** Lowest common multiple of the cacheline sizes for this edge */
  uint32 size_cacheline;

  /** Offset of the byte following the last allocation from the uncached side.
   ** Compared against the value in SMEM to determine if the remote endpoint
   ** has allocated from the heap. */
  uint32 offset_free_uncached;

  /** Offset of the byte following the last allocation from the cached side.
   ** Compared against the value in SMEM to determine if the remote endpoint
   ** has allocated from the heap. */
  uint32 offset_free_cached;

  /** Offset at the end of all pages that have been mapped as uncached in this
   ** partition.  This is the base address of the next-highest-address page. */
  uint32 offset_mapped_uncached;

  /** Offset at the tip of all pages that have been mapped as cached in this
   ** partition.  This is the base address of the lowest-address cached page 
   ** that has already been mapped. */
  uint32 offset_mapped_cached;

  /** Debug info */
  smem_partition_debug_info_type debug_info;

} smem_partition_info_type;

/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/
extern const smem_funcs_type smem_part_funcs;

/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
FUNCTION      smem_partition_init
=============================================================================*/
/**
  Initializes the SMEM partitioning structures.

  This function checks for the existence of the SMEM table of contents and then
  reads it to determine partition locations and size.  Data structures are
  initialized to enable SMEM allocation from protected partitions.

  @return
*/
/*===========================================================================*/
void smem_part_init(void);

/*=============================================================================
FUNCTION      smem_part_alloc_ex
=============================================================================*/
/**
  Requests a pointer to a buffer in shared memory.  Upon success, params.buffer
  points to the allocation in shared memory.

  @param[inout] params  See definition of smem_alloc_params_type for details.

  @return
  SMEM_STATUS_SUCCESS          - Allocation was successful,
                                 or already exists with specified size.
                                 Pointer to SMEM buffer is saved in
                                 params.buffer.
                                 params.size contains the requested size rounded
                                 up to 8 bytes.
                                 params.flags contains the status of the cached
                                 flag, which may not match what was
                                 requested if the item has been allocated
                                 previously.
  SMEM_STATUS_SIZE_MISMATCH    - Allocation exists, but has different size.
                                 This is possible when another processor has
                                 already allocated an item with this SMEM ID and
                                 a different size.
                                 Pointer to SMEM buffer is saved in
                                 params.buffer.
                                 params.size contains the originally allocated
                                 size rounded up to 8 bytes.
                                 params.flags contains the status of the cached
                                 flag, which may not match what was
                                 requested if the item has been allocated
                                 previously.
  SMEM_STATUS_INVALID_PARAM    - Invalid parameter
  SMEM_STATUS_OUT_OF_RESOURCES - Not enough SMEM to allocate this buffer
  SMEM_STATUS_FAILURE          - General failure

  @sideeffects
  This function uses spinlocks for exclusive access to the shared memory heap.
*/
/*===========================================================================*/
int32 smem_part_alloc_ex
(
  smem_alloc_params_type *params
);

/*=============================================================================
FUNCTION      smem_get_addr_ex
=============================================================================*/
/**
  Requests the address and size of an allocated buffer in shared memory.
  If found, sets the buffer and size fields of the params struct.

  @param[inout] params  params.smem_type must be set to the ID to search for.
                        params.remote_host must be set to the ID of the remote
                        host of the partition.
                        If params.flags has the SMEM_ALLOC_FLAG_PARTITION_ONLY
                        flag set, then this function will not search the default
                        partition for the item.  This is useful when SMEM item
                        X is possible in both the default partition and an edge-
                        pair partition, and the caller wants to find out if the
                        item exists in the edge-pair partition only.

  @return
  SMEM_STATUS_SUCCESS       - Allocated buffer found
                              params.buffer contains a pointer to the allocation.
                              params.size contains the size of the allocation
                              as originally requested, rounded up to 8 bytes.
                              params.flags contains the status of the cached
                              flag.
  SMEM_STATUS_NOT_FOUND     - Allocated buffer not found
  SMEM_STATUS_INVALID_PARAM - Invalid parameter
  SMEM_STATUS_FAILURE       - Failure occurred

  @sideeffects
  This function uses spinlocks for exclusive access to the shared memory heap.
  This function updates the size_remaining field for the partition.
*/
/*===========================================================================*/
int32 smem_part_get_addr_ex
(
  smem_alloc_params_type *params
);

/*=============================================================================
FUNCTION      smem_part_alloc
=============================================================================*/
/**
@brief        Request a pointer to an item in shared/external memory. If the
              space for the item has been allocated in either shared/external 
              memory, return the pointer to that location, otherwise allocate
              buffer for the specified item from the shared memory.

@param[in]    smem_type     Type of memory to get a pointer for.
@param[in]    buf_size      Size of the buffer requested.  For pre-allocated
                            buffers, an ERR_FATAL will occur if the buffer
                            size requested does not match the size of the
                            existing buffer.  If the buf_size
                            is not 64-bit aligned, this function will top
                            the size off to an aligned one.

@dependencies None

@return      return NULL if smem_boot_init is not called yet. Otherwise, either
             return a valid SMEM address to the requested buffer or error fatal

@sideeffect   Uses spinlocks for exclusive access to the shared memory heap.
*/
/*===========================================================================*/
void *smem_part_alloc
(
  smem_mem_type smem_type,
  uint32        buf_size
);

/*=============================================================================
FUNCTION      smem_part_get_addr
=============================================================================*/
/**
@brief        Request a pointer to an already allocated buffer in shared
              memory. Returns the address and size of the allocated buffer.

              Newly-allocated shared memory buffers, which have never been
              allocated on any processor, are guaranteed to be zeroed.

@param[in]    smem_type     Type of memory to get a pointer for.
@param[out]   buf_size      (Output) Size of buffer allocated in shared
                            memory.

@dependencies The buffer must already have been allocated.

@return       Pointer to the requested buffer, or NULL if the buffer has
              not been allocated.

@sideeffect   Uses spinlocks for exclusive access to the shared memory heap.
*/
/*===========================================================================*/
void *smem_part_get_addr
(
  smem_mem_type  smem_type,
  uint32        *buf_size
);

#ifdef __cplusplus
}
#endif

#endif /* SMEM_PARTITION_H */
