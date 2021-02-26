/*==============================================================================
  FILE:         rsc_env.c

  OVERVIEW:     This file sets up the environment where rsc is running. It can
                be different based on different processors/OS/Exception level
                it may run.

  DEPENDENCIES: Environment specific.
                RSC_SS_BASE - must be defined in build script.
                
                Copyright (c) 2015 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary
================================================================================*/

#include "msmhwiobase.h"
#include "rsc.h"
#include "rsc_internal.h"

/*==============================================================================
                                  MACRO DEFINITIONS
 =============================================================================*/
#define RSC_SS_BASE MODEM_TOP_BASE

/*==========================================================================
 *                            GLOBAL FUNCTIONS
 *=========================================================================*/

void rsc_init_base_address(rsc_handle handle)
{
  static uint8* g_rsc_ss_base = NULL;

  if(NULL == g_rsc_ss_base)
  {
    g_rsc_ss_base = (uint8 *)AOSS_BASE;
  }

  handle->addr = g_rsc_ss_base + handle->offset;
}
