#ifndef QMI_MB_DEPRECATED_H
#define QMI_MB_DEPRECATED_H
/******************************************************************************
  @file    qmi_mb_deprecated.h
  @brief   The QMI message bus header file.

  DESCRIPTION
  QMI message bus public APIs which are currently deprecated

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  qmi_mb_init() needs to be called before sending or receiving of any
  service specific messages

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/api/mproc/qmi_mb_deprecated.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*=============================================================================
  DEPRECATED OPTION: SET Event Callback
=============================================================================*/
#define QMI_MB_OPTIONS_SET_EVENT_CB(options, event_cb, event_cb_data)

/*=============================================================================
  CALLBACK FUNCTION qmi_mb_msg_cb *deprecated*
=============================================================================*/
/*!
@brief
  This callback function is called by the infrastructure when a
  message is received. This callback is registered at initialization time.

@param[in]   user_handle        Handle used by the infrastructure to
                                identify different clients.
@param[in]   ep_handle          Handle for the end-point
@param[in]   msg_id             Message ID
@param[in]   msg_buf            Pointer to the message*
@param[in]   msg_buf_len        Length of the message
@param[in]   msg_cb_data        User-data

@Notes
  * Deprecated in favor of using qmi_mb_recv_msg after getting the event
    callback with the QMI_BM_EVENT_MSG.
 
  * If the raw flag was set on this handle (See QMI_MB_OPTIONS_SET_RAW_FLAG)
    then the user is responsible to decode the message. Else, this is the
    pointer to the C structure of the decoded message.

*/
/*=========================================================================*/
typedef void (*qmi_mb_msg_cb)
(
 qmi_mb_type                    user_handle,
 qmi_mb_ep_type                 ep_handle,
 unsigned int                   msg_id,
 void                           *msg_buf,
 unsigned int                   msg_buf_len,
 void                           *msg_cb_data
);

/*===========================================================================
  FUNCTION  qmi_mb_init *deprecated*
===========================================================================*/
/*!
@brief

  This function is used for initializing a message bus handle

@param[in]   service_obj        Service object
@param[in]   options            Options (See QMI_MB_OPTIONS_*)
@param[in]   msg_cb             Received message callback function
@param[in]   msg_cb_data        Received message callback user-data
@param[out]  user_handle        Handle used by the infrastructure to
                                identify different clients.
@return
  Sets the user_handle and returns QMI_MB_NO_ERR if successful,
  error code if not successful

@note

  This function is deprecated in favor of qmi_mb_create.
  qmi_mb_create deprecates the use of msg_cb in favor of event_cb 
  to get notifications of incoming messages and allows the user to 
  receive the message in a different context using qmi_mb_recv_msg
  (See qmi_mb_msg_recv from more info).

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
qmi_mb_error_type
qmi_mb_init
(
 qmi_idl_service_object_type service_obj,
 qmi_mb_options *options,
 qmi_mb_msg_cb msg_cb,
 void *msg_cb_data,
 qmi_mb_type *user_handle
);

#endif /* QMI_MB_DEPRECATED_H  */

