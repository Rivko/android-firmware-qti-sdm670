/*=============================================================================

                            SMEM BOOT

 Copyright (c)  2011-2013, 2016-2017 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/src/smem_boot.c#1 $

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
#include "smsm_type.h"
#include "smem_boot.h"
#include "smem_boot_targ.h"
#include "smem_toc.h"
#include "spinlock.h"
#include "smem_partition.h"
#include "smem_internal.h"

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/


/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/
/* SMEM boot info */
typedef struct
{
  uint32 smem_version;
  uint8  *smem_phys_base_addr;
  uint8  *smem_base_addr;
  uint32 smem_size;
}smem_boot_info_type;

/*=============================================================================

                       PUBLIC/GLOBAL DATA DEFINATIONS

=============================================================================*/


/*=============================================================================

                       LOCAL DATA DEFINATIONS

=============================================================================*/
smem_boot_info_type smem_boot_info;

/*----------------------------------------------------------------------
 * Table describing shared memory layout of the fixed buffers.
 * This table matches the shared memory layout 1.0
 *----------------------------------------------------------------------*/
#ifdef SMEM_FEATURE_LEGACY_VERSION_SUPPORT
extern const smem_legacy_buf_data_type smem_legacy_buf_data_tbl[];
#endif /* SMEM_FEATURE_LEGACY_VERSION_SUPPORT */

/*=============================================================================

                      LOCAL FUNCTION DEFINATIONS

=============================================================================*/

/*===========================================================================
  FUNCTION  smem_boot_toc_setup
===========================================================================*/
/**
  Sets up the SMEM Table of Contents describing each protected partition, and
  fills out the headers at the beginning of each partition.
 
  @return
  None.
*/
/*=========================================================================*/
static void smem_boot_toc_entry_setup
(
  smem_toc_entry_type *toc_entry,
  uint32              offset,
  uint32              size,
  uint32              flags,
  uint16              host0,
  uint16              host1,
  uint32              size_cacheline,
  const uint32        exclusion_sizes[SMEM_TOC_MAX_EXCLUSIONS],
  boolean             set_partn_header
)
{
  smem_partition_header_type *partn_header;
  uint32                     idx;
  uint32                     total_excl_size;

  toc_entry->offset         = offset;
  toc_entry->size           = size;
  toc_entry->flags          = flags;
  toc_entry->host0          = host0;
  toc_entry->host1          = host1;
  toc_entry->size_cacheline = size_cacheline;
  
  total_excl_size = 0;
  if (exclusion_sizes != NULL)
  {
    for (idx = 0; idx < SMEM_TOC_MAX_EXCLUSIONS; idx++)
    {
      toc_entry->exclusion_sizes[idx] = exclusion_sizes[idx];
      total_excl_size += exclusion_sizes[idx];
    }
  }

  if ((toc_entry->size != 0) && set_partn_header)
  {
    /* Set up the header struct at the beginning of this partition. */
    partn_header = (smem_partition_header_type *)
                      (smem_boot_info.smem_base_addr + toc_entry->offset);
    partn_header->identifier = SMEM_PARTITION_HEADER_ID;
    partn_header->host0      = host0;
    partn_header->host1      = host1;

    /* Size does not count the exclusion ranges. */
    partn_header->size = size - total_excl_size;
    /* Offsets in this partition are from 0 to the allocatable size. */
    /* Upward free offset starts after the end of this partition header. */
    partn_header->offset_free_uncached = sizeof(smem_partition_header_type);
    /* Downward free offset is the last free byte, also excluding the exclusion
     * regions. */
    partn_header->offset_free_cached = size;
  }
}
  
/*===========================================================================
  FUNCTION  smem_boot_toc_setup
===========================================================================*/
/**
  Sets up the SMEM Table of Contents describing each protected partition, and
  fills out the headers at the beginning of each partition.
 
  @return
  None.
*/
/*=========================================================================*/
static void smem_boot_toc_setup(void)
{
  const smem_toc_config_type *smem_toc_config_tbl, *partn_config_entry;
  smem_toc_type              *toc_header;
  
  uint32 toc_version, num_partitions, i, entry_idx;
  uint32 total_partn_size, heap_remaining, free_offset;
  uint32 additional_partn;

  toc_version = smem_boot_targ_toc_config_get(&smem_toc_config_tbl);
  if (smem_toc_config_tbl == NULL)
  {
    return;
  }
  if (smem_boot_info.smem_version == SMEM_LEGACY_VERSION_ID)
  {
    volatile smem_static_allocs_type *static_allocs = 
              (volatile smem_static_allocs_type *)smem_boot_info.smem_base_addr;
              
    free_offset      = static_allocs->heap_info.free_offset;
    total_partn_size = SMEM_TOC_SIZE;
    heap_remaining   = static_allocs->heap_info.heap_remaining - SMEM_TOC_SIZE;
    
    /* TOC and legacy partitions along with partitions 
     * mentioned in thee TOC config */
    additional_partn = 2;
  }
  else
  {
    /* 4KB TOC at end and 4KB legacy region at starting of SMEM */
    free_offset      = SMEM_LEGACY_SMEM_SIZE;
    total_partn_size = SMEM_LEGACY_SMEM_SIZE + SMEM_TOC_SIZE;
    heap_remaining   = smem_boot_info.smem_size - total_partn_size;
    
    /* TOC, legacy and common partitions along with partitions 
     * mentioned in thee TOC config */
    additional_partn = 3;
  }

  /* Include the entry for the TOC itself, the legacy partition 
   * and/or common partition in the num_partitions count.
   * The last entry has invalid hosts and size 0 to signify the end. */
  for (i = 0, num_partitions = 0;
         (smem_toc_config_tbl[i].host0 != SMEM_INVALID_HOST) &&
         (smem_toc_config_tbl[i].host1 != SMEM_INVALID_HOST); i++)
  {
    /* Calculate number of partitions and total size for sanity. */
    partn_config_entry = &(smem_toc_config_tbl[i]);
    
    if (partn_config_entry->size == 0)
    {
      continue;
    }

    total_partn_size += partn_config_entry->size;
    
    /* Don't add ignore type partitions into TOC */
    if (!(partn_config_entry->flags & SMEM_TOC_ENTRY_FLAGS_IGNORE_PARTITION))
    {
      num_partitions++;
    }
  }

  /* Check total partition size it should greater than SMEM size leaving 
   * 4KB TOC at end and 4 KB legacy region at starting */
  if (heap_remaining < total_partn_size)
  {
    SMEM_OS_LOG_FATAL(2, "smem_setup_toc: Heap remaining (%d) < total partitions sz (%d)!", 
              heap_remaining, total_partn_size);
  }

  /* The TOC table in SMEM must fit in the last 4kB page. */
  if (sizeof(smem_toc_type) + 
      (sizeof(smem_toc_entry_type) * (num_partitions + additional_partn)) > SMEM_TOC_SIZE)
  {
    SMEM_OS_LOG_FATAL(2, "smem_boot_setup_toc: TOC too big! Size: %d Bytes, "
              "must be %d or less.", 
              sizeof(smem_toc_type) + (sizeof(smem_toc_entry_type)*num_partitions), 
              SMEM_TOC_SIZE);
  }

  toc_header = (smem_toc_type *) 
      (smem_boot_info.smem_base_addr + 
       smem_boot_info.smem_size - SMEM_TOC_SIZE);
       
  toc_header->identifier  = SMEM_TOC_IDENTIFIER;
  toc_header->version     = toc_version;
  toc_header->num_entries = num_partitions + additional_partn;
  
  entry_idx = 0;
  
  /* Add the TOC itself as the first partition entry in the SMEM TOC. */
  smem_boot_toc_entry_setup(&toc_header->entry[entry_idx],
                            free_offset + heap_remaining,
                            SMEM_TOC_SIZE,
                            SMEM_TOC_ENTRY_FLAGS_ENABLE_READ_PROTECTION,
                            SMEM_INVALID_HOST,
                            SMEM_INVALID_HOST,
                            0,
                            NULL,
                            FALSE);
                            
  /* Fill in each partition entry in the SMEM TOC, from the smem_toc_config_tbl 
   * obtained from DevConfig. */
  for (i = 0; (smem_toc_config_tbl[i].host0 != SMEM_INVALID_HOST) &&
              (smem_toc_config_tbl[i].host1 != SMEM_INVALID_HOST); i++)
  {
    /* Calculate number of partitions and total size for sanity. */
    partn_config_entry = &(smem_toc_config_tbl[i]);
    
    if (partn_config_entry->size == 0)
    {
      continue;
    }

    /* First reduce the generic SMEM heap by the size of this partition, 
     * then fill out the TOC entry and header for this partition. */
    heap_remaining -= partn_config_entry->size;
    
    /* Don't add ignore type partitions into TOC, just leave the space */
    if (partn_config_entry->flags & SMEM_TOC_ENTRY_FLAGS_IGNORE_PARTITION)
    {
      continue;
    }
    
    smem_boot_toc_entry_setup(&toc_header->entry[++entry_idx],
                              free_offset + heap_remaining,
                              partn_config_entry->size,
                              partn_config_entry->flags,
                              partn_config_entry->host0,
                              partn_config_entry->host1,
                              partn_config_entry->size_cacheline,
                              partn_config_entry->exclusion_sizes,
                              TRUE);
  }

  if (smem_boot_info.smem_version == SMEM_LEGACY_VERSION_ID)
  {
    volatile smem_static_allocs_type *static_allocs = 
        (volatile smem_static_allocs_type *)smem_boot_info.smem_base_addr;
    /* Create legacy partition which is starting at base */
    smem_boot_toc_entry_setup(&toc_header->entry[++entry_idx],
                              0,
                              heap_remaining + free_offset,
                              SMEM_TOC_ENTRY_FLAGS_ENABLE_RW_PROTECTION,
                              SMEM_INVALID_HOST,
                              SMEM_INVALID_HOST,
                              0,
                              NULL,
                              FALSE);
    /* Update the remaining SMEM as heap */
    static_allocs->heap_info.free_offset = free_offset;
    static_allocs->heap_info.heap_remaining = heap_remaining;
  }
  else
  {
    /* Remaining SMEM is common partition which can accessible by all hosts */
    smem_boot_toc_entry_setup(&toc_header->entry[++entry_idx],
                              SMEM_LEGACY_SMEM_SIZE,
                              heap_remaining,
                              SMEM_TOC_ENTRY_FLAGS_ENABLE_RW_PROTECTION,
                              SMEM_COMMON_HOST,
                              SMEM_COMMON_HOST,
                              32, /* setting default cache size */
                              NULL,
                              TRUE);
    /* Create legacy partition which is starting 4KB */
    smem_boot_toc_entry_setup(&toc_header->entry[++entry_idx],
                              0,
                              SMEM_LEGACY_SMEM_SIZE,
                              SMEM_TOC_ENTRY_FLAGS_ENABLE_RW_PROTECTION,
                              SMEM_INVALID_HOST,
                              SMEM_INVALID_HOST,
                              0,
                              NULL,
                              FALSE);
  }
} /* smem_boot_toc_setup */

/*=============================================================================
  FUNCTION  smem_boot_version_set()
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
static void smem_boot_version_set(void)
{
  volatile smem_static_allocs_type *static_allocs = 
            (volatile smem_static_allocs_type *)smem_boot_info.smem_base_addr;

  static_allocs->version[SMEM_VERSION_BOOT_OFFSET] = smem_boot_info.smem_version;
}

/*===========================================================================
  FUNCTION  smem_boot_legacy_alloc_table_init
===========================================================================*/
/**
  Initializes the heap info structure.

  @return
  None.\
*/
/*=========================================================================*/
#ifdef SMEM_FEATURE_LEGACY_VERSION_SUPPORT 
static void smem_boot_legacy_alloc_table_init(void)
{
  volatile smem_legacy_allocation_table_type *alloc_table;
  uint32 offset;
  uint32 idx;
  uint32 tbl_idx;
  
  
  /* In Legacy SMEM allocation table starts after new version static allocation */
  alloc_table = (volatile smem_legacy_allocation_table_type *)
                 (smem_boot_info.smem_base_addr + sizeof(smem_static_allocs_type));

  /*------------------------------------------------------------
   * Setup Allocation table for "fixed" structures. Since everyone
   * is compatible. We can just write the values in shared memory
   * without spin locking. The values will be written once for each
   * call to smem_init, but everyone is writing the same values.
   *------------------------------------------------------------*/
  offset = 0;

  for(idx = 0; smem_legacy_buf_data_tbl[idx].tag != SMEM_INVALID; idx++)
  {
    tbl_idx = (uint32) smem_legacy_buf_data_tbl[idx].tag;
    alloc_table[tbl_idx].offset = offset;
    alloc_table[tbl_idx].size   = smem_legacy_buf_data_tbl[idx].size;
    alloc_table[tbl_idx].allocated = SMEM_ALLOCATION_ENTRY_ALLOCATED;

    offset += smem_legacy_buf_data_tbl[idx].size;
  }
}
#endif /* SMEM_FEATURE_LEGACY_VERSION_SUPPORT */

/*===========================================================================
  FUNCTION  smem_boot_heap_info_init
===========================================================================*/
/**
  Initializes the heap info structure.

  @return
  None.\
*/
/*=========================================================================*/
static void smem_boot_heap_info_init(void)
{
  volatile smem_static_allocs_type *static_allocs = 
            (volatile smem_static_allocs_type *)smem_boot_info.smem_base_addr;

  if (smem_boot_info.smem_version == SMEM_LEGACY_VERSION_ID)
  {
#ifdef SMEM_FEATURE_LEGACY_VERSION_SUPPORT 
    volatile smem_legacy_allocation_table_type *alloc_table;
    uint32 offset;
  
    /* In Legacy SMEM allocation table starts after new version static allocation */
    alloc_table = (volatile smem_legacy_allocation_table_type *)
                   (smem_boot_info.smem_base_addr + sizeof(smem_static_allocs_type));
    
    offset = (alloc_table[SMEM_LAST_FIXED_BUFFER].offset + 
              alloc_table[SMEM_LAST_FIXED_BUFFER].size);

    /* TODO - do we need any special alignment here? */
    static_allocs->heap_info.free_offset    = offset;
    static_allocs->heap_info.heap_remaining = 
                                    smem_boot_targ_size_get() - offset;
    static_allocs->heap_info.initialized    = SMEM_HEAP_INFO_INIT;
#endif /* SMEM_FEATURE_LEGACY_VERSION_SUPPORT */
  }
  else
  {
    /* No more free memory in legacy region */
    static_allocs->heap_info.free_offset    = SMEM_LEGACY_SMEM_SIZE;
    static_allocs->heap_info.heap_remaining = 0;
    static_allocs->heap_info.initialized    = SMEM_HEAP_INFO_INIT;
  }

} /* smem_boot_heap_info_init */

/*=============================================================================

                     PUBLIC FUNCTION DEFINATION

=============================================================================*/

/*=============================================================================
  FUNCTION  <function>
=============================================================================*/
/**
 * @param[in]
 *
 * @return
 *
 * @sideeffects
 */
/*===========================================================================*/
void smem_boot_init(void)
{
  /* Initialize the target specific functionality */
  smem_boot_targ_init();
  
  /* Get the version number that need to support */
  smem_boot_info.smem_version = smem_boot_targ_version_get();
  smem_boot_info.smem_version &= SMEM_MAJOR_VERSION_MASK;
  
#ifdef SMEM_FEATURE_LEGACY_VERSION_SUPPORT 
  if ((smem_boot_info.smem_version != SMEM_LEGACY_VERSION_ID) &&
      (smem_boot_info.smem_version != (SMEM_VERSION_ID & SMEM_MAJOR_VERSION_MASK)))
#else /* !SMEM_FEATURE_LEGACY_VERSION_SUPPORT */
  if (smem_boot_info.smem_version != (SMEM_VERSION_ID & SMEM_MAJOR_VERSION_MASK))
#endif /* SMEM_FEATURE_LEGACY_VERSION_SUPPORT */
  {
    SMEM_OS_LOG_FATAL(1, "SMEM version=0x%X not supported.", smem_boot_info.smem_version);
    return;
  }

  /* Set the SMEM base physical address and size */
  smem_boot_info.smem_phys_base_addr = smem_boot_targ_base_addr_get();
  smem_boot_info.smem_size           = smem_boot_targ_size_get();
  
  /* Map the SMEM base physical address to virtual address */
  smem_boot_info.smem_base_addr = smem_os_map_base_va(smem_boot_info.smem_phys_base_addr,
                                                      smem_boot_info.smem_size);

  /* zero out the entire smem region */
  memset(smem_boot_info.smem_base_addr, 0, smem_boot_info.smem_size);

#ifdef SMEM_FEATURE_LEGACY_VERSION_SUPPORT 
  /* Legacy allocation table init */
  if (smem_boot_info.smem_version == SMEM_LEGACY_VERSION_ID)
  {
    smem_boot_legacy_alloc_table_init();
  }
#endif  /* SMEM_FEATURE_LEGACY_VERSION_SUPPORT */
  /* Setup heap info */
  smem_boot_heap_info_init();
  
   /* Set up SMEM Partition Table of Contents and Partition headers. */
  smem_boot_toc_setup();

  smem_os_mem_barrier();
  
  /* Save the SMEM target information after the TOC has been filled out.*/
  smem_boot_targ_info_set();
  
  /* Set SMEM boot version */
  smem_boot_version_set();
  
  smem_os_mem_barrier();
  
  /* Do SMEM driver initialization before doing any SMEM allocations */
  smem_init();

}

/*===========================================================================
FUNCTION      smem_boot_debug_init
===========================================================================*/
/**
@brief        Initialize shared memory for crash debugging.
 
              This function may be called at boot to recover enough state from
              shared memory to allow debug to recover state information from
              shared memory that may be used for offline debugging. This
              function will:

              (1) Zero the spinlocks
              (2) Configure the local data structures to allow access to
                  shared memory.

              Debug may call smem_get_addr() to determine the address and size
              of shared memory items. Debug should not write to the shared
              memory items.

              After calling this function, and recovering the relevant
              debugging information, boot must call smem_boot_init() to
              reinitialize shared memory before continuing.

              This function is supported only in the first system bootloader.

@dependencies None

@return       None

@sideeffect   None
*/
/*===========================================================================*/
void smem_boot_debug_init( void )
{
} /* smem_boot_debug_init */
