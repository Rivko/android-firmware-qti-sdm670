#ifndef QMI_CCI_COMMON_H
#define QMI_CCI_COMMON_H
/******************************************************************************
  @file    qmi_cci_common.h
  @brief   The QMI Common Client Interface (CCI) common header file

  DESCRIPTION
  QMI Common Client common defines
   
  ---------------------------------------------------------------------------
  Copyright (c) 2010-2013 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "qmi_common.h"
#include "qmi_client.h"

/*=============================================================================
     Type definitions
=============================================================================*/

struct qmi_cci_client_s;
typedef struct qmi_cci_client_s qmi_cci_client_type;

struct qmi_cci_xport_s
{
  LINK(struct qmi_cci_xport_s, link);
  struct qmi_cci_xport_ops_s *ops;
  void *handle;
  uint32_t addr_len;
};

typedef enum
{
  QMI_CCI_NOTIFIER_CLIENT = 0,
  QMI_CCI_DORMANT_CLIENT,
  QMI_CCI_CONNECTED_CLIENT
} qmi_cci_client_category_type;

struct qmi_cci_txn_s;
struct qmi_cci_xport_ops_s;
struct qmi_cci_client_s
{
  /* Private infrastructure information */
  struct {
    /* Client ID */
    unsigned int        clid;
    /* Client structure reference counting */
    int                 ref_count;
    
    /* Link required as part of the global client table */
    LINK(qmi_cci_client_type, link);
  } priv;

  /* service object */
  qmi_idl_service_object_type service_obj;

  /* xport data */
  LIST(struct qmi_cci_xport_s, xport);

  /* release callback */
  qmi_client_release_cb release_cb;
  void *release_cb_data;

  /* error callback */
  qmi_cci_client_category_type category;
  union
  {
    struct
    {
      qmi_client_error_cb err_cb;
      void *err_cb_data;
      uint32_t err_pending;

      qmi_client_log_cb log_cb;
      void *log_cb_data;

      /* indication callback */
      qmi_client_ind_cb ind_cb;
      void *ind_cb_data;

      /* server address */
      uint8_t server_addr[MAX_ADDR_LEN];

      /* list of outstanding transactions */
      LIST(struct qmi_cci_txn_s, txn_list);
      uint16_t next_txn_id;

      LIST(struct qmi_cci_txn_s, tx_q);
      int accepting_txns;

      /* Lock to protect txn_list */
      qmi_cci_lock_type txn_list_lock;
      /* Lock to protect tx_q */
      qmi_cci_lock_type tx_q_lock;

    } client;

    struct
    {
      qmi_client_notify_cb notify_cb;
      void *notify_cb_data;
      uint32_t notify_pending;

      /* pointer to external signal, if provided */
      QMI_CCI_OS_SIGNAL *ext_signal;

    } notifier;
  } info;

  qmi_cci_lock_type            info_lock;

  /* OS-defined data such as signal/event */
  QMI_CCI_OS_SIGNAL signal;
};

typedef enum
{
  TXN_INVALID   = 0x0,
  TXN_SYNC      = 0x1,
  TXN_ASYNC     = 0x2,
  TXN_MSG       = 0x4,
  TXN_RAW       = 0x8,
  TXN_SYNC_MSG  = TXN_SYNC  | TXN_MSG,
  TXN_SYNC_RAW  = TXN_SYNC  | TXN_RAW,
  TXN_ASYNC_MSG = TXN_ASYNC | TXN_MSG,
  TXN_ASYNC_RAW = TXN_ASYNC | TXN_RAW
} qmi_cci_txn_enum_type;

typedef struct qmi_cci_txn_s
{
  /* links to prev and next txns */
  LINK(struct qmi_cci_txn_s, link);

  /* TX Queue list */
  LINK(struct qmi_cci_txn_s, tx_link);

  /* type of txn */
  qmi_cci_txn_enum_type type;

  /* txn and msg ids */
  uint32_t txn_id;
  uint32_t msg_id;

  qmi_client_recv_msg_async_cb rx_cb;
  void                         *rx_cb_data;

  uint8_t *rx_buf;
  uint32_t rx_buf_len;
  uint32_t reply_len;

  /* return code */
  int32_t rc;

  int ref_count;

  qmi_cci_client_type *client;

  void *tx_buf;
  uint32_t tx_buf_len;

  /* Lock to protect ref count of the txn */
  qmi_cci_lock_type lock;

  /* Transaction signal */
  QMI_CCI_OS_SIGNAL signal;
} qmi_cci_txn_type;

typedef struct
{
  uint8_t xport;
  uint8_t version;
  uint8_t instance;
  uint8_t reserved;
  uint8_t addr[MAX_ADDR_LEN];
} qmi_cci_service_info;


/*=============================================================================
    Initialization routines
=============================================================================*/

/*===========================================================================
  FUNCTION  qmi_cci_common_init
===========================================================================*/
/*!
@brief

  One time initialization of the QCCI stack.

@return
  None  

@note
  This function is NOT thread safe.
  The only safe place to call this is during init.
*/
/*=========================================================================*/
void qmi_cci_common_init(void);

/*===========================================================================
  FUNCTION  qmi_cci_common_deinit
===========================================================================*/
/*!
@brief

  de-initialization of the QCCI stack.

@return
  None  

@note
  This function is NOT thread safe. 
  The only safe place to call this is during library deinit.
*/
/*=========================================================================*/
void qmi_cci_common_deinit(void);

/*=============================================================================
     Transport abstraction prototypes - infrastructure to xport down-calls
=============================================================================*/

/*=============================================================================
  CALLBACK FUNCTION qmi_cci_open_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure to open a new
  transport 

@param[in]   xport_data         Opaque parameter to the xport (e.g. port ID)
@param[in]   clnt               Pointer to infrastructure's client struct.
                                Can be treated as opaque, but prototyped for
                                ease of debugging.
@param[in]   service_id         Service ID
@param[in]   version            Version of the service
@param[in]   addr               Address of the server
@param[in]   max_rx_len         Maximum length of messages that can be
                                received. Used by xport to allocate a buffer
                                if the underlyin transport cannot pass the
                                message through a callback.  

@retval      Opaque handle to the transport. NULL on failure.

*/
/*=========================================================================*/
typedef void *(*qmi_cci_open_fn_type)
  (
   void *xport_data,
   qmi_cci_client_type *clnt,
   uint32_t service_id,
   uint32_t version,
   void *addr,
   uint32_t max_rx_len
  );

/*=============================================================================
  CALLBACK FUNCTION qmi_cci_send_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure to send data to
  a server.

@param[in]   handle             Opaque handle returned by the open call
@param[in]   addr               Opaque address sent to the infrastructure
                                through the connect or recv calls.
@param[in]   msg                Pointer to message to be sent
@param[in]   msg_len            Length of the message

@retval      QMI_NO_ERR         Success

*/
/*=========================================================================*/
typedef qmi_client_error_type (*qmi_cci_send_fn_type)
  (
   void *handle,
   void *addr,
   uint8_t *msg,
   uint32_t msg_len
  );

/*=============================================================================
  CALLBACK FUNCTION qmi_cci_close_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure to close the
  transport usually when the service is unregistered. It is crucial that the
  xport synchronize the deallocation of memory and its callback functions so
  the callbacks do not occur after the data associated with the xport has been
  deallocated.

@param[in]   handle              Opaque handle returned by the open call

*/
/*=========================================================================*/
typedef void (*qmi_cci_close_fn_type)
  (
   void *handle
  );

/*=============================================================================
  CALLBACK FUNCTION qmi_cci_open_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure to open a new
  transport 

@param[in]      xport_data         Opaque data associated with the transport
@param[in]      xport_num          Framework assigned enumeration of the xport
@param[in]      service_id         Service ID
@param[in]      version            Version of the service
@param[in/out]  num_entries        Number of entries in the array and number
                                   of entries filled.
@param[out]     service_list       Linked list of server records

@retval         Total number of servers found

*/
/*=========================================================================*/
typedef uint32_t (*qmi_cci_lookup_fn_type)
  (
   void    *xport_data,
   uint8_t  xport_num,
   uint32_t service_id,
   uint32_t version,
   uint32_t *num_entries,
   qmi_cci_service_info *service_info
  );

/*=============================================================================
  CALLBACK FUNCTION qmi_cci_addr_len_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCCI infrastructure to retrieve the
  length of the (destination) address of the xport.

@retval         Length of address

*/
/*=========================================================================*/
typedef uint32_t (*qmi_cci_addr_len_fn_type)
  (
   void
  );

/* xport ops table type */
typedef struct qmi_cci_xport_ops_s
{
  qmi_cci_open_fn_type open;
  qmi_cci_send_fn_type send;
  qmi_cci_close_fn_type close;
  qmi_cci_lookup_fn_type lookup;
  qmi_cci_addr_len_fn_type addr_len;
} qmi_cci_xport_ops_type;

/*=============================================================================
                        Xport to infrastructure up-calls
=============================================================================*/
/*=============================================================================
  FUNCTION qmi_cci_xport_start
=============================================================================*/
/*!
@brief
  This function is used to register a transport with the infrastructure

@param[in]   ops                Pointer to transport operations table
@param[in]   xport_data         Opaque data associated with the transport,
                                such as port ID or other parameters.

@notes
  A client is aware of xports started before it. There is no way
  to make a client aware of xports started after its creation.
*/
/*=========================================================================*/
void qmi_cci_xport_start
(
 qmi_cci_xport_ops_type *ops,
 void *xport_data
 );

/*=============================================================================
  FUNCTION qmi_cci_xport_stop
=============================================================================*/
/*!
@brief
  This function is used to un-register a transport with the infrastructure

@param[in]   ops                Pointer to transport operations table
@param[in]   xport_data         Opaque data associated with the transport,
                                such as port ID or other parameters.

@notes
  This call only makes sure that future clients wont use the xport, 
  clients started after the xport is started to this point will continue
  to use the xport -- There is no way to work around this this. 
*/
/*=========================================================================*/
void qmi_cci_xport_stop
(
 qmi_cci_xport_ops_type *ops,
 void *xport_data
 );

/*=============================================================================
  FUNCTION qmi_cci_xport_resume
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal the infrastructure to
  resume if a previous send was was blocked on flow control.

@param[in]   clnt               Pointer to infrastructure's client struct

@retval      QMI_CSI_NO_ERR     Success
*/
/*=========================================================================*/
void qmi_cci_xport_resume
(
  qmi_cci_client_type *clnt
);

/*=============================================================================
  FUNCTION qmi_cci_xport_recv
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal the infrastructure to
  process the incoming message, one at a time.

@param[in]   clnt               Pointer to infrastructure's client struct
@param[in]   addr               Pointer to source address.
@param[in]   buf                Pointer to message to be received
@param[in]   len                Length of the message

@retval      QMI_CSI_NO_ERR     Success
*/
/*=========================================================================*/
qmi_client_error_type qmi_cci_xport_recv
(
 qmi_cci_client_type *clnt,
 void *addr,
 uint8_t *buf,
 uint32_t len
 );

/*=============================================================================
  FUNCTION qmi_cci_xport_closed
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal the infrastructure after
  the transport is fully closed so the infrastructure can free up client's
  data structure.

@param[in]   clnt               Pointer to infrastructure's client struct

*/
/*=========================================================================*/
void qmi_cci_xport_closed
(
 qmi_cci_client_type *clnt
 );

/*=============================================================================
  FUNCTION qmi_cci_xport_event_new_server
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal the infrastructure of a new
  server registration event. Client can query lookup for the new server

@param[in]   clnt               Pointer to infrastructure's client struct
@param[in]   addr               Pointer to source address.

*/
/*=========================================================================*/
void qmi_cci_xport_event_new_server
(
 qmi_cci_client_type *clnt,
 void *addr
 );

/*=============================================================================
  FUNCTION qmi_cci_xport_event_remove_server
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal the infrastructure of a 
  server unregistration event.

@param[in]   clnt               Pointer to infrastructure's client struct
@param[in]   addr               Pointer to source address.

*/
/*=========================================================================*/
void qmi_cci_xport_event_remove_server
(
 qmi_cci_client_type *clnt,
 void *addr
 );

/*=============================================================================
  FUNCTION qmi_cci_xport_event_server_error
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal the infrastructure of a 
  server unreachable event with an error code.

@param[in]   clnt               Pointer to infrastructure's client struct
@param[in]   addr               Pointer to source address.
@param[in]   error              Error type in connection termination

*/
/*=========================================================================*/
void qmi_cci_xport_event_server_error
(
 qmi_cci_client_type *clnt,
 void *addr,
 int error
 );


#endif
