/*=============================================================================

                            SMEM OS QURT

 Copyright (c)  2016 Qualcomm Technologies, Inc. All Rights Reserved.
 Qualcomm Technologies, Inc. Confidential and Proprietary.

=============================================================================*/

/*=============================================================================

                         EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/smem/inc/smem_os.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/11/16   rv      Initial version.
===========================================================================*/

#ifndef SMEM_OS_H
#define SMEM_OS_H

#ifdef __cplusplus
extern "C" {
#endif

/*=============================================================================

                           INCLUDE FILES

=============================================================================*/
#include "comdef.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stringl.h"
#include "qurt.h"
#include "err.h"
#include "msg.h"
#include "smem_target.h"

/*=============================================================================

                         MACRO DEFINITIONS

=============================================================================*/

/* Use DAL is APIs they are supported.
 * Comment below line if DAL SYS is not supported */
#define SMEM_OS_DAL_SYS_SUPPORTED

#if defined(SMEM_OS_DAL_SYS_SUPPORTED)
#include "DALSys.h"
#endif /* SMEM_OS_DAL_SYS_SUPPORTED */


/* msg.h defines MSG_0() as MSG() */
#ifndef MSG_0
#define MSG_0 MSG
#endif

/* Allow var arg style calls to MSG */
#define _MSGPASTE(a,b) _MSGPASTE_(a,b)
#define _MSGPASTE_(a,b) a##b
#define _MSGCNT(...) _MSGCNT_(__VA_ARGS__,12,11,10,9,8,7,6,5,4,3,2,1,0)
#define _MSGCNT_(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,cnt,...) cnt
#define MSGVA(level, fmt, ...) \
  _MSGPASTE(MSG_,_MSGCNT(MSG_SSID_DFLT,level,fmt,##__VA_ARGS__))\
  (MSG_SSID_DFLT,level,fmt,## __VA_ARGS__)

#define SMEM_OS_LOG_FATAL(args_cnt, fmt, ...) \
  MSGVA(MSG_LEGACY_FATAL, fmt, ##__VA_ARGS__)
  
#define SMEM_OS_LOG_ERR(args_cnt, fmt, ...)   \
  MSGVA(MSG_LEGACY_ERROR, fmt, ##__VA_ARGS__)
  
#define SMEM_OS_LOG_INFO(args_cnt, fmt, ...)  \
  MSGVA(MSG_LEGACY_MED, fmt, ##__VA_ARGS__)
  
#define SMEM_OS_LOG_ASSERT(cond) \
  do { \
    if (!(cond)) \
    { \
      SMEM_OS_LOG_FATAL(0, "Assertion \"" #cond "\" failed"); \
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
#if defined(SMEM_OS_DAL_SYS_SUPPORTED)
/* The critical section structure for the intlock */
extern DALSYSSyncHandle smem_os_intlock_handle;
#endif /* SMEM_OS_DAL_SYS_SUPPORTED */

/*=============================================================================

                    PUBLIC FUNCTION DECLARATIONS

=============================================================================*/

/*=============================================================================
  FUNCTION  smem_os_init
=============================================================================*/
/**
  This function does the OS specific initialization.

  @return
  None.
*/
/*===========================================================================*/
void smem_os_init(void);

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
#if defined(SMEM_OS_DAL_SYS_SUPPORTED)
static inline void smem_os_intlock(void)
{
  /* Lock the non-preemtable critical section */
  DALSYS_SyncEnter(smem_os_intlock_handle);
}
#else /* !SMEM_OS_DAL_SYS_SUPPORTED */
void smem_os_intlock(void);
#endif /* SMEM_OS_DAL_SYS_SUPPORTED */

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
#if defined(SMEM_OS_DAL_SYS_SUPPORTED)
static inline void smem_os_intfree(void)
{
  /* Unlock the non-preemptable critical section */
  DALSYS_SyncLeave(smem_os_intlock_handle);
}
#else /* !SMEM_OS_DAL_SYS_SUPPORTED */
void smem_os_intfree(void);
#endif /* SMEM_OS_DAL_SYS_SUPPORTED */

/*=============================================================================
  FUNCTION  smem_os_mem_copy
=============================================================================*/
/**
  Copies bytes from the source buffer to the destination buffer.

  This function ensures that there will not be a copy beyond the size of 
  the destination buffer.

  The result of calling this on overlapping source and destination
  buffers is undefined.
  
  @param[out] dst       Destination buffer.
  @param[in]  dst_size  Size of the destination buffer in bytes.
  @param[in]  src       Source buffer.
  @param[in]  copy_size Number of bytes to copy from source buffer.

  @return
  The number of bytes copied to the destination buffer.  It is the
  caller's responsibility to check for trunction if it cares about it -
  truncation has occurred if the return value is less than src_size.
*/
/*===========================================================================*/
static inline uint32 smem_os_mem_copy
(
  void       *dst, 
  uint32     dst_size, 
  const void *src, 
  uint32     copy_size
)
{
  return memscpy(dst, dst_size, src, copy_size);
}

/*=============================================================================
  FUNCTION  smem_os_mem_move
=============================================================================*/
/**
  Moves bytes from the source buffer to the destination buffer.

  This function ensures that there will not be a copy beyond 
  the size of the destination buffer. 

  This function should be used in preference to memscpy() if there
  is the possiblity of source and destination buffers overlapping.
  The result of the operation is defined to be as if the copy were from
  the source to a temporary buffer that overlaps neither source nor
  destination, followed by a copy from that temporary buffer to the
  destination.
   
  @param[out] dst       Destination buffer.
  @param[in]  dst_size  Size of the destination buffer in bytes.
  @param[in]  src       Source buffer.
  @param[in]  copy_size Number of bytes to copy from source buffer.
   
  @return 
  The number of bytes copied to the destination buffer.  It is the
  caller's responsibility to check for trunction if it cares about it -
  truncation has occurred if the return value is less than src_size.
*/
/*===========================================================================*/
static inline uint32 smem_os_mem_move
(
  void       *dst, 
  uint32     dst_size, 
  const void *src, 
  uint32     copy_size
)
{
  return memsmove(dst, dst_size, src, copy_size);
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
void *smem_os_map_hwio_region(const char *hwio_region_name);

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
void *smem_os_map_memory_va(void* phys_addr, uint32 size);

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
void *smem_os_map_base_va(void *smem_phys_addr, uint32 size);

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
uint32 smem_os_map_page_attr
(
  uint32  page_offset, 
  uint32  page_size, 
  boolean cached
);

/*=============================================================================
  FUNCTION  smem_os_mem_barrier
=============================================================================*/
/**
  Calls OS specific memory barrier.

  @return

  @sideeffects
 */
/*===========================================================================*/
static inline void smem_os_mem_barrier(void)
{
  qurt_mem_barrier();
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
uint32 smem_os_timetick_get(void);

#ifdef __cplusplus
}
#endif

#endif /* SMEM_OS_H */
