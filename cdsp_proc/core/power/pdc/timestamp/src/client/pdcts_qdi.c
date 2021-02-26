/*============================================================================== 
  FILE:         pdcts_qdi.c

  OVERVIEW:     Implements QDI layer for PDC TS API's in island mode

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/timestamp/src/client/pdcts_qdi.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include "comdef.h"
#include "CoreVerify.h"
#include "pdcts_qdi.h"
#include "uPdcTimestamp.h"
#include "qurt.h"

/*==============================================================================
                             GLOBAL VARIABLES
 =============================================================================*/
/* Client QDI opener object */
pdcts_qdi_data g_pdcTSClientObj[PDCTS_SUPPORTED_USER_PDS];

/* Data structure required for QDI init */
const pdcts_qdi_opener g_pdcTSOpenerData = 
{
  {pdcTimestamp_QDIInvoke, QDI_REFCNT_PERM, 0} 
};

/*==============================================================================
                            FUNCTION PROTOTYPES
 =============================================================================*/
pdcTimestamp_status pdcQdi_tsGetData(int                clientHandle, 
                                     pdcTimestamp_unit  *unitData, 
                                     uint32             intVector);

pdcTimestamp_status pdcQdi_tsEnable(int     clientHandle, 
                                    uint32  intVector,
                                    boolean enable);

/*==============================================================================
                           INTERNAL FUNCTIONS
 =============================================================================*/
/**
 * pdcQdi_tsGetData
 */
static inline pdcTimestamp_status uPdcQdi_tsGetIslandData(int                clientHandle, 
                                                          pdcTimestamp_unit  *unitData, 
                                                          uint32             intVector) 
{
  pdcTimestamp_status status;
  pdcTimestamp_unit   localData;

  status = uPdcTimestamp_getUnitData(&localData, intVector);

  if(status == PDC_TS_SUCCESS || status == PDC_TS_OVERFLOW)
  {
    qurt_qdi_copy_to_user(clientHandle, unitData, &localData, sizeof(pdcTimestamp_unit));
  }

  return status;
}

/*==============================================================================
                           EXTERNAL FUNCTIONS
 =============================================================================*/
/*
 * pdcTimestamp_QDIInvoke
 */
int pdcTimestamp_QDIInvoke(int             clientHandle,
                           qurt_qdi_obj_t  *pobj,
                           int             method,
                           qurt_qdi_arg_t  a1,
                           qurt_qdi_arg_t  a2,
                           qurt_qdi_arg_t  a3,
                           qurt_qdi_arg_t  a4,
                           qurt_qdi_arg_t  a5,
                           qurt_qdi_arg_t  a6,
                           qurt_qdi_arg_t  a7,
                           qurt_qdi_arg_t  a8,
                           qurt_qdi_arg_t  a9)
{
  pdcts_qdi_data *clntobj = (pdcts_qdi_data *)pobj;
   
  switch(method)
  {
    case QDI_ISLAND_CHECK_ALLOC:
      return TRUE;
      
    case QDI_OPEN:
      return pdcTimestamp_QDIOpen(clientHandle, clntobj);

    case PDCTS_QDI_GET_DATA:
      return pdcQdi_tsGetData(clientHandle, (pdcTimestamp_unit *)a1.ptr, a2.num);

    case PDCTS_QDI_GET_ISLAND_DATA:
      return uPdcQdi_tsGetIslandData(clientHandle, (pdcTimestamp_unit *)a1.ptr, a2.num);

    case PDCTS_QDI_ENABLE:
      return pdcQdi_tsEnable(clientHandle, a1.num, a2.num);
    
    case QDI_CLOSE:
    default:
      return qurt_qdi_method_default(clientHandle, pobj, method,
                                     a1, a2, a3, a4, a5, a6, a7, a8, a9);
  }
}

