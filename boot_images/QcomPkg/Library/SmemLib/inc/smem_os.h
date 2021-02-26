/*=============================================================================

                            SMEM OS XBL

 Copyright (c)  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/boot.xf/2.1/QcomPkg/Library/SmemLib/inc/smem_os.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/09/17   yps     Replaced Macro __FILE__ with __FILE_BASENAME__
05/24/16   rv      Initial version.
===========================================================================*/

#ifndef SMEM_OS_H
#define SMEM_OS_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "com_dtypes.h"
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include "stdlib.h"
#include "string.h"

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/
/** This feature allows smem_version.h to uniquely identify UEFI to ensure
 * that all SMEM clients are using the same version of the protocol. */
#define FEATURE_SMEM_XBL

#define SMEM_OS_LOG_PRINT_0(mask, fmt, ...) \
  DebugPrint(mask, fmt)

#define SMEM_OS_LOG_PRINT_N(mask, fmt, ...) \
  DebugPrint(mask, fmt, __VA_ARGS__)

#define SMEM_OS_LOG_PRINT_1 SMEM_OS_LOG_PRINT_N
#define SMEM_OS_LOG_PRINT_2 SMEM_OS_LOG_PRINT_N
#define SMEM_OS_LOG_PRINT_3 SMEM_OS_LOG_PRINT_N
#define SMEM_OS_LOG_PRINT_4 SMEM_OS_LOG_PRINT_N
#define SMEM_OS_LOG_PRINT_5 SMEM_OS_LOG_PRINT_N
#define SMEM_OS_LOG_PRINT_6 SMEM_OS_LOG_PRINT_N
#define SMEM_OS_LOG_PRINT_7 SMEM_OS_LOG_PRINT_N
#define SMEM_OS_LOG_PRINT_8 SMEM_OS_LOG_PRINT_N
#define SMEM_OS_LOG_PRINT_9 SMEM_OS_LOG_PRINT_N
  
#define SMEM_OS_LOG_FATAL(cnt, fmt, ...) \
  do { \
    SMEM_OS_LOG_PRINT_##cnt(EFI_D_ERROR, fmt, __VA_ARGS__); \
    DebugAssert(__FILE_BASENAME__, __LINE__, fmt); \
    for(;;) {} \
  } while(0)

#define SMEM_OS_LOG_ERR(cnt, fmt, ...) \
  do { \
    SMEM_OS_LOG_PRINT_##cnt(EFI_D_ERROR, fmt, __VA_ARGS__); \
  } while(0)

#define SMEM_OS_LOG_INFO(cnt, fmt, ...) \
  do { \
    SMEM_OS_LOG_PRINT_##cnt(EFI_D_INFO, fmt, __VA_ARGS__); \
  } while(0)

#define SMEM_OS_LOG_ASSERT(cond)  \
  do { \
    if (!(cond)) \
    { \
      SMEM_OS_LOG_FATAL(0, "Assertion " #cond " failed"); \
    } \
  } while(0)

#define SMEM_OS_INIT_FUNC_RECLAIM_SECTION  


/* Defines invalid address */
#define SMEM_INVALID_ADDR               ((void *)(-1))

/* Mask for smallest possible page mapping size. */
#define SMEM_PAGE_ALIGN_MASK            0x00000FFF

/*=============================================================================

                         TYPE DEFINITIONS

=============================================================================*/

/*=============================================================================

                     PUBLIC VARIABLE DECLARATIONS

=============================================================================*/


/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
  FUNCTION  <function>
=============================================================================*/
/**
  This function does the OS specific initialization.

  @return
  None.
 */
/*===========================================================================*/
static inline void smem_os_init(void)
{
}

/*=============================================================================
  FUNCTION  smem_os_intlock
=============================================================================*/
/**
  Locks the critical section used by the spinlock.

  This function uses the OS-supplied mutual exclusion primitives so that
  multiple tasks that try to acquire the spinlock can be scheduled, elevating
  priority to prevent priority-inversion deadlocks.

  In order to avoid context switches while the spinlock is held, the task's
  priority is elevated on multi-core processors, or preemption is disabled
  entirely on single-core processors. That way the amount of time the remote
  processor spins is reduced to the smallest amount of time possible
  while the spinlock is held.

  @return
  None

  @dependencies
  None.

  @sideeffects
  Prevents context switch.
 */
/*===========================================================================*/
static inline void smem_os_intlock(void)
{

}

/*=============================================================================
  FUNCTION  smem_os_intfree
=============================================================================*/
/**
  Releases the critical section used by the spinlock.

  This function uses the OS-supplied mutual exclusion primitives so that
  multiple tasks that try to release the spinlock can be scheduled, allowing
  context switch once the spinlock is unlocked.

  @return
  None

  @dependencies
  None.

  @sideeffects
  Allows context switch.
*/
/*===========================================================================*/
static inline void smem_os_intfree(void)
{

}

/*=============================================================================
  FUNCTION  smem_os_map_hwio_region
=============================================================================*/
/**
  Creates a virtual address of the HWIO region.

  @param[in]  hwio_region_name     HWIO base name to map. i.e. "CORE_TOP_CSR".

  @return
  Virtual address if successfully mapped, or NULL
  otherwise.

  @dependencies
  None.

  @sideeffects
  None.
*/
/*===========================================================================*/
static inline void *smem_os_map_hwio_region(const char *hwio_region_name)
{
  return (void *)hwio_region_name;
}

/*=============================================================================
  FUNCTION  smem_os_map_memory_va
=============================================================================*/
/**
  Map the memory region and return the virtual address of the mapped memory 
  region.

  @return
  Virtual address of the mapped memory region.

  @param[in]    phys_addr    Memory region base (physical) address.
  @param[in]    size         Size of the memory region.

  @dependencies
  None.

  @sideeffects
  None
*/
/*===========================================================================*/
static inline void *smem_os_map_memory_va(void* phys_addr, uint32 size)
{
  return phys_addr;
}

/*=============================================================================
  FUNCTION  smem_os_map_base_va
=============================================================================*/
/**
  Map the memory region and return the virtual address of the mapped (physical)
  base address of SMEM.

  @param[in]    smem_phys_addr  SMEM region base (physical) address.
  @param[in]    size            Size of the SMEM region.

  @return
  Virtual address of the mapped SMEM region.
*/
/*===========================================================================*/
static inline void *smem_os_map_base_va(void *smem_phys_addr, uint32 size)
{
  return smem_phys_addr;
}

/*=============================================================================
  FUNCTION  smem_os_map_page_attr
=============================================================================*/
/**
  Map the page of SMEM (or a whole address range) at offset page_offset and 
  size of at least page_size.  The cached argument allows the SMEM module to 
  dynamically decide upon each address range's cacheability.
  
  The mapping will be contiguous, from SMEM's base physical address + 
  page_offset to SMEM's base virtual address + page_offset, with "RW"
  permisions.  Use the returned actual page-aligned size to determine where
  the mapping stops and when a new page must be mapped.

  @param[in]    page_offset   The offset from the base address to map (will be 
                              rounded down to a 4kB alignment).
  @param[in]    page_size     Size of this SMEM page or address range (will be
                              rounded up to a 4kB multiple).
  @param[in]    cached        Should this address range be cached memory?

  @return
  The actual page size mapped, rounded up to a 4kB multiple.
*/
/*===========================================================================*/
static inline uint32 smem_os_map_page_attr
(
  uint32  page_offset, 
  uint32  page_size, 
  boolean cached
)
{
  uint32 aligned_offset;

  /* Must still align the page_size in case the remote side is using dynamic 
   * mapping of SMEM pages, which would limit the allocations in both 
   * directions to page boundaries. */

  /* Round offset down to be 4kB aligned.  This really only is for downward
   * allocations; caller must make sure that the page has not been mapped 
   * already. */
  aligned_offset = page_offset & (~SMEM_PAGE_ALIGN_MASK);
  /* Adjust the size accounting for the offset adjustment. */
  page_size += (page_offset - aligned_offset);
  /* Round page size up to a 4kB multiple.  Caller should confirm this 
   * returned value in order to map the next page starting at the correct 
   * aligned offset (whether it is upwards or downwards). */
  page_size = (page_size + SMEM_PAGE_ALIGN_MASK) & (~SMEM_PAGE_ALIGN_MASK);

  /* Don't actually perform any mapping, since all of SMEM is mapped already. */
  return page_size;
}

/*=============================================================================
  FUNCTION  smem_os_mem_barrier
=============================================================================*/
/**
  Calls OS specific memory barrier.

  @return

  @sideeffects
 */
/*===========================================================================*/
void ArmDataMemoryBarrier(void);
static inline void smem_os_mem_barrier(void)
{
  ArmDataMemoryBarrier();
}

/*=============================================================================
  FUNCTION  smem_os_timetick_get
=============================================================================*/
/**
  Returns the current timetick value.
 
  @return
  The current timetick value, measured in ticks.
  0 if an error occurred.
*/
/*===========================================================================*/
static inline uint32 smem_os_timetick_get(void)
{
   return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* SMEM_OS_H */
