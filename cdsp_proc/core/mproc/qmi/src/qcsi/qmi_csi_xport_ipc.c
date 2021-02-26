/******************************************************************************
  @file    qmi_csi_xport_ipc.c
  @brief   The QMI Common Service Interface (CSI) transport adapter to
           IPC Router

  DESCRIPTION
  QMI Common Service Interface transport adapter module for IPC router
   
  ---------------------------------------------------------------------------
  Copyright (c) 2010-2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_csi_target.h"
#include "qmi_csi.h"
#include "qmi_csi_common.h"
#include "ipc_router_core.h"

/* Tx queues for handling flow control:
 * +--------+  +--------+  +--------+
 * | q head |->| dest 1 |->| dest 2 |->...
 * +--------+  +--------+  +--------+
 *                 |            |
 *                buf 1        buf 1
 *                 |            |
 *                buf 2        buf 2
 *                 |            |
 *                ...          ...
 */
typedef struct buf_s
{
  LINK(struct buf_s, link);
  void *msg;
  ipc_router_packet_type *pkt;
  uint32 len;
  ipc_router_address_type src_addr;
  boolean confirm_rx;
  boolean disconnect;
  boolean resume_tx;
  boolean async_conn;
} buf_type;

typedef struct dest_s
{
  ipc_router_address_type addr;
  boolean busy;
  LINK(struct dest_s, link);
  LIST(struct buf_s, bufs);
} dest_type;

struct qmi_csi_xport_handle
{
  qmi_csi_xport_type *xport;
  ipc_router_client_type *handle;
  qmi_csi_os_params sig;
  qmi_csi_lock_type rx_q_lock;
  LIST(struct buf_s, rx_q);
  qmi_csi_lock_type tx_q_lock;
  LIST(struct dest_s, tx_q);
  qmi_csi_xport_options_type *xport_options;
};



static dest_type *find_tx_q(struct qmi_csi_xport_handle *xp, ipc_router_address_type *addr)
{
  dest_type *dest = LIST_HEAD(xp->tx_q);
  while(dest)
  {
    if(!memcmp(addr, &dest->addr, sizeof(ipc_router_address_type)))
      break;
    dest = dest->link.next;
  }
  return dest;
}

static dest_type *get_tx_q(struct qmi_csi_xport_handle *xp, ipc_router_address_type *addr)
{
  dest_type *dest = find_tx_q(xp, addr);
  if(!dest)
  {
    dest = CALLOC(1, sizeof(dest_type));
    if(dest)
    {
      LINK_INIT(dest->link);
      LIST_INIT(dest->bufs);
      dest->addr = *addr;
      LIST_ADD(xp->tx_q, dest, link);
    }
  }
  return dest;
}

static void purge_tx_q(struct qmi_csi_xport_handle *xp, dest_type *dest)
{
  buf_type *buf = LIST_HEAD(dest->bufs);
  LIST_REMOVE(xp->tx_q, dest, link);
  while(buf)
  {
    buf_type *to_free = buf;
    FREE(buf->msg);
    buf = buf->link.next;
    FREE(to_free);
  }
  FREE(dest);
}

/* rx_q lock must be held */
static int find_in_rx_q(struct qmi_csi_xport_handle *xp, ipc_router_address_type *addr)
{
  buf_type *buf;
  LIST_FIND(xp->rx_q, buf, link, memcmp(&buf->src_addr, addr, sizeof(*addr)) == 0);
  return buf ? 1 : 0;
}

static void flush_tx_q(struct qmi_csi_xport_handle *xp, ipc_router_address_type *addr)
{
  dest_type *dest;
  buf_type *buf;

  LOCK(&xp->tx_q_lock);
  dest = find_tx_q(xp, addr);
  if(!dest)
  {
    UNLOCK(&xp->tx_q_lock);
    return;
  }

  while(NULL != (buf = LIST_HEAD(dest->bufs)))
  {
    if(IPC_ROUTER_STATUS_SUCCESS !=
        ipc_router_core_send(xp->handle, addr, buf->msg, buf->len))
    {
      /* we will not handle errors here. If the ep has 
       * terminated, we will handle it in the remove
       * client cb */
      break;
    }
    LIST_REMOVE(dest->bufs, buf, link);
    FREE(buf->msg);
    FREE(buf);
  }

  /* Allocate resume buf if required with TX Q lock held
   * so we may not set the busy flag to FALSE if allocation fails. */
  buf = NULL;
  if(LIST_CNT(dest->bufs) == 0 && dest->busy == TRUE)
  {
    buf = CALLOC(1, sizeof(*buf));
    if(buf)
    {
      dest->busy = FALSE;
      buf->resume_tx = TRUE;
      buf->src_addr = dest->addr;
      buf->msg = NULL;
      LINK_INIT(buf->link);
    }
  }
  UNLOCK(&xp->tx_q_lock);

  /* If buf is valid, then we have a resume
   * buffer to enqueue into the RX pipe */
  if(buf)
  {
    LOCK(&xp->rx_q_lock);
    LIST_ADD(xp->rx_q, buf, link);
    UNLOCK(&xp->rx_q_lock);
    QMI_CSI_OS_SIGNAL_SET(&xp->sig);
  }
}

static void qmi_csi_rx_cb
(
 ipc_router_client_type     *client,
 void                       *cb_data,
 ipc_router_packet_type     *msg,
 ipc_router_address_type    *src_addr,
 uint32                      len,
 boolean                     confirm_rx
 )
{
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)cb_data;
  buf_type *buf;

  buf = CALLOC(1, sizeof(buf_type));
  if(!buf)
  {
    ipc_router_packet_free(&msg);
    QMI_CSI_OS_LOG_ERROR();
    return;
  }

  LINK_INIT(buf->link);
  buf->msg = NULL;
  buf->pkt = msg;
  buf->len = len;
  buf->src_addr = *src_addr;
  buf->confirm_rx = confirm_rx;

  LOCK(&xp->rx_q_lock);
  LIST_ADD(xp->rx_q, buf, link);
  UNLOCK(&xp->rx_q_lock);
  QMI_CSI_OS_SIGNAL_SET(&xp->sig);

}

static void qmi_csi_event_cb
(
 ipc_router_client_type *client,
 void *cb_data,
 ipc_router_event_type event,
 ipc_router_event_info_type *info
 )
{
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)cb_data;

  if(!xp)
    return;

  switch(event)
  {
    case IPC_ROUTER_EVENT_RESUME_TX:
      flush_tx_q(xp, &info->client);
      break;
    case IPC_ROUTER_EVENT_NEW_SERVER:
      break;
    case IPC_ROUTER_EVENT_REMOVE_SERVER:
      break;
    case IPC_ROUTER_EVENT_REMOVE_CLIENT:
    {
      buf_type *buf;
      dest_type *dest;
      int conn_status;

      LOCK(&xp->tx_q_lock);
      dest = find_tx_q(xp, &info->client);
      if(dest)
        purge_tx_q(xp, dest);
      UNLOCK(&xp->tx_q_lock);

      conn_status = qmi_csi_xport_conn_status(xp->xport, &info->client);
      
      LOCK(&xp->rx_q_lock);
      /* No such client is known by the service, and no such
       * address is present in the RX Queue */
      if(conn_status == 0 && find_in_rx_q(xp, &info->client) == 0)
      {
        UNLOCK(&xp->rx_q_lock);
        break;
      }

      /* queue an event to Rx queue */
      buf = CALLOC(1, sizeof(buf_type));
      if(!buf)
      {
        UNLOCK(&xp->rx_q_lock);
        QMI_CSI_OS_LOG_ERROR();
        break;
      }
      LINK_INIT(buf->link);
      buf->src_addr = info->client;
      buf->disconnect = TRUE;
      LIST_ADD(xp->rx_q, buf, link);
      UNLOCK(&xp->rx_q_lock);
      QMI_CSI_OS_SIGNAL_SET(&xp->sig);
      break;
    }
    case IPC_ROUTER_EVENT_CLOSED:
    {
      dest_type *dest, *dest_to_free;
      buf_type *buf, *buf_to_free;

      qmi_csi_xport_closed(xp->xport);
      
      LOCK(&xp->rx_q_lock);
      buf = LIST_HEAD(xp->rx_q);
      while(buf)
      {
        buf_to_free = buf;
        if(buf->msg)
          FREE(buf->msg);
        buf = buf->link.next;
        FREE(buf_to_free);
      }
      UNLOCK(&xp->rx_q_lock);
      
      LOCK_DEINIT(&xp->rx_q_lock);

      LOCK(&xp->tx_q_lock);
      dest = LIST_HEAD(xp->tx_q);
      while(dest)
      {
        dest_to_free = dest;
        dest = dest->link.next;
        purge_tx_q(xp, dest_to_free);
      }
      UNLOCK(&xp->tx_q_lock);

      LOCK_DEINIT(&xp->tx_q_lock);
      FREE(xp);
      break;
    }
    default:
      break;
  }
}

static void *xport_open
(
 void *xport_data,
 qmi_csi_xport_type *xport,
 uint32_t max_rx_len,
 qmi_csi_os_params *os_params,
 qmi_csi_xport_options_type *options
 )
{
  int rc;
  struct qmi_csi_xport_handle *xp = CALLOC(1, sizeof(struct qmi_csi_xport_handle));

  /* Be backwards compatible with old IPC Router interface */
#ifdef IPC_ROUTER_CORE_PORT_OPTIONS_INIT
  ipc_router_core_options_type ipc_options;

  IPC_ROUTER_CORE_PORT_OPTIONS_INIT(ipc_options);
#endif

#ifdef IPC_ROUTER_CORE_PORT_OPTIONS_SET_SCOPE
  if(options && options->scope_valid)
  {
    IPC_ROUTER_CORE_PORT_OPTIONS_SET_SCOPE(ipc_options, options->scope);
  }
#endif

  if(xp)
  {
    LOCK_INIT(&xp->tx_q_lock);
    LOCK_INIT(&xp->rx_q_lock);
    LIST_INIT(xp->rx_q);
    LIST_INIT(xp->tx_q);
    xp->xport = xport;
    xp->xport_options = options;
    xp->sig = *os_params;
#ifdef IPC_ROUTER_CORE_PORT_OPTIONS_INIT
    rc = ipc_router_core_open_with_options(&xp->handle, 0, qmi_csi_rx_cb, xp, 
                                           qmi_csi_event_cb, xp, &ipc_options);
#else
    rc = ipc_router_core_open(&xp->handle, 0, qmi_csi_rx_cb, xp, qmi_csi_event_cb, xp);
#endif

    if(rc != IPC_ROUTER_STATUS_SUCCESS)
    {
      LOCK_DEINIT(&xp->tx_q_lock);
      LOCK_DEINIT(&xp->rx_q_lock);
      FREE(xp);
      return NULL;
    }
  }

  return xp;
}

static qmi_csi_error xport_reg
(
 void *handle,
 uint32_t service_id,
 uint32_t version
 )
{
  int rc;
  ipc_router_service_type service;
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)handle;

  service.type = service_id;
  service.instance = version;

  rc = ipc_router_core_reg_server(xp->handle, &service);
  if(rc != IPC_ROUTER_STATUS_SUCCESS)
  {
    return QMI_CSI_INTERNAL_ERR;
  }
  return QMI_CSI_NO_ERR;
}

static qmi_csi_error xport_unreg
(
 void *handle,
 uint32_t service_id,
 uint32_t version
 )
{
  int rc;
  ipc_router_service_type service;
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)handle;

  service.type = service_id;
  service.instance = version;

  rc = ipc_router_core_dereg_server(xp->handle, &service);
  if(rc != IPC_ROUTER_STATUS_SUCCESS)
  {
    return QMI_CSI_INTERNAL_ERR;
  }
  return QMI_CSI_NO_ERR;
}


static qmi_csi_error xport_send
(
 void *handle,
 void *addr,
 uint8_t *msg,
 uint32_t len,
 uint32_t flags,
 void **client_data
 )
{
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)handle;
  dest_type *dest;
  int rc;
  uint32_t max_queue_len = 0;

  if(0 != (flags & QMI_CSI_SEND_FLAG_RATE_LIMITED) && NULL != xp->xport_options)
  {
    max_queue_len = xp->xport_options->rate_limited_queue_size;
  }

  LOCK(&xp->tx_q_lock);

  dest = find_tx_q(xp, addr);
  if(dest && LIST_CNT(dest->bufs))
  {
    /* queue to the end so message is not sent out of order */
    rc = IPC_ROUTER_STATUS_BUSY;
  }
  else
  {
    rc = ipc_router_core_send(xp->handle, addr, msg, len);
    if(rc != IPC_ROUTER_STATUS_SUCCESS && rc != IPC_ROUTER_STATUS_BUSY)
    {
      /* unrecoverable error */
      UNLOCK(&xp->tx_q_lock);
      if (rc == IPC_ROUTER_STATUS_INVALID_ENDPOINT)
      {
        return QMI_CSI_CONN_REFUSED;
      }
      return QMI_CSI_INTERNAL_ERR;
    }
  }

  if(rc == IPC_ROUTER_STATUS_BUSY)
  {
    /* queue to tx queue */
    buf_type *buf;

    dest = get_tx_q(xp, addr);
    if(!dest)
    {
      UNLOCK(&xp->tx_q_lock);
      return QMI_CSI_INVALID_HANDLE;
    }
    if(max_queue_len > 0 && LIST_CNT(dest->bufs) >= max_queue_len)
    {
      /* too many in the queue, return error */
      dest->busy = TRUE;
      UNLOCK(&xp->tx_q_lock);
      return QMI_CSI_CONN_BUSY;
    }
    buf = CALLOC(1, sizeof(buf_type));
    if(!buf)
    {
      UNLOCK(&xp->tx_q_lock);
      return QMI_CSI_NO_MEM;
    }
    LINK_INIT(buf->link);
    buf->len = len;
    buf->msg = MALLOC(len);
    if(!buf->msg)
    {
      FREE(buf);
      UNLOCK(&xp->tx_q_lock);
      return QMI_CSI_NO_MEM;
    }
    QMI_MEM_COPY(buf->msg, len, msg, len);
    LIST_ADD(dest->bufs, buf, link);
  }
  UNLOCK(&xp->tx_q_lock);

  /* Try and flush */
  flush_tx_q(xp, (ipc_router_address_type *)addr);

  return QMI_CSI_NO_ERR;
}

static void xport_close
(
 void *handle
 )
{
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)handle;
  ipc_router_core_close(xp->handle);
}

static uint32_t xport_addr_len
(
 void
 )
{
  return sizeof(ipc_router_address_type);
}

static buf_type *dequeue_rx(struct qmi_csi_xport_handle *xp)
{
  buf_type *buf;
  LOCK(&xp->rx_q_lock);
  buf = LIST_HEAD(xp->rx_q);
  if(buf)
  {
    if(!buf->async_conn)
    {
      LIST_REMOVE(xp->rx_q, buf, link);
    }
    else
    {
      buf = NULL;
    }
  }
  UNLOCK(&xp->rx_q_lock);
  return buf;
}

static void xport_handle_event
(
 void *handle,
 qmi_csi_os_params *os_params
 )
{
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)handle;
  buf_type *buf;
  qmi_csi_error rc;

  while((buf = dequeue_rx(xp)) != NULL)
  {
    if(buf->disconnect)
    {
      qmi_csi_xport_disconnect(xp->xport, &buf->src_addr);
    }
    else if(buf->resume_tx)
    {
      qmi_csi_xport_resume_client(xp->xport, &buf->src_addr);
    }
    else
    {
      if(buf->confirm_rx)
      {
        ipc_router_core_send_conf(xp->handle, &buf->src_addr);
      }
      buf->msg = ipc_router_packet_peek(buf->pkt, buf->len);
      if(buf->msg)
      {
        /* The buffer is already linear in the packet, we don't 
         * have to linearize it */
        rc = qmi_csi_xport_recv(xp->xport, &buf->src_addr, buf->msg, buf->len);
      }
      else
      {
        buf->msg = MALLOC(buf->len);
        if(buf->msg)
        {
          /* Linearize the packet to buf->msg and then free it when we are done */
          ipc_router_packet_read(&buf->pkt, buf->msg, buf->len);
          rc = qmi_csi_xport_recv(xp->xport, &buf->src_addr, buf->msg, buf->len);
          FREE(buf->msg);
        }
        else
        {
          /* Allocation failed, just read the header and send it up
           * so the common layer can handle it as an allocation failure
           * or at best a decode error */
          uint8_t header[QMI_HEADER_SIZE] = {0};
          ipc_router_packet_read(&buf->pkt, header, QMI_HEADER_SIZE);
          rc = qmi_csi_xport_recv(xp->xport, &buf->src_addr, (void *)header, QMI_HEADER_SIZE);
        }
      }
      if(rc == QMI_CSI_DEFERRED)
      {
        buf->async_conn = 1;
        LOCK(&xp->rx_q_lock);
        if(!xp->rx_q.tail)
        {
          xp->rx_q.head = xp->rx_q.tail = buf;
        }
        else
        {
          buf->link.prev = NULL;
          buf->link.next = xp->rx_q.head;
          xp->rx_q.head->link.prev = buf;
          xp->rx_q.head = buf;
        }
        xp->rx_q.count++;
        UNLOCK(&xp->rx_q_lock);
        return;
      }
      ipc_router_packet_free(&buf->pkt);
      /* We got a message, see if we can flush the TX Queue */
      flush_tx_q(xp, &buf->src_addr);
    }
    FREE(buf);
  }
}

static void xport_conn_complete(void *handle, unsigned int accepted)
{
  uint8_t header[QMI_HEADER_SIZE] = { 0 };
  struct qmi_csi_xport_handle *xp = (struct qmi_csi_xport_handle *)handle;
  buf_type *buf;
  if(!xp)
  {
    return;
  }
  LOCK(&xp->rx_q_lock);
  buf = LIST_HEAD(xp->rx_q);  
  if(!buf || !buf->async_conn)
  {
    UNLOCK(&xp->rx_q_lock);
    return;
  }
  buf->async_conn = 0;

  if(accepted != 0)
  {
    UNLOCK(&xp->rx_q_lock);
    QMI_CSI_OS_SIGNAL_SET(&xp->sig);
    return;
  }

  LIST_REMOVE(xp->rx_q, buf, link);
  UNLOCK(&xp->rx_q_lock);
  /* Client rejected, need to send an error response */
  ipc_router_packet_read(&buf->pkt, header, QMI_HEADER_SIZE);
  qmi_csi_xport_send_err_resp(xp->xport, &buf->src_addr, (void *)header, QMI_HEADER_SIZE, QMI_CSI_CB_CONN_REFUSED);

  ipc_router_packet_free(&buf->pkt);
  FREE(buf);
  QMI_CSI_OS_SIGNAL_SET(&xp->sig);
}

qmi_csi_xport_ops_type qcsi_ipc_ops = {
  NULL, /* Legacy Open */
  xport_reg,
  xport_unreg,
  NULL, /* Legacy Send */
  xport_handle_event,
  xport_close,
  xport_addr_len,
  xport_open,
  xport_send,
  xport_conn_complete
};
