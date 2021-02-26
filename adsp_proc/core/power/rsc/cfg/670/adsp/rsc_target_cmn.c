/*==============================================================================
  FILE:         rsc_target.c

  OVERVIEW:     This file implements any target specific functions for rsc.

  DEPENDENCIES: None

  NOTES:        This needs to be refactored once full DeviceCfg (or equivalent)
                support is present.
                
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/
#include "comdef.h"
#include "rsc.h"
#include "rsc_internal.h"

/*=======================================================================
 *                             INTERNAL VARIABLES
 *======================================================================*/
/* Global variables to be used by driver. This must be set up during target
 * initialization. */
rsc_t  *g_rscs    = NULL;
uint32 g_rscCount = 0;

