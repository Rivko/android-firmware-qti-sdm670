/*==============================================================================
  FILE:         client_init.c

  OVERVIEW:     This file provides the initialization routines for PDC timestamp in
                user PD's

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/timestamp/src/client_user/ddr/client_init.c#1 $
$DateTime: 2017/10/16 10:14:08 $
==============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "pdcts_qdi.h"
#include "qurt.h"

/*==============================================================================
                              EXTERNAL VARIABLES
 =============================================================================*/
extern int g_pdcTSQDIClientHandle; /* Island section QDI handle */

/*==============================================================================
                           EXTERNAL FUNCTIONS
 =============================================================================*/
/**
 * pdcTimestamp_clientQDIInit
 * 
 * @brief Client init function that will setup the timestamp system in the 
 *        current user PD 
 */
void pdcTimestamp_clientQDIInit(void) 
{
  int clientPID = qurt_getpid();

  /* This function should not be called from guest OS PD.  A 0 value indicates
   * that is was. */
  CORE_VERIFY(0 != clientPID);

  /* Ensure this function is only called once per PD init */
  CORE_VERIFY(g_pdcTSQDIClientHandle < 0);

  /* Open the QDI handler so API's may be called from user PD */
  g_pdcTSQDIClientHandle = qurt_qdi_open(PDCTS_QDI_DRIVER_NAME);
  
  CORE_VERIFY(g_pdcTSQDIClientHandle >= 0);
  
  return;
}

