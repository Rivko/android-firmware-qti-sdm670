/*====================================================================
 *
 * FILE:        flash_dal_util.c
 *
 * SERVICES:    Utility functions for flash DAL
 *
 * DESCRIPTION: Utility functions for flash DAL
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/05/12     sv      Add functions for cache maintenance
 * 05/21/11     bb      Flash driver Optimization
 * 10/21/10     sv      Add flash_busy_wait API 
 * 08/11/10     nr      Added stats framework support
 * 07/21/09     sv      Modified to use DALSYS_memcpy
 * 07/14/09     eo      Moved flash_extract_uuid() to this file
 * 05/11/09     bb      Support for babylon
 * 03/24/09     bb      Added safe string copy function
 * 11/26/08     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_dal_util.h"

/* Instance creation for holding flash debug info */
struct flash_debug_info flash_dbg;

/* Entries for virtual to physical address conversion */
#define FLASH_MAX_VTOP_ENTRIES  2

struct vtop_item
{
  uint32 vstart;   /* Start of the virtual addr */
  uint32 size;     /* Virtual address range     */
  int32  pstart;   /* Start of the physical addr */
};

struct vtop_list
{
  uint32 no_of_entries;
  struct vtop_item vtop_entry[FLASH_MAX_VTOP_ENTRIES];
};

static struct vtop_list virt_phys_list;
/*
 * This API resets number of entries in the list to 0
 */
void flash_vtop_init_list()
{
  virt_phys_list.no_of_entries = 0;
}

/*
 * This API resets number of entries in the list to a specific value
 */
void flash_vtop_set_list_count(uint32 count)
{
  virt_phys_list.no_of_entries = count;
}

/*
 * This API adds an entry to the list
 */
void flash_vtop_add_item(uint32 vaddr, uint32 phys_addr, uint32 size)
{
  struct vtop_item *vitem;

  /* Ensure we have not exceeded the max number of entries possible */
  if (FLASH_MAX_VTOP_ENTRIES > virt_phys_list.no_of_entries)
  {
    vitem = &virt_phys_list.vtop_entry[virt_phys_list.no_of_entries];

    vitem->vstart = vaddr;
    vitem->size = size;
    vitem->pstart = phys_addr;

    virt_phys_list.no_of_entries++;
  }
  else
  {
    FLASHLOG(3, ("Exceeded FLASH_MAX_VTOP_ENTRIES\n"));
  }
}

/*
 * This API looks for the entry that matches within which lies the
 * virt_addr and returns the corresponding physical address
 */
uint32 flash_vtop_get_physical(uint32 virt_addr)
{
  uint32 item_count = 0;
  uint32 phy_address = 0;
  struct vtop_item *vitem;

  while (item_count < virt_phys_list.no_of_entries)
  {
    vitem = &virt_phys_list.vtop_entry[item_count];

    /* Check if vitem has a valid start address.
       if so, determine the virtual address range in which virt_addr
       belongs.  Address range for each item is calculated as
       vitem->vstart  ....   (vitem->vstart + vitem->size)
    */
    if (((uintnt)vitem->vstart != DALSYS_MEM_ADDR_NOT_SPECIFIED) &&
        (virt_addr >= vitem->vstart) &&
        (virt_addr < (vitem->vstart + vitem->size)))
    {
      /* calculate the phycial addr of virt_addr using the offset
         (virt_addr-vitem->vstart) from vitem->pstart
      */
      phy_address = vitem->pstart + (virt_addr-vitem->vstart);
    }
    item_count++;
  }

  return phy_address;
}

/*
 * Safe string copy function for flash
 */
void flash_strcpy(uint8 *dest, const uint8 *src, uint32 len)
{
  DALSYS_memcpy(dest, src, len);
  dest[len-1] ='\0';
}

/*
 *  This function does a wait for a desired time in us.
 */
void flash_busy_wait(uint32 wait_time_us)
{
  DALSYS_BusyWait(wait_time_us);
}

/* This function does cache invalidation on the given address */
void flash_dcache_clean(uint32 addr, uint32 len)
{
  DALSYS_CacheCommand(DALSYS_CACHE_CMD_CLEAN, (uint32)addr, len);
}

/* This function does cache clean on the given address */
void flash_dcache_inval(uint32 addr, uint32 len)
{
  DALSYS_CacheCommand(DALSYS_CACHE_CMD_INVALIDATE, (uint32)addr, len);
}
