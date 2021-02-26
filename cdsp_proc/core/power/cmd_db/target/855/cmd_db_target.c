/*===========================================================================
                             cmd_db_target.c

DESCRIPTION:
  Target specific functions for command DB in client image.

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.
QUALCOMM Proprietary.  Export of this technology or software is regulated
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/


//===========================================================================
//                   Includes and Public Data Declarations
//===========================================================================

//---------------------------------------------------------------------------
// Include Files
//------------------- --------------------------------------------------------
#include "cmd_db_int.h"
#include "qurt_memory.h"
#include "CoreVerify.h"
#include "msmhwiobase.h"

//---------------------------------------------------------------------------
// Constant / Define Declarations
//---------------------------------------------------------------------------
#define DRV_ID 7
#define AOP_INFO_ADDR (AOP_SS_MSG_RAM_START_ADDRESS_BASE + DRV_ID * 0x10000)
#define CMD_DB_AOP_DATA_ADDR (AOP_INFO_ADDR + 0x0c)
#define CMD_DB_AOP_DATA_SZ   (AOP_INFO_ADDR + 0x10)
#define ROUND_4k(N) (( (N) + 4095 ) & (~0xFFF))
//---------------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Forward Declarations
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Exeternal Declarations
//---------------------------------------------------------------------------

char *cmd_db_addr = NULL;

//===========================================================================
//                             Function Definitions
//===========================================================================
/**
 * <!-- cmd_db_pre_db_access -->
 *
 * @brief Prepare image for db data access.
 *
 *  @return None.
 *
 */
void cmd_db_prep_access(void)
{
  return;
}

/**
 * <!-- cmd_db_access_done -->
 *
 * @brief Prepare image for db data access.
 *
 *  @return None.
 *
 */
void cmd_db_access_done(void)
{
  return;
}

/**
 * <!-- cmd_db_target_mem_init -->
 *
 * @brief Return target address for Command DB data.
 *
 *  @return Command DB data address for success, NULL for
 *          failure.
 *
*/
char *cmd_db_target_mem_init(void)
{
  qurt_mem_region_t       mem_region;
  qurt_mem_region_attr_t  region_attr;
  unsigned int            virt_addr;
  int                     ret;
  qurt_addr_t page_paddr, page_vaddr;
  qurt_mem_cache_mode_t cacheability;


  char *phys_addr = (char*)(uint64_t)(*((uint32_t *)CMD_DB_AOP_DATA_ADDR));
  cmd_db_addr = phys_addr;
  uint32_t size = (uint32_t)(*((uint32_t*)CMD_DB_AOP_DATA_SZ));
  size = ROUND_4k(size);

  if (!phys_addr)
    return NULL;

  /* Create memory region first */
  qurt_mem_region_attr_init(&region_attr);
  qurt_mem_region_attr_set_mapping(&region_attr, QURT_MEM_MAPPING_NONE);
  qurt_mem_region_attr_set_physaddr(&region_attr, (qurt_paddr_t)phys_addr);
  ret = qurt_mem_region_create(&mem_region, (qurt_size_t)size,
                               qurt_mem_default_pool, &region_attr);

  CORE_VERIFY(ret == QURT_EOK);

  /* Reretrieve attributes struct since qurt_mem_region_create may have altered
   * the base addresses or size to fit page alignment (hopefully alignment is
   * unnecessary). */
  ret = qurt_mem_region_attr_get(mem_region, &region_attr);
  CORE_VERIFY(ret == QURT_EOK);
  qurt_mem_region_attr_get_virtaddr(&region_attr, &virt_addr);

  /* Now map the region */
  page_paddr = (qurt_addr_t)phys_addr;
  page_vaddr = (qurt_addr_t)virt_addr;
  /* I don't always cache, but when I do, I prefer Writethrough.
   * (Writethrough makes more sense for an interprocessor usecase.) */
  cacheability = QURT_MEM_CACHE_NONE_SHARED;

  /* Map this page's virtual to physical address, making sure to keep the same
   * offset in order to maintain contiguity.  Permissions should be "R". */
  ret = QURT_EOK;
  if (qurt_lookup_physaddr(page_vaddr) == 0)
  {
      ret = qurt_mapping_create(page_vaddr, page_paddr, size, cacheability,
                               QURT_PERM_READ);
  }
  CORE_VERIFY(ret == QURT_EOK);

  return (char *)virt_addr;
}
