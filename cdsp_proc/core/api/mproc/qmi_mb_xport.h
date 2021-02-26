#ifndef QMI_MB_XPORT_H
#define QMI_MB_XPORT_H
/******************************************************************************
  @file    qmi_mb_xport.h
  @brief   The QMI Message Bus (MB) Xport Header file.

  DESCRIPTION
  QMI MB xport routines for different transport implementations
  for initializing and handling messages

  ---------------------------------------------------------------------------
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/

#include "qmi_mb.h"

/*=============================================================================
  CALLBACK FUNCTION qmi_mb_open_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QMB infrastructure to open a new
  transport

@param[in]   xport_data          Opaque parameter to the xport (e.g. port ID)
@param[in]   xport               Pointer to infrastructure's transport struct.
                                 (Must be used for up calls)
@param[in]   service_id          Service ID of the bus
@param[in]   version             Version of the service
@param[in]   subgroup            Subgroup of the bus. Usually 0.
@param[in]   max_rx_len          Maximum length of messages that can be
                                 received. Used by xport to allocate a buffer
                                 if the underlying transport cannot pass the
                                 message through a callback.

@retval      Opaque handle to the transport. NULL on failure.

*/
/*=========================================================================*/
typedef void *(*qmi_mb_open_fn_type)
(
 void *xport_data,
 void *xport,
 uint32_t service_id,
 uint32_t version,
 uint32_t subgroup,
 uint32_t max_rx_len
);

/*=============================================================================
  CALLBACK FUNCTION qmi_mb_reg_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QMB infrastructure to register
  or unregister with a message bus.

@param[in]   handle              Opaque handle returned by the open call

@retval      QMI_MB_NO_ERR       Success

*/
/*=========================================================================*/
typedef qmi_mb_error_type (*qmi_mb_reg_fn_type)
(
 void *handle
);

/*=============================================================================
  CALLBACK FUNCTION qmi_mb_send_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QMB infrastructure to send data to
  an end-point.

@param[in]   handle              Opaque handle returned by the open call
@param[in]   addr                Opaque address sent to the infrastructure
                                 through the join or recv calls. If NULL,
                                 broadcast to (sub)group.
@param[in]   msg                 Pointer to message to be sent
@param[in]   msg_len             Length of the message

@retval      QMI_MB_NO_ERR       Success

*/
/*=========================================================================*/
typedef qmi_mb_error_type (*qmi_mb_send_fn_type)
(
 void *handle,
 void *addr,
 void *msg,
 uint32_t msg_len
);

/*=============================================================================
  CALLBACK FUNCTION qmi_mb_close_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QMB infrastructure to close the
  transport usually when the user releases the handle. It is crucial that the
  xport synchronize the deallocation of memory and its callback functions
  before calling qmi_mb_xport_closed() to free up the rest of the data
  associated with the service.

@param[in]   handle              Opaque handle returned by the open call

*/
/*=========================================================================*/
typedef void (*qmi_mb_close_fn_type)
(
 void *handle
);

/*=============================================================================
  CALLBACK FUNCTION qmi_mb_addr_len_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QMB infrastructure to retrieve the
  length of the (source) address of the xport.

@retval         Length of address

*/
/*=========================================================================*/
typedef uint32_t (*qmi_mb_addr_len_fn_type)
(
 void
);

/* xport ops table type */
typedef struct qmi_mb_xport_ops_s
{
  qmi_mb_open_fn_type open;
  qmi_mb_reg_fn_type  reg;
  qmi_mb_reg_fn_type  unreg;
  qmi_mb_send_fn_type send;
  qmi_mb_close_fn_type close;
  qmi_mb_addr_len_fn_type addr_len;
} qmi_mb_xport_ops_type;

/*=============================================================================
                        Xport to infrastructure up-calls
=============================================================================*/
/*=============================================================================
  FUNCTION qmi_mb_xport_start
=============================================================================*/
/*!
@brief
  This function is used to register a transport with the infrastructure

@param[in]   ops                Pointer to transport operations table
@param[in]   xport_data         Opaque data associated with the transport,
                                such as port ID or other parameters.
*/
/*=========================================================================*/
void qmi_mb_xport_start
(
 qmi_mb_xport_ops_type *ops,
 void *xport_data
 );

/*=============================================================================
  FUNCTION qmi_mb_xport_stop
=============================================================================*/
/*!
@brief
  This function is used to register a transport with the infrastructure

@param[in]   ops                Pointer to transport operations table
@param[in]   xport_data         Opaque data associated with the transport,
                                such as port ID or other parameters.
*/
/*=========================================================================*/
void qmi_mb_xport_stop
(
 qmi_mb_xport_ops_type *ops,
 void *xport_data
 );

/*=============================================================================
  FUNCTION qmi_mb_xport_join
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal to the infrastructure that
  a new subscriber has joined the bus.

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.

@retval      QMI_MB_NO_ERR      Success
*/
/*=========================================================================*/
qmi_mb_error_type qmi_mb_xport_join
(
 void *xport,
 void *addr
 );

/*=============================================================================
  FUNCTION qmi_mb_xport_leave
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal to the infrastructure that
  a new client has left the bus.

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.

@retval      QMI_MB_NO_ERR      Success
*/
/*=========================================================================*/
qmi_mb_error_type qmi_mb_xport_leave
(
 void *xport,
 void *addr
 );

/*=============================================================================
  FUNCTION qmi_mb_xport_recv
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal the infrastructure to
  process the incoming message, one at a time.

  In most cases, it is triggered by a callback to the handle_event function.

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.
@param[in]   buf                Pointer to the message buffer
@param[in]   len                Length of the message

@retval      QMI_MB_NO_ERR      Success
*/
/*=========================================================================*/
qmi_mb_error_type qmi_mb_xport_recv
(
 void *xport,
 void *addr,
 void *buf,
 uint32_t len
 );

/*=============================================================================
  FUNCTION qmi_mb_xport_disconnect
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal to the infrastructure that
  a client has disconnected from the network/xport.

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.

@retval      QMI_MB_NO_ERR      Success
*/
/*=========================================================================*/
qmi_mb_error_type qmi_mb_xport_disconnect
(
 void *xport,
 void *addr
 );

/*=============================================================================
  FUNCTION qmi_mb_xport_closed
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal to the infrastructure that
  the transport has been fully closed, no more callbacks can occur and the
  xport-specific data has been deallocated. The infrastructure will then free
  data associated with the clients and service that unregistered.

@param[in]   xport              Pointer to infrastructure's xport struct
*/
/*=========================================================================*/
void qmi_mb_xport_closed
(
 void *xport
 );

#endif /* QMI_MB_XPORT_H */
