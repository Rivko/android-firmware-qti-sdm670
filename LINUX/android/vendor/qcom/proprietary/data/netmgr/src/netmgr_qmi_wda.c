/******************************************************************************

                       N E T M G R _ Q M I _ W D A . C

******************************************************************************/

/******************************************************************************

  @file    netmgr_qmi_wda.c
  @brief   Netmanager QMI Wireless Data Administrative helper

  DESCRIPTION
  Netmanager QMI Wireless Data Administrative helper

******************************************************************************/
/*===========================================================================

  Copyright (c) 2013-2015 Qualcomm Technologies, Inc. All Rights Reserved

  Qualcomm Technologies Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

===========================================================================*/

#include "qmi_client.h"
#include "qmi_cci_target.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_common.h"
#include "netmgr_qmi_wda.h"
#include "netmgr_qmi.h"
#include "qmi_client_instance_defs.h"

/* These following defines are present in both qmi_client.h(qmi frameworks)
 * and qmi.h(legacy qmi). In order to supress compiler warnings of
 * redifinitions, undef the defines here
 */
#undef QMI_INTERNAL_ERR
#undef QMI_SERVICE_ERR
#undef QMI_TIMEOUT_ERR
#undef QMI_EXTENDED_ERR
#undef QMI_PORT_NOT_OPEN_ERR
#undef QMI_MEMCOPY_ERROR
#undef QMI_INVALID_TXN
#undef QMI_CLIENT_ALLOC_FAILURE

#include "netmgr_util.h"

#ifndef NULL
#define NULL 0
#endif

#define NETMGR_QMI_MIN_HANDLE 0
#define NETMGR_QMI_NO_HANDLE  -1

/* To prevent inadvertent release of stack related variable we
 * will make os_params as global */
qmi_cci_os_signal_type os_params;

/*===========================================================================
  FUNCTION  rmnet_wda_release_qmi_client
===========================================================================*/
/*!
@brief
  Initialize QMI handle
*/
/*=========================================================================*/
static int rmnet_wda_init_qmi_client
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

  rc = qmi_client_init_instance(wda_get_service_object_v01(),
                                qcci_conn_id,
                                NULL,
                                NULL,
                                &os_params,
                                NETMGR_QMI_TIMEOUT,
                                qmi_clnt_hndl);


  netmgr_log_low("%s(): qmi_client_init returned %d\n", __func__, rc);
  return rc;
}

/*===========================================================================
  FUNCTION  rmnet_wda_release_qmi_client
===========================================================================*/
/*!
@brief
  Release QMI handle
*/
/*=========================================================================*/
static void rmnet_wda_release_qmi_client
(
  qmi_client_type  *qmi_clnt_hndl
)
{
  int rc;

  if( NULL == qmi_clnt_hndl )
  {
    netmgr_log_err("%s: Invalid parameters",__func__);
  }
  else
  {
    rc = qmi_client_release(*qmi_clnt_hndl);
    netmgr_log_low("%s(): qmi_client_release of qmi_clnt_hndl returned %d\n", __func__, rc);
  }
}

#define NETMGR_LOG_SET_DATA_FORMAT_STRUCT(X, Y, Z) do { \
  netmgr_log_med("%s(): %s dev=%s, ll_prot=%d, qos=%d, ul=%d, dl=%d, dl_pkt=%d, dl_size=%d, qos_hdr=%d, dl_pad=%d, fc=%d", \
                   __func__, \
                   X, \
                   Y, \
                   (Z)->link_prot, \
                   (Z)->qos_format, \
                   (Z)->ul_data_aggregation_protocol, \
                   (Z)->dl_data_aggregation_protocol, \
                   (Z)->dl_data_aggregation_max_datagrams, \
                   (Z)->dl_data_aggregation_max_size, \
                   (Z)->qos_header_format, \
                   (Z)->dl_minimum_padding, \
                   (Z)->flow_control); \
  } while (0);

#define NETMGR_LOG_SET_QMAP_SETTINGS_STRUCT(X, Y, Z) do { \
  netmgr_log_med("%s(): %s dev=%s, ibfc=%d", \
                   __func__, \
                   X, \
                   Y, \
                   (Z)->in_band_flow_control); \
  } while (0);


/*===========================================================================
  FUNCTION  netmgr_wda_set_data_format
===========================================================================*/
/*!
@brief
  Sets link data format with WDA_SET_DATA_FORMAT message

@arg *dev_id Name of device to call WDA_SET_DATA_FORMAT on
@arg *request Structure of available WDA data format parameters
@arg *response Structure of WDA data format parameters which the modem
               replied with

@return
  int - NETMGR_WDA_SUCCESS on successful operation,
        NETMGR_WDA_BAD_ARGUMENTS if called with null/invalid arguments
        NETMGR_WDA_QMI_FAILURE if there was an error sending QMI message
        NETMGR_WDA_MODEM_REJECTED if modem rejected the WDA format
*/
/*=========================================================================*/
int netmgr_wda_set_data_format
(
  const char *dev_id,
  wda_set_data_format_req_msg_v01 *request,
  wda_set_data_format_resp_msg_v01 *response
)
{
  int rc;
  qmi_client_type  qmi_clnt_hndl;
  qmi_client_qmux_instance_type qcci_conn_id = 0;

  if ( !dev_id || !request || !response )
  {
    netmgr_log_err("%s(): Called with null pointers!\n", __func__);
    return NETMGR_WDA_BAD_ARGUMENTS;
  }

  rc = NETMGR_WDA_SUCCESS;

  NETMGR_LOG_SET_DATA_FORMAT_STRUCT("Setting data format", dev_id, request);

  if (NETMGR_SUCCESS !=( netmgr_qmi_map_dev_conn_instance(dev_id,
                                                          &qcci_conn_id)))
  {
    netmgr_log_err("%s(): bad device ID received\n", __func__);
    return NETMGR_WDA_BAD_ARGUMENTS;
  }

  qmi_cci_qmux_xport_register(qcci_conn_id);

  /* For a single QMUX channel there's not need to send it on a specific QMUX instance */
  if (TRUE == netmgr_main_cfg.single_qmux_ch_enabled)
  {
    qcci_conn_id = QMI_CLIENT_INSTANCE_ANY;
  }

  rmnet_wda_init_qmi_client(qcci_conn_id, &qmi_clnt_hndl);
  if ( qmi_clnt_hndl == NULL)
  {
      rc = NETMGR_WDA_QMI_FAILURE;
      goto bail;
  }
  memset(response, 0, sizeof(wda_set_data_format_resp_msg_v01));

  /* Values picked from netmgr_main_cfg */
  request->ep_id_valid = TRUE;
  request->ep_id.iface_id = netmgr_main_cfg.epid ;
  request->ep_id.ep_type = netmgr_main_cfg.ep_type;

  rc = qmi_client_send_msg_sync(qmi_clnt_hndl,
                                QMI_WDA_SET_DATA_FORMAT_REQ_V01,
                                request,
                                sizeof(wda_set_data_format_req_msg_v01),
                                response,
                                sizeof(wda_set_data_format_resp_msg_v01),
                                NETMGR_QMI_TIMEOUT);
  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): Error sending QMI WDA_SET_DATA_FORMAT message: %d", __func__, rc);
    rc = NETMGR_WDA_QMI_FAILURE;
    goto bail;
  }

  /* Validate response from modem */
  if (    (request->qos_format_valid &&                        (response->qos_format != request->qos_format))
       || (request->link_prot_valid &&                         (response->link_prot != request->link_prot))
       || (request->ul_data_aggregation_protocol_valid &&      (response->ul_data_aggregation_protocol != request->ul_data_aggregation_protocol))
       || (request->dl_data_aggregation_protocol_valid &&      (response->dl_data_aggregation_protocol != request->dl_data_aggregation_protocol))
       || (request->dl_data_aggregation_max_datagrams_valid && (response->dl_data_aggregation_max_datagrams != request->dl_data_aggregation_max_datagrams))
       || (request->dl_data_aggregation_max_size_valid &&      (response->dl_data_aggregation_max_size != request->dl_data_aggregation_max_size))
       || (request->qos_header_format_valid &&                 (response->qos_header_format != request->qos_header_format))
       || (request->dl_minimum_padding_valid &&                (response->dl_minimum_padding != request->dl_minimum_padding))
       || (request->flow_control_valid &&                      (response->flow_control != request->flow_control)) )
  {
    NETMGR_LOG_SET_DATA_FORMAT_STRUCT("Modem rejected data format. Responded with", dev_id, response);
    rc = NETMGR_WDA_MODEM_REJECTED;
  }
  else
    rc = NETMGR_WDA_SUCCESS;

bail:
  qmi_cci_qmux_xport_unregister(qcci_conn_id);
  rmnet_wda_release_qmi_client(&qmi_clnt_hndl);
  return rc;
}

/*===========================================================================
  FUNCTION  netmgr_wda_set_qmap_format
===========================================================================*/
/*!
@brief
  Sets QMAP format parameters with WDA_SET_DATA_FORMAT message

@arg *dev_id Name of device to call WDA_SET_DATA_FORMAT on
@arg *request Structure of available QMAP format parameters
@arg *response Structure of available QMAP format parameters returned

@return
  int - NETMGR_WDA_SUCCESS on successful operation,
        NETMGR_WDA_BAD_ARGUMENTS if called with null/invalid arguments
        NETMGR_WDA_QMI_FAILURE if there was an error sending QMI message
        NETMGR_WDA_MODEM_REJECTED if modem rejected the WDA format
*/
/*=========================================================================*/
int netmgr_wda_set_qmap_settings
(
  const char *dev_id,
  wda_set_qmap_settings_req_msg_v01  *request,
  wda_set_qmap_settings_resp_msg_v01 *response
)
{
  int rc;
  qmi_client_type  qmi_clnt_hndl;
  qmi_client_qmux_instance_type qcci_conn_id = 0;

  if ( !dev_id || !request || !response )
  {
    netmgr_log_err("%s(): Called with null pointers!\n", __func__);
    return NETMGR_WDA_BAD_ARGUMENTS;
  }

  rc = NETMGR_WDA_SUCCESS;

  NETMGR_LOG_SET_QMAP_SETTINGS_STRUCT("Setting QMAP settings", dev_id, request);

  if (NETMGR_SUCCESS !=( netmgr_qmi_map_dev_conn_instance(dev_id,
                                                          &qcci_conn_id)))
  {
    netmgr_log_err("%s(): bad device ID received\n", __func__);
    return NETMGR_WDA_BAD_ARGUMENTS;
  }

  qmi_cci_qmux_xport_register(qcci_conn_id);

  /* For a single QMUX channel there's not need to send it on a specific QMUX instance */
  if (TRUE == netmgr_main_cfg.single_qmux_ch_enabled)
  {
    qcci_conn_id = QMI_CLIENT_INSTANCE_ANY;
  }

  rmnet_wda_init_qmi_client(qcci_conn_id, &qmi_clnt_hndl);
  if ( qmi_clnt_hndl == NULL)
  {
      return NETMGR_WDA_QMI_FAILURE;
  }

  /* Values picked from netmgr_main_cfg */
  request->ep_id_valid = TRUE;
  request->ep_id.iface_id = netmgr_main_cfg.epid ;
  request->ep_id.ep_type = netmgr_main_cfg.ep_type;

  memset(response, 0, sizeof(wda_set_qmap_settings_resp_msg_v01));
  rc = qmi_client_send_msg_sync(qmi_clnt_hndl,
                                QMI_WDA_SET_QMAP_SETTINGS_REQ_V01,
                                request,
                                sizeof(wda_set_qmap_settings_req_msg_v01),
                                response,
                                sizeof(wda_set_qmap_settings_resp_msg_v01),
                                NETMGR_QMI_TIMEOUT);
  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): Error sending QMI WDA_SET_QMAP_SETTINGS message: %d", __func__, rc);
    return NETMGR_WDA_QMI_FAILURE;
  }

  /* Validate response from modem */
  if ((request->in_band_flow_control_valid && (response->in_band_flow_control != request->in_band_flow_control)))
  {
    NETMGR_LOG_SET_QMAP_SETTINGS_STRUCT("Modem rejected data format. Responded with", dev_id, response);
    rc = NETMGR_WDA_MODEM_REJECTED;
  }
  else
    rc = NETMGR_WDA_SUCCESS;

  qmi_cci_qmux_xport_unregister(qcci_conn_id);
  rmnet_wda_release_qmi_client(&qmi_clnt_hndl);
  return rc;
}

/*===========================================================================
  FUNCTION netmgr_wda_get_data_format
===========================================================================*/
/*!
@brief
  Gets link data format with WDA_GET_DATA_FORMAT message

@arg *dev_id Name of device to call WDA_GET_DATA_FORMAT on
@arg *response Structure of WDA data format parameters which the modem
      replied with

@return
  int - NETMGR_WDA_SUCCESS on successful operation,
  NETMGR_WDA_BAD_ARGUMENTS if called with null/invalid arguments
  NETMGR_WDA_QMI_FAILURE if there was an error sending QMI message
*/
/*=========================================================================*/
int netmgr_wda_get_data_format
(
  const char *dev_id,
  uint8_t use_alt_ep_id,
  data_ep_id_type_v01 *alt_ep_id,
  wda_get_data_format_resp_msg_v01 *response
)
{
  int rc;
  wda_get_data_format_req_msg_v01 request;
  qmi_client_type  qmi_clnt_hndl;
  qmi_client_qmux_instance_type qcci_conn_id = 0;

  memset(&request, 0, sizeof(wda_get_data_format_req_msg_v01));

  if ( !dev_id || !response )
  {
    netmgr_log_err("%s(): Called with null pointers!\n", __func__);
    return NETMGR_WDA_BAD_ARGUMENTS;
  }

  if (use_alt_ep_id && alt_ep_id)
  {
    request.ep_id_valid = 1;
    memcpy(&request.ep_id, alt_ep_id, sizeof(data_ep_id_type_v01));
  }

  rc = NETMGR_WDA_SUCCESS;

  if (NETMGR_SUCCESS !=(netmgr_qmi_map_dev_conn_instance(dev_id,
                                                         &qcci_conn_id)))
  {
    netmgr_log_err("%s(): bad device ID received\n", __func__);
    return NETMGR_WDA_BAD_ARGUMENTS;
  }

  qmi_cci_qmux_xport_register(qcci_conn_id);

  /* For a single QMUX channel there's not need to send it on a specific QMUX instance */
  if (TRUE == netmgr_main_cfg.single_qmux_ch_enabled)
  {
    qcci_conn_id = QMI_CLIENT_INSTANCE_ANY;
  }

  rmnet_wda_init_qmi_client(qcci_conn_id, &qmi_clnt_hndl);

  if ( qmi_clnt_hndl == NULL)
  {
    rc = NETMGR_WDA_QMI_FAILURE;
    goto bail;
  }

  /* Values picked from netmgr_main_cfg */
  request.ep_id_valid = TRUE;
  request.ep_id.iface_id = netmgr_main_cfg.epid ;
  request.ep_id.ep_type = netmgr_main_cfg.ep_type;

  memset(response, 0, sizeof(wda_get_data_format_resp_msg_v01));
  rc = qmi_client_send_msg_sync(qmi_clnt_hndl,
                                QMI_WDA_GET_DATA_FORMAT_REQ_V01,
                                &request,
                                sizeof(wda_get_data_format_req_msg_v01),
                                response,
                                sizeof(wda_get_data_format_resp_msg_v01),
                                NETMGR_QMI_TIMEOUT);
  if (QMI_NO_ERR != rc)
  {
    netmgr_log_err("%s(): Error sending QMI WDA_GET_DATA_FORMAT message: %d",
                   __func__, rc);
    rc = NETMGR_WDA_QMI_FAILURE;
    goto bail;
  }
  rc = NETMGR_WDA_SUCCESS;

  netmgr_log_low("%s(): Modem returns [qos_format_valid:qos_format][%d:%d]\n",
                 __func__, response->qos_format_valid, response->qos_format);
  netmgr_log_low("%s(): Modem returns [link_prot_valid:link_prot][%d:%d]\n",
                 __func__, response->link_prot_valid, response->link_prot);
  netmgr_log_low("%s(): Modem returns [ul_data_aggregation_protocol_valid:ul_data_aggregation_protocol][%d:%d]\n",
                 __func__, response->ul_data_aggregation_protocol_valid, response->ul_data_aggregation_protocol);
  netmgr_log_low("%s(): Modem returns [dl_data_aggregation_protocol_valid:dl_data_aggregation_protocol][%d:%d]\n",
                 __func__, response->dl_data_aggregation_protocol_valid, response->dl_data_aggregation_protocol);
  netmgr_log_low("%s(): Modem returns [ndp_signature_valid:ndp_signature][%d:%d]\n",
                 __func__, response->ndp_signature_valid, response->ndp_signature);
  netmgr_log_low("%s(): Modem returns [dl_data_aggregation_max_datagrams_valid:dl_data_aggregation_max_datagrams][%d:%d]\n",
                 __func__, response->dl_data_aggregation_max_datagrams_valid, response->dl_data_aggregation_max_datagrams);
  netmgr_log_low("%s(): Modem returns [dl_data_aggregation_max_size_valid:dl_data_aggregation_max_size][%d:%d]\n",
                 __func__, response->dl_data_aggregation_max_size_valid, response->dl_data_aggregation_max_size);
  netmgr_log_low("%s(): Modem returns [ul_data_aggregation_max_datagrams_valid:ul_data_aggregation_max_datagrams][%d:%d]\n",
                 __func__, response->ul_data_aggregation_max_datagrams_valid, response->ul_data_aggregation_max_datagrams);
  netmgr_log_low("%s(): Modem returns [ul_data_aggregation_max_size_valid:ul_data_aggregation_max_size][%d:%d]\n",
                 __func__, response->ul_data_aggregation_max_size_valid, response->ul_data_aggregation_max_size);
  netmgr_log_low("%s(): Modem returns [qos_header_format_valid:qos_header_format][%d:%d]\n",
                 __func__, response->qos_header_format_valid, response->qos_header_format);
  netmgr_log_low("%s(): Modem returns [dl_minimum_padding_valid:dl_minimum_padding][%d:%d]\n",
                 __func__, response->dl_minimum_padding_valid, response->dl_minimum_padding);
  netmgr_log_low("%s(): Modem returns [flow_control_valid:flow_control][%d:%d]\n",
                 __func__, response->flow_control_valid, response->flow_control);

bail:
  qmi_cci_qmux_xport_unregister(qcci_conn_id);
  rmnet_wda_release_qmi_client(&qmi_clnt_hndl);
  return NETMGR_WDA_SUCCESS;
}

/*===========================================================================
  FUNCTION netmgr_wda_get_ul_agg_modem_limits
===========================================================================*/
/*!
@brief
  Gets maximum supported UL aggregation parameters from the modem
  Values are set to 0 if QMI messaging fails

@arg *dev_id Name of device to call WDA_GET_DATA_FORMAT on
@arg *ul_data_aggregation_max_datagrams
@arg *ul_data_aggregation_max_size

@return
  int - NETMGR_WDA_SUCCESS on successful operation,
  NETMGR_WDA_BAD_ARGUMENTS if called with null/invalid arguments
  NETMGR_WDA_QMI_FAILURE if there was an error sending QMI message
*/
/*=========================================================================*/
int
netmgr_wda_get_ul_agg_modem_limits
(
  const char *dev_id,
  int *ul_data_aggregation_max_datagrams,
  int *ul_data_aggregation_max_size
)
{
  int rc;
  wda_get_data_format_resp_msg_v01 modem_dataformat;

  if (ul_data_aggregation_max_size)
    *ul_data_aggregation_max_size = 0;

  if (ul_data_aggregation_max_datagrams)
    *ul_data_aggregation_max_datagrams = 0;

  rc = netmgr_wda_get_data_format (dev_id, 0, 0, &modem_dataformat);
  if (NETMGR_WDA_SUCCESS != rc)
    return rc;

  if (ul_data_aggregation_max_size && modem_dataformat.ul_data_aggregation_max_size_valid)
    *ul_data_aggregation_max_size = modem_dataformat.ul_data_aggregation_max_size;

  if (ul_data_aggregation_max_datagrams && modem_dataformat.ul_data_aggregation_max_datagrams_valid)
    *ul_data_aggregation_max_datagrams = modem_dataformat.ul_data_aggregation_max_datagrams;

  return NETMGR_WDA_SUCCESS;
}
