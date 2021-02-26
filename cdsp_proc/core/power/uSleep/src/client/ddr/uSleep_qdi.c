/*============================================================================== 
  FILE:         uSleep_qdi.c

  OVERVIEW:     QDI layer for uSleep API's in DDR mode

  DEPENDENCIES: None
  
                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/uSleep/src/client/ddr/uSleep_qdi.c#1 $
$DateTime: 2018/07/30 01:21:36 $
==============================================================================*/
#include <stdlib.h>
#include "CoreVerify.h"
#include "comdef.h"
#include "uSleep_qdi.h"
#include "client_os.h"
#include "qurt.h"
#include "uSleep_transi.h"
#include "uSleep_ddr_log.h"

/*==============================================================================
                             EXTERNAL VARIABLES
 =============================================================================*/
extern const uSleep_qdi_opener  g_uSleepOpenerData;
extern uSleep_qdi_data          g_uSleepClientObj[USLEEP_SUPPORTED_USER_PDS];

/*==============================================================================
                             INTERNAL VARIABLES
 =============================================================================*/
/* Lock for adding user PD client */
static qurt_mutex_t g_uSleepQdiLock;

/*==============================================================================
                           EXTERNAL FUNCTIONS
 =============================================================================*/
/*
 * uSleep_QDIRelease
 */ 
void uSleep_QDIRelease(qurt_qdi_obj_t *obj)
{
  uSleep_qdi_data *clientObj = (uSleep_qdi_data *)obj;

  uSleepDDRLog_printf(USLEEP_DDR_LOG_LEVEL_TRANSITION, 0, "Client QDI release");

  uSleep_deregisterUserNotificationCallback(clientObj->client_signals,
                                            clientObj->enterLat,
                                            clientObj->exitLat);

  qurt_mutex_lock(&g_uSleepQdiLock);

  /* Reset the clients callback data */
  clientObj->qdiobj.release = NULL;
  clientObj->client_signals = NULL;
  clientObj->enterLat       = 0;
  clientObj->exitLat        = 0;

  qurt_mutex_unlock(&g_uSleepQdiLock);

  return;
}

/*
 * uSleep_QDIOpen
 */
int uSleep_QDIOpen(int clientHandle, uSleep_qdi_data *me)
{
  int status;
  uSleep_qdi_data   *qdiClient;
  uint32            nPD;

  /* Lock global obj access */
  qurt_mutex_lock(&g_uSleepQdiLock);

  for(nPD = 0; nPD < USLEEP_SUPPORTED_USER_PDS; nPD++)
  {
    qdiClient = &g_uSleepClientObj[nPD];

    if(NULL == qdiClient->qdiobj.release)
    {
      break;
    }
  }

  CORE_LOG_VERIFY(nPD < USLEEP_SUPPORTED_USER_PDS,
                  uSleepDDRLog_printf(USLEEP_DDR_LOG_LEVEL_TRANSITION, 0,
                                      "!! Maximum number of user PDs reached !!"));

  uSleepDDRLog_printf(USLEEP_DDR_LOG_LEVEL_TRANSITION, 1,
                      "Creating QDI client (handle: 0x%x)",
                      clientHandle);
  
  qdiClient->qdiobj.invoke   = uSleep_QDIInvoke;
  qdiClient->qdiobj.refcnt   = QDI_REFCNT_INIT;
  qdiClient->qdiobj.release  = uSleep_QDIRelease;
  
  status = qurt_qdi_handle_create_from_obj_t(clientHandle, (qurt_qdi_obj_t *)qdiClient);

  qurt_mutex_unlock(&g_uSleepQdiLock);

  return status;
}

/*
 * uSleep_QDIInit
 */
void uSleep_QDIInit(void)
{
  qurt_mutex_init(&g_uSleepQdiLock);
  qurt_qdi_register_devname(USLEEP_QDI_DRIVER_NAME, &g_uSleepOpenerData.qdiobj);

  return;
}

/*
 * uSleep_QDIInsertCallback
 */
uint32 uSleep_QDIInsertCallback(int            clientHandle,
                                uSleep_signals *signal)
{
  return (uSleep_registerUserNotificationCallback(signal));
}

/*
 * uSleep_QDIAddCallbackLatency
 */
uint32 uSleep_QDIAddCallbackLatency(int   clientHandle,
                                    int32 enterLat,
                                    int32 exitLat)
{
  return (uSleep_addUserCallbackLatency(enterLat, exitLat));
}

/*
 * uSleep_QDIResetTransStats
 */
uint32 uSleep_QDIResetTransStats(int clientHandle)
{
  uSleepTrans_resetTimes();
  return USLEEP_SUCCESS;
}

