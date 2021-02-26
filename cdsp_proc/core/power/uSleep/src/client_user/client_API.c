/*==============================================================================
  FILE:         uSleep_api.c

  OVERVIEW:     This file provides uSleep API functions for user PD that are
                located in the ISLAND section

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/client_user/client_API.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "uSleep.h"
#include "uSleep_qdi.h"
#include "qurt.h"

/*==============================================================================
                           INTERNAL VARIABLES
 =============================================================================*/
/* QDI Client Handle to communicate with Guest OS layer. This variable needs to
 * be located in island section memory, but is initialized while in normal
 * operational mode */
int g_uSleepQDIClientHandle = -1;

/*==============================================================================
                          EXTERNAL API FUNCTIONS
 =============================================================================*/
/*
 * uSleep_exit
 */
uSleep_status uSleep_exit(void)
{
  return qurt_qdi_handle_invoke(g_uSleepQDIClientHandle, USLEEP_QDI_EXIT);
}

