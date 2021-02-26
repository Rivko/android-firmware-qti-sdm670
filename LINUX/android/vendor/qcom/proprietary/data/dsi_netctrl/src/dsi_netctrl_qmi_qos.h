/******************************************************************************

                       DSI_NETCTRL_WDS. H

******************************************************************************/

/******************************************************************************

  @file    dsi_netctrl_qmi_qos.h
  @brief   DSI QMI data filter service header

  DESCRIPTION
  DSI QMI data filter service header

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015 Qualcomm Technologies, Inc. All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/
#ifndef DSI_NETCTRL_QMI_QOS_H
#define DSI_NETCTRL_QMI_QOS_H

#include "qmi_client.h"
#include "qmi.h"

#include "quality_of_service_v01.h"

/*===========================================================================
  FUNCTION  dsi_qmi_qos_init_qmi_client
===========================================================================*/
/*!
@brief
  Initialize QMI QOS handle

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_init_qmi_client
(
  const char                     *dev_str,
  void                           *ind_cb_data,
  qos_bind_subscription_enum_v01  subs_id,
  qmi_client_type                *clnt_handle
);


/*===========================================================================
  FUNCTION  dsi_qmi_qos_release_qmi_client
===========================================================================*/
/*!
@brief
  Release QMI QOS handle

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_qos_release_qmi_client
(
  qmi_client_type          clnt_hndl
);

/*===========================================================================
    FUNCTION  dsi_qmi_qos_set_client_ip_pref
===========================================================================*/
/*!
@brief
  Set IP family preference for a client

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_set_client_ip_pref
(
  qmi_client_type          clnt_handle,
  qos_ip_family_enum_v01   ip_family
);


/*===========================================================================
    FUNCTION  dsi_qmi_qos_request_qos
===========================================================================*/
/*!
@brief
  Send QOS request to the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_request_qos
(
  qmi_client_type             clnt_handle,
  int                         num_qos_specs,
  qmi_qos_spec_type          *qos_spec_array,
  qmi_qos_req_opcode_type     req_opcode_type,
  unsigned long              *qos_id_array,    /* Valid if rc == QMI_NO_ERR */
  qmi_qos_err_rsp_type       *qos_spec_errs   /* Valid if rc == QMI_SERVICE_ERR */
);


/*===========================================================================
    FUNCTION  dsi_qmi_qos_release_qos
===========================================================================*/
/*!
@brief
  Send QOS release to the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_release_qos
(
  qmi_client_type          clnt_handle,
  int                      num_qos_ids,
  unsigned long           *qos_id_array
);


/*===========================================================================
    FUNCTION  dsi_qmi_qos_modify_qos
===========================================================================*/
/*!
@brief
  Send QOS modify to the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_modify_qos
(
  qmi_client_type          clnt_handle,
  boolean                  is_primary,
  unsigned int             num_qos_specs,
  qmi_qos_spec_type       *qos_spec_list,
  qmi_qos_err_rsp_type    *qos_spec_err
);

/*===========================================================================
    FUNCTION  dsi_qmi_qos_suspend_qos
===========================================================================*/
/*!
@brief
  Send QOS suspend to the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_suspend_qos
(
  qmi_client_type          clnt_handle,
  unsigned char            num_qos_ids,
  unsigned long           *qos_id_array
);

/*===========================================================================
    FUNCTION  dsi_qmi_qos_resume_qos
===========================================================================*/
/*!
@brief
  Send QOS resume to the modem

@return
  int - DSI_SUCCESS   on successful operation
        DSI_ERROR   if there was an error sending QMI message

*/
/*=========================================================================*/
int
dsi_qmi_qos_resume_qos
(
  qmi_client_type          clnt_handle,
  unsigned char            num_qos_ids,
  unsigned long            *qos_id_array
);

/*===========================================================================
  FUNCTION  dsi_qmi_qos_get_granted_qos
===========================================================================*/
/*!
@brief
  Return the QoS parameters for a particular QOS flow.

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_qos_get_granted_qos
(
  qmi_client_type                clnt_hndl,
  boolean                        is_primary,
  uint32                         flow_id,
  qmi_qos_granted_info_rsp_type  *granted_info
);


/*===========================================================================
  FUNCTION  dsi_qmi_qos_get_status
===========================================================================*/
/*!
@brief
  Get the QOS status from the modem

@return
  int - DSI_SUCCESS on successful operation,
        DSI_ERROR if there was an error sending QMI message

*/
/*=========================================================================*/
int dsi_qmi_qos_get_status
(
  qmi_client_type                clnt_hndl,
  unsigned long                  qos_identifier,
  qmi_qos_status_info           *status_resp
);

#endif /* DSI_NETCTRL_QMI_QOS_H */
