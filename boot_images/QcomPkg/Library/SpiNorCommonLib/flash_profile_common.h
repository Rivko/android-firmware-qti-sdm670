#ifndef FLASH_PROFILE_COMMON_H
#define FLASH_PROFILE_COMMON_H
/*====================================================================
 *
 * FILE: flash_profile_common.h
 *
 * SERVICES: Functions for LA based profiling
 *
 * DESCRIPTION: This file contain initialization and interface APIs
 *              of flash config data, which is used across all NOR
 *              devices
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
 * 01/03/12     sv     Update the namings for flash profile
 * 02/02/11     bb     Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "flash_dal_target.h"

#ifdef FLASH_PROFILING_ENABLED

  #include "comdef.h"
//  #include "customer.h"
  #define FLASH_PROFILE_DEFAULT_NPAGES 0xFF

  enum flash_profile_switch_state
  {
    FLASH_PROFILE_OFF = 0,        /* Turns off profiling */
    FLASH_PROFILE_BAB_ON = 1,     /* Turn on babylon */
    FLASH_PROFILE_SFWK_ON = 2,    /* Turn on software peformance framework */
    FLASH_PROFILE_LA_ON = 3       /* Turn on LA */
  };

  enum  flash_profile_types
  {
    FLASH_PROFILE_READ_MAIN = 0,      /* Marker for reading main only */
    FLASH_PROFILE_READ_SPARE ,        /* Marker for read of spare only */
    FLASH_PROFILE_READ_MAIN_SPARE ,   /* Marker for read of main and spare */
    FLASH_PROFILE_READ_RAW,           /* Marker for raw read */
    FLASH_PROFILE_WRITE_MAIN,         /* Marker for write main only */
    FLASH_PROFILE_WRITE_SPARE,        /* Marker for write spare only */
    FLASH_PROFILE_WRITE_MAIN_SPARE,   /* Marker for write in main and spare */
    FLASH_PROFILE_WRITE_RAW,          /* Marker for raw write */
    FLASH_PROFILE_WRITE_PARTIAL_PAGE, /* Markwer for partial page write */
    FLASH_PROFILE_ERASE_BLOCKS,       /* Marker for Erase blocks */
    FLASH_PROFILE_BUS_TRANSFER,       /* DM or SPI Transfer */
    FLASH_PROFILE_HAL_TRANSFER,       /* Flash HAL Transfer */
    FLASH_PROFILE_HAL_PPROCESS,       /* Flash HAL Postprocess */
    FLASH_PROFILE_OPS_START,          /* Flash SPI ops start */
    FLASH_PROFILE_OPS_END,            /* Flash SPI ops end */
    FLASH_PROFILE_FRAMEWORK_MAX,      /* Flash profile type Max */
  };

  /* Structure definition used for profiling */
  struct flash_profile_tbl
  {
    uint32 is_enabled;  /* enable flag for core driver */
    void (*init) (void);    /* init marker */
    void (*start) (uint8 id, uint32 npages);           /* start marker */
    void (*end) (uint8 id);                           /* end marker */
    void (*set_info)(flash_client_ctxt *client_ctxt); /* sets info */
    void (*lock) (void);                                  /* fn for sync-lock */
    void (*unlock) (void);                                /* fn for sync-unlock */
  };

  /* This variable holds the babylon configurations,
   * and is defined in flash_dal_util.c
   */
  extern struct flash_profile_tbl flash_profile_cfg;

  /* Start Marker in Core Driver */
  #define FLASH_CORE_PROFILE_START(id, npages)               \
    do                                                       \
    {                                                        \
      if (flash_profile_cfg.is_enabled)                      \
      {                                                      \
        flash_profile_cfg.start((uint8) id, (uint32) npages); \
      }                                                      \
    } while(0)

  /* End marker  in Core Driver */
  #define FLASH_CORE_PROFILE_END(id)                         \
    do                                                       \
    {                                                        \
      if (flash_profile_cfg.is_enabled)                      \
      {                                                      \
        flash_profile_cfg.end((uint8) id);                   \
      }                                                      \
    } while(0)

  /* Initialize profiling in Core Driver */
  #define FLASH_CORE_PROFILE_INIT()                          \
    do                                                       \
    {                                                        \
      if (flash_profile_cfg.is_enabled)                      \
      {                                                      \
        flash_profile_cfg.init();                            \
      }                                                      \
    } while(0)

   /* Sets the page and other info in the framework structures */
  #define FLASH_CORE_PROFILE_SET_INFO(client_ctxt)           \
    do                                                       \
    {                                                        \
      if (flash_profile_cfg.is_enabled)                      \
      {                                                      \
        flash_profile_cfg.set_info(client_ctxt);             \
      }                                                      \
    } while(0)

  /* Sync locks for multipage support */
  #define FLASH_CORE_PROFILE_LOCK()                          \
    do                                                       \
    {                                                        \
      if (flash_profile_cfg.is_enabled)                      \
      {                                                      \
        flash_profile_cfg.lock();                            \
      }                                                      \
    } while(0)

  /* Sync unlocks for multipage support */
  #define FLASH_CORE_PROFILE_UNLOCK()                        \
    do                                                       \
    {                                                        \
      if (flash_profile_cfg.is_enabled)                      \
      {                                                      \
        flash_profile_cfg.unlock();                          \
      }                                                      \
    } while(0)

#else /* FLASH_PROFILING_ENABLED */

  /* Initialize profiling in Core Driver */
  #define FLASH_CORE_PROFILE_INIT()

  /* Start Marker in Core Driver */
  #define FLASH_CORE_PROFILE_START(id, npages)

  /* End marker  in Core Driver */
  #define FLASH_CORE_PROFILE_END(id)

   /* Sets the page and other info in the framework structures */
  #define FLASH_CORE_PROFILE_SET_INFO(client_ctxt)

  /* Sync locks for multipage support */
  #define FLASH_CORE_PROFILE_LOCK()

  /* Sync unlocks for multipage support */
  #define FLASH_CORE_PROFILE_UNLOCK()

#endif /* FLASH_PROFILING_ENABLED */

#endif /* FLASH_PROFILE_COMMON_H */
