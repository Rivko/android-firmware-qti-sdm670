/*=============================================================================

                           SMEM LEGACY

 Copyright (c)  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/inc/smem_legacy.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/16   rv      Initial version.
===========================================================================*/

#ifndef SMEM_LEGACY_H
#define SMEM_LEGACY_H

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


/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/


/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/
extern const smem_funcs_type smem_legacy_funcs;

/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
  FUNCTION  smem_legacy_init
=============================================================================*/
/**
  Initializes the shared memory allocation structures.

  @return
  None.

  @dependencies
  Shared memory must have been cleared and initialized by the first system 
  bootloader.
 */
/*===========================================================================*/
void smem_legacy_init(void);

/*===========================================================================
FUNCTION      smem_legacy_alloc
===========================================================================*/
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
/*=========================================================================*/
void *smem_legacy_alloc
(
  smem_mem_type smem_type,
  uint32        buf_size
);

/*===========================================================================
FUNCTION      smem_legacy_get_addr
===========================================================================*/
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
/*=========================================================================*/
void *smem_legacy_get_addr
(
  smem_mem_type  smem_type,
  uint32        *buf_size
);

/*===========================================================================
FUNCTION      smem_legacy_alloc_ex
===========================================================================*/
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
/*=========================================================================*/
int32 smem_legacy_alloc_ex
(
  smem_alloc_params_type *params
);

/*===========================================================================
FUNCTION      smem_legacy_get_addr_ex
===========================================================================*/
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
/*=========================================================================*/
int32 smem_legacy_get_addr_ex
(
  smem_alloc_params_type *params
);

#ifdef __cplusplus
}
#endif

#endif /* SMEM_LEGACY_H */
