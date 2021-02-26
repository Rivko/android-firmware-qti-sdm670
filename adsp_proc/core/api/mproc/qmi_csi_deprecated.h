#ifndef QMI_CSI_DEPRECATED_H
#define QMI_CSI_DEPRECATED_H
/******************************************************************************
  @file    qmi_csi_deprecated.h
  @brief   The QMI common service Interface Header file for deprecated
           functions.

  DESCRIPTION
  This header file consists of function declaration to support legacy
  QMI Interfaces.

  ---------------------------------------------------------------------------
  Copyright (c) 2012 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/api/mproc/qmi_csi_deprecated.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/19/12   fr      Initial Version
===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


#include "qmi_csi.h"
/*=============================================================================
  FUNCTION  qmi_csi_get_txn_id
=============================================================================*/
/*!
@brief
  Gets a tansaction id corresponding to the request handle.

@param[in]  req_handle            Handle used by QCSI infrastructure to
                                  identify the transaction and the destination
                                  client.
@param[out] txn_id                Transaction ID corresponding to the message.

@retval     QMI_CSI_NO_ERR        Success
@retval     QMI_CSI_.....         Look into the enumeration qmi_csi_error for
                                  the error values.
@note       
This API will return error once the response corresponding to the req_handle
has been sent as the transaction is not valid after the response has been sent. 
*/
/*=========================================================================*/
qmi_csi_error
qmi_csi_get_txn_id
(
 qmi_req_handle     req_handle,
 unsigned int       *txn_id
);

/*=============================================================================
  FUNCTION  qmi_csi_conn_complete
=============================================================================*/
/*!
@brief
  Call into QCSI to note if a client connection is accepted or refused

@param[in]  client_handle         Handle provided in the connection_cb
@param[in]  accepted              If the client connection is accepted (1)
                                  or refused (0)

@retval     QMI_CSI_NO_ERR        Success
@retval     QMI_CSI_.....         Look into the enumeration qmi_csi_error for
                                  the error values.
*/
/*=========================================================================*/
void qmi_csi_conn_complete
(
  qmi_client_handle client_handle,
  uint8_t           accepted
);

/*=============================================================================
MACRO  QMI_CSI_OPTIONS_SET_ASYNC_CONNECT_CB
=============================================================================*/
/*!
@brief
Set the QCSI connection callback to an async method. Services must call
qmi_csi_conn_complete to complete a client connection. This option is 
deptrecated and should not be used.

@param[in]  opt                 Service options object
*/
/*=========================================================================*/
#define QMI_CSI_OPTIONS_SET_ASYNC_CONNECT_CB(opt) \
do{  \
  (opt).options_set |= QMI_CSI_OPTIONS_ASYNC_CONN_CB_VALID; \
} while(0)

#ifdef __cplusplus
}
#endif
#endif /* QMI_CSI_DEPRECATED_H */
