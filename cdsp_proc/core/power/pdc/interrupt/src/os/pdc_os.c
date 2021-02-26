/*==========================================================================
 
  FILE:       pdc_os.c

  OVERVIEW:   This file implements default OS specific functions of the PDC driver

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/interrupt/src/os/pdc_os.c#1 $
$DateTime: 2018/07/30 01:21:36 $
=============================================================================*/ 
#include "comdef.h"
#include "pdc_target.h"
#include "pdc_internal.h"
#include "pdcLog.h"

/*=========================================================================
 *                   GLOBAL DEFINITIONS & DECLARATIONS
 *========================================================================*/
/**
 * pdcOS_initialize
 * 
 * @brief Main initialization function, typically called from the rcinit framework
 */
void pdcOS_initialize(void)
{
  /* Init the main driver */
  pdcLog_printf(0, "Initializing main driver");
  pdc_initialize();

  /* Initialize target specific data */
  pdcLog_printf(0, "Initializing target");
  pdcTarget_initialize();

  return;
}

