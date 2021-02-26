/*==============================================================================
  FILE:         client_api.c

  OVERVIEW:     This file provides API functions for user PD located in normal mode

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1/power/pdc/timestamp/src/client_user/ddr/client_api.c#2 $
$DateTime: 2017/11/05 22:36:52 $
==============================================================================*/
#include "comdef.h"
#include "pdcts_qdi.h"
#include "pdcTimestamp.h"
#include "qurt.h"

/*==============================================================================
                              EXTERNAL VARIABLES
 =============================================================================*/
extern int g_pdcTSQDIClientHandle; /* Island section QDI handle */

/*==============================================================================
                          EXTERNAL API FUNCTIONS
 =============================================================================*/
/*
 * pdcTimestamp_enable
 */
pdcTimestamp_status pdcTimestamp_enable(uint32 intVector, boolean enable)
{
  return qurt_qdi_handle_invoke(g_pdcTSQDIClientHandle,
                                PDCTS_QDI_ENABLE,
                                intVector,
                                enable);
}

/*
 * pdcTimestamp_getUnitData
 */
pdcTimestamp_status pdcTimestamp_getUnitData(pdcTimestamp_unit *unitData, uint32 intVector)
{
  return qurt_qdi_handle_invoke(g_pdcTSQDIClientHandle,
                                PDCTS_QDI_GET_DATA,
                                unitData,
                                intVector);
}

