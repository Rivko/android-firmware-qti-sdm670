
/*===========================================================================
Copyright (c) 2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.
============================================================================*/
#include <stddef.h>
#include <stdint.h>
#include "teetest.h"
#include "teetest_mmu.h"
#include "teetest_hal_qgic.h"

#include "IxErrno.h"

/* Align val down to be a multiple of size, size needs to be a power of two */
#define ALIGN_DOWN(val, size) ((val) & ~((size) - 1))

/* Align val up to be a multiple of size, size needs to be a power of two */
#define ALIGN_UP(val, size)   (((val) + (size) - 1) & ~((size) - 1))

/*===========================================================================
 *  Function :  enable_el1_mmu
 * ==========================================================================
 */
/**
 * Called during init to program MMU related system registers and
 * enable the MMU
 *
 * @param ttbr0 Translation table base registeer
 * @param nmrr  Memory attribute register
 * @param prrr  Memory attribute register
 */
void enable_el1_mmu(uintnt ttbr0, uintnt nmrr, uintnt prrr);
static void mmu_map(unsigned int v_addr, unsigned int p_addr, unsigned int len, unsigned int cfg);

/*
* Stack variables and access functions for teetest_entry.s
*/
static unsigned char __attribute__((section("SVC_STACK"),aligned(4096))) svc_stack[0x8000];
static unsigned char __attribute__((section("UND_STACK"),aligned(4096))) und_stack[0x1000];
static unsigned char __attribute__((section("ABT_STACK"),aligned(4096))) abt_stack[0x1000];
static unsigned char __attribute__((section("FIQ_STACK"),aligned(4096))) fiq_stack[0x1000];
static unsigned char __attribute__((section("IRQ_STACK"),aligned(4096))) irq_stack[0x1000];

/* Get the stacks as relative to the current PC */
unsigned int get_svc_stack(void) {
  return ((unsigned int)&svc_stack + sizeof(svc_stack));
}

unsigned int get_und_stack(void) {
  return ((unsigned int)&und_stack + sizeof(und_stack));
}

unsigned int get_abt_stack(void) {
  return ((unsigned int)&abt_stack + sizeof(abt_stack));
}

unsigned int get_fiq_stack(void) {
  return ((unsigned int)&fiq_stack + sizeof(fiq_stack));
}

unsigned int get_irq_stack(void) {
  return ((unsigned int)&irq_stack + sizeof(irq_stack));
}

/* Assign 32kB for the L1 table. A 16kB aligned pointer will
 * be assigned to this table at run time. */
uint8_t  l1_region[32*1024];
l1_tt_t  *l1;
l2_tt_t   __attribute__((section("L2_TT_MEM"),aligned(4096))) l2[NUM_L2_TTS];

static inline uintnt get_ttbr_mem_attrs(void) { return TTBR0_MEM_ATTS; }
static inline uintnt get_el1_nmrr      (void) { return NMRR;           }
static inline uintnt get_el1_prrr      (void) { return PRRR;           }

static volatile uintnt ttbr_mem_attrs = 0;
static volatile uintnt el1_nmrr = 0;
static volatile uintnt el1_prrr =  0;

static int       cur_num_l2_tts = 0;

extern void clean_and_inval_dcache_region(uint32_t vaddr, uint32_t len);

static uint32_t get_num_ttes(uint32_t level, uint32_t v_addr, uint32_t size_kb) {
  if (0 == size_kb || level > 2)
    return 0;

  switch (level) {
  case 1:
    return KB2NUML1TTES(B2KB(v_addr) + size_kb - 1) - B2NUML1TTES(v_addr) + 1;
  case 2:
    return KB2NUML2TTES(B2KB(v_addr) + size_kb - 1) - B2NUML2TTES(v_addr) + 1;
  }
  return 0;
}

static l2_tt_t* l1_walk(sd_tte_t l1_tte) {
  return (l2_tt_t *) (uint32_t)(l1_tte & 0xFFFFFC00);
}

/*
   Checks the L1 entries at the given address to see how many already
   point to allocated level two translation tables and how many need
   new allocations

   retval number of level two translation tables needed for the given
          virtual address and number of 1 MB sections
*/
static int get_num_l2tts_needed(
  const uint32_t v_addr, 
  const uint32_t num_l1ttes) {
  uint32_t    l1tte_ii, tt_ii;
  int       num_l2tts_needed = 0;
  sd_tte_t *l1_tt_ptr        = &l1->tt[0];

  for (l1tte_ii = 0; l1tte_ii < num_l1ttes; l1tte_ii++) {
    tt_ii = B2L1TTE(v_addr) + l1tte_ii;

    /* Does this L1 TTE already point to an allocated L2 TTE? */
    if ((*(l1_tt_ptr + tt_ii) & 0x3) != L1_SECONDARY_TTE_DESCRIPTOR) {
      num_l2tts_needed++;
    }
  }
  return num_l2tts_needed;
}

static int num_l2tts_available(void)
{
  return (NUM_L2_TTS - cur_num_l2_tts);
}

static l2_tt_t* l2tt_iter_start(l2tt_iter_t* iter) {
  *iter = 0;
  return &l2[0];
}

static l2_tt_t* l2tt_iter_next(l2tt_iter_t* iter) {
  (*iter)++;

  if (*iter < ARRAY_SIZE(l2)) {
    return &l2[*iter];
  } else {
    return NULL;
  }
}

static boolean is_l2tt_used(l2_tt_t* tt_ptr) {
  /* Not currently using large L2 pages, so this is our "free" indication */
  return (((uint32_t) tt_ptr->tt[0] & 0x3) != L2_LARGE_PAGE_DESCRIPTOR);
}

static int alloc_l2tt(l2_tt_t** l2tt_ptr_ptr) {
  l2_tt_t*    tt_ptr = 0;
  l2tt_iter_t iter;

  /* Any available memory for L2 pages? */
  if (num_l2tts_available() == 0) {
    /* Unrecoverable error */
    tee_panic();
  }

  /* Loop through first page entry in each L2 remap area and look for
   * the first available remap area */
  for (tt_ptr = l2tt_iter_start(&iter); tt_ptr != NULL; tt_ptr = l2tt_iter_next(&iter)) {
    if (!is_l2tt_used(tt_ptr)) {
      /* Found an open slot */
      *l2tt_ptr_ptr = tt_ptr;
      cur_num_l2_tts++;
      return E_SUCCESS;
    }
  }

  /* Unrecoverable error */
  tee_panic();

  /* Shouldn't get here */
  return E_FAILURE;
}

/**
   For a given virtual address, this function will retrieve the level
   two translation table address, if the level one translation table
   was programmed to point to one, or it will return a newly allocated
   level two translation table and return the L1 translation table
   configuration, translated to L2 format and the old physical address
   mapping of the L1 translation table.

   @retval E_SUCCESS Level one and two translation tables populated
                     successfully

   @pre l1 must be allocated
   @pre l2 must be allocated
*/
static void get_or_make_l2tt (uint32_t    v_addr,    l2_tt_t** l2_tt_ptr_ptr,
                              uint32_t*   l1_p_addr, boolean*  newl2tt) {
  uint32_t    tt_ii     =  B2L1TTE(v_addr);
  sd_tte_t *l1_tt_ptr = &l1->tt[0];

  if ((*(l1_tt_ptr + tt_ii) & 0x3) == L1_SECONDARY_TTE_DESCRIPTOR) {
    *l2_tt_ptr_ptr = l1_walk(*(l1_tt_ptr + tt_ii));
    *l1_p_addr     = 0;
    *newl2tt       = FALSE;
  } else {
    if (alloc_l2tt(l2_tt_ptr_ptr) != E_SUCCESS) {
      /* Error scenario */
      tee_panic();
    }
    *newl2tt       = TRUE;
    *l1_p_addr     = (*(l1_tt_ptr + tt_ii) & 0xFFF00000);
  }
}

#define DEFAULT_MEM_ATTR   0x17
#define INIT_MEM_ATTR      0x1E

/* API for other modules to use, maps with default attributes*/
void tee_mmu_map(uintptr_t addr, size_t sz_in_kb){
   mmu_map(
      addr,
      addr,
      sz_in_kb,
      DEFAULT_MEM_ATTR
   );   
}

/* For simplicity, map everything with 4KB pages */
static void mmu_map(
  unsigned int v_addr,
  unsigned int p_addr,
  unsigned int len,
  unsigned int cfg) {

  const uint32_t num_l1ttes    = get_num_ttes(1, v_addr, len);
  const uint32_t num_l2ttes    = get_num_ttes(2, v_addr, len);
  const uint32_t num_l2tts_req = get_num_l2tts_needed(v_addr, num_l1ttes);

  if (num_l2tts_req > num_l2tts_available()) {
    /* Error condition */
    tee_panic();
  }

  sd_tte_t *l1_tt_ptr = &l1->tt[0];
  l2_tt_t  *l2_tt_ptr;
  uint32_t    l1_p_addr;
  uint32_t    tt_offset;
  uint32_t    tt_base;
  uint32_t    tt_limit;
  uint32_t    tt_ii;
  boolean   newl2tt;
  
  tt_base  = B2L1TTE(v_addr);
  tt_limit = tt_base + num_l1ttes;

  /* First initialize the first level descriptor for each 1 MB High 22
     bits provide the physical base address of the level 2 translation
     table */

  /* L1 TTE for Page Diagram
      _______________________________________________________________________
     |                                                  | |        | |   |   |
     |31----------------------------------------------10|9|8------5|4|3 2|1 0|
     |         level 2 PT Address base                  |0| DOMAIN |0|0 0|0 1|
     |__________________________________________________|_|________|_|___|___|
  */

  /* We'll have to do each step for every l1 translation table entry
     that gets replaced with a pointer to a l2 translation table
   */
  for (uint32_t l1tt_ii = 0; l1tt_ii < num_l1ttes; l1tt_ii++) {
    get_or_make_l2tt( v_addr + L1TTE2B(l1tt_ii), &l2_tt_ptr, 
                     &l1_p_addr,                 &newl2tt);

    /* The given mapping may not begin at the 1 MB boundary for the
       first L2 TTE of the given memory block.  Calculate the l2
       translation table index for the start of the given mapping. */
    tt_offset = (l1tt_ii == 0) ? B2L2TTE(v_addr) : 0;

    /* If there are multiple L2 TTs needed, all L2 TTs will iterate to
       the end except the last one */
    tt_limit = (l1tt_ii == (num_l1ttes - 1)) ?
      B2L2TTE(v_addr) + num_l2ttes - NUM_L2_TTES_PER_L1_TTE * l1tt_ii :
      NUM_L2_TTES_PER_L1_TTE;

    /* Now we need to program the L2 translation table */
    /* Page Diagram
        _____________________________________________________________________
       |                               |  |  |     |     |       |  |  |  |  |
       |31---------------------------12|11|10|--9--|8---6|5-----4|-3|-2|-1|-0|
       |     Phys Address base         |nG| S|AP[2]| TEX |AP[1:0]| C| B| 1|XN|
       |_______________________________|__|__|_____|_____|_______|__|__|__|__|
    */

    /* Iterate through all the 4KB pages in the 1MB section */
    for (tt_ii = 0; tt_ii < NUM_L2_TTES_PER_L1_TTE; tt_ii++) {
      /* If the index lies between the base index and the limit, then
         this is the given section we need to map */
      if ((tt_offset <= tt_ii) && (tt_ii < tt_limit)) {
        l2_tt_ptr->tt[tt_ii] = (((v_addr & L1_ADDR_MSK) +
                                 L2TTE2B(tt_ii) + L1TTE2B(l1tt_ii)) | cfg);

        /* Invalidate TLB for each entry as it's updated */
        asm ("dsb sy \n mcr p15,0,%0,c8,c3,1" :: "r"
             ((uint64_t) ((v_addr & L1_ADDR_MSK) + L2TTE2B(tt_ii) +
                        L1TTE2B(l1tt_ii)) >> 12));
      }
      /* If this is a newly mapped L2 TT, TLB invalidation is done below
         when the L1 is changed to a L2 pointer. */
      else if (newl2tt) {
        l2_tt_ptr->tt[tt_ii] = 0;
      }
    }

    /* Enter the L2 base address and mark the descriptor for L2
     * translation table and default L1 TTE for L2 TTE info.
     */
    if (newl2tt) {
      uint32_t tt_ii_local = B2L1TTE(v_addr + L1TTE2B(l1tt_ii));

      /* Write entire new L2TT to DDR */
      clean_and_inval_dcache_region((v_addr_t) l2_tt_ptr, sizeof(*l2_tt_ptr));

      *(l1_tt_ptr + tt_ii_local) =
        ((uint32_t) l2_tt_ptr | L1_DEFAULT_DOMAIN | L1_SECONDARY_TTE_DESCRIPTOR);
      asm ("dsb sy \n mcr p15,0,%0,c8,c3,1" :: "r"
           ((uint64_t) (L1TTE2B(tt_ii_local) >> 12)));
      clean_and_inval_dcache_region((v_addr_t) &*(l1_tt_ptr + tt_ii_local),
                                    sizeof(*(l1_tt_ptr + tt_ii)));
    } else {
      /* Write only changes out to DDR */
      clean_and_inval_dcache_region((v_addr_t) &l2_tt_ptr->tt[tt_offset],
                                    (tt_limit - tt_offset) *
                                    sizeof(l2_tt_ptr->tt[tt_offset]));
    }
  }

  asm ("dsb sy \n isb"::);
}

/*
 * Convenience wrapper for mmu_map 
 * Aligns the address to 4KB boundary
 */
#define TZ_VAS_LAST_ADDRESS (VAS_SIZE - 1)
static void mmu_map_wrapper(
  uintptr_t addr,
  size_t    size,
  uint32_t  msk
)
{
  /* Check for zero size or address overflow */
  if(!size || (TZ_VAS_LAST_ADDRESS - size + 1 < addr)){ 
    return;
  }
  
  uintptr_t align_start = ALIGN_DOWN(addr, SIZE_4KB);
  size_t align_size  = B2KB(ALIGN_UP((addr - align_start) + size, SIZE_4KB));
  
  (void)mmu_map(
    (v_addr_t)align_start,
    (p_addr_t)align_start,
    align_size,
    msk
  );
}

/*
 * Sets all page table elements to unused
 */
static void init_pgtables(void)
{
  size_t i = 0;
  size_t j = 0;
  
  /* L1 table setup. Since this code is relocatable we must
   * ensure that the L1 table pointer is on a 16KB boundary */
  l1 = (l1_tt_t *)ALIGN_UP((uintptr_t)l1_region, KB2B(16));
    
  /* Set up the L2 tables */
  for (i = 0; i < NUM_L2_TTS; i++) {
    for (j = 0; j < NUM_L2_TTES_PER_L1_TTE; j++) {
      /* Not currently using large L2 pages, so this is our "free" indication */
      l2[i].tt[j] = L2_LARGE_PAGE_DESCRIPTOR;
    }
  }
}

/* Set up any structures and variables required */
void tee_mmu_init(uintptr_t tee_phy_addr, size_t tee_len, 
                  uintptr_t ipc_addr,     size_t ipc_len) 
{
  init_pgtables();
  
  /* First map the entire memory region as
   *
   *   normal memory
   *   innner write through, no write alloc
   *   outer  write through, no write alloc
   *
   *     _____________________________________________________________________
   *    |                               |  |  |     |     |       |  |  |  |  |
   *    |31---------------------------12|11|10|--9--|8---6|5-----4|-3|-2|-1|-0|
   *    |     Phys Address base         |nG| S|AP[2]| TEX |AP[1:0]| C| B| 1|XN|
   *    |     Phys Address base         |0 | 0|  0  | 000 |  01   | 1| 1| 1| 0|
   *    |_______________________________|__|__|_____|_____|_______|__|__|__|__|
   */
  mmu_map_wrapper (tee_phy_addr, tee_len, INIT_MEM_ATTR);
  
  /* Now remap the translation table regions as
   *
   *   normal memory
   *   non-cacheable
   *   xn
   *
   *     _____________________________________________________________________
   *    |                               |  |  |     |     |       |  |  |  |  |
   *    |31---------------------------12|11|10|--9--|8---6|5-----4|-3|-2|-1|-0|
   *    |     Phys Address base         |nG| S|AP[2]| TEX |AP[1:0]| C| B| 1|XN|
   *    |     Phys Address base         |0 | 0|  0  | 000 |  01   | 0| 1| 1| 1|
   *    |_______________________________|__|__|_____|_____|_______|__|__|__|__|
   */
  mmu_map_wrapper ((uintptr_t)l1,  sizeof(l1_tt_t), DEFAULT_MEM_ATTR );
  mmu_map_wrapper ((uintptr_t)&l2, sizeof(l2),      DEFAULT_MEM_ATTR );
  
  /* Map in the IPC memory as per the translation table regions */
  mmu_map_wrapper (ipc_addr, ipc_len, DEFAULT_MEM_ATTR );

  /* Map in the QGIC */
  mmu_map_wrapper (HWIO_GICD_SGIR_ADDR, SIZE_1MB, DEFAULT_MEM_ATTR);
  
  ttbr_mem_attrs = get_ttbr_mem_attrs();
  el1_nmrr = get_el1_nmrr();
  el1_prrr = get_el1_prrr();

  /* Enable the MMU */
  enable_el1_mmu((uintptr_t)l1 | ttbr_mem_attrs,
                 el1_nmrr,
                 el1_prrr);
}

