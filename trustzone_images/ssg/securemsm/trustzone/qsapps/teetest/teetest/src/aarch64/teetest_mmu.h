
/*===========================================================================
Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/

#ifndef __TEETEST_MMU_H
#define __TEETEST_MMU_H

#include <stdint.h>
#include <stddef.h>

#include "tt_ld.h"
#include "tt_tz.h"
#include "mair.h"

#define NUM_L2_TTS            4
#define NUM_L3_TTS            8

#define SIZE_4KB  TZ_4KB
#define SIZE_1MB  TZ_1MB

/*****************************************************************************
 *                             !!! CAUTION !!!                               *
 * Indices must match tt_tz.h or else memory type abstraction will not work. *
 *****************************************************************************/
#define TEETEST64_MAIR_INDEX_0  MAIR_STRONGLY_ORDERED
#define TEETEST64_MAIR_INDEX_1  MAIR_DEVICE
#define TEETEST64_MAIR_INDEX_2  MAIR_NORMAL
#define TEETEST64_MAIR_INDEX_3 (MAIR_OUTER_WT_RWA_NTR    | MAIR_INNER_WT_RWA_NTR)
#define TEETEST64_MAIR_INDEX_4 (MAIR_OUTER_WT_RA_NWA_NTR | MAIR_INNER_WT_RA_NWA_NTR)
#define TEETEST64_MAIR_INDEX_5 (MAIR_OUTER_WB_RWA_NTR    | MAIR_INNER_WB_RWA_NTR)
#define TEETEST64_MAIR_INDEX_6 (MAIR_OUTER_WB_RA_NWA_NTR | MAIR_INNER_WB_RA_NWA_NTR)
#define TEETEST64_MAIR_INDEX_7  MAIR_STRONGLY_ORDERED

#define TEETEST64_MAIR ((TEETEST64_MAIR_INDEX_0)       | (TEETEST64_MAIR_INDEX_1 << 8)  |  \
                        (TEETEST64_MAIR_INDEX_2 << 16) | (TEETEST64_MAIR_INDEX_3 << 24) |  \
                        (TEETEST64_MAIR_INDEX_4 << 32) | (TEETEST64_MAIR_INDEX_5 << 40) |  \
                        (TEETEST64_MAIR_INDEX_6 << 48) | (TEETEST64_MAIR_INDEX_7 << 56))

/*****************************************************************************
 *    TCR_EL1 - Translation Control Register
 *
 *    TCR_EL1 - Translation Control Register
 *
 *    TBI1[38]     = 0b0
 *    TBI0[37]     = 0b0
 *    AS[36]       = 0b0      (8-bit ASID)
 *    PS[34:32]    = 0b001    (36-bits, 64 GB)
 *    TG1[31:30]   = 0b10     (4 KB)
 *    SH1[29:28]   = 0b11     (IS)
 *    ORGN1[27:26] = 0b10     (Normal, Outer WTWA)
 *    IRGN1[25:24] = 0b10     (Normal, Inner WTWA)
 *    EPD1[23]     = 0b1      (TTBR1_EL1 is disable)
 *    A1[22]       = 0b0      (TTBR0_EL1.ASID defines the ASID)
 *    T1SZ[21:16]  = 0b011100 (36 bit address space)
 *    TG0[15:14]   = 0b00     (4 KB)
 *    SH0[13:12]   = 0b11     (IS)
 *    ORGN0[11:10] = 0b10     (Normal, Outer WTWA)
 *    IRGN0[9:8]   = 0b10     (Normal, Inner WTWA)
 *    EPD0[7]      = 0b0      (TTBR0_EL1 is enable)
 *    T0SZ[5:0]    = 0b011100 (36 bit address space)
 *
 *    NOTE: The cache-ability of the inner and outer region in TCR_EL3 should
 *          match the cache-ability of the translation table memory region.
 *****************************************************************************/
#define TEETEST64_TCR    0x1BA9C3A1C


#ifndef _ARM_ASM_


typedef uint32_t tz_cfg_t;
typedef enum
{
  SPINLOCK_TYPE    = 1,
  BAKERY_LOCK_TYPE = 2,
}lock_type_t;


typedef struct
{
  v_addr_t v_addr;
  p_addr_t p_addr;
  size_t   sz_in_kb;
  tz_cfg_t tz_cfg;
} mem_block_t;

typedef int (*tt_lock_func)(void* lock);
typedef int (*tt_release_func)(void* lock);
typedef boolean (*tt_is_locked_func)(void* lock);

typedef struct mmu_ctx_s
{
  boolean      dflt_mem_init_done;

  lock_type_t       lock_type;
  void*             lock;
  tt_lock_func      lock_func;
  tt_release_func   release_func;
  tt_is_locked_func is_locked_func;

  l1_tt_t*     l1_tt_ptr;
  l2_tt_t*     l2_tt_pool_ptr;
  uint32_t     l2_pool_len;
  l3_tt_t*     l3_tt_pool_ptr;
  uint32_t     l3_pool_len;
  uint16_t *    num_alloc_tts;
  uint8_t      l1_invalidate_done;

  mem_block_t* dflt_memory_map;
  size_t       dflt_memmap_size;
}mmu_ctx_t;


#endif /* _ARM_ASM_ */

/* End tt_ld.h */


/*****************************************************************************
 * MMU related functions
 *****************************************************************************/
void tee_mmu_map (uintptr_t addr, size_t size);
void tee_mmu_init(uintptr_t tee_phy_addr, size_t tee_len, uintptr_t ipc_addr, size_t ipc_len);
extern void teetest64_enable_el1_mmu(void *l, uintnt mair, uintnt tcr);

#endif
