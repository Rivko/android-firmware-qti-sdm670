/*==============================================================================
  FILE:         rsc_target.c

  OVERVIEW:     This file implements any target specific functions for rsc.

  DEPENDENCIES: None

  NOTES:        This needs to be refactored once full DeviceCfg (or equivalent)
                support is present.
                
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/

#include "rsc.h"
#include "rsc_internal.h"

/*=======================================================================
 *                             INTERNAL VARIABLES
 *======================================================================*/

/**
 * @brief Global variables to be used by driver. This must be set up during
 *        target initialization.
 */
rsc_t *g_rscs = NULL;

/**
 * @brief Externed variables containing rsc configuration information for
 *        different versions.
 */
extern rsc_t g_rsc_instances[];

/*=======================================================================
 *                             GLOBAL FUNCTIONS
 *======================================================================*/
/**
 * <!-- rsc_target_init -->
 *
 * @brief Target specific initialization. This function must be called
 *        as part of rsc common driver initialization.
 */
void rsc_target_init()
{
  /* Add version specific initialization if any */
  g_rscs = g_rsc_instances;
}
