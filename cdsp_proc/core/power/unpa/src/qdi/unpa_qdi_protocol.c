/*==============================================================================
  FILE:         unpa_qdi_protocol.c
  
  OVERVIEW:     Implements the client side of the UNPA/QDI interface in the userPDs -
                APIs that allow clients in the userPD to create clients and query handles to
                resources in the rootPD and subsequently, issue requests and make queries.

  
  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc. (QTI).
                All Rights Reserved.
                Qualcomm Confidential and Proprietary.
                Export of this technology or software is regulated by the U.S. Government.
                Diversion contrary to U.S. law prohibited.
=================================================================================
$Header: //components/rel/core.qdsp6/2.1.c4/power/unpa/src/qdi/unpa_qdi_protocol.c#1 $
$DateTime: 2018/07/30 01:21:36 $
===============================================================================*/
#include "comdef.h"
#include "CoreVerify.h"
#include "qurt.h"
#include "unpa_qdi.h"
#include "unpa_remote.h"

/*==============================================================================
                             INTERNAL VARIABLES
 =============================================================================*/
static int32 unpa_qdi_drvh;

/*==============================================================================
                            FUNCTION DEFINITIONS
 =============================================================================*/
/*
 * unpaRemote_createClientEX
 */
unpa_remote_client unpaRemote_createClientEX(const char *clientName,
                                             uint32      clientType,
                                             const char *resourceName,
                                             uint32      clientVal,
                                             void       *clientRef)
{
  int ret, rch;

  CORE_VERIFY_PTR(clientName);
  CORE_VERIFY_PTR(resourceName);
  
  ret = qurt_qdi_handle_invoke(unpa_qdi_drvh, UNPA_QDI_CREATE_CLIENT,
                               resourceName,
                               clientName, strlen(clientName),
                               clientType, clientVal, clientRef,
                               &rch);

  return (ret == 0 ? rch : -1);
}

/*
 * unpaRemote_issueRequestEX
 */
void unpaRemote_issueRequestEX(unpa_remote_client  rch, 
                               unpa_resource_state request,
                               uint32              requestAttr)
{
  CORE_VERIFY(rch >= 0);
  CORE_VERIFY(qurt_qdi_handle_invoke(rch, UNPA_QDI_ISSUE_SCALAR_REQUEST,
                                     request, requestAttr) == 0);
}

/*
 * unpaRemote_createQueryHandle
 */
unpa_remote_query_handle unpaRemote_createQueryHandle(const char *resourceName)
{
  int ret, rqh;

  CORE_VERIFY_PTR(resourceName);
  
  ret = qurt_qdi_handle_invoke(unpa_qdi_drvh, UNPA_QDI_CREATE_QUERY_HANDLE,
                               resourceName, &rqh);
                                
  return (ret == 0 ? rqh : -1);
}

/*
 * unpaRemote_destroyProxy
 */
void unpaRemote_destroyProxy(int32 rh)
{
  CORE_VERIFY(rh >= 0);
  qurt_qdi_close(rh);
  return;
}

/*
 * unpaRemote_query
 */
unpa_query_status unpaRemote_query(unpa_remote_query_handle rqh, 
                                   unpa_query_id            id,
                                   unpa_query_result       *result)
{
  int ret;
  CORE_VERIFY(rqh >= 0);
  
  ret = qurt_qdi_handle_invoke(rqh, UNPA_QDI_QUERY, id, result);
  return (ret < 0 ? UNPA_QUERY_QDI_ERROR : ret);
}

/* 
 * unpaRemote_qdiProtocolInit
 */ 
void unpaRemote_qdiProtocolInit(void)
{
  unpa_qdi_drvh = qurt_qdi_open("/dev/unpa");
  CORE_VERIFY(unpa_qdi_drvh > 0);
  return;
}
