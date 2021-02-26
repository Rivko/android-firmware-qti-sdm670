/*====================================================================
 *
 * FILE: flash_properties.c
 *
 * SERVICES: Target specific flash properties
 *
 * DESCRIPTION: This file contain target specific flash driver
 *              config data, which is used across all flash
 *              devices (NAND, SPI-NOR, I2C EEPROM)
 *
 *
 * PUBLIC CLASSES AND STATIC FUNCTIONS:
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:
 *
 * Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
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
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 07/27/12     sv      Remove DM specific codes
 * 07/20/11     sv      Add Watchdog for NOR tools
 * 05/20/11     sv      Updated to support SPI/I2C NOR
 * 01/22/11     bb      Initial Revision
 *==================================================================*/

/*====================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_properties.h"
#include "flash_dal_target.h"
#ifdef FLASH_ENABLE_NOR_SPI
  #include "flash_spi_wrapper.h"
#endif
/*===================================================================
 *
 *             FLASH  PROBE CONFIGURATION
 *
 ====================================================================*/

/* Flash driver treats devices as below,

    Primary storage - NAND, SPI-NOR
    Secondary storage - I2C

   At present the driver doesn't allow to use two different type
   of primary storage devices
 */

/* Default primary device probe function */
#ifndef FLASH_PRIMARY_DEV_PROBE
  #define FLASH_PRIMARY_DEV_PROBE    NULL
#endif

/* Default secondary device probe function */
#ifndef FLASH_SECONDARY_DEV_PROBE
  #define FLASH_SECONDARY_DEV_PROBE  NULL
#endif

/* Flash Probe table for primary storage devices (NAND/SPI). */
static flash_probe_tbl flash_probe_table[] = {
  {FLASH_PRIMARY_DEV_PROBE},
  {NULL}
};

/* Flash Probe table for secondary storage devices (I2C). */
static flash_probe_tbl flash_probe_table_2[] = {
  {FLASH_SECONDARY_DEV_PROBE},
  {NULL}
};

/* Kick watchdog function pointer table */
void kick_watchdog(void);
flash_watchdog_type watchdog_table = {
  kick_watchdog	
};	
/*===================================================================
 *
 *             HAL MEMCPY CONFIGURATION
 *
 ====================================================================*/

/* Memory util api's to be used by HAL */
#ifndef FLASH_HAL_MEM_UTIL_TYPE
  /* Use Standard memutil api's(from stdlib) */
  #define FLASH_HAL_MEM_UTIL_TYPE   FLASH_HAL_USE_STD_MEMUTIL
#endif

/*===================================================================
 *
 *            FLASH DMA CONFIGURATION
 *
 ====================================================================*/

#ifdef FLASH_ENABLE_DMA
  /* flash DMA state, 0-Disable,1-Enable */
  #define FLASH_DMA_STATE    1
#else
  /* flash DM state, 0-Disable,1-Enable */
  #define FLASH_DMA_STATE    0

#endif

/*===================================================================
 *
 *            FLASH SPI CONFIGURATION
 *
 ====================================================================*/

#ifdef FLASH_ENABLE_NOR_SPI
  /* Configure the SPI Init routines */
  static flash_spi_config_tbl flash_spi_config =
  {
    flash_spi_configure
  };
#endif

/*===================================================================
 *
 *            COMMON FLASH DRIVER CONFIGURATION
 *
 ====================================================================*/

/* Default multi read/write count */
#ifndef FLASH_MULTI_RW_SUPPORT_COUNT
  #define FLASH_MULTI_RW_SUPPORT_COUNT   1
#endif

/* Default HAL Memory allocated */
#ifndef FLASH_HAL_WS_BUFF_SIZE_BYTES
  #define FLASH_HAL_WS_BUFF_SIZE_BYTES   0
#endif

/* Default DAL Memory allocated */
#ifndef FLASH_DRV_WS_BUFF_SIZE_BYTES
  #define FLASH_DRV_WS_BUFF_SIZE_BYTES   0
#endif

/* Default DAL Memory allocated */
#ifndef FLASH_SPI_WS_BUFF_SIZE_BYTES
  #define FLASH_SPI_WS_BUFF_SIZE_BYTES   0
#endif

/* Default build environment */
#ifndef FLASH_BUILD_ENV
  #error "Undefined FLASH_BUILD_ENV! Failed to determine the environment!!"
#endif

/*===================================================================
 *
 *            FLASH DRIVER GET PROPERTY FUNCTION
 *
 ====================================================================*/
/*
 * Return target specific properties
 */
uintnt flash_get_property(enum flash_property_id prop_id)
{
  /* Return the property value if a valid property ID passed */
  switch (prop_id)
  {
    /* DMA state for Flash driver */
    case FLASH_DMA_STATE_ID:
      return FLASH_DMA_STATE;

    /* Memory util api's used by HAL     */
    case FLASH_HAL_MEM_UTIL_TYPE_ID :
      return FLASH_HAL_MEM_UTIL_TYPE;

    /* Flash HAL memory size */
    case FLASH_HAL_WS_BUFF_SIZE_ID :
      return FLASH_HAL_WS_BUFF_SIZE_BYTES;

    /* Driver workspace memory size */
    case FLASH_DRV_WS_BUFF_SIZE_ID :
      return FLASH_DRV_WS_BUFF_SIZE_BYTES;

    /* Number of pages supported in a multi page operation */
    case FLASH_MULTI_RW_SUPPORT_COUNT_ID :
      return FLASH_MULTI_RW_SUPPORT_COUNT;

    /* Flash DAL build environment */
    case FLASH_DAL_BUILD_ENV_ID :
      return ((uint32) FLASH_BUILD_ENV);

    /* Primary probe Devices */
    case FLASH_PROBE_TABLE_ID :
      return ((uint32) flash_probe_table);

    /* Secondary probe Devices */
    case FLASH_PROBE_TABLE_ID_2 :
      return ((uint32) flash_probe_table_2);

#ifdef FLASH_ENABLE_NOR_SPI
    /* Flash SPI DAL device id */
    case FLASH_SPI_DAL_DEVICE_ID :
      return ((uint32) FLASH_USE_SPI_DEV_ID); 

    /* SPI Workspace memory size */
    case FLASH_SPI_WS_BUFF_SIZE_ID :
      return ((uint32) FLASH_SPI_WS_BUFF_SIZE_BYTES);

    /* Flash SPI GSBI port */
    case FLASH_SPI_GSBI_PORT_ID :
      return ((uint32)FLASH_SPI_GSBI_PORT);

    /* Flash SPI config table */
    case FLASH_SPI_CONFIG_TABLE_ID :
      return ((uint32) &flash_spi_config);

    case FLASH_PROBE_NOR_TABLE_ID :
    {
      flash_probe_tbl *probe_tbl = NULL;
      flash_nor_spi_get_probe_table(&probe_tbl);
      return ((uintnt)probe_tbl);
    }      
#endif

    case FLASH_KICK_WATCHDOG_ID :
      return ((uintnt)&watchdog_table);
	  
    default:
      return FLASH_PROPERTY_NOT_DEFINED;
  }
}

#ifdef ENABLE_FLASH_WATCHDOG
  void kick_watchdog() 
  {
    *((volatile uintnt *)WDOG_RESET_ADDR) = WDOG_RESET_VAL;
  };
#else  
  void kick_watchdog() {};
#endif
