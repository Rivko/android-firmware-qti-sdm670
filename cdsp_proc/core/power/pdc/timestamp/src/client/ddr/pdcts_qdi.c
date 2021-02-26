/*============================================================================== 
  FILE:         pdcts_qdi.c

  OVERVIEW:     QDI layer for PDC ts API's in DDR mode

  DEPENDENCIES: None
  
                Copyright (c) 2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/pdc/timestamp/src/client/ddr/pdcts_qdi.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include "CoreVerify.h"
#include "comdef.h"
#include "pdcTimestamp.h"
#include "pdcts_qdi.h"
#include "pdcLog.h"
#include "qurt.h"

/*==============================================================================
                             EXTERNAL VARIABLES
 =============================================================================*/
extern const pdcts_qdi_opener  g_pdcTSOpenerData;
extern pdcts_qdi_data          g_pdcTSClientObj[PDCTS_SUPPORTED_USER_PDS];

/*==============================================================================
                             INTERNAL VARIABLES
 =============================================================================*/
/* Lock for adding user PD client */
static qurt_mutex_t g_pdcTSQdiLock;

/*==============================================================================
                              EXTERNAL FUNCTIONS
 =============================================================================*/
/*
 * pdcTimestamp_QDIRelease
 */ 
void pdcTimestamp_QDIRelease(qurt_qdi_obj_t *obj)
{
  pdcts_qdi_data *clientObj = (pdcts_qdi_data *)obj;

  qurt_mutex_lock(&g_pdcTSQdiLock);

  /* Reset the clients callback data */
  clientObj->qdiobj.release = NULL;

  qurt_mutex_unlock(&g_pdcTSQdiLock);

  return;
}

/*
 * pdcTimestamp_QDIOpen
 */
int pdcTimestamp_QDIOpen(int clientHandle, pdcts_qdi_data *me)
{
  int             status;
  pdcts_qdi_data  *qdiClient;
  uint32          nPD;

  /* Lock global obj access */
  qurt_mutex_lock(&g_pdcTSQdiLock);

  for(nPD = 0; nPD < PDCTS_SUPPORTED_USER_PDS; nPD++)
  {
    qdiClient = &g_pdcTSClientObj[nPD];

    if(NULL == qdiClient->qdiobj.release)
    {
      break;
    }
  }

  CORE_VERIFY(nPD < PDCTS_SUPPORTED_USER_PDS);

  pdcLog_printf(1,
                "Creating PDCTS QDI client (handle: 0x%x)",
                clientHandle);

  qdiClient->qdiobj.invoke   = pdcTimestamp_QDIInvoke;
  qdiClient->qdiobj.refcnt   = QDI_REFCNT_INIT;
  qdiClient->qdiobj.release  = pdcTimestamp_QDIRelease;
  
  status = qurt_qdi_handle_create_from_obj_t(clientHandle, (qurt_qdi_obj_t *)qdiClient);

  qurt_mutex_unlock(&g_pdcTSQdiLock);

  return status;
}

/*
 * pdcTimestamp_QDIInit
 */
void pdcTimestamp_QDIInit(void)
{
  qurt_mutex_init(&g_pdcTSQdiLock);
  qurt_qdi_register_devname(PDCTS_QDI_DRIVER_NAME, &g_pdcTSOpenerData.qdiobj);
  return;
}

/*==============================================================================
                              QDI TIMESTAMP FRAMEWORK
 =============================================================================*/
/**
 * pdcQdi_tsGetData
 */
pdcTimestamp_status pdcQdi_tsGetData(int                clientHandle, 
                                     pdcTimestamp_unit  *unitData, 
                                     uint32             intVector) 
{
  pdcTimestamp_status status;
  pdcTimestamp_unit   localData;

  status = pdcTimestamp_getUnitData(&localData, intVector);

  if(status == PDC_TS_SUCCESS || status == PDC_TS_OVERFLOW)
  {
    qurt_qdi_copy_to_user(clientHandle, unitData, &localData, sizeof(pdcTimestamp_unit));
  }

  return status;
}

/**
 * pdcQdi_tsEnable
 */
pdcTimestamp_status pdcQdi_tsEnable(int     clientHandle, 
                                    uint32  intVector,
                                    boolean enable)
{
  pdcTimestamp_status status;

  status = pdcTimestamp_enable(intVector, enable);

  return status;
}
