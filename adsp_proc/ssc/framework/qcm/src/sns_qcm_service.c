/*=============================================================================
  @file sns_qcm_service.c

  This file handles the QSocket-specific service handling.  A corresponding
  file may be written to support other communication mechanisms.

  @note
  QSockets does not notify the service when a client has disconnected.  The
  service is only notified when qsendto fails.  If this becomes an issue, we
  should consider sending a periodic "still there" message.

  PEND: QSocket will not notify a service when one of its clients has
  disconnected: services will only be made aware once a subsequent sendto
  fails.  Therefore in order to "clean-up" disconnected clients, the QCM
  should send a periodic "isalive" message.  Perhaps once an hour; or once
  its socket descriptor count reaches some threshold.

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_decode.h"
#include "pb_encode.h"
#include "qsocket_ids.h"
#include "sns_assert.h"
#include "sns_client.pb.h"
#include "sns_client_qsocket.pb.h"
#include "sns_fw_diag_service.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_osa_lock.h"
#include "sns_osa_thread.h"
#include "sns_pwr_scpm_mgr.h"
#include "sns_printf.h"
#include "sns_qcm.h"
#include "sns_qcm_service.h"
#include "sns_thread_prio.h"
#include "sns_time.h"

#ifdef SNS_ISLAND_INCLUDE_QCM
#include "uqsocket.h"
#include "uqsocket_ipcr.h"

#define sns_qsocket uqsocket
#define sns_qsendto uqsendto
#define sns_qrecvfrom uqrecvfrom
#define sns_qbind uqbind
#else
#include "qsocket.h"
#include "qsocket_ipcr.h"

#define sns_qsocket qsocket
#define sns_qsendto qsendto
#define sns_qrecvfrom qrecvfrom
#define sns_qbind qbind
#endif

/*=============================================================================
 * Macros and Constants
 ============================================================================*/

// Size of the QCM Thread
#define SNS_QCM_SERVICE_STACK_SIZE 2048
#define SNS_QCM_SERVICE_THREAD_NAME "SNS_QCM_SERVICE"

/* Maximum posible size for outgoing indication messages. */
// PEND: This may need to be increased, based on CHRE use cases
#define SNS_QCM_IND_LEN_MAX 1000
/* Confirm client presence at most every X clock ticks (60 minutes). */
#define SNS_QCM_CLIENT_CHECK_TIME 19200000ULL * 60 * 60

/* Approximately how much space should be set aside per indication message
 * for the header and other meta data (other than the events themselves). */
#define SNS_QCM_SERVICE_IND_HDR_LEN 100

/*=============================================================================
  Data Type Definitions
  ===========================================================================*/

/**
 * Singleton Client Manager state.
 */
typedef struct sns_qcm_service
{
  /* QSocket handle identifying this service */
  int sock_desc;

  /* Thread which will process all incoming QSocket tasks */
  sns_osa_thread *thread;

  /* For power voting purposes */
  sns_time start_time;
  sns_time prev_start_time;
  sns_time total_time;
  sns_time active_time;
  sns_time start_active_time;

  /* All client connections */
  sns_isafe_list connections;
  sns_osa_lock connections_lock;

  /* Connections pending closure */
  sns_isafe_list closing_conns;
  sns_osa_lock closing_conns_lock;

  /* Service callback functions */
  sns_qcm_service_connect connect;
  sns_qcm_service_disconnect disconnect;
  sns_qcm_service_req request;
  sns_qcm_service_resume resume;

  /* QCM Sensor Handle; used only for Printf */
  struct sns_sensor *sensor;

  void *service_cookie;
} sns_qcm_service;

/**
 * If QSockets is busy while trying to send a response message, QCM must
 * buffer the response until the channel is available.  Empirically, this
 * occurs rarely, and hence is not optimized.
 */
typedef struct sns_qcm_resp
{
  struct sns_qcm_resp *next;
  uint32_t request_id;
  sns_rc err;
} sns_qcm_resp;

/**
 * A connection to some client.
 */
typedef struct sns_qcm_conn
{
  /* On sns_qcm_service::connections */
  sns_isafe_list_item item;

  /* Unique identifier for this client */
  uint64_t client_id;
  /* Service on which this client is connected;
   * Note: At the moment, there is only one Service. */
  sns_qcm_service *service;
  /* Client address */
  struct qsockaddr_ipcr addr;
  /* Opaque data returned from sns_qcm_service_connect() */
  void *conn_handle;
  /* Maximum indication length as specified by the client */
  uint32_t ind_max_len;
  /* Time at which the most recent event was sent */
  sns_time last_sent_time;
  /* Pending responses on this connection; oldest at head */
  sns_qcm_resp *pending_resp;
} sns_qcm_conn;

/*=============================================================================
  Static Data
  ===========================================================================*/

/* Stack used by the QSocket thread (waits on QSocket file descriptor) */
static uint8_t service_thread_stack[SNS_QCM_SERVICE_STACK_SIZE]
  SNS_SECTION(".data.sns");

/* Similar scratch space for outgoing indication messages */
static uint8_t ind_msg[SNS_QCM_IND_LEN_MAX] SNS_SECTION(".data.sns");

/* Used to assign unique client IDs to each incoming connection */
static uint64_t client_id_next SNS_SECTION(".data.sns") = 0;

/* Protects ind_msg and client_id_next state */
static sns_osa_lock qcm_service_lock;

/* Sampling period to use when determining MCPS vote for QCM thread */
static sns_time thread_util_sampling_period;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Find the connection associated with the specific sender address.  If not
 * found, create a new connection object.
 *
 * @param[i] service The singleton QCM Service object
 * @param[i] sender_addr Address of the client which sent QCM a message
 *
 * @return Found or created connection; never NULL
 */
SNS_SECTION(".text.sns") static sns_qcm_conn*
find_connection(sns_qcm_service *service, struct qsockaddr_ipcr *sender_addr)
{
  sns_isafe_list_iter iter;
  sns_qcm_conn *conn;

  sns_osa_lock_acquire(&service->connections_lock);
  for(sns_isafe_list_iter_init(&iter, &service->connections, true),
      conn = (sns_qcm_conn*)sns_isafe_list_iter_get_curr_data(&iter);
      NULL != conn;
      conn = (sns_qcm_conn*)sns_isafe_list_item_get_data(sns_isafe_list_iter_advance(&iter)))
  {
    if(0 == sns_memcmp(sender_addr, &conn->addr, sizeof(*sender_addr)))
      break;
  }
  sns_osa_lock_release(&service->connections_lock);

  if(NULL == conn)
  {
    sns_rc rc;

#ifdef SNS_ISLAND_INCLUDE_QCM
    conn = sns_malloc(SNS_HEAP_ISLAND, sizeof(*conn));
#endif
    if(NULL == conn)
    {
      SNS_ISLAND_EXIT();
      conn = sns_malloc(SNS_HEAP_MAIN, sizeof(*conn));
    }
    SNS_ASSERT(NULL != conn);
    conn->addr = *sender_addr;
    conn->service = service;
    conn->pending_resp = NULL;
    conn->ind_max_len = UINT32_MAX;

    sns_osa_lock_acquire(&qcm_service_lock);
    conn->client_id = client_id_next++;
    SNS_ASSERT(UINT64_MAX != client_id_next);
    sns_osa_lock_release(&qcm_service_lock);

    sns_isafe_list_item_init(&conn->item, conn);

    SNS_PRINTF(MED, service->sensor, "New client (%i, %i)",
        sender_addr->address.addr.port_addr.node_id,
        sender_addr->address.addr.port_addr.port_id);

    sns_osa_lock_acquire(&service->connections_lock);
    sns_isafe_list_iter_init(&iter, &service->connections, false);
    sns_isafe_list_iter_insert(&iter, &conn->item, true);
    sns_osa_lock_release(&service->connections_lock);

    rc = service->connect(conn, service->service_cookie, &conn->conn_handle);
    if(SNS_RC_SUCCESS != rc)
    {
      SNS_PRINTF(ERROR, service->sensor, "service_connect %i", rc);
      sns_free(conn);
      conn = NULL;
    }
  }

  return conn;
}

/**
 * Complete the disconnect process for any closing connections; in most cases
 * there will be no closing connections.
 *
 * No locks may be held when calling the service callbacks, to ensure that the
 * library lock may be always acquired FIRST.
 */
SNS_SECTION(".text.sns") static void
remove_closing(sns_qcm_service *service)
{
  sns_isafe_list_iter iter;

  sns_osa_lock_acquire(&service->closing_conns_lock);
  sns_isafe_list_iter_init(&iter, &service->closing_conns, true);
  while(NULL != sns_isafe_list_iter_curr(&iter))
  {
    sns_qcm_conn *conn;

    conn = sns_isafe_list_item_get_data(sns_isafe_list_iter_remove(&iter));
    sns_osa_lock_release(&service->closing_conns_lock);

    while(NULL != conn->pending_resp)
    {
      sns_qcm_resp *next = conn->pending_resp->next;
      sns_free(conn->pending_resp);
      conn->pending_resp = next;
    }

    service->disconnect(conn->conn_handle, service->service_cookie);
    sns_free(conn);

    sns_osa_lock_acquire(&service->closing_conns_lock);
    sns_isafe_list_iter_init(&iter, &service->closing_conns, true);
  }
  sns_osa_lock_release(&service->closing_conns_lock);
}

/**
 * Encode callback used by the DIAG Service.  Since our log packet is already
 * encoded, there's no need to do anything special here.
 */
SNS_SECTION(".text.sns") static sns_rc
log_encode_cb(void *restrict log, size_t log_size, size_t encoded_log_size,
    void *restrict encoded_log, size_t *bytes_written)
{
  sns_memscpy(encoded_log, encoded_log_size, log, log_size);
  *bytes_written = log_size;

  return SNS_RC_SUCCESS;
}

/**
 * Initiate a log submission to the DIAG Service.
 *
 * @param[i] conn
 * @param[i] payload Encoded message of type sns_client_qsocket_msg
 * @param[i] payload_len Length of the payload buffer
 */
SNS_SECTION(".text.sns") static void
log_msg(sns_qcm_conn *conn, void *payload, uint32_t payload_len)
{
  sns_fw_diag_service_log_id log_id = SNS_LOG_QSOCKET_CLIENT_API;
  void *buf;
  char data_type[] = SNS_QCM_SENSOR_NAME;

  // PEND: Check debug_config (of QCM Sensor) to check if enabled

  buf = sns_diag_log_alloc(payload_len, log_id);
  if(NULL != buf)
  {
    sns_memscpy(buf, payload_len, payload, payload_len);

    sns_diag_publish_sensor_log((size_t)conn, &qcm_sensor_suid, data_type,
        log_id, payload_len, buf, payload_len, &log_encode_cb);
  }
}

/**
 * Send a message over a QSocket Connection.
 *
 * @note If an erroneous message was received from a client, we wish to send
 * and response message, but not create a persistent connection object.
 *
 * @param[i] conn Persistent connection if client_id != UINT64_MAX
 * @param[i] msg Encoded message of type sns_client_qsocket_msg
 * @param[i] msg_len Length of msg buffer
 *
 * @return See sns_qcm_service_send.
 */
SNS_SECTION(".text.sns") sns_rc
send_msg(sns_qcm_conn *conn, void *msg, uint32_t msg_len)
{
  sns_rc rv = SNS_RC_SUCCESS;
  int err;

  if(msg_len >= 4)
    log_msg(conn, msg, msg_len);

  err = sns_qsendto(conn->service->sock_desc, msg, msg_len,
      QMSG_DONTWAIT, (struct qsockaddr*)&conn->addr, sizeof(conn->addr));

  conn->last_sent_time = sns_get_system_time();

  SNS_PRINTF(LOW, conn->service->sensor, "qsendto %i (%i, %i)",
      err,conn->addr.address.addr.port_addr.node_id,
      conn->addr.address.addr.port_addr.port_id); // PEND: Remove
  if(QEWOULDBLOCK == err)
  {
    rv = SNS_RC_NOT_AVAILABLE;
  }
  else if(QENOMEM == err)
  {
    rv = SNS_RC_FAILED;
  }
  else if(UINT64_MAX != conn->client_id &&
      (QENOTCONN == err || QEHOSTUNREACH == err))
  {
    sns_isafe_list_iter iter;

    // Move connection to "closing" list
    sns_osa_lock_acquire(&conn->service->connections_lock);
    sns_isafe_list_item_remove(&conn->item);
    sns_osa_lock_release(&conn->service->connections_lock);

    sns_osa_lock_acquire(&conn->service->closing_conns_lock);
    sns_isafe_list_iter_init(&iter, &conn->service->closing_conns, false);
    sns_isafe_list_iter_insert(&iter, &conn->item, true);
    sns_osa_lock_release(&conn->service->closing_conns_lock);

    rv = SNS_RC_INVALID_STATE;
  }

  return rv;
}

/**
 * Send the required response back to the client.
 *
 * @param[i] conn
 * @param[i] request_id Identifier as received in the request message
 * @param[i] err The value to be placed in sns_client_qsocket_resp::error
 *
 * @return See sns_qcm_service_send.
 */
SNS_SECTION(".text.sns") static sns_rc
send_resp(sns_qcm_conn *conn, uint32_t request_id, sns_rc err)
{
  sns_client_qsocket_msg msg = sns_client_qsocket_msg_init_default;
  pb_ostream_t stream;
  uint8_t resp_msg[20];
  sns_rc rv = SNS_RC_FAILED;

  msg.which_msg = sns_client_qsocket_msg_resp_tag;
  msg.msg.resp.client_id = conn->client_id;
  msg.msg.resp.error = (sns_std_error)err;
  msg.msg.resp.request_id = request_id;

  stream = pb_ostream_from_buffer(resp_msg, sizeof(resp_msg));
  if(!pb_encode(&stream, sns_client_qsocket_msg_fields, &msg))
    SNS_PRINTF(ERROR, conn->service->sensor, "Encode failed");
  else
    rv = send_msg(conn, resp_msg, stream.bytes_written);

  return rv;
}

/**
 * Decode and remove the "header" information used by QSockets, and pass along
 * the encoded sns_client_request_msg to the Client Manager.
 *
 * @param[i] conn Connection on which this request arrived
 * @param[i] msg Encoded message of type sns_client_qsocket_msg
 * @param[i] msg_len Length of msg buffer
 * @param[o] request_id Extracted identifier sns_client_qsocket_req::request_id
 *
 * @return See sns_qcm_service_req
 */
SNS_SECTION(".text.sns") static sns_rc
handle_msg(sns_qcm_conn *conn, void *msg, size_t msg_len, uint32_t *request_id)
{
  pb_istream_t istream;
  pb_wire_type_t wire_type;
  bool eof = false;
  uint32_t tag;
  sns_rc rv = SNS_RC_FAILED;

  log_msg(conn, msg, msg_len);
  istream = pb_istream_from_buffer(msg, msg_len);
  while(pb_decode_tag(&istream, &wire_type, &tag, &eof) && !eof)
  {
    if(PB_WT_STRING == wire_type && sns_client_qsocket_msg_req_tag == tag)
    {
      pb_istream_t sub_stream;
      if(pb_make_string_substream(&istream, &sub_stream))
      {
        while(pb_decode_tag(&sub_stream, &wire_type, &tag, &eof) && !eof)
        {
          if(PB_WT_32BIT == wire_type && sns_client_qsocket_resp_request_id_tag == tag)
          {
            if(!pb_decode_fixed32(&sub_stream, request_id))
              SNS_PRINTF(ERROR, conn->service->sensor, "Error decoding request_id");
          }
          else if(PB_WT_32BIT == wire_type && sns_client_qsocket_req_ind_len_max_tag == tag)
          {
            if(!pb_decode_fixed32(&sub_stream, &conn->ind_max_len))
              SNS_PRINTF(ERROR, conn->service->sensor, "Error decoding ind_max_len");
          }
          else if(PB_WT_STRING == wire_type && sns_client_qsocket_req_request_tag == tag)
          {
            pb_istream_t inner_stream;
            if(pb_make_string_substream(&sub_stream, &inner_stream))
            {
              rv = conn->service->request(conn->conn_handle, NULL,
                  inner_stream.state, inner_stream.bytes_left,
                  conn->service->service_cookie);
              pb_read(&inner_stream, NULL, inner_stream.bytes_left);
              pb_close_string_substream(&sub_stream, &inner_stream);
            }
            // PEND: If substring creation fails, abort handle_msg
          }
          else
            pb_skip_field(&sub_stream, wire_type);
        }
      }
      break;
    }
    pb_skip_field(&istream, wire_type);
  }

  return rv;
}

/**
 * Send all pending responses (if any) for a connection.
 *
 * @return true if all responses sent successfully; false if busy
 */
SNS_SECTION(".text.sns") static bool
send_pending_resp(sns_qcm_conn *conn)
{
  while(NULL != conn->pending_resp)
  {
    sns_qcm_resp *pending_resp = conn->pending_resp;
    if(SNS_RC_NOT_AVAILABLE ==
        send_resp(conn, pending_resp->request_id, pending_resp->err))
      return false;

    conn->pending_resp = pending_resp->next;
    sns_free(pending_resp);
  }

  return true;
}

/**
 * Store a response message which could not be sent due to QSocket flow
 * control.
 */
SNS_SECTION(".text.sns") static void
store_pending_resp(sns_qcm_conn *conn, uint32_t request_id, sns_rc err)
{
  sns_qcm_resp *pending_resp = NULL;
#ifdef SNS_ISLAND_INCLUDE_QCM
  pending_resp = sns_malloc(SNS_HEAP_ISLAND, sizeof(*pending_resp));
#endif
  if(NULL == pending_resp)
  {
    SNS_ISLAND_EXIT();
    pending_resp = sns_malloc(SNS_HEAP_MAIN, sizeof(*pending_resp));
  }
  SNS_ASSERT(NULL != pending_resp);

  pending_resp->next = NULL;
  pending_resp->request_id = request_id;
  pending_resp->err = err;

  if(NULL == conn->pending_resp)
  {
    conn->pending_resp = pending_resp;
  }
  else
  {
    sns_qcm_resp *temp;
    for(temp = conn->pending_resp; NULL != temp->next; temp = temp->next) ;
    temp->next = pending_resp;
  }
}

/**
 * Wait for an incoming event; once arrived, perform all necessary
 * processing handling.
 *
 * @return System time at which the message was received
 */
SNS_SECTION(".text.sns") static sns_time
wait_and_handle(sns_qcm_service *service)
{
  uint32_t request_id = 0;
  struct qsockaddr_ipcr sender_addr;
  size_t addr_len = sizeof(sender_addr);
  int len;
  sns_rc rc = SNS_RC_NOT_AVAILABLE;
  sns_qcm_conn *conn = NULL;
  void *buffer = NULL;
  sns_time rv;

  sns_osa_thread_idle();
  // Note: sender_addr Doesn't seem to be populated with QMSG_PEEK
  len = sns_qrecvfrom(service->sock_desc, NULL, 0, QMSG_PEEK,
      (struct qsockaddr*)&sender_addr, &addr_len);
  rv = sns_get_system_time();
  sns_osa_thread_active();

  if(len >= 0)
  {
    len = SNS_MAX(len, 1);
#ifdef SNS_ISLAND_INCLUDE_QCM
    buffer = sns_malloc(SNS_HEAP_ISLAND, len);
#endif
    if(NULL == buffer)
    {
      SNS_ISLAND_EXIT();
      buffer = sns_malloc(SNS_HEAP_MAIN, len);
    }
    SNS_ASSERT(NULL != buffer);

    len = sns_qrecvfrom(service->sock_desc, buffer, len, 0,
      (struct qsockaddr*)&sender_addr, &addr_len);
  }

  remove_closing(service);

  if(QEMSGSIZE == len)
  {
    rc = SNS_RC_POLICY;
  }
  else if(len < 0)
  {
    SNS_PRINTF(ERROR, service->sensor, "qrecvfrom %i", len);
  }
  else
  {
    conn = find_connection(service, &sender_addr);
    if(NULL != conn)
    {
      if(0 == len)
      {
        if(send_pending_resp(conn))
          service->resume(conn->conn_handle, service->service_cookie);
      }
      else
      {
        rc = handle_msg(conn, buffer, len, &request_id);
      }
    }
  }
  sns_free(buffer);

  // PEND: Need to ensure we always send back the proper request_id, even
  // in error scenarios
  if(0 != len)
  {
    sns_qcm_conn temp = (sns_qcm_conn){ .client_id = UINT64_MAX,
      .service = service, .addr = sender_addr, .conn_handle = NULL };

    if(SNS_RC_NOT_AVAILABLE == send_resp(NULL == conn ? &temp : conn, request_id, rc) &&
        NULL != conn)
      store_pending_resp(conn, request_id, rc);
  }

  return rv;
}

/**
 * QCM Service thread main function.  Wait upon and respond to QSocket tasks.
 */
SNS_SECTION(".text.sns") static void
qcm_service_main(void *arg)
{
  int err;
  static struct qsockaddr_ipcr addr;
  sns_qcm_service *service = (sns_qcm_service*)arg;

  sns_osa_thread_active();
  service->sock_desc = sns_qsocket(AF_IPC_ROUTER, QSOCK_STREAM, 0);
  SNS_ASSERT(service->sock_desc >= 0);

  SNS_PRINTF(LOW, service->sensor, "qcm_service_main");

  addr.sa_family = AF_IPC_ROUTER;
  addr.address.addrtype = IPCR_ADDR_NAME;
  addr.address.addr.port_name.service = QSOCKET_SNS_CLIENT_SVC_ID;
  addr.address.addr.port_name.instance = 10;
  err = sns_qbind(service->sock_desc, (struct qsockaddr*)&addr, sizeof(addr));
  SNS_ASSERT(0 == err);

  service->start_active_time = sns_get_system_time();
  thread_util_sampling_period =
    sns_convert_ns_to_ticks(SNS_THREAD_UTIL_SAMPLING_PERIOD_NS);

  for(;;)
  {
    service->prev_start_time = service->start_time;
    service->start_time = sns_get_system_time();
    if(service->start_active_time != 0)
    {
      service->total_time += (service->start_time - service->prev_start_time);
      service->active_time += (service->start_time - service->start_active_time);
      if(service->total_time > thread_util_sampling_period)
      {
        uint32_t thread_utilization =
          (uint16_t)((service->active_time * 100) / service->total_time);
        sns_pwr_scpm_mgr_update_thread_utilization(
            service->thread, thread_utilization);
        service->total_time = 0;
        service->active_time = 0;
      }
    }

    service->start_active_time = wait_and_handle(service);
  }
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

void
sns_qcm_service_init(sns_qcm_service **service_out,
    void *service_cookie, sns_qcm_service_connect connect,
    sns_qcm_service_disconnect disconnect, sns_qcm_service_req request,
    sns_qcm_service_resume resume, struct sns_sensor *sensor)
{
  sns_osa_thread_attr tattr;
  sns_osa_lock_attr lattr;
  sns_rc rc;
  sns_qcm_service *service = NULL;
  sns_osa_mem_type mem_type = SNS_OSA_MEM_TYPE_NORMAL;

#ifdef SNS_ISLAND_INCLUDE_QCM
  service = sns_malloc(SNS_HEAP_ISLAND, sizeof(*service));
  mem_type = SNS_OSA_MEM_TYPE_ISLAND;
#endif
  if(NULL == service)
    service = sns_malloc(SNS_HEAP_MAIN, sizeof(*service));
  SNS_ASSERT(NULL != service);
  service->service_cookie = service_cookie;
  service->connect = connect;
  service->disconnect = disconnect;
  service->request = request;
  service->resume = resume;
  service->sensor = sensor;

  sns_isafe_list_init(&service->connections);
  sns_isafe_list_init(&service->closing_conns);

  rc = sns_osa_lock_attr_init(&lattr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
#ifdef SNS_ISLAND_INCLUDE_QCM
  rc = sns_osa_lock_attr_set_memory_partition(&lattr, SNS_OSA_MEM_TYPE_ISLAND);
#else
  rc = sns_osa_lock_attr_set_memory_partition(&lattr, SNS_OSA_MEM_TYPE_NORMAL);
#endif
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  sns_osa_lock_init(&lattr, &service->closing_conns_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  sns_osa_lock_init(&lattr, &service->connections_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  sns_osa_lock_init(&lattr, &qcm_service_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_osa_thread_attr_init(&tattr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_thread_attr_set_stack(&tattr, (uintptr_t)service_thread_stack,
      sizeof(service_thread_stack));
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_thread_attr_set_priority(&tattr, SNS_CM_THREAD_PRIO);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_thread_attr_set_name(&tattr, SNS_QCM_SERVICE_THREAD_NAME);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_thread_attr_set_memory_partition(&tattr, mem_type);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  rc = sns_osa_thread_create(&qcm_service_main, service, &tattr,
      &service->thread);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  *service_out = service;
}

SNS_SECTION(".text.sns") sns_rc
sns_qcm_service_send(sns_qcm_conn *conn, sns_client_event_msg const *event)
{
  sns_rc rv = SNS_RC_INVALID_VALUE;
  pb_ostream_t stream;
  sns_client_qsocket_msg msg = sns_client_qsocket_msg_init_default;

  if(!sns_island_is_island_ptr((intptr_t)conn))
    SNS_ISLAND_EXIT();

  msg.which_msg = sns_client_qsocket_msg_ind_tag;
  msg.msg.ind.client_id = conn->client_id;
  msg.msg.ind.event = *event;

  sns_osa_lock_acquire(&qcm_service_lock);
  stream = pb_ostream_from_buffer(ind_msg, SNS_MIN(sizeof(ind_msg), conn->ind_max_len));
  if(!pb_encode(&stream, sns_client_qsocket_msg_fields, &msg))
    SNS_SPRINTF(ERROR, conn->service->sensor,
        "Error encoding sns_client_qsocket_msg %s", PB_GET_ERROR(&stream));
  else
    rv = send_msg(conn, ind_msg, stream.bytes_written);
  sns_osa_lock_release(&qcm_service_lock);

  return rv;
}

void
sns_qcm_service_check(struct sns_qcm_service *service)
{
  sns_isafe_list_iter iter;
  sns_qcm_conn *conn;
  uint8_t msg = 0;
  sns_time curr_time = sns_get_system_time();

  for(sns_isafe_list_iter_init(&iter, &service->connections, true),
      conn = (sns_qcm_conn*)sns_isafe_list_iter_get_curr_data(&iter);
      NULL != conn;
      conn = (sns_qcm_conn*)sns_isafe_list_iter_get_curr_data(&iter))
  {
    sns_isafe_list_iter_advance(&iter);
    if(curr_time < conn->last_sent_time + SNS_QCM_CLIENT_CHECK_TIME)
      send_msg(conn, &msg, sizeof(msg));
  }
}

SNS_SECTION(".text.sns") uint32_t
sns_qcm_service_max_ind_len(struct sns_qcm_conn *conn)
{
  return SNS_MIN(sizeof(ind_msg), conn->ind_max_len) - SNS_QCM_SERVICE_IND_HDR_LEN;
}
