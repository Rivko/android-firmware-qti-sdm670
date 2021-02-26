#ifndef FLASH_DAL_UTIL_H
#define FLASH_DAL_UTIL_H

/*====================================================================
 *
 * FILE:        flash_dal_util.h
 *
 * SERVICES:    Utility function declarations for flash DAL
 *
 * DESCRIPTION: Utility functions declarations for flash DAL
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
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
 * 09/05/12     sv      Move cache maintenance macros to functions
 * 05/21/11     bb      Flash driver optimization 
 * 10/21/10     sv      Add flash_busy_wait API 
 * 07/14/09     eo      Moved flash_extract_uuid() to this file
 * 03/20/09     bb      Moved cache wrappers to this file
 * 03/24/09     bb      Added safe string copy function
 * 03/12/09     bb      Add comments
 * 11/26/08     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_dal.h"

/* Aligns an address to 32 byte */
#define FLASH_ALIGN_32(a) ((a)&0x1F)?((((a)+32)>>5)<<5):(a)

/*===================================================================
 *
 *                     DEBUG DATA STRUCTURE
 *
 ====================================================================*/

/* This structure holds all the information required to
 debug the flash driver. */
struct flash_debug_info
{
  uint32 current_hal_id;     /* HAL ID of current client */
  uint32 current_hal_status; /* Current operation status in HAL */
  int    current_op_status;  /* Current operation status in driver */
  uint32 client_ctxt_base;   /* Base client context address */
  uint32 op_list_base;       /* Base operation list address */
};

/* This variable act as a start point to the flash driver debugging */
extern struct flash_debug_info flash_dbg;
extern struct flash_stats_tbl flash_cfg;

#define SET_FLASH_DBG_HAL_STATUS(x)    flash_dbg.current_hal_status = x
#define SET_FLASH_DBG_HAL_ID(x)        flash_dbg.current_hal_id = x
#define SET_FLASH_DBG_OP_STATUS(x)     flash_dbg.current_op_status = x
#define SET_FLASH_DBG_CLIENT_CTX(x)    flash_dbg.client_ctxt_base = (uint32)x
#define SET_FLASH_DBG_OP_LIST_BASE(x)  flash_dbg.op_list_base = (uint32)x

/*===================================================================
 *
 *                     UTIL FUNCTIONS
 *
 ====================================================================*/

/*
 * This API resets number of entries in the list to 0
 */
void flash_vtop_init_list(void);

/*
 * This API resets number of entries in the list to a speciifc value
 */
void flash_vtop_set_list_count(uint32 count);

/*
 * This API adds an entry to the list
 */
void flash_vtop_add_item(uint32 vaddr, uint32 phys_addr, uint32 size);

/*
 * This API looks for the entry that matches within which lies the
 * virt_addr and returns the corresponding physical address
 */
uint32 flash_vtop_get_physical(uint32 virt_addr);

/*
 * Safe string copy function for flash
 */
void flash_strcpy(uint8 *dest, const uint8 *src, uint32 len);

/*
 *  This function does a wait for a desired time in us.
 */
void flash_busy_wait(uint32 wait_time_us);

/* 
 * This function does cache clean on the given address 
 */
void flash_dcache_clean(uint32 addr, uint32 len);

/*
 * This function does cache invalidation on the given address 
 */
void flash_dcache_inval(uint32 addr, uint32 len);

#endif /* FLASH_DAL_UTIL_H */
