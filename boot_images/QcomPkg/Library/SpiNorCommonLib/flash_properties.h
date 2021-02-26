#ifndef FLASH_PROPERTIES_H
#define FLASH_PROPERTIES_H
/*=============================================================================
 *
 * FILE:      flash_properties.h
 *
 * DESCRIPTION: Target specific flash driver customization file
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2011 Qualcomm Technologies Incorporated.
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
 * 07/27/12     sv      Remove DM specific codes
 * 05/10/11     bb/sv   Flash Optimization
 * 05/18/11     jz      Added property FLASH_EBI2_CLOCK_ID
 * 03/02/11     sv      Remove sleep properties
 * 10/11/10     bn      Add NOR I2C support 
 * 09/02/10     sv      NOR tools support
 * 08/11/10     nr      Added stats framework support
 * 06/29/10     eo      Add support for Sleep configuration
 * 06/23/10     eo      Support SPI DAL Device ID configuration
 * 03/15/10     bb      Support for isolating NOR build from NAND
 * 01/24/10     mcg     Add support for DM configuration
 * 01/04/10     sv      Added APPSBOOT build enviroment ID
 * 10/23/09     mh      Add ctlr mode of operation 
 * 10/15/09     bb      Added support for using DMOV in interrupt mode
 * 07/28/09     sv      Add property id for flash hal memutil type
 * 07/27/09     eo      Add kick watchdog support flash properties
 * 05/11/09     bb      Support for babylon
 * 03/12/09     bb      Add property not defined macro definition
 * 01/30/09     bb      Moved property structure to .c file
 * 10/24/08     bb      Initial Revision
 *==================================================================*/

#include "DALStdDef.h"
#include "flash_dal.h"

/* Flash OneNAND Configuration */
#define FLASH_ONENAND_AC_CHARACTERISTICS 0x0001
#define FLASH_ONENAND_BURSTRW_MODE       0x0002

/* Ctlr  mnode of operation */
#define FLASH_HAL_NAND_DEFAULT_MODE      0x1
#define FLASH_HAL_NAND_INTERLEAVE_MODE   0x2

/* Flash Build Environment Ids */
#define FLASH_BUILD_DEFAULT      0
#define FLASH_BUILD_BOOTLOADER   1
#define FLASH_BUILD_NAND_TOOLS   2
#define FLASH_BUILD_NOR_TOOLS    3
#define FLASH_BUILD_HOSTDL       4
#define FLASH_BUILD_EHOSTDL      5
#define FLASH_BUILD_MODEM        6
#define FLASH_BUILD_APPS         7
#define FLASH_BUILD_APPS_BOOT    8

/* This defines the IDs for various target specific flash
   properties */
enum flash_property_id
{
  FLASH_DMA_STATE_ID,              /* DMA State for flash driver */
  FLASH_HAL_WS_BUFF_SIZE_ID,       /* Workspace buffer for flash HAL */
  FLASH_DRV_WS_BUFF_SIZE_ID,       /* Workspace buffer for flash DAL */
  FLASH_MULTI_RW_SUPPORT_COUNT_ID, /* Number of pages supported
                                      in a multi page operation */
  FLASH_KICK_WATCHDOG_ID,          /* Watchdog register to kick */
  FLASH_PROBE_TABLE_ID,            /* Devices to be probed */
  FLASH_PROBE_TABLE_ID_2,          /* Devices to be probed for device ID 2 */
  FLASH_PROBE_NAND_TABLE_ID,       /* NAND Probe Table */
  FLASH_PROBE_NOR_TABLE_ID,        /* NOR Probe Table */
  FLASH_PROBE_NOR_TABLE_REDUCED_ID, /* NOR Probe Table Reduced */
  FLASH_DAL_BUILD_ENV_ID,          /* Flash DAL build environment */
  FLASH_STATS_FRAMEWORK_CONFIG_TABLE_ID,   /* Flash Stats configuration Table */
  FLASH_HAL_MEM_UTIL_TYPE_ID,      /* Memutil type for flash HAL */
  FLASH_HAL_NAND_CTLR_MODE_ID,     /* Controller mode of operation */
  FLASH_SPI_CONFIG_TABLE_ID,       /* SPI configuration Table */
  FLASH_SLEEP_CONFIG_TABLE_ID,     /* Sleep configuration Table */
  FLASH_SPI_DAL_DEVICE_ID,         /* SPI DAL device id */
  FLASH_SPI_WS_BUFF_SIZE_ID,       /* Workspace buffer for SPI wrapper */
  FLASH_SPI_GSBI_PORT_ID,          /* GSBI port for SPI NOR */
  FLASH_I2C_CONFIG_TABLE_ID,       /* I2C configuration Table */
  FLASH_EBI2_CLOCK_ID,             /* EBI2 clock rate */
  FLASH_PROPERTY_ID_MAX = 0xFF     /* Maximum property IDs */
};

/* Bits to indicate probing of NOR and NAND*/
#define FLASH_NOR_PROBE 0x1
#define FLASH_NAND_PROBE 0x2

/* Property not defined */
#define FLASH_PROPERTY_NOT_DEFINED  (0xFFFFFFFF)

/* Specify flash HAL to use standard memutils */
#define FLASH_HAL_USE_STD_MEMUTIL          0

/* Specify flash HAL to use internal memutils used to handle
 * the issue with memory operations on uncatched memory
 */
#define FLASH_HAL_USE_INT_MEMUTIL   1

/* Structure definition used to make an array of probe functions */
typedef struct
{
  int (*device_prober)(flash_client_ctxt *);  /* Device Specific Probe */
} flash_probe_tbl;

/* Structure to initialize Data mover interface */
typedef struct
{
  void (*dm_config)(void *);
} flash_dm_config_tbl;

/* Structure to initialize SPI interface */
typedef struct
{
  void (*spi_config)(void *);
} flash_spi_config_tbl;

/* Structure to initialize I2C interface */
typedef struct
{
  void (*i2c_config)(void *);
} flash_i2c_config_tbl;

/* Structure defining the watchdog to kick function */
typedef struct
{
  void (*kick_watchdog)(void);
} flash_watchdog_type;


/* Get the property configured for the target */
uintnt flash_get_property(enum flash_property_id prop_id);

#endif /* FLASH_PROPERTIES_H */
