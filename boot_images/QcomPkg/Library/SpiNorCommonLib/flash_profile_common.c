/*====================================================================
 *
 * FILE: flash_profile_common.c
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
 * 01/03/12     sv     Update the struct names for flash profile
 * 02/02/11     bb     Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_profile_common.h"

#ifdef FLASH_PROFILING_ENABLED

#ifdef FLASH_PROFILE_BABYLON_ENABLED

  #include "flash_profile_bab.h"

  struct flash_profile_tbl flash_profile_cfg =
  {
    FLASH_PROFILE_BAB_ON,
    flash_bab_init,     /* Babylon init marker */
    flash_bab_start,    /* Babylon start marker */
    flash_bab_end,      /* Babylon end marker */
    flash_bab_set_info, /* Babylon set info */
    flash_bab_lock,     /* Babylon lock */
    flash_bab_unlock    /* Babylon unlock */
  };

#elif defined (FLASH_PROFILE_STATFWK_ENABLED)

  #include "flash_profile_sfwk.h"

  struct flash_profile_tbl flash_profile_cfg =
  {
    FLASH_PROFILE_SFWK_ON,
    flash_stats_init,     /* Stats Fwk init marker */
    flash_stats_start,    /* Stats Fwk start marker */
    flash_stats_end,      /* Stats Fwk end marker */
    flash_stats_set_info, /* Stats Fwk set info */
    flash_stats_lock,     /* Stats Fwk lock */
    flash_stats_unlock    /* Stats Fwk unlock */
  };

#elif defined (FLASH_PROFILE_LA_ENABLED)

  #include "flash_profile_la.h"

  struct flash_profile_tbl flash_profile_cfg =
  {
    FLASH_PROFILE_LA_ON,
    flash_la_init,     /* LA init marker */
    flash_la_start,    /* LA start marker */
    flash_la_end,      /* LA end marker */
    flash_la_set_info, /* LA set info */
    flash_la_lock,     /* LA lock */
    flash_la_unlock    /* LA unlock */
  };

#endif

#endif /* FLASH_PROFILING_ENABLED */
