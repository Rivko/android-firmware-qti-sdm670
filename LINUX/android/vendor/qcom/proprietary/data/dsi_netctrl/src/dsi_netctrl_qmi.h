/******************************************************************************

                        D S I _ N E T C T R L _ Q M I . H

******************************************************************************/

/******************************************************************************

  @file    dsi_netctrl_qmi.h
  @brief   DSI QMI Driver Interface header file

  DESCRIPTION
  Header file for DSI QMI Driver interface.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/******************************************************************************

                      EDIT HISTORY FOR FILE

  $Id:$

when       who        what, where, why
--------   ---        -------------------------------------------------------
02/11/10   ar         Initial version (derived from DSC file)

******************************************************************************/

#ifndef __DSI_NETCTRL_QMI_H__
#define __DSI_NETCTRL_QMI_H__

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#include "qmi_client.h"
#include "qmi.h"
#include "qmi_client_instance_defs.h"


/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

#define DSI_QMI_TIMEOUT                (10000)

typedef enum
{
  DSI_QMI_SYS_EVENT_INVALID,
  DSI_QMI_SYS_EVENT_MODEM_OUT_OF_SERVICE_IND,
  DSI_QMI_SYS_EVENT_MODEM_IN_SERVICE_IND
} dsi_qmi_sys_event_type;


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
);

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
);

#endif /* __DSI_NETCTRL_QMI_H__ */
