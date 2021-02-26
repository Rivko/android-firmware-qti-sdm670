/*=============================================================================

                          SMEM LEGACY

 Copyright (c)  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/src/smem_legacy.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/06/16   rv      Initial version.
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
#include "smem_internal.h"
#include "smem_partition.h"
#include "smem_legacy.h"

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/
/* Flags for smem_allocation_table_type.flags */
/* The last two bits of base_addr field (previously reserved) are
 * still kept reserved for future use */
#define SMEM_WORD_ALIGN_MASK            0x00000003
#define SMEM_ALLOC_TBL_RESERVED_MASK    SMEM_WORD_ALIGN_MASK

typedef struct {
  volatile smem_heap_info_type *smem_heap_info;

/*----------------------------------------------------------------------
 * Allocation table - keeps track of which buffers have been allocated
 * and their sizes. Buffers are only allocated once, subsequent calls
 * to smem_alloc for that buffer return the same address regardless of
 * where smem_alloc is called.
 *----------------------------------------------------------------------*/
  volatile smem_legacy_allocation_table_type *smem_allocation_table;

/*----------------------------------------------------------------------
 * Table describing shared memory layout of the fixed buffers.
 * This table matches the shared memory layout 1.0
 *----------------------------------------------------------------------*/

  const smem_legacy_buf_data_type* smem_buf_data_tbl;
} smem_legacy_info_type;

/*=============================================================================

                       LOCAL DATA DEFINATIONS

=============================================================================*/
/*----------------------------------------------------------------------
 * Table describing shared memory layout of the fixed buffers.
 * This table matches the shared memory layout 1.0
 *----------------------------------------------------------------------*/
const smem_legacy_buf_data_type smem_legacy_buf_data_tbl[] =
{
  { SMEM_FIRST_FIXED_BUFFER,
    SMEM_MEM_FIRST_SIZE_BYTES },

  { SMEM_VERSION_INFO,
    SMEM_VERSION_INFO_SIZE * sizeof(uint32) },

  { SMEM_HEAP_INFO,
    sizeof(smem_heap_info_type) },

  { SMEM_ALLOCATION_TABLE,
    SMEM_LEGACY_ALLOCATION_TABLE_SIZE * sizeof(smem_legacy_allocation_table_type) },

  { SMEM_HW_RESET_DETECT,
    sizeof(smem_hw_reset_id_type) },

  { SMEM_RESERVED_AARM_WARM_BOOT,
    sizeof(uint32) },

  { SMEM_DIAG_ERR_MESSAGE,
    SMEM_DIAG_BUFFER_LEN },

  { SMEM_SPINLOCK_ARRAY,
    SPINLOCK_NUM_LOCKS * sizeof(int) },

  { SMEM_MEMORY_BARRIER_LOCATION,
    sizeof(uint32) },

  { SMEM_INVALID,
    0 }
};

/* Initialize the global SMEM Init info stuct 
 */
static smem_legacy_info_type smem_legacy_info =
                           {
                             SMEM_INVALID_ADDR,       /* smem_heap_info */
                             SMEM_INVALID_ADDR,       /* smem_allocation_table */
                             smem_legacy_buf_data_tbl /* smem_buf_data_tbl */
                           };

/*=============================================================================

                       PUBLIC/GLOBAL DATA DEFINATIONS

=============================================================================*/
const smem_funcs_type smem_legacy_funcs = {
#if defined(SMEM_FEATURE_ALLOC_SUPPORT)
  smem_legacy_alloc,
#else /* !SMEM_FEATURE_ALLOC_SUPPORT */
  NULL,
#endif /* SMEM_FEATURE_ALLOC_SUPPORT */

#if defined(SMEM_FEATURE_GET_ADDRS_SUPPORT)
  smem_legacy_get_addr,
#else /* !SMEM_FEATURE_GET_ADDRS_SUPPORT */
  NULL,
#endif /* SMEM_FEATURE_GET_ADDRS_SUPPORT */

#if defined(SMEM_FEATURE_ALLOC_SUPPORT) && defined(SMEM_FEATURE_PARTITION_SUPPORT)
  smem_legacy_alloc_ex,
#else /* !SMEM_FEATURE_ALLOC_SUPPORT || !SMEM_FEATURE_PARTITION_SUPPORT */
  NULL,
#endif /* SMEM_FEATURE_ALLOC_SUPPORT && SMEM_FEATURE_PARTITION_SUPPORT */

#if defined(SMEM_FEATURE_GET_ADDRS_SUPPORT) && defined(SMEM_FEATURE_PARTITION_SUPPORT)
  smem_legacy_get_addr_ex,
#else /* !SMEM_FEATURE_GET_ADDRS_SUPPORT || !SMEM_FEATURE_PARTITION_SUPPORT */
  NULL,
#endif /* SMEM_FEATURE_GET_ADDRS_SUPPORT && SMEM_FEATURE_PARTITION_SUPPORT */
};

/*=============================================================================

                       LOCAL DATA DEFINATIONS

=============================================================================*/


/*=============================================================================

                      LOCAL FUNCTION DEFINATIONS

=============================================================================*/

/*===========================================================================
FUNCTION      smem_legacy_get_offset
                                                                             
DESCRIPTION   Returns the offset to one of the "fixed" memory buffers.  This
              function uses the local smem_buf_data_tbl, and does not touch
              the actual allocation table in SMEM.

ARGUMENTS     tag     Type of memory to get a pointer for.
                  
DEPENDENCIES  None
      
RETURN VALUE  Return offset to the requested buffer, otherwise fatal error.
  
SIDE EFFECTS  None
===========================================================================*/
static uint32 smem_legacy_get_offset(smem_mem_type tag)
{
  uint32 idx;
  uint32 offset;

  /* Search table for tag */
  offset = 0;

  for (idx = 0; (smem_legacy_info.smem_buf_data_tbl[idx].tag != SMEM_INVALID &&
                 smem_legacy_info.smem_buf_data_tbl[idx].tag != tag ); idx++)
  {
    offset += smem_legacy_info.smem_buf_data_tbl[idx].size;
  }

  if (smem_legacy_buf_data_tbl[idx].tag != tag)
  {
    SMEM_OS_LOG_FATAL(1, "Invalid memory type %d", (uint32)tag);
  }

  return offset;
} /* smem_legacy_get_offset */

/*===========================================================================
FUNCTION      smem_legacy_get_static_size

DESCRIPTION   Returns the size of one of the "fixed" memory buffers.  This
              function uses the local smem_buf_data_tbl, and does not touch
              the actual allocation table in SMEM.

ARGUMENTS     tag     The SMEM item whose size to query.

DEPENDENCIES  None

RETURN VALUE  Return offset to the requested buffer, otherwise fatal error.

SIDE EFFECTS  None
===========================================================================*/
static uint32 smem_legacy_get_static_size(smem_mem_type tag)
{
  uint32 buf_tbl_idx;

  /* Must find the index within smem_buf_data_tbl for the correct info. */
  for (buf_tbl_idx = 0; buf_tbl_idx <= SMEM_LAST_FIXED_BUFFER && 
       smem_legacy_info.smem_buf_data_tbl[buf_tbl_idx].tag != tag;
       buf_tbl_idx++)
  {
    /* Loop until we find the structure with the appropriate tag. */
  }
  SMEM_OS_LOG_ASSERT(smem_legacy_info.smem_buf_data_tbl[buf_tbl_idx].tag != SMEM_INVALID);

  return smem_legacy_info.smem_buf_data_tbl[buf_tbl_idx].size;
}

/*===========================================================================
  FUNCTION  smem_legacy_map_generic_pages
===========================================================================*/
/**
  First map the static SMEM items only, then map the entire generic region
  after reading its size from the static heap info structure.

  The generic region (including static items) will be mapped as uncached.
  This function must be called before attempting to access anything in the
  SMEM region whatsoever, including reads and allocations.  Preferably, it
  should be called as soon as we have reserved a virtual address range for
  the SMEM region.

  @return
  None
*/
/*=========================================================================*/
static void smem_legacy_map_generic_pages( void )
{
  uint32 mapping_offset, mapping_size;
  volatile smem_heap_info_type *heap_info;

  /* IMPORTANT: Must map at least as much of static SMEM to cover the heap info
   * structure, before we can dereference any pointer to it! */

  /* 1.) First map the static SMEM items only, since we know they are always 
   * there.  This will include the heap info structure as well as the 
   * allocation table.  The region of static items always starts at offset 0. */
  mapping_size = smem_legacy_get_offset(SMEM_LAST_FIXED_BUFFER) + 
                   smem_legacy_get_static_size(SMEM_LAST_FIXED_BUFFER);
  /* First 4KB legacy/static memory is already mapped, so map remaining */
  mapping_offset = smem_os_map_page_attr(SMEM_LEGACY_SMEM_SIZE, 
                                         mapping_size - SMEM_LEGACY_SMEM_SIZE, 
                                         FALSE);
  mapping_offset += SMEM_LEGACY_SMEM_SIZE;

  /* 2.) Then map all of the generic unprotected SMEM region as uncached.  We 
   * can dereference the heap info structure now. */
  heap_info = smem_legacy_info.smem_heap_info;

  /* The dynamic generic SMEM region always starts after the static items. */
  SMEM_ACCESS_CONFIGURE_SHARED_RAM();
  mapping_size = heap_info->free_offset + heap_info->heap_remaining - 
                   mapping_offset;
  SMEM_ACCESS_CONFIGURE_RESTORE();
  smem_os_map_page_attr(mapping_offset, mapping_size, FALSE);
}

/*===========================================================================
FUNCTION      smem_legacy_alloc_static
                                                                             
DESCRIPTION   Request a pointer to a static buffer in shared memory.  This
              function is designed to be callable at all times, and should
              never ERR_FATAL.

ARGUMENTS     smem_type     Type of memory to get a pointer for.  This 
                            function will only service static SMEM buffers.
              buf_size      Size of the buffer requested.  If the incorrect
                            size is provided, this request will fail.
                  
DEPENDENCIES  SMEM_HW_RESET_DETECT must be the second allocation in SMEM

RETURN VALUE  NULL if an error occurred.  Otherwise, pointer to the 
              requested buffer.
  
SIDE EFFECTS  If not already set, smem_base_addr will be initialized.
===========================================================================*/
static void *smem_legacy_alloc_static
(
  smem_mem_type smem_type,
  uint32        buf_size
)
{
  uint32 tbl_idx;

  /*------------------------------------------------------------
   * On the MODEM Processor, SMEM_HW_RESET_DETECT may be queried 
   * at times when memory is corrupted.  
   * Since we cannot gaurentee that smem_buf_tbl is not also 
   * corrupted, we must calculate the address using preprocessor 
   * resources.
   * Note:  This will not work if the MODEM processor needs to
   *        use virtual addressing for SMEM
   *------------------------------------------------------------*/
  if (smem_type == SMEM_HW_RESET_DETECT)
  {
    if (buf_size != sizeof(smem_hw_reset_id_type))
    {
      return NULL;
    }
    
    return (void *)
      ( SMEM_INFO_SMEM_BASE_ADDR() +
        SMEM_MEM_FIRST_SIZE_BYTES +
        SMEM_VERSION_INFO_SIZE * sizeof(uint32) +
        sizeof(smem_heap_info_type)  +
        SMEM_LEGACY_ALLOCATION_TABLE_SIZE * sizeof(smem_legacy_allocation_table_type));
  }
  
  for (tbl_idx = 0; 
       (smem_legacy_info.smem_buf_data_tbl[tbl_idx].tag != SMEM_INVALID &&
        smem_legacy_info.smem_buf_data_tbl[tbl_idx].tag != smem_type );
       tbl_idx++)
  {
    /* Loop until we find the structure with the appropriate tag. */
  }

  if( smem_legacy_info.smem_buf_data_tbl[tbl_idx].tag != smem_type ||
      smem_legacy_info.smem_buf_data_tbl[tbl_idx].size != buf_size )
  {
    return NULL;
  }

  return (void *)(SMEM_INFO_SMEM_BASE_ADDR() +
                  smem_legacy_get_offset(smem_type));
}/* smem_legacy_alloc_static */

/*===========================================================================
FUNCTION      smem_legacy_alloc_dynamic
                                                                             
DESCRIPTION   Request a pointer to a dynamic item buffer in shared memory. 
              This function returns the virtual address of the allocated 
              memory item. 
              This function does not handle statically allocated item types.

ARGUMENTS     smem_type     Type of memory to get a pointer for.  This 
                            function will not service static item types.
              buf_size      Size of the buffer requested.  If the incorrect
                            size is provided, this request will fail.
                  
RETURN VALUE  NULL if an error occurred.  Otherwise, pointer to the 
              requested buffer.
  
SIDE EFFECTS  If not already set, smem_base_addr will be initialized.
===========================================================================*/
static void *smem_legacy_alloc_dynamic
(
  smem_mem_type smem_type,
  uint32        buf_size
)
{
  uint32  remaining;
  uint32  offset;
  void   *result;

  /* We do not support allocation of static smem types, these item types
   * should have been dealt with by the caller of this function */
  SMEM_OS_LOG_ASSERT(smem_type > SMEM_LAST_FIXED_BUFFER); 

  /* Do not service invalid types */
  /* Convert both values to uint32 to resolve compiler warnings and
   * to do the right thing no matter what type the compiler makes smem_type */
  if ((uint32) smem_type > (uint32) SMEM_MEM_LAST)
  {
    return NULL;
  }

  /* buffer size topped off to a 64 bit alignment */
  buf_size = SMEM_ALIGN_SZ(buf_size);

  smem_spin_lock(SMEM_SPINLOCK_SMEM_ALLOC);
  SMEM_ACCESS_CONFIGURE_SHARED_RAM();

  if (smem_legacy_info.smem_allocation_table[smem_type].allocated == 
      SMEM_ALLOCATION_ENTRY_ALLOCATED)
  {
    /** If memory has already been allocated, we ignore the mem_bank param
     * and calculate the allocated item's address based on what is in the
     * smem_allocation_tbl */

    /* Check for valid buf_size */
    if (buf_size == smem_legacy_info.smem_allocation_table[smem_type].size)
    {
      /* If Memory allocated from external memory region and not SMEM */
      if (smem_legacy_info.smem_allocation_table[smem_type].base_ext & 
           (~SMEM_ALLOC_TBL_RESERVED_MASK)) 
      {
        /* TODO: Not supported the ext memory */
        result = NULL;
      }
      else
      {
        result = SMEM_INFO_SMEM_BASE_ADDR() + 
                 smem_legacy_info.smem_allocation_table[smem_type].offset;
      }
    }
    else
    {
      result = NULL;
    }
  }
  else
  {
    /* Item not allocated, do it here */
    remaining = smem_legacy_info.smem_heap_info->heap_remaining;

    if (buf_size <= remaining)
    {
      offset = smem_legacy_info.smem_heap_info->free_offset;

      /* Update heap info */
      smem_legacy_info.smem_heap_info->free_offset    = offset + buf_size;
      smem_legacy_info.smem_heap_info->heap_remaining = remaining - buf_size;

      /* Update allocation table entry */
      smem_legacy_info.smem_allocation_table[smem_type].offset    = (uint32)offset;
      smem_legacy_info.smem_allocation_table[smem_type].size      = buf_size;
      smem_legacy_info.smem_allocation_table[smem_type].allocated = 
                                              SMEM_ALLOCATION_ENTRY_ALLOCATED;

      result = SMEM_INFO_SMEM_BASE_ADDR() + offset;
    }      
    else
    {
      result = NULL;
    }

    smem_os_mem_barrier();
  }

  SMEM_ACCESS_CONFIGURE_RESTORE();
  smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);

  return result;
} /* smem_alloc_dynamic */

/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

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
#if defined(SMEM_FEATURE_ALLOC_SUPPORT)
void *smem_legacy_alloc
(
  smem_mem_type smem_type,
  uint32        buf_size
)
{
  /* Hand static types to smem_alloc_static */
  /* Convert both values to uint32 to resolve compiler warnings and
   * to do the right thing no matter what type the compiler makes smem_type */
  if (smem_type <= SMEM_LAST_FIXED_BUFFER)
  {
    return smem_legacy_alloc_static(smem_type, buf_size);
  }

  /* Call the internal alloc API for dynamic smem items */
  return smem_legacy_alloc_dynamic(smem_type, buf_size);
} /* smem_legacy_alloc */
#endif /* SMEM_FEATURE_ALLOC_SUPPORT */

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
#ifdef SMEM_FEATURE_GET_ADDRS_SUPPORT
void *smem_legacy_get_addr
(
  smem_mem_type  smem_type,
  uint32        *buf_size
)
{
  void  *result;
  
  /* Hand static (fixed) smem types to smem_legacy_alloc_static. */
  if (smem_type <= SMEM_LAST_FIXED_BUFFER)
  {
    *buf_size = smem_legacy_get_static_size(smem_type);
    return smem_legacy_alloc_static(smem_type, *buf_size);
  }

  /* Do not service invalid types */
  /* Convert both values to uint32 to resolve compiler warnings and
   * to do the right thing no matter what type the compiler makes smem_type */
  if (smem_type > SMEM_MEM_LAST)
  {
    SMEM_OS_LOG_FATAL(1, "Invalid SMEM memory type, %d, requested.", smem_type);
  }

  /* Even though this is read only operation, use spinlock to ensure that
     the state of the table is consistent */
  smem_spin_lock(SMEM_SPINLOCK_SMEM_ALLOC);
  SMEM_ACCESS_CONFIGURE_SHARED_RAM();

  if (smem_legacy_info.smem_allocation_table[smem_type].allocated ==
      SMEM_ALLOCATION_ENTRY_ALLOCATED)
  {
    *buf_size = smem_legacy_info.smem_allocation_table[smem_type].size;
    /* If Memory allocated from external memory region and not SMEM */
    if (smem_legacy_info.smem_allocation_table[smem_type].base_ext & 
         (~SMEM_ALLOC_TBL_RESERVED_MASK)) 
    {
      *buf_size = 0;
      /* TODO: Not supported */
      result = NULL;
    }
    else
    {
      result = SMEM_INFO_SMEM_BASE_ADDR() + 
               smem_legacy_info.smem_allocation_table[smem_type].offset;
    }
  }
  else
  {
    /* object not allocated yet */
    *buf_size = 0;
    result = NULL;
  }

  /* Make sure the shared memory item fits in shared memory */
  if (result != NULL && 
      (smem_legacy_info.smem_allocation_table[smem_type].base_ext == 0))
  {
    uint8 *smem_data_end_addr = SMEM_INFO_SMEM_BASE_ADDR() +
                               SMEM_INFO_SMEM_SIZE();
    if (((uint8 *)result < SMEM_INFO_SMEM_BASE_ADDR()) ||
        ((uint8 *)result >= smem_data_end_addr) ||
        (((uint8 *)result + *buf_size) >= smem_data_end_addr))
    {
      /* The shared memory heap is corrupt, or this shared memory item was
       * allocated by a processor with a different shared memory size. */
      *buf_size = 0;
      result = NULL;
    }
  }
  
  SMEM_ACCESS_CONFIGURE_RESTORE();
  smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);

  return result;
} /* smem_legacy_get_addr */
#endif /* SMEM_FEATURE_GET_ADDRS_SUPPORT */
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
#if defined(SMEM_FEATURE_ALLOC_SUPPORT) && defined(SMEM_FEATURE_PARTITION_SUPPORT)
int32 smem_legacy_alloc_ex
(
  smem_alloc_params_type *params
)
{
  int32 status;

  if (params->remote_host != SMEM_INVALID_HOST &&
      params->remote_host >= SMEM_NUM_HOSTS)
  {
    return SMEM_STATUS_INVALID_PARAM;
  }

  /* If remote host is invalid or local host, call legacy routine.
   * Caller can use SMEM_INVALID_HOST to allocate unprotected SMEM.
   * Also, if there is no partition for this edge, call legacy routine */
  if ((params->remote_host == SMEM_INVALID_HOST) ||
      (params->remote_host == SMEM_THIS_HOST))
  {
    uint32 size_rounded8 = ROUND_UP(params->size, 8);
    void *ret = smem_legacy_alloc(params->smem_type, size_rounded8);
    if (ret)
    {
      params->buffer = ret;
      params->size = size_rounded8;
      status = SMEM_STATUS_SUCCESS;
    }
    else
    {
      status = SMEM_STATUS_FAILURE;
    }
  }
  else
  {
    status = smem_part_alloc_ex(params);
  }
  
  return status;
}
#endif /* SMEM_FEATURE_ALLOC_SUPPORT && SMEM_FEATURE_PARTITION_SUPPORT */

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
#if defined(SMEM_FEATURE_GET_ADDRS_SUPPORT) && defined(SMEM_FEATURE_PARTITION_SUPPORT)
int32 smem_legacy_get_addr_ex
(
  smem_alloc_params_type *params
)
{
  int32 status;

  /* Check smem_type field */
  if (params->smem_type >= SMEM_INVALID)
  {
    return SMEM_STATUS_INVALID_PARAM;
  }
  
  if (params->remote_host != SMEM_INVALID_HOST &&
      params->remote_host >= SMEM_NUM_HOSTS)
  {
    return SMEM_STATUS_INVALID_PARAM;
  }

  /* Check remote_host field */
  /* If remote host is invalid or local host, call legacy routine. */
  if ((params->remote_host == SMEM_INVALID_HOST) ||
      (params->remote_host == SMEM_THIS_HOST))
  {
    /* save callers size since smem_get_addr will set it to 0
     * if not found. */
    uint32 local_size;
    void *ret = smem_get_addr(params->smem_type, &local_size);
    if (ret)
    {
      params->buffer = ret;
      params->size   = local_size;
      status = SMEM_STATUS_SUCCESS;
    }
    else
    {
      status = SMEM_STATUS_FAILURE;
    }
  }
  else
  {
    status = smem_part_get_addr_ex(params);
  }
  
  return status;
}
#endif /* SMEM_FEATURE_GET_ADDRS_SUPPORT && SMEM_FEATURE_PARTITION_SUPPORT */

/*===========================================================================
  FUNCTION  smem_alloc_add_table_entry
===========================================================================*/
/**
  Add an entry to the SMEM allocation table without allocating any memory and 
  without updating the heap info structure.

  Caller must ensure that there is not currently an allocation at this location.

  This function is designed for use with the backwards compatibility mode 
  used during the roll out of SMEM partitioning.

  @param[in]    smem_type   SMEM ID to use for the allocation
  @param[in]    buffer      pointer to the existing allocation
  @param[in]    size        size of the existing allocation

  @return
  None.

  @dependencies
  SMEM must be initialized before calling.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_alloc_add_table_entry
( 
  smem_mem_type smem_type, 
  void * buffer, 
  uint32 size 
)
{
  uint32 offset = (uint32)buffer - (uint32)SMEM_INFO_SMEM_BASE_ADDR();
  
  if (smem_legacy_info.smem_allocation_table != SMEM_INVALID_ADDR)
  {
    if (smem_type >= SMEM_MEM_LAST)
    {
      SMEM_OS_LOG_FATAL(1, "Invalid smem_type=%d", smem_type);
      return;
    }
    /* Enter critical section */
    smem_spin_lock(SMEM_SPINLOCK_SMEM_ALLOC);

    /* Update the allocation table */
    smem_legacy_info.smem_allocation_table[smem_type].offset    = offset;
    smem_legacy_info.smem_allocation_table[smem_type].size      = size;
    smem_legacy_info.smem_allocation_table[smem_type].allocated = 
                                                SMEM_ALLOCATION_ENTRY_ALLOCATED;

    /* Modify the heap info.  LA uses this field to detect any SMEM changes
     * and if unmodified, LA will not scan for new SMD ports. */
    smem_legacy_info.smem_heap_info->free_offset    += 8;
    smem_legacy_info.smem_heap_info->heap_remaining -= 8;

    smem_os_mem_barrier();

    /* End critical section */
    smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
  }
  else
  {
    SMEM_OS_LOG_FATAL(0, "In new SMEM version smem_alloc_add_table_entry() is not supported");
  }
}

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
SMEM_OS_INIT_FUNC_RECLAIM_SECTION
void smem_legacy_init(void)
{
  /* The heap info should have already been intialized
   *
   * The heap info offset is already memory mapped before 
   * calling into legacy init */
  smem_legacy_info.smem_heap_info = 
        (smem_heap_info_type *)(SMEM_INFO_SMEM_BASE_ADDR() + 
                                smem_legacy_get_offset(SMEM_HEAP_INFO));
  SMEM_ACCESS_CONFIGURE_SHARED_RAM();
  if (smem_legacy_info.smem_heap_info->initialized != SMEM_HEAP_INFO_INIT)
  {
    SMEM_ACCESS_CONFIGURE_RESTORE();
    SMEM_OS_LOG_FATAL(0, "Can't initialize heap info");
  }
  SMEM_ACCESS_CONFIGURE_RESTORE();
  
  /* Memory map the static SMEM items and entire generic region */
  smem_legacy_map_generic_pages();

  /* Validate that the allocation table is large enough and 
   * intialize allocation table pointer */
  if (SMEM_LEGACY_ALLOCATION_TABLE_SIZE < SMEM_MEM_LAST)
  {
    SMEM_OS_LOG_FATAL(0, "The smem allocation table is too small");
  }

  smem_legacy_info.smem_allocation_table = (smem_legacy_allocation_table_type *)
                            (SMEM_INFO_SMEM_BASE_ADDR() + 
                            smem_legacy_get_offset(SMEM_ALLOCATION_TABLE));
}

