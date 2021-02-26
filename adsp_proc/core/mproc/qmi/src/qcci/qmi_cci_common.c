/******************************************************************************
  @file    qmi_cci_common.c
  @brief   The QMI common client interface common module

  DESCRIPTION
  QMI common client routines.  All client will be build on top of these
  routines for initializing, sending messages and receiving responses/
  indications.

  INITIALIZATION AND SEQUENCING REQUIREMENTS
  qmi_client_init() needs to be called before sending or receiving of any
  service specific messages

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2016 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1/mproc/qmi/src/qcci/qmi_cci_common.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
===========================================================================*/

/*===========================================================================
                      INCLUDES
===========================================================================*/
#include <string.h>
#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"

/*===========================================================================
                      MACROS AND DEFINES
===========================================================================*/

/* Backward compatibility macros */
#ifndef QMI_CCI_OS_LOG_TX_EXT
#define QMI_CCI_OS_LOG_TX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do {  \
    uint8_t tmp_header[QMI_HEADER_SIZE];  \
    encode_header(tmp_header, cntl_flag, txn_id, msg_id, msg_len);  \
    QMI_CCI_OS_LOG_TX(tmp_header);  \
  } while(0)
#endif

#ifndef QMI_CCI_OS_LOG_RX_EXT
#define QMI_CCI_OS_LOG_RX_EXT(svc_obj, cntl_flag, txn_id, msg_id, msg_len, addr, addr_len) \
  do {  \
    uint8_t tmp_header[QMI_HEADER_SIZE];  \
    encode_header(tmp_header, cntl_flag, txn_id, msg_id, msg_len);  \
    QMI_CCI_OS_LOG_RX(tmp_header);  \
  } while(0)
#endif

/* By default there is no OS Parameters to copy
 * (This is the case for all OSes except REX). Note
 * this is NOT supposed to copy the entire OS parameters
 * variable but must copy just the user provided parameters
 * which will be required to call QMI_CCI_OS_SIGNAL_INIT* */
#ifndef QMI_CCI_COPY_OS_PARAMS
#define QMI_CCI_COPY_OS_PARAMS(dest, src)
#endif

#ifndef QMI_CCI_OS_SIGNAL_INIT_SELF
#define QMI_CCI_OS_SIGNAL_INIT_SELF(ptr, os_params) QMI_CCI_OS_SIGNAL_INIT(ptr, os_params)
#endif

#ifndef QMI_CCI_LOG_ERR
#define QMI_CCI_LOG_ERR(fmt, a, b, c)
#endif

#ifndef QMI_CCI_OS_SIGNAL_START_TIMER
#define QMI_CCI_OS_SIGNAL_START_TIMER(ptr, timeout_ms)
#endif

#define QMI_CCI_FREE_TXN(txn) do {\
  if((txn)->type & TXN_SYNC)  \
    QMI_CCI_OS_SIGNAL_DEINIT(&txn->signal); \
  LOCK_DEINIT(&txn->lock);  \
  FREE(txn);  \
}while(0)

/* Use only when txn_list_lock is held */
#define QMI_CCI_INC_TXN(txn) do { \
  (txn)->ref_count++; \
} while(0)

#define QMI_CCI_DEC_TXN(txn) do {  \
  (txn)->ref_count--; \
  if((txn)->ref_count == 0) { \
    QMI_CCI_FREE_TXN(txn);  \
  } \
} while(0)

#define QMI_CCI_INC_TXN_SAFE(clnt,txn) do {  \
  LOCK(&(clnt)->info.client.txn_list_lock); \
  QMI_CCI_INC_TXN(txn); \
  UNLOCK(&(clnt)->info.client.txn_list_lock); \
} while(0)


#define QMI_CCI_DEC_TXN_SAFE(clnt,txn) do {  \
  LOCK(&(clnt)->info.client.txn_list_lock); \
  QMI_CCI_DEC_TXN(txn);  \
  UNLOCK(&(clnt)->info.client.txn_list_lock); \
} while(0)

#define INVALIDATE_TXN_RX_BUF(txn) do { \
  LOCK(&(txn)->lock); \
  (txn)->rx_buf = NULL; \
  (txn)->rx_buf_len = 0;  \
  (txn)->rx_cb_data = NULL; \
  UNLOCK(&(txn)->lock); \
} while(0)

#define INVALIDATE_TXN_TX_BUF(txn) do { \
  if((txn)->tx_buf) \
    FREE((txn)->tx_buf);  \
  (txn)->tx_buf = NULL; \
  (txn)->tx_buf_len = 0;  \
} while(0)

/* 0 is a surprisingly common 'random' number. So consider it as invalid */
#define INVALID_CLID   0
#define INVALID_HANDLE 0

/* Client handle even though handed out as a pointer is internally
 * used as an integer. Considering the value was never a pointer, this
 * cast has no consequence with 32/64 bit usage */
#define CAST_HANDLE(handle) ((unsigned int)(uintptr_t)(handle))
#define CAST_INDEX(idx) ((qmi_client_type)(uintptr_t)(idx))
#define CLIENT_HANDLE(clnt) CAST_INDEX(((clnt)->priv.clid))

#define CLIENT_TBL_COUNT (16)
#define CLID2IDX(clid) ((clid) & (CLIENT_TBL_COUNT - 1))

#define QCCI_MIN(a, b) ((a) > (b) ? (b) : (a))

#define CLIENT_XPORT_VALID(clnt) \
          (LIST_HEAD((clnt)->xport)->ops != NULL && \
           LIST_HEAD((clnt)->xport)->handle != NULL)

/* Allow compilation env to set this differently
 * for memory constrained systems */
#ifndef MAX_XPORTS
#define MAX_XPORTS 4
#endif

/*===========================================================================
                      LOCAL DATATYPES
===========================================================================*/

struct qmi_cci_xport_tbl_s
{
  qmi_cci_xport_ops_type *ops;
  void *xport_data;
  uint8_t uid;
};

struct qmi_cci_waiter_s
{
  QMI_CCI_OS_SIGNAL signal;
  int               rc;
};

/*===========================================================================
                      MODULE GLOBALS
===========================================================================*/

static struct qmi_cci_xport_tbl_s xport_tbl[MAX_XPORTS];
static unsigned int num_xports = 0;
static uint8_t next_xport_uid = 0;
static qmi_cci_lock_type xport_tbl_lock;

static qmi_cci_lock_type qcci_tbl_lock;
static unsigned int next_clid = 1;
static LIST(qmi_cci_client_type, qcci_client_tbl)[CLIENT_TBL_COUNT];
static int qcci_fw_inited = 0;


/*===========================================================================
                      MODULE LOCAL FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_cci_log_tx
===========================================================================*/
static void qmi_cci_log_tx
(
  qmi_cci_client_type *clnt,
  uint8_t cntl_flag,
  unsigned int txn_id,
  unsigned int msg_id,
  void *raw_msg,
  unsigned int len
)
{
  QMI_CCI_OS_LOG_TX_EXT(clnt->service_obj, cntl_flag,
                        (uint16_t)txn_id, (uint16_t)msg_id,
                        (uint16_t)len, clnt->info.client.server_addr, MAX_ADDR_LEN);
  if(clnt->info.client.log_cb)
  {
    clnt->info.client.log_cb(CLIENT_HANDLE(clnt), QMI_IDL_REQUEST, msg_id, txn_id,
                              raw_msg, len, QMI_NO_ERR,
                              clnt->info.client.log_cb_data);
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_log_rx
===========================================================================*/
static void qmi_cci_log_rx
(
  qmi_cci_client_type *clnt,
  uint8_t cntl_flag,
  unsigned int txn_id,
  unsigned int msg_id,
  void *raw_msg,
  unsigned int len,
  int status
)
{
  if(status == QMI_NO_ERR)
  {
    QMI_CCI_OS_LOG_RX_EXT(clnt->service_obj, cntl_flag, txn_id, msg_id,
                            len, clnt->info.client.server_addr, MAX_ADDR_LEN);
  }
  if(clnt->info.client.log_cb)
  {
    qmi_idl_type_of_message_type type = cntl_flag == QMI_RESPONSE_CONTROL_FLAG ?
                                          QMI_IDL_RESPONSE : QMI_IDL_INDICATION;
    clnt->info.client.log_cb(CLIENT_HANDLE(clnt), type, msg_id, txn_id,
                              raw_msg, len, status,
                              clnt->info.client.log_cb_data);
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_common_init
===========================================================================*/
void qmi_cci_common_init(void)
{
  if(qcci_fw_inited == 0)
  {
    LOCK_INIT(&qcci_tbl_lock);
    LOCK_INIT(&xport_tbl_lock);
    qcci_fw_inited = 1;
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_common_deinit
===========================================================================*/
void qmi_cci_common_deinit(void)
{
  if(qcci_fw_inited)
  {
    qcci_fw_inited = 0;
    LOCK_DEINIT(&qcci_tbl_lock);
    LOCK_DEINIT(&xport_tbl_lock);
  }
}


/*===========================================================================
  FUNCTION  qmi_cci_client_lookup
===========================================================================*/
/*!
@brief

  Lookup and return the client structure by taking in the client ID as a key

@return

  pointer to the client handle upon success.

@note
  qcci_tbl_lock must be held by the caller
*/
/*=========================================================================*/
static qmi_cci_client_type *qmi_cci_client_lookup
(
  unsigned int clid
)
{
  qmi_cci_client_type *clnt;

  LIST_FIND(qcci_client_tbl[CLID2IDX(clid)], clnt, priv.link,
                      clnt->priv.clid == clid);
  return clnt;
}

/*===========================================================================
  FUNCTION  qmi_cci_client_alloc
===========================================================================*/
/*!
@brief

  Allocate a client handle and return its pointer

@return

  pointer to the client handle upon success.

@note
  the client handle will be inserted into the client list.
  the pointer MUST be freed using qmi_cci_client_free() only.
*/
/*=========================================================================*/
static qmi_client_error_type qmi_cci_client_alloc
(
  qmi_idl_service_object_type service_obj,
  qmi_cci_client_category_type category,
  qmi_client_os_params *os_params,
  qmi_client_ind_cb ind_cb,
  void *ind_cb_data,
  qmi_cci_client_type **client
 )
{
  qmi_cci_client_type *clnt;
  unsigned int idx;
  *client = NULL;
  /* This is NOT thread safe, but coming down to it,
   * it is better than nothing protecting locking an
   * uninitialized lock */
  if(!qcci_fw_inited)
    return QMI_CLIENT_FW_NOT_UP;

  clnt = (qmi_cci_client_type *)CALLOC(1, sizeof(*clnt));
  if(!clnt)
    return QMI_CLIENT_ALLOC_FAILURE;

  QMI_CCI_OS_SIGNAL_INIT(&clnt->signal, os_params);
  #ifdef QMI_CCI_OS_SIGNAL_VALID
  if(!QMI_CCI_OS_SIGNAL_VALID(&clnt->signal))
  {
    FREE(clnt);
    return QMI_CLIENT_INVALID_SIG;
  }
  #endif

  if(category == QMI_CCI_NOTIFIER_CLIENT)
  {
    if(os_params)
    {
      QMI_CCI_OS_EXT_SIGNAL_INIT(clnt->info.notifier.ext_signal, os_params);
      #ifdef QMI_CCI_OS_EXT_SIGNAL_VALID
      if(!QMI_CCI_OS_EXT_SIGNAL_VALID(clnt->info.notifier.ext_signal))
      {
        QMI_CCI_OS_SIGNAL_DEINIT(&clnt->signal);
        FREE(clnt);
        return QMI_CLIENT_INVALID_SIG;
      }
      #endif
    }
    else
    {
      clnt->info.notifier.ext_signal = NULL;
    }

    clnt->info.notifier.notify_cb = NULL;
    clnt->info.notifier.notify_cb_data = NULL;
    clnt->info.notifier.notify_pending = 0;
  }
  else
  {
    LOCK_INIT(&clnt->info.client.txn_list_lock);
    LIST_INIT(clnt->info.client.txn_list);
    clnt->info.client.next_txn_id = 1;

    LOCK_INIT(&clnt->info.client.tx_q_lock);
    LIST_INIT(clnt->info.client.tx_q);
    clnt->info.client.accepting_txns = 1;

    clnt->info.client.err_cb = NULL;
    clnt->info.client.err_cb_data = NULL;
    clnt->info.client.err_pending = 0;

    clnt->info.client.log_cb = NULL;
    clnt->info.client.log_cb_data = NULL;

    clnt->info.client.ind_cb = ind_cb;
    clnt->info.client.ind_cb_data = ind_cb_data;
  }

  LOCK_INIT(&clnt->info_lock);
  LIST_INIT(clnt->xport);
  clnt->category = category;
  clnt->service_obj = service_obj;
  clnt->release_cb = NULL;
  clnt->release_cb_data = NULL;
  /* Allocate one ref for this call */
  clnt->priv.ref_count = 1;
  LINK_INIT(clnt->priv.link);

  LOCK(&qcci_tbl_lock);

  clnt->priv.clid = INVALID_CLID;

  /* Get a new unused and valid clid */
  while(clnt->priv.clid == INVALID_CLID)
  {
    clnt->priv.clid = next_clid++;
    if(qmi_cci_client_lookup(clnt->priv.clid) != NULL)
    {
      clnt->priv.clid = INVALID_CLID;
    }
  }

  idx = CLID2IDX(clnt->priv.clid);
  LIST_ADD(qcci_client_tbl[idx], clnt, priv.link);
  UNLOCK(&qcci_tbl_lock);
  *client = clnt;
  return QMI_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_cci_client_unlink
===========================================================================*/
/*!
@brief

  Unlinks a client handle from the client list.

@return

  None

@note
  Needs qcci_tbl_lock to be held by the caller.
*/
/*=========================================================================*/
static void qmi_cci_client_unlink(qmi_cci_client_type *clnt)
{
  qmi_cci_client_type *i;
  unsigned int idx;
  if(!clnt)
    return;
  idx = CLID2IDX(clnt->priv.clid);
  LIST_FIND(qcci_client_tbl[idx], i, priv.link,
                i->priv.clid == clnt->priv.clid);
  if(i)
  {
    LIST_REMOVE(qcci_client_tbl[idx], clnt, priv.link);
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_client_free
===========================================================================*/
/*!
@brief

  Frees a client handle

@return

  None

@note
  The client handle will be unlinked if required.
*/
/*=========================================================================*/
static void qmi_cci_client_free(qmi_cci_client_type *clnt)
{
  struct qmi_cci_xport_s *xport;

  /* Unlink just to be sure, in most cases this should do nothing
   * as qmi_cci_get_ref(handle, 1) would have removed this
   * from the list */
  LOCK(&qcci_tbl_lock);
  qmi_cci_client_unlink(clnt);
  UNLOCK(&qcci_tbl_lock);

  QMI_CCI_OS_SIGNAL_DEINIT(&clnt->signal);
  LOCK_DEINIT(&clnt->info_lock);

  if(clnt->category == QMI_CCI_NOTIFIER_CLIENT)
  {
    if(clnt->info.notifier.ext_signal)
    {
      QMI_CCI_OS_SIGNAL_DEINIT(clnt->info.notifier.ext_signal);
    }
  }
  else
  {
    LOCK_DEINIT(&clnt->info.client.txn_list_lock);
    LOCK_DEINIT(&clnt->info.client.tx_q_lock);
  }

  if(clnt->release_cb)
  {
    clnt->release_cb(clnt->release_cb_data);
  }

  /* Now it is safe to modify the xport list */
  while(NULL != (xport = LIST_HEAD(clnt->xport)))
  {
    LIST_REMOVE(clnt->xport, xport, link);
    FREE(xport);
  }
  FREE(clnt);
}

/*===========================================================================
  FUNCTION  qmi_cci_get_ref
===========================================================================*/
/*!
@brief

  Gets a reference on the client. If the unlink parameter is set,
  then the client handle will be removed from the global table
  so future calls to qmi_cci_get_ref() will fail.

@return

  Pointer to the client handle if successful. NULL otherwise.

@note
  The client's reference must be released when one is done with it by
  calling qmi_cci_put_ref
*/
/*=========================================================================*/
static qmi_cci_client_type *qmi_cci_get_ref
(
  qmi_client_type client_handle,
  int unlink
)
{
  unsigned int clid = CAST_HANDLE(client_handle);
  qmi_cci_client_type *clnt;

  LOCK(&qcci_tbl_lock);
  clnt = qmi_cci_client_lookup(clid);
  if(!clnt)
  {
    UNLOCK(&qcci_tbl_lock);
    return NULL;
  }

  if(clnt->priv.ref_count < 0)
  {
    /* This should never happen, but the check is there
       so I can sleep at night */
    UNLOCK(&qcci_tbl_lock);
    return NULL;
  }
  clnt->priv.ref_count++;

  /* Unlink the client from the global table so future get_ref's fail */
  if(unlink)
    qmi_cci_client_unlink(clnt);

  UNLOCK(&qcci_tbl_lock);
  return clnt;
}

/*===========================================================================
  FUNCTION  qmi_cci_put_ref
===========================================================================*/
/*!
@brief

  Releases a reference on the client.

@return

  Current reference count after releasing one's reference.

@note
  The client structure must NOT be accessed after calling this function.
*/
/*=========================================================================*/
static void qmi_cci_put_ref(qmi_cci_client_type *clnt)
{
  int ref = 0;

  if(!clnt)
    return;

  LOCK(&qcci_tbl_lock);
  ref = --clnt->priv.ref_count;
  UNLOCK(&qcci_tbl_lock);

  if(ref == 0)
  {
    qmi_cci_client_free(clnt);
  }
}

/*===========================================================================
  FUNCTION  remove_txn
===========================================================================*/
/*!
@brief

  Find, remove a transaction from client's transaction list and release
  the list's reference count on the txn if found.

@return
  None

@note
  Caller must have a reference to the client structure.
*/
/*=========================================================================*/
static qmi_client_error_type remove_txn
(
  qmi_cci_client_type *clnt,
  qmi_cci_txn_type *txn
)
{
  qmi_cci_txn_type *i;
  qmi_client_error_type rc = QMI_INTERNAL_ERR;

  if(!clnt || !txn)
    return rc;

  LOCK(&clnt->info.client.txn_list_lock);
  LIST_FIND(clnt->info.client.txn_list, i, link, i == txn);
  if(i)
  {
    LIST_REMOVE(clnt->info.client.txn_list, i, link);
    QMI_CCI_DEC_TXN(i);
    rc = QMI_NO_ERR;
  }
  UNLOCK(&clnt->info.client.txn_list_lock);
  return rc;
}

/*===========================================================================
  FUNCTION  handle_txn_error
===========================================================================*/
/*!
@brief
  Handle transaction error base on its type and set return code to error

@note
  Transaction is freed in the async case. The thread waiting on a sync response
  will free the transation after waking up.

  The caller must have removed txn from all possible lists before calling
  this function.
*/
/*=========================================================================*/
static void handle_txn_error
(
 qmi_cci_client_type *clnt,
 qmi_cci_txn_type *txn,
 int error
 )
{
  if(!txn)
    return;

  qmi_cci_log_rx(clnt, QMI_IDL_RESPONSE, txn->txn_id, txn->msg_id, NULL, 0, error);

  txn->rc = error;
  if(txn->type & TXN_SYNC)
  {
    QMI_CCI_OS_SIGNAL_SET(&txn->signal);
  }
  else
  {
    if(txn->rx_cb)
    {
      txn->rx_cb(CLIENT_HANDLE(clnt), txn->msg_id, txn->rx_buf,
                   0, txn->rx_cb_data, txn->rc);
    }
  }
}



/*===========================================================================
  FUNCTION  qmi_client_get_txn
===========================================================================*/
/*!
@brief

  Create a transaction and return the handle

@return
  qmi_client_error_type

@note
  The transaction is added to the client's outstand transaction list
  The caller must have acquired a reference to the client structure.
*/
/*=========================================================================*/
static qmi_client_error_type qmi_client_get_txn
(
 qmi_cci_client_type              *clnt,
 qmi_cci_txn_enum_type             type,
 unsigned int                      msg_id,
 void                              *resp_buf,
 unsigned int                      resp_buf_len,
 qmi_client_recv_msg_async_cb      rx_cb,
 void                              *rx_cb_data,
 qmi_cci_txn_type                  **txn_handle
 )
{
  qmi_cci_txn_type *txn;

  LOCK(&clnt->info_lock);
  if(clnt->category != QMI_CCI_CONNECTED_CLIENT)
  {
    UNLOCK(&clnt->info_lock);
    return QMI_SERVICE_ERR;
  }
  UNLOCK(&clnt->info_lock);

  txn = (qmi_cci_txn_type *)CALLOC(1, sizeof(qmi_cci_txn_type));
  if(!txn)
    return QMI_CLIENT_ALLOC_FAILURE;

  LINK_INIT(txn->link);
  LINK_INIT(txn->tx_link);
  txn->type = type;
  txn->msg_id = msg_id;
  txn->rx_cb = rx_cb;
  txn->rx_cb_data = rx_cb_data;
  txn->rx_buf = (uint8_t *)resp_buf;
  txn->rx_buf_len = resp_buf_len;
  txn->tx_buf = NULL;
  txn->client = clnt;
  LOCK_INIT(&txn->lock);
  txn->ref_count = 2; /* The txn_list takes a reference.
                         The caller gets a reference too.
                         The txn_list's reference will be
                         released only when it is removed
                         from the list.
                         The caller’s reference will be released
                         When they are done. */

  /* Initialize a signal only for Sync transactions */
  if(type & TXN_SYNC)
  {
    QMI_CCI_OS_SIGNAL_INIT_SELF(&txn->signal, &clnt->signal);
#ifdef QMI_CCI_OS_SIGNAL_VALID
    if(!QMI_CCI_OS_SIGNAL_VALID(&txn->signal))
    {
      LOCK_DEINIT(&txn->lock);
      FREE(txn);
      return QMI_CLIENT_INVALID_SIG;
    }
#endif
  }

  LOCK(&clnt->info.client.txn_list_lock);
  while((txn->txn_id = clnt->info.client.next_txn_id++) == 0);
  LIST_ADD(clnt->info.client.txn_list, txn, link);
  UNLOCK(&clnt->info.client.txn_list_lock);

  *txn_handle = txn;
  return QMI_NO_ERR;
}

/*===========================================================================
  FUNCTION  get_dest_addr
===========================================================================*/
/*!
@brief

  Gets the destination address of the service.

@return

  QMI_NO_ERR if server exists and is valid (addr will be populated with the
             correct address)
  QMI_INTERNAL_ERR - Invalid parameters
  QMI_SERVICE_ERR - Address to the service is not valid (Service does not
  exist)

@note
  The caller must have taken a reference count.
*/
/*=========================================================================*/
static qmi_client_error_type get_dest_addr(qmi_cci_client_type *clnt, uint8_t *addr)
{
  if(!clnt || !addr)
    return QMI_CLIENT_PARAM_ERR;
  LOCK(&clnt->info_lock);
  if(clnt->category != QMI_CCI_CONNECTED_CLIENT)
  {
    QMI_CCI_LOG_ERR("qcci get_dest_addr: invalid server address. svc_id: %d",
                    clnt->service_obj->service_id, 0, 0);
    UNLOCK(&clnt->info_lock);
    return QMI_SERVICE_ERR;
  }
  QMI_MEM_COPY(addr, MAX_ADDR_LEN, clnt->info.client.server_addr, MAX_ADDR_LEN);
  UNLOCK(&clnt->info_lock);
  return QMI_NO_ERR;
}

/*===========================================================================
  FUNCTION  cleanup_client_lists
===========================================================================*/
/*!
@brief

  Cleans up all client lists.

@return
  None

@note
  The caller must have acquired a reference to the client structure.
*/
/*=========================================================================*/
static void cleanup_client_lists
(
  qmi_cci_client_type *clnt,
  int error
)
{
  qmi_cci_txn_type *txn;

  if(clnt->category == QMI_CCI_NOTIFIER_CLIENT)
  {
    return;
  }

  LOCK(&clnt->info.client.tx_q_lock);
  txn = LIST_HEAD(clnt->info.client.tx_q);
  LIST_INIT(clnt->info.client.tx_q);
  UNLOCK(&clnt->info.client.tx_q_lock);

  while(txn)
  {
    qmi_cci_txn_type *to_free = txn;
    txn = txn->tx_link.next;
    /* No need for lock as it is no longer in the list */
    INVALIDATE_TXN_TX_BUF(to_free);
    /* Give up tx_q list reference */
    QMI_CCI_DEC_TXN_SAFE(clnt, to_free);
  }

  LOCK(&clnt->info.client.txn_list_lock);
  /* The refcount for the txn_list gets transferred
   * into handle_txn_error */
  txn = LIST_HEAD(clnt->info.client.txn_list);
  LIST_INIT(clnt->info.client.txn_list);
  UNLOCK(&clnt->info.client.txn_list_lock);

  /* Handle error on each txn */
  while(txn)
  {
    qmi_cci_txn_type *to_free = txn;
    txn = txn->link.next;

    /* handle transaction error base on its type */
    handle_txn_error(clnt, to_free, error);
    INVALIDATE_TXN_RX_BUF(to_free);
    QMI_CCI_DEC_TXN_SAFE(clnt, to_free);
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_flush_tx_q
===========================================================================*/
/*!
@brief

  Tries and transmits all pending transactions in the tx queue.

@return
  None

@note
  The caller must have acquired a reference to the client structure.
*/
/*=========================================================================*/
void qmi_cci_flush_tx_q
(
  qmi_cci_client_type *clnt
)
{
  int rc;
  qmi_cci_txn_type *txn;
  uint8_t dest_addr[MAX_ADDR_LEN];

  rc = get_dest_addr(clnt, dest_addr);
  if(rc != QMI_NO_ERR)
  {
    cleanup_client_lists(clnt, QMI_SERVICE_ERR);
    return;
  }

  LOCK(&clnt->info.client.tx_q_lock);
  while(NULL != (txn = LIST_HEAD(clnt->info.client.tx_q)))
  {
    struct qmi_cci_xport_s *xport = LIST_HEAD(clnt->xport);

    if(clnt->info.client.accepting_txns && xport && xport->handle && xport->ops)
    {
      /* Implies that xport cannot do up calls in send. If it does,
       * the up call should not try to acquire the tx_q_lock */
      rc = xport->ops->send(xport->handle, dest_addr,
                              (uint8_t *)txn->tx_buf, txn->tx_buf_len);
    }
    else
    {
      rc = QMI_CLIENT_INVALID_CLNT;
    }

    /* xport is flow controlled, try again later */
    if(rc == QMI_XPORT_BUSY_ERR)
      break;

    LIST_REMOVE(clnt->info.client.tx_q, txn, tx_link);
    INVALIDATE_TXN_TX_BUF(txn);
    UNLOCK(&clnt->info.client.tx_q_lock);

    /* Release tx_q ref count */
    QMI_CCI_DEC_TXN_SAFE(clnt, txn);

    /* Error sending txn */
    if(rc != QMI_NO_ERR)
    {
      qmi_cci_txn_type *to_find = txn;
      QMI_CCI_LOG_ERR("QCCI qmi_cci_flush_tx_q: Error sending TXN: svc_id: %d txn_id: %d msg_id: %d",
                      clnt->service_obj->service_id, txn->txn_id, txn->msg_id);
      LOCK(&clnt->info.client.txn_list_lock);
      LIST_FIND(clnt->info.client.txn_list, txn, link, txn == to_find);
      if(txn)
      {
        LIST_REMOVE(clnt->info.client.txn_list, txn, link);
        /* Txn_list's ref count is transferred
         * into handle_txn_error */
        UNLOCK(&clnt->info.client.txn_list_lock);
        handle_txn_error(clnt, txn, rc);
        QMI_CCI_DEC_TXN_SAFE(clnt, txn);
      }
      else
      {
        UNLOCK(&clnt->info.client.txn_list_lock);
      }
      if(rc == QMI_SERVICE_ERR)
      {
        qmi_cci_xport_event_server_error(clnt, dest_addr, QMI_SERVICE_ERR);
      }
    }
    /* Lock and check the tx Q again */
    LOCK(&clnt->info.client.tx_q_lock);
  }
  UNLOCK(&clnt->info.client.tx_q_lock);
}

/*===========================================================================
  FUNCTION  qmi_cci_send
===========================================================================*/
/*!
@brief

  Transmit a message

@return
  None

@note
  The caller must have obtained a reference to the client handle.
  The caller should free the buffer only if this function returns error.
  The caller should provide buffers which are allocated on the heap only.
*/
/*=========================================================================*/
static qmi_client_error_type qmi_cci_send
(
  qmi_cci_client_type *clnt,
  qmi_cci_txn_type *txn,
  void *msg,
  uint32_t len
)
{
  int flush_req = 0;

  if(!txn || !len || !msg)
  {
    return QMI_CLIENT_PARAM_ERR;
  }

  /* Check server addr validity once before sending. */
  LOCK(&clnt->info_lock);
  if(clnt->category != QMI_CCI_CONNECTED_CLIENT)
  {
    UNLOCK(&clnt->info_lock);
    return QMI_SERVICE_ERR;
  }
  UNLOCK(&clnt->info_lock);

  /* Take a reference for the tx queue */
  QMI_CCI_INC_TXN_SAFE(clnt, txn);

  LOCK(&clnt->info.client.tx_q_lock);
  if(!clnt->info.client.accepting_txns)
  {
    QMI_CCI_LOG_ERR("QCCI qmi_cci_send: clnt has been released. svc_id: %d",
                    clnt->service_obj->service_id, 0, 0);
    UNLOCK(&clnt->info.client.tx_q_lock);
    /* Give up reference */
    QMI_CCI_DEC_TXN_SAFE(clnt, txn);
    return QMI_CLIENT_INVALID_CLNT;
  }
  txn->tx_buf = msg;
  txn->tx_buf_len = len;
  LIST_ADD(clnt->info.client.tx_q, txn, tx_link);
  /* Flush only if this is the first packet. If another
     packet is pending a flush, then we let the resume
     process continue the flush */
  flush_req = LIST_CNT(clnt->info.client.tx_q) == 1;
  UNLOCK(&clnt->info.client.tx_q_lock);
  if( flush_req)
  {
    qmi_cci_flush_tx_q(clnt);
  }

  qmi_cci_log_tx(clnt, QMI_REQUEST_CONTROL_FLAG, txn->txn_id, txn->msg_id,
                  (void *)((uint8_t *)msg + QMI_HEADER_SIZE), len - QMI_HEADER_SIZE);

  return QMI_NO_ERR;
}

/*===========================================================================
  FUNCTION  encode_and_send
===========================================================================*/
/*!
@brief

  Encode and send a message to the client

@return
  qmi_client_error_type

@note
  The caller must hold a reference to the client handle.
*/
/*=========================================================================*/
static qmi_client_error_type encode_and_send
(
 qmi_cci_client_type *clnt,
 qmi_cci_txn_type *txn,
 void *c_struct,
 unsigned int c_struct_len
)
{
  int rc;
  uint32_t max_msg_len;
  uint32_t out_len, idl_c_struct_len;
  unsigned char *msg;
  uint8_t cntl_flag = QMI_REQUEST_CONTROL_FLAG;
  qmi_idl_type_of_message_type msg_type = QMI_IDL_REQUEST;

  rc = qmi_idl_get_message_c_struct_len(clnt->service_obj, msg_type, (uint16_t)txn->msg_id,
      &idl_c_struct_len);
  if(rc != QMI_IDL_LIB_NO_ERR)
      return rc;

  /* Allow users to pass c_struct_len == 0. This is useful in cases when the c
     structure has only optional members (thus idl_c_struct_len would be non-zero)
     and the user requires to send the message with all options turned off */
  if(c_struct_len != 0 && c_struct_len != idl_c_struct_len)
    return QMI_CLIENT_PARAM_ERR;

  if(c_struct && c_struct_len)
  {
    rc = qmi_idl_get_max_message_len(clnt->service_obj, msg_type, (uint16_t)txn->msg_id,
        &max_msg_len);
    if(rc != QMI_IDL_LIB_NO_ERR)
      return rc;

    msg = (unsigned char *)MALLOC(max_msg_len + QMI_HEADER_SIZE);
    if(!msg)
      return QMI_CLIENT_ALLOC_FAILURE;

    rc = qmi_idl_message_encode(
        clnt->service_obj,
        msg_type,
        (uint16_t)txn->msg_id,
        c_struct,
        c_struct_len,
        msg + QMI_HEADER_SIZE,
        max_msg_len,
        (uint32_t *)&out_len);

    if(rc != QMI_IDL_LIB_NO_ERR)
    {
      FREE(msg);
      return rc;
    }
  }
  else
  {
    /* Empty message */
    out_len = 0;
    msg = (unsigned char *)MALLOC(QMI_HEADER_SIZE);
    if(!msg)
      return QMI_CLIENT_ALLOC_FAILURE;
  }

  /* fill in header */
  encode_header(msg, cntl_flag, (uint16_t)txn->txn_id, (uint16_t)txn->msg_id, (uint16_t)out_len);

  out_len += QMI_HEADER_SIZE;

  rc = qmi_cci_send(clnt, txn, msg, out_len);
  if(rc != QMI_NO_ERR)
  {
    FREE(msg);
  }

  return rc;
}

/*===========================================================================
 *   FUNCTION  qmi_cci_response_wait_loop
===========================================================================*/
/*!
 * @brief
 *
 * wait for a transaction response message.
 *
 * @return
 *  error code
 *
 * @note
 * The caller is required to hold a reference to the client structure.
 */
/*=========================================================================*/
static qmi_client_error_type qmi_cci_response_wait_loop
(
  qmi_cci_txn_type *txn,
  unsigned int timeout_msecs
)
{
  qmi_client_error_type ret = QMI_NO_ERR;

  do
  {
    QMI_CCI_OS_SIGNAL_WAIT(&txn->signal, timeout_msecs);
    QMI_CCI_OS_SIGNAL_CLEAR(&txn->signal);

    if(QMI_CCI_OS_SIGNAL_TIMED_OUT(&txn->signal))
    {
      ret = QMI_TIMEOUT_ERR;
      break;
    }

    /* Not a stray wake-up break out */
    if(txn->rc != QMI_TIMEOUT_ERR)
    {
      ret = txn->rc;
      break;
    }
  } while(1);

  return ret;
}

/*===========================================================================
  FUNCTION  release_cb_internal
===========================================================================*/
/*!
@brief

  Internal callback function used by qmi_client_release()

@return
  None

@note
  unblocks qmi_client_release()
*/
/*=========================================================================*/
static void release_cb_internal(void *cb_data)
{
  struct qmi_cci_waiter_s *waiter = (struct qmi_cci_waiter_s *)cb_data;
  if(waiter)
  {
    waiter->rc = QMI_NO_ERR;
    QMI_CCI_OS_SIGNAL_SET(&waiter->signal);
  }
}



/*===========================================================================
                   EXPORTED FUNCTIONS RELATING TO XPORT
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_cci_xport_closed
===========================================================================*/
void qmi_cci_xport_closed
(
 qmi_cci_client_type *clnt
 )
{
  /* Release the reference taken for this transport */
  qmi_cci_put_ref(clnt);
}

/*===========================================================================
  FUNCTION  qmi_cci_xport_event_new_server
===========================================================================*/
void qmi_cci_xport_event_new_server
(
 qmi_cci_client_type *clnt,
 void *addr
 )
{
  qmi_client_notify_cb notify_cb = NULL;
  void *cb_data = NULL;
  QMI_CCI_OS_SIGNAL *ext_signal = NULL;

  if(!clnt || !addr)
  {
    return;
  }

  LOCK(&clnt->info_lock);
  if(clnt->category == QMI_CCI_NOTIFIER_CLIENT)
  {
    notify_cb = clnt->info.notifier.notify_cb;
    cb_data   = clnt->info.notifier.notify_cb_data;
    if(!notify_cb)
      clnt->info.notifier.notify_pending = 1;
    ext_signal = clnt->info.notifier.ext_signal;
  }
  UNLOCK(&clnt->info_lock);

  if(ext_signal)
  {
    QMI_CCI_OS_SIGNAL_SET(ext_signal);
  }

  if(notify_cb)
  {
    notify_cb(CLIENT_HANDLE(clnt), clnt->service_obj, QMI_CLIENT_SERVICE_COUNT_INC, cb_data);
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_xport_event_server_error
===========================================================================*/
void qmi_cci_xport_event_server_error
(
 qmi_cci_client_type *clnt,
 void *addr,
 int error
 )
{
  qmi_client_error_cb err_cb = NULL;
  qmi_client_notify_cb notify_cb = NULL;
  QMI_CCI_OS_SIGNAL *ext_signal = NULL;
  void *cb_data = NULL;

  if(!clnt)
    return;

  LOCK(&clnt->info_lock);
  if(clnt->category == QMI_CCI_NOTIFIER_CLIENT)
  {
    ext_signal = clnt->info.notifier.ext_signal;
    notify_cb = clnt->info.notifier.notify_cb;
    cb_data   = clnt->info.notifier.notify_cb_data;
  }
  else if(clnt->category == QMI_CCI_CONNECTED_CLIENT)
  {
    if(memcmp(addr, clnt->info.client.server_addr, LIST_HEAD(clnt->xport)->addr_len) == 0)
    {
      memset(clnt->info.client.server_addr, 0, MAX_ADDR_LEN);
      clnt->category = QMI_CCI_DORMANT_CLIENT;
      err_cb = clnt->info.client.err_cb;
      cb_data = clnt->info.client.err_cb_data;
      if(!err_cb)
        clnt->info.client.err_pending = 1;
    }
    else
    {
      UNLOCK(&clnt->info_lock);
      return;
    }
  }
  UNLOCK(&clnt->info_lock);

  cleanup_client_lists(clnt, error);

  if(ext_signal)
  {
    QMI_CCI_OS_SIGNAL_SET(ext_signal);
  }

  if(err_cb)
  {
    err_cb(CLIENT_HANDLE(clnt), error, cb_data);
  }

  if(notify_cb)
  {
    notify_cb(CLIENT_HANDLE(clnt), clnt->service_obj,
                            QMI_CLIENT_SERVICE_COUNT_DEC, cb_data);
  }
}

/*===========================================================================
  FUNCTION  qmi_cci_xport_event_remove_server
===========================================================================*/
void qmi_cci_xport_event_remove_server
(
 qmi_cci_client_type *clnt,
 void *addr
 )
{
  qmi_cci_xport_event_server_error(clnt, addr, QMI_SERVICE_ERR);
}

/*=============================================================================
  FUNCTION qmi_cci_xport_resume
=============================================================================*/
void qmi_cci_xport_resume
(
 qmi_cci_client_type *clnt
)
{

  if(!clnt)
    return;

  /* xport already has a ref count, we dont need one */
  qmi_cci_flush_tx_q(clnt);
}

/*===========================================================================
  FUNCTION  qmi_cci_xport_recv
===========================================================================*/
qmi_client_error_type qmi_cci_xport_recv
(
 qmi_cci_client_type *clnt,
 void *addr,
 uint8_t *buf,
 uint32_t len
 )
{
  uint8_t cntl_flag;
  uint16_t txn_id, msg_id, msg_len;
  qmi_cci_txn_type *txn;

  if(!clnt || len < QMI_HEADER_SIZE || !addr)
    return QMI_CLIENT_PARAM_ERR;

  /* decode message header and find the transaction */
  decode_header(buf, &cntl_flag, &txn_id, &msg_id, &msg_len);
  buf += QMI_HEADER_SIZE;
  len -= QMI_HEADER_SIZE;

  len = len < msg_len ? len : msg_len;

  LOCK(&clnt->info_lock);
  if(clnt->category != QMI_CCI_CONNECTED_CLIENT)
  {
    QMI_CCI_LOG_ERR("QCCI qmi_cci_xport_recv: received msg to invalid client svc_id: %d\n",
                     clnt->service_obj->service_id, 0, 0);
    UNLOCK(&clnt->info_lock);
    return QMI_INTERNAL_ERR;
  }
  UNLOCK(&clnt->info_lock);

  if(cntl_flag != QMI_RESPONSE_CONTROL_FLAG &&
      cntl_flag != QMI_INDICATION_CONTROL_FLAG)
  {
    QMI_CCI_LOG_ERR("QCCI qmi_cci_xport_recv: cntl_flag = 0x%x invalid. svc_id: %d cntl_flag: %d",
                    cntl_flag, clnt->service_obj->service_id, cntl_flag);
    return QMI_INTERNAL_ERR;
  }

  qmi_cci_log_rx(clnt, cntl_flag, txn_id, msg_id, buf, len, QMI_NO_ERR);

  /* process indication */
  if(cntl_flag == QMI_INDICATION_CONTROL_FLAG)
  {
    if(clnt->info.client.ind_cb)
    {
      clnt->info.client.ind_cb(CLIENT_HANDLE(clnt), msg_id, len ? buf : NULL, len,
                                clnt->info.client.ind_cb_data);
    }
    return QMI_NO_ERR;
  }

  /* process transaction */
  LOCK(&clnt->info.client.txn_list_lock);
  LIST_FIND(clnt->info.client.txn_list, txn, link, txn->txn_id == txn_id);
  if(txn)
  {
    /* Txn list's reference is transferred to xport_recv */
    LIST_REMOVE(clnt->info.client.txn_list, txn, link);
  }
  UNLOCK(&clnt->info.client.txn_list_lock);

  /* transaction not found */
  if(!txn)
  {
    QMI_CCI_LOG_ERR("QCCI qmi_cci_xport_recv: txn not found. svc_id: %d",
                    clnt->service_obj->service_id, 0, 0);
    return QMI_INTERNAL_ERR;
  }

  /* mismatched msg_id, something went wrong - bail */
  if(txn->msg_id != msg_id)
  {
    handle_txn_error(clnt, txn, QMI_INVALID_TXN);
    QMI_CCI_DEC_TXN_SAFE(clnt, txn);
    return QMI_INTERNAL_ERR;
  }

  LOCK(&txn->lock);
  if(txn->rx_buf)
  {
    if(txn->type & TXN_MSG)
    {
      txn->rc = qmi_idl_message_decode(
                      clnt->service_obj,
                      QMI_IDL_RESPONSE,
                      msg_id,
                      buf,
                      len,
                      txn->rx_buf,
                      txn->rx_buf_len);
      txn->reply_len = txn->rx_buf_len;
    }
    else
    {
      txn->rc = QMI_NO_ERR;
      txn->reply_len = QMI_MEM_COPY(txn->rx_buf, txn->rx_buf_len, buf, len);
    }

    if(txn->type & TXN_SYNC)
    {
      QMI_CCI_OS_SIGNAL_SET(&txn->signal);
    }
    else
    {
      if(txn->rx_cb)
      {
        txn->rx_cb(CLIENT_HANDLE(clnt), msg_id, txn->rx_buf,
                   txn->reply_len, txn->rx_cb_data, txn->rc);
      }
    }
  }

  UNLOCK(&txn->lock);

  QMI_CCI_DEC_TXN_SAFE(clnt, txn);

  return QMI_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_cci_xport_start
===========================================================================*/
void qmi_cci_xport_start
(
 qmi_cci_xport_ops_type *ops,
 void *xport_data
 )
{
  unsigned int i;

  if(!ops || !qcci_fw_inited)
  {
    return;
  }

  LOCK(&xport_tbl_lock);
  for(i = 0; i < num_xports; i++)
  {
    if(xport_tbl[i].ops == ops && xport_tbl[i].xport_data == xport_data)
    {
      UNLOCK(&xport_tbl_lock);
      return;
    }
  }
  if(num_xports < MAX_XPORTS)
  {
    i = 0;
    while(i < num_xports)
    {
      if(xport_tbl[i].uid == next_xport_uid)
      {
        next_xport_uid++;
        i = 0;
      }
      else
      {
        i++;
      }
    }
    xport_tbl[num_xports].ops = ops;
    xport_tbl[num_xports].xport_data = xport_data;
    xport_tbl[num_xports].uid = next_xport_uid++;
    num_xports++;
  }
  UNLOCK(&xport_tbl_lock);
}

/*===========================================================================
  FUNCTION  qmi_cci_xport_stop
===========================================================================*/
void qmi_cci_xport_stop
(
 qmi_cci_xport_ops_type *ops,
 void *xport_data
 )
{
  unsigned int i, j;

  if(!qcci_fw_inited)
    return;

  LOCK(&xport_tbl_lock);
  for(i = 0; i < num_xports; i++)
  {
    if(xport_tbl[i].ops == ops && xport_tbl[i].xport_data == xport_data)
    {
      /* Left shift all the xports */
      for(j = i; j < num_xports - 1; j++)
      {
        xport_tbl[j] = xport_tbl[j + 1];
      }
      num_xports--;
      break;
    }
  }
  UNLOCK(&xport_tbl_lock);
}

/*===========================================================================
                   PUBLIC EXPORTED FUNCTIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  qmi_client_notifier_init
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_notifier_init
(
 qmi_idl_service_object_type               service_obj,
 qmi_client_os_params                      *os_params,
 qmi_client_type                           *user_handle
 )
{
  qmi_cci_client_type *clnt;
  qmi_client_type ret_user_handle;
  uint32_t service_id, idl_version, i;
  int rc;

  if(!user_handle)
    return QMI_CLIENT_PARAM_ERR;

  *user_handle = INVALID_HANDLE;

  if(!service_obj)
    return QMI_CLIENT_PARAM_ERR;

  /* Extract service id */
  rc =  qmi_idl_get_service_id(service_obj, &service_id);
  if (rc !=  QMI_IDL_LIB_NO_ERR)
    return rc;

  /* Get IDL version */
  rc =  qmi_idl_get_idl_version(service_obj, &idl_version);
  if (rc !=  QMI_IDL_LIB_NO_ERR)
    return rc;

  rc = qmi_cci_client_alloc(service_obj, QMI_CCI_NOTIFIER_CLIENT,
                              os_params, NULL, NULL, &clnt);
  if(rc != QMI_NO_ERR)
    return rc;

  ret_user_handle = CLIENT_HANDLE(clnt);
  rc = QMI_CLIENT_TRANSPORT_ERR;

  LOCK(&xport_tbl_lock);
  /* go through all the xports and find the service */
  for(i = 0; i < num_xports; i++)
  {
    struct qmi_cci_xport_s *xport;

    xport = (struct qmi_cci_xport_s *)CALLOC(1, sizeof(*xport));
    if(!xport)
    {
      continue;
    }

    clnt = qmi_cci_get_ref(ret_user_handle, 0);
    if(!clnt)
    {
      FREE(xport);
      return QMI_CLIENT_INVALID_CLNT; /* Should never happen */
    }

    /* opening up the xport */
    LINK_INIT(xport->link);
    LIST_ADD(clnt->xport, xport, link);
    xport->ops = xport_tbl[i].ops;
    xport->addr_len = QCCI_MIN(MAX_ADDR_LEN, xport->ops->addr_len());
    xport->handle = xport->ops->open(xport_tbl[i].xport_data,
                      clnt, service_id, idl_version, NULL, 0);
    /* unable to open xport, go to the next one */
    if(!xport->handle)
    {
      LIST_REMOVE(clnt->xport, xport, link);
      FREE(xport);
      qmi_cci_put_ref(clnt);
      continue;
    }

    /* if server exists, signal notifier */
    if(xport->ops->lookup(xport_tbl[i].xport_data,
                            (uint8_t)xport_tbl[i].uid, service_id,
                            idl_version, NULL, NULL))
    {
      if(os_params)
      {
        QMI_CCI_OS_SIGNAL_SET(os_params);
      }
      clnt->info.notifier.notify_pending = 1;
    }

    rc = QMI_NO_ERR;
  }
  UNLOCK(&xport_tbl_lock);

  if(rc == QMI_NO_ERR)
  {
    *user_handle = ret_user_handle;
  }

  qmi_cci_put_ref(clnt);

  return rc;
}

/*===========================================================================
  FUNCTION  qmi_client_init
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_init
(
 qmi_service_info                          *service_info,
 qmi_idl_service_object_type               service_obj,
 qmi_client_ind_cb                         ind_cb,
 void                                      *ind_cb_data,
 qmi_client_os_params                      *os_params,
 qmi_client_type                           *user_handle
 )
{
  qmi_cci_service_info *svc = (qmi_cci_service_info *)service_info;
  qmi_cci_client_type *clnt;
  struct qmi_cci_xport_s *xport = NULL;
  qmi_client_type ret_user_handle;
  uint32_t service_id, idl_version;
  uint32_t max_msg_len, i;
  int rc;
  unsigned int num_entries = 0, num_services;
  qmi_service_info *service_array = NULL;

  if(!svc || !user_handle)
    return QMI_CLIENT_PARAM_ERR;

  *user_handle = INVALID_HANDLE;

  /* Extract service id */
  rc =  qmi_idl_get_service_id(service_obj, &service_id);
  if (rc !=  QMI_IDL_LIB_NO_ERR)
    return rc;

  /* Get IDL version */
  rc =  qmi_idl_get_idl_version(service_obj, &idl_version);
  if (rc !=  QMI_IDL_LIB_NO_ERR)
    return rc;

  /* Get max message length */
  rc = qmi_idl_get_max_service_len(service_obj, &max_msg_len);
  if(rc != QMI_IDL_LIB_NO_ERR)
    return rc;

  rc = qmi_cci_client_alloc(service_obj, QMI_CCI_CONNECTED_CLIENT, os_params,
                             ind_cb, ind_cb_data, &clnt);
  if(rc != QMI_NO_ERR)
    return rc;

  ret_user_handle = CLIENT_HANDLE(clnt);

  /* Lookup and get a handle from the xport */
  LOCK(&xport_tbl_lock);
  rc = QMI_CLIENT_PARAM_ERR;
  for(i = 0; i < num_xports; i++)
  {
    if(xport_tbl[i].uid == svc->xport)
    {
      xport = (struct qmi_cci_xport_s *)CALLOC(1, sizeof(*xport));
      if(!xport)
      {
        rc = QMI_CLIENT_ALLOC_FAILURE;
        break;
      }
      xport->ops = xport_tbl[i].ops;
      xport->addr_len = QCCI_MIN(MAX_ADDR_LEN, xport->ops->addr_len());
      xport->handle = NULL;
      LINK_INIT(xport->link);
      clnt = qmi_cci_get_ref(ret_user_handle, 0);
      if(!clnt)
      {
        rc = QMI_CLIENT_INVALID_CLNT;
        FREE(xport);
        break;
      }
      LIST_ADD(clnt->xport, xport, link);
      xport->handle = xport->ops->open(xport_tbl[i].xport_data, clnt, service_id,
                                       idl_version, clnt->info.client.server_addr,
                                       max_msg_len);
      if(!xport->handle)
      {
        /* Release the xport reference */
        LIST_REMOVE(clnt->xport, xport, link);
        qmi_cci_put_ref(clnt);
        FREE(xport);
        rc = QMI_CLIENT_TRANSPORT_ERR;
        break;
      }
      rc = QMI_NO_ERR;
      break;
    }
  }
  UNLOCK(&xport_tbl_lock);
  if(rc != QMI_NO_ERR)
    goto release_init_bail;

  /* fill in server address */
  QMI_MEM_COPY(clnt->info.client.server_addr, MAX_ADDR_LEN, svc->addr, xport->addr_len);

  /* redo lookup to make sure the server actually exits */
  while(1)
  {
    rc = qmi_client_get_service_list(service_obj, service_array, &num_entries,
        &num_services);

    if(rc != QMI_NO_ERR)
    {
      if(service_array)
      {
        FREE(service_array);
        service_array = NULL;
      }
      break;
    }

    if(num_entries == num_services)
      break;

    if(service_array)
      FREE(service_array);

    service_array = (qmi_service_info *)MALLOC(sizeof(qmi_service_info) * num_services);
    if(!service_array)
    {
      rc = QMI_CLIENT_ALLOC_FAILURE;
      goto init_close_bail;
    }

    num_entries = num_services;
  }

  rc = QMI_SERVICE_ERR;
  if(service_array)
  {
    for(i = 0; i < num_entries; i++)
    {
      qmi_cci_service_info *s = (qmi_cci_service_info *)&service_array[i];
      if(!memcmp(s->addr, svc->addr, xport->addr_len) && (s->xport == svc->xport))
      {
        *user_handle = ret_user_handle;
        rc = QMI_NO_ERR;
        break;
      }
    }
    FREE(service_array);
  }

init_close_bail:
  if(rc != QMI_NO_ERR)
    xport->ops->close(xport->handle);
release_init_bail:
  qmi_cci_put_ref(clnt);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_client_send_raw_msg_async
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_send_raw_msg_async
(
 qmi_client_type                   user_handle,
 unsigned int                      msg_id,
 void                              *req_buf,
 unsigned int                      req_buf_len,
 void                              *resp_buf,
 unsigned int                      resp_buf_len,
 qmi_client_recv_raw_msg_async_cb  resp_cb,
 void                              *resp_cb_data,
 qmi_txn_handle                    *txn_handle
 )
{
  qmi_cci_client_type *clnt;
  qmi_cci_txn_type *txn = NULL;
  unsigned char *buf;
  int rc;

  if(!resp_buf)
    return QMI_CLIENT_PARAM_ERR;

  if(req_buf_len > 0 && req_buf == NULL)
    return QMI_CLIENT_PARAM_ERR;

  if(txn_handle)
    *txn_handle = (qmi_txn_handle)NULL;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  if(!CLIENT_XPORT_VALID(clnt))
  {
    rc = QMI_CLIENT_INVALID_CLNT;
    goto bail;
  }

  /* allocate output buffer */
  buf = (unsigned char *)MALLOC(req_buf_len + QMI_HEADER_SIZE);
  if(!buf)
  {
    rc = QMI_CLIENT_ALLOC_FAILURE;
    goto bail;
  }

  rc = qmi_client_get_txn(clnt, TXN_ASYNC_RAW, msg_id, resp_buf, resp_buf_len,
                            resp_cb, resp_cb_data, &txn);

  if(rc != QMI_NO_ERR)
  {
    FREE(buf);
    goto bail;
  }

  /* encode header and copy payload */
  encode_header(buf, QMI_REQUEST_CONTROL_FLAG, (uint16_t)txn->txn_id,
      (uint16_t)msg_id, (uint16_t)req_buf_len);

  if(req_buf_len > 0)
  {
    QMI_MEM_COPY(buf + QMI_HEADER_SIZE, req_buf_len, req_buf, req_buf_len);
  }

  /* send message and return */
  rc = qmi_cci_send(clnt, txn, buf, req_buf_len + QMI_HEADER_SIZE);

  if(rc != QMI_NO_ERR)
  {
    FREE(buf);
    if(remove_txn(clnt, txn) != QMI_NO_ERR)
    {
      /* The transaction is already removed meaning
       * the resp cb has already been called */
      rc = QMI_NO_ERR;
    }
  }

  if(rc == QMI_NO_ERR && txn_handle)
  {
    *txn_handle = (qmi_txn_handle)txn;
  }

  QMI_CCI_DEC_TXN_SAFE(clnt, txn);

bail:
  qmi_cci_put_ref(clnt);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_client_send_msg_async
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_send_msg_async
(
 qmi_client_type                 user_handle,
 unsigned int                    msg_id,
 void                            *req_c_struct,
 unsigned int                    req_c_struct_len,
 void                            *resp_c_struct,
 unsigned int                    resp_c_struct_len,
 qmi_client_recv_msg_async_cb    resp_cb,
 void                            *resp_cb_data,
 qmi_txn_handle                  *txn_handle
 )
{
  qmi_cci_client_type *clnt;
  qmi_cci_txn_type *txn = NULL;
  int rc;

  if(!resp_c_struct)
    return QMI_CLIENT_PARAM_ERR;

  if(req_c_struct_len > 0 && req_c_struct == NULL)
    return QMI_CLIENT_PARAM_ERR;

  if(txn_handle)
    *txn_handle = (qmi_txn_handle)NULL;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  if(!CLIENT_XPORT_VALID(clnt))
  {
    rc = QMI_CLIENT_INVALID_CLNT;
    goto bail;
  }

  rc = qmi_client_get_txn(clnt, TXN_ASYNC_MSG, msg_id, resp_c_struct,
                            resp_c_struct_len, resp_cb, resp_cb_data, &txn);

  if(rc != QMI_NO_ERR)
  {
    goto bail;
  }

  /* encode message */
  rc = encode_and_send(clnt, txn, req_c_struct, req_c_struct_len);

  if(rc != QMI_NO_ERR)
  {
    if(remove_txn(clnt, txn) != QMI_NO_ERR)
    {
      /* The transaction is already removed meaning
       * the resp cb has already been called */
      rc = QMI_NO_ERR;
    }
  }

  if(rc == QMI_NO_ERR && txn_handle)
  {
    *txn_handle = (qmi_txn_handle)txn;
  }

  QMI_CCI_DEC_TXN_SAFE(clnt, txn);

bail:
  qmi_cci_put_ref(clnt);
  return rc;
}


/*===========================================================================
  FUNCTION  qmi_client_delete_async_txn
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_delete_async_txn
(
  qmi_client_type  user_handle,
  qmi_txn_handle   async_txn_handle
)
{
  qmi_cci_client_type *clnt;
  qmi_cci_txn_type *txn;
  qmi_cci_txn_type *find_txn = (qmi_cci_txn_type *)async_txn_handle;
  int rc = QMI_INVALID_TXN;

  if(!async_txn_handle)
    return QMI_CLIENT_PARAM_ERR;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  LOCK(&clnt->info_lock);
  if(clnt->category == QMI_CCI_NOTIFIER_CLIENT)
  {
    UNLOCK(&clnt->info_lock);
    rc = QMI_CLIENT_INVALID_CLNT;
    goto bail;
  }
  UNLOCK(&clnt->info_lock);

  /* Find and remove txn from tx queue first */
  LOCK(&clnt->info.client.tx_q_lock);
  LIST_FIND(clnt->info.client.tx_q, txn, tx_link, txn == find_txn);
  if(txn)
  {
    INVALIDATE_TXN_TX_BUF(txn);
    LIST_REMOVE(clnt->info.client.tx_q, txn, tx_link);
    UNLOCK(&clnt->info.client.tx_q_lock);
    /* Release the tx_q ref count */
    QMI_CCI_DEC_TXN_SAFE(clnt, txn);
  }
  else
  {
    UNLOCK(&clnt->info.client.tx_q_lock);
  }

  /* lock list and look for txn */
  LOCK(&clnt->info.client.txn_list_lock);
  LIST_FIND(clnt->info.client.txn_list, txn, link, txn == find_txn);
  if(txn)
  {
    LIST_REMOVE(clnt->info.client.txn_list, txn, link);
  }
  UNLOCK(&clnt->info.client.txn_list_lock);

  if(txn)
  {
    INVALIDATE_TXN_RX_BUF(txn);
    QMI_CCI_DEC_TXN_SAFE(clnt, txn);
    rc = QMI_NO_ERR;
  }
bail:
  qmi_cci_put_ref(clnt);
  return rc;
}


/*===========================================================================
  FUNCTION  qmi_client_send_raw_msg_sync
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_send_raw_msg_sync
(
 qmi_client_type           user_handle,
 unsigned int              msg_id,
 void                      *req_buf,
 unsigned int              req_buf_len,
 void                      *resp_buf,
 unsigned int              resp_buf_len,
 unsigned int              *resp_buf_recv_len,
 unsigned int              timeout_msecs
 )
{
  unsigned char *buf;
  qmi_cci_client_type *clnt;
  qmi_cci_txn_type *txn = NULL;
  int rc;

  if(!resp_buf || !resp_buf_recv_len)
    return QMI_CLIENT_PARAM_ERR;

  if(req_buf_len > 0 && req_buf == NULL)
    return QMI_CLIENT_PARAM_ERR;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  if(!CLIENT_XPORT_VALID(clnt))
  {
    rc = QMI_CLIENT_INVALID_CLNT;
    goto bail;
  }

  /* allocate output buffer */
  buf = (unsigned char *)MALLOC(req_buf_len + QMI_HEADER_SIZE);
  if(!buf)
  {
    rc = QMI_CLIENT_ALLOC_FAILURE;
    goto bail;
  }

  rc = qmi_client_get_txn(clnt, TXN_SYNC_RAW, msg_id, resp_buf, resp_buf_len,
                            NULL, NULL, &txn);

  if(rc != QMI_NO_ERR)
  {
    FREE(buf);
    goto bail;
  }

  /* encode header and copy payload */
  encode_header(buf, QMI_REQUEST_CONTROL_FLAG, (uint16_t)txn->txn_id,
      (uint16_t)msg_id, (uint16_t)req_buf_len);

  if(req_buf_len > 0)
  {
    QMI_MEM_COPY(buf + QMI_HEADER_SIZE, req_buf_len, req_buf, req_buf_len);
  }

  /* clear signal */
  QMI_CCI_OS_SIGNAL_CLEAR(&txn->signal);
  txn->rc = QMI_TIMEOUT_ERR;

  /* Start the timer */
  QMI_CCI_OS_SIGNAL_START_TIMER(&txn->signal, timeout_msecs);

  /* send message */
  rc = qmi_cci_send(clnt, txn, buf, req_buf_len + QMI_HEADER_SIZE);

  if(rc == QMI_NO_ERR)
  {
    rc = qmi_cci_response_wait_loop(txn, timeout_msecs);
    if(rc == QMI_NO_ERR)
    {
      *resp_buf_recv_len = txn->reply_len;
    }
    else if(rc == QMI_TIMEOUT_ERR)
    {
      qmi_cci_txn_type *iter_txn = NULL;

      /* Remove txn if it still present in the tx_q */
      LOCK(&clnt->info.client.tx_q_lock);
      
      LIST_FIND(clnt->info.client.tx_q, iter_txn, tx_link, iter_txn == txn);
      
      if(iter_txn)
      {
        LIST_REMOVE(clnt->info.client.tx_q, iter_txn, tx_link);
      }
      
      UNLOCK(&clnt->info.client.tx_q_lock);
      
      if(iter_txn)
      {
        /* Release tx_q ref count */
        QMI_CCI_DEC_TXN_SAFE(clnt, txn);
      }
    }
  }
  else
  {
    FREE(buf);
  }

  INVALIDATE_TXN_RX_BUF(txn);
  (void)remove_txn(clnt, txn);
  QMI_CCI_DEC_TXN_SAFE(clnt, txn);

bail:
  qmi_cci_put_ref(clnt);

  return rc;
}

/*===========================================================================
  FUNCTION  qmi_client_send_msg_sync
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_send_msg_sync
(
 qmi_client_type    user_handle,
 unsigned int       msg_id,
 void               *req_c_struct,
 unsigned int       req_c_struct_len,
 void               *resp_c_struct,
 unsigned int       resp_c_struct_len,
 unsigned int       timeout_msecs
 )
{
  qmi_cci_client_type *clnt;
  qmi_cci_txn_type *txn = NULL;
  int rc;

  if(!resp_c_struct)
    return QMI_CLIENT_PARAM_ERR;

  if(req_c_struct_len > 0 && req_c_struct == 0)
    return QMI_CLIENT_PARAM_ERR;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  if(!CLIENT_XPORT_VALID(clnt))
  {
    rc = QMI_CLIENT_INVALID_CLNT;
    goto bail;
  }

  rc = qmi_client_get_txn(clnt, TXN_SYNC_MSG, msg_id, resp_c_struct,
                            resp_c_struct_len, NULL, NULL, &txn);

  if(rc != QMI_NO_ERR)
  {
    goto bail;
  }

  /* clear signal */
  QMI_CCI_OS_SIGNAL_CLEAR(&txn->signal);
  txn->rc = QMI_TIMEOUT_ERR;

  /* start the timer */
  QMI_CCI_OS_SIGNAL_START_TIMER(&txn->signal, timeout_msecs);

  /* encode message */
  rc = encode_and_send(clnt, txn, req_c_struct, req_c_struct_len);

  if(rc == QMI_NO_ERR)
  {
    rc = qmi_cci_response_wait_loop(txn, timeout_msecs);

    if(rc == QMI_TIMEOUT_ERR)
    {
      qmi_cci_txn_type *iter_txn = NULL;

      /* Remove txn if it still present in the tx_q */
      LOCK(&clnt->info.client.tx_q_lock);
      
      LIST_FIND(clnt->info.client.tx_q, iter_txn, tx_link, iter_txn == txn);
      
      if(iter_txn)
      {
        LIST_REMOVE(clnt->info.client.tx_q, iter_txn, tx_link);
      }
      
      UNLOCK(&clnt->info.client.tx_q_lock);
      
      if(iter_txn)
      {
        /* Release tx_q ref count */
        QMI_CCI_DEC_TXN_SAFE(clnt, txn);
      }
    }
  }

  INVALIDATE_TXN_RX_BUF(txn);
  (void)remove_txn(clnt, txn);
  QMI_CCI_DEC_TXN_SAFE(clnt, txn);

bail:
  qmi_cci_put_ref(clnt);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_client_release_async
===========================================================================*/
QMI_EXPORT qmi_client_error_type qmi_client_release_async
(
  qmi_client_type user_handle,
  qmi_client_release_cb release_cb,
  void *release_cb_data
)
{
  qmi_cci_client_type *clnt;
  struct qmi_cci_xport_s *xport;

  clnt = qmi_cci_get_ref(user_handle, 1);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  if(clnt->category != QMI_CCI_NOTIFIER_CLIENT)
  {
    LOCK(&clnt->info.client.tx_q_lock);
    /* From now on all calls to qmi_cci_send will fail */
    clnt->info.client.accepting_txns = 0;
    UNLOCK(&clnt->info.client.tx_q_lock);
  }

  cleanup_client_lists(clnt, QMI_INTERNAL_ERR);

  clnt->release_cb = release_cb;
  clnt->release_cb_data = release_cb_data;

  /* close each of the opened xports allowing each to free the
   * reference allocated to it. */
  for(xport = LIST_HEAD(clnt->xport); xport; xport = xport->link.next)
  {
    void *handle = xport->handle;
    /* From now, all down calls (initiated before release()) will fail */
    xport->handle = NULL;
    xport->ops->close(handle);
  }

  qmi_cci_put_ref(clnt);

  return QMI_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_client_release
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_release
(
 qmi_client_type     user_handle
 )
{
  qmi_cci_client_type *clnt;
  qmi_client_error_type rc;
  struct qmi_cci_waiter_s waiter;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  waiter.rc = QMI_TIMEOUT_ERR;
  QMI_CCI_OS_SIGNAL_INIT_SELF(&waiter.signal, &clnt->signal);
#ifdef QMI_CCI_OS_SIGNAL_VALID
  if(!QMI_CCI_OS_SIGNAL_VALID(&waiter.signal))
  {
    qmi_cci_put_ref(clnt);
    return QMI_CLIENT_INVALID_SIG;
  }
#endif
  QMI_CCI_OS_SIGNAL_CLEAR(&waiter.signal);

  /* Release this call's reference */
  qmi_cci_put_ref(clnt);

  rc = qmi_client_release_async(user_handle, release_cb_internal, (void *)&waiter);

  if(rc == QMI_NO_ERR)
  {
    while(waiter.rc == QMI_TIMEOUT_ERR)
    {
      QMI_CCI_OS_SIGNAL_WAIT(&waiter.signal, 0);
      QMI_CCI_OS_SIGNAL_CLEAR(&waiter.signal);
    }
  }
  QMI_CCI_OS_SIGNAL_DEINIT(&waiter.signal);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_client_message_encode
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_message_encode
(
 qmi_client_type                      user_handle,
 qmi_idl_type_of_message_type         req_resp_ind,
 unsigned int                         message_id,
 const void                           *p_src,
 unsigned int                         src_len,
 void                                 *p_dst,
 unsigned int                         dst_len,
 unsigned int                         *dst_encoded_len
 )
{
  qmi_cci_client_type *clnt;
  qmi_idl_service_object_type service_obj;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  service_obj = clnt->service_obj;
  qmi_cci_put_ref(clnt);

  return (
      qmi_idl_message_encode(
        service_obj,
        req_resp_ind,
        (uint16_t)message_id,
        p_src,
        src_len,
        p_dst,
        dst_len,
        (uint32_t*)dst_encoded_len)
      );
}

/*===========================================================================
  FUNCTION  qmi_client_message_decode
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_message_decode
(
 qmi_client_type                         user_handle,
 qmi_idl_type_of_message_type            req_resp_ind,
 unsigned int                            message_id,
 const void                              *p_src,
 unsigned int                            src_len,
 void                                    *p_dst,
 unsigned int                            dst_len
 )
{
  qmi_cci_client_type *clnt;
  qmi_idl_service_object_type service_obj;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  service_obj = clnt->service_obj;
  qmi_cci_put_ref(clnt);

  return (
      qmi_idl_message_decode(
        service_obj,
        req_resp_ind,
        (uint16_t)message_id,
        p_src,
        src_len,
        p_dst,
        dst_len)
      );
}

/*===========================================================================
  FUNCTION  qmi_client_get_service_list
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_get_service_list
(
 qmi_idl_service_object_type service_obj,
 qmi_service_info *service_info_array,
 unsigned int *num_entries,
 unsigned int *num_services
 )
{
  qmi_cci_service_info *svc = (qmi_cci_service_info *)service_info_array;
  unsigned int i, to_fill = 0, filled = 0;
  uint32_t service_id, idl_version;
  int rc;

  if(!num_services)
    return QMI_CLIENT_PARAM_ERR;

  *num_services = 0;

  if(num_entries && *num_entries && svc)
  {
    to_fill = *num_entries;
    *num_entries = 0;
  }

  /* Extract service id */
  rc =  qmi_idl_get_service_id(service_obj, &service_id);
  if (rc !=  QMI_IDL_LIB_NO_ERR)
    return rc;

  /* Get IDL version */
  rc =  qmi_idl_get_idl_version(service_obj, &idl_version);
  if (rc !=  QMI_IDL_LIB_NO_ERR)
    return rc;

  /* go through all the xports and find the service */
  LOCK(&xport_tbl_lock);
  for(i = 0; i < num_xports; i++)
  {
    /* look up server address */
    uint32_t entries = to_fill - filled;
    unsigned int service_xport;
    (*num_services) += service_xport = xport_tbl[i].ops->lookup(
        xport_tbl[i].xport_data,
        xport_tbl[i].uid,
        service_id,
        idl_version,
        entries ? &entries : NULL,
        entries ? &svc[filled] : NULL);
    /* only increment if lookup returns non-zero */
    filled += service_xport ? entries : 0;
  }
  UNLOCK(&xport_tbl_lock);

  if(num_entries)
    (*num_entries) = filled;

  return *num_services ? QMI_NO_ERR : QMI_SERVICE_ERR;
}

#ifndef QMI_CLIENT_INSTANCE_ANY
#define QMI_CLIENT_INSTANCE_ANY 0xffff

/*===========================================================================
  FUNCTION  qmi_client_get_any_service
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_get_any_service
(
 qmi_idl_service_object_type service_obj,
 qmi_service_info *service_info
 )
{
  return qmi_client_get_service_instance(service_obj, QMI_CLIENT_INSTANCE_ANY, service_info);
}
#endif

/*===========================================================================
  FUNCTION  qmi_client_get_service_instance
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_get_service_instance
(
 qmi_idl_service_object_type service_obj,
 qmi_service_instance instance_id,
 qmi_service_info *service_info
 )
{
  unsigned int num_entries = 1, num_services, i;
  qmi_client_error_type rc;
  qmi_service_info *service_array;

  if(!service_info)
    return QMI_CLIENT_PARAM_ERR;

  if(instance_id == QMI_CLIENT_INSTANCE_ANY)
  {
    return qmi_client_get_service_list(service_obj, service_info, &num_entries, &num_services);
  }

  rc = qmi_client_get_service_list(service_obj, NULL, NULL, &num_services);
  if(rc != QMI_NO_ERR)
    return rc;

  service_array = (qmi_service_info *)MALLOC(sizeof(qmi_service_info) * num_services);
  if(!service_array)
    return QMI_CLIENT_ALLOC_FAILURE;

  num_entries = num_services;
  rc = qmi_client_get_service_list(service_obj, service_array, &num_entries,
      &num_services);
  if(rc != QMI_NO_ERR)
    goto free_bail;

  rc = QMI_SERVICE_ERR;
  for(i = 0; i < num_entries; i++)
  {
    qmi_cci_service_info *svc = (qmi_cci_service_info *)&service_array[i];
    if(svc->instance == instance_id)
    {
      QMI_MEM_COPY(service_info, sizeof(qmi_service_info), svc, sizeof(qmi_service_info));
      rc = QMI_NO_ERR;
      break;
    }
  }

free_bail:
  FREE(service_array);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_client_get_instance_id
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_get_instance_id
(
 qmi_service_info *service_info,
 qmi_service_instance *instance_id
 )
{
  qmi_cci_service_info *svc = (qmi_cci_service_info *)service_info;

  if(!svc || !instance_id)
    return QMI_CLIENT_PARAM_ERR;

  *instance_id = svc->instance;
  return QMI_NO_ERR;
}

/*===========================================================================
  FUNCTION  qmi_client_register_log_cb
===========================================================================*/
QMI_EXPORT qmi_client_error_type qmi_client_register_log_cb
(
  qmi_client_type user_handle,
  qmi_client_log_cb log_cb,
  void              *log_cb_data
)
{
  qmi_cci_client_type *clnt;
  int rc = QMI_CLIENT_INVALID_CLNT;

  if(!log_cb)
    return QMI_CLIENT_PARAM_ERR;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  LOCK(&clnt->info_lock);
  if(clnt->category == QMI_CCI_CONNECTED_CLIENT)
  {
    clnt->info.client.log_cb_data = log_cb_data;
    clnt->info.client.log_cb = log_cb;
    rc = QMI_NO_ERR;
  }
  UNLOCK(&clnt->info_lock);
  qmi_cci_put_ref(clnt);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_client_register_error_cb
===========================================================================*/
QMI_EXPORT qmi_client_error_type qmi_client_register_error_cb
(
 qmi_client_type user_handle,
 qmi_client_error_cb err_cb,
 void *err_cb_data
 )
{
  qmi_cci_client_type *clnt;
  unsigned int err_pending = 0;
  int rc = QMI_NO_ERR;

  if(!err_cb)
    return QMI_CLIENT_PARAM_ERR;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  /* Do not allow error callback registration
   * on the notifier as it is not connected to any
   * physical service */
  LOCK(&clnt->info_lock);
  if(clnt->category == QMI_CCI_NOTIFIER_CLIENT)
  {
    rc = QMI_CLIENT_INVALID_CLNT;
  }
  else
  {
    clnt->info.client.err_cb = err_cb;
    clnt->info.client.err_cb_data = err_cb_data;
    err_pending = clnt->info.client.err_pending;
  }
  UNLOCK(&clnt->info_lock);

  if(err_pending)
  {
    err_cb(CLIENT_HANDLE(clnt), QMI_SERVICE_ERR, err_cb_data);
    rc = QMI_SERVICE_ERR;
  }
  qmi_cci_put_ref(clnt);
  return rc;
}

/*===========================================================================
  FUNCTION  qmi_client_register_notify_cb
===========================================================================*/
QMI_EXPORT qmi_client_error_type qmi_client_register_notify_cb
(
 qmi_client_type user_handle,
 qmi_client_notify_cb notify_cb,
 void *notify_cb_data
)
{
  qmi_cci_client_type *clnt;
  unsigned int notify_pending = 0;
  int rc = QMI_NO_ERR;

  if(!notify_cb)
    return QMI_CLIENT_PARAM_ERR;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  LOCK(&clnt->info_lock);
  if(clnt->category != QMI_CCI_NOTIFIER_CLIENT)
  {
    /* Do not allow notify callback registration
     * on a connected or dormant client */
    rc = QMI_CLIENT_INVALID_CLNT;
  }
  else
  {
    clnt->info.notifier.notify_cb = notify_cb;
    clnt->info.notifier.notify_cb_data = notify_cb_data;
    notify_pending = clnt->info.notifier.notify_pending;
  }
  UNLOCK(&clnt->info_lock);

  if(notify_pending)
  {
    notify_cb(CLIENT_HANDLE(clnt), clnt->service_obj,
                QMI_CLIENT_SERVICE_COUNT_INC, notify_cb_data);
  }
  qmi_cci_put_ref(clnt);
  return rc;
}


/*==========================================================================
  FUNCTION  qmi_client_get_async_txn_id
===========================================================================*/
/*!
@brief

  Gets a transaction id from the transaction handle

@return
  qmi_client_error_type

@note
  This API is added to support the legacy messages that needs access to the
  transcation ID.
*/
/*=========================================================================*/

QMI_EXPORT qmi_client_error_type
qmi_client_get_async_txn_id
(
 qmi_client_type  user_handle,
 qmi_txn_handle   async_txn_handle,
 uint32_t *txn_id
)
{
  qmi_cci_txn_type *i;
  qmi_cci_client_type *clnt;
  qmi_cci_txn_type *txn = (qmi_cci_txn_type *)async_txn_handle;
  qmi_client_error_type rc = QMI_INVALID_TXN;

  if (!txn_id || !txn )
    return QMI_CLIENT_PARAM_ERR;

  clnt = qmi_cci_get_ref(user_handle, 0);
  if(!clnt)
    return QMI_CLIENT_INVALID_CLNT;

  *txn_id = 0;

  LOCK(&clnt->info_lock);
  if(clnt->category == QMI_CCI_NOTIFIER_CLIENT)
  {
    rc = QMI_CLIENT_INVALID_CLNT;
    UNLOCK(&clnt->info_lock);
    goto bail;
  }
  UNLOCK(&clnt->info_lock);

  LOCK(&clnt->info.client.txn_list_lock);
  LIST_FIND(clnt->info.client.txn_list, i, link, i == txn);
  if(i)
  {
    *txn_id = i->txn_id;
    rc = QMI_NO_ERR;
  }
  UNLOCK(&clnt->info.client.txn_list_lock);

bail:
  qmi_cci_put_ref(clnt);

  return rc;
}

/*===========================================================================
  FUNCTION  qmi_client_init_instance
===========================================================================*/
QMI_EXPORT qmi_client_error_type
qmi_client_init_instance
(
 qmi_idl_service_object_type service_obj,
 qmi_service_instance        instance_id,
 qmi_client_ind_cb           ind_cb,
 void                        *ind_cb_data,
 qmi_client_os_params        *os_params,
 uint32_t                    timeout,
 qmi_client_type             *user_handle
)
{
  int rc;
  qmi_client_type notifier;
  qmi_service_info info;
  qmi_client_os_params notifier_os_params;

  if(!user_handle || !service_obj)
  {
    return QMI_CLIENT_PARAM_ERR;
  }

  *user_handle = INVALID_HANDLE;

  /* The common case when we do not have to wait for the service,
   * avoid creation of the notifier */
  rc = qmi_client_get_service_instance(service_obj, instance_id, &info);
  if(rc == QMI_NO_ERR)
  {
    rc = qmi_client_init(&info, service_obj, ind_cb, ind_cb_data, os_params, user_handle);
    if(rc == QMI_NO_ERR || rc != QMI_SERVICE_ERR)
    {
      return rc;
    }
  }

  memset(&notifier_os_params, 0, sizeof(notifier_os_params));
  QMI_CCI_COPY_OS_PARAMS(&notifier_os_params, os_params);

  rc = qmi_client_notifier_init(service_obj, &notifier_os_params, &notifier);
  if(rc != QMI_NO_ERR)
  {
    return rc;
  }
  // Start the notifier timeout
  QMI_CCI_OS_SIGNAL_START_TIMER(&notifier_os_params, timeout);
  while(1)
  {
    QMI_CCI_OS_SIGNAL_CLEAR(&notifier_os_params);
    rc = qmi_client_get_service_instance(service_obj, instance_id, &info);
    if(rc == QMI_NO_ERR)
    {
      rc = qmi_client_init(&info, service_obj, ind_cb, ind_cb_data, os_params, user_handle);
      if(rc == QMI_NO_ERR || rc != QMI_SERVICE_ERR)
      {
        /* Success or a generic error occured */
        break;
      }
    }
    QMI_CCI_OS_SIGNAL_WAIT(&notifier_os_params, timeout);
    if(QMI_CCI_OS_SIGNAL_TIMED_OUT(&notifier_os_params))
    {
      rc = QMI_TIMEOUT_ERR;
      break;
    }
  }
  qmi_client_release(notifier);
  return rc;
}
