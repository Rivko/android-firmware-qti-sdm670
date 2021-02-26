/*===========================================================================
                             msg_ram_layout.c

DESCRIPTION:
    This file implements message ram access functions

Copyright (c) 2016 QUALCOMM Technologies, Inc. (QTI). All Rights Reserved.  
QUALCOMM Proprietary.  Export of this technology or software is regulated 
by the U.S. Government. Diversion contrary to U.S. law prohibited.
===========================================================================*/

#include "msg_ram_layout.h"
#include "msg_ram_sections.h"
#include "CoreVerify.h"
#include "time_service.h"

#define AOP_BOOT_COOKIE 0xA0C00C1E

extern msg_ram_sec_t msg_ram_sections[MSG_RAM_MAX]; 

/**
 * <!-- msg_ram_get_mem_area -->
 *
 * @brief Return the pointer to a msg ram memory area 
 *
 * @param section : which memory section we want to retrieve
 */ 
void *msg_ram_get_mem_area(msg_ram_section_usage_enum section)
{
  CORE_VERIFY(section < MSG_RAM_MAX);	
  return msg_ram_sections[section].start;
}

uint32 msg_ram_get_wdog_cookie()
{
  aop_info_0_t *info = (aop_info_0_t*)msg_ram_sections[MSG_RAM_AOP_INFO_0].start;
  return info->wdog_cookie;
}

void msg_ram_set_wdog_cookie(uint32 cookie_val)
{
  aop_info_0_t *info = (aop_info_0_t*)msg_ram_sections[MSG_RAM_AOP_INFO_0].start;
  info->wdog_cookie = cookie_val;
}

/**
 * <!-- msg_ram_set_init -->
 *
 * @brief Initialize the dictionary and sleep stats types 
 */ 
void msg_ram_init(void)
{
  //initialize the dictionary. AOP boot cookie and sleep stats addresses	
  aop_info_0_t *info = (aop_info_0_t*)msg_ram_sections[MSG_RAM_AOP_INFO_0].start;
  info->dictionary.boot_cookie_offset = (uint32)&(info->boot_cookie) - AOP_SS_MSG_RAM_START_ADDRESS_BASE;
  info->dictionary.sleep_stats_offset = (uint32)&(info->sleep_stats) - AOP_SS_MSG_RAM_START_ADDRESS_BASE;
  info->dictionary.wdog_cookie_offset = (uint32)&(info->wdog_cookie) - AOP_SS_MSG_RAM_START_ADDRESS_BASE;
}

/**
 * <!-- msg_ram_cmd_db_set_init -->
 *
 * @brief Initialize the dictionary entry for cmd_db
 */
void msg_ram_cmd_db_set_init(uint32 data[MSG_RAM_CMD_DB_DATA_SIZE])
{
  aop_info_0_t *info = (aop_info_0_t*)msg_ram_sections[MSG_RAM_AOP_INFO_0].start;
  uint8 i;
  if (data == NULL)
  {
    return;
  }

  for (i = 0; i < MSG_RAM_CMD_DB_DATA_SIZE; i++)
  {
    info->dictionary.cmd_db_data_offset[i] = data[i];
  }
}

/**
 * <!-- msg_ram_ddrstats_set_init -->
 *
 * @brief Initialize the dictionary entry for ddr_statistics
 */
void msg_ram_ddrstats_set_init(void *ddr_stats_dictionary)
{
  aop_info_0_t *info = (aop_info_0_t*)msg_ram_sections[MSG_RAM_AOP_INFO_0].start;
  if (info == NULL)
  {
    return;
  }

  info->dictionary.ddr_stats_dictionary_offset = (uint32)(ddr_stats_dictionary) - AOP_SS_MSG_RAM_START_ADDRESS_BASE;
}

/**
 * <!-- msg_ram_get_smem_addr -->
 *
 * @brief Retrieve an smem address from message ram. These addresses are populated by XBL. 
 *
 * @param smem_id : The smem ID correpsonding to the address we are retrieving
 * @param smem_addr : A pointer to a uint32 that will be filled in the the smem address value
 *
 * @return SMEM_TABLE_SUCCESS if we found the address, smem_err code otherwise
 */ 
smem_table_err msg_ram_get_smem_addr(uint32 smem_id, uint32 *smem_addr)
{
  aop_smem_table_t *smem_table = (aop_smem_table_t*)msg_ram_sections[MSG_RAM_SMEM_TABLE].start;
  if(smem_table->initialized != 1)
  {
    //smem_table not initialized	  
    return SMEM_TABLE_NOT_INITIALIZED;
  }

  //make sure we've allocated sufficient space for what XBL has populated
  CORE_VERIFY(smem_table->num_addrs <= SMEM_TABLE_MAX_SIZE);
 
  for (int i = 0; i < smem_table->num_addrs; i++)
  {
    smem_addr_t t = smem_table->table[i];    
    if(t.smem_id == smem_id)
    {
      *smem_addr = t.addr;	    
      //smem_id found, return the address	    
      return SMEM_TABLE_SUCCESS;
    }
  }

  //smem id not found
  return SMEM_TABLE_ID_NOT_FOUND;
}

/**
 * <!-- msg_ram_set_boot_cookie -->
 *
 * @brief Set the boot cookie to indicate AOP is initialized 
 */ 
void msg_ram_set_boot_cookie(void)
{
  aop_info_0_t *info = (aop_info_0_t*)msg_ram_sections[MSG_RAM_AOP_INFO_0].start;
  info->boot_cookie = AOP_BOOT_COOKIE;
}

/**
 * <!-- msg_ram_get_sleep_stats_area -->
 *
 * @brief Return the pointer to a msg ram memory area where sleep stats are populated
 *
 */ 
void *msg_ram_get_sleep_stats_area (void)
{
  sleep_stats_t *info = ((aop_info_0_t*)msg_ram_sections[MSG_RAM_AOP_INFO_0].start)->sleep_stats;
  return info;
}

/**
 * <!-- msg_ram_get_frequency_stats_area -->
 *
 * @brief Return the pointer to a msg ram memory area where frequency stats are populated
 *
 */ 
void *msg_ram_get_aop_area (void)
{
  return msg_ram_get_mem_area(MSG_RAM_AOP);
}

/**
 * <!-- msg_ram_get_cprf_area -->
 *
 * @brief Return the pointer to a msg ram memory area where debug data is populated
 *
 */ 
#ifndef SDX24_STUBS
void *msg_ram_get_cprf_area (void)
{
  return msg_ram_get_mem_area(MSG_RAM_ARC_CPRF);
}
#endif

/**
 * <!-- msg_ram_get_frequency_stats_area -->
 *
 * @brief Return the pointer to a msg ram memory area where frequency stats are populated
 *
 */ 
void *msg_ram_get_frequency_stats_area (void)
{
  return msg_ram_get_mem_area(MSG_RAM_DDR_STATS);
}

/**
 * <!-- msg_ram_get_aop_logging_area -->
 *
 * @brief Return the pointer to a msg ram memory area where frequency stats are populated
 *
 */ 
void *msg_ram_get_aop_logging_area (void)
{
  return msg_ram_get_mem_area(MSG_RAM_AOP_LOGGING);
}

//compile time check to make sure we don't run off the end of msg_ram, and make sure aop_info_0 stays within the alloted size
#define STATIC_CORE_VERIFY(tag, cond) extern int COMPILE_TIME_ASSERT__##tag[(cond)?1:-1]
STATIC_CORE_VERIFY(msg_ram_size, ((((MSG_RAM_MAX) * MSG_RAM_SECTION_SIZE) + AOP_SS_MSG_RAM_START_ADDRESS_BASE) - 1) <= AOP_SS_MSG_RAM_END_ADDRESS_BASE);
STATIC_CORE_VERIFY(aop_info_0_size, MSG_RAM_INFO_0_SIZE == MSG_RAM_DRV_SIZE);
STATIC_CORE_VERIFY(smem_table_size, MSG_RAM_SMEM_TABLE_SIZE == MSG_RAM_DRV_SIZE);

