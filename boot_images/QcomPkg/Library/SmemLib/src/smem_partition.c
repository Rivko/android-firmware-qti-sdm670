/*=============================================================================

               Shared Memory Partition Implementation File

 Copyright (c)  2013-2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/src/smem_partition.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/14/16   vr      SMEM Version 0x000C0000 changes.
07/10/15   db      Fixes in smem_alloc_ex while allocating from legacy partition
05/06/15   db      Fixed conditions to deprecate old SMEM_INVALID_HOST and assign 
                   new value to SMEM_INVALID_HOST (-1)
06/04/14   rv      params.flags is properly updated while returning from 
                   smem_alloc_ex()/smem_get_addr_ex_locked()
03/13/14   sm      64-bit support.
02/19/14   an      smem refactored to prevent deadlock in qnx
12/06/13   an      Fix address calculation for preallocated items in cached partition.
10/21/13   bt      Check remote_host range before accessing smem_info_prt.
10/09/13   bt      Ensure to map partition pages before accessing.
08/09/13   bt      Add smem_get_part_sz, for internal support.
07/29/13   pa      Make SMD use secure SMEM partitions, with BW compatibility.
07/02/13   pa      Enable caller to use remote_host == SMEM_THIS_HOST to
                   access default partition.  This makes it easier to loop
                   through SMEM hosts.
06/25/13   pa      Remove local list usage; instead just search SMEM.
                   Use smem_get_base_addr and smem_get_size for SMEM base and
                   size.
                   Changed documentation and naming from 'upwards' to 
                   'uncached' and 'downwards' to 'cached' to avoid confusion.
                   Added canary and padding size to allocation header.
                   Hide padding and actual SMEM allocation from caller by 
                   using saved padding values.
                   Minor changes to support backwards compatibility with 
                   legacy SMEM allocation table.
                   Other misc cleanup.
06/06/13   bt      Resolve some type conversion warnings.
05/30/13   pa      Extern smem_init_info to get virtual SMEM base.
04/24/13   pa      Initial version.
===========================================================================*/

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "smem_os.h"
#include "smem.h"
#include "smem_v.h"
#include "smem_version.h"
#include "smem_target.h"
#include "spinlock.h"
#include "smem_toc.h"
#include "smem_internal.h"
#include "smem_partition.h"

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/


/*=============================================================================

                       PUBLIC/GLOBAL DATA DEFINATIONS

=============================================================================*/
const smem_funcs_type smem_part_funcs = {
#if defined(SMEM_FEATURE_ALLOC_SUPPORT)
  smem_part_alloc,
#else /* !SMEM_FEATURE_ALLOC_SUPPORT */
  NULL,
#endif /* SMEM_FEATURE_ALLOC_SUPPORT */

#if defined(SMEM_FEATURE_GET_ADDRS_SUPPORT)
  smem_part_get_addr,
#else /* !SMEM_FEATURE_GET_ADDRS_SUPPORT */
  NULL,
#endif /* SMEM_FEATURE_GET_ADDRS_SUPPORT */

#if defined(SMEM_FEATURE_ALLOC_SUPPORT)
  smem_part_alloc_ex,
#else /* !SMEM_FEATURE_ALLOC_SUPPORT */
  NULL,
#endif /* SMEM_FEATURE_ALLOC_SUPPORT */

#if defined(SMEM_FEATURE_GET_ADDRS_SUPPORT)
  smem_part_get_addr_ex,
#else /* !SMEM_FEATURE_GET_ADDRS_SUPPORT */
  NULL,
#endif /* SMEM_FEATURE_GET_ADDRS_SUPPORT */
};

/*=============================================================================

                       LOCAL DATA DEFINATIONS

=============================================================================*/
/** The main info structure for SMEM partitions. 
 ** Contains local copy of allocation info from each of the SMEM partitions.
 ** Updated on calls to smem_get_addr_ex() */
#if defined(SMEM_FEATURE_PARTITION_SUPPORT)
static smem_partition_info_type smem_info_prt[SMEM_NUM_HOSTS];
#else /* !SMEM_FEATURE_PARTITION_SUPPORT */
static smem_partition_info_type smem_info_prt;
#endif
/*=============================================================================

                      LOCAL FUNCTION DEFINATIONS

=============================================================================*/
/*=============================================================================
FUNCTION      smem_part_ensure_uncached_mapping
=============================================================================*/
/**
  Compares the current offset index for the mapped uncached pages in the
  partition against the new offset of actual uncached SMEM allocations.  If
  the new allocation(s) fall outside the currently mapped pages (new_offset is
  greater than the current mapped offset), the difference will be mapped,
  rounded up to the nearest minimum page size multiple.

  @param[in]    info        Pointer to the local partition info structure.
  @param[in]    new_offset  The new offset that must be mapped up to
                            (as uncached memory).

  @return
  None.
*/
/*===========================================================================*/
static void smem_part_ensure_uncached_mapping
(
  smem_partition_info_type *info,
  uint32                    new_offset
)
{
  if (new_offset > info->offset_mapped_uncached)
  {
    uint32 mapping_size   = new_offset - info->offset_mapped_uncached;
    uint32 mapping_offset = info->offset_mapped_uncached +
                            (uint32)((uint8 *)info->header - SMEM_INFO_SMEM_BASE_ADDR());
                            
    mapping_size = smem_os_map_page_attr(mapping_offset,
                                         mapping_size, 
                                         FALSE);
    info->offset_mapped_uncached += mapping_size;
  }
}

/*=============================================================================
FUNCTION      smem_part_ensure_cached_mapping
=============================================================================*/
/**
  Compares the current offset index for the mapped cached pages in the
  partition against the new offset of actual cached SMEM allocations.  If
  the new allocation(s) fall outside the currently mapped pages (new_offset is
  less than the current mapped offset), the difference will be mapped,
  rounded up to the nearest minimum page size multiple.

  @param[in]    info        Pointer to the local partition info structure.
  @param[in]    new_offset  The new offset that must be mapped down to
                            (as cached memory).

  @return
  None.
*/
/*===========================================================================*/
static void smem_part_ensure_cached_mapping
(
  smem_partition_info_type *info,
  uint32                    new_offset
)
{
  if (new_offset < info->offset_mapped_cached)
  {
    uint32 mapping_size   = info->offset_mapped_cached - new_offset;
    uint32 mapping_offset = new_offset +
                            (uint32)((uint8 *)info->header - SMEM_INFO_SMEM_BASE_ADDR());

    mapping_size = smem_os_map_page_attr(mapping_offset,
                                         mapping_size, 
                                         TRUE);
    info->offset_mapped_cached -= mapping_size;
  }
}

/*=============================================================================
FUNCTION      smem_part_process_item
=============================================================================*/
/**
  Reads info about an allocation from SMEM.

  This function was added to combine and reuse code in smem_get_addr_ex.
  It shouldn't be called from other functions.

  @param[in]    info      Pointer to the local partition info structure.
  @param[in]    params    If new allocation matches the smem type here,
                          the function returns TRUE and params.buffer and
                          params.size are updated.
  @param[inout] item_hdr  Pointer to item allocation header
  @param[in]    padding_header  Number of bytes of padding on the item header
  @param[inout] size_remaining  Number of bytes remaining in the heap.
                                Used to validate size of new item.
  @param[in]    cached    Flag indicating if this is a cached item

  @return
  TRUE  - if item matches searched for item
  FALSE - otherwise
*/
/*===========================================================================*/
static boolean smem_part_process_item
(
  smem_partition_info_type              *info,
  smem_alloc_params_type                *params,
  smem_partition_allocation_header_type **item_hdr,
  uint16                                padding_header,
  uint32                                *size_remaining,
  boolean                               cached
)
{
  uint32 size_item;
  uint16 smem_type;
  uint16 canary;
  uint32 size_total;
  uint32 padding_data;
  boolean found = FALSE;

  /* Read details from SMEM */
  canary    = SMEM_READ_SMEM_2(&((*item_hdr)->canary));
  smem_type = SMEM_READ_SMEM_2(&((*item_hdr)->smem_type));
  size_item = SMEM_READ_SMEM_4(&((*item_hdr)->size));

  /* Calculate total size of the item */
  size_total = size_item + padding_header +
               sizeof(smem_partition_allocation_header_type);

  /* Canary check */
  if (canary != SMEM_ALLOC_HDR_CANARY)
  {
    SMEM_OS_LOG_ERR(2, "Invalid SMEM canary 0x%X at location 0x%X",
        canary, (uint32)(*item_hdr));
    return FALSE;
  }

  if (size_item > info->size)
  {
    SMEM_OS_LOG_ERR(3, "Invalid SMEM item size:%d, Partition size:%d smem_type:0x%04X.",
        size_item, info->size, smem_type);
    return FALSE;
  }
  
  /* Size check */
  if (size_total > *size_remaining)
  {
    SMEM_OS_LOG_ERR(2, "Invalid SMEM item size. Total size: %d, remaining: %d",
        size_total, *size_remaining);
    return FALSE;
  }

  /* Update remaining size */
  *size_remaining -= size_total;

  /* Finally, check if this is the item we are searching for */
  if (smem_type == params->smem_type)
  {
    if (cached)
    {
      /* buffer is directly below the header */
      params->buffer = (void *)((size_t)(*item_hdr) - size_item);
      params->flags |= SMEM_ALLOC_FLAG_CACHED;
    }
    else
    {
      /* buffer is after header */
      params->buffer = (void *)((size_t)(*item_hdr) +
        sizeof(smem_partition_allocation_header_type));
      params->flags &= ~SMEM_ALLOC_FLAG_CACHED;
    }

    /* remove padding size when returning to caller */
    padding_data = SMEM_READ_SMEM_2(&(*item_hdr)->padding_data);
    if (padding_data > size_item)
    {
      SMEM_OS_LOG_ERR(3, "Invalid padding data size:%d, size_item:%d smem_type:0x%04X.",
          padding_data, size_item, smem_type);
      return FALSE;
    }

    params->size = size_item - padding_data;

    /* but round up to 8 bytes to be consistent with legacy smem_get_addr */
    params->size = ROUND_UP(params->size, 8);

    found = TRUE;
  }

  /* Point to the next item */
  if (cached)
  {
    *item_hdr = (smem_partition_allocation_header_type *)
                   ((size_t)(*item_hdr) - size_total);
  }
  else
  {
    *item_hdr = (smem_partition_allocation_header_type *)
                   ((size_t)(*item_hdr) + size_total);
  }

  return found;
}

/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

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
#if defined(SMEM_FEATURE_ALLOC_SUPPORT)
int32 smem_part_alloc_ex
(
  smem_alloc_params_type *params
)
{
  int32  status;
  uint32 size_rounded8;
  uint32 size_remaining;
  uint32 size_item;
  uint32 size_total;
  uint16 padding_data;

  smem_partition_allocation_header_type *item_hdr;
  smem_partition_info_type              *info = NULL;

  if (params->remote_host != SMEM_INVALID_HOST &&
      params->remote_host >= SMEM_NUM_HOSTS)
  {
    return SMEM_STATUS_INVALID_PARAM;
  }
  
  /* Check smem_type and size fields for valid params .
   * Note that size is checked in more detail further below. */
  if (params->smem_type >= SMEM_INFO_SMEM_MAX_ITEMS() ||
      params->size == 0 ||
      params->size > SMEM_INFO_SMEM_SIZE())
  {
    return SMEM_STATUS_INVALID_PARAM;
  }

#if defined(SMEM_FEATURE_PARTITION_SUPPORT)
  /* If remote host is invalid or local host, allocate the memory
   * from common partition */
  if (params->remote_host == SMEM_INVALID_HOST)
  {
    info = &smem_info_prt[SMEM_THIS_HOST];
  }
  else
  {
    /* Set local info pointer to make the code less wordy */
    info = &smem_info_prt[params->remote_host];
  }
#else /* !SMEM_FEATURE_PARTITION_SUPPORT */
  if ((params->remote_host == SMEM_INVALID_HOST) ||
      (params->remote_host == SMEM_THIS_HOST))
  {
    info = &smem_info_prt;
  }
  else
  {
    return SMEM_STATUS_INVALID_PARAM;
  }
#endif

  /* Support only partition allocations */
  if (info->size == 0)
  {
    params->buffer = NULL;
    return SMEM_STATUS_OUT_OF_RESOURCES;
  }
  
  /* Save the size that the caller requested.
   * Round up to be consistent with legacy smem_alloc(). */
  size_rounded8 = ROUND_UP(params->size, 8);

  /* If allocating from the top (cached range), add padding size */
  if (info != NULL && (params->flags & SMEM_ALLOC_FLAG_CACHED))
  {
    /* Round size up to a cacheline */
    size_item = ROUND_UP(params->size, info->size_cacheline);

    /* Total size is rounded size, plus header size, plus padding */
    size_total = size_item + sizeof(smem_partition_allocation_header_type) +
      SMEM_PARTITION_ITEM_PADDING(info->size_cacheline);
  }
  else
  {
    /* Round size to 8 bytes */
    size_item = size_rounded8;

    /* Total size is rounded size param plus size of the header */
    size_total = size_item + sizeof(smem_partition_allocation_header_type);
  }

  /* Save the amount that we've rounded up.  This is used to hide the rounding
   * up from the caller, who may ignore the updated size param that is returned */
  padding_data = size_item - params->size;

  /* Grab SMEM spin lock.
   * Spinlock is acquired here to make the scan + allocation atomic.
   * Unlocking inbetween the scan and allocation would allow a remote processor
   * to modify smem and we would be out of sync. */
  smem_spin_lock(SMEM_SPINLOCK_SMEM_ALLOC);
  
  /* Check if item already exists.  Note that this function will update the
   * params structure if found. */
  status = smem_part_get_addr_ex(params);
  if (SMEM_STATUS_SUCCESS == status)
  {
    smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);

    /* If size does not match, indicate so in the return value */
    return params->size == size_rounded8 ?
           SMEM_STATUS_SUCCESS :
           SMEM_STATUS_SIZE_MISMATCH;
  }

  SMEM_ACCESS_CONFIGURE_SHARED_RAM();

  if (params->flags & SMEM_ALLOC_FLAG_CACHED)
  {
    /* Calculate size remaining, which is the difference between the cached
     * offset and the furthest uncached page that has been mapped.  Note that
     * these pointers are updated in the info struct in smem_get_addr_ex(). */
    size_remaining = info->offset_free_cached - info->offset_mapped_uncached;

    /* Check for room in the partition heap.  Do this after checking for smem
     * item already existing to ensure that size_remaining is up to date. */
    if (size_total > size_remaining)
    {
      smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
      return SMEM_STATUS_OUT_OF_RESOURCES;
    }

    /* Allocate from cached side.
     * Note that the padding exists above the item header and below the previous
     * item. */
    item_hdr = (smem_partition_allocation_header_type*)
                    ((size_t)info->header +
                             info->offset_free_cached -
                             sizeof(smem_partition_allocation_header_type) -
                             SMEM_PARTITION_ITEM_PADDING(info->size_cacheline));
    params->buffer = (void *)((size_t)info->header +
                                      info->offset_free_cached -
                                      size_total);
    params->size = size_rounded8;

    /* We must map any additional required pages at least down to the bottom
     * of this new item. */
    smem_part_ensure_cached_mapping(info, info->offset_free_cached - size_total);

    /* Fill in the item header in SMEM before updating the offset */
    SMEM_WRITE_SMEM_2(&item_hdr->canary,          SMEM_ALLOC_HDR_CANARY);
    SMEM_WRITE_SMEM_2(&item_hdr->smem_type,       params->smem_type);
    SMEM_WRITE_SMEM_4(&item_hdr->size,            size_item);
    SMEM_WRITE_SMEM_2(&item_hdr->padding_data,    padding_data);
    SMEM_WRITE_SMEM_2(&item_hdr->padding_header,
      SMEM_PARTITION_ITEM_PADDING(info->size_cacheline));

    /* Update the offset, both locally and in smem */
    info->offset_free_cached -= size_total;
    SMEM_WRITE_SMEM_4(&info->header->offset_free_cached, info->offset_free_cached);

    /* Increment debug count */
    info->debug_info.num_alloc_cached++;
  }
  else
  {
    /* Calculate size remaining, which is the difference between the uncached
     * offset and the furthest cached page that has been mapped.  Note that
     * these pointers are updated in the info struct in smem_get_addr_ex(). */
    size_remaining = info->offset_mapped_cached - info->offset_free_uncached;

    /* Check for room in the partition heap.  Do this after checking for smem
     * item already existing to ensure that size_remaining is up to date. */
    if (size_total > size_remaining)
    {
      smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
      return SMEM_STATUS_OUT_OF_RESOURCES;
    }

    /* Allocate from non-cached side */
    item_hdr = (smem_partition_allocation_header_type*)
                    ((size_t)info->header +
                             info->offset_free_uncached);
    params->buffer = (void *)((size_t)info->header +
                                      info->offset_free_uncached +
                                      sizeof(smem_partition_allocation_header_type));
    params->size = size_rounded8;

    /* We must map any additional required pages at least up to the end of this
     * new item. */
    smem_part_ensure_uncached_mapping(info, info->offset_free_uncached + size_total);

    /* fill in the item header in SMEM before updating the offset */
    SMEM_WRITE_SMEM_2(&item_hdr->canary,          SMEM_ALLOC_HDR_CANARY);
    SMEM_WRITE_SMEM_2(&item_hdr->smem_type,       params->smem_type);
    SMEM_WRITE_SMEM_4(&item_hdr->size,            size_item);
    SMEM_WRITE_SMEM_2(&item_hdr->padding_data,    padding_data);
    SMEM_WRITE_SMEM_2(&item_hdr->padding_header,  0);

    /* update the offset, both locally and in smem */
    info->offset_free_uncached += size_total;
    SMEM_WRITE_SMEM_4(&info->header->offset_free_uncached, info->offset_free_uncached);

    /* Increment debug count */
    info->debug_info.num_alloc_uncached++;
  }
  
  smem_os_mem_barrier();
  
  SMEM_ACCESS_CONFIGURE_RESTORE();

  /* End critical section */
  smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);

  return SMEM_STATUS_SUCCESS;
}
#endif /* SMEM_FEATURE_ALLOC_SUPPORT */

/*=============================================================================
FUNCTION      smem_part_get_addr_ex
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
)
{
  smem_partition_info_type              *info;
  smem_partition_allocation_header_type *item_hdr;
  uint32 size_remaining;
  uint32 limit_addr;
  uint16 padding_header;

  if (params->remote_host != SMEM_INVALID_HOST &&
      params->remote_host >= SMEM_NUM_HOSTS)
  {
    return SMEM_STATUS_INVALID_PARAM;
  }
#if defined(SMEM_FEATURE_PARTITION_SUPPORT)
  /* If remote host is invalid or local host, allocate the memory
   * from common partition */
  if (params->remote_host == SMEM_INVALID_HOST)
  {
    info = &smem_info_prt[SMEM_THIS_HOST];
  }
  else
  {
    info = &smem_info_prt[params->remote_host];
  }
#else
  /* Only supports common allocations */
  if ((params->remote_host == SMEM_INVALID_HOST) ||
      (params->remote_host == SMEM_THIS_HOST))
  {
    info = &smem_info_prt;
  }
  else
  {
    return SMEM_STATUS_INVALID_PARAM;
  }
#endif

  /* Support only partition allocations */
  if (info->size == 0)
  {
    params->buffer = NULL;
    return SMEM_STATUS_NOT_FOUND;
  }

  /* Grab multiprocessor lock on SMEM allocations */
  smem_spin_lock(SMEM_SPINLOCK_SMEM_ALLOC);
  
  SMEM_ACCESS_CONFIGURE_SHARED_RAM();

  /* Read the heap pointers in SMEM. */
  info->offset_free_cached   = SMEM_READ_SMEM_4(
      &info->header->offset_free_cached);
  info->offset_free_uncached = SMEM_READ_SMEM_4(
      &info->header->offset_free_uncached);

  /* Check pointer values.
   * Uncached offset must always be less than or equal to cached offset.
   * Both offsets must be less than the size of the partition.
   * This protects against a possible overflow outside of the partition. */
  if ((info->offset_free_uncached > info->offset_free_cached) ||
      (info->offset_free_uncached > info->size) ||
      (info->offset_free_cached   > info->size))
  {
    SMEM_ACCESS_CONFIGURE_RESTORE();
    smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
    SMEM_OS_LOG_ERR(2, "Invalid SMEM heap pointers. Uncached: 0x%X, cached: 0x%X",
        info->offset_free_uncached, info->offset_free_cached);
    return SMEM_STATUS_OUT_OF_RESOURCES;
  }

  /* We must map any unmapped pages at least up to the offset_free_uncached,
   * before accessing those items and their headers. */
  smem_part_ensure_uncached_mapping(info, info->offset_free_uncached);

  /* Init size.  We will use this to check for valid item sizes during the
   * scan. */
  size_remaining = info->size - sizeof(smem_partition_header_type);

  /* Padding is 0 for uncached allocations */
  padding_header = 0;

  /* Point to first item */
  item_hdr = (smem_partition_allocation_header_type *)
                ((uint32)info->header + sizeof(smem_partition_header_type));

  /* Set limit of scan */
  limit_addr = (uint32)info->header + info->offset_free_uncached;

  /* Scan from uncached side of heap */
  while ((uint32)item_hdr < limit_addr)
  {
    /* Process the next item, and check if it is the item we are searching
     * for. */
    if (smem_part_process_item(
          info,             /* Pointer to the local partition info */
          params,           /* callers parameters */
          &item_hdr,        /* pointer to item header in SMEM */
          padding_header,   /* size of padding */
          &size_remaining,  /* size remaining */
          FALSE))           /* cached flag */
    {
      SMEM_ACCESS_CONFIGURE_RESTORE();
      smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
      return SMEM_STATUS_SUCCESS;
    }
  }

  /* Now map any required pages in the cached direction. */
  smem_part_ensure_cached_mapping(info, info->offset_free_cached);

  /* Padding size is fixed for cached allocations based on the TOC */
  padding_header = SMEM_PARTITION_ITEM_PADDING(info->size_cacheline);

  /* Scan from cacheable side of heap.
   * Point to first item */
  item_hdr = (smem_partition_allocation_header_type *)
                ((uint32)info->header + info->size - padding_header -
                sizeof(smem_partition_allocation_header_type));

  /* Set limit of scan */
  limit_addr = (uint32)info->header + info->offset_free_cached;

  /* Scan from cached side of heap */
  while ((uint32)item_hdr >= limit_addr)
  {
    /* Process the next item, and check if it is the item we are searching
     * for. */
    if (smem_part_process_item(
          info,             /* Pointer to the local partition info */
          params,           /* callers parameters */
          &item_hdr,        /* pointer to item header in SMEM */
          padding_header,   /* size of padding */
          &size_remaining,  /* size remaining */
          TRUE))            /* cached flag */
    {
      SMEM_ACCESS_CONFIGURE_RESTORE();
      smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
      return SMEM_STATUS_SUCCESS;
    }
  }

  SMEM_ACCESS_CONFIGURE_RESTORE();
  /* End critical section */
  smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);

  return SMEM_STATUS_NOT_FOUND;
}

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
#if defined(SMEM_FEATURE_ALLOC_SUPPORT)
void *smem_part_alloc
(
  smem_mem_type smem_type,
  uint32        buf_size
)
{
  smem_alloc_params_type params;
  int32                  ret;
  
  if (smem_type <= SMEM_LAST_FIXED_BUFFER)
  {
    SMEM_OS_LOG_ERR(0, "Fixed buffer allocation not supported in new SMEM version");
    return NULL;
  }
  
  /* Allocate the memory from common partition */
  params.remote_host = SMEM_INVALID_HOST;
  params.smem_type   = smem_type;
  params.size        = buf_size;
  params.buffer      = NULL;
  params.flags       = SMEM_ALLOC_FLAG_NONE;

  ret = smem_part_alloc_ex(&params);
  
  return (ret == SMEM_STATUS_SUCCESS) ? params.buffer : NULL;
}
#endif /* SMEM_FEATURE_ALLOC_SUPPORT */

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
#if defined(SMEM_FEATURE_GET_ADDRS_SUPPORT)
void *smem_part_get_addr
(
  smem_mem_type  smem_type,
  uint32        *buf_size
)
{
  smem_alloc_params_type params;
  int32                  ret;
  
  /* Get memory address from common partition */
  params.remote_host = SMEM_INVALID_HOST;
  params.smem_type   = smem_type;
  params.size        = 0;
  params.buffer      = NULL;
  params.flags       = SMEM_ALLOC_FLAG_NONE;

  ret = smem_part_get_addr_ex(&params);
  *buf_size = params.size;
  return (ret == SMEM_STATUS_SUCCESS) ? params.buffer : NULL;
}
#endif /* SMEM_FEATURE_GET_ADDRS_SUPPORT */

/*=============================================================================
FUNCTION      smem_part_init
=============================================================================*/
/**
  Initializes the SMEM partitioning structures.

  This function checks for the existence of the SMEM table of contents and then
  reads it to determine partition locations and size.  Data structures are
  initialized to enable SMEM allocation from protected partitions.

*/
/*===========================================================================*/
SMEM_OS_INIT_FUNC_RECLAIM_SECTION 
void smem_part_init(void)
{
  smem_alloc_params_type params;
  smem_toc_type          *toc;
  uint8                  *smem_base;
  uint32                 smem_size;
  
  uint32 i;
  uint32 size;
  int32  status;

  /* Initialize size to 0 */
#if defined(SMEM_FEATURE_PARTITION_SUPPORT)
  for (i = 0; i < SMEM_NUM_HOSTS; i++)
  {
    /* Init size to zero.  After this routine is finished, size of 0 indicates
     * that there is no protected partition for the edge */
    smem_info_prt[i].size = 0;
  }
#else /* !SMEM_FEATURE_PARTITION_SUPPORT */
  smem_info_prt.size = 0;
#endif /* SMEM_FEATURE_PARTITION_SUPPORT */

  SMEM_ACCESS_CONFIGURE_SHARED_RAM();

  /* Calculate SMEM TOC location and map it if necessary. */
  smem_base = SMEM_INFO_SMEM_BASE_ADDR();
  smem_size = SMEM_INFO_SMEM_SIZE();
  smem_os_map_page_attr(smem_size - SMEM_TOC_SIZE, SMEM_TOC_SIZE, FALSE);
  
  toc = (smem_toc_type *)(smem_base + smem_size - SMEM_TOC_SIZE);

  /* Check if we have a valid TOC. If not, nothing to do.
   * Absence of TOC is supported as the Secure SMEM feature is rolled out.
   * When there is no TOC, calls to smem_alloc_ex and smem_get_addr_ex will
   * call the legacy functions which operate on non-protected SMEM. */
  if (SMEM_READ_SMEM_4(&toc->identifier) != SMEM_TOC_IDENTIFIER)
  {
    SMEM_ACCESS_CONFIGURE_RESTORE();
    return;
  }

  /* Check that TOC has the expected fields */
  SMEM_OS_LOG_ASSERT(SMEM_READ_SMEM_4(&toc->version) == 1);
  SMEM_OS_LOG_ASSERT(SMEM_READ_SMEM_4(&toc->num_entries) <= SMEM_MAX_PARTITIONS);

  /* Loop through the TOC and grab the following info:
   * From the TOC, get the base addr and subsystem IDs of the partition owners.
   * From the partition headers, get the partition size since this value has
   * the exclusion ranges already subtracted out. */
  for (i = 0; i < toc->num_entries; i++)
  {
    smem_toc_entry_type *entry = &toc->entry[i];
    
    smem_host_type host0 = (smem_host_type)SMEM_READ_SMEM_2(&entry->host0);
    smem_host_type host1 = (smem_host_type)SMEM_READ_SMEM_2(&entry->host1);
    smem_host_type remote_host;

    /* Check for common partition, host ids should be 0xFFFE */
    if (host0 == SMEM_COMMON_HOST && host1 == SMEM_COMMON_HOST)
    {
      /* Use local host for common partition */
      remote_host = SMEM_THIS_HOST;
    }
    else
    {
      /* Determine if the local host is part of this entry, and what the remote
       * host is as well. If local host is not part of this entry, the variable
       * is set to invalid. */
      remote_host = host0 == SMEM_THIS_HOST ? host1 :
                    host1 == SMEM_THIS_HOST ? host0 :
                    SMEM_INVALID_HOST;
    }
    /* ignore remote host if local host does not know about remote host ID */
    if (remote_host >= SMEM_NUM_HOSTS)
    {
      remote_host = SMEM_INVALID_HOST;
    }

    /* Read size of the partition from the TOC entry */
    size = SMEM_READ_SMEM_4(&entry->size);
        
    /* Skip this entry if TOC size is 0 or host is invalid */
    if (size != 0 && remote_host != SMEM_INVALID_HOST)
    {
      uint32 partition_offset = SMEM_READ_SMEM_4(&entry->offset);
      uint32 size_cacheline   = SMEM_READ_SMEM_4(&entry->size_cacheline);
      
      smem_partition_header_type *hdr;
      uint32                     offset_mapped_uncached;
      uint32                     part_size;

      /* Map first page of partition, in order to access the header. */
      offset_mapped_uncached = smem_os_map_page_attr(partition_offset,
          sizeof(smem_partition_header_type), FALSE);
      hdr = (smem_partition_header_type *)(smem_base + partition_offset);

      SMEM_OS_LOG_ASSERT(((unsigned int)hdr & SMEM_PAGE_ALIGN_MASK) == 0);
      SMEM_OS_LOG_ASSERT(hdr->identifier == SMEM_PARTITION_HEADER_ID);
      SMEM_OS_LOG_ASSERT(partition_offset > 0);
      SMEM_OS_LOG_ASSERT(partition_offset < smem_size - SMEM_TOC_SIZE);

      /* Read size from partition header and validate it against entry->size */
      part_size = SMEM_READ_SMEM_4(&hdr->size);
      if (part_size != size)
      {
        SMEM_OS_LOG_ERR(4, "Invalid partition size(%d). Actual partition size:%d,"
                        "host0:0x%04X, host1:0x%04X.", part_size, size, host0, host1);
        continue;
      }

      /* If size is non-zero, then we have a valid partition */
      if (size)
      {
        smem_partition_info_type *prt;
#if defined(SMEM_FEATURE_PARTITION_SUPPORT)
        prt = &smem_info_prt[remote_host];
#else /* !SMEM_FEATURE_PARTITION_SUPPORT */
        if (remote_host == SMEM_THIS_HOST)
        {
          prt = &smem_info_prt;
        }
        else
        {
          continue;
        }
#endif /* SMEM_FEATURE_PARTITION_SUPPORT */
        
        /* Init local info structure to default values indicating that
         * there are no known SMEM items.
         * These will be updated on the call to smem_get_addr_ex. */
        prt->header                 = hdr;
        prt->size                   = size;
        prt->size_cacheline         = size_cacheline;
        prt->offset_free_uncached   = sizeof(smem_partition_header_type);
        prt->offset_free_cached     = size;
        prt->offset_mapped_uncached = offset_mapped_uncached;
        prt->offset_mapped_cached   = prt->offset_free_cached;

        /* Clear out debug info */
        memset(&prt->debug_info, 0, sizeof(prt->debug_info));

        /* Scan SMEM to read the current state of partition allocations.
         * This call will update the info struct for this edge.
         * It doesn't matter what item ID we search for since the allocation list
         * is empty at this point.  The only needed param is the remote_host
         * so that we scan the right partition. */
        params.remote_host = remote_host;
        params.smem_type   = SMEM_MEM_FIRST;
        
        /* Restore SMEM access config as smem_part_get_addr_ex() do it again */
        SMEM_ACCESS_CONFIGURE_RESTORE();
        
        status = smem_part_get_addr_ex(&params);
        
        SMEM_ACCESS_CONFIGURE_SHARED_RAM();
        
        if (status != SMEM_STATUS_SUCCESS && status != SMEM_STATUS_NOT_FOUND)
        {
          SMEM_OS_LOG_FATAL(2, "SMEM Partition %d init failed with status 0x%x",
                remote_host, status);
        }
      }
    }
  }
  
  SMEM_ACCESS_CONFIGURE_RESTORE();
}

/*===========================================================================
FUNCTION      smem_get_partition_sz
===========================================================================*/
/**
  Return the size of the SMEM partition between the specified remote_host and
  the local processor.

  @param[in] remote_host  The remote processor's smem_mem_type.

  @return
  The size of the SMEM partition, if any, shared with remote_host.
  0 if no partition for this edge has been reserved.
*/
/*=========================================================================*/
#if defined(SMEM_FEATURE_PARTITION_SUPPORT)
uint32 smem_get_partition_sz(smem_host_type remote_host)
{
  return (remote_host < SMEM_NUM_HOSTS) ? smem_info_prt[remote_host].size : 0;
}
#endif
