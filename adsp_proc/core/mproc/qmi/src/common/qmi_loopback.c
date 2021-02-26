/******************************************************************************
  @file    qmi_loopback.c
  @brief   The QMI Common Service/Client Interface (CSI/CCI) loopback 
           transport adapter

  DESCRIPTION
  QMI Common Service/Client Interface transport adapter module for loopback
   
  ---------------------------------------------------------------------------
  Copyright (c) 2012-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/

/*===========================================================================

                                                EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.qdsp6/2.1/mproc/qmi/src/common/qmi_loopback.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/30/12   dc      Initial Version

===========================================================================*/
#include <string.h>
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#undef LOCK_INIT /* Make sure the QCCI lock is not used */
#include "qmi_csi_common.h"
#include "qmi_csi_target.h"
#include "qmi_csi.h"

typedef struct buf_s
{
  LINK(struct buf_s, link);
  void    *msg;
  uint32   len;

  /* Carries clnt loop back xport handle */
  struct qmi_cci_lb_xport_handle *clnt_handle;
  uint32 disconnect;
} buf_type;

/* QCCI LOOP BACK XPORT HANDLE */
struct qmi_cci_lb_xport_handle
{
  LINK(struct qmi_cci_lb_xport_handle, link);
  qmi_cci_client_type       *clnt;
  uint32_t                   service_id;
  uint32_t                   version;
  uintptr_t                  svc_addr; /* 0 if not connected to any service */
};

/* QCSI LOOP BACK XPORT HANDLE */
struct qmi_csi_lb_xport_handle
{
  qmi_csi_xport_type  *xport;
  qmi_csi_os_params    sig;
  qmi_csi_lock_type    rx_q_lock;
  LIST(struct buf_s,   rx_q);
};

/* Used in service loopback table */
typedef struct qmi_server_struct_type
{
  LINK(struct qmi_server_struct_type, link);
  struct qmi_csi_lb_xport_handle *handle;
  uint32_t                        service_id;
  uint32_t                        version;
} qmi_server_struct;

/*****************************************************************************/
/* LOOPBACK GLOBAL VARIABLES                                                 */
/*****************************************************************************/

#define QMI_LOOPBACK_SVC_TBL_SIZE (8)
#define SVC2IDX(svc_id) ((svc_id) & (QMI_LOOPBACK_SVC_TBL_SIZE - 1))
static LIST(qmi_server_struct, qmi_loopback_server_tbl)[QMI_LOOPBACK_SVC_TBL_SIZE];

#define QMI_LOOPBACK_CLNT_TBL_SIZE (16)
#define CADDR2IDX(addr) (((addr) >> 4) & (QMI_LOOPBACK_CLNT_TBL_SIZE - 1))
static LIST(struct qmi_cci_lb_xport_handle, qmi_loopback_client_tbl)[QMI_LOOPBACK_CLNT_TBL_SIZE];

static uint32_t qmi_csi_loopback_xport_addr_len(void);
static qmi_cci_lock_type qmi_loopback_lock;
static boolean qmi_loopback_init;

typedef enum
{
  QMI_LOOPBACK_NEW_SERVER,
  QMI_LOOPBACK_REMOVE_SERVER
} qmi_loopback_server_event;

void qmi_loopback_xport_init(void)
{
  if(!qmi_loopback_init)
  {
    qmi_loopback_init = TRUE;
    LOCK_INIT(&qmi_loopback_lock);
  }
}

/*****************************************************************************/
/* QCSI LOOPBACK TRANSPORT FUNCTION                                          */
/*****************************************************************************/
/* *qmi_loopback_lock needs to be held by the caller */
static void broadcast_server_event
(
  qmi_loopback_server_event event,
  uint32_t service_id,
  uint32_t version,
  void *addr
)
{
  uint32_t i;
  struct qmi_cci_lb_xport_handle *c;
  for(i = 0; i < QMI_LOOPBACK_CLNT_TBL_SIZE; i++)
  {
    c = LIST_HEAD(qmi_loopback_client_tbl[i]);
    while(c)
    {
      if(c->service_id == service_id &&
          c->version == version)
      {
        if(event == QMI_LOOPBACK_NEW_SERVER)
        {
          qmi_cci_xport_event_new_server(c->clnt, addr);
        }
        else
        {
          qmi_cci_xport_event_remove_server(c->clnt, addr);
        }
      }
      c = c->link.next;
    }
  }
}


/*****************************************************************************/
/* QCSI LOOPBACK TRANSPORT                                                   */
/*****************************************************************************/

/* qmi_csi_loopback_rx() */
static int qmi_csi_loopback_rx
(
  struct qmi_cci_lb_xport_handle *xp_client,
  uint8_t                        *input,
  uint32_t                        len,
  struct qmi_csi_lb_xport_handle *xp_server
)
{
  buf_type *buf;

  buf = (buf_type *)MALLOC(sizeof(buf_type) + len);
  if(!buf)
  {
    return QMI_INTERNAL_ERR;
  }
  memset(buf, 0, sizeof(buf_type));
  buf->msg = (void *)(buf + 1);
  memcpy(buf->msg, input, len);
  buf->len        = len;
  buf->clnt_handle = xp_client;

  LOCK(&xp_server->rx_q_lock);
  LIST_ADD(xp_server->rx_q, buf, link);
  UNLOCK(&xp_server->rx_q_lock);

  QMI_CSI_OS_SIGNAL_SET(&xp_server->sig);
  return QMI_NO_ERR;
}

static qmi_csi_error qmi_csi_loopback_xport_send
(
 void *handle,
 void *_addr,
 uint8_t *msg,
 uint32_t len,
 uint32_t flags,
 void **client_data
)
{
  struct qmi_cci_lb_xport_handle *xp_client;
  void *addr;
  qmi_cci_client_type *clnt = NULL;
  qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;

  memcpy(&addr, _addr, sizeof(addr));

  LOCK(&qmi_loopback_lock);
  xp_client = LIST_HEAD(qmi_loopback_client_tbl[CADDR2IDX((uintptr_t)addr)]);
  while(xp_client)
  {
    if(xp_client == addr)
    {
      clnt = xp_client->clnt;
      break;
    }
    xp_client = xp_client->link.next;
  }
  
  if(clnt)
  {
    /* This means RX and ind callbacks will be made with the global
     * lock held. Any one client hogging the callback can bring
     * down the entire system. */
    if(qmi_cci_xport_recv(clnt, (void *)&handle, msg, len) == QMI_NO_ERR)
    {
      rc = QMI_CSI_NO_ERR;
    }
  }

  UNLOCK(&qmi_loopback_lock);

  return rc;
} /* qmi_csi_loopback_xport_send() */

static uint32_t qmi_csi_loopback_xport_addr_len
(
  void
)
{
  return sizeof(void *);
} /* qmi_csi_loopback_xport_addr_len() */

static buf_type *qmi_csi_dequeue_rx(struct qmi_csi_lb_xport_handle *xp)
{
  buf_type *buf;
  LOCK(&xp->rx_q_lock);
  buf = LIST_HEAD(xp->rx_q);
  if(buf)
  {
    LIST_REMOVE(xp->rx_q, buf, link);
  }
  UNLOCK(&xp->rx_q_lock);
  return buf;
} /* qmi_csi_dequeue_rx() */

static void qmi_csi_loopback_xport_handle_event
(
  void              *handle,
  qmi_csi_os_params *os_params
)
{
  struct qmi_csi_lb_xport_handle *xp = (struct qmi_csi_lb_xport_handle *)handle;
  buf_type *buf;

  while ((buf = qmi_csi_dequeue_rx(xp)) != NULL)
  {
    if (buf->disconnect)
    {
      qmi_csi_xport_disconnect(xp->xport, &buf->clnt_handle);
      qmi_cci_xport_closed(buf->clnt_handle->clnt);
      FREE(buf->clnt_handle);
    }
    else
    {
      /* clnt_handle is also the client address size = sizeof(void *) */
      qmi_csi_xport_recv(xp->xport, (void *)&buf->clnt_handle, buf->msg, buf->len);
    }
    FREE(buf);
  }
} /* qmi_csi_loopback_xport_handle_event */

static void *qmi_csi_loopback_xport_open
(
 void *xport_data,
 qmi_csi_xport_type *xport,
 uint32_t max_rx_len,
 qmi_csi_os_params *os_params,
 qmi_csi_xport_options_type *options
)
{
  struct qmi_csi_lb_xport_handle *xp = CALLOC(1, sizeof(struct qmi_csi_lb_xport_handle));

  if (xp)
  {
    LOCK_INIT(&xp->rx_q_lock);

    xp->xport = xport;
    xp->sig   = *os_params;
  }

  return xp;
} /* qmi_csi_loopback_xport_open() */

static void qmi_csi_loopback_xport_close
(
  void *handle
)
{
  struct qmi_csi_lb_xport_handle *xp = (struct qmi_csi_lb_xport_handle *)handle;
  qmi_server_struct *svc;
  buf_type *buf;
  uint32_t i;

  if(!xp)
    return;

  /* Make sure no one is holding on to a link to this service,
   * if so, auto-unregister then */
  LOCK(&qmi_loopback_lock);
  for(i = 0; i < QMI_LOOPBACK_SVC_TBL_SIZE; i++)
  {
    svc = LIST_HEAD(qmi_loopback_server_tbl[i]);
    while(svc)
    {
      if(svc->handle == xp)
      {
        qmi_server_struct *to_free = svc;
        svc = svc->link.next;
        LIST_REMOVE(qmi_loopback_server_tbl[i], to_free, link);
        broadcast_server_event(QMI_LOOPBACK_REMOVE_SERVER, to_free->service_id, 
                                GET_VERSION(to_free->version), &xp);
        FREE(to_free);
        continue;
      }
      svc = svc->link.next;
    }
  }

  /* Flush and clean the RX queue of the service */
  while(NULL != (buf = qmi_csi_dequeue_rx(xp)))
  {
    if(buf->disconnect)
    {
      qmi_cci_xport_closed(buf->clnt_handle->clnt);
      FREE(buf->clnt_handle);
    }
    FREE(buf);
  }

  UNLOCK(&qmi_loopback_lock);
  qmi_csi_xport_closed(xp->xport);
  LOCK_DEINIT(&xp->rx_q_lock);
  FREE(xp);
} /* qmi_csi_loopback_xport_close() */

static qmi_csi_error qmi_csi_loopback_xport_reg
(
  void     *handle,
  uint32_t  service_id,
  uint32_t  version
)
{
  qmi_server_struct *svc = CALLOC(1, sizeof(*svc));

  if(!svc)
    return QMI_CSI_NO_MEM;

  svc->handle = handle;
  svc->service_id = service_id;
  svc->version = version;

  LOCK(&qmi_loopback_lock);

  LIST_ADD(qmi_loopback_server_tbl[SVC2IDX(service_id)], svc, link);

  broadcast_server_event(QMI_LOOPBACK_NEW_SERVER, service_id, GET_VERSION(version), &handle);

  UNLOCK(&qmi_loopback_lock);

  return QMI_CSI_NO_ERR;
} /* qmi_csi_loopback_xport_reg() */

static qmi_csi_error qmi_csi_loopback_xport_unreg
(
  void     *handle,
  uint32_t  service_id,
  uint32_t  version
)
{
  qmi_server_struct *svc = NULL;

  LOCK(&qmi_loopback_lock);
  svc = LIST_HEAD(qmi_loopback_server_tbl[SVC2IDX(service_id)]);
  while(svc)
  {
    if(svc->service_id == service_id &&
        svc->version == version &&
        svc->handle == handle)
    {
      LIST_REMOVE(qmi_loopback_server_tbl[SVC2IDX(service_id)], svc, link);
      broadcast_server_event(QMI_LOOPBACK_REMOVE_SERVER, service_id, 
                                GET_VERSION(version), &svc->handle);
      FREE(svc);
      break;
    }
    svc = svc->link.next;
  }
  UNLOCK(&qmi_loopback_lock);

  if(!svc)
    return QMI_CSI_INVALID_ARGS;

  return QMI_CSI_NO_ERR;
} /* qmi_csi_loopback_xport_unreg() */

qmi_csi_xport_ops_type qcsi_loopback_ops = {
  NULL,
  qmi_csi_loopback_xport_reg,
  qmi_csi_loopback_xport_unreg,
  NULL,
  qmi_csi_loopback_xport_handle_event,
  qmi_csi_loopback_xport_close,
  qmi_csi_loopback_xport_addr_len,
  qmi_csi_loopback_xport_open,
  qmi_csi_loopback_xport_send,
  NULL
};

/*****************************************************************************/
/* QCSI LOOPBACK TRANSPORT END                                               */
/*****************************************************************************/

/*****************************************************************************/
/* QCCI LOOPBACK TRANSPORT BEGIN                                             */
/*****************************************************************************/
static void *qmi_cci_loopback_xport_open
(
void *xport_data,
qmi_cci_client_type *clnt,
uint32_t service_id,
uint32_t version,
void *addr,
uint32_t max_rx_len
)
{
  struct qmi_cci_lb_xport_handle *xp = CALLOC(1, sizeof(struct qmi_cci_lb_xport_handle));

  if (xp)
  {
    xp->clnt         = clnt;
    xp->service_id   = service_id;
    xp->version      = version;
    xp->svc_addr     = 0;

    LOCK(&qmi_loopback_lock);
    LIST_ADD(qmi_loopback_client_tbl[CADDR2IDX((uintptr_t)xp)], xp, link);
    UNLOCK(&qmi_loopback_lock);
  }

  return xp;
}

static qmi_client_error_type qmi_cci_loopback_xport_send
(
void     *handle,
void     *addr,
uint8_t  *buf,
uint32_t  len
)
{
  struct qmi_cci_lb_xport_handle *xp = (struct qmi_cci_lb_xport_handle *)handle;
  qmi_server_struct *i;
  qmi_client_error_type rc = QMI_NO_ERR;

  /* Get service address */
  memcpy(&xp->svc_addr, addr, sizeof(void *));

  LOCK(&qmi_loopback_lock);

  i = LIST_HEAD(qmi_loopback_server_tbl[SVC2IDX(xp->service_id)]);
  while(i)
  {
    if(i->service_id == xp->service_id && (uintptr_t)i->handle == xp->svc_addr)
    {
      break;
    }
    i = i->link.next;
  }

  if(!i)
  {
    UNLOCK(&qmi_loopback_lock);
    return QMI_SERVICE_ERR;
  }

  rc = qmi_csi_loopback_rx(xp, buf, len, i->handle);

  UNLOCK(&qmi_loopback_lock);

  return rc;
}

static void qmi_cci_loopback_xport_close
(
void *handle
)
{
  buf_type *buf;
  struct qmi_cci_lb_xport_handle *xp;
  qmi_server_struct *xp_server;
  uint32_t idx = CADDR2IDX((uintptr_t)handle);

  LOCK(&qmi_loopback_lock);
  xp = LIST_HEAD(qmi_loopback_client_tbl[idx]);
  while(xp)
  {
    if(xp == handle)
    {
      break;
    }
    xp = xp->link.next;
  }

  if(!xp)
  {
    /* We should never be here, unless QCCI screwed up and 
     * tried to close twice */
    UNLOCK(&qmi_loopback_lock);
    return;
  }

  LIST_REMOVE(qmi_loopback_client_tbl[idx], xp, link);

  /* If not a connected client, QCSI need not be 
   * notified */
  if(xp->svc_addr == 0)
  {
    goto err_bail;
  }

  xp_server = LIST_HEAD(qmi_loopback_server_tbl[SVC2IDX(xp->service_id)]);
  while(xp_server)
  {
    if(xp_server->service_id == xp->service_id && 
        (uintptr_t)xp_server->handle == xp->svc_addr)
    {
      break;
    }
    xp_server = xp_server->link.next;
  }

  /* Service has been removed */
  if(!xp_server)
  {
    goto err_bail;
  }

  /* queue an event to Rx queue */
  buf = CALLOC(1, sizeof(buf_type));
  if (!buf)
  {
    qmi_cci_client_type *clnt = xp->clnt;
    xp->clnt = NULL;
    UNLOCK(&qmi_loopback_lock);
    qmi_cci_xport_closed(clnt);
    /* we cannot free xp without telling the service. 
     * We cannot tell the service if CALLOC fails. Thus
     * at this point, we choose to bare with a memory leak
     * rather than access to an invalid pointer.  */
    return;
  }

  /* buf->clnt_handle is used to carry client handle, 
   * which will be freed in server context.
   */
  buf->clnt_handle = xp;
  buf->disconnect = TRUE;

  /* XXX Unlock global lock and then lock rx_q? */
  LOCK(&xp_server->handle->rx_q_lock);
  LIST_ADD(xp_server->handle->rx_q, buf, link);
  UNLOCK(&xp_server->handle->rx_q_lock);
  QMI_CSI_OS_SIGNAL_SET(&xp_server->handle->sig);

  UNLOCK(&qmi_loopback_lock);

  return;

err_bail:
  UNLOCK(&qmi_loopback_lock);
  qmi_cci_xport_closed(xp->clnt);
  FREE(xp);
}

static uint32_t qmi_cci_loopback_xport_lookup
(
void                 *xport_data,
uint8_t               xport_num,
uint32_t              service_id,
uint32_t              version,
uint32_t             *num_entries,
qmi_cci_service_info *service_info
)
{
  uint32_t idx = SVC2IDX(service_id);
  qmi_server_struct *svc;
  uint32_t num_servers = 0;
  uint32_t info_size = 0;

  if(num_entries)
  {
    info_size = *num_entries;
    *num_entries = 0;
  }

  LOCK(&qmi_loopback_lock);
  svc = LIST_HEAD(qmi_loopback_server_tbl[idx]);
  while(svc)
  {
    if(svc->service_id == service_id &&
        version == GET_VERSION(svc->version))
    {
      num_servers++;
      if(num_entries && *num_entries < info_size)
      {
        uint32_t i = *num_entries;
        *num_entries = i + 1;
        service_info[i].xport = xport_num;
        service_info[i].version = GET_VERSION(svc->version);
        service_info[i].instance = GET_INSTANCE(svc->version);
        service_info[i].reserved = 0;
        /* handle, is treated as the service address */
        memcpy(&service_info[i].addr, &svc->handle, sizeof(void *));
      }
    }
    svc = svc->link.next;
  }
  UNLOCK(&qmi_loopback_lock);
  return num_servers;
}

static uint32_t qmi_cci_loopback_xport_addr_len
(
void
)
{
  return sizeof(void *);
}

qmi_cci_xport_ops_type qcci_loopback_ops = 
{
  qmi_cci_loopback_xport_open,
  qmi_cci_loopback_xport_send,
  qmi_cci_loopback_xport_close,
  qmi_cci_loopback_xport_lookup,
  qmi_cci_loopback_xport_addr_len
};

/*****************************************************************************/
/* QCCI LOOPBACK TRANSPORT END                                               */
/*****************************************************************************/

