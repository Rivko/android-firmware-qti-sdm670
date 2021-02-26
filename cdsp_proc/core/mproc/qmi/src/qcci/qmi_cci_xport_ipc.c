/******************************************************************************
  @file    qmi_cci_xport_xxx.h
  @brief   The QMI Common Client Interface (CCI) transport adapter

  DESCRIPTION
  QMI Common Client transport adapter module for IPC router
   
  ---------------------------------------------------------------------------
  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <string.h>
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#include "ipc_router_core.h"
#include "stdint.h"

struct qmi_cci_xport_handle
{
  qmi_cci_client_type *clnt;
  ipc_router_client_type *handle;
  ipc_router_service_type service;
};

static void qmi_cci_rx_cb
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
  struct qmi_cci_xport_handle *xp = (struct qmi_cci_xport_handle *)cb_data;

  if(confirm_rx)
  {
    ipc_router_core_send_conf(client, src_addr);
  }

  if(!xp)
  {
    ipc_router_packet_free(&msg);
    QMI_CCI_OS_LOG_ERROR();
    return;
  }

  buf = ipc_router_packet_peek(msg, len);
  if(buf)
  {
    qmi_cci_xport_recv(xp->clnt, src_addr, buf, len);
  }
  else
  {
    buf = MALLOC(len);
    if(buf)
    {
      ipc_router_packet_read(&msg, buf, len);
      qmi_cci_xport_recv(xp->clnt, src_addr, buf, len);
      FREE(buf);
    }
    else
    {
      uint8_t header[QMI_HEADER_SIZE] = {0};
      ipc_router_packet_read(&msg, header, QMI_HEADER_SIZE);
      qmi_cci_xport_recv(xp->clnt, src_addr, header, QMI_HEADER_SIZE);
    }
  }
  ipc_router_packet_free(&msg);
}

static void qmi_cci_event_cb
(
 ipc_router_client_type *client,
 void *cb_data,
 ipc_router_event_type event,
 ipc_router_event_info_type *info
 )
{
  struct qmi_cci_xport_handle *xp = (struct qmi_cci_xport_handle *)cb_data;

  if(!xp)
    return;

  switch(event)
  {
    case IPC_ROUTER_EVENT_RESUME_TX:
      qmi_cci_xport_resume(xp->clnt);
      break;

    case IPC_ROUTER_EVENT_NEW_SERVER:
      if(xp->clnt && xp->service.type == info->server.service.type && 
          xp->service.instance == GET_VERSION(info->server.service.instance))
      {
        qmi_cci_xport_event_new_server(xp->clnt, &info->server.addr);
      }
      break;
    case IPC_ROUTER_EVENT_REMOVE_SERVER:
      if(xp->clnt && xp->service.type == info->server.service.type && 
          xp->service.instance == GET_VERSION(info->server.service.instance))
      {
        qmi_cci_xport_event_remove_server(xp->clnt, &info->server.addr);
      }
      break;
    case IPC_ROUTER_EVENT_REMOVE_CLIENT:
      break;
    case IPC_ROUTER_EVENT_CLOSED:
      qmi_cci_xport_closed(xp->clnt);
      FREE(xp);
      break;
    default:
      break;
  }
}

static void *xport_open
(
 void *xport_data,
 qmi_cci_client_type *clnt,
 uint32_t service_id,
 uint32_t version,
 void *addr,
 uint32_t max_rx_len
 )
{
  struct qmi_cci_xport_handle *xp = CALLOC(1, sizeof(struct qmi_cci_xport_handle));
  if(xp)
  {
    xp->clnt = clnt;
    xp->service.type = service_id;
    xp->service.instance = version;
    if(ipc_router_core_open(&xp->handle, 0, qmi_cci_rx_cb, xp, qmi_cci_event_cb,
          xp) != IPC_ROUTER_STATUS_SUCCESS)
    {
      FREE(xp);
      return NULL;
    }
  }
  
  return xp;
}

static qmi_client_error_type xport_send
(
 void *handle,
 void *addr,
 uint8_t *buf,
 uint32_t len
 )
{
  int rc;
  struct qmi_cci_xport_handle *xp = (struct qmi_cci_xport_handle *)handle;

  rc = ipc_router_core_send(xp->handle, addr, buf, len);
  switch(rc)
  {
    case IPC_ROUTER_STATUS_SUCCESS:
      rc = QMI_NO_ERR;
      break;

    case IPC_ROUTER_STATUS_BUSY:
      rc = QMI_XPORT_BUSY_ERR;
      break;

    case IPC_ROUTER_STATUS_INVALID_ENDPOINT:
      rc = QMI_SERVICE_ERR;
      break;

    default:
      rc  = QMI_CLIENT_TRANSPORT_ERR;
      break;
  }
  return rc;
}

static void xport_close
(
 void *handle
 )
{
  struct qmi_cci_xport_handle *xp = (struct qmi_cci_xport_handle *)handle;
  ipc_router_core_close(xp->handle);
}

static uint32_t xport_lookup
(
 void *xport_data,
 uint8_t xport_num,
 uint32_t service_id,
 uint32_t version,
 uint32_t *num_entries,
 qmi_cci_service_info *service_info
 )
{
  ipc_router_server_type *servers = NULL;
  ipc_router_service_type service;
  uint32_t num_servers, i;

  service.type = service_id;
  service.instance = version;

  if(num_entries)
  {
    servers = MALLOC(sizeof(ipc_router_server_type) * (*num_entries));
    if(!servers)
      return 0;
  }

  if(ipc_router_core_find_all_servers(NULL, &service, servers, 
        (uint32 *)num_entries, (uint32 *)&num_servers, VERSION_MASK) 
      != IPC_ROUTER_STATUS_SUCCESS)
  {
    if(servers)
      FREE(servers);
    return 0;
  }
  
  if(num_entries)
  {
    for(i = 0; i < *num_entries; i++)
    {
      service_info[i].xport = xport_num;
      service_info[i].version = GET_VERSION(servers[i].service.instance);
      service_info[i].instance = GET_INSTANCE(servers[i].service.instance);
      service_info[i].reserved = 0;
      QMI_MEM_COPY(&service_info[i].addr, MAX_ADDR_LEN, &servers[i].addr, 
          sizeof(ipc_router_address_type));
    }
    FREE(servers);
  }
  return num_servers;
}

static uint32_t xport_addr_len
(
 void
 )
{
  return sizeof(ipc_router_address_type);
}

qmi_cci_xport_ops_type qcci_ipc_ops = 
{
  xport_open,
  xport_send,
  xport_close,
  xport_lookup,
  xport_addr_len
};
 
