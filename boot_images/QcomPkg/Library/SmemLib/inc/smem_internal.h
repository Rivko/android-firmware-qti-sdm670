/*=============================================================================

                          SMEM INTERNAL

 Copyright (c)  2011-2013, 2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/inc/smem_internal.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/16   rv      SMEM Version 0x000C0000 changes.
10/09/13   bt      Add base physical address to smem_init_info_type.
06/25/13   pa      Add smem_get_base_addr and smem_get_size.
                   Also, add smem_data_size to smem_init_info_type.
05/30/12   bm      Add heap_size field in smem_ext_heap_info_type
01/19/12   bm      Add fatures to support SMEM allocation from ouside of
                   SMEM. Feature added for RPM clients.
01/05/12   hwu     Moved the host id defines to smem_type.h.
12/13/11   rs      Removed DALSYS and DALHWIO dependencies: adding Spinlocks
                   Region base address initialization
07/25/11   rs      Added type to define SMEM hosts
05/31/11   bm      Initial Version - type/function declarations to be used
                   between smem.c and smem_boot.c
===========================================================================*/

#ifndef SMEM_INTERNAL_H
#define SMEM_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "smem_os.h"
#include "smem.h"
#include "smem_version.h"

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/
/*----------------------------------------------------------------------
 * Heap info structure - keeps track of which part of heap has been
 * allocated. The heap management scheme is trivial - we just allocate
 * from the end of the heap, we never free anything.
 *----------------------------------------------------------------------*/
#define SMEM_HEAP_INFO_INIT                    1
#define SMEM_ALLOCATION_ENTRY_ALLOCATED        1

/* This macro rounds an address up to be a multiple of 8 */
#define SMEM_ALIGN_SZ(VALUE) (((VALUE) + 7) & ~0x00000007)

/* smem_info helper macros */
#define SMEM_INFO_SMEM_PHYS_BASE_ADDR()  (smem_info.smem_phys_base_addr)
#define SMEM_INFO_SMEM_BASE_ADDR()       (smem_info.smem_base_addr)
#define SMEM_INFO_SMEM_SIZE()            (smem_info.smem_size)
#define SMEM_INFO_SMEM_MAX_ITEMS()       (smem_info.max_items)

/* SHOULD BE MULTIPLES OF PAGE SIZE */
#define SMEM_LEGACY_SMEM_SIZE   (4 * 1024)

#define SMEM_LEGACY_ALLOCATION_TABLE_SIZE    512

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/
/*----------------------------------------------------------------------
 * Shared memory initialization state - tracks what initialization
 * has been done. 
 *----------------------------------------------------------------------*/
typedef enum
{
  SMEM_STATE_UNINITIALIZED,    /**< shared memory original state */ 
  SMEM_STATE_PRE_BOOT_INIT,    /**< shared memory is not zero-initialized yet */ 
  SMEM_STATE_BOOT_INITIALIZED, /**< shared memory is zero-ed out and initialized */ 
  SMEM_STATE_INITIALIZED       /**< shared memory is initialized */
}smem_state_type;

/*----------------------------------------------------------------------
 * Allocation and get address function pointers. 
 * Based on the SMEM version these function pointers gets updated and used.
 *----------------------------------------------------------------------*/
typedef struct {
  void* (*alloc      )(smem_mem_type smem_type, uint32 buf_size);
  void* (*get_addr   )(smem_mem_type smem_type, uint32 *buf_size);
  int32 (*alloc_ex   )(smem_alloc_params_type *params);
  int32 (*get_addr_ex)(smem_alloc_params_type *params);
} smem_funcs_type;

/*----------------------------------------------------------------------
 * SMEM legacy heap info
 *----------------------------------------------------------------------*/
typedef struct
{
  uint32 initialized;
  uint32 free_offset;
  uint32 heap_remaining;
  uint32 reserved;
} smem_heap_info_type;

/*----------------------------------------------------------------------
 * Static allocations (SHOULD NOT CHANGES ORDER OF MEMBERS)
 *----------------------------------------------------------------------*/
typedef struct {
  uint32              proc_comm[16];
  uint32              version[SMEM_VERSION_INFO_SIZE];
  smem_heap_info_type heap_info;
  
  /* Remaining static buffers not needed */
} smem_static_allocs_type;

/*----------------------------------------------------------------------
 * Table describing shared memory layout of the fixed buffers.
 * This table matches the shared memory layout 1.0
 *----------------------------------------------------------------------*/
typedef struct
{
  smem_mem_type tag;            /* Tag associated with buffer */
  uint32        size;           /* Size of the buffer */
} smem_legacy_buf_data_type;

/*----------------------------------------------------------------------
 * Allocation table - keeps track of which buffers have been allocated
 * and their sizes. Buffers are only allocated once, subsequent calls
 * to smem_alloc for that buffer return the same address regardless of
 * where smem_alloc is called.
 *----------------------------------------------------------------------*/
typedef struct
{
  uint32 allocated;
  uint32 offset;
  uint32 size;
  uint32 base_ext;
} smem_legacy_allocation_table_type;

/*----------------------------------------------------------------------
 * SMEM info structure.
 *----------------------------------------------------------------------*/
typedef struct {
/*----------------------------------------------------------------------
 * Base virtual address of shared memory - it may be equivalent to the 
 * physical address depending on the environment.
 *----------------------------------------------------------------------*/
  uint8 *smem_base_addr;

/*----------------------------------------------------------------------
 * Base physical address of shared memory - usually read from IMEM.
 * Should not be dereferenced, so it is an address and not a pointer.
 *----------------------------------------------------------------------*/
  uint8 *smem_phys_base_addr;

/*----------------------------------------------------------------------
 * Size of shared memory
 *----------------------------------------------------------------------*/
  uint32 smem_size;

/*----------------------------------------------------------------------
 * Version
 *----------------------------------------------------------------------*/
  uint32 version;
  
/*----------------------------------------------------------------------
 * Max numer of SMEM items
 *----------------------------------------------------------------------*/
  uint16 max_items;
  
/*----------------------------------------------------------------------
 * Allocation and get address function pointers.
 * Based on the SMEM version these function pointers gets updated and used.
 *----------------------------------------------------------------------*/
 const smem_funcs_type *funcs;
  
/*----------------------------------------------------------------------
 * Shared memory initialization state - tracks what initialization
 * has been done. 
 *----------------------------------------------------------------------*/
  smem_state_type state;
} smem_info_type;

/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/
extern smem_info_type smem_info;

/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* SMEM_INTERNAL_H */
