#ifndef QMI_CSI_COMMON_H
#define QMI_CSI_COMMON_H
/******************************************************************************
  @file    qmi_csi_common.h
  @brief   The QMI Common Service Interface (CSI) common header file

  DESCRIPTION
  QMI Common Service Interface types

  ---------------------------------------------------------------------------
  Copyright (c) 2010-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
*******************************************************************************/
#include "qmi_common.h"
#include "qmi_csi.h"

/*
 * The data structure looks as follows:
 * - The service list has a list of active services
 * - Each service has a table of transports (xports) associated with it
 * - Each service also has a list of connected clients
 * - Each client has a pointer to the xport it connected from
 * - Each client also has a list of outstanding transactions (txns)
 *
 * Service list -> service 1 -> service 2 ->...
 *                 ^
 *                 |
 *                 +-> [xport 1] [xport 2] [...]
 *                 |      ^
 *                 |      |-----------+
 *                 |      |           |
 * Glb clnt lst -> +-> client 1 -> client 2 ->...
 *                      ^
 *                      |
 * Global txn list ->   +-> txn 1 -> txn 2 ->...
 *
 * The global client and txn lists allow client and txn handles to be validated
 *
 * The transport abstraction abstracts the xport from the common QCSI layer
 * Up-calls from the xport:
 * - connect, receive, notify, disconnect and closed
 *
 * The receive call is used to push data up the stack, 1 msg at a time
 *
 * The notify call is used to notify the stack of an event (e.g. Rx ready,
 * connect, disconnect) on the transport channel. It is optional for systems
 * supporting select()
 *
 * These callbacks should be serialized on a per transport basis
 * (e.g. while in receive, connect/disconnect/closed should not happen)
 *
 * The xport_start function regsiters a function table with the common layer
 * consisting of:
 * - open, register server, unregister server, send, handle event, close and
 *   get address length functions
 *
 * The handle event function is optional and is used for handling requests
 * and events from the server's context, instead of inside the transport's
 * Rx callback so not to block or starve other requests.
 *
 * The get address length function is used to determine how many bytes of the
 * address pointer is to be copied or compared as each transport has a different
 * address structure.
 *
 * The os_param parameter into qmi_csi_register is used two ways:
 *
 * 1) On Linux, it is used as an output containing the active fd_set that
 *    can be passed into select() so the service can listen on multiple file
 *    descriptors or transports. Upon returning from select(), the active
 *    read fd_set is passed into qmi_csi_handle_event() as another os_param
 *    so the file descriptor can be read and the events can be processed by
 *    invoking the service's callbacks. To not block on the read, the file
 *    descriptors need to be configured as non-blocking by the xport layer.
 *
 * 2) On AMSS, it is used as an input containing the TCB and signal to set
 *    when an event occurs on the transport. Upon receiving the signal, the
 *    service calls qmi_csi_handle_event() with the received signal as an
 *    os_param to allow the event and service callbacks to be executed in the
 *    service's context.
 */
typedef uint16_t txn_id_type;
struct qmi_csi_service_s;
struct qmi_csi_client_s;
struct qmi_csi_txn_s;
struct qmi_csi_xport_s;
struct qmi_csi_xport_ops_s;
struct qmi_power_save_s;

#ifndef TXN_POOL_SIZE
#define TXN_POOL_SIZE 5
#endif
#define MAX_NUM_STATES 2

/*=============================================================================
     Xport Send option flags
=============================================================================*/

/** This option can be used with an xport's send method to instruct
  * the transport to make sure that sending this packet does not increase
  * the total number of packets in the TX Queue beyond a predefined limit.
  * The limit is configured at open
  */
#define QMI_CSI_SEND_FLAG_RATE_LIMITED (1)

/*=============================================================================
     Xport options
=============================================================================*/
typedef struct
{
  /* If xport's send method is called with the flag QMI_CSI_SEND_FLAG_RATE_LIMITED
   * set, then make sure that the internal transmit queue length does not exceed
   * this value.
   */
  uint32_t rate_limited_queue_size;

  /* Scope passed in my client */
  uint64_t scope;
  int      scope_valid;
} qmi_csi_xport_options_type;


typedef struct qmi_csi_service_s
{
  /* links to prev and next in service list */
  LINK(struct qmi_csi_service_s, link);

  /* unique service handle */
  unsigned int handle;

  /* service registration data */
  qmi_idl_service_object_type service_obj;
  qmi_csi_connect     service_connect;
  qmi_csi_disconnect  service_disconnect;
  qmi_csi_process_req service_process_req;
  qmi_csi_process_req service_process_pre_req;
  qmi_csi_resume_ind  resume_ind_cb;
  qmi_csi_req_handler_tbl_type  *req_handler_tbl;
  uint16_t                      req_handler_size;
  qmi_csi_log_msg     log_message_cb;
  qmi_csi_log_msg_ext log_message_ext_cb;
  uint8_t             async_conn_cb;
  void                *service_cookie;

  qmi_csi_xport_options_type xport_options;

  /* xports associated with the server */
  struct qmi_csi_xport_s **xports;
  uint32_t num_xports;

  /* list of active clients associated with this service */
  /* since a global lock is needed to add the node to the global list, no need
   * to have a finer grain lock here */
  /* qmi_csi_lock_type client_list_lock; */
  LIST(struct qmi_csi_client_s, client_list);

  /* IDL Version, which contains instance ID if set */
  uint32_t idl_version;
  /* place holder for implementation/OS specific data */
#ifdef QMI_CSI_OS_DATA
  QMI_CSI_OS_DATA;
#endif

} qmi_csi_service_type;

typedef struct qmi_csi_xport_s
{
  /* xport ops table */
  struct qmi_csi_xport_ops_s *ops;

  /* address length of xport */
  uint32_t addr_len;

  /* opaque handle returned by xport open */
  void *handle;

  /* pointer back to service */
  struct qmi_csi_service_s *service;
} qmi_csi_xport_type;

typedef struct qmi_csi_txn_s
{
  /* local and global links to prev and next in txn list */
  LINK(struct qmi_csi_txn_s, local);
  LINK(struct qmi_csi_txn_s, global);

  /* unique txn handle */
  unsigned int handle;

  /* pointer to client */
  struct qmi_csi_client_s *client;

  /* transaction ID */
  txn_id_type txn_id;

  uint16_t pool_allocated;

  /* message ID for verification */
  uint16_t msg_id;
} qmi_csi_txn_type;

typedef uint8_t qmi_power_state;

typedef enum {
  QMI_POWER_SAVE_WHITE_LIST = 0,
  QMI_POWER_SAVE_BLACK_LIST = 1,
}qmi_power_save_list_type;

/* Internal struct to keep track of clients' filter rules*/
typedef struct qmi_power_save_s{
  qmi_power_state state;

  qmi_power_save_list_type list_type;

  uint32_t message_list_len;
  uint16_t *message_list;

} qmi_power_save_type;

typedef struct qmi_csi_client_s
{
  /* local and global links to prev and next in client list */
  LINK(struct qmi_csi_client_s, local);
  LINK(struct qmi_csi_client_s, global);

  /* unique client handle */
  unsigned int handle;

  struct
  {
    /* pointer to xport */
    qmi_csi_xport_type *xport;
    /* address of client - opaque storage */
    uint8_t addr[MAX_ADDR_LEN];
    /* per-client xport-level storage. e.g. tx queue */
    void *client_data;
  } xport;

  void *connection_handle;

  /* list of active transactions */
  /* since a global lock is needed to add the node to the global list, no need
   * to have a finer grain lock here */
  /* qmi_csi_lock_type txn_list_lock; */
  LIST(struct qmi_csi_txn_s, txn_list);
  LIST(struct qmi_csi_txn_s, txn_free_list);

  struct qmi_csi_txn_s txn_pool[TXN_POOL_SIZE];

  /* pointer back to service */
  struct qmi_csi_service_s *service;

  /* TXN ID counter for indications */
  uint16_t next_ind_txn_id;

#ifdef QMI_POWER_SAVE_FILTERING
  /* Boolean value to determine if a power filter was set */
  uint8_t filter_set;

  /* TXN ID counter for indications */
  qmi_power_save_type filter_rules[MAX_NUM_STATES];
#endif
} qmi_csi_client_type;


/*=============================================================================
     Transport abstraction prototypes - infrastructure to xport down-calls
=============================================================================*/

/*=============================================================================
  CALLBACK FUNCTION qmi_csi_open_legacy_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure to open a new
  transport

@param[in]   xport_data          Opaque parameter to the xport (e.g. port ID)
@param[in]   xport               Pointer to infrastructure's transport struct.
                                 Can be treated as opaque, but prototyped for
                                 ease of debugging.
@param[in]   max_rx_len          Maximum length of messages that can be
                                 received. Used by xport to allocate a buffer
                                 if the underlyin transport cannot pass the
                                 message through a callback.
@param[in]   os_params           OS-specific parameters passed into
                                 qmi_csi_register. Used as output in case of
                                 fd_set to be used with select().

@retval      Opaque handle to the transport. NULL on failure.

*/
/*=========================================================================*/
typedef void *(*qmi_csi_open_legacy_fn_type)
  (
   void *xport_data,
   qmi_csi_xport_type *xport,
   uint32_t max_rx_len,
   qmi_csi_os_params *os_params
  );


/*=============================================================================
  CALLBACK FUNCTION qmi_csi_open_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure to open a new
  transport

@param[in]   xport_data          Opaque parameter to the xport (e.g. port ID)
@param[in]   xport               Pointer to infrastructure's transport struct.
                                 Can be treated as opaque, but prototyped for
                                 ease of debugging.
@param[in]   max_rx_len          Maximum length of messages that can be
                                 received. Used by xport to allocate a buffer
                                 if the underlyin transport cannot pass the
                                 message through a callback.
@param[in]   os_params           OS-specific parameters passed into
                                 qmi_csi_register. Used as output in case of
                                 fd_set to be used with select().
@param[in]   options             Options for the xport.
                                 See qmi_csi_xport_options_type for more
                                 information.

@retval      Opaque handle to the transport. NULL on failure.

*/
/*=========================================================================*/
typedef void *(*qmi_csi_open_fn_type)
  (
   void *xport_data,
   qmi_csi_xport_type *xport,
   uint32_t max_rx_len,
   qmi_csi_os_params *os_params,
   qmi_csi_xport_options_type *options
  );

/*=============================================================================
  CALLBACK FUNCTION qmi_csi_reg_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure to register a
  new server

@param[in]   handle              Opaque handle returned by the open call
@param[in]   service_id          Service ID of the server
@param[in]   version             Version of the service
                                 received. Used by xport to allocate a buffer
                                 if the underlyin transport cannot pass the
                                 message through a callback.

@retval      QMI_CSI_NO_ERR      Success

*/
/*=========================================================================*/
typedef qmi_csi_error (*qmi_csi_reg_fn_type)
  (
   void *handle,
   uint32_t service_id,
   uint32_t version
  );

/*=============================================================================
  CALLBACK FUNCTION qmi_csi_send_legacy_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure to send data to
  a client. The tx queue length or flow is unspecified and left to the
  xport.

@param[in]   handle              Opaque handle returned by the open call
@param[in]   addr                Opaque address sent to the infrastructure
                                 through the connect or recv calls.
@param[in]   msg                 Pointer to message to be sent
@param[in]   msg_len             Length of the message
@param[in]   client_data         Pointer to client-specific storage, if defined

@retval      QMI_CSI_NO_ERR      Success

*/
/*=========================================================================*/
typedef qmi_csi_error (*qmi_csi_send_legacy_fn_type)
  (
   void *handle,
   void *addr,
   uint8_t *msg,
   uint32_t msg_len,
   void **client_data
  );

/*=============================================================================
  CALLBACK FUNCTION qmi_csi_send_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure to send data to
  a client.

@param[in]   handle              Opaque handle returned by the open call
@param[in]   addr                Opaque address sent to the infrastructure
                                 through the connect or recv calls.
@param[in]   msg                 Pointer to message to be sent
@param[in]   msg_len             Length of the message
@param[in]   flags               Or'd flag options for this send.
                                 Currently supported:
                                    QMI_CSI_SEND_FLAG_RATE_LIMITED
                                      - Rate limit this send.
@param[in]   client_data         Pointer to client-specific storage, if defined

@retval      QMI_CSI_NO_ERR      Success

*/
/*=========================================================================*/
typedef qmi_csi_error (*qmi_csi_send_fn_type)
  (
   void *handle,
   void *addr,
   uint8_t *msg,
   uint32_t msg_len,
   uint32_t flags,
   void **client_data
  );

/*=============================================================================
CALLBACK FUNCTION qmi_csi_send_fn_type
=============================================================================*/
/*!
@brief
This callback function is called by the QCSI infrastructure to confirm a client
connection

@param[in]   handle              handle provided in the initial connection
@param[in]   accepted            if the client has been accepted or rejected

@retval      QMI_CSI_NO_ERR      Success

*/
/*=========================================================================*/
typedef void(*qmi_csi_conn_complete_fn_type)
(
  void *handle,
  unsigned int accepted
);

/*=============================================================================
  CALLBACK FUNCTION qmi_csi_msg_handler
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure to automatically
  handle certain message IDs.

@param[in]   service_obj         QMI IDL Service Object
@param[in]   instance_id         QCSI Service Instance
@param[in]   msg_id              Message ID
@param[in]   req_c_struct        Request Structure
@param[in]   req_c_struct_len    Request Structure Length
@param[in]   clnt                Pointer to client

@retval      QMI_CSI_NO_ERR      Success

*/
/*=========================================================================*/
typedef qmi_csi_error (*qmi_csi_msg_handler)
(
  qmi_idl_service_object_type service_obj,
  uint32_t instance_id,
  uint16_t msg_id,
  qmi_req_handle req_handle,
  void *req_c_struct,
  unsigned int req_c_struct_len,
  qmi_csi_client_type *clnt
);
/*=============================================================================
  CALLBACK FUNCTION qmi_csi_handle_event_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure to handle event
  after the service wakes up and calls qmi_csi_handle_events. The function
  should dequeue all received data and call the appropriate functions into
  the infrastructure.

@param[in]   handle              Opaque handle returned by the open call
@param[in]   os_params           OS-specific parameters (e.g. fd_set returned
                                 by select(), signals, events, or NULL)
*/
/*=========================================================================*/
typedef void (*qmi_csi_handle_event_fn_type)
  (
   void *handle,
   qmi_csi_os_params *os_params
  );

/*=============================================================================
  CALLBACK FUNCTION qmi_csi_close_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure to close the
  transport usually when the service is unregistered. It is crucial that the
  xport synchronize the deallocation of memory and its callback functions
  before calling qmi_csi_xport_closed() to free up the rest of the data
  associated with the service.

@param[in]   handle              Opaque handle returned by the open call

*/
/*=========================================================================*/
typedef void (*qmi_csi_close_fn_type)
  (
   void *handle
  );

/*=============================================================================
  CALLBACK FUNCTION qmi_csi_addr_len_fn_type
=============================================================================*/
/*!
@brief
  This callback function is called by the QCSI infrastructure to retrieve the
  length of the (source) address of the xport.

@retval         Length of address

*/
/*=========================================================================*/
typedef uint32_t (*qmi_csi_addr_len_fn_type)
  (
   void
  );

/* xport ops table type */
typedef struct qmi_csi_xport_ops_s
{
  qmi_csi_open_legacy_fn_type open_legacy;
  qmi_csi_reg_fn_type  reg;
  qmi_csi_reg_fn_type  unreg;
  qmi_csi_send_legacy_fn_type send_legacy;
  qmi_csi_handle_event_fn_type handle_event;
  qmi_csi_close_fn_type close;
  qmi_csi_addr_len_fn_type addr_len;
  qmi_csi_open_fn_type open;
  qmi_csi_send_fn_type send;
  qmi_csi_conn_complete_fn_type conn_complete;
} qmi_csi_xport_ops_type;

typedef struct qmi_csi_msg_handler_type_s
{
  LINK(struct qmi_csi_msg_handler_type_s, link);

  /* Message ID */
  uint16_t msg_id;

  /* Function to handle the message */
  qmi_csi_msg_handler msg_handler;
} qmi_csi_msg_handler_type;

/*=============================================================================
  FUNCTION qmi_csi_add_msg_handler
=============================================================================*/
/*!
@brief
  This function is used to register a message handler function and message ID
  pair with the QCSI Framework.  This function is intended to be called at
  framework initialization, and should never be called externally.

@param[in]   msg_id               Message ID
@param[in]   msg_handler_func     Pointer to message handler function
*/
/*=========================================================================*/
qmi_csi_error qmi_csi_add_msg_handler
(
  uint16_t msg_id,
  qmi_csi_msg_handler msg_handler_func
);
/*=============================================================================
                        Xport to infrastructure up-calls
=============================================================================*/
/*=============================================================================
  FUNCTION qmi_csi_xport_start
=============================================================================*/
/*!
@brief
  This function is used to register a transport with the infrastructure

@param[in]   ops                Pointer to transport operations table
@param[in]   xport_data         Opaque data associated with the transport,
                                such as port ID or other parameters.
*/
/*=========================================================================*/
void qmi_csi_xport_start
(
 qmi_csi_xport_ops_type *ops,
 void *xport_data
 );

/*=============================================================================
  FUNCTION qmi_csi_xport_conn_status
=============================================================================*/
/*!
@brief
  Returns if the service is connected to the client provided the address

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.

@retval      0                  The client address is unknown to the service
             1                  The client address is known to the service
*/
/*=========================================================================*/
int qmi_csi_xport_conn_status
(
 qmi_csi_xport_type *xport,
 void *addr
);

/*=============================================================================
  FUNCTION qmi_csi_xport_resume_client
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal the infrastructure that
  a previously busy endpoint is now accepting indications.

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.

@retval      None
*/
/*=========================================================================*/
void qmi_csi_xport_resume_client
(
 qmi_csi_xport_type *xport,
 void *addr
);

/*=============================================================================
  FUNCTION qmi_csi_xport_connect
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal to the infrastructure that
  a new client has connected. In a connectionless environment, this step is
  unnecessary.

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.

@retval      QMI_CSI_NO_ERR     Success
*/
/*=========================================================================*/
qmi_csi_error qmi_csi_xport_connect
(
 qmi_csi_xport_type *xport,
 void *addr
 );

/*=============================================================================
FUNCTION  qmi_csi_xport_send_err_resp
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal the infrastructure to
  process the incoming message, one at a time.

  In most cases, it is triggered by a callback to the handle_event function.

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.
@param[in]   msg                Pointer to message to be received
@param[in]   msg_len            Length of the message
@param[in]   err                Error value to return

@retval      QMI_CSI_NO_ERR     Success
*/
/*=========================================================================*/
qmi_csi_error qmi_csi_xport_send_err_resp
(
qmi_csi_xport_type *xport,
void *addr,
uint8_t *buf,
uint32_t len,
uint32_t err
);

/*=============================================================================
  FUNCTION qmi_csi_xport_recv
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal the infrastructure to
  process the incoming message, one at a time.

  In most cases, it is triggered by a callback to the handle_event function.

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.
@param[in]   msg                Pointer to message to be received
@param[in]   msg_len            Length of the message

@retval      QMI_CSI_NO_ERR     Success
*/
/*=========================================================================*/
qmi_csi_error qmi_csi_xport_recv
(
 qmi_csi_xport_type *xport,
 void *addr,
 uint8_t *buf,
 uint32_t len
 );

/*=============================================================================
  FUNCTION qmi_csi_xport_disconnect
=============================================================================*/
/*!
@brief
  This function is used by the transport to signal to the infrastructure that
  a client has disconnected. The data associated with the client will be freed

@param[in]   xport              Pointer to infrastructure's xport struct
@param[in]   addr               Pointer to source address.

@retval      QMI_CSI_NO_ERR     Success
*/
/*=========================================================================*/
qmi_csi_error qmi_csi_xport_disconnect
(
 qmi_csi_xport_type *xport,
 void *addr
 );

/*=============================================================================
  FUNCTION qmi_csi_xport_closed
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
void qmi_csi_xport_closed
(
 qmi_csi_xport_type *xport
 );

 /*=============================================================================
   FUNCTION qmi_csi_filter_init
 =============================================================================*/
 /*!
 @brief
   Adds message handler (handle_qmi_power_save_filter) to QCSI for
   QMI_CSI_SET_POWER_SAVE_FILTER_ID and initializes power state manager.

 */
 /*=========================================================================*/
 void qmi_csi_filter_init
 (
   void
 );

 /*=============================================================================
   FUNCTION qmi_csi_filter_deinit
 =============================================================================*/
 /*!
 @brief
   For future use

 */
 /*=========================================================================*/
 void qmi_csi_filter_deinit
 (
   void
 );

 /*=============================================================================
   FUNCTION qmi_csi_filter_cleanup_client
 =============================================================================*/
 /*!
 @brief
  Cleans up client's filter rules and their respective message lists.
 */
 /*=========================================================================*/
 void qmi_csi_filter_cleanup_client
 (
   qmi_csi_client_type *clnt
 );

 /*=============================================================================
   FUNCTION qmi_csi_msg_list_find
 =============================================================================*/
 /*!
 @brief
  Generic binary search function, that finds the index of msg_id in a list if
  msg_id exists. It assumes the following:
  1) List is sorted
  2) msg_id is of type (uint16_t)
  3) If a pointer to a struct is given, msg_id is the first element in the
  struct and the struct is not packed.

 @param[in]   list           Pointer to message list
 @param[in]   count          Number of elements in the list
 @param[in]   e_size         Size of each element in the list
 @param[in]   msg_id         Message ID to find

 @retval      Pointer to location in list if found, NULL otherwise
 */
 /*=========================================================================*/
 void *qmi_csi_msg_list_find
 (
   void *list,
   size_t count,
   size_t e_size,
   uint16_t msg_id
 );

 /*=============================================================================
   FUNCTION qmi_csi_msg_list_sort
 =============================================================================*/
 /*!
 @brief
 Recursive sort function, that sorts a list of elements by msg_id.
 It assumes the following:
 1) List is sorted
 2) msg_id is of type (uint16_t)
 3) If a pointer to a struct is given, msg_id is the first element in the
    struct and the struct is not packed.

 @param[in]   list           Pointer to message list
 @param[in]   count          Number of elements in the list
 @param[in]   e_size         Size of each element in the list
 @param[in]   key_ptr        Temp store to hold the key during sorting

 */
 /*=========================================================================*/
 void qmi_csi_msg_list_sort
 (
   void * arr,
   size_t count,
   size_t e_size,
   void * key_ptr
 );

 /*=============================================================================
   FUNCTION qmi_csi_filter_can_send_ind
 =============================================================================*/
 /*!
 @brief
  This function will check with Power State Manager to check if a certain
  indication should be fitlered based on the client's filter rules

 @param[in]   clnt           Pointer to client struct
 @param[in]   msg_id         Message ID to check against

 @retval      1 if can send, 0 otherwise

 */
 /*=========================================================================*/
 uint8_t qmi_csi_filter_can_send_ind
 (
   qmi_csi_client_type *clnt,
   uint16_t msg_id
 );

#endif
