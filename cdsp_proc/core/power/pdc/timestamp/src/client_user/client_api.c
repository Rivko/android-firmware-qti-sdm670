/*==============================================================================
  FILE:        client_api.c

  OVERVIEW:     This file provides API functions for user PD that are
                located in the ISLAND section

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/timestamp/src/client_user/client_api.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include "comdef.h"
#include "pdcts_qdi.h"
#include "pdcTimestamp.h"
#include "qurt.h"

/*==============================================================================
                           INTERNAL VARIABLES
 =============================================================================*/
/* QDI Client Handle to communicate with Guest OS layer. This variable needs to
 * be located in island section memory, but is initialized while in normal
 * operational mode */
int g_pdcTSQDIClientHandle = -1;

/*==============================================================================
                          EXTERNAL API FUNCTIONS
 =============================================================================*/
/*
 * uPdcTimestamp_getUnitData
 */
pdcTimestamp_status uPdcTimestamp_getUnitData(pdcTimestamp_unit *unitData, uint32 intVector)
{
  return qurt_qdi_handle_invoke(g_pdcTSQDIClientHandle,
                                PDCTS_QDI_GET_ISLAND_DATA,
                                unitData,
                                intVector);
}

