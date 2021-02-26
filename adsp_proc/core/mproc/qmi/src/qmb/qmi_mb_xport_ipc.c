/******************************************************************************
  @file    qmi_mb_xport_ipc.h
  @brief   The QMI Message Bus (QMB) transport adapter for IPC Router

  DESCRIPTION
  QMI Message Bus transport adapter module for IPC router
   
  ---------------------------------------------------------------------------
  Copyright (c) 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "qmi_mb_xport.h"
#include "qmi_mb_os.h"
#include "ipc_router_core.h"

/*===========================================================================
                      LOCAL DATATYPES
===========================================================================*/

struct qmi_mb_xport_handle
{
  void *xport;
  ipc_router_client_type *handle;
  ipc_router_service_type service;
};

/*===========================================================================
                      MODULE HELPER METHODS
===========================================================================*/

/*===========================================================================
  FUNCTION  convert_ipc_error
===========================================================================*/
/*!
@brief

  Converts errors from IPC Router to QMB

@return
  Error code

@note
  None
*/
/*=========================================================================*/
static qmi_mb_error_type convert_ipc_error(int rc)
{
  qmi_mb_error_type ret;

  switch(rc)
  {
    case IPC_ROUTER_STATUS_SUCCESS:
      ret = QMI_MB_NO_ERR;
      break;

    case IPC_ROUTER_STATUS_BUSY:
      ret = QMI_MB_BUSY_ERR;
      break;

    case IPC_ROUTER_STATUS_NOPERM:
      ret = QMI_MB_PERM_ERR;
      break;

    default:
      ret = QMI_MB_INTERNAL_ERR;
      break;
  }
  return ret;
}


/*===========================================================================
                   CALLBACKS REGISTERED WITH IPC ROUTER
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_mb_rx_cb
===========================================================================*/
/*!
@brief

  Called by the router when there is a new message

@return
  None

@note
  None
*/
/*=========================================================================*/
static void qmi_mb_ipc_rx_cb
(
 ipc_router_client_type     *client,
 void                       *cb_data,
 ipc_router_packet_type     *msg,
 ipc_router_address_type    *src_addr,
 uint32                      len,
 boolean                     confirm_rx
 )
{
  void *buf;
  struct qmi_mb_xport_handle *xp = (struct qmi_mb_xport_handle *)cb_data;

  if(confirm_rx)
  {
    ipc_router_core_send_conf(client, src_addr);
  }

  if(!xp)
  {
    ipc_router_packet_free(&msg);
    return;
  }
  buf = ipc_router_packet_peek(msg, len);
  if(buf)
  {
    qmi_mb_xport_recv(xp->xport, src_addr, buf, len);
  }
  else
  {
    buf = MALLOC(len);
    if(buf)
    {
      ipc_router_packet_read(&msg, buf, len);
      qmi_mb_xport_recv(xp->xport, src_addr, buf, len);
      FREE(buf);
    }
    else
    {
      /* TODO Log error */
    }
  }
  ipc_router_packet_free(&msg);
}

/*===========================================================================
  FUNCTION  qmi_mb_event_cb
===========================================================================*/
/*!
@brief

  Called by the router when there are new events

@return
  None

@note
  None
*/
/*=========================================================================*/
static void qmi_mb_ipc_event_cb
(
 ipc_router_client_type *client,
 void *cb_data,
 ipc_router_event_type event,
 ipc_router_event_info_type *info
 )
{
  struct qmi_mb_xport_handle *xp = (struct qmi_mb_xport_handle *)cb_data;

  if(!xp)
    return;

  switch(event)
  {
    case IPC_ROUTER_EVENT_NEW_SERVER:
      if(xp->service.type == info->server.service.type && 
          xp->service.instance == info->server.service.instance)
      {
        (void)qmi_mb_xport_join(xp->xport, &info->server.addr);
      }
      break;
    case IPC_ROUTER_EVENT_REMOVE_SERVER:
      if(xp->service.type == info->server.service.type && 
          xp->service.instance == info->server.service.instance)
      {
        (void)qmi_mb_xport_leave(xp->xport, &info->server.addr);
      }
      break;
    case IPC_ROUTER_EVENT_REMOVE_CLIENT:
      (void)qmi_mb_xport_disconnect(xp->xport, &info->client);
      break;

    case IPC_ROUTER_EVENT_CLOSED:
      qmi_mb_xport_closed(xp->xport);
      FREE(xp);
      break;
    default:
      break;
  }
}

/*===========================================================================
                           TRANSPORT METHODS
===========================================================================*/

/*===========================================================================
  FUNCTION  xport_open
===========================================================================*/
/*!
@brief

  Handles open() down calls for each new QMB user

@return
  valid handle for each user, NULL upon failure

@note
  None
*/
/*=========================================================================*/
static void *xport_open
(
 void *xport_data,
 void *cb_handle,
 uint32_t service_id,
 uint32_t version,
 uint32_t subgroup,
 uint32_t max_rx_len
)
{
  struct qmi_mb_xport_handle *xport;
  int rc;

  if(((version & 0xff) != version) ||
     ((subgroup & 0xffff) != subgroup))
  {
    return NULL;
  }

  xport = CALLOC(1, sizeof(*xport));
  if(!xport)
  {
    return NULL;
  }
  xport->xport = cb_handle;
  xport->service.type = service_id;
  /* Set first byte  to 0xff so QMI clients wont 
   * accidentally connect to this assuming that it 
   * is a service. Also allows users to use the 
   * same service obj for QCCI, QCSI & QMB */
  xport->service.instance  = 0xff; 
  xport->service.instance |= (version << 8);
  xport->service.instance |= (subgroup << 16);

  rc = ipc_router_core_open(&xport->handle, 0,
                            qmi_mb_ipc_rx_cb, xport,
                            qmi_mb_ipc_event_cb, xport);
  if(rc != IPC_ROUTER_STATUS_SUCCESS)
  {
    FREE(xport);
    return NULL;
  }
  return xport;
}

/*===========================================================================
  FUNCTION  xport_reg
===========================================================================*/
/*!
@brief

  Handles reg() down calls for each QMB user's join() request

@return
  0 on success, error code on failure

@note
  None
*/
/*=========================================================================*/
static qmi_mb_error_type xport_reg
(
  void *handle 
)
{
  struct qmi_mb_xport_handle *xport = (struct qmi_mb_xport_handle *)handle;
  int rc;

  if(!xport)
    return QMI_MB_PARAM_ERR;

  rc = ipc_router_core_reg_server(xport->handle, &xport->service);
  return convert_ipc_error(rc);
}

/*===========================================================================
  FUNCTION  xport_unreg
===========================================================================*/
/*!
@brief

  Handles unreg() down calls for each QMB user's leave() request

@return
  0 on success, error code on failure

@note
  None
*/
/*=========================================================================*/
static qmi_mb_error_type xport_unreg
(
  void *handle 
)
{
  struct qmi_mb_xport_handle *xport = (struct qmi_mb_xport_handle *)handle;
  int rc;

  if(!xport)
    return QMI_MB_PARAM_ERR;

  rc = ipc_router_core_dereg_server(xport->handle, &xport->service);
  return convert_ipc_error(rc);
}

/*===========================================================================
  FUNCTION  xport_send
===========================================================================*/
/*!
@brief

  Handles send() down calls for each new QMB user's publish() or send()  
  request

@return
  0 on success, error code on failure

@note
  None
*/
/*=========================================================================*/
static qmi_mb_error_type xport_send
(
  void *handle,
  void *addr,
  void *msg,
  uint32_t msg_len
)
{
  struct qmi_mb_xport_handle *xport = (struct qmi_mb_xport_handle *)handle;
  int rc;

  if(!xport)
    return QMI_MB_PARAM_ERR;

  if(addr)
  {
    rc = ipc_router_core_send(xport->handle, addr, msg, msg_len);
  }
  else
  {
    rc = ipc_router_core_send_to_name(xport->handle, &xport->service,
                                      msg, msg_len);
  }

  return convert_ipc_error(rc);
}

/*===========================================================================
  FUNCTION  xport_close
===========================================================================*/
/*!
@brief

  Handles close() down calls for each new QMB user's release() request

@return
  None

@note
  None
*/
/*=========================================================================*/
static void xport_close
(
  void *handle 
)
{
  struct qmi_mb_xport_handle *xport = (struct qmi_mb_xport_handle *)handle;
  if(xport)
  {
    ipc_router_core_close(xport->handle);
  }
}

/*===========================================================================
  FUNCTION  xport_addr_len
===========================================================================*/
/*!
@brief

  Handles addr_len() down calls and return's the address length of this
  transport

@return
  None

@note
  None
*/
/*=========================================================================*/
static uint32_t xport_addr_len(void)
{
  return sizeof(ipc_router_address_type);
}

/*===========================================================================
  FUNCTION TABLE qmb_ipc_ops
===========================================================================*/
/*!
@brief

  Group of transport methods for this transport adaptation layer.
  This must be registered with QMB via:
  qmi_mb_xport_start(&qmb_ipc_ops, 0);
*/
/*=========================================================================*/
qmi_mb_xport_ops_type qmb_ipc_ops = 
{
  xport_open,
  xport_reg,
  xport_unreg,
  xport_send,
  xport_close,
  xport_addr_len
};

