/**
   @file cache_ops.c
   @brief Implements ARM architected cache API's for TZ 
*/
/*===========================================================================
   Copyright (c) 2014-2017 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE

  $Header: //components/rel/ssg.tz/1.0.2/securemsm/trustzone/qsapps/teetest/teetest/src/aarch64/cache_ops.c#1 $
  $DateTime: 2018/02/06 03:00:17 $
  $Author: pwbldsvc $


when       who      what, where, why
--------   ---     ------------------------------------------------------------
01/24/17   fm      Initial revision.
=============================================================================*/

#include <stdint.h>

#include "cache.h"
#include "teetest.h"
#include "teetest_mmu.h"

#define TZBSP_MIN(a,b) ((a)>(b)?(b):(a))

typedef void (*cache_op)(uintptr_t addr);

static size_t get_dcache_granule_size(void)
{
  uint64_t ctr_el0;
  __asm volatile("mrs %0, CTR_EL0" : "=r" (ctr_el0) :);
  return 4 << ((ctr_el0 >> 16) & 0xf);
}

static size_t get_icache_granule_size(void)
{
  uint64_t ctr_el0;
  __asm volatile("mrs %0, CTR_EL0" : "=r" (ctr_el0) :);
  return 4 << (ctr_el0 & 0xf);
}

static void clean_dcache_granule(uintptr_t addr)
{
  __asm volatile("dc cvac, %0" : : "r" (addr));
}

static void clean_inval_dcache_granule(uintptr_t addr)
{
  __asm volatile("dc civac, %0" : : "r" (addr));
}

static void inval_dcache_granule(uintptr_t addr)
{
  __asm volatile("dc ivac, %0" : : "r" (addr));
}

static void clean_dcache_granule_pou(uintptr_t addr)
{
  __asm volatile("dc cvau, %0" : : "r" (addr));
}

static void inval_icache_granule(uintptr_t addr)
{
  __asm volatile("ic ivau, %0" : : "r" (addr));
}

static void unify_cache_granule(uintptr_t addr)
{
  inval_icache_granule(addr);
  clean_dcache_granule_pou(addr);
}

static void do_dcache_op_by_addr(uintptr_t addr, size_t length, cache_op op, size_t granule_size)
{
  uintptr_t start_addr = addr;
  
  if(0 == length)
  {
    return;
  }
  
  __asm volatile("dsb sy" : : );
  for(addr &= ~(granule_size - 1); addr < start_addr + length; addr += granule_size)
  {
    op(addr);
  }
  __asm volatile("dsb sy" : : );
}

void clean_and_inval_dcache_region(v_addr_t vaddr, size_t len)
{
  do_dcache_op_by_addr(vaddr, len, clean_inval_dcache_granule, get_dcache_granule_size());
}

void inval_dcache_region(v_addr_t vaddr, size_t len)
{
  do_dcache_op_by_addr(vaddr, len, inval_dcache_granule, get_dcache_granule_size());
}

void clean_dcache_region(v_addr_t vaddr, size_t len)
{
  do_dcache_op_by_addr(vaddr, len, clean_dcache_granule, get_dcache_granule_size());
}

void clean_dcache_region_to_pou(v_addr_t vaddr, size_t len)
{
  do_dcache_op_by_addr(vaddr, len, clean_dcache_granule_pou, get_dcache_granule_size());
}

void inval_icache_region(v_addr_t vaddr, size_t len)
{
  do_dcache_op_by_addr(vaddr, len, inval_icache_granule, get_icache_granule_size());
}

void unify_caches(v_addr_t vaddr, size_t len)
{
  size_t granule_size = TZBSP_MIN(get_icache_granule_size(), get_dcache_granule_size());
  do_dcache_op_by_addr(vaddr, len, unify_cache_granule, granule_size);
}

void tzbsp_dcache_clean_region(void *addr, size_t length)
{
  clean_dcache_region((v_addr_t)addr,length);
}
void tzbsp_dcache_inval_region(void *addr, size_t length)
{
  inval_dcache_region((v_addr_t)addr,length); 
}
void tzbsp_dcache_flush_region(void *addr, size_t length)
{
  clean_and_inval_dcache_region((v_addr_t)addr,length);
}

