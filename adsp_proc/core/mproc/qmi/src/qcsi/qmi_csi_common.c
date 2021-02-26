/******************************************************************************
  @file    qmi_csi_common.c
  @brief   The QMI Common Service Interface (CSI)

  DESCRIPTION
  QMI common server routines.  All server will be build on top of these
  routines for initializing, sending responses and indications.

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
  *******************************************************************************/
#include <string.h>
#include "qmi_idl_lib.h"
#include "qmi_csi.h"
#include "qmi_csi_target.h"
#include "qmi_csi_common.h"
#include "common_v01.h"

#ifndef QMI_POWER_SAVE_FILTERING
#define qmi_csi_filter_can_send_ind(clnt,msg_id) (1)
#define qmi_csi_filter_cleanup_client(clnt)
#endif

#define MAX_XPORTS 10
#define QCSI_MIN(a, b) ((a) > (b) ? (b) : (a))
#define QMI_CSI_FW_MSG_HANDLER_MAX_MSG_ID 0x1F

#define DEFAULT_MAX_TX_BUFS (30)

#define GET_PTR_FROM_LIST(list, i, e_size) ((uint8_t*)list + (i*e_size))
#define GET_MSGID_FROM_LIST(list, i, e_size) *((uint16_t*)GET_PTR_FROM_LIST(list, i, e_size))

/* Backward compatibility macros */
#ifndef QMI_CSI_OS_LOG_TX_EXT
#define QMI_CSI_OS_LOG_TX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do {  \
    uint8_t tmp_header[QMI_HEADER_SIZE];  \
    encode_header(tmp_header, cntl_flag, txn_id, msg_id, msg_len);  \
    QMI_CSI_OS_LOG_TX(tmp_header);  \
  } while(0)
#endif

#ifndef QMI_CSI_OS_LOG_RX_EXT
#define QMI_CSI_OS_LOG_RX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do {  \
    uint8_t tmp_header[QMI_HEADER_SIZE];  \
    encode_header(tmp_header, cntl_flag, txn_id, msg_id, msg_len);  \
    QMI_CSI_OS_LOG_RX(tmp_header);  \
  } while(0)
#endif

/* Service handle even though handed out as a pointer is internally
 * used as an integer. Considering the value was never a pointer, this
 * cast has not consequence with 32/64 bit usage */
#define SVC_IDX2HDL(idx) (qmi_csi_service_handle)(uintptr_t)(idx)
#define SVC_HDL2IDX(hdl) (unsigned int)(uintptr_t)(hdl)
#define INVALID_SVC_HANDLE NULL

/* Client handle even though handed out as pointers is internally
 * used as an integer. Considering the value was never a pointer, this
 * cast has not consequence with 32/64 bit usage */
#define CLNT_IDX2HDL(idx) (qmi_client_handle)(uintptr_t)(idx)
#define CLNT_HDL2IDX(hdl) (unsigned int)(uintptr_t)(hdl)
#define INVALID_CLNT_HANDLE NULL

/* Request handle even though handed out as pointers is internally
 * used as an integer. Considering the value was never a pointer, this
 * cast has not consequence with 32/64 bit usage */
#define REQ_IDX2HDL(idx) (qmi_req_handle)(uintptr_t)(idx)
#define REQ_HDL2IDX(hdl) (unsigned int)(uintptr_t)(hdl)
#define INVALID_REQ_HANDLE NULL


/* Global lists of services, clients and outstanding txns */
static qmi_csi_lock_type service_list_lock;
static LIST(qmi_csi_service_type, service_list);
static qmi_csi_lock_type client_list_lock;
static LIST(qmi_csi_client_type, client_list);
static qmi_csi_lock_type txn_list_lock;
static LIST(qmi_csi_txn_type, txn_list);
static LIST(qmi_csi_msg_handler_type, msg_handler_list);

/* globally unique service, client and txn descriptors - monotonically
 * increasing numbers. Use list locks to protect these
 */
static unsigned int global_service_desc;
static unsigned int global_client_desc;
static unsigned int global_txn_desc;

static uint32_t num_xports;
struct qmi_csi_xport_tbl_s
{
  qmi_csi_xport_ops_type *ops;
  void *xport_data;
};

static struct qmi_csi_xport_tbl_s xport_tbl[MAX_XPORTS];
static uint32_t inited;

/* Translate QCSI CB errors to common error types */
static qmi_error_type_v01 qmi_csi_cb_err_translate(qmi_csi_cb_error err)
{
  static uint16_t qmi_csi_cb_err_array[] = { QMI_ERR_NONE_V01, /* QMI_CSI_CB_NO_ERR */
    QMI_ERR_INVALID_CLIENT_ID_V01, /* QMI_CSI_CB_CONN_REFUSED */
    QMI_ERR_NO_MEMORY_V01, /* QMI_CSI_CB_NO_MEM */
    QMI_ERR_INTERNAL_V01, /* QMI_CSI_CB_INTERNAL_ERR */
    QMI_ERR_NOT_SUPPORTED_V01, /* QMI_CSI_CB_UNSUPPORTED_ERR */
    QMI_ERR_INTERNAL_V01 /* QMI_CSI_CB_REQ_HANDLED */
  };
  static int qmi_csi_cb_err_array_size = sizeof(qmi_csi_cb_err_array) / sizeof(uint16_t);
  if ((int)err > qmi_csi_cb_err_array_size)
  {
    return QMI_ERR_INTERNAL_V01;
  }
  return (qmi_error_type_v01)qmi_csi_cb_err_array[err];
}

/* Translate QCSI errors to common error types */
static uint16_t qmi_csi_err_translate(uint32_t csi_err)
{
  static uint16_t qmi_csi_err_array[] = { QMI_ERR_NONE_V01, /* QMI_CSI_NO_ERR */
    QMI_ERR_CLIENT_IDS_EXHAUSTED_V01, /* QMI_CSI_CONN_REFUSED */
    QMI_ERR_INTERNAL_V01, /* QMI_CSI_CONN_BUSY */
    QMI_ERR_INTERNAL_V01, /* QMI_CSI_INVALID_HANDLE */
    QMI_ERR_INTERNAL_V01, /* QMI_CSI_INVALID_ARGS */
    QMI_ERR_ENCODING_V01, /* QMI_CSI_ENCODE_ERR */
    QMI_ERR_ENCODING_V01, /* QMI_CSI_DECODE_ERR */
    QMI_ERR_NO_MEMORY_V01, /* QMI_CSI_NO_MEM */
    QMI_ERR_INTERNAL_V01 /* QMI_CSI_INTERNAL_ERR */
  };
  static uint32_t qmi_csi_err_array_size = sizeof(qmi_csi_err_array) / sizeof(uint16_t);
  if (csi_err > qmi_csi_err_array_size)
  {
    return QMI_ERR_INTERNAL_V01;
  }
  return qmi_csi_err_array[csi_err];
}

/* Translate QCSI errors to common error types */
static uint16_t qmi_csi_idl_err_translate(int32_t idl_err)
{
  uint16_t rc = QMI_ERR_ENCODING_V01;
  switch(idl_err)
  {
  case QMI_IDL_LIB_MESSAGE_ID_NOT_FOUND:
    rc = QMI_ERR_INVALID_MESSAGE_ID_V01;
    break;
  case QMI_IDL_LIB_MISSING_TLV:
    rc = QMI_ERR_MISSING_ARG_V01;
    break;
  case QMI_IDL_LIB_ARRAY_TOO_BIG:
    rc = QMI_ERR_ARG_TOO_LONG_V01;
    break;
  case QMI_IDL_LIB_LENGTH_INCONSISTENCY:
  case QMI_IDL_LIB_TLV_DUPLICATED:
    rc = QMI_ERR_MALFORMED_MSG_V01;
  default:
    break;
  }
  return rc;
}

static void log_message
(
  qmi_csi_service_type *service,
  qmi_csi_client_type  *client,
  qmi_idl_type_of_message_type msg_type,
  unsigned int msg_id,
  void *msg_buf,
  unsigned int msg_len,
  unsigned int txn_id
)
{
  if(service->log_message_ext_cb)
  {
    qmi_csi_log_info info = {0};
    info.service_obj = service->service_obj;
    info.service_cookie = service->service_cookie;
    info.client_handle = CLNT_IDX2HDL(client->handle);
    info.connection_handle = client->connection_handle;
    info.msg_type  = msg_type;
    info.msg_id = msg_id;
    info.msg_buf = msg_buf;
    info.msg_len = msg_len;
    info.txn_id = txn_id;

    service->log_message_ext_cb(&info);
  }
  else if(service->log_message_cb)
  {
    service->log_message_cb(service->service_obj, msg_type, msg_id, msg_buf, 
                             msg_len, txn_id);
  }
}

/* add service to the global list */
static qmi_csi_service_handle add_service(qmi_csi_service_type *svc)
{
  unsigned int service_handle;

  LOCK(&service_list_lock);
  service_handle = svc->handle = ++global_service_desc;
  LIST_ADD(service_list, svc, link);
  UNLOCK(&service_list_lock);

  return SVC_IDX2HDL(service_handle);
}

/* remove a service from the global list */
static void remove_service(qmi_csi_service_type *svc)
{
  LOCK(&service_list_lock);
  LIST_REMOVE(service_list, svc, link);
  UNLOCK(&service_list_lock);
}

/* service list lock needs to be held */
static qmi_csi_service_type *find_service(qmi_csi_service_handle _handle)
{
  unsigned int handle = SVC_HDL2IDX(_handle);
  qmi_csi_service_type *svc = LIST_HEAD(service_list);

  while (svc)
  {
    if (svc->handle == handle)
      return svc;
    svc = svc->link.next;
  }
  return NULL;
}

/* neither svc nor clnt can be NULL, client_list_lock held */
static void add_client(qmi_csi_service_type *svc, qmi_csi_client_type *clnt)
{
  /* set unique client handle */
  clnt->handle = ++global_client_desc;
  LIST_ADD(svc->client_list, clnt, local);
  LIST_ADD(client_list, clnt, global);
}

/* client list lock needs to be held */
static qmi_csi_client_type *find_client(qmi_client_handle handle)
{
  unsigned int to_find = CLNT_HDL2IDX(handle);
  qmi_csi_client_type *clnt;
  LIST_FIND(client_list, clnt, global, clnt->handle == to_find);
  return clnt;
}

/* txn_list_lock needs to be held */
static qmi_req_handle add_txn(qmi_csi_client_type *clnt, qmi_csi_txn_type *txn)
{
  txn->handle = ++global_txn_desc;
  LIST_ADD(clnt->txn_list, txn, local);
  LIST_ADD(txn_list, txn, global);
  return REQ_IDX2HDL(txn->handle);
}

/* txn_list_lock needs to be held */
static void remove_txn(qmi_csi_client_type *clnt, qmi_csi_txn_type *txn)
{
  LIST_REMOVE(clnt->txn_list, txn, local);
  LIST_REMOVE(txn_list, txn, global);
}

/* txn list lock needs to be held */
static qmi_csi_txn_type *find_txn(qmi_req_handle handle)
{
  unsigned int to_find = REQ_HDL2IDX(handle);
  qmi_csi_txn_type *txn;
  LIST_FIND(txn_list, txn, global, txn->handle == to_find);
  return txn;
}

/* txn_list_lock should be held */
static qmi_csi_txn_type *get_txn
(
  qmi_csi_client_type *clnt,
  uint16_t txn_id,
  uint16_t msg_id,
  qmi_req_handle *req_handle
)
{
  qmi_csi_txn_type *txn = NULL;
  if (LIST_CNT(clnt->txn_free_list) > 0)
  {
    txn = LIST_TAIL(clnt->txn_free_list);
    LIST_REMOVE(clnt->txn_free_list, txn, local);
    txn->pool_allocated = 1;
  }
  else
  {
    txn = CALLOC(1, sizeof(*txn));
    if (!txn)
    {
      return NULL;
    }
    /* txn->pool_alloacted = 0 (calloc) */
  }
  LINK_INIT(txn->local);
  LINK_INIT(txn->global);
  txn->client = clnt;
  txn->txn_id = txn_id;
  txn->msg_id = msg_id;
  *req_handle = add_txn(clnt, txn);
  return txn;
}

/* Txn list lock should be held */
static void release_txn
(
  qmi_csi_txn_type *txn
)
{
  qmi_csi_client_type *clnt = txn->client;
  if (txn->pool_allocated)
  {
    LINK_INIT(txn->local);
    LIST_ADD(clnt->txn_free_list, txn, local);
  }
  else
  {
    FREE(txn);
  }
}

static qmi_csi_client_type *find_client_by_addr
(
  qmi_csi_xport_type *xport,
  void *addr
)
{
  qmi_csi_client_type *clnt;

  if (!xport || !xport->service)
    return NULL;

  clnt = LIST_HEAD(xport->service->client_list);
  while (clnt)
  {
    if (clnt->xport.xport == xport &&
        !memcmp(clnt->xport.addr, addr, xport->addr_len))
        return clnt;
    clnt = clnt->local.next;
  }
  return NULL;
}

/* client list should be locked */
static void clean_txns(qmi_csi_client_type *clnt)
{
  qmi_csi_txn_type *txn;
  LOCK(&txn_list_lock);
  txn = LIST_HEAD(clnt->txn_list);
  while (txn)
  {
    qmi_csi_txn_type *to_free = txn;
    txn = txn->local.next;
    /* remove from global txn list */
    LIST_REMOVE(txn_list, to_free, global);
    release_txn(to_free);
  }
  UNLOCK(&txn_list_lock);
}

/* client list lock needs to be held */
static qmi_csi_client_type *create_client
(
  qmi_csi_xport_type *xport,
  void *addr
)
{
  qmi_csi_service_type *svc = xport->service;
  qmi_csi_client_type  *clnt;
  int i;

  /* client not found, create new connection */
  clnt = CALLOC(1, sizeof(qmi_csi_client_type));
  if (!clnt)
    return NULL;
  LINK_INIT(clnt->local);
  LINK_INIT(clnt->global);
  LIST_INIT(clnt->txn_list);

  for (i = 0; i < TXN_POOL_SIZE; i++)
  {
    struct qmi_csi_txn_s *txn = &clnt->txn_pool[i];
    txn->pool_allocated = 1;
    LIST_ADD(clnt->txn_free_list, txn, local);
  }

  add_client(svc, clnt);

  /* initialize client struct fields */
  clnt->service = svc;
  clnt->xport.xport = xport;
  clnt->next_ind_txn_id = 1;
  QMI_MEM_COPY(clnt->xport.addr, MAX_ADDR_LEN, addr, xport->addr_len);

  return clnt;
}

/* neither svc nor clnt can be NULL, client_list_lock held */
static void free_client(qmi_csi_service_type *svc, qmi_csi_client_type *clnt)
{
  LIST_REMOVE(svc->client_list, clnt, local);
  LIST_REMOVE(client_list, clnt, global);
  qmi_csi_filter_cleanup_client(clnt);
  clean_txns(clnt);
  FREE(clnt);
}

/* encode a send message to client */
static qmi_csi_error internal_send
(
  qmi_csi_service_type *svc,
  qmi_csi_client_type *clnt,
  qmi_idl_type_of_message_type msg_type,
  txn_id_type txn_id,
  uint16_t msg_id,
  void *c_struct,
  unsigned int c_struct_len,
  int encode
)
{
  qmi_csi_xport_type *xport;
  uint32_t max_msg_len = 0, out_len;
  qmi_csi_error rc;
  int32_t encdec_rc;
  unsigned char *msg;
  uint8_t cntl_flag;
  uint32_t send_flags = 0;

  if (encode)
  {
    uint32_t idl_c_struct_len;
    encdec_rc = qmi_idl_get_message_c_struct_len(svc->service_obj, msg_type, msg_id,
                                                 &idl_c_struct_len);
    if (encdec_rc != QMI_IDL_LIB_NO_ERR)
      return QMI_CSI_ENCODE_ERR;

    if (c_struct_len != idl_c_struct_len && c_struct_len != 0)
      return QMI_CSI_ENCODE_ERR;

    encdec_rc = qmi_idl_get_max_message_len(svc->service_obj, msg_type, msg_id,
                                            &max_msg_len);
    if (encdec_rc != QMI_IDL_LIB_NO_ERR)
      return QMI_CSI_ENCODE_ERR;
  }
  else
  {
    max_msg_len = c_struct_len;
  }

  if (c_struct && c_struct_len)
  {
    msg = MALLOC(max_msg_len + QMI_HEADER_SIZE);
    if (!msg)
      return QMI_CSI_NO_MEM;

    if (encode)
    {
      if (qmi_idl_message_encode(
        svc->service_obj,
        msg_type,
        msg_id,
        c_struct,
        c_struct_len,
        msg + QMI_HEADER_SIZE,
        max_msg_len,
        &out_len) != QMI_IDL_LIB_NO_ERR)
      {
        FREE(msg);
        return QMI_CSI_ENCODE_ERR;
      }
    }
    else
    {
      QMI_MEM_COPY(msg + QMI_HEADER_SIZE, max_msg_len, c_struct, c_struct_len);
      out_len = c_struct_len;
    }
  }
  else
  {
    /* Empty message */
    out_len = 0;
    msg = MALLOC(QMI_HEADER_SIZE);
    if (!msg)
      return QMI_CSI_NO_MEM;
  }

  switch (msg_type)
  {
  case QMI_IDL_INDICATION:
    cntl_flag = QMI_INDICATION_CONTROL_FLAG;
    send_flags |= QMI_CSI_SEND_FLAG_RATE_LIMITED;
    break;
  case QMI_IDL_RESPONSE:
    cntl_flag = QMI_RESPONSE_CONTROL_FLAG;
    break;
  default:
    cntl_flag = QMI_REQUEST_CONTROL_FLAG;
    break;
  }

  /* fill in header */
  encode_header(msg, cntl_flag, txn_id, msg_id, (uint16_t)out_len);

  out_len += QMI_HEADER_SIZE;

  if (clnt)
  {

    /* Log the encoded message payload */
    QMI_CSI_OS_LOG_TX_EXT(svc->service_obj, cntl_flag, txn_id, msg_id,
                            out_len, clnt->xport.addr, clnt->xport.xport->addr_len);
    log_message(svc, clnt, msg_type, (unsigned int)msg_id, 
                        msg + QMI_HEADER_SIZE, (unsigned int)out_len - QMI_HEADER_SIZE, (unsigned int)txn_id);

    xport = clnt->xport.xport;
    if (xport->ops->send_legacy)
    {
      rc = xport->ops->send_legacy(xport->handle, clnt->xport.addr, msg, out_len,
                                   &clnt->xport.client_data);
    }
    else
    {
      rc = xport->ops->send(xport->handle, clnt->xport.addr, msg, out_len,
                            send_flags, &clnt->xport.client_data);
    }
  }
  else
  {
    /* broadcast too all clients. ignore errors */
    clnt = LIST_HEAD(svc->client_list);
    while (clnt)
    {
      /* Log the encoded message payload */
      QMI_CSI_OS_LOG_TX_EXT(svc->service_obj, cntl_flag, txn_id, msg_id,
                              out_len, clnt->xport.addr, clnt->xport.xport->addr_len);
      log_message(svc, clnt, msg_type, (unsigned int)msg_id, 
                        msg + QMI_HEADER_SIZE, (unsigned int)out_len - QMI_HEADER_SIZE, (unsigned int)txn_id);

      xport = clnt->xport.xport;
      if (xport->ops->send_legacy)
      {
        xport->ops->send_legacy(xport->handle, clnt->xport.addr, msg, out_len,
                                &clnt->xport.client_data);
      }
      else
      {
        xport->ops->send(xport->handle, clnt->xport.addr, msg, out_len,
                         send_flags, &clnt->xport.client_data);
      }
      clnt = clnt->local.next;
    }
    rc = QMI_CSI_NO_ERR;
  }

  FREE(msg);

  return rc;
}

/* encode and send error response to client */
static qmi_csi_error encode_and_send_resp
(
  qmi_csi_xport_type *xport,
  void *addr,
  uint16_t msg_id,
  uint16_t txn_id,
  uint16_t result,
  uint16_t error
)
{
  uint32_t resp_msg_len;
  qmi_csi_error rc;
  unsigned char *msg;

  resp_msg_len = qmi_idl_get_std_resp_tlv_len();

  msg = MALLOC(resp_msg_len + QMI_HEADER_SIZE);
  if (!msg)
    return QMI_CSI_NO_MEM;

  if (qmi_idl_encode_resp_tlv(
    result,
    error,
    msg + QMI_HEADER_SIZE,
    resp_msg_len
    ) != QMI_IDL_LIB_NO_ERR)
  {
    FREE(msg);
    return QMI_CSI_ENCODE_ERR;
  }

  /* fill in header */
  encode_header(msg, QMI_RESPONSE_CONTROL_FLAG, txn_id, msg_id,
                (uint16_t)resp_msg_len);

  QMI_CSI_OS_LOG_TX_EXT(xport->service->service_obj, QMI_RESPONSE_CONTROL_FLAG,
                          txn_id, msg_id, resp_msg_len, addr, xport->addr_len);

  resp_msg_len += QMI_HEADER_SIZE;

  if (xport->ops->send_legacy)
  {
    rc = xport->ops->send_legacy(xport->handle, addr, msg, resp_msg_len, NULL);
  }
  else
  {
    /* Do not rate limit responses */
    rc = xport->ops->send(xport->handle, addr, msg, resp_msg_len, 0, NULL);
  }

  FREE(msg);

  return rc;
}

/* self-init function. not reentrant */
static void qmi_csi_init(void)
{
  /* initialize locks */
  LOCK_INIT(&service_list_lock);
  LOCK_INIT(&client_list_lock);
  LOCK_INIT(&txn_list_lock);
#ifdef QMI_POWER_SAVE_FILTERING
  qmi_csi_filter_init();
#endif
  inited = 1;
}

/*=============================================================================
                                PUBLIC FUNCTIONS
  =============================================================================*/

/*=============================================================================
                               Up-calls from xport
  =============================================================================*/
qmi_csi_error qmi_csi_add_msg_handler
(
  uint16_t msg_id,
  qmi_csi_msg_handler msg_handler_func
)
{
  qmi_csi_msg_handler_type *msg_handler;

  if (!msg_handler_func || msg_id > QMI_CSI_FW_MSG_HANDLER_MAX_MSG_ID)
  {
    return QMI_CSI_INTERNAL_ERR;
  }

  msg_handler = CALLOC(1, sizeof(qmi_csi_msg_handler_type));
  if (!msg_handler)
  {
    return QMI_CSI_NO_MEM;
  }
  LINK_INIT(msg_handler->link);
  msg_handler->msg_id = msg_id;
  msg_handler->msg_handler = msg_handler_func;
  LIST_ADD(msg_handler_list, msg_handler, link);
  return QMI_CSI_NO_ERR;
}

/*=============================================================================
  FUNCTION  qmi_csi_xport_conn_status
  =============================================================================*/
int qmi_csi_xport_conn_status
(
  qmi_csi_xport_type *xport,
  void *addr
)
{
  qmi_csi_client_type *clnt;

  if (!xport || !addr)
    return 0;
  LOCK(&client_list_lock);
  clnt = find_client_by_addr(xport, addr);
  UNLOCK(&client_list_lock);
  return clnt ? 1 : 0;
}

/*=============================================================================
  FUNCTION  qmi_csi_xport_connect
  =============================================================================*/
qmi_csi_error qmi_csi_xport_connect
(
  qmi_csi_xport_type *xport,
  void *addr
)
{
  qmi_csi_client_type  *clnt;
  qmi_csi_service_type *svc;
  qmi_csi_connect connect_cb = NULL;
  qmi_client_handle client_handle = INVALID_CLNT_HANDLE;
  void *service_cookie = NULL, *connection_handle = NULL;
  qmi_csi_error rc = QMI_CSI_INTERNAL_ERR;
  qmi_csi_cb_error cb_rc;

  if (!xport || !xport->service || !addr)
    return QMI_CSI_INTERNAL_ERR;

  LOCK(&client_list_lock);
  /* figure out if client address exists in client list */
  clnt = find_client_by_addr(xport, addr);

  if (clnt)
  {
    /* client exits, do nothing */
    UNLOCK(&client_list_lock);
    return QMI_CSI_NO_ERR;
  }

  clnt = create_client(xport, addr);

  if (!clnt)
  {
    UNLOCK(&client_list_lock);
    return QMI_CSI_NO_MEM;
  }

  /* cache params to the callback */
  svc = clnt->service;
  connect_cb = svc->service_connect;
  service_cookie = svc->service_cookie;
  client_handle = CLNT_IDX2HDL(clnt->handle);

  UNLOCK(&client_list_lock);

  /* invoke service_connect without lock held */
  if (connect_cb)
  {
    cb_rc = connect_cb(client_handle, service_cookie, &connection_handle);

    /* re-lock client list and re-lookup client with handle */
    LOCK(&client_list_lock);
    clnt = find_client(client_handle);
    if (cb_rc != QMI_CSI_CB_NO_ERR)
    {
      if (clnt)
      {
        free_client(clnt->service, clnt);
      }
      if (cb_rc == QMI_CSI_CB_NO_MEM)
      {
        rc = QMI_CSI_NO_MEM;
      }
      else
      {
        rc = QMI_CSI_CONN_REFUSED;
      }
    }
    else
    {
      /* check to see if clnt still exits */
      if (clnt)
      {
        clnt->connection_handle = connection_handle;
        rc = QMI_CSI_NO_ERR;
      }
    }
    UNLOCK(&client_list_lock);
  }

  return rc;
}

static qmi_error_type_v01 handle_process_req_tbl
(
  qmi_csi_req_handler_tbl_type *handler,
  uint16_t msg_id,
  uint32_t c_struct_len,
  unsigned char *c_struct,
  void *connection_handle,
  void *service_cookie,
  qmi_req_handle txn_handle
)
{
  int rc = QMI_ERR_INTERNAL_V01;

  rc = handler->service_process_req(
    connection_handle,
    txn_handle,
    msg_id,
    c_struct_len ? c_struct : NULL,
    c_struct_len,
    service_cookie);
  return qmi_csi_cb_err_translate((qmi_csi_cb_error)rc);
}

/*=============================================================================
  FUNCTION qmi_csi_msg_list_find
=============================================================================*/
void * qmi_csi_msg_list_find
(
  void *list,
  size_t count,
  size_t e_size,
  uint16_t msg_id
)
{
  int imin, imax, imid;

  if(list == NULL || count == 0 || e_size == 0)
  {
    return NULL;
  }

  imin = 0;
  imax = (int)count - 1; //index of last element
  while(imax >= imin)
  {
    imid = (imax + imin) >> 1;
    if(GET_MSGID_FROM_LIST(list, imid, e_size) < msg_id)
    {
      imin = imid + 1;
    }
    else if(GET_MSGID_FROM_LIST(list, imid, e_size) > msg_id)
    {
      imax = imid - 1;
    }
    else
    {
      return GET_PTR_FROM_LIST(list,imid, e_size);
    }
  }
  return NULL;
}

static qmi_error_type_v01 process_message
(
  qmi_csi_service_type *svc,
  unsigned int msg_id,
  uint8_t *buf,
  unsigned int msg_len,
  void *connection_handle,
  qmi_req_handle txn_handle,
  qmi_csi_client_type *clnt,
  void *service_cookie
)
{
  uint32_t c_struct_len = 0;
  void *c_struct = NULL;
  int32_t rc = QMI_ERR_NONE_V01;
  qmi_csi_req_handler_tbl_type *req_handler = NULL;
  qmi_csi_msg_handler_type *msg_handler = NULL;
  /* Handle pre-request */
  if (svc->service_process_pre_req)
  {
    rc = svc->service_process_pre_req(connection_handle,
                                      txn_handle,
                                      msg_id,
                                      msg_len ? buf + QMI_HEADER_SIZE : NULL,
                                      msg_len,
                                      service_cookie);

    if (rc == QMI_CSI_CB_REQ_HANDLED)
    {
      return QMI_ERR_NONE_V01;
    }
    if (rc != QMI_CSI_CB_NO_ERR)
    {
      return QMI_ERR_NOT_SUPPORTED_V01;
    }
  }

  req_handler = qmi_csi_msg_list_find(svc->req_handler_tbl,
                                      svc->req_handler_size,
                                      sizeof(qmi_csi_req_handler_tbl_type),
                                      msg_id);

  if (msg_id <= QMI_CSI_FW_MSG_HANDLER_MAX_MSG_ID && !req_handler)
  {
    msg_handler = LIST_HEAD(msg_handler_list);
    while (msg_handler)
    {
      if (msg_handler->msg_id == msg_id)
      {
        break;
      }
      msg_handler = msg_handler->link.next;
    }
  }
  if ((req_handler && req_handler->decoded) || !(svc->req_handler_tbl) || (msg_handler && !req_handler))
  {
    /* decode message */
    rc = qmi_idl_get_message_c_struct_len(svc->service_obj, QMI_IDL_REQUEST,
                                          (uint16_t)msg_id, &c_struct_len);
    if (rc != QMI_IDL_LIB_NO_ERR)
    {
      return (qmi_error_type_v01)qmi_csi_idl_err_translate(rc);
    }
    if (c_struct_len)
    {
      c_struct = MALLOC(c_struct_len);

      if (!c_struct)
      {
        return QMI_ERR_NO_MEMORY_V01;
      }

      rc = qmi_idl_message_decode(
        svc->service_obj,
        QMI_IDL_REQUEST,
        (uint16_t)msg_id,
        buf + QMI_HEADER_SIZE,
        msg_len,
        c_struct,
        (uint16_t)c_struct_len);
      if (rc != QMI_IDL_LIB_NO_ERR)
      {
        if (rc < QMI_IDL_LIB_NO_ERR)
        {
          rc = qmi_csi_idl_err_translate(rc);
        }
        FREE(c_struct);
        return (qmi_error_type_v01)rc;
      }
    }
  }
  /* Handle Reserved Messages */
  if (msg_handler && !req_handler)
  {
    if (msg_handler->msg_handler(svc->service_obj, GET_INSTANCE(svc->idl_version), (uint16_t)msg_id,
      txn_handle, (void*)c_struct, c_struct_len, clnt) == QMI_CSI_NO_ERR)
    {
      if (c_struct)
      {
        FREE(c_struct);
      }
      return QMI_ERR_NONE_V01;
    }
  }
  if (svc->req_handler_tbl)
  {
    if (req_handler)
    {
      if (!req_handler->decoded)
      {
        c_struct = (unsigned char *)(buf + QMI_HEADER_SIZE);
        c_struct_len = msg_len;
      }
      rc = handle_process_req_tbl(req_handler, (uint16_t)msg_id, c_struct_len, c_struct,
                                  connection_handle, service_cookie, txn_handle);
      if (req_handler->decoded)
      {
        if (c_struct)
        {
          FREE(c_struct);
        }
      }
      return (qmi_error_type_v01)rc;
    }
    else
    {
      return QMI_ERR_NOT_SUPPORTED_V01;
    }
  }

  /* Finally handle the request */
  rc = svc->service_process_req(
    connection_handle,
    txn_handle,
    msg_id,
    c_struct,
    c_struct_len,
    service_cookie
    );

  if (c_struct)
  {
    FREE(c_struct);
  }
  return qmi_csi_cb_err_translate((qmi_csi_cb_error)rc);
}

/*=============================================================================
FUNCTION  qmi_csi_xport_send_err_resp
=============================================================================*/
qmi_csi_error qmi_csi_xport_send_err_resp
(
qmi_csi_xport_type *xport,
void *addr,
uint8_t *buf,
uint32_t len,
uint32_t err
)
{
  uint8_t cntl_flag;
  uint16_t txn_id, msg_id, msg_len;

  if(!xport || !addr || len < QMI_HEADER_SIZE)
  {
    return QMI_CSI_INTERNAL_ERR;
  }

  decode_header(buf, &cntl_flag, &txn_id, &msg_id, &msg_len);
  encode_and_send_resp(xport, addr, msg_id, txn_id,
                       QMI_RESULT_FAILURE_V01, (uint16_t)qmi_csi_err_translate(err));
  return QMI_CSI_NO_ERR;
}

/*=============================================================================
  FUNCTION  qmi_csi_xport_recv
  =============================================================================*/
qmi_csi_error qmi_csi_xport_recv
(
  qmi_csi_xport_type *xport,
  void *addr,
  uint8_t *buf,
  uint32_t len
)
{
  qmi_csi_service_type *svc;
  qmi_csi_client_type  *clnt;
  qmi_csi_txn_type     *txn;
  qmi_req_handle txn_handle = INVALID_REQ_HANDLE;
  uint8_t cntl_flag;
  uint16_t txn_id, msg_id, msg_len;
  int rc = QMI_ERR_INTERNAL_V01;
  void *connection_handle, *service_cookie;

  if (!xport || !xport->service || !addr || len < QMI_HEADER_SIZE)
    return QMI_CSI_INTERNAL_ERR;

  svc = xport->service;

  decode_header(buf, &cntl_flag, &txn_id, &msg_id, &msg_len);

  QMI_CSI_OS_LOG_RX_EXT(svc->service_obj, cntl_flag, txn_id, msg_id, msg_len,
                              addr, xport->addr_len);

  /* got a client struct, handle only request */
  if (cntl_flag != QMI_REQUEST_CONTROL_FLAG)
  {
    rc = QMI_ERR_MALFORMED_MSG_V01;
    goto rx_cb_bail;
  }

  /* if received message is shorter than the size in the header then there
   * might have been a memory allocation error
   */
  if ((len - QMI_HEADER_SIZE) != msg_len)
  {
    rc = QMI_ERR_NO_MEMORY_V01;
    goto rx_cb_bail;
  }

  LOCK(&client_list_lock);

  /* figure out if client address exists in client list */
  clnt = find_client_by_addr(xport, addr);
  if (!clnt)
  {
    UNLOCK(&client_list_lock);
    /* Auto connect on first packet */
    rc = qmi_csi_xport_connect(xport, addr);
    if (rc != QMI_CSI_NO_ERR)
    {
      rc = qmi_csi_err_translate(rc);
      if(rc == QMI_ERR_INVALID_CLIENT_ID_V01)
      {
        rc = QMI_ERR_CLIENT_IDS_EXHAUSTED_V01;
      }
      goto rx_cb_bail;
    }
    if(svc->async_conn_cb)
    {
      return QMI_CSI_DEFERRED;
    }

    /* Lock and find the client, if not found,
     * do not proceed. */
    LOCK(&client_list_lock);
    clnt = find_client_by_addr(xport, addr);
    if (!clnt)
    {
      rc = QMI_ERR_INTERNAL_V01;
      UNLOCK(&client_list_lock);
      goto rx_cb_bail;
    }
  }

  LOCK(&txn_list_lock);
  txn = get_txn(clnt, txn_id, msg_id, &txn_handle);
  UNLOCK(&txn_list_lock);
  if (!txn)
  {
    rc = QMI_ERR_NO_MEMORY_V01;
    UNLOCK(&client_list_lock);
    goto rx_cb_bail;
  }

  /* cache handle values before unlocking and invoking callback */
  connection_handle = clnt->connection_handle;
  service_cookie = svc->service_cookie;
  UNLOCK(&client_list_lock);

  /* Log the encoded message payload */
  log_message(svc, clnt, QMI_IDL_REQUEST, (unsigned int)msg_id, buf + QMI_HEADER_SIZE,
                        (unsigned int)msg_len, (unsigned int)txn_id);

  rc = process_message(svc, msg_id, buf, msg_len, connection_handle, txn_handle, clnt, service_cookie);

  if (rc == QMI_ERR_NONE_V01)
  {
    return QMI_CSI_NO_ERR;
  }

  LOCK(&txn_list_lock);
  txn = find_txn(txn_handle);
  if (txn)
  {
    remove_txn(txn->client, txn);
    release_txn(txn);
  }
  UNLOCK(&txn_list_lock);
rx_cb_bail:
  encode_and_send_resp(xport, addr, msg_id, txn_id,
                       QMI_RESULT_FAILURE_V01, (uint16_t)rc);
  return QMI_CSI_NO_ERR;
}

/*=============================================================================
  FUNCTION  qmi_csi_xport_resume_client
  =============================================================================*/
void qmi_csi_xport_resume_client
(
  qmi_csi_xport_type *xport,
  void *addr
)
{
  qmi_csi_service_type *svc;
  qmi_csi_client_type  *clnt;
  qmi_csi_resume_ind resume_cb = NULL;
  qmi_client_handle client_handle = 0;
  void *service_cookie = NULL, *connection_handle = NULL;

  if (!xport || !xport->service || !addr)
    return;

  svc = xport->service;

  LOCK(&client_list_lock);
  /* try to look for client, if found, call disconnect handler */
  clnt = find_client_by_addr(xport, addr);
  if (clnt)
  {
    resume_cb = svc->resume_ind_cb;
    client_handle = CLNT_IDX2HDL(clnt->handle);
    connection_handle = clnt->connection_handle;
    service_cookie = svc->service_cookie;
  }
  UNLOCK(&client_list_lock);

  if (resume_cb)
  {
    resume_cb(client_handle, connection_handle, service_cookie);
  }
}


/*=============================================================================
  FUNCTION  qmi_csi_xport_disconnect
  =============================================================================*/
qmi_csi_error qmi_csi_xport_disconnect
(
  qmi_csi_xport_type *xport,
  void *addr
)
{
  qmi_csi_service_type *svc;
  qmi_csi_client_type  *clnt;
  qmi_csi_disconnect disconnect_cb = NULL;
  void *conn_handle = NULL, *service_cookie = NULL;

  if (!xport || !xport->service || !addr)
    return QMI_CSI_INTERNAL_ERR;

  svc = xport->service;

  LOCK(&client_list_lock);
  /* try to look for client, if found, call disconnect handler */
  clnt = find_client_by_addr(xport, addr);
  if (clnt)
  {
    /* remove client from active list */
    disconnect_cb = svc->service_disconnect;
    conn_handle = clnt->connection_handle;
    service_cookie = svc->service_cookie;
    free_client(svc, clnt);
  }
  UNLOCK(&client_list_lock);

  if (disconnect_cb)
  {
    disconnect_cb(conn_handle, service_cookie);
  }

  return QMI_CSI_NO_ERR;
}

/*=============================================================================
  FUNCTION  qmi_csi_xport_closed
  =============================================================================*/
void qmi_csi_xport_closed(qmi_csi_xport_type *xport)
{
  qmi_csi_service_type *svc;
  qmi_csi_client_type  *clnt;
  unsigned int i, active = 0;

  if (!xport || !xport->service)
    return;

  /* go through client list and clean up clients associated with the xport */
  svc = xport->service;
  LOCK(&client_list_lock);
  clnt = LIST_HEAD(svc->client_list);
  while (clnt)
  {
    qmi_csi_client_type *to_free = clnt;
    clnt = clnt->local.next;
    if (to_free->xport.xport == xport)
    {
      free_client(svc, to_free);
    }
  }
  UNLOCK(&client_list_lock);

  /* remove xport from xport list. When no xport left, free service */
  for (i = 0; i < num_xports; i++)
  {
    if (svc->xports[i] == xport)
    {
      svc->xports[i] = NULL;
      svc->num_xports--;
    }
    if (svc->xports[i])
      active++;
  }
  FREE(xport);

  if (!active)
  {
    /* no more xport left */
    remove_service(svc);
    FREE(svc->xports);
    if (svc->req_handler_tbl)
    {
      FREE(svc->req_handler_tbl);
    }
    FREE(svc);
  }
}

/*=============================================================================
  FUNCTION  qmi_csi_xport_start
  =============================================================================*/
void qmi_csi_xport_start(qmi_csi_xport_ops_type *ops, void *xport_data)
{
  if (!inited)
    qmi_csi_init();

  if (ops->send_legacy == NULL && ops->send == NULL)
  {
    /* Do not allow an xport without either send */
    return;
  }

  if (ops->open_legacy == NULL && ops->open == NULL)
  {
    return;
  }

  if (num_xports < MAX_XPORTS)
  {
    xport_tbl[num_xports].ops = ops;
    xport_tbl[num_xports].xport_data = xport_data;
    num_xports++;
  }
}

/*===========================================================================
  FUNCTION  qmi_csi_msg_list_sort
  ===========================================================================*/

void qmi_csi_msg_list_sort
(
  void * arr,
  size_t count,
  size_t e_size,
  void * key_ptr
)
{
  int i, j;
  uint16_t key;
  if(arr == NULL || count == 0 || e_size == 0 || key_ptr == NULL)
    return;
  for (i = 1; i < count; i++)
  {
    key = GET_MSGID_FROM_LIST(arr, i, e_size);//key = arr[i]
    QMI_MEM_COPY(key_ptr,e_size,GET_PTR_FROM_LIST(arr,i,e_size),e_size);
    for(j = i -1; j >= 0 && GET_MSGID_FROM_LIST(arr, j, e_size) > key; j--)
    {
      //arr[j+1] = arr[j]
      QMI_MEM_COPY(GET_PTR_FROM_LIST(arr,(j+1),e_size),e_size,GET_PTR_FROM_LIST(arr,j,e_size),e_size);
    }
    QMI_MEM_COPY(GET_PTR_FROM_LIST(arr,(j+1),e_size),e_size,key_ptr,e_size); //arr[j+1] = key
  }
}

/*===========================================================================
  FUNCTION  sort_handler_tbl
  ===========================================================================*/
static int sort_msg_list(unsigned int size, qmi_csi_req_handler_tbl_type *list)
{
  unsigned int i;
  qmi_csi_req_handler_tbl_type tmp_key;
  qmi_csi_msg_list_sort(list,size,sizeof(qmi_csi_req_handler_tbl_type),&tmp_key);
  for (i = 0; i < size; i++)
  {
    if (!list[i].service_process_req)
    {
      return QMI_CSI_INVALID_ARGS;
    }
  }
  return QMI_CSI_NO_ERR;
}

/*===========================================================================
  FUNCTION  set_req_handler_tbl
  ===========================================================================*/
static int set_req_handler_tbl(qmi_csi_service_type *svc, qmi_csi_options *options)
{
  int rc;

  if (!options->req_handler_size || !options->req_handler_tbl)
  {
    return QMI_CSI_INVALID_ARGS;
  }

  svc->req_handler_size = (uint16_t)options->req_handler_size;
  svc->req_handler_tbl = CALLOC(svc->req_handler_size, sizeof(qmi_csi_req_handler_tbl_type));
  if (!svc->req_handler_tbl)
  {
    return QMI_CSI_NO_MEM;
  }
  QMI_MEM_COPY(svc->req_handler_tbl, sizeof(qmi_csi_req_handler_tbl_type)* svc->req_handler_size,
               options->req_handler_tbl, sizeof(qmi_csi_req_handler_tbl_type)* svc->req_handler_size);
  rc = sort_msg_list(svc->req_handler_size, svc->req_handler_tbl);
  if (rc != QMI_CSI_NO_ERR)
  {
    FREE(svc->req_handler_tbl);
    svc->req_handler_tbl = NULL;
  }
  return rc;
}

/*=============================================================================
                            QMI CSI API Implementation
  =============================================================================*/

/*=============================================================================
  FUNCTION  qmi_csi_register_with_options
  =============================================================================*/
QMI_EXPORT qmi_csi_error
qmi_csi_register_with_options
(
  qmi_idl_service_object_type               service_obj,
  qmi_csi_connect                           service_connect,
  qmi_csi_disconnect                        service_disconnect,
  qmi_csi_process_req                       service_process_req,
  void                                      *service_cookie,
  qmi_csi_os_params                         *os_params,
  qmi_csi_options                           *options,
  qmi_csi_service_handle                    *service_provider
)
{
  qmi_csi_service_type *svc;
  qmi_csi_xport_type *xport;
  uint32_t service_id, i, max_msg_len;
  int rc;
  int active = 0;

  if (!service_obj || !service_connect || !service_disconnect || !service_provider)
    return QMI_CSI_INVALID_ARGS;

  *service_provider = INVALID_SVC_HANDLE;

  svc = CALLOC(1, sizeof(qmi_csi_service_type));
  if (!svc)
  {
    /* failed to allocate memory */
    return QMI_CSI_NO_MEM;
  }

  LINK_INIT(svc->link);
  LIST_INIT(svc->client_list);
  svc->service_obj = service_obj;
  svc->service_connect = service_connect;
  svc->service_disconnect = service_disconnect;
  svc->service_process_req = service_process_req;
  svc->service_process_pre_req = NULL;
  svc->service_cookie = service_cookie;
  svc->resume_ind_cb = NULL;
  svc->log_message_cb = NULL;
  svc->log_message_ext_cb = NULL;

  /* Set xport options to its default values */
  svc->xport_options.rate_limited_queue_size = DEFAULT_MAX_TX_BUFS;
  svc->xport_options.scope_valid = 0;

  if (qmi_idl_get_service_id(service_obj, &service_id) != QMI_IDL_LIB_NO_ERR ||
      qmi_idl_get_idl_version(service_obj, &(svc->idl_version)) != QMI_IDL_LIB_NO_ERR ||
      qmi_idl_get_max_service_len(service_obj, &max_msg_len) != QMI_IDL_LIB_NO_ERR)
  {
    FREE(svc);
    return QMI_CSI_INTERNAL_ERR;
  }

  if (options)
  {
    if (options->options_set & QMI_CSI_OPTIONS_INSTANCE_ID_VALID)
    {
      svc->idl_version |= SET_INSTANCE(options->instance_id);
    }

    if (options->options_set & QMI_CSI_OPTIONS_MAX_OUTSTANDING_INDS_VALID)
    {
      if (options->max_outstanding_inds > 0)
      {
        svc->xport_options.rate_limited_queue_size = options->max_outstanding_inds;
      }
    }

    if (options->options_set & QMI_CSI_OPTIONS_SCOPE_VALID)
    {
      svc->xport_options.scope = options->scope;
      svc->xport_options.scope_valid = 1;
    }

    if (options->options_set & QMI_CSI_OPTIONS_RAW_REQUEST_VALID)
    {
      FREE(svc);
      return QMI_CSI_NOT_SUPP_ERR;
    }

    if (options->options_set & QMI_CSI_OPTIONS_PRE_REQUEST_VALID)
    {
      svc->service_process_pre_req = options->pre_request_cb;
    }

    if (options->options_set & QMI_CSI_OPTIONS_RESUME_VALID)
    {
      svc->resume_ind_cb = options->resume_ind_cb;
    }

    if (options->options_set & QMI_CSI_OPTIONS_REQ_HANDLER_TBL_VALID)
    {
      rc = set_req_handler_tbl(svc, options);
      if (rc != QMI_CSI_NO_ERR)
      {
        FREE(svc);
        return (qmi_csi_error)rc;
      }
    }

    if (options->options_set & QMI_CSI_OPTIONS_LOG_MSG_CB_VALID)
    {
      svc->log_message_cb = options->log_msg_cb;
    }

    if(options->options_set & QMI_CSI_OPTIONS_LOG_MSG_EXT_CB_VALID)
    {
      svc->log_message_ext_cb = options->log_msg_ext_cb;
    }

    if (options->options_set & QMI_CSI_OPTIONS_ASYNC_CONN_CB_VALID)
    {
      svc->async_conn_cb = 1;
      for(i = 0; i < num_xports; i++)
      {
        if(!(xport_tbl[i].ops->conn_complete))
        {
          FREE(svc);
          return QMI_CSI_NOT_SUPP_ERR;
        }
      }
    }
  }
  if (!service_process_req && !svc->req_handler_tbl)
  {
    FREE(svc);
    return QMI_CSI_INVALID_ARGS;
  }

  max_msg_len += QMI_HEADER_SIZE;

  /* handle OS-specific parameters such as storing away signal/event & TCB */
#ifdef QMI_CSI_OS_PARAMS_PROLOG
  QMI_CSI_OS_PARAMS_PROLOG(svc, os_params);
#endif

  svc->xports = CALLOC(num_xports, sizeof(qmi_csi_xport_type *));
  if (!svc->xports)
  {
    FREE(svc);
    return QMI_CSI_NO_MEM;
  }
  svc->num_xports = 0;

  /* open up all the known xports and register service */
  for (i = 0; i < num_xports; i++)
  {
    xport = CALLOC(1, sizeof(qmi_csi_xport_type));
    if (!xport)
    {
      continue;
    }
    xport->ops = xport_tbl[i].ops;
    xport->addr_len = QCSI_MIN(MAX_ADDR_LEN, xport->ops->addr_len());
    xport->service = svc;
    /* open xport */
    if (xport->ops->open_legacy)
    {
      xport->handle = xport->ops->open_legacy(xport_tbl[i].xport_data, xport,
                                              max_msg_len, os_params);
    }
    else
    {
      xport->handle = xport->ops->open(xport_tbl[i].xport_data, xport,
                                       max_msg_len, os_params, &svc->xport_options);
    }

    if (!xport->handle)
    {
      FREE(xport);
      continue;
    }

    /* At least one xport needs to be closed */
    svc->xports[svc->num_xports] = xport;
    svc->num_xports++;

    /* register service on xport */
    if (xport->ops->reg(xport->handle, service_id, svc->idl_version)
        != QMI_CSI_NO_ERR)
    {
      continue;
    }

    active = 1;
  }

  if (!active)
  {
    uint32_t num_xports = svc->num_xports;

    if (num_xports == 0)
    {
      FREE(svc->xports);
      FREE(svc);
      return QMI_CSI_INTERNAL_ERR;
    }

    /* num_xports is cached as close() could
     * free svc */
    for (i = 0; i < num_xports; i++)
    {
      if (svc->xports[i] && svc->xports[i]->handle)
      {
        svc->xports[i]->ops->close(svc->xports[i]->handle);
        /* svc->xports[i] freed in close.
         * svc freed in last close */
      }
    }
    return QMI_CSI_INTERNAL_ERR;
  }

  /* handle OS-specific parameters such as returning fd_set */
#ifdef QMI_CSI_OS_PARAMS_EPILOG
  QMI_CSI_OS_PARAMS_EPILOG(svc, params);
#endif

  /* add to service list */
  *service_provider = add_service(svc);

  return QMI_CSI_NO_ERR;
}

/*=============================================================================
  FUNCTION  qmi_csi_register
  =============================================================================*/
QMI_EXPORT qmi_csi_error
qmi_csi_register
(
  qmi_idl_service_object_type               service_obj,
  qmi_csi_connect                           service_connect,
  qmi_csi_disconnect                        service_disconnect,
  qmi_csi_process_req                       service_process_req,
  void                                      *service_cookie,
  qmi_csi_os_params                         *os_params,
  qmi_csi_service_handle                    *service_provider
)
{
  return qmi_csi_register_with_options(
    service_obj,
    service_connect,
    service_disconnect,
    service_process_req,
    service_cookie,
    os_params,
    NULL,
    service_provider);
}

/*=============================================================================
  FUNCTION  qmi_csi_handle_event
  =============================================================================*/
QMI_EXPORT qmi_csi_error
qmi_csi_handle_event
(
  qmi_csi_service_handle                    service_provider,
  qmi_csi_os_params                         *os_params
)
{
  qmi_csi_service_type *svc;
  qmi_csi_xport_type **xports;
  unsigned int i;
  uint32_t cnt_xports;

  LOCK(&service_list_lock);
  svc = find_service(service_provider);

  if (!svc)
  {
    UNLOCK(&service_list_lock);
    return QMI_CSI_INVALID_HANDLE;
  }

  /* save pointer to xport table and unlock, service must not unregister
   * at the same time since we don't hold the lock
   */
  xports = svc->xports;
  cnt_xports = svc->num_xports;
  UNLOCK(&service_list_lock);

  /* walk through the xport list and call handle_event on each, if avail */
  for (i = 0; i < cnt_xports; i++)
  {
    if (xports[i] && xports[i]->ops->handle_event)
      xports[i]->ops->handle_event(xports[i]->handle, os_params);
  }


  return QMI_CSI_NO_ERR;
}

/*=============================================================================
  FUNCTION  qmi_csi_send_resp_internal
  =============================================================================*/
qmi_csi_error
qmi_csi_send_resp_internal
(
  qmi_req_handle     req_handle,
  unsigned int       msg_id,
  void               *c_struct,
  unsigned int       c_struct_len,
  int                encode
)
{
  qmi_csi_txn_type *txn;
  qmi_csi_client_type *clnt;
  qmi_csi_error rc;

  if (c_struct_len <= 0)
    return QMI_CSI_INVALID_ARGS;

  /* as long as we hold the txn_list_lock, the client, service, and xport ptrs
   * will be valid
   */
  LOCK(&txn_list_lock);
  txn = find_txn(req_handle);

  if (!txn || msg_id > 0xffff || txn->msg_id != (uint16_t)msg_id || !txn->client || !txn->client->service ||
      !txn->client->xport.xport)
  {
    rc = QMI_CSI_INVALID_HANDLE;
    goto send_resp_bail;
  }

  clnt = txn->client;

  /* remove the transaction from the active list so it doesn't get freed
   * while we are holding onto the pointer
   */
  remove_txn(clnt, txn);

  /* encode and send */
  rc = internal_send(clnt->service, clnt, QMI_IDL_RESPONSE, txn->txn_id,
                     (uint16_t)msg_id, c_struct, c_struct_len, encode);

  release_txn(txn);

send_resp_bail:
  UNLOCK(&txn_list_lock);
  return rc;
}

/*=============================================================================
  FUNCTION  qmi_csi_send_resp
  =============================================================================*/
QMI_EXPORT qmi_csi_error
qmi_csi_send_resp
(
  qmi_req_handle     req_handle,
  unsigned int       msg_id,
  void               *c_struct,
  unsigned int       c_struct_len
)
{
  return qmi_csi_send_resp_internal(req_handle, msg_id, c_struct, c_struct_len, 1);
}

/*=============================================================================
  FUNCTION  qmi_csi_send_resp_raw
  =============================================================================*/
QMI_EXPORT qmi_csi_error
qmi_csi_send_resp_raw
(
  qmi_req_handle     req_handle,
  unsigned int       msg_id,
  void               *buf,
  unsigned int       buf_len
)
{
  return qmi_csi_send_resp_internal(req_handle, msg_id, buf, buf_len, 0);
}

/*=============================================================================
  FUNCTION  qmi_csi_send_ind_internal
  =============================================================================*/
qmi_csi_error
qmi_csi_send_ind_internal
(
  qmi_client_handle  client_handle,
  unsigned int       msg_id,
  void               *c_struct,
  unsigned int       c_struct_len,
  int                encode
)
{
  qmi_csi_client_type *clnt;
  qmi_csi_error rc;

  LOCK(&client_list_lock);
  clnt = find_client(client_handle);

  if (!clnt || !clnt->service || !clnt->xport.xport)
  {
    rc = QMI_CSI_INVALID_HANDLE;
    goto send_ind_bail;
  }

  if(!qmi_csi_filter_can_send_ind(clnt,msg_id))
  {
    UNLOCK(&client_list_lock);
    return QMI_CSI_IND_FILTERED;
  }
  /* encode and send */
  rc = internal_send(clnt->service, clnt, QMI_IDL_INDICATION, clnt->next_ind_txn_id,
                     (uint16_t)msg_id, c_struct, c_struct_len, encode);
  if (rc == QMI_CSI_NO_ERR)
  {
    clnt->next_ind_txn_id++;
    if (clnt->next_ind_txn_id == 0)
    {
      clnt->next_ind_txn_id++;
    }
  }


send_ind_bail:
  UNLOCK(&client_list_lock);
  return rc;
}

/*=============================================================================
  FUNCTION  qmi_csi_send_ind
  =============================================================================*/
QMI_EXPORT qmi_csi_error
qmi_csi_send_ind
(
  qmi_client_handle  client_handle,
  unsigned int       msg_id,
  void               *c_struct,
  unsigned int       c_struct_len
)
{
  return qmi_csi_send_ind_internal(client_handle, msg_id, c_struct, c_struct_len, 1);
}

/*=============================================================================
  FUNCTION  qmi_csi_send_ind_raw
  =============================================================================*/
QMI_EXPORT qmi_csi_error
qmi_csi_send_ind_raw
(
  qmi_client_handle  client_handle,
  unsigned int       msg_id,
  void               *buf,
  unsigned int       buf_len
)
{
  return qmi_csi_send_ind_internal(client_handle, msg_id, buf, buf_len, 0);
}

/*=============================================================================
  FUNCTION  qmi_csi_send_broadcast_ind_internal
  =============================================================================*/
qmi_csi_error
qmi_csi_send_broadcast_ind_internal
(
  qmi_csi_service_handle   service_provider,
  unsigned int             msg_id,
  void                     *c_struct,
  unsigned int             c_struct_len,
  int                      encode
)
{
  qmi_csi_service_type *svc;
  qmi_csi_error rc;

  /* lock client list first so if we find the service, the client list is
   * not going to be changed
   */
  LOCK(&client_list_lock);
  LOCK(&service_list_lock);
  svc = find_service(service_provider);

  if (!svc)
  {
    rc = QMI_CSI_INVALID_HANDLE;
    goto broadcast_ind_bail;
  }

  /* encode and send */
  rc = internal_send(svc, NULL, QMI_IDL_INDICATION, 0, (uint16_t)msg_id,
                     c_struct, c_struct_len, encode);

broadcast_ind_bail:
  UNLOCK(&service_list_lock);
  UNLOCK(&client_list_lock);
  return rc;
}

/*=============================================================================
  FUNCTION  qmi_csi_send_broadcast_ind
  =============================================================================*/
QMI_EXPORT qmi_csi_error
qmi_csi_send_broadcast_ind
(
  qmi_csi_service_handle   service_provider,
  unsigned int             msg_id,
  void                     *c_struct,
  unsigned int             c_struct_len
)
{
  return qmi_csi_send_broadcast_ind_internal(service_provider, msg_id,
                                             c_struct, c_struct_len, 1);
}

/*=============================================================================
  FUNCTION  qmi_csi_send_broadcast_ind_raw
  =============================================================================*/
QMI_EXPORT qmi_csi_error
qmi_csi_send_broadcast_ind_raw
(
  qmi_csi_service_handle   service_provider,
  unsigned int             msg_id,
  void                     *buf,
  unsigned int             buf_len
)
{
  return qmi_csi_send_broadcast_ind_internal(service_provider, msg_id,
                                             buf, buf_len, 0);
}

/*=============================================================================
  FUNCTION  qmi_csi_unregister
  =============================================================================*/
QMI_EXPORT qmi_csi_error
qmi_csi_unregister
(
  qmi_csi_service_handle     service_provider
)
{
  qmi_csi_service_type *svc;
  qmi_csi_xport_type **xports;
  unsigned int i;
  uint32_t cnt_xports;

  /* Tricky problem because we may get a callback at the same time as the xport
   * is being closed. Instead, a close callback is used to synchronize freeing
   * of memory. We just need to close the transports here.
   */
  LOCK(&service_list_lock);
  svc = find_service(service_provider);

  if (!svc)
  {
    UNLOCK(&service_list_lock);
    return QMI_CSI_INVALID_HANDLE;
  }

  /* save pointer to xport table and unlock */
  xports = svc->xports;
  cnt_xports = svc->num_xports;
  UNLOCK(&service_list_lock);

  for (i = 0; i < cnt_xports; i++)
  {
    if (xports[i])
    {
      xports[i]->ops->close(xports[i]->handle);
    }
  }

  return QMI_CSI_NO_ERR;
}
/*=============================================================================
  FUNCTION  qmi_csi_get_txn_id
  =============================================================================*/
QMI_EXPORT qmi_csi_error
qmi_csi_get_txn_id
(
  qmi_req_handle     req_handle,
  unsigned int       *txn_id
)
{
  qmi_csi_txn_type *txn;

  if (!txn_id)
    return QMI_CSI_INTERNAL_ERR;

  LOCK(&txn_list_lock);
  txn = find_txn(req_handle);
  if (!txn)
  {
    UNLOCK(&txn_list_lock);
    return QMI_CSI_INVALID_HANDLE;
  }

  *txn_id = txn->txn_id;
  UNLOCK(&txn_list_lock);
  return  QMI_CSI_NO_ERR;
}

/*=============================================================================
FUNCTION  qmi_csi_conn_complete
=============================================================================*/
QMI_EXPORT void qmi_csi_conn_complete
(
  qmi_client_handle client_handle,
  unsigned int      accepted
)
{
  qmi_csi_client_type *clnt;
  qmi_csi_xport_type *xport;

  LOCK(&client_list_lock);
  clnt = find_client(client_handle);

  if(!clnt || !clnt->service || !clnt->xport.xport)
  {
    UNLOCK(&client_list_lock);
    return;
  }

  xport = clnt->xport.xport;
  if(!accepted)
  {
    free_client(clnt->service, clnt);
  }
  UNLOCK(&client_list_lock);

  if(xport->ops->conn_complete)
  {
    xport->ops->conn_complete(xport->handle, accepted);
  }
}
