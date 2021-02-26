/**************************************************************************************

       N M _ M D M P R X Y _ M A R K _ P R O V I D E R _ F A C T O R Y . C

***************************************************************************************/

/**************************************************************************************

  @file    nm_mdmproxy_mark_provider_factory.c
  @brief   Modem proxy plugin

  DESCRIPTION
  Mark provider factory determines which set of marking rules we need to
  use for for forwarding traffic to the modem based on the modem's
  capabilities

**************************************************************************************/
/*====================================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

======================================================================================*/

/*=====================================================================================
                                  INCLUDE FILES
=====================================================================================*/

#include "netmgr.h"
#include "netmgr_util.h"

/* QMI DFS Includes for querying modem */
#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "qmi_client.h"
#include "qmi.h"

#include "nm_mdmprxy_mark_provider_factory.h"
#include "nm_mdmprxy_iptables_helper.h"

extern nm_mdmprxy_mark_prov_t  nm_mdmprxy_legacy_mark_provider;
extern nm_mdmprxy_mark_prov_t  nm_mdmprxy_socket_mark_provider;

/*=====================================================================================
                      LOCAL DEFINITIONS AND DECLARATIONS
=====================================================================================*/

#define NM_MDMPRXY_QMI_DFS_TIMEOUT  (10000)

qmi_client_type          qmi_dfs_clnt_hndl;
qmi_cci_os_signal_type   os_params;

typedef enum
{
  CAPABILITY_INVALID = -1,
  CAPABILITY_LEGACY_MARK,
  CAPABILITY_SOCKET_MARK,
  CAPABILITY_MAX
} modem_caps_t;

static dfs_remote_socket_handling_ver_enum_v01 dfs_version;

/*====================================================================================
  FUNCTION  nm_mdmprxy_query_modem_capability
====================================================================================*/
/*!
@brief
  Query the modem to determine the capability supported

*/
/*==================================================================================*/
static modem_caps_t
nm_mdmprxy_query_modem_capability(void)
{
  modem_caps_t  modem_caps = CAPABILITY_INVALID;
  int rc;
  dfs_get_capability_req_msg_v01  request;
  dfs_get_capability_resp_msg_v01 response;

  /* Create the QCCI client */
  rc = qmi_client_init_instance(dfs_get_service_object_v01(),
                                QMI_CLIENT_INSTANCE_ANY,
                                NULL,
                                NULL,
                                &os_params,
                                NM_MDMPRXY_QMI_DFS_TIMEOUT,
                                &qmi_dfs_clnt_hndl);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): failed on qmi_client_init_instance with rc [%d]", __func__, rc);
    goto bail;
  }

  /* Query for capability on the modem */
  memset(&request, 0 , sizeof(request));
  memset(&response, 0 , sizeof(response));

  netmgr_log_low("%s(): Querying modem capability...", __func__);

  rc = qmi_client_send_msg_sync(qmi_dfs_clnt_hndl,
                                QMI_DFS_GET_CAPABILITY_REQ_V01,
                                &request,
                                sizeof(dfs_get_capability_req_msg_v01),
                                &response,
                                sizeof(dfs_get_capability_resp_msg_v01),
                                NM_MDMPRXY_QMI_DFS_TIMEOUT);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): modem query capability request failed, rc [%d]", __func__, rc);

    /* If the query failed, default to legacy marking */
    modem_caps = CAPABILITY_LEGACY_MARK;
    goto bail;
  }

  if (response.resp.result != QMI_RESULT_SUCCESS_V01)
  {
    netmgr_log_err("%s(): Query capability request failed with result [%d], err [0x%x]",
                   __func__, response.resp.result, response.resp.error);
    modem_caps = CAPABILITY_LEGACY_MARK;
    goto bail;
  }

  if (NETMGR_SUCCESS != nm_mdmprxy_check_iptables_socket_match_support())
  {
    /* Socket mark provider also depends on certain iptables options to be supported on target
       If those options are not supported we need to fall back to legacy marking */
    netmgr_log_err("%s(): iptables does not support advanced options for socket matching,"
                   " falling back to legacy", __func__);
    modem_caps = CAPABILITY_LEGACY_MARK;
    goto bail;
  }

  netmgr_log_low("%s(): Modem query capability succeeded", __func__);
  netmgr_log_low("%s(): response.remote_socket_capability_valid [%d], "
                  "response.remote_socket_capability [%d]",
                  __func__,
                  response.remote_socket_capability_valid,
                  response.remote_socket_capability);

  if (response.remote_socket_capability_valid
        && response.remote_socket_capability)
  {
    /* Remote bind support is available on the modem */
    modem_caps = CAPABILITY_SOCKET_MARK;

    /* Check the version to determine whether advanced port request feature is supported */
    if (response.remote_socket_capability_version_valid)
    {
      dfs_version = response.remote_socket_capability_version;
    }
    else
    {
      dfs_version = DFS_REMOTE_SOCKET_HANDLING_V0_V01;
    }
  }
  else
  {
    modem_caps = CAPABILITY_LEGACY_MARK;
  }

bail:
  if (qmi_dfs_clnt_hndl)
  {
    rc = qmi_client_release(qmi_dfs_clnt_hndl);
    netmgr_log_med("%s(): Releasing qmi dfs handle returned rc [%d]", __func__, rc);
  }

  return modem_caps;
}

/*=====================================================================================
                               PUBLIC FUNCTIONS
=====================================================================================*/

/*====================================================================================
  FUNCTION  nm_mdmprxy_get_mark_provider
====================================================================================*/
/*!
@brief
  Returns the mark provider instance based on modem query support

@return
  nm_mdmprxy_mark_prov_t instance

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*==================================================================================*/
nm_mdmprxy_mark_prov_t *nm_mdmprxy_get_mark_provider(void)
{
  modem_caps_t  modem_caps = nm_mdmprxy_query_modem_capability();

  if (CAPABILITY_SOCKET_MARK == modem_caps)
  {
    return &nm_mdmprxy_socket_mark_provider;
  }
  else if (CAPABILITY_LEGACY_MARK == modem_caps)
  {
    return &nm_mdmprxy_legacy_mark_provider;
  }
  else
  {
    return NULL;
  }
}

/*====================================================================================
  FUNCTION  nm_mdmprxy_get_dfs_service_version
====================================================================================*/
/*!
@brief
  Returns the version number for DFS service

@return
  nm_mdmprxy_mark_prov_t instance

@note

  - Dependencies
    - We have requested for capability information from modem via
      QMI_DFS_GET_CAPABILITY_REQ_V01

  - Side Effects
    - None
*/
/*==================================================================================*/
dfs_remote_socket_handling_ver_enum_v01 nm_mdmprxy_get_dfs_service_version(void)
{
  return dfs_version;
}
