#ifndef FLASH_DAL_TARGET_H
#define FLASH_DAL_TARGET_H
/*=============================================================================
 *
 * FILE:      flash_dal_target.h
 *
 * DESCRIPTION: Flash dal target specific config file for 9X15
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
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
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 09/11/12     sv      Add BAM based NAND driver support 
 * 08/21/12     sb      Add SPI NOR tools support for 9x25
 * 01/13/12     sb      Allocate max page size DAL WS buffer 
 * 09/16/11     jz      ehostdl image size reduction
 * 08/26/11     sv      Set correct SPI Device ID for 9x15
 * 07/22/11     sv      Add watchdog config for NOR tools
 * 06/22/11     eo      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nor_entry.h"
#include "DALDeviceId.h"

/*===================================================================
 *
 *             FLASH  BUILD ENVIRONMENT CONFIGURATION
 *
 ====================================================================*/
#if defined(BUILD_BOOT_CHAIN)
  #define FLASH_BUILD_ENV  FLASH_BUILD_BOOTLOADER
#elif defined(BUILD_NAND)
  #define FLASH_BUILD_ENV    FLASH_BUILD_NAND_TOOLS
#elif defined(BUILD_HOSTDL)
  #define FLASH_BUILD_ENV    FLASH_BUILD_HOSTDL
#elif defined(BUILD_NOR_TOOLS)
  #define FLASH_BUILD_ENV    FLASH_BUILD_NOR_TOOLS
#elif defined (IMAGE_APPS_PROC) || defined(IMAGE_QDSP6_PROC)
  #define FLASH_BUILD_ENV    FLASH_BUILD_APPS
#else
  #define FLASH_BUILD_ENV    FLASH_BUILD_MODEM
#endif

/*===================================================================
 *
 *             FLASH PROBE CONFIG
 *
 ====================================================================*/
/* Primary device probe function */
#if defined(FLASH_ENABLE_NOR_SPI)
  #define FLASH_PRIMARY_DEV_PROBE    nor_entry_device_probe
#endif

/*===================================================================
 *
 *             WATCHDOG CONFIG
 *
 ====================================================================*/
#if (FLASH_BUILD_ENV==FLASH_BUILD_NOR_TOOLS)
  #define ENABLE_FLASH_WATCHDOG
  #define WDOG_RESET_ADDR FLASH_WATCHDOG_RESET_ADDR
  #define WDOG_RESET_VAL  0x1
#endif

/*===================================================================
 *
 *            SPI NOR CONFIGURATIONS
 *
 ====================================================================*/

#define FLASH_SPI_GSBI_PORT    5 

#define FLASH_USE_SPI_DEV_ID   DALDEVICEID_SPIPD_DEVICE_5

/*===================================================================
 *
 *            FLASH  DAL/HAL MEMORY CONFIGURATION
 *
 ====================================================================*/

/* Number of pages supported in a multi page operation.

   IMPORTANT : SET THIS TO 1 WHEN DEMAND PAGING ENABLED AS
               FLASH DRIVER IS SUPPOSED TO RELEASE LOCKS
               AFTER EVERY PAGE OPERATION.
*/
#if defined(FLASH_ENABLE_NAND)
  #define FLASH_MULTI_RW_SUPPORT_COUNT    1

  #if defined(FLASH_ENABLE_DMA)
    #if (FLASH_BUILD_ENV == FLASH_BUILD_HOSTDL)
      #define FLASH_HAL_WS_BUFF_SIZE_BYTES    2048  /* No OTP required */
    #else
      /* HAL Memory allocated */
      #define FLASH_HAL_WS_BUFF_SIZE_BYTES    3072  /* OTP Support needed */
    #endif
  #else
    #define FLASH_HAL_WS_BUFF_SIZE_BYTES      0     /* No HAL buffer needed for non-DM */
  #endif

  /* Flash DAL workspace buffer size
   *   Main page Bytes + Spare Bytes + 64 driver miscellaneous
   * NOTE: DAL WS buffer is not re-adjusted in non DM path
   * as per runtime detected page size
   */

  #define FLASH_DRV_WS_BUFF_SIZE_BYTES    (4096+160+64)

#elif defined(FLASH_ENABLE_NOR_SPI)
  #define FLASH_MULTI_RW_SUPPORT_COUNT    2

  /* Maximum Flash device page size supported */
  #define FLASH_MAX_PAGE_SIZE             256

  /* Total buffer used in driver - in bytes (32 cmd/addr) */
  #define FLASH_DRV_WS_BUFF_SIZE_BYTES    32
  
  /* Total buffer used in SPI wrapper - in bytes (512 - data, 16 cmd/addr/pad) */
  #define FLASH_SPI_WS_BUFF_SIZE_BYTES    (16 + \
    (FLASH_MAX_PAGE_SIZE * FLASH_MULTI_RW_SUPPORT_COUNT))
#endif

/*===================================================================
 *
 *            MEMCPY CONFIGURATION
 *
 ====================================================================*/

/* Memory util api's to be used by HAL */
#ifdef FEATURE_QDSP6
  /* Currently Q6 has issues with standard memory utils when trying to do
   * memory operations on uncached memory. So use the memutil api's developed
   * in HAL to handle this. This setting says the HAL to use internal memutil
   * APIs instead of standard APIs
   */
  #define FLASH_HAL_MEM_UTIL_TYPE   FLASH_HAL_USE_INT_MEMUTIL
#endif

#endif /* #ifndef FLASH_DAL_TARGET_H */
