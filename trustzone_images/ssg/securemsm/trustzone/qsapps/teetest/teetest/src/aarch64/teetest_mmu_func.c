
/*===========================================================================
Copyright (c) 2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                          EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/

/*============================================================================

                            INCLUDE FILES FOR MODULE

============================================================================*/
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "IxErrno.h"

#include "teetest.h"
#include "teetest_mmu.h"
#include "mapper_ld.h"
#include "cache.h"
#include "util.h"
#include "teetest_hal_qgic.h"

/*============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

============================================================================*/
/* Default memory mapping attributes  */
#define DEFAULT_MEM_ATTR ( TZ_READ_WRITE_PL1 | TZ_MEM_NON_CACHEABLE | TZ_NON_EXECUTABLE | TZ_PL1_NON_EXECUTABLE )

/* Initial memory mapping attributes - used for whole TEE image */
#define INIT_MEM_ATTR ( TZ_READ_WRITE_PL1 | TZ_MEM_WRITE_THROUGH_NWA )

static void mmu_map_wrapper(
  mmu_ctx_t* const ctx,
  uintptr_t addr,
  size_t   size,
  tz_cfg_t msk
);


static l1_tt_t __attribute__((section("TEETEST64_L1_TT_MEM"),aligned(4096))) l1;
static l2_tt_t __attribute__((section("TEETEST64_L2_TT_MEM"),aligned(4096))) l2[NUM_L2_TTS];
static l3_tt_t __attribute__((section("TEETEST64_L3_TT_MEM"),aligned(4096))) l3[NUM_L3_TTS];

static uint16_t __attribute__((section("TEETEST64__MAPPER_MEM"))) num_ddr_alloc_tts[4] = {0};
static mmu_ctx_t __attribute__((section("TEETEST64__MAPPER_MEM"))) teetest64_mmu_ctx = {0};

/* Mutex for synchronizing the cores accessing MMU lib */
static spinlock_t __attribute__((section("TEETEST64__MAPPER_MEM"),aligned(128))) teetest64_mmu_lock = {0};

/*============================================================================
                              FUNCTION DEFINITIONS
============================================================================*/
/*
  Initialise MMU Context for mapping operations in mapper_ld.c
*/
void setup_mmu_ctx(mmu_ctx_t* ctx){
  ctx->l1_tt_ptr        = &l1;
  ctx->l2_tt_pool_ptr   = l2;
  ctx->l2_pool_len      = ARRAY_SIZE(l2);
  ctx->l3_tt_pool_ptr   = l3;
  ctx->l3_pool_len      = ARRAY_SIZE(l3);
  ctx->num_alloc_tts    = num_ddr_alloc_tts;

  ctx->dflt_mem_init_done = FALSE;

  ctx->lock             = &teetest64_mmu_lock;
  ctx->lock_func        = (tt_lock_func)spinlock_obtain;
  ctx->release_func     = (tt_release_func)spinlock_release;
  ctx->is_locked_func   = (tt_is_locked_func)spinlock_is_mine;
}

/*
  API for e.g. teetest_ipc.c to map a region of memory 
  with default attributes
*/
void tee_mmu_map(uintptr_t addr, size_t size){
  mmu_map_wrapper(
    &teetest64_mmu_ctx,
    addr,
    size,
    DEFAULT_MEM_ATTR
  );   
}

/*
  Convenience wrapper for mmu_map 
  Aligns the address to 4KB boundary
*/
static void mmu_map_wrapper(
  mmu_ctx_t* const ctx,
  uintptr_t addr,
  size_t    size,
  tz_cfg_t  msk
)
{
  /* Check for zero size or address overflow */
  if(!size || (TZ_VAS_LAST_ADDRESS - size + 1 < addr)){ 
    return;
  }
  
  uintptr_t align_start = ALIGN_DOWN(addr, SIZE_4KB);
  size_t align_size  = B2KB(ALIGN_UP((addr - align_start) + size, SIZE_4KB));
      
  (void)mmu_map(
    ctx,
    (v_addr_t)align_start,
    (p_addr_t)align_start,
    align_size,
    msk
  );
}

/*
  Sets all page table elements to unused
*/
static void init_pgtables(
  mmu_ctx_t *ctx
)
{
  size_t i = 0;
  size_t j = 0;
  
  /* Set up the L2 tables */
  for (i = 0; i < ctx->l2_pool_len; i++) {
    for (j = 0; j < NUM_L2_TTES_PER_L1_TTE; j++) {
      ctx->l2_tt_pool_ptr[i].tt[j] = L2_UNUSED_DESC;
    }
  }
  
  /* Set up the L3 tables */
  for (i = 0; i < ctx->l3_pool_len; i++) {
    for (j = 0; j < NUM_L3_TTES_PER_L2_TTE; j++) {
      ctx->l3_tt_pool_ptr[i].tt[j] = L3_UNUSED_DESC;
    }
  }
}

/*
  Initalise Pagetables & enable MMU for TEE 
*/
void tee_mmu_init(uintptr_t tee_phy_addr, size_t tee_len, 
                  uintptr_t ipc_addr,     size_t ipc_len) 
{
  volatile int ret = 0;
  mmu_ctx_t *mmu_ctx = &teetest64_mmu_ctx;
  setup_mmu_ctx(mmu_ctx);
  init_pgtables(mmu_ctx);
    
  /* First map the entire memory region as
  *   normal memory
  *   innner write through, no write alloc
  *   outer  write through, no write alloc
  */
  mmu_map_wrapper (mmu_ctx, tee_phy_addr, tee_len, INIT_MEM_ATTR);
  
  /* Now remap the translation table regions as
  *   normal memory
  *   non-cacheable
  *   xn
  */
  mmu_map_wrapper (mmu_ctx, (uintptr_t)&l1, sizeof(l1), DEFAULT_MEM_ATTR );
  mmu_map_wrapper (mmu_ctx, (uintptr_t)&l2, sizeof(l2), DEFAULT_MEM_ATTR );
  mmu_map_wrapper (mmu_ctx, (uintptr_t)&l3, sizeof(l3), DEFAULT_MEM_ATTR );
  
  /* Map in the IPC memory as per the translation table regions */
  mmu_map_wrapper (mmu_ctx, ipc_addr, ipc_len, DEFAULT_MEM_ATTR );

  /* Map in the QGIC */
  mmu_map_wrapper (mmu_ctx, HWIO_GICD_SGIR_ADDR, SIZE_1MB, DEFAULT_MEM_ATTR);
  
  mmu_ctx->dflt_mem_init_done = TRUE;
  
  /* Enable the MMU */
  teetest64_enable_el1_mmu(&l1, TEETEST64_MAIR, TEETEST64_TCR);
}
