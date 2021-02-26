#ifndef _SMEM_V_H_
#define _SMEM_V_H_

/*===========================================================================

                    S H A R E D   M E M O R Y

DESCRIPTION
    Defines the AU public api that is used to allocate memory in physical
    SMEM.  This API deal strictly with void * to reduce dependencies.

Copyright (c) 2009-2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved

===========================================================================*/
/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/20/14   kedara  Include com_dtypes.h instead of comdef.h.
08/27/13   pa      Adding SMEM_ALLOC_FLAG*.
08/16/13   bt      Add smem_get_part_sz for internal clients.
06/25/13   pa      Added smem_alloc_add_table_entry for SMEM backwards 
                   compatibility. 
05/11/12   bm      Added smem_alloc_rpm() API to allocate from MSG RAM.
01/20/12   bm      Add API smem_set_ext_mem_region(), smem_alloc_ext_mem()
                   to support memory allocation from outside SMEM for RPM.
12/13/11   rs      Removed DALSYS and DALHWIO dependencies: adding Spinlocks
                   Region base address initialization
02/17/11   bm      Added API declaration for 'smem_map_base_va'
01/06/11   bt      Added definition for size of first SMEM region.
12/08/10   rs      Added generic SMEM memory barrier
03/02/10   tl      Added debug-smem-at-boot support
11/25/09   rs      CMI updates 
11/08/09   hwu     Remove redundant structure defines. 
04/20/09   bfc     Initial version...Split smem.h into AU public and AU
                   private headers

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
/*===========================================================================

                        TYPE DEFINITIONS

===========================================================================*/
#include "com_dtypes.h"
#include "smem_type.h"

#define SMEM_DIAG_BUFFER_LEN        0xC8

/* This macro rounds an address up to be a multiple of 8 */
#define SMEM_ALIGN_SZ(VALUE) ( ( (VALUE) +7 ) & ~0x00000007 )

/* This is used for offset purposes in the absence of the Prom Comm SMEM 
 * definitions.  This value replaces 
 * SMEM_PROC_COMM_NUM_PROCS * sizeof(smem_proc_comm_data_type) */
#define SMEM_MEM_FIRST_SIZE_BYTES     (4 * 16)

/** Flags for use with smem_alloc_ex()/
 ** For use by Mproc code only. */
#define SMEM_ALLOC_FLAG_CACHED          0x80000000 /**< Allocate from cached 
                                                    **  heap */
#define SMEM_ALLOC_FLAG_PARTITION_ONLY  0x40000000 /**< Allocate from edge 
                                                    **  partition only. */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================
FUNCTION      smem_boot_init

DESCRIPTION   Initialize the smem memory allocation structures and the SMEM
              itself.

ARGUMENTS     None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smem_boot_init( void );

/*===========================================================================
FUNCTION      smem_boot_debug_init

DESCRIPTION   This function may be called at boot to recover enough state from
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

ARGUMENTS     None

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smem_boot_debug_init( void );

/*===========================================================================
  FUNCTION  smem_generic_memory_barrier
===========================================================================*/
/**
  This function implements the generic memory barrier by performing a write
  and read to uncached/unbuffered location in SMEM.
 
  This function can be used when memory barrier is not implemented as a system
  function for a given processor, but it is not guaranteed for any specific
  architecture.

  @return
  None.

  @dependencies
  Variable smem_memory_barrier should be configured as uncached/unbuffered.

  @sideeffects
  None.
*/
/*=========================================================================*/
void smem_generic_memory_barrier( void );

/*===========================================================================
  FUNCTION  smem_map_spinlock_region_va
===========================================================================*/
/**
  Creates a virtual address of the spinlocks region, to enable use of HWIO
  macros.

  @return
  The spinlocks region base virtual address if successfully mapped, or NULL
  otherwise.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*=========================================================================*/
void *smem_map_spinlock_region_va( void );


/*===========================================================================
  FUNCTION  smem_map_memory_va
===========================================================================*/
/**
  Map the memory region and return the virtual address of the mapped memory 
  region.

  @return
  Virtual address of the mapped memory region.

  @param[in]    mem_region   Memory region base address.
  @param[in]    size         Size of the memory region.

  @dependencies
  None.

  @sideeffects
  None
*/
/*=========================================================================*/
void *smem_map_memory_va( void* mem_region, uint32 size );

/*===========================================================================
FUNCTION      smem_alloc_ext_mem
===========================================================================*/
/**
@brief        Request a memory region to be allocated from the external memory
              region provided to smem_set_ext_mem_region().

              For Internal use only (Introduced for RPM clients).

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

@sideeffect  Uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
void* smem_alloc_ext_mem
(
  smem_mem_type         smem_type,
  uint32                buf_size
);

/* The function smem_alloc_rpm is supposed to be used only by the RPM
 * processor to set SMEM drivers to use RPM ram, and to allocate port
 * from that memory region with appropriate XPU settings.
 */
#ifdef IMAGE_RPM_PROC
/*===========================================================================
FUNCTION      smem_alloc_rpm
===========================================================================*/
/**
@brief        Request a pointer to an item in RPM MSG RAM

@param[in]    smem_host_type The other host for the SMEM region
@param[in]    smem_type      Type of memory to get a pointer for. Returns NULL
                             if a static SMEM item is requested.
@param[in]    buf_size       Size of the buffer requested.  For pre-allocated
                             buffers, an ERR_FATAL will occur if the buffer
                             size requested does not match the size of the
                             existing buffer. If the buf_size is not 64-bit 
                             aligned, this function will top the size off to
                             an aligned one.

@dependencies None

@return      return NULL if smem_boot_init is not called yet or the requested
             item is a statically allocated one. Otherwise, either return a 
             valid SMEM address to the requested buffer or error fatal

@sideeffect  None
*/
/*=========================================================================*/
void* smem_alloc_rpm
(
  smem_host_type        other_host,
  smem_mem_type         smem_type,
  uint32                buf_size
);
#endif

/*===========================================================================
FUNCTION      smem_set_ext_mem_region
===========================================================================*/
/**
@brief        Sets the External Memory region from which smem_alloc_ext_mem
              allocates the items.

              For Internal use only (Introduced for RPM clients).

@param[in]    ext_mem_region External memory region base address.
@param[out]   size           Size of the external memory region.

@dependencies smem_init must have been called before this API.

@return       0  Success
             -1  Failure

@sideeffect   This function is intended for internal use only.
*/
/*=========================================================================*/
int32 smem_set_ext_mem_region(void* ext_mem_region, uint32 size);

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
);

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
uint32 smem_get_partition_sz( smem_host_type remote_host );

#endif /* _SMEM_V_H_ */