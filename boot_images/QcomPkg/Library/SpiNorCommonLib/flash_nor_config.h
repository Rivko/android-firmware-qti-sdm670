#ifndef FLASH_NOR_CONFIG_H
#define FLASH_NOR_CONFIG_H

/*=======================================================================
 * FILE:        flash_nor_config.h
 *
 * SERVICES:    Common header for NOR Flash Configurations.
 *
 * DESCRIPTION:
 *
 * Copyright (c) 2010-2012, 2014 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 03/18/14     whc     Add device operating frequency support
 * 09/03/12     sb      Clean up
 * 03/19/12     sv      Add SPI NOR Power management support
 * 05/20/11     sv      Flash driver Optimization - add fn prototypes
 * 10/04/10     sv      Move SPI structures defines to flash_nor_spi.h
 * 03/15/10     bb      Initial Revision
 *==================================================================*/

/*-----------------------------------------------------------------------
 *            Include Files
 *----------------------------------------------------------------------*/

#include "flash_dal.h"
#include "flash_properties.h"

/*-----------------------------------------------------------------------
 *           Type Declarations
 *----------------------------------------------------------------------*/
struct flash_nor_params
{
  enum flash_nor_family family;       /* NOR family type */
  enum flash_type  type;              /* Device Technology Type */
  enum flash_width width;             /* Device Interface Width */
  enum flash_write_style write_style; /* Write style specific to Flash */
  uint8  device_name[32];             /* Device name string */
  uint32 num_ids;                     /* Num ids */
  uint32 dev_ids[4];                  /* Device ID */
  uint32 block_count;                 /* Number of total blocks in device */
  uint32 pages_per_block;             /* Number of pages in a block */
  uint32 page_size_bytes;             /* Logical page size */
  uint32 device_size_MB;              /* Total density in Mega Bytes */
  uint32 wbuffer_size_bytes;          /* Write Buffer size in bytes */
  uint32 flags1;                      /* Device specific flags */
  uint32 clk;                         /* Device operating frequency in KHz*/
  uint32 base_address;                /* Device base address */
  uint32 dpd_delay_time_us;           /* Delay to Enter DPD power mode */
  uint32 rel_dpd_delay_time_us;       /* Delay to Release from DPD power mode */
  uint32 num_banks;                   /* Total banks */
  uint32 num_erase_regions;           /* Total erase regions */
  struct flash_nor_bank_info bank_info[FLASH_NOR_BANKS_MAX]; /* Bank matrix */
  struct flash_nor_erase_region_info erase_region_info[FLASH_NOR_ERASE_REGION_MAX];
                                      /* Erase region matrix */
  struct flash_nor_block_erase_cfg_info erase_cfg_info;  
};

/* Flash NOR device parameter data */
typedef struct flash_nor_params flash_nor_params_type;

/* Structure for NOR configuration parameters */
struct flash_nor_cfg_data
{
  enum flash_type dev_type;   /* Intel/Spansion/Samsung family type   */
  uint32 cs_num;                       /* Chip select number */
  uint32 num_devices;                  /* Number of devices */
  struct flash_nor_params *dev_params; /* Device param */
};

/*-----------------------------------------------------------------------
 *           NOR Configuration APIs
 *----------------------------------------------------------------------*/


/* Gets the configuration data associated with all supported SPI NOR devices */
void flash_nor_spi_get_configs (struct flash_nor_cfg_data **cfg_data);

/* Gets the configuration data associated with all supported I2C NOR devices */
void flash_nor_i2c_get_configs (struct flash_nor_cfg_data **cfg_data);

/* Gets the probe table which has the list of SPI NOR probe routines */
void flash_nor_spi_get_probe_table(flash_probe_tbl **probe_tbl);

/* Gets the probe table which has the list of I2C NOR probe routines */
void flash_nor_i2c_get_probe_table(flash_probe_tbl **probe_tbl);

#endif  /*ifndef FLASH_NOR_CONFIG_H */
