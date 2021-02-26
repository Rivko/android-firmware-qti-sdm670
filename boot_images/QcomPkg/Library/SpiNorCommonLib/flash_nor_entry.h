#ifndef FLASH_NOR_ENTRY_H
#define FLASH_NOR_ENTRY_H

/*==========================================================================
 *
 * FILE:      flash_nor_entry.h
 *
 * DESCRIPTION: Function and data structure declarations for nor entry layer
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2010-2012 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *=========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 * This section contains comments describing changes made to the
 * module. Notice that changes are listed in reverse chronological
 * order.
 *
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/12/12     sv      Update flash profile include file
 * 03/19/12     eo      Support for erase blocks by size
 * 07/20/11     sv      Remove unused defines and structures.
 * 05/20/11     sv      Flash driver Optimization
 * 10/04/10     sv      Move SPI specific structure defines to flash_nor_spi.h
 * 09/13/10     eo      Add formalized partitions support.
 * 09/02/10     sv      Add NOR tools support
 * 08/11/10     nr      Added stats framework support
 * 03/31/10     sv      Updated for SPI NOR support
 * 03/28/10     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "DALFramework.h"

#include "flash_dal.h"
#include "flash_dal_util.h"
#include "flash_properties.h"
#include "flash_nor_config.h"
#include "flash_profile_common.h"

/* Flash memory information  */
struct nor_flash_meminfo
{
    DALSYSMemHandle  handle_drv_mem;   /* Driver Workspace mem handle */
    DALSYSMemInfo    drv_mem_info;     /* Driver Workspace mem info */
};

/* Data common to all NOR drivers */
struct nor_entry_data
{
  uint32 nor_mem_initialized;          /* Flag to indicate flash
                                          memory initialization */
  uint32 multi_rw_support_count;       /* Number of pages to read
                                          in a single SPI transfer */
  uint32 build_env;                    /* Build environment ID */
  uint32 default_block_size_shift;     /* Save default block size shift */
  uint32 default_pages_per_block;      /* Save default pages per block */
  uint32 default_parti_block_start;    /* Save default partition start block */
  uint32 default_parti_block_limit;    /* Save default partition block limit */
  struct nor_flash_meminfo meminfo;    /* Flash memory allocation info */
};

/* Macro to test cache maintanence required or not */
#define DO_NOR_CACHE_MAINTENANCE \
  (nor_entry_init_data.meminfo.drv_mem_info.dwProps != DALSYS_MEM_PROPS_UNCACHED)

extern struct nor_entry_data nor_entry_init_data;

/* NOR Probe function */
int nor_entry_device_probe(flash_client_ctxt *client_ctxt);

#endif /* FLASH_NOR_ENTRY_H */
