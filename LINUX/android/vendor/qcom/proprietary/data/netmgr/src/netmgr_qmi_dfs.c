/******************************************************************************

                       N E T M G R _ Q M I _D F S . C

******************************************************************************/

/******************************************************************************

  @file    netmgr_qmi_dfs.c
  @brief   Network Manager QMI data filter service helper

  DESCRIPTION
  Network Manager QMI data filter service helper

******************************************************************************/
/*===========================================================================

  Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_common.h"
#include "netmgr_qmi_dfs.h"
#include "netmgr_exec.h"
#include "netmgr_qmi_wda.h"
#include "netmgr_kif.h"
#include "netmgr_main.h"
#include "qmi_client_instance_defs.h"
#include "qmi_platform.h"

#include "netmgr_util.h"
#include "netmgr_cb.h"
#include "netmgr_qmi_cb.h"

#ifndef NULL
#define NULL 0
#endif

#define NETMGR_QMI_MIN_HANDLE 0
#define NETMGR_QMI_NO_HANDLE  -1


/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

qmi_client_type  qmi_dfs_clnt_hndl;
qmi_cci_os_signal_type netmgr_qmi_dfs_os_params;

#define NETMGR_QMI_DFS_MSM_SIO_PORT_START      (0xE13)
#define NETMGR_QMI_DFS_MSM_SIO_PORT(rev_link)  \
  (NETMGR_QMI_DFS_MSM_SIO_PORT_START + ((rev_link) - QMI_CONN_ID_REV_RMNET_0))

#define NETMGR_QMI_DFS_MDM_SIO_PORT_START      (0xE24)
#define NETMGR_QMI_DFS_MDM_SIO_PORT(rev_link)  \
  (NETMGR_QMI_DFS_MDM_SIO_PORT_START + ((rev_link) - QMI_CONN_ID_REV_RMNET_USB_0))

/*===========================================================================
  FUNCTION netmgr_qmi_dfs_msm_get_sio_port
===========================================================================*/
/*!
@brief
  maps qmi connection string to the corresponding SIO port for MSM targets

@arg *qmi_conn_str Name of device to get the connection instance
@arg *sio_port populated with the SIO port value

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE on error
*/
/*=========================================================================*/
static int netmgr_qmi_dfs_msm_get_sio_port
(
  const char   *qmi_conn_str,
  unsigned int *sio_port
)
{
  unsigned int i;
  int rc = NETMGR_FAILURE;

  static struct
  {
    const char *qmi_conn_str;
    unsigned int sio_port;
  }
  netmgr_qmi_dfs_sio_map[] =
  {
    { QMI_PORT_REV_RMNET_0, NETMGR_QMI_DFS_MSM_SIO_PORT(QMI_CONN_ID_REV_RMNET_0) },
    { QMI_PORT_REV_RMNET_1, NETMGR_QMI_DFS_MSM_SIO_PORT(QMI_CONN_ID_REV_RMNET_1) },
    { QMI_PORT_REV_RMNET_2, NETMGR_QMI_DFS_MSM_SIO_PORT(QMI_CONN_ID_REV_RMNET_2) },
    { QMI_PORT_REV_RMNET_3, NETMGR_QMI_DFS_MSM_SIO_PORT(QMI_CONN_ID_REV_RMNET_3) },
    { QMI_PORT_REV_RMNET_4, NETMGR_QMI_DFS_MSM_SIO_PORT(QMI_CONN_ID_REV_RMNET_4) },
    { QMI_PORT_REV_RMNET_5, NETMGR_QMI_DFS_MSM_SIO_PORT(QMI_CONN_ID_REV_RMNET_5) },
    { QMI_PORT_REV_RMNET_6, NETMGR_QMI_DFS_MSM_SIO_PORT(QMI_CONN_ID_REV_RMNET_6) },
    { QMI_PORT_REV_RMNET_7, NETMGR_QMI_DFS_MSM_SIO_PORT(QMI_CONN_ID_REV_RMNET_7) },
    { QMI_PORT_REV_RMNET_8, NETMGR_QMI_DFS_MSM_SIO_PORT(QMI_CONN_ID_REV_RMNET_8) },
  };

  if (!qmi_conn_str || !sio_port)
  {
    netmgr_log_err("%s(): invalid input!\n", __func__);
    return rc;
  }

  for (i = 0; i < (sizeof(netmgr_qmi_dfs_sio_map) / sizeof(netmgr_qmi_dfs_sio_map[0])); ++i)
  {
    if (0 == strcmp(qmi_conn_str, netmgr_qmi_dfs_sio_map[i].qmi_conn_str))
    {
      netmgr_log_med("%s(): found matching sio_port=0x%x for qmi_conn_str=%s\n",
                     __func__,
                     netmgr_qmi_dfs_sio_map[i].sio_port,
                     netmgr_qmi_dfs_sio_map[i].qmi_conn_str);

      *sio_port = netmgr_qmi_dfs_sio_map[i].sio_port;
      rc = NETMGR_SUCCESS;
      break;
    }
  }

  return rc;
}

/*===========================================================================
  FUNCTION netmgr_qmi_dfs_mdm_get_sio_port
===========================================================================*/
/*!
@brief
  maps qmi connection string to the corresponding SIO port for MDM targets

@arg *qmi_conn_str Name of device to get the connection instance
@arg *sio_port populated with the SIO port value

@return
  int - NETMGR_SUCCESS on successful operation,
        NETMGR_FAILURE on error
*/
/*=========================================================================*/
static int netmgr_qmi_dfs_mdm_get_sio_port
(
  const char   *qmi_conn_str,
  unsigned int *sio_port
)
{
  unsigned int i;
  int rc = NETMGR_FAILURE;

  static struct
  {
    const char *qmi_conn_str;
    unsigned int sio_port;
  }
  netmgr_qmi_dfs_sio_map[] =
  {
    { QMI_PORT_REV_RMNET_USB_0, NETMGR_QMI_DFS_MDM_SIO_PORT(QMI_CONN_ID_REV_RMNET_USB_0) },
    { QMI_PORT_REV_RMNET_USB_1, NETMGR_QMI_DFS_MDM_SIO_PORT(QMI_CONN_ID_REV_RMNET_USB_1) },
    { QMI_PORT_REV_RMNET_USB_2, NETMGR_QMI_DFS_MDM_SIO_PORT(QMI_CONN_ID_REV_RMNET_USB_2) },
    { QMI_PORT_REV_RMNET_USB_3, NETMGR_QMI_DFS_MDM_SIO_PORT(QMI_CONN_ID_REV_RMNET_USB_3) },
    { QMI_PORT_REV_RMNET_USB_4, NETMGR_QMI_DFS_MDM_SIO_PORT(QMI_CONN_ID_REV_RMNET_USB_4) },
    { QMI_PORT_REV_RMNET_USB_5, NETMGR_QMI_DFS_MDM_SIO_PORT(QMI_CONN_ID_REV_RMNET_USB_5) },
    { QMI_PORT_REV_RMNET_USB_6, NETMGR_QMI_DFS_MDM_SIO_PORT(QMI_CONN_ID_REV_RMNET_USB_6) },
    { QMI_PORT_REV_RMNET_USB_7, NETMGR_QMI_DFS_MDM_SIO_PORT(QMI_CONN_ID_REV_RMNET_USB_7) },
    { QMI_PORT_REV_RMNET_USB_8, NETMGR_QMI_DFS_MDM_SIO_PORT(QMI_CONN_ID_REV_RMNET_USB_8) },
  };

  if (!qmi_conn_str || !sio_port)
  {
    netmgr_log_err("%s(): invalid input!\n", __func__);
    return rc;
  }

  for (i = 0; i < (sizeof(netmgr_qmi_dfs_sio_map) / sizeof(netmgr_qmi_dfs_sio_map[0])); ++i)
  {
    if (0 == strcmp(qmi_conn_str, netmgr_qmi_dfs_sio_map[i].qmi_conn_str))
    {
      netmgr_log_med("%s(): found matching sio_port=0x%x for qmi_conn_str=%s\n",
                     __func__,
                     netmgr_qmi_dfs_sio_map[i].sio_port,
                     netmgr_qmi_dfs_sio_map[i].qmi_conn_str);

      *sio_port = netmgr_qmi_dfs_sio_map[i].sio_port;
      rc = NETMGR_SUCCESS;
      break;
    }
  }

  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_dfs_callback
===========================================================================*/
/*!
@brief
  Call back function registered for QMI DFS indications
*/
/*=========================================================================*/
static
void netmgr_qmi_dfs_callback
(
  qmi_client_type    user_handle,
  unsigned int       msg_id,
  void               *ind_buf,
  unsigned int       ind_buf_len,
  void               *ind_cb_data
)
{
  qmi_client_error_type   qmi_err;
  dfs_low_latency_traffic_ind_msg_v01 filterModeStatus;

  (void) ind_cb_data;

  if (ind_buf == NULL)
  {
    netmgr_log_err("%s(): Called with null pointers!\n", __func__);
    return;
  }

  switch (msg_id)
  {
    case QMI_DFS_LOW_LATENCY_TRAFFIC_IND_V01:
      qmi_err = qmi_client_message_decode(user_handle,
                                          QMI_IDL_INDICATION,
                                          msg_id,
                                          ind_buf,
                                          ind_buf_len,
                                          &filterModeStatus,
                                          sizeof(filterModeStatus));
      if(QMI_NO_ERR != qmi_err)
      {
        netmgr_log_err("%s(): Invalid filter mode ind msg error %d", __func__, qmi_err);
        return;
      }

      netmgr_kif_ifioctl_change_sleep_state((const char *)netmgr_main_get_phys_net_dev(),
                                            !filterModeStatus.traffic_start);
      netmgr_log_med("%s(): Got QMI_DFS_LOW_LATENCY_TRAFFIC_IND_V01 traffic_state: %d\n",
                     __func__,
                     filterModeStatus.traffic_start);

      break;

    default:
      netmgr_log_med("%s(): No handler for DFS IND id %d", __func__, msg_id);
  }
  return;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_dfs_init_client
===========================================================================*/
/*!
@brief
  Initialize QMI handle

@return
  int - NETMGR_QMI_DFS_SUCCESS on successful operation,
        NETMGR_QMI_DFS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
static int netmgr_qmi_dfs_init_client
(
  qmi_client_qmux_instance_type qcci_conn_id,
  qmi_client_type  *qmi_clnt_hndl
)
{
  int rc = NETMGR_FAILURE;

  if( NULL == qmi_clnt_hndl)
  {
    netmgr_log_err("%s: Invalid parameters",__func__);
    return rc;
  }

  rc = qmi_client_init_instance(dfs_get_service_object_v01(),
                                qcci_conn_id,
                                netmgr_qmi_dfs_callback,
                                NULL,
                                &netmgr_qmi_dfs_os_params,
                                NETMGR_QMI_TIMEOUT,
                                qmi_clnt_hndl);

  netmgr_log_low("%s(): qmi_client_init returned %d\n", __func__, rc);
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_dfs_ind_cb
===========================================================================*/
/*!
@brief
 Main callback function regisgterd during client init. It posts a command to do the
 required processing in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static
void netmgr_qmi_dfs_ind_cb
(
  qmi_client_type                user_handle,
  unsigned int                   msg_id,
  void                           *ind_buf,
  unsigned int                   ind_buf_len,
  void                           *ind_cb_data
)
{
  int link;
  netmgr_exec_cmd_t * cmd = NULL;
  qmi_client_error_type   qmi_err;

  NETMGR_LOG_FUNC_ENTRY;

  NETMGR_ASSERT(ind_buf != NULL);

  /* Get link id from user data ptr */
  if (NULL == ind_cb_data)
  {
    netmgr_log_err("%s(): Received invalid input!\n",__func__);
    goto bail;
  }

  /* While initializing the client and registering for callback we were passing the link argument
     as a void* in order to read it back we need to typecast it to int* and dereference */
  link = (int)((uintptr_t) ind_cb_data);

  /* Verify link id */
  NETMGR_ASSERT(netmgr_kif_verify_link(link) == NETMGR_SUCCESS);

  /* Allocate a command object */
  cmd = netmgr_exec_get_cmd();
  NETMGR_ASSERT(cmd);

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
  netmgr_util_wakelock_acquire();
#endif

  netmgr_log_med("%s(): received indication=%d on link=%d\n", __func__, msg_id, link);

  switch (msg_id)
  {
#ifdef FEATURE_DATA_IWLAN
    case QMI_DFS_REVERSE_IP_TRANSPORT_FILTERS_UPDATED_IND_V01:
      qmi_err = qmi_client_message_decode(user_handle,
                                          QMI_IDL_INDICATION,
                                          msg_id,
                                          ind_buf,
                                          ind_buf_len,
                                          &cmd->data.info.qmi_msg.data.dfs_ind.info.filter_ind,
                                          sizeof(cmd->data.info.qmi_msg.data.dfs_ind.info.filter_ind));

      if (QMI_NO_ERR != qmi_err)
      {
        goto bail;
      }
      break;
#else
    UNUSED(ind_buf_len);
    UNUSED(user_handle);
#endif /* FEATURE_DATA_IWLAN */
    default:
      netmgr_log_err("received unsupported indication type %d\n", msg_id);
      goto bail;
  }

  cmd->data.type                     = NETMGR_QMI_MSG_CMD;
  cmd->data.link                     = link;
  cmd->data.info.qmi_msg.type        = NETMGR_QMI_DFS_IND_CMD;
  cmd->data.info.qmi_msg.data.dfs_ind.link   = link;
  cmd->data.info.qmi_msg.data.dfs_ind.ind_id = msg_id;

  /* Post command for processing in the command thread context */
  if( NETMGR_SUCCESS != netmgr_exec_put_cmd( cmd ) ) {
    NETMGR_ABORT("%s(): failed to put commmand\n", __func__);
    goto bail;
  }

  return;

bail:
  if (NULL != cmd)
  {
    netmgr_exec_release_cmd( cmd );
  }
  NETMGR_LOG_FUNC_EXIT;
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_qmi_dfs_process_rev_ip_filter_rules
===========================================================================*/
/*!
@brief
 Performs processing of an incoming DFS Indication message. This function
 is executed in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static void
netmgr_qmi_dfs_process_rev_ip_filter_rules
(
  int                                               link,
  dfs_reverse_ip_transport_filters_action_enum_v01  action,
  dfs_filter_rule_type_v01                          *rules,
  unsigned int                                      num_rules
)
{
  unsigned int i;
  struct netmgr_qmi_cb_ifinfo ifinfo;
  rev_ip_filter_op_t filter_op = REV_IP_FILTER_OP_INVALID;

  if (!rules)
  {
    netmgr_log_err("%s(): invalid input!\n", __func__);
    return;
  }

  if (!NETMGR_KIF_IS_REV_RMNET_LINK(link))
  {
    netmgr_log_high("Ignoring QMI DFS rev ip filter update ind on forward link=%d\n", link);
    return;
  }

  for (i = 0; i < num_rules; ++i)
  {
    if ((DFS_PROTO_ESP_V01 == rules[i].xport_info.xport_protocol) &&
        (QMI_DFS_IPSEC_FILTER_MASK_SPI_V01 & rules[i].xport_info.esp_info.valid_params))
    {
      int ip_family = (DFS_IP_FAMILY_IPV4_V01 == rules[i].ip_info.ip_version) ? AF_INET : AF_INET6;

      if (DFS_REVERSE_IP_TRANSPORT_FILTERS_ADDED_V01 == action)
      {
        netmgr_log_med("%s(): installing ip_family=%d SPI filter=0x%x on link=%d!\n",
                       __func__,
                       rules[i].ip_info.ip_version,
                       htonl(rules[i].xport_info.esp_info.spi),
                       link);

        filter_op = REV_IP_FILTER_OP_ADD;

        /* TODO : Eventually all the reverse rmnet related DFS stuff should move into its own
           module */
      }
      else if (DFS_REVERSE_IP_TRANSPORT_FILTERS_DELETED_V01 == action)
      {
        netmgr_log_med("%s(): removing ip_family=%d SPI filter=0x%x on link=%d!\n",
                       __func__,
                       rules[i].ip_info.ip_version,
                       htonl(rules[i].xport_info.esp_info.spi),
                       link);

        filter_op = REV_IP_FILTER_OP_REMOVE;

        /* TODO: The SPI based rules will now be installed from modem_proxy module
           Run the corresponding chain*/
      }
      else
      {
        netmgr_log_err("%s(): unknown action=%d on link=%d!\n", __func__, action, link);
      }

      ifinfo.ip_family = ip_family;
      ifinfo.link = link;
      ifinfo.type = NETMGR_QMI_CB_IFTYPE_REVERSE;
      ifinfo.spi = htonl(rules[i].xport_info.esp_info.spi);
      ifinfo.filter_op = filter_op;

      netmgr_cb_runall(NETMGR_QMI_CB_TABLE, NETMGR_QMI_CB_DFS_REVIP_FILTER_IND, (void *) &ifinfo);
    }
  }
}
#endif /* FEATURE_DATA_IWLAN */

/*===========================================================================
                            GLOBAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  rmnet_qmi_dfs_release_qmi_client
===========================================================================*/
/*!
@brief
  Release QMI handle

@arg *dev_id Name of device to release the QMI DFS client

@return
  void

*/
/*=========================================================================*/
void rmnet_qmi_dfs_release_qmi_client
(
  const char *dev_id
)
{
  int rc;
  qmi_client_qmux_instance_type qcci_conn_id = 0;
  if (!dev_id)
  {
    netmgr_log_err("%s(): Called with null pointers!\n", __func__);
    return;
  }

  if (NETMGR_SUCCESS !=(netmgr_qmi_map_dev_conn_instance(dev_id,
                                                         &qcci_conn_id)))
  {
    netmgr_log_err("%s(): bad device ID received\n", __func__);
    return;
  }
  qmi_cci_qmux_xport_unregister(qcci_conn_id);

  rc = qmi_client_release(qmi_dfs_clnt_hndl);
  netmgr_log_low("%s(): qmi_client_release of qmi_clnt_hndl returned %d\n", __func__, rc);

}

/*===========================================================================
  FUNCTION  netmgr_qmi_dfs_enable_low_latency_filters
===========================================================================*/
/*!
@brief
  Enable the low latency filter indications

@arg *dev_id Name of device to enable the low latency filter indications on

@return
  int - NETMGR_QMI_DFS_SUCCESS on successful operation,
        NETMGR_QMI_DFS_BAD_ARGUMENTS if called with null/invalid arguments
        NETMGR_QMI_DFS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_dfs_enable_low_latency_filters
(
  const char *dev_id
)
{
  int rc;
  qmi_client_qmux_instance_type qcci_conn_id = 0;
  dfs_indication_register_req_msg_v01 request;
  dfs_indication_register_resp_msg_v01 response;

  if (!dev_id)
  {
    netmgr_log_err("%s(): Called with null pointers!\n", __func__);
    return NETMGR_QMI_DFS_BAD_ARGUMENTS;
  }

  if (NETMGR_SUCCESS !=(netmgr_qmi_map_dev_conn_instance(dev_id,
                                                         &qcci_conn_id)))
  {
    netmgr_log_err("%s(): bad device ID received\n", __func__);
    return NETMGR_QMI_DFS_BAD_ARGUMENTS;
  }

  qmi_cci_qmux_xport_register(qcci_conn_id);

  netmgr_qmi_dfs_init_client(QMI_CLIENT_INSTANCE_ANY, &qmi_dfs_clnt_hndl);
  if (qmi_dfs_clnt_hndl == NULL)
  {
      rc = NETMGR_QMI_DFS_FAILURE;
      goto bail;
  }

  request.report_low_latency_traffic_valid = TRUE;
  request.report_low_latency_traffic = TRUE;

  rc = qmi_client_send_msg_sync(qmi_dfs_clnt_hndl,
                                QMI_DFS_INDICATION_REGISTER_REQ_V01,
                                &request,
                                sizeof(dfs_indication_register_req_msg_v01),
                                &response,
                                sizeof(dfs_indication_register_resp_msg_v01),
                                NETMGR_QMI_TIMEOUT);
  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): Error sending QMI_DFS_INDICATION_REGISTER_REQ_V01 message: %d",
                   __func__,
                   rc);
    rc = NETMGR_QMI_DFS_FAILURE;
    goto bail;
  }

  netmgr_log_med("%s(): Successfully registered for QMI_DFS_INDICATION_REGISTER_REQ_V01: %d",
                 __func__,
                 rc);

  return NETMGR_QMI_DFS_SUCCESS;

bail:
  rmnet_qmi_dfs_release_qmi_client(dev_id);
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_qmi_dfs_init_qmi_client
===========================================================================*/
/*!
@brief
  Initialize QMI handle

@return
  int - NETMGR_QMI_DFS_SUCCESS on successful operation,
        NETMGR_QMI_DFS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_dfs_init_qmi_client
(
  int                     link,
  const char              *dev_str,
  dfs_ip_family_enum_v01  ip_family,
  qmi_client_type         *user_handle
)
{
  qmi_connection_id_type conn_id;
  qmi_client_error_type rc = QMI_INTERNAL_ERR;
  dfs_bind_client_req_msg_v01 req;
  dfs_bind_client_resp_msg_v01 resp;
  unsigned int sio_port = 0;
  int epid = -1;
  int ep_type = -1;
  int mux_id = -1;
  qmi_client_type  qmi_hndl = NULL;
  ds_target_t target = ds_get_target();

  /* On 64-bit compilers we cannot typecast int to void* directly */

  memset(&netmgr_qmi_dfs_os_params, 0, sizeof(qmi_cci_os_signal_type));

  *user_handle = NULL;

  /* Create client using QCCI libary - this is a blocking call.
     DFS service should be available over IPC Router */
  rc = qmi_client_init_instance(dfs_get_service_object_v01(),
                                QMI_CLIENT_INSTANCE_ANY,
                                netmgr_qmi_dfs_ind_cb,
                                (void *)(uintptr_t)link,
                                &netmgr_qmi_dfs_os_params,
                                NETMGR_QMI_TIMEOUT,
                                &qmi_hndl);

  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): failed on qmi_client_init_instance with rc=%d!\n", __func__, rc);
    goto bail;
  }

  memset (&req, 0, sizeof(req));
  memset (&resp, 0, sizeof(resp));

  if ( (DS_TARGET_MSM == target) &&
       (NETMGR_SUCCESS == netmgr_qmi_dfs_msm_get_sio_port(dev_str, &sio_port)) &&
       (sio_port > 0) )
  {
    /* Bind to corresponding SIO port */
    req.data_port_valid = 1;
    req.data_port = sio_port;

    req.ip_preference_valid = 1;
    req.ip_preference = ip_family;
  }
  else if ( (DS_TARGET_MDM == target) &&
            (NETMGR_SUCCESS == netmgr_qmi_dfs_mdm_get_sio_port(dev_str, &sio_port)) &&
            (sio_port > 0) )
  {
    /* Bind to corresponding SIO port */
    req.data_port_valid = 1;
    req.data_port = sio_port;

    req.ip_preference_valid = 1;
    req.ip_preference = ip_family;
  }
  else if ( (QMI_CONN_ID_INVALID != (conn_id = QMI_PLATFORM_DEV_NAME_TO_CONN_ID_EX(dev_str,
                                                                                   &ep_type,
                                                                                   &epid,
                                                                                   &mux_id))) &&
            (mux_id > 0) )
  {
    /* Bind to epid/mux-id */
    req.ep_id_valid = (epid != -1);
    req.ep_id.ep_type = (data_ep_type_enum_v01) ep_type;
    req.ep_id.iface_id = (uint32_t) epid;

    req.mux_id_valid = (mux_id != -1);
    req.mux_id = (uint8_t) mux_id;

    req.ip_preference_valid = 1;
    req.ip_preference = ip_family;
  }
  else
  {
    netmgr_log_err("%s(): failed to find sio_port/mux_id for dev_str=%s!\n", __func__, dev_str);
    rc = QMI_INTERNAL_ERR;
    goto bail;
  }

  /* Send bind_req */
  rc = qmi_client_send_msg_sync(qmi_hndl,
                                QMI_DFS_BIND_CLIENT_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)&resp,
                                sizeof(resp),
                                NETMGR_QMI_TIMEOUT);

bail:
  if (rc != QMI_NO_ERR || QMI_RESULT_SUCCESS_V01 != resp.resp.result)
  {
    if (NULL != qmi_hndl)
    {
      qmi_client_release(qmi_hndl);
    }
    return NETMGR_FAILURE;
  }
  else
  {
    *user_handle = qmi_hndl;
    return NETMGR_SUCCESS;
  }
}

/*===========================================================================
  FUNCTION  netmgr_qmi_dfs_process_ind
===========================================================================*/
/*!
@brief
 Performs processing of an incoming DFS Indication message. This function
 is executed in the Command Thread context.

@return
  void

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
void
netmgr_qmi_dfs_process_ind
(
  int           link,
  unsigned int  ind_id,
  void          *ind_data
)
{
  NETMGR_LOG_FUNC_ENTRY;

  /* Verify link id is valid before proceeding */
  NETMGR_ASSERT(netmgr_kif_verify_link(link) == NETMGR_SUCCESS);

  NETMGR_ASSERT(ind_data);

  /* Process based on indication type */
  switch (ind_id)
  {
#ifdef FEATURE_DATA_IWLAN
    case QMI_DFS_REVERSE_IP_TRANSPORT_FILTERS_UPDATED_IND_V01:
    {
      dfs_reverse_ip_transport_filters_updated_ind_msg_v01  *filter_ind;

      filter_ind = (dfs_reverse_ip_transport_filters_updated_ind_msg_v01 *) ind_data;
      if (filter_ind->filter_rules_valid)
      {
        netmgr_qmi_dfs_process_rev_ip_filter_rules(link,
                                                   filter_ind->filter_action,
                                                   filter_ind->filter_rules,
                                                   filter_ind->filter_rules_len);
      }
      break;
    }
#endif /* FEATURE_DATA_IWLAN */

    default:
      /* Ignore all other indications */
      netmgr_log_high("Ignoring QMI DFS IND of type 0x%x\n", ind_id);
      break;
  }

#if defined(FEATURE_DS_LINUX_ANDROID) && !defined(NETMGR_OFFTARGET)
  netmgr_util_wakelock_release();
#endif

  NETMGR_LOG_FUNC_EXIT;
  return;
}

#ifdef FEATURE_DATA_IWLAN
/*===========================================================================
  FUNCTION  netmgr_qmi_dfs_query_and_process_rev_ip_filters
===========================================================================*/
/*!
@brief
  Query and process reverse IP transport filters

@return
  int - NETMGR_QMI_DFS_SUCCESS on successful operation,
        NETMGR_QMI_DFS_FAILURE if there was an error sending QMI message

*/
/*=========================================================================*/
int netmgr_qmi_dfs_query_and_process_rev_ip_filters
(
  int                link,
  qmi_client_type    user_handle
)
{
  qmi_client_error_type rc = QMI_NO_ERR;
  int ret = NETMGR_SUCCESS;
  dfs_get_reverse_ip_transport_filters_req_msg_v01   req;
  dfs_get_reverse_ip_transport_filters_resp_msg_v01  resp;

  memset (&req, 0, sizeof(req));
  memset (&resp, 0, sizeof(resp));

  /* Send bind_req */
  rc = qmi_client_send_msg_sync(user_handle,
                                QMI_DFS_GET_REVERSE_IP_TRANSPORT_FILTERS_REQ_V01,
                                (void *)&req,
                                sizeof(req),
                                (void*)&resp,
                                sizeof(resp),
                                NETMGR_QMI_TIMEOUT);

  if (QMI_NO_ERR != rc || QMI_RESULT_SUCCESS_V01 != resp.resp.result)
  {
    netmgr_log_err("%s(): rev ip filter query failed rc=%d on link=%d\n", __func__, rc, link);
    ret = NETMGR_FAILURE;
    goto bail;
  }

  if (resp.filter_rules_valid)
  {
    netmgr_qmi_dfs_process_rev_ip_filter_rules(link,
                                               DFS_REVERSE_IP_TRANSPORT_FILTERS_ADDED_V01,
                                               resp.filter_rules,
                                               resp.filter_rules_len);
  }

bail:
  return ret;
}

#endif /* FEATURE_DATA_IWLAN */


