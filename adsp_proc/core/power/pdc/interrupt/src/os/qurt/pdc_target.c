/*===========================================================================
  FILE:         pdc_target.c
  
  OVERVIEW:     This file contains the function that obtains target specific
                data for mpm driver and assigns them to various structures
                variables so that common driver can use them.

  DEPENDENCY:   None

                Copyright (c) 2016 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
===============================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/interrupt/src/os/qurt/pdc_target.c#1 $
$DateTime: 2017/07/21 22:10:47 $
=============================================================================*/ 
#include "comdef.h"
#include "pdc_internal.h"
#include "pdc_target.h"
#ifdef PDC_MULTI_PD
#include "pdcQdi_root.h"
#endif

/*============================================================================
 *                            INTERNAL VARIABLES
 *===========================================================================*/
/* PDC mutex lock used when updating any mapping data. */
PDC_LOCK_TYPE PDC_LOCK_NAME = NULL;

/*==========================================================================
 *                               GLOBAL FUNCTIONS
 *==========================================================================*/
/* 
 * pdcTarget_initialize 
 */
void pdcTarget_initialize(void)
{
  /* Initilize PDC register lock */
  PDC_LOCK_INIT();

#ifdef PDC_MULTI_PD
  /* Setup multi-PD support */
  pdcQdi_initialize();
#endif

  /* Perform any more target specific init requirements here */

  return;
}

