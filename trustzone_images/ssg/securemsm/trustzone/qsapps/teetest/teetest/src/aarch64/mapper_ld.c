/*============================================================================

                  Map Long-Descripter Translation Table Entries

GENERAL DESCRIPTION
  Functions for configuring the translation tables used by the MMU.

Copyright 2012 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/


/*============================================================================

                            EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/teetest/teetest/src/aarch64/mapper_ld.c#2 $
  
  From securemsm/trustzone/armlib/v8/src/

when       who     what, where, why
--------   ---     ------------------------------------------------------------
07/11/11   jct     Include test MMU PT definitions conditionally
04/20/11   jct     Conditional #include depending on inclusion/exclusion of
                   MobiCore
01/26/11   pre     Intial revision.
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
#include "cache.h"
#include "util.h"
#include "mapper_ld.h"
#include "tt_ld.h"
#include "tt_tz.h"
#include "mair.h"


/*============================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

============================================================================*/

int mmu_map
(
  mmu_ctx_t* const ctx,
  v_addr_t v_addr,
  p_addr_t p_addr,
  size_t   sz_in_kb,
  tz_cfg_t msk
);

int mmu_map_block
(
  mmu_ctx_t* const ctx,
  const mem_block_t* map
);

/*============================================================================

                              FUNCTION DEFINITIONS

============================================================================*/

static void tlbi_vaexis(v_addr_t v_addr)
{
  uint32_t current_el = get_current_el();

  switch (current_el)
  {
    case CURRENT_EL_EL0:
    case CURRENT_EL_EL1:
      asm ("tlbi vae1is, %0" :: "r"(v_addr));
      break;
    case CURRENT_EL_EL3:
      asm ("tlbi vae3is, %0" :: "r"(v_addr));
      break;
    case CURRENT_EL_EL2:
    default:
      break;
  }
}

static void mapper_dsb(mmu_ctx_t* const ctx)
{
  if (TRUE == ctx->dflt_mem_init_done)
  {
    asm ("dsb sy" :::);
  }
}

static void mapper_dsb_isb(mmu_ctx_t* const ctx)
{
  /* This is done in clean_dcache_region_wrapper API */
}

static boolean is_tte_valid(ld_tte_t lxtte)
{
  return (lxtte & 0x1) ? TRUE : FALSE;
}

static boolean is_l3tte_valid(ld_tte_t l3tte)
{
  return (l3tte & TTE_DESC_MSK) == L3_BLK_DESC;
}

/* Default memory init happens before truning on MMU/Cache.
 * No need to do tlbi while in default memory map init. The wrapper
 * improves cold boot time.
 */
static void tlbi_vaexis_wrapper(mmu_ctx_t* const ctx, v_addr_t vaddr)
{
  if (TRUE == ctx->dflt_mem_init_done)
  {
    tlbi_vaexis(vaddr);
  }
}

/* Default memory init happens before truning on MMU/Cache.
 * No need to clean dcache while in default memory map init. The
 * wrapper improves cold boot time.
 */
static void clean_dcache_region_wrapper(mmu_ctx_t* const ctx, v_addr_t vaddr, size_t len)
{
  if (TRUE == ctx->dflt_mem_init_done)
  {
    clean_and_inval_dcache_region(vaddr, len);

    asm ("dsb sy" :::);
    asm ("isb" :::);
  }
}

/* TODO: this is the physical address of the L2 TT and may not
   necessarily correspond to the VA of the L2 TT.  This is broken
   for long descriptors since the PA is 64 bits and the VA is 32
   bits. */
static l2_tt_t* l1_walk(ld_tte_t l1_tte)
{
  return (l2_tt_t *) L1TBLADDRMSK(l1_tte);
}

/* TODO: this is the physical address of the L3 TT and may not
   necessarily correspond to the VA of the L3 TT.  This is broken
   for long descriptors since the PA is 64 bits and the VA is 32
   bits. */
static l3_tt_t* l2_walk(ld_tte_t l2_tte)
{
  return (l3_tt_t *) L2TBLADDRMSK(l2_tte);
}

/**
   Returns number of TTE's for VA, size, and mmu level combo

*/
static uint32_t get_num_ttes(uint32_t level, uint32_t v_addr, uint32_t size_kb)
{
  if (0 == size_kb || level > 3)
  {
    return 0;
  }

  /* The first term has -1 in order to make the result [start, end)
     instead of [end, next_end - 1).  The +1 is change results from
     zero-based to 1-based. */
  switch (level)
  {
  case 1:
    return KB2NUML1TTES(B2KB(v_addr) + size_kb - 1) - B2NUML1TTES(v_addr) + 1;
  case 2:
    return KB2NUML2TTES(B2KB(v_addr) + size_kb - 1) - B2NUML2TTES(v_addr) + 1;
  case 3:
    return KB2NUML3TTES(B2KB(v_addr) + size_kb - 1) - B2NUML3TTES(v_addr) + 1;
  }

  return 0;
}

/**
   Converts TZ mmu attribute flags to ARMv8 Block Decsriptor
   attribute flags

*/
static uint64_t tz_to_cmn_blk_cfg(uint32_t tzcfg)
{
  uint64_t xn   = (tzcfg & TZ_XN_MSK);
  uint64_t pxn  = (tzcfg & TZ_PXN_MSK);
  uint64_t ng   = BLK_G;
  uint64_t af   = BLK_AF;
  uint64_t sh   = (tzcfg & TZ_SH_MSK);
  uint64_t ap   = (tzcfg & TZ_AP_MSK);
  uint64_t ns   = (tzcfg & TZ_NS_MSK);
  uint64_t memt = (tzcfg & TZ_MEMT_MSK);

  if      (TZ_NON_EXECUTABLE == xn) xn = BLK_XN;
  else                              xn = BLK_X;

  if      (TZ_PL1_NON_EXECUTABLE == pxn) pxn = BLK_PXN;
  else                                   pxn = BLK_PX;

  if      (TZ_OUT_SHAREABLE == sh) sh = BLK_OUT_SHAREABLE;
  else if (TZ_INR_SHAREABLE == sh) sh = BLK_INR_SHAREABLE;
  else                             sh = BLK_NOT_SHAREABLE;

  if      (TZ_READ_WRITE_PL1 == ap) ap = BLK_AP_PL1_RW;
  else if (TZ_READ_WRITE_PL0 == ap) ap = BLK_AP_RW;
  else if (TZ_READ_ONLY_PL1  == ap) ap = BLK_AP_PL1_RO;
  else if (TZ_READ_ONLY_PL0  == ap) ap = BLK_AP_RO;
  else                              ap = 0;

  if      (TZ_NON_SECURE == ns) ns = BLK_NS;
  else                          ns = BLK_S;

  if      (TZ_MEM_STRONGLY_ORDERED  == memt) memt = BLK_SO;
  else if (TZ_MEM_DEVICE            == memt) memt = BLK_DVC;
  else if (TZ_MEM_NON_CACHEABLE     == memt) memt = BLK_NC;
  else if (TZ_MEM_WRITE_THROUGH_WA  == memt) memt = BLK_WT_WA;
  else if (TZ_MEM_WRITE_THROUGH_NWA == memt) memt = BLK_WT_NWA;
  else if (TZ_MEM_WRITE_BACK_WA     == memt) memt = BLK_WB_WA;
  else if (TZ_MEM_WRITE_BACK_NWA    == memt) memt = BLK_WB_NWA;
  else if (TZ_MEM_NOT_USED          == memt) memt = BLK_NOT_USED;
  else                                       memt = BLK_SO;

  return xn | pxn | ng | af | sh | ap | ns | memt;
}

/**
   Converts TZ Valid/Invalid TT flags to ARMv8 L1 Block
   Decsriptor attribute flags

*/
static ld_tte_t tz_to_l1_blk_cfg(uint32_t tzcfg)
{
  uint64_t invalid = ((tzcfg >> 11) & 0x1) ? 0 : L1_BLK_DESC;

  return (ld_tte_t) (tz_to_cmn_blk_cfg(tzcfg) | invalid);
}

/**
   Converts TZ Valid/Invalid TT flags to ARMv8 L2 Block
   Decsriptor attribute flags

*/
static ld_tte_t tz_to_l2_blk_cfg(uint32_t tzcfg)
{
  return tz_to_l1_blk_cfg(tzcfg);
}

/**
   Converts TZ Valid/Invalid TT flags to ARMv8 L3 Page
   Decsriptor attribute flags

*/
static ld_tte_t tz_to_l3_blk_cfg(uint32_t tzcfg)
{
  uint64_t invalid = ((tzcfg >> 11) & 0x1) ? 0 : L3_BLK_DESC;

  return (ld_tte_t) (tz_to_cmn_blk_cfg(tzcfg) | invalid);
}

/**
   Translate L1 Block Descriptor attributes to L1 Table
   Descriptor attributes

*/
static ld_tte_t l1_blk_cfg_to_l1_tbl_cfg(uint64_t l1tte)
{
  /* Not yet supporting table configuration bits */
  return 0;
}

/**
   Translate L2 Block Descriptor attributes to L2 Table
   Descriptor attributes

*/
static ld_tte_t l2_blk_cfg_to_l2_tbl_cfg(ld_tte_t l1tte)
{
  /* Not yet supporting table configuration bits */
  return 0;
}

/**
   Extracts L1 Block Descriptor attributes

*/
static ld_tte_t l1_blk_cfg_to_l2_blk_cfg(ld_tte_t l1tte)
{
  return l1tte & 0xFFF0000000000FFF;
}

/**
   Converts L2 Block/Table descriptor to L3 Page/Invalid
   descriptor.

*/
static ld_tte_t l2_blk_cfg_to_l3_blk_cfg(ld_tte_t l2tte)
{
  uint64_t l2cfg = (uint64_t) l1_blk_cfg_to_l2_blk_cfg(l2tte);

  if (L2_BLK_DESC == (l2cfg & TTE_DESC_MSK))
  {
    return (ld_tte_t) ((l2cfg & ~TTE_DESC_MSK) | L3_BLK_DESC);
  }
  else
  {
    return (ld_tte_t) (l2cfg & ~TTE_DESC_MSK);
  }
}

/**
   Converts L3 Page descriptor to L2 Block/Invalid descriptor.

*/
static ld_tte_t l3_blk_cfg_to_l2_blk_cfg(ld_tte_t l3tte)
{
  /* Grab only the attribute bits */
  uint64_t l3cfg = (uint64_t)l1_blk_cfg_to_l2_blk_cfg(l3tte);

  if (is_l3tte_valid(l3cfg))
  {
    return (ld_tte_t) ((l3cfg & ~TTE_DESC_MSK) | L2_BLK_DESC);
  }
  else
  {
    return (ld_tte_t) (l3cfg & ~TTE_DESC_MSK); /* Set bit 0 as '0' to indicate invalid */
  }
}

/**
   Retrieve pointer to L1/L2/L3 TTE's for the mmu context

*/
static void* tt_iter_init(mmu_ctx_t* const ctx, int lvl, tt_iter_t* iter_ptr)
{
  iter_ptr->ii = 0;
  iter_ptr->lvl = lvl;

  switch (lvl)
  {
  case 1:
    return (void*) ctx->l1_tt_ptr;
  case 2:
    return (void*) ctx->l2_tt_pool_ptr;
  case 3:
    return (void*) ctx->l3_tt_pool_ptr;
  default:
    return NULL;
  }
}

/**
   @todo This may need to be extended to support non-contiguous l2tt
         memory given by secure applications.
 */
static void* tt_iter_next(mmu_ctx_t* const ctx, tt_iter_t* iter_ptr)
{
  iter_ptr->ii++;

  switch (iter_ptr->lvl)
  {
  case 1:
    return NULL;
  case 2:
    if (iter_ptr->ii < ctx->l2_pool_len)
    {
      return &ctx->l2_tt_pool_ptr[iter_ptr->ii];
    }
    else
    {
      return NULL;
    }
  case 3:
    if (iter_ptr->ii < ctx->l3_pool_len)
    {
      return &ctx->l3_tt_pool_ptr[iter_ptr->ii];
    }
    else
    {
      return NULL;
    }
  default:
    return NULL;
  }
}

static boolean is_tt_free(void* tt_ptr, int lvl)
{
  switch (lvl)
  {
  case 2:
    return L2_UNUSED_DESC != ((ld_tte_t) (((l2_tt_t*) tt_ptr)->tt[0] & 0x3));
  case 3:
    return L3_UNUSED_DESC != ((ld_tte_t) (((l3_tt_t*) tt_ptr)->tt[0] & 0x3));
  default:
    return FALSE;
  }
}

static int num_free_tts(mmu_ctx_t* const ctx, int lvl)
{
  switch (lvl)
  {
  case 2:
    return ctx->l2_pool_len - ctx->num_alloc_tts[1];
  case 3:
    return ctx->l3_pool_len - ctx->num_alloc_tts[2];
  default:
    return 0;
  }
}


static void free_tt(mmu_ctx_t* const ctx, void* tt_ptr, int lvl, v_addr_t v_addr)
{
  switch (lvl)
  {
  case 1:
  {
    l1_tt_t* l1_tt_ptr = (l1_tt_t*) tt_ptr;
    uint32_t ii;

    /* A L1 TT may have table TTEs.  Those also should be freed. */
    mapper_dsb(ctx);
    for (ii = 0; ii < NUM_L1_TTES; ii++)
    {
      if (ISL1TBL(l1_tt_ptr->tt[ii]))
      {
        free_tt(ctx, (void*) l1_walk(l1_tt_ptr->tt[ii]), 2, v_addr + L1TTE2B(ii));
      }

      if (INV_DESC != l1_tt_ptr->tt[ii])
      {
        l1_tt_ptr->tt[ii] = INV_DESC;
        tlbi_vaexis_wrapper(ctx, (v_addr + L1TTE2B(ii)) >> 12);
      }
    }
    mapper_dsb_isb(ctx);

    clean_dcache_region_wrapper(ctx, (v_addr_t) l1_tt_ptr, sizeof(*l1_tt_ptr));

    break;
  }
  case 2:
  {
    l2_tt_t* l2_tt_ptr = (l2_tt_t*) tt_ptr;

    if ((ld_tte_t) L2_UNUSED_DESC != l2_tt_ptr->tt[0])
    {
      uint32_t ii;

      mapper_dsb(ctx);
      for (ii = 0; ii < NUM_L2_TTES_PER_L1_TTE; ii++)
      {
        /* A L2 TT may have table TTEs.  Those also should be freed. */
        if (ISL2TBL(l2_tt_ptr->tt[ii]))
        {
          free_tt(ctx, (void*) l2_walk(l2_tt_ptr->tt[ii]), 3, v_addr + L2TTE2B(ii));
        }

        /* Set each entry to invalid and invalidate TLB for each VA */
        if (INV_DESC != l2_tt_ptr->tt[ii])
        {
          l2_tt_ptr->tt[ii] = INV_DESC;
          tlbi_vaexis_wrapper(ctx, (v_addr + L2TTE2B(ii)) >> 12);
        }
      }
      mapper_dsb_isb(ctx);

      l2_tt_ptr->tt[0] = (ld_tte_t) L2_UNUSED_DESC;
      ctx->num_alloc_tts[1]--;
      clean_dcache_region_wrapper(ctx, (v_addr_t) l2_tt_ptr, sizeof(*l2_tt_ptr));
    }
    break;
  }
  case 3:
  {
    l3_tt_t* l3_tt_ptr = (l3_tt_t*) tt_ptr;

    if ((ld_tte_t) L3_UNUSED_DESC != l3_tt_ptr->tt[0])
    {
      uint32_t ii;

      mapper_dsb(ctx);
      /* Set each entry to invalid and invalidate TLB for each VA */
      for (ii = 0; ii < NUM_L3_TTES_PER_L2_TTE; ii++)
      {
        if (INV_DESC != l3_tt_ptr->tt[ii])
        {
          l3_tt_ptr->tt[ii] = INV_DESC;
          tlbi_vaexis_wrapper(ctx, (v_addr + L3TTE2B(ii)) >> 12);
        }
      }
      mapper_dsb_isb(ctx);

      l3_tt_ptr->tt[0] = (ld_tte_t) L3_UNUSED_DESC;
      ctx->num_alloc_tts[2]--;
      clean_dcache_region_wrapper(ctx, (v_addr_t) l3_tt_ptr, sizeof(*l3_tt_ptr));
    }
    break;
  }
  default:
    break;
  }
}

static void set_tt_allocated(void* tt_ptr, int lvl)
{
  switch(lvl)
  {
  case 2:
    ((l2_tt_t*) tt_ptr)->tt[0] = L2_INV_DESC;
    break;
  case 3:
    ((l3_tt_t*) tt_ptr)->tt[0] = L3_INV_DESC;
    break;
  default:
    break;
  }
}

/**
   @todo This may need to be extended to allocate pages from either
         IMEM or DDR depending on the security concerns.  Currently
         we're not allowing DDR based TTs.
 */
static int alloc_tt(mmu_ctx_t* const ctx, void** tt_ptr_ptr, int lvl)
{
  void* tt_ptr;
  tt_iter_t iter;

  if (2 != lvl && 3 != lvl)
  {
    return -E_FAILURE;
  }

  if (0 == num_free_tts(ctx, lvl))
  {
    return -E_NO_MEMORY;
  }

  for (tt_ptr = (l2_tt_t*) tt_iter_init(ctx, lvl, &iter);
       tt_ptr != NULL;
       tt_ptr = tt_iter_next(ctx, &iter))
  {
    if (!is_tt_free(tt_ptr, lvl))
    {
      /* Found an open slot */
      *tt_ptr_ptr = tt_ptr;
      ctx->num_alloc_tts[lvl - 1]++;
      set_tt_allocated(tt_ptr, lvl);

      return E_SUCCESS;
    }
  }

  return -E_FAILURE;
}

static void check_l2_tte_consolidation(l2_tt_t* l2_tt_ptr, uint32_t v_addr)
{
  uint32_t ii;

  for (ii = 1; ii < NUM_L2_TTES_PER_L1_TTE; ii++)
  {
    /* Mappings must have the exact same memory attributes and must
       map to continuous physical addresses */
    /* TODO: */
    /* if ((l2_tt_ptr[ii] & 0x00000FFF) !=  (l2_tt_ptr[0]      & 0x00000FFF) || */
    /*     (l2_tt_ptr[ii] & 0xFFFFF000) != ((l2_tt_ptr[ii - 1] & 0xFFFFF000) + L2TTE2B(1))) */
    /* { */
    return;
    /* } */
  }

  /* l1.tt[B2L1TTE(v_addr)] = */
  /*   (l2_tt_ptr[0] & 0xFFF00000) | tzbsp_get_1mb_l1_cfg_from_4kb_l2_cfg(l2_tt_ptr[0]); */

  /*  tlbi_vaexis_wrapper(ctx, (v_addr & L1_ADDR_MSK) >> 12);*/

  /* clean_dcache_region_wrapper(ctx, &l1.tt[B2L1TTE(v_addr)], sizeof(l1.tt[B2L1TTE(v_addr)])); */

  /* tzbsp_free_l3_tt(l2_tt, v_addr & L1_ADDR_MSK); */
}

static void check_l3_tte_consolidation(mmu_ctx_t* const ctx, l3_tt_t* l3_tt_ptr, ld_tte_t* l2_tte_ptr)
{
  uint32_t l3_tt_ii;

  /* Initial values for state variables are based off the first
     translation table entry. */
  boolean all_valid = is_l3tte_valid(l3_tt_ptr->tt[0]);
  boolean all_invalid = !all_valid;
  boolean all_same_config = TRUE;

  for ( l3_tt_ii = 1; l3_tt_ii < NUM_L3_TTES_PER_L2_TTE; l3_tt_ii++ )
  {
    /* Only allow one way state transition for these variables. */
    if ( is_l3tte_valid(l3_tt_ptr->tt[l3_tt_ii]) )
    {
      all_invalid = FALSE;
    }
    else
    {
      all_valid = FALSE;
    }

    /* Check if attributes are the same, and output address has same offset */
    if ( !((l3_tt_ptr->tt[l3_tt_ii] & (TTE_ATTR_MSK | TTE_DESC_MSK)) ==
           (l3_tt_ptr->tt[0] & (TTE_ATTR_MSK | TTE_DESC_MSK))  &&
           ((L3BLKADDRMSK(l3_tt_ptr->tt[l3_tt_ii])) ==
           ((L3BLKADDRMSK(l3_tt_ptr->tt[l3_tt_ii-1])) + L3TTE2B(1)))) )
    {
      all_same_config = FALSE;
    }

    /* Escape loop if we can, to save time. */
    if ( !all_invalid && (!all_valid || !all_same_config) )
    {
      return;
    }
  }

  /* Consolidate this L3 table if:
   * - All entries are invalid
   * OR
   * - All entries are valid and have the same attributes and output
   *   address offset.
   */
  if ( all_invalid || (all_valid && all_same_config) )
  {
    /* Take bits 39:21 for the output address */
    *l2_tte_ptr =
      L2BLKADDRMSK(l3_tt_ptr->tt[0]) |
      l3_blk_cfg_to_l2_blk_cfg(l3_tt_ptr->tt[0]);

    clean_dcache_region_wrapper(ctx, (v_addr_t) l2_tte_ptr, sizeof(ld_tte_t));
    free_tt(ctx, l3_tt_ptr, 3, (v_addr_t) L3BLKADDRMSK(l3_tt_ptr->tt[0]));
  }
}

static void inval_tt_lvl(mmu_ctx_t* const ctx, uint32_t lvl, v_addr_t v_addr)
{
  l2_tt_t* tt_ptr;
  tt_iter_t iter;

  if (0 < lvl && lvl < 4)
  {
    for (tt_ptr = (l2_tt_t*) tt_iter_init(ctx, lvl, &iter);
         tt_ptr != NULL;
         tt_ptr = (l2_tt_t*) tt_iter_next(ctx, &iter))
    {
      free_tt(ctx, tt_ptr, lvl, v_addr);
    }

    ctx->num_alloc_tts[lvl - 1] = 0;
  }
}

void discover_used_tts(mmu_ctx_t* const ctx, uint32_t lvl)
{
  l2_tt_t* tt_ptr;
  tt_iter_t iter;

  if (0 < lvl && lvl < 4)
  {
    for (tt_ptr = (l2_tt_t*) tt_iter_init(ctx, lvl, &iter);
         tt_ptr != NULL;
         tt_ptr = (l2_tt_t*) tt_iter_next(ctx, &iter))
    {
      if (is_tt_free(tt_ptr, lvl))
      {
        /* Found a used slot */
        ctx->num_alloc_tts[lvl - 1]++;
      }
    }
  }
}

/**
   Maps a block descriptor into the translation table as l1 ttes

   @retval None

   @par Side Effects Existing entries in the tt will be replaced.

   @dep l1_tt must be allocated
*/
static void map_using_only_l1_ttes
(
  mmu_ctx_t* const ctx,
  const mem_block_t* block
)
{
  const ld_tte_t cfg = tz_to_l1_blk_cfg(block->tz_cfg);
  const uint32_t num_l1_ttes = get_num_ttes(1, block->v_addr,
                                          block->sz_in_kb);
  uint32_t tt_ii, tt_limit, tt_base;
  p_addr_t p_addr;
  l1_tt_t* l1_tt_ptr = ctx->l1_tt_ptr;
  boolean is_tlbi_needed;

  /* Convert our base address into an index into the translation table */
  tt_base = B2L1TTE(block->v_addr);

  /* Upper limit of mapping range */
  tt_limit = tt_base + num_l1_ttes;

  /* Mask out any unimportant address bits */
  p_addr = L1BLKADDRMSK(block->p_addr);

  mapper_dsb(ctx);
  for (tt_ii = tt_base; tt_ii < tt_limit; tt_ii++, p_addr += L1_MAP_SIZE)
  {
    l2_tt_t* l2_tt_ptr = NULL;

    /* If this L1 TTE points to a L2 TT, save the L2 TT so it can be
       freed later */
    if (ISL1TBL(l1_tt_ptr->tt[tt_ii]))
    {
      l2_tt_ptr = l1_walk(l1_tt_ptr->tt[tt_ii]);
    }

    /*  First-Level (L1) Block Format:
       ______________________________________________________________________
      |                        |                     |                       |
      |63--------------------52|51-----------------40|39-------------------32|
      | Upper block attributes |       UNK/SBZP      |    Output address     |
      |________________________|_____________________|_______________________|
      |                          |           |                           | | |
      |31----------------------30|29-------12|11------------------------2|1|0|
      | Output address (cont...) |  UNK/SBZP | Lower block attributes    |0|1|
      |__________________________|___________|___________________________|_|_|

      Upper Attributes:
       ______________________________________________________
      |          |                   |  |    |               |
      |63------59|58---------------55|54|-53-|------52-------|
      |  Ignored |  Reserved for SW  |XN|PXN |Contiguous hint|
      |__________|___________________|__|____|_______________|

      Lower Attributes:
       ______________________________________
      |  |  |       |       |  |             |
      |11|10|9-----8|7-----6|-5|4-----------2|
      |nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|
      |__|__|_______|_______|__|_____________|
    */

    is_tlbi_needed = is_tte_valid(l1_tt_ptr->tt[tt_ii]);
    l1_tt_ptr->tt[tt_ii] = p_addr | cfg;
    if (TRUE == is_tlbi_needed)
    {
      tlbi_vaexis_wrapper(ctx, (v_addr_t) L1TTE2B(tt_ii) >> 12);
    }

    /* If the L1 TTE pointed to a L2 TT, free the L2 TT now that
       the L1 TTE has been modified. */
    if (NULL != l2_tt_ptr)
    {
      free_tt(ctx, l2_tt_ptr, 2, L1TTE2B(tt_ii));
    }
  }
  mapper_dsb_isb(ctx);

  /* Write changes out to DDR */
  clean_dcache_region_wrapper(ctx, (v_addr_t) &l1_tt_ptr->tt[tt_base],
                              (tt_limit - tt_base) * sizeof(l1_tt_ptr->tt[tt_base]));

} /* map_using_only_l1_ttes */


/**
   For a given virtual address, this function will retrieve the level
   two translation table address, if the level one translation table
   was programmed to point to one, or it will return a newly allocated
   level two translation table and return the L1 translation table
   configuration, translated to L2 format and the old physical address
   mapping of the L1 translation table.

   @retval E_SUCCESS Level one and two translation tables populated
                     successfully

   @pre l1.tt must be allocated
   @pre l2.tt must be allocated
*/
static int alloc_or_get_l2_tt
(
  mmu_ctx_t* const ctx,
  v_addr_t  v_addr,
  l2_tt_t** l2_tt_ptr_ptr,
  ld_tte_t* l2_cfg_from_l1,
  p_addr_t* p_addr_from_l1,
  boolean*  new_l2_tt
)
{
  /* Convert our virtual base address into an index into the L1
     translation table. */
  uint32_t tt_ii = B2L1TTE(v_addr);
  l1_tt_t* l1_tt_ptr = ctx->l1_tt_ptr;

  /* Does this L1 TTE already point to an allocated L2 TT? */
  if (ISL1TBL(l1_tt_ptr->tt[tt_ii]))
  {
    /* Walk the table to get the L2 TT address */
    *l2_tt_ptr_ptr = l1_walk(l1_tt_ptr->tt[tt_ii]);
    *l2_cfg_from_l1 = 0;
    *p_addr_from_l1 = 0;
    *new_l2_tt = FALSE;
  }
  else
  {
    uint64_t cfg;

    /* Allocate a new translation table */
    if (alloc_tt(ctx, (void**) l2_tt_ptr_ptr, 2) != E_SUCCESS)
    {
      return -E_NO_MEMORY;
    }

    *new_l2_tt = TRUE;
    *l2_cfg_from_l1 = l1_blk_cfg_to_l2_blk_cfg(l1_tt_ptr->tt[tt_ii]);
    *p_addr_from_l1 = L1BLKADDRMSK(l1_tt_ptr->tt[tt_ii]);
  }

  return E_SUCCESS;
}

static int alloc_or_get_l3_tt
(
  mmu_ctx_t* const ctx,
  l2_tt_t*  l2_tt_ptr,
  uint32_t    l2_tt_ii,
  l3_tt_t** l3_tt_ptr_ptr,
  ld_tte_t* l3_cfg_from_l2,
  p_addr_t* p_addr_from_l2,
  boolean*  new_l3_tt
)
{
  /* Does this L2 TTE already point to an allocated L3 TT? */
  if (ISL2TBL(l2_tt_ptr->tt[l2_tt_ii]))
  {
    /* Walk the table to get the L3 TT address */
    *l3_tt_ptr_ptr = l2_walk(l2_tt_ptr->tt[l2_tt_ii]);
    *l3_cfg_from_l2 = 0;
    *p_addr_from_l2 = 0;
    *new_l3_tt = FALSE;
  }
  else
  {
    uint64_t cfg;

    /* Allocate a new translation table */
    if (alloc_tt(ctx, (void**) l3_tt_ptr_ptr, 3) != E_SUCCESS)
    {
      return -E_NO_MEMORY;
    }

    *new_l3_tt = TRUE;
    *l3_cfg_from_l2 = l2_blk_cfg_to_l3_blk_cfg(l2_tt_ptr->tt[l2_tt_ii]);
    *p_addr_from_l2 = L2BLKADDRMSK(l2_tt_ptr->tt[l2_tt_ii]);
  }

  return E_SUCCESS;
}

/**
   Checks the L1 entries at the given address to see how many already
   point to allocated level two translation tables and how many need
   new allocations

   @retval number of level two translation tables needed for the given
           virtual address and number of L2 TTEs
*/
static int get_num_l2_tts_needed
(
  mmu_ctx_t* const ctx,
  const uint32_t v_addr,
  const uint32_t num_l1_ttes
)
{
  uint32_t l1_tte_ii, tt_ii;
  int    num_l2_tts_needed = 0;
  l1_tt_t* l1_tt_ptr = ctx->l1_tt_ptr;

  /* Check that there is space for all the newly needed l2 translation
     tables */
  for (l1_tte_ii = 0; l1_tte_ii < num_l1_ttes; l1_tte_ii++)
  {
    /* Convert our virtual base address into an index into the L1
       translation table. */
    tt_ii = B2L1TTE(v_addr) + l1_tte_ii;

    /* Does this L1 TTE already point to an allocated L2 TT? */
    if (!ISL1TBL(l1_tt_ptr->tt[tt_ii]))
    {
      num_l2_tts_needed++;
    }
  }

  return num_l2_tts_needed;
}

static int get_num_l3_tts_needed
(
  mmu_ctx_t* const ctx,
  const uint32_t v_addr,
  const uint32_t num_l2_ttes
)
{
  uint32_t l2_tte_ii, l1_tte_ii;
  int    num_l3_tts_needed = 0;
  l2_tt_t* l2_tt_ptr;
  l1_tt_t* l1_tt_ptr = ctx->l1_tt_ptr;

  l1_tte_ii = B2L1TTE(v_addr);

  if (ISL1TBL(l1_tt_ptr->tt[l1_tte_ii]))
  {
    l2_tt_ptr = l1_walk(l1_tt_ptr->tt[l1_tte_ii]);

    /* Check that there is space for all the newly needed l3 translation
       tables */
    for (l2_tte_ii = B2L2TTE(v_addr);
         l2_tte_ii < B2L2TTE(v_addr) + num_l2_ttes;
         l2_tte_ii++)
    {
      /* Does this L2 TTE already point to an allocated L3 TT? */
      if (!ISL2TBL(l2_tt_ptr->tt[l2_tte_ii]))
      {
        num_l3_tts_needed++;
      }
    }
  }
  else
  {
     num_l3_tts_needed = num_l2_ttes;
  }

  return num_l3_tts_needed;
}

/**
   Maps second level translation table entries.

   @retval E_SUCCESS Level one and two translation tables populated
                     successfully


   @par Side Effects
   Existing entries in the tt will be replaced.

   @pre l2_tt must be allocated
*/
static int map_using_only_l2_ttes
(
  mmu_ctx_t* const ctx,
  const mem_block_t* block
)
{
  const uint32_t num_l1_ttes = get_num_ttes(1, block->v_addr,
                                          block->sz_in_kb);
  const uint32_t num_l2_ttes = get_num_ttes(2, block->v_addr,
                                          block->sz_in_kb);
  const ld_tte_t cfg = tz_to_l2_blk_cfg(block->tz_cfg);
  const uint32_t num_l2_tts_req = get_num_l2_tts_needed(ctx,
                                                      block->v_addr,
                                                      num_l1_ttes);

  l1_tt_t* l1_tt_ptr = ctx->l1_tt_ptr;
  l2_tt_t* l2_tt_ptr;
  uint32_t  tt_ii, tt_limit, tt_offset, l1_tt_ii;
  uint64_t  p_addr_from_l1;
  int64_t   p_offset = block->p_addr - block->v_addr;
  ld_tte_t l2_cfg_from_l1;
  boolean new_l2_tt;
  boolean is_tlbi_needed;

  if (num_l2_tts_req > num_free_tts(ctx, 2))
  {
    return -E_NO_MEMORY;
  }

  /* We'll have to do each step for every l1 translation table entry
     that gets replaced with a pointer to a l2 translation table
   */
  mapper_dsb(ctx);
  for (l1_tt_ii = 0; l1_tt_ii < num_l1_ttes; l1_tt_ii++)
  {
    int retval = E_SUCCESS;

    retval =
      alloc_or_get_l2_tt(ctx,
                         (block->v_addr + L1TTE2B(l1_tt_ii)),
                         &l2_tt_ptr, &l2_cfg_from_l1,
                         &p_addr_from_l1, &new_l2_tt);

    RETCHK(retval);

    /* There will only be an offset for the first L1 TT and a limit
       for the last L1 TT. */
    tt_offset = (l1_tt_ii == 0) ? B2L2TTE(block->v_addr) : 0;
    tt_limit = NUM_L2_TTES_PER_L1_TTE;
    if(l1_tt_ii == (num_l1_ttes - 1))
    {
      tt_limit = (B2L2TTE(block->v_addr) + num_l2_ttes) % NUM_L2_TTES_PER_L1_TTE;
      /* For last iteration if pending L2 TT count reaches max index,
         L2 tt_limit will become 0x0 above but need to loop for max L2
         index per L1 (NUM_L2_TTES_PER_L1_TTE) */
      if(!tt_limit) tt_limit = NUM_L2_TTES_PER_L1_TTE;
    }

    /* Second-Level (L2) Block format:
       ______________________________________________________________________
      |                        |                     |                       |
      |63--------------------52|51-----------------40|39-------------------32|
      | Upper block attributes |       UNK/SBZP      |    Output address     |
      |________________________|_____________________|_______________________|
      |                          |           |                           | | |
      |31----------------------21|20-------12|11------------------------2|1|0|
      | Output address (cont...) |  UNK/SBZP | Lower block attributes    |0|1|
      |__________________________|___________|___________________________|_|_|

      Upper Attributes:
       ______________________________________________________
      |          |                   |  |    |               |
      |63------59|58---------------55|54|-53-|------52-------|
      |  Ignored |  Reserved for SW  |XN|PXN |Contiguous hint|
      |__________|___________________|__|____|_______________|

      Lower Attributes:
       ______________________________________
      |  |  |       |       |  |             |
      |11|10|9-----8|7-----6|-5|4-----------2|
      |nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|
      |__|__|_______|_______|__|_____________|
    */

    /* Iterate through all the L2 descriptors in the L1 descriptor */
    for (tt_ii = 0; tt_ii < NUM_L2_TTES_PER_L1_TTE; tt_ii++)
    {
      /* If the index lies between the base index and the limit, then
         this is the given l2tte we need to map */
      if ((tt_offset <= tt_ii) && (tt_ii < tt_limit))
      {
        l3_tt_t* l3_tt_ptr = NULL;

        /* If this L2 TTE points to a L3 TT, save the L3 TT to free it
           later */
        if (ISL2TBL(l2_tt_ptr->tt[tt_ii]))
        {
          l3_tt_ptr = l2_walk(l2_tt_ptr->tt[tt_ii]);
        }

        is_tlbi_needed = is_tte_valid(l2_tt_ptr->tt[tt_ii]);
        l2_tt_ptr->tt[tt_ii] = ((ld_tte_t) (block->v_addr & L1_ADDR_MSK) +
                                L1TTE2B(l1_tt_ii) + L2TTE2B(tt_ii) +
                                p_offset) | cfg;

        /* If the L2 TTE pointed to a L3 TT, free the L3 TT now that
           the L2 TTE has been modified. */
        if (NULL != l3_tt_ptr)
        {
          free_tt(ctx, (void*) l3_tt_ptr, 3,
                  (block->v_addr & L1_ADDR_MSK) + L1TTE2B(l1_tt_ii) +
                  L2TTE2B(tt_ii));
        }
        else if (TRUE == is_tlbi_needed)
        {
          /* TLB invalidation is done as part of the above function
             call to free the L3 table. */
           tlbi_vaexis_wrapper(ctx, ((block->v_addr & L1_ADDR_MSK) + L1TTE2B(l1_tt_ii) +
                      L2TTE2B(tt_ii)) >> 12);
        }
      }
      /* If this is a newly mapped L2 TT, write the non-specified L2
         TTEs with the mapping retrieved from the L1 TTE.  TLB
         invalidation for this case is done when reassigning the L1
         block to a L1 table. */
      else if (new_l2_tt)
      {
        l2_tt_ptr->tt[tt_ii] = ((p_addr_from_l1 + L2TTE2B(tt_ii)) |
                                l2_cfg_from_l1);
      }
    }

    if (new_l2_tt)
    {
      uint32_t tt_ii_local = B2L1TTE(block->v_addr + L1TTE2B(l1_tt_ii));
      ld_tte_t l1_cfg = l1_blk_cfg_to_l1_tbl_cfg(l1_tt_ptr->tt[tt_ii_local]);

      /* Write entire new L2TT to DDR */
      clean_dcache_region_wrapper(ctx, (v_addr_t) l2_tt_ptr, sizeof(*l2_tt_ptr));

      /* First-Level (L1) Table Format:
       ______________________________________________________________________
       |                        |                     |                       |
       |-63--|62-61|-60--|-59---|58---52|51----40|39------------------------32|
       |NSTbl|APTbl|XNTbl|PxNTbl|Ignored|UNK/SBZP|  Next-level table address  |
       |________________________|_____________________|_______________________|
       |                                      |                           | | |
       |31----------------------------------12|11------------------------2|1|0|
       |  Next-level table address (cont...)  |       Ignored             |1|1|
       |______________________________________|___________________________|_|_|
      */

      /* Enter the L2 base address and mark the descriptor for L2
       * translation table and default L1 TTE for L2 TTE info.
       */
      is_tlbi_needed = is_tte_valid(l1_tt_ptr->tt[tt_ii_local]);
      l1_tt_ptr->tt[tt_ii_local] =
        (((ld_tte_t) L1TBLADDRMSK((uint64_t) l2_tt_ptr)) | l1_cfg |
         L1_TBL_DESC);

      if (TRUE == is_tlbi_needed)
      {
        tlbi_vaexis_wrapper(ctx, (v_addr_t) L1TTE2B(tt_ii_local) >> 12);
      }

      clean_dcache_region_wrapper(ctx, (v_addr_t) &l1_tt_ptr->tt[tt_ii_local],
                                  sizeof(l1_tt_ptr->tt[tt_ii_local]));
    }
    else
    {
      /* Write only changes out to DDR */
      clean_dcache_region_wrapper(ctx, (v_addr_t) &l2_tt_ptr->tt[tt_offset],
                                  (tt_limit - tt_offset) *
                                  sizeof(l2_tt_ptr->tt[tt_offset]));
    }

    /* Determine if all L2TTEs in the given L2TT can be combined into
       one L1TTE */
    check_l2_tte_consolidation(l2_tt_ptr,
                               (block->v_addr + L1TTE2B(l1_tt_ii)));
  }
  mapper_dsb_isb(ctx);

  return E_SUCCESS;
} /* map_using_only_l2_ttes */

static int map_using_only_l3_ttes
(
  mmu_ctx_t* const ctx,
  const mem_block_t * block
)
{
  const ld_tte_t cfg = tz_to_l3_blk_cfg(block->tz_cfg);

  const uint32_t num_l1_ttes = get_num_ttes(1, block->v_addr,
                                          block->sz_in_kb);
  const uint32_t num_l2_ttes = get_num_ttes(2, block->v_addr,
                                          block->sz_in_kb);
  const uint32_t num_l3_ttes = get_num_ttes(3, block->v_addr,
                                          block->sz_in_kb);

  const uint32_t num_l2_tts_req = get_num_l2_tts_needed(ctx,
                                                      block->v_addr,
                                                      num_l1_ttes);
  const uint32_t num_l3_tts_req = get_num_l3_tts_needed(ctx,
                                                      block->v_addr,
                                                      num_l2_ttes);
  l1_tt_t* l1_tt_ptr = ctx->l1_tt_ptr;
  l3_tt_t* l3_tt_ptr;
  l2_tt_t* l2_tt_ptr;
  uint32_t l1_tt_ii;
  uint32_t l2_tt_ii, l2_tt_limit, l2_tt_offset;
  uint32_t l3_tt_ii, l3_tt_limit, l3_tt_offset;
  p_addr_t p_addr_from_l1, p_addr_from_l2;
  int64_t  p_offset = block->p_addr - block->v_addr;
  ld_tte_t l2_cfg_from_l1, l3_cfg_from_l2;
  boolean new_l2_tt, new_l3_tt;
  boolean is_tlbi_needed;

  if (num_l2_tts_req > num_free_tts(ctx, 2) || num_l3_tts_req > num_free_tts(ctx, 3))
  {
    return -E_NO_MEMORY;
  }

  /* We'll have to do each step for every l1 translation table entry
     that gets replaced with a pointer to a l2 translation table
   */
  mapper_dsb(ctx);
  for (l1_tt_ii = 0; l1_tt_ii < num_l1_ttes; l1_tt_ii++)
  {
    int retval = E_SUCCESS;

    retval =
      alloc_or_get_l2_tt(ctx,
                         (block->v_addr + L1TTE2B(l1_tt_ii)),
                         &l2_tt_ptr, &l2_cfg_from_l1,
                         &p_addr_from_l1, &new_l2_tt);

    RETCHK(retval);

    /* There will only be an offset for the first L1 TT and a limit
       for the last L1 TT. */
    l2_tt_offset = (0 == l1_tt_ii) ? B2L2TTE(block->v_addr) : 0;
    l2_tt_limit = NUM_L2_TTES_PER_L1_TTE;
    if(l1_tt_ii == (num_l1_ttes - 1))
    {
      l2_tt_limit = (B2L2TTE(block->v_addr) + num_l2_ttes) % NUM_L2_TTES_PER_L1_TTE;
      /* For last iteration if pending L2 TT count reaches max index,
         L2 tt_limit will become 0x0 above but need to loop for max L2
         index per L1 (NUM_L2_TTES_PER_L1_TTE) */
      if(!l2_tt_limit) l2_tt_limit = NUM_L2_TTES_PER_L1_TTE;
    }


    for (l2_tt_ii = 0; l2_tt_ii < NUM_L2_TTES_PER_L1_TTE; l2_tt_ii++)
    {
      /* Filling in L2 info from L1 info must be done before filling
         in L3 info so L1 configurations can properly propagate down
         to the L3 TTs. TLB invalidation will be done when the L1 is
         changed from a block entry to a table entry. */
      if (new_l2_tt)
      {
        l2_tt_ptr->tt[l2_tt_ii] = ((ld_tte_t) (p_addr_from_l1 +
                                               L2TTE2B(l2_tt_ii)) |
                                               l2_cfg_from_l1);
      }

      if ((l2_tt_offset <= l2_tt_ii) && (l2_tt_ii < l2_tt_limit))
      {
        retval =
          alloc_or_get_l3_tt(ctx, l2_tt_ptr, l2_tt_ii,
                             &l3_tt_ptr, &l3_cfg_from_l2, &p_addr_from_l2,
                             &new_l3_tt);

        RETCHK(retval);

        /* For the first iteration the TTE index offset (VA offset) is
           taken from the virtual base parameter.  The physical address
           must include all bits of information from the physical base
           parameter. */
        if ((l2_tt_offset == l2_tt_ii) && (0 == l1_tt_ii) )
        {
          l3_tt_offset = B2L3TTE(block->v_addr);
        }
        /* For each remaining iteration the TTE index offset (VA offset)
           will be zero and the non L1 entry length aligned bits must be
           removed from the physical base parameter. */
        else
        {
          l3_tt_offset = 0;
        }

        /* The last iteration TTE index limit (VA limit) may not be the
           last index. Let's find the limit for last iteration.
           Every other iteration must end at the last index. */
        l3_tt_limit = NUM_L3_TTES_PER_L2_TTE;
        if ((l2_tt_ii == (l2_tt_limit - 1)) && (l1_tt_ii == (num_l1_ttes - 1)))
        {
          l3_tt_limit = (B2L3TTE(block->v_addr) + num_l3_ttes) % NUM_L3_TTES_PER_L2_TTE;
          /* For last iteration if pending L3 TT count reaches max index,
             L3 tt_limit will become 0x0 above but need to loop for max L3
             index per L2 (NUM_L3_TTES_PER_L2_TTE) */
          if(!l3_tt_limit) l3_tt_limit = NUM_L3_TTES_PER_L2_TTE;
        }

        /* Third-Level (L3) Page (Block) Format:
        ______________________________________________________________________
       |                        |                     |                       |
       |63--------------------52|51-----------------40|39-------------------32|
       | Upper page attributes  |       UNK/SBZP      |    Output address     |
       |________________________|_____________________|_______________________|
       |                                      |                           | | |
       |31----------------------------------12|11------------------------2|1|0|
       |      Output address (cont...)        |  Lower page attributes    |1|1|
       |______________________________________|___________________________|_|_|

       Upper Attributes:
        ______________________________________________________
       |          |                   |  |    |               |
       |63------59|58---------------55|54|-53-|------52-------|
       |  Ignored |  Reserved for SW  |XN|PXN |Contiguous hint|
       |__________|___________________|__|____|_______________|

       Lower Attributes:
        ______________________________________
       |  |  |       |       |  |             |
       |11|10|9-----8|7-----6|-5|4-----------2|
       |nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|
       |__|__|_______|_______|__|_____________| */


        /* Iterate through all the L3 descriptors in the L2 descriptor */
        for (l3_tt_ii = 0; l3_tt_ii < NUM_L2_TTES_PER_L1_TTE;
             l3_tt_ii++)
        {
          /* If the index lies between the base index and the limit, then
             this is the area that needs mapping */
          if ((l3_tt_offset <= l3_tt_ii) && (l3_tt_ii < l3_tt_limit))
          {
            is_tlbi_needed = is_tte_valid(l3_tt_ptr->tt[l3_tt_ii]);
            l3_tt_ptr->tt[l3_tt_ii] =
              ((ld_tte_t) (block->v_addr & L1_ADDR_MSK) + L1TTE2B(l1_tt_ii) +
               L2TTE2B(l2_tt_ii) + L3TTE2B(l3_tt_ii) + p_offset) | cfg;
            if (TRUE == is_tlbi_needed)
            {
              tlbi_vaexis_wrapper(ctx, ((block->v_addr & L1_ADDR_MSK) + L1TTE2B(l1_tt_ii) +
                   L2TTE2B(l2_tt_ii) + L3TTE2B(l3_tt_ii)) >> 12);
            }
          }
          /* If this is a newly mapped L2 TT, write the non-specified L2
             TTEs with the mapping retrieved from the L1 TTE */
          else if (new_l3_tt)
          {
            l3_tt_ptr->tt[l3_tt_ii] = (((ld_tte_t) p_addr_from_l2 +
                                        L3TTE2B(l3_tt_ii)) | l3_cfg_from_l2);

            /* TLB invalidation is done on the L2 block entry when
               it is reassigned to a L3 table entry. */
          }
        }

        if (new_l3_tt)
        {
          ld_tte_t l2_cfg = l2_blk_cfg_to_l2_tbl_cfg(l2_tt_ptr->tt[l2_tt_ii]);

          /* Write entire new L3 TT to DDR */
          clean_dcache_region_wrapper(ctx, (v_addr_t) l3_tt_ptr, sizeof(*l3_tt_ptr));

          /* Second-Level (L2) Table Format:
       _______________________________________________________________________
       |     |     |     |      |       |        |                            |
       |-63--|62-61|-60--|-59---|58---52|51----40|39------------------------32|
       |NSTbl|APTbl|XNTbl|PxNTbl|Ignored|UNK/SBZP|  Next-level table address  |
       |_____|_____|_____|______|_______|________|____________________________|
       |                                      |                           | | |
       |31----------------------------------12|11------------------------2|1|0|
       |  Next-level table address (cont...)  |       Ignored             |1|1|
       |______________________________________|___________________________|_|_|
          */

          /* Enter the L3 base address and mark the descriptor for L3
           * translation table and default L2 TTE for L3 TTE info.
           */
          is_tlbi_needed = is_tte_valid(l2_tt_ptr->tt[l2_tt_ii]);
          l2_tt_ptr->tt[l2_tt_ii] =
            (((ld_tte_t) L2TBLADDRMSK((uint64_t) l3_tt_ptr)) | l2_cfg |
             L2_TBL_DESC);
          if (TRUE == is_tlbi_needed)
          {
            tlbi_vaexis_wrapper(ctx, ((block->v_addr & L1_ADDR_MSK) +
                         L1TTE2B(l1_tt_ii) + L2TTE2B(l2_tt_ii)) >> 12);
          }

          clean_dcache_region_wrapper(ctx, (v_addr_t) &l2_tt_ptr->tt[l2_tt_ii],
                                      sizeof(l2_tt_ptr->tt[l2_tt_ii]));
        }
        else
        {
          /* Write only changes out to DDR */
          clean_dcache_region_wrapper(ctx, (v_addr_t) &l3_tt_ptr->tt[l3_tt_offset],
                                      (l3_tt_limit - l3_tt_offset) *
                                      sizeof(l3_tt_ptr->tt[l3_tt_offset]));
        }

        /* Determine if all L3TTEs in the given L3TT can be combined into
           one L2TTE */
        check_l3_tte_consolidation(ctx, l3_tt_ptr, &l2_tt_ptr->tt[l2_tt_ii]);

      }
    }

    if (new_l2_tt)
    {
      uint32_t tt_ii_local = B2L1TTE(block->v_addr + L1TTE2B(l1_tt_ii));
      ld_tte_t l1_cfg = l1_blk_cfg_to_l1_tbl_cfg(l1_tt_ptr->tt[tt_ii_local]);

      /* Write entire new L2TT to DDR */
      clean_dcache_region_wrapper(ctx, (v_addr_t) l2_tt_ptr, sizeof(*l2_tt_ptr));

      /* First-Level (L1) Table Format:
       ______________________________________________________________________
       |                        |                     |                       |
       |-63--|62-61|-60--|-59---|58---52|51----40|39------------------------32|
       |NSTbl|APTbl|XNTbl|PxNTbl|Ignored|UNK/SBZP|  Next-level table address  |
       |________________________|_____________________|_______________________|
       |                                      |                           | | |
       |31----------------------------------12|11------------------------2|1|0|
       |  Next-level table address (cont...)  |       Ignored             |1|1|
       |______________________________________|___________________________|_|_|
      */

      /* Enter the L2 base address and mark the descriptor for L2
       * translation table and default L1 TTE for L2 TTE info.
       */
      is_tlbi_needed = is_tte_valid(l1_tt_ptr->tt[tt_ii_local]);
      l1_tt_ptr->tt[tt_ii_local] =
        (((ld_tte_t) L1TBLADDRMSK((uint64_t) l2_tt_ptr)) | l1_cfg |
         L1_TBL_DESC);

      if (TRUE == is_tlbi_needed)
      {
        tlbi_vaexis_wrapper(ctx, (v_addr_t) L1TTE2B(tt_ii_local) >> 12);
      }

      clean_dcache_region_wrapper(ctx, (v_addr_t) &l1_tt_ptr->tt[tt_ii_local],
                                  sizeof(l1_tt_ptr->tt[tt_ii_local]));
    }
    else
    {
      /* Write only changes out to DDR */
      clean_dcache_region_wrapper(ctx, (v_addr_t) &l2_tt_ptr->tt[l2_tt_offset],
                                  (l2_tt_limit - l2_tt_offset) *
                                  sizeof(l2_tt_ptr->tt[l2_tt_offset]));
    }

    /* Determine if all L2TTEs in the given L2TT can be combined into
       one L1TTE */
    check_l2_tte_consolidation(l2_tt_ptr,
                               (block->v_addr + L1TTE2B(l1_tt_ii)));

  }
  mapper_dsb_isb(ctx);

  return E_SUCCESS;
} /* map_using_only_l3_ttes */

/**
   Maps both first and second level translation table entries

   @retval E_SUCCESS Level one and two translation tables populated
                     successfully

   @par Side Effects Existing entries in the tt will be replaced.

   @pre l1.tt must be allocated
   @pre l2.tt must be allocated
 */
static int map_using_l1_ttes
(
  mmu_ctx_t* const ctx,
  const mem_block_t * block
)
{
  const uint64_t pre_l1_sz_kb =
    ((L1_SIZE_MSK & block->v_addr) ?
     B2KB((L1_MAP_SIZE - (L1_SIZE_MSK & block->v_addr))) : 0);

  const uint64_t l1_sz_kb = ((block->sz_in_kb - pre_l1_sz_kb) &
                           L1_KB_ADDR_MSK);

  const uint64_t post_l1_sz_kb = ((block->sz_in_kb - pre_l1_sz_kb) -
                                l1_sz_kb);

  const uint64_t l1_off = KB2B(pre_l1_sz_kb);

  const uint64_t post_l1_off = KB2B(pre_l1_sz_kb + l1_sz_kb);

  const uint32_t num_pre_l1_l2s =
    get_num_l2_tts_needed(ctx,
                          block->v_addr,
                          (0 < pre_l1_sz_kb) ? 1 : 0);

  const uint32_t num_post_l1_l2s =
    get_num_l2_tts_needed(ctx,
                          (block->v_addr + post_l1_off),
                          (0 < post_l1_sz_kb) ? 1 : 0);

  const uint32_t num_l2_tts_req = num_pre_l1_l2s + num_post_l1_l2s;

  const uint32_t num_pre_l1_l3s =
    get_num_l3_tts_needed(ctx,
                          block->v_addr,
                          ((B2KB(L2_MAP_SIZE)) >
                          (pre_l1_sz_kb & L2_KB_SIZE_MSK)) ? 1 : 0);

  const uint32_t num_post_l1_l3s =
    get_num_l2_tts_needed(ctx,
                          (block->v_addr + post_l1_off),
                          ((B2KB(L2_MAP_SIZE)) >
                          (post_l1_sz_kb & L2_KB_SIZE_MSK)) ? 1 : 0);

  const uint32_t num_l3_tts_req = num_pre_l1_l3s + num_post_l1_l3s;

  int retval;

  if (num_l2_tts_req > num_free_tts(ctx, 2) || num_l3_tts_req > num_free_tts(ctx, 3))
  {
    return -E_NO_MEMORY;
  }

  /* Enforcing L1 page size alignment for VA and PA. This is
   * due to the fact that TZ does not have enough memory to
   * do non-aligned mapping.
   *
   * TODO: In platform that support larger memory for mapping,
   * the condition can be moved to mmu_map_block so that
   * the mapping can fallback to TTEs with smaller page size.
   */
  if (0 != (L1_SIZE_MSK & (block->v_addr + l1_off)) ||
      0 != (L1_SIZE_MSK & (block->p_addr + l1_off)))
  {
    return -E_NO_MEMORY;
  }

  /* Don't call map and handle failures when the map size is zero */
  if (pre_l1_sz_kb > 0)
  {
    retval = mmu_map(ctx,
                     block->v_addr,
                     block->p_addr,
                     pre_l1_sz_kb,
                     block->tz_cfg);

    RETCHK(retval);
  }

  /* Don't call map and handle failures when the map size is zero */
  if (post_l1_sz_kb > 0)
  {
    retval = mmu_map(ctx,
                     block->v_addr + post_l1_off,
                     block->p_addr + post_l1_off,
                     post_l1_sz_kb,
                     block->tz_cfg);

    RETCHK(retval);
  }

  return mmu_map(ctx,
                 block->v_addr + l1_off,
                 block->p_addr + l1_off,
                 l1_sz_kb,
                 block->tz_cfg);
}

static int map_using_l2_ttes
(
  mmu_ctx_t* const ctx,
  const mem_block_t * block
)
{
  const uint64_t pre_l2_sz_kb =
    ((L2_SIZE_MSK & block->v_addr) ?
     B2KB((L2_MAP_SIZE - (L2_SIZE_MSK & block->v_addr))) : 0);

  const uint64_t l2_sz_kb = ((block->sz_in_kb - pre_l2_sz_kb) &
                           L2_KB_ADDR_MSK);
  const uint64_t post_l2_sz_kb = ((block->sz_in_kb - pre_l2_sz_kb) -
                                l2_sz_kb);

  const uint64_t l2_off = KB2B(pre_l2_sz_kb);
  const uint64_t post_l2_off = KB2B(pre_l2_sz_kb + l2_sz_kb);

  const uint32_t num_l3_tts_req =
    get_num_l3_tts_needed(ctx,
                          block->v_addr,
                          (pre_l2_sz_kb > 0) ? 1 : 0) +
    get_num_l3_tts_needed(ctx,
                          (block->v_addr + post_l2_off),
                          (post_l2_sz_kb > 0) ? 1 : 0);

  int retval;

  if (num_l3_tts_req > num_free_tts(ctx, 3))
  {
    return -E_NO_MEMORY;
  }

  /* Enforcing L2 page size alignment for VA and PA. This is
   * due to the fact that TZ does not have enough memory to
   * do non-aligned mapping.
   *
   * TODO: In platform that support larger memory for mapping,
   * the condition can be moved to mmu_map_block so that
   * the mapping can fallback to TTEs with smaller page size.
   */
  if (0 != (L2_SIZE_MSK & (block->v_addr + l2_off)) ||
      0 != (L2_SIZE_MSK & (block->p_addr + l2_off)))
  {
    return -E_NO_MEMORY;
  }

  /* Don't call map and handle failures when the map size is zero */
  if (pre_l2_sz_kb > 0)
  {
    retval = mmu_map(ctx,
                     block->v_addr,
                     block->p_addr,
                     pre_l2_sz_kb,
                     block->tz_cfg);

    RETCHK(retval);
  }

  /* Don't call map and handle failures when the map size is zero */
  if (post_l2_sz_kb > 0)
  {
    retval = mmu_map(ctx,
                     block->v_addr + post_l2_off,
                     block->p_addr + post_l2_off,
                     post_l2_sz_kb,
                     block->tz_cfg);

    RETCHK(retval);
  }

  return mmu_map(ctx,
                 block->v_addr + l2_off,
                 block->p_addr + l2_off,
                 l2_sz_kb,
                 block->tz_cfg);
}

/**
   @copydoc mmu_add_block_default_region
 */
int mmu_add_block_default_region
(
  mmu_ctx_t* const ctx,
  const mem_block_t  * map
)
{
  mem_block_t temp;
  int            ii, jj;
  boolean        swap = FALSE;

  if (map == NULL)
  {
    return -E_FAILURE;
  }

  /* Find end of default memory map */
  for (ii = 0;
       (ii < (ctx->dflt_memmap_size - 1)) &&
         (ctx->dflt_memory_map[ii].p_addr != TZ_VAS_LAST_ADDRESS);
       ii++);

  /* Add all block items to the end of the default memory map */
  for (jj = 0;
       (ii < (ctx->dflt_memmap_size - 1)) &&
         (map[jj].p_addr != TZ_VAS_LAST_ADDRESS);
       ii++, jj++)
  {
    memcpy(&ctx->dflt_memory_map[ii], &map[jj], sizeof(mem_block_t));
  }

  /* This would be very bad to hit */
  if (map[jj].p_addr != TZ_VAS_LAST_ADDRESS)
  {
    /* The default configuration structure needs to be larger */
    volatile int aa = 1;
    while (aa);
  }

  /* Sort in order of physical address and range. Order is set first by
   * the physical address, then by the size. */
  for (ii = 0;ii < (ctx->dflt_memmap_size - 1);ii ++)
  {
    /* Last valid entry check */
    if (ctx->dflt_memory_map[ii].p_addr == TZ_VAS_LAST_ADDRESS)
    {
      break;
    }
    for (jj = (ii + 1);jj < (ctx->dflt_memmap_size - 1);jj ++)
    {
      /* Last valid entry check */
      if (ctx->dflt_memory_map[jj].p_addr == TZ_VAS_LAST_ADDRESS)
      {
        break;
      }

      /* Compare the two entries to determine if a swap is required */
      if (ctx->dflt_memory_map[jj].p_addr <= ctx->dflt_memory_map[ii].p_addr)
      {
        /* Start address implies a swap is required. Need to compare the size
         * only if the start addresses are equal. The assumption is that if
         * the start addresses are equal the larger map comes first in the
         * table to prevent the overwriting of smaller maps */
        if (ctx->dflt_memory_map[jj].p_addr == ctx->dflt_memory_map[ii].p_addr)
        {
          if (ctx->dflt_memory_map[ii].sz_in_kb <
              ctx->dflt_memory_map[jj].sz_in_kb)
          {
            swap = TRUE;
          }
        }
        else
        {
          swap = TRUE;
        }

        if (TRUE == swap)
        {
          /* Swap the two entries */
          temp = ctx->dflt_memory_map[ii];
          ctx->dflt_memory_map[ii] = ctx->dflt_memory_map[jj];
          ctx->dflt_memory_map[jj] = temp;
        }
        swap = FALSE;
      }
    }
  }

  return E_SUCCESS;
}

/**
   @copydoc mmu_map
 */
int mmu_map
(
  mmu_ctx_t* const ctx,
  v_addr_t v_addr,
  p_addr_t p_addr,
  size_t   sz_in_kb,
  tz_cfg_t msk
)
{
  mem_block_t pt[2];

  pt[0].p_addr   = p_addr;
  pt[0].v_addr   = v_addr;
  pt[0].sz_in_kb = sz_in_kb;
  pt[0].tz_cfg   = msk;

  pt[1].v_addr   = (v_addr_t) TZ_VAS_LAST_ADDRESS;
  pt[1].p_addr   = (p_addr_t) TZ_VAS_LAST_ADDRESS;

  return mmu_map_block(ctx, pt);
}

#if DEBUG_MAP_CALLS
/* begin logging after MMU is enabled */
extern boolean begin_logging;
extern uint32_t Load$$TZBSP_DP_DDR$$Base;
extern uint32_t Load$$TZBSP_DP_DDR$$Limit;
#endif

/**
   @copydoc mmu_map_block
*/
int mmu_map_block
(
  mmu_ctx_t* const ctx,
  const mem_block_t* map
)
{
  int retval = 0;

  if (map == NULL)
  {
    return -E_FAILURE;
  }

  while ( map->v_addr != TZ_VAS_LAST_ADDRESS )
  {
    p_addr_t paddr  = map->p_addr;
    v_addr_t vaddr  = map->v_addr;
    size_t   len_kb = map->sz_in_kb;

#if DEBUG_MAP_CALLS
    if (begin_logging)
    {
      if (vaddr < 0x80000000)
      {
        uint32_t dp_base = ( (unsigned int)&Load$$TZBSP_DP_DDR$$Base );
        uint32_t dp_end  = ( (unsigned int)&Load$$TZBSP_DP_DDR$$Limit );
        if (vaddr < dp_base || vaddr > dp_end)
        {
          if ((map->tz_cfg & TZ_INVALID_MSK) == TZ_TTE_VALID)
          {
            TZBSP_LOG(0, "mapping 0x%08x++0x%08x as valid", vaddr, len_kb<<10);
          }
          else
          {
            TZBSP_LOG(0, "mapping 0x%08x++0x%08x as invalid", vaddr, len_kb<<10);
          }
        }
      }
    }
#endif

    /* unaligned pointer */
    if ((FLOOR_4K(vaddr) != vaddr) || (FLOOR_4K(paddr) != paddr))
    {
      return -E_BAD_ADDRESS;
    }

    /* length must be a page multiple */
    if (0 != (len_kb & 0x3))
    {
      return -E_OUT_OF_RANGE;
    }

    /* Check the address first to return -E_BAD_ADDRESS if the address
       starts beyond the addressable range. */
    if (vaddr > TZ_VAS_LAST_ADDRESS || paddr > TZ_VAS_LAST_ADDRESS)
    {
      return -E_BAD_ADDRESS;
    }

    /* Confirm the mapping length request is below the maximum
       acceptable range.  Also makes sure the length can be converted
       to bytes without overflowing.  Also makes sure that there is no
       overflow when calculating the end address.  End address is
       inclusive and length is exclusive. */
    if (len_kb > (size_t) B2KB((uint64_t) TZ_VAS_LAST_ADDRESS + 1))
    {
      return -E_OUT_OF_RANGE;
    }

    /* Check that the mapping does not go beyond the addressable
       range. Last address is inclusive and end address is
       exclusive */
    if (vaddr + KB2B(len_kb) > (v_addr_t) TZ_VAS_LAST_ADDRESS + 1 ||
        paddr + KB2B(len_kb) > (p_addr_t) TZ_VAS_LAST_ADDRESS + 1)
    {
      return -E_OUT_OF_RANGE;
    }

    /* Determine if this request can be handled with only L1 TTEs */
    if (0 == (L1_SIZE_MSK & vaddr) &&
        0 == (L1_SIZE_MSK & paddr) &&
        0 == (len_kb & L1_KB_SIZE_MSK))
    {
      map_using_only_l1_ttes(ctx, map);
    }

    /* Determine if part of this request can be handled with L1 TTEs */
    else if (((B2KB(vaddr) + len_kb) & L1_KB_ADDR_MSK) >
             ((B2KB(vaddr) + B2KB(L1_MAP_SIZE) - 1) & L1_KB_ADDR_MSK))
    {
      retval = map_using_l1_ttes(ctx, map);

      RETCHK(retval);
    }

    /* Determine if the request can be completed only with L2 TTEs */
    else if (0 == (L2_SIZE_MSK & vaddr) &&
             0 == (L2_SIZE_MSK & paddr) &&
             0 == (len_kb & L2_KB_SIZE_MSK))
    {
      retval = map_using_only_l2_ttes(ctx, map);

      RETCHK(retval);
    }

    /* Determine if part of this request can be handled with L2 TTEs */
    else if (((B2KB(vaddr) + len_kb) & L2_KB_ADDR_MSK) >
             ((B2KB(vaddr) + B2KB(L2_MAP_SIZE) - 1) & L2_KB_ADDR_MSK))
    {
      retval = map_using_l2_ttes(ctx, map);

      RETCHK(retval);
    }

    /* The request can only be completed with only L3 TTEs */
    else
    {
      retval = map_using_only_l3_ttes(ctx, map);

      RETCHK(retval);
    }

    map++;
  }

  return E_SUCCESS;
}

/**
   Zeros out all entries in the mmu translation table.  Having an
   entry of type 0 raises an exception.

   @retval None

   @par Side Effects
   All entries in the tt are invalidated.  If the mmu is enabled this
   will raise a section translation exception each time memory access is tried

   @dep l1_tt must be allocated
*/
void map_all_ttes_as_invalid
(
  mmu_ctx_t* const ctx
)
{
  if(!ctx->l1_invalidate_done)
  {
    inval_tt_lvl(ctx, 1, 0);
    ctx->l1_invalidate_done = 1;
  }
  inval_tt_lvl(ctx, 2, 0);
  inval_tt_lvl(ctx, 3, 0);
}

