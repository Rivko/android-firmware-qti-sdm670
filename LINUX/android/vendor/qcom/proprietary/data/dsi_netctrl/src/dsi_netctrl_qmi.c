/******************************************************************************

                     D S I _ N E T C T R L _ Q M I . C

******************************************************************************/

/******************************************************************************

  @file    dsi_netctrl_qmi.c
  @brief   Common QMI functions for DSI NETCTRL

  DESCRIPTION
  Common QMI functions for DSI NETCTRL

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "dsi_netctrli.h" /* declarations shared internally */
#include "qmi_platform.h"
#include "qmi_util.h"

#define DSI_QMI_RESULT_CODE_TYPE_ID       0x0002

/*===========================================================================
                     GLOBAL FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  dsi_qmi_map_dev_conn_instance
===========================================================================*/
/*!
@brief
  Does the mapping of device id to instance id

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_map_dev_conn_instance
(
  const char                      *dev_id,
  qmi_client_qmux_instance_type   *conn_id
)
{
  int ret = DSI_ERROR;
  int mux_id = -1;
  int ep_type = -1;
  int epid = -1;

  DSI_LOG_ENTRY;

  if ( !dev_id || !conn_id )
  {
    DSI_LOG_ERROR("%s(): Called with null pointers!\n", __func__);
    goto bail;
  }

  switch (QMI_PLATFORM_DEV_NAME_TO_CONN_ID_EX(dev_id, &ep_type, &epid, &mux_id))
  {
    case QMI_CONN_ID_RMNET_0:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_0;
      break;
    case QMI_CONN_ID_RMNET_1:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_1;
      break;
    case QMI_CONN_ID_RMNET_2:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_2;
      break;
    case QMI_CONN_ID_RMNET_3:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_3;
      break;
    case QMI_CONN_ID_RMNET_4:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_4;
      break;
    case QMI_CONN_ID_RMNET_5:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_5;
      break;
    case QMI_CONN_ID_RMNET_6:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_6;
      break;
    case QMI_CONN_ID_RMNET_7:
      *conn_id = QMI_CLIENT_QMUX_RMNET_INSTANCE_7;
      break;

    case QMI_CONN_ID_RMNET_MDM_0:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_0;
      break;
    case QMI_CONN_ID_RMNET_MDM_1:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_1;
      break;
    case QMI_CONN_ID_RMNET_MDM_2:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_2;
      break;
    case QMI_CONN_ID_RMNET_MDM_3:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_3;
      break;
    case QMI_CONN_ID_RMNET_MDM_4:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_4;
      break;
    case QMI_CONN_ID_RMNET_MDM_5:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_5;
      break;
    case QMI_CONN_ID_RMNET_MDM_6:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_6;
      break;
    case QMI_CONN_ID_RMNET_MDM_7:
      *conn_id = QMI_CLIENT_QMUX_RMNET_USB_INSTANCE_7;
      break;

    case QMI_CONN_ID_RMNET_MHI_0:
      *conn_id = QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_0;
      break;
    case QMI_CONN_ID_RMNET_MHI_1:
      *conn_id = QMI_CLIENT_QMUX_RMNET_MHI_INSTANCE_1;
      break;

    default:
      DSI_LOG_ERROR("Invalid connection ID %s passed no match received", dev_id);
      goto bail;
    break;
  }
  ret = DSI_SUCCESS;

bail:
  DSI_LOG_EXIT;
  return ret;
}


/*===========================================================================
  FUNCTION  dsi_qmi_srvc_extract_return_code
===========================================================================*/
/*!
@brief
  Utility function to extract the error code TLV values from the given buffer

@return
  QMI_NO_ERR if operation was sucessful
  QMI_INTERNA_ERR on failure
*/
/*=========================================================================*/
int
dsi_qmi_srvc_extract_return_code
(
  unsigned char          *msg,
  int                     msg_size,
  qmi_response_type_v01  *resp
)
{
  unsigned long type;
  unsigned long length;
  unsigned char *value_ptr;
  uint16_t result;
  uint32_t error;
  int rc = QMI_INTERNAL_ERR;

  if (!msg || !resp)
  {
    DSI_LOG_ERROR("%s(): invalid input", __func__);
    goto bail;
  }

  memset(resp, 0, sizeof(*resp));

  while (msg_size > 0)
  {
    if (qmi_util_read_std_tlv (&msg,
                               &msg_size,
                               &type,
                               &length,
                               &value_ptr) < 0)
    {
      goto bail;
    }

    /* Read the result and error code values */
    if (DSI_QMI_RESULT_CODE_TYPE_ID == type)
    {
      READ_16_BIT_VAL (value_ptr,result);
      resp->result = (qmi_result_type_v01) result;
      READ_16_BIT_VAL (value_ptr,error);
      resp->error = (qmi_error_type_v01) error;

      rc = QMI_NO_ERR;
      break;
    }
  }

bail:
  return rc;
}
