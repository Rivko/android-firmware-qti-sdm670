/*===========================================================================

  page_select.c - handle setting page select register for LPAE targets

  Copyright (c) 2013, 2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#include "page_select.h"
#include "comdef.h"
#include <stdlib.h>
#include "CoreVerify.h"
#include "msmhwio.h"
#include "aop_hwio.h"

static uint32 refcount = 0;

uint32_t set_ddr_access(uint64_t address) __attribute__((noinline))
{
    /* Handle DDR paged memory addressing */
    // bits 31-34 are used to set the page select bits
  uint32 out = (uint32_t)((address)>>30);
  uint32 ret = (uint32) address;

  if (out)
  {
    HWIO_AOP_PAGE_SELECT_OUT(out);

    // truncate address down to 30 bits,
    // truncate to 1GB address range
    // offset to 0 at 0x60000000 - DDR is mapped starting at 0x60000000 on LPAE targets
    ret = ((((uint32_t)address) & 0x3FFFFFFF) + 0x60000000);

    refcount ++;
  }

  return ret;
  //return (void*)(((uint32)smem_imem_entry->smem_base_phys_addr)+0x60000000);
}

void set_page_select(uint8_t page_select_val) __attribute__((noinline))
{
    CORE_VERIFY(page_select_val < 64);
    uint32 val = HWIO_AOP_PAGE_SELECT_IN;

    if (val > 0)
    {
      if (page_select_val == 0)
      {
        --refcount;
      }
      else
      {
        ++refcount;
      }
    }
    
    if (refcount == 0)
    {
      // directly set the page select value
      HWIO_AOP_PAGE_SELECT_OUT(page_select_val);
    }
}

