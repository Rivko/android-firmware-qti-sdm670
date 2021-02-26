/*=============================================================================
  @file sns_qsocket_client.c

  Copyright (c) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <inttypes.h>
#include <stringl.h>
#include "pb_decode.h"
#include "pb_encode.h"
#include "qsocket_ids.h"
#include "sns_client.pb.h"
#include "sns_client_qsocket.pb.h"
#include "sns_client.h"
#include "sns_osa.h"
#include "timer.h"
#include "uTimetick.h"

#ifdef SNS_ISLAND_INCLUDE_QCM
#include "uqsocket.h"
#include "uqsocket_ipcr.h"

#define sns_qsocket uqsocket
#define sns_qsend uqsend
#define sns_qrecv uqrecv
#define sns_qconnect uqconnect
#define sns_qpoll uqpoll
#define sns_qclose uqclose
#define sns_ipcr_find_name uipcr_find_name
#else
#include "qsocket.h"
#include "qsocket_ipcr.h"

#define sns_qsocket qsocket
#define sns_qsend qsend
#define sns_qrecv qrecv
#define sns_qconnect qconnect
#define sns_qpoll qpoll
#define sns_qclose qclose
#define sns_ipcr_find_name ipcr_find_name
#endif

/* How many failed qsend attempts before abort (QEWOULDBLOCK and QENOMEM only )*/
#define QSOCKET_SEND_RETRY_CNT 40

/* Wait time between qsend retries (5ms) */
#define QSOCKET_SEND_RETRY_WAIT_US 5000

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/* An outgoing request which is awaiting the corresponding response message. */
typedef struct sns_request
{
  struct sns_request *next;
  /* Request ID sent by this client */
  uint32_t request_id;

  /* Client registered callback */
  sns_client_resp resp_cb;
  void *resp_cb_data;
} sns_request;

/* A client connection; a client process may open multiple qsocket
 * connections. */
typedef struct sns_client
{
  struct sns_client *next;

  /* Pending requests/outstanding responses; assumed that responses will be
   * received in the same order as requests are sent. */
  sns_request *requests;

  /* Client registered callbacks */
  sns_client_ind ind_cb;
  void *ind_cb_data;
  sns_client_error error_cb;
  void *error_cb_data;

  /* QSocket file descriptor */
  int fd;
} sns_client;

/* Singleton global state for this client process */
typedef struct sns_qsocket_state
{
  /* List of active clients */
  sns_client *clients;

  /* Protects all state */
  struct sns_osa_mutex *mutex;

  /* Ensures qpoll() is not called when no clients are present */
  struct sns_osa_sem *sem;
  struct sns_osa_thread *thread;

  /* Dynamically allocated array of active fds, one per client */
  struct qpollfd *fds;
  /* Len of qsocket_fds array */
  size_t fds_len;
  /* If clients have been added/removed, they will be added here; after
   * qpoll returns next, this list will replace qsocket_fds */
  struct qpollfd *new_fds;
  size_t new_fds_len;
} sns_qsocket_state;

/*=============================================================================
  Static Data
  ===========================================================================*/

/* Singleton global state for this client process */
static sns_qsocket_state *qsocket_state = NULL;

/* ID to be assigned to the next outgoing request; sequentially incremented */
static uint32_t next_request_id = 0;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Handle an incoming indication message by decoding and extracting the outer
 * message layer, and passing the payload to the client.
 *
 * @param[i] client
 * @param[i] msg Encoded message of type sns_client_qsocket_ind
 * @param[i] msg_len
 */
static void
handle_ind(sns_client *client, void *msg, uint32_t msg_len)
{
  pb_istream_t stream;
  pb_wire_type_t wire_type;
  bool eof = false;
  uint32_t tag;

  stream = pb_istream_from_buffer(msg, msg_len);
  while(pb_decode_tag(&stream, &wire_type, &tag, &eof) && !eof)
  {
    if(PB_WT_STRING == wire_type && sns_client_qsocket_ind_event_tag == tag)
    {
      pb_istream_t sub_stream;
      if(pb_make_string_substream(&stream, &sub_stream))
      {
        client->ind_cb(client, sub_stream.state,
            sub_stream.bytes_left, client->ind_cb_data);
      }
      break;
    }
    pb_skip_field(&stream, wire_type);
  }
}

/**
 * Find the matching request object given a received request ID.  Remove that
 * request from the client's request queue.
 *
 * @return Found request object; should never be NULL
 */
static sns_request*
get_request(sns_client *client, uint32_t request_id)
{
  sns_request *last = NULL, *request = NULL;
  bool found = false;

  sns_osa_mutex_lock(qsocket_state->mutex);
  request = client->requests;
  while(NULL != request)
  {
    if(request->request_id == request_id)
    {
      found = true;
      break;
    }
    last = request;
    request = request->next;
  }

  if(!found)
    SNS_LOG(WARN, "Unable to find request %"PRIu32" (client fd %i)",
        request_id, client->fd);
  else if(NULL != last)
    last->next = request->next;
  else
    client->requests = request->next;

  sns_osa_mutex_unlock(qsocket_state->mutex);
  return request;
}

/**
 * Add a new outgoing request to this client's queue.  This queue will be
 * used to appropriately handle the received response message.
 *
 * @param[i] client
 * @param[i] resp_cb Callback to be called upon response receipt
 * @param[i] resp_cb_data Optional callback to be delivered
 *
 * @return Newly create request objct
 */
static sns_request*
add_request(sns_client *client, sns_client_resp resp_cb,
    void *resp_cb_data)
{
  sns_request *request;

  request = sns_malloc(sizeof(*request));
  SNS_ASSERT(NULL != request);

  request->resp_cb = resp_cb;
  request->resp_cb_data = resp_cb_data;

  sns_osa_mutex_lock(qsocket_state->mutex);
  request->request_id = next_request_id++;

  request->next = client->requests;
  client->requests = request;
  sns_osa_mutex_unlock(qsocket_state->mutex);

  return request;
}

/**
 * Handle an incoming response message by decoding the PB message, and
 * delivering the contained error code to the client.
 *
 * @param[i] client
 * @param[i] msg Encoded message of type sns_client_qsocket_resp
 * @param[i] msg_len
 */
static void
handle_resp(sns_client *client, void *msg, uint32_t msg_len)
{
  sns_client_qsocket_resp resp = sns_client_qsocket_resp_init_default;
  pb_istream_t stream;

  stream = pb_istream_from_buffer(msg, msg_len);
  if(!pb_decode(&stream, sns_client_qsocket_resp_fields, &resp))
  {
    SNS_LOG(ERROR, "Decoding failed");
  }
  else
  {
    sns_request *request = get_request(client, resp.request_id);

    if(NULL != request)
    {
      if(NULL != request->resp_cb)
        request->resp_cb(client, resp.error, request->resp_cb_data);
      sns_free(request);
    }
  }
}

/**
 * Process and handle a message received from a QSocket.
 *
 * @param[i] msg Encoded message of type sns_client_qsocket_msg
 */
static void
handle_msg(sns_client *client, void *msg, uint32_t msg_len)
{
  pb_istream_t stream;
  pb_wire_type_t wire_type;
  bool eof = false;
  uint32_t tag;

  stream = pb_istream_from_buffer(msg, msg_len);
  while(pb_decode_tag(&stream, &wire_type, &tag, &eof) && !eof)
  {
    if(PB_WT_STRING == wire_type && sns_client_qsocket_msg_ind_tag == tag)
    {
      pb_istream_t sub_stream;
      if(pb_make_string_substream(&stream, &sub_stream))
      {
        if(NULL != client->ind_cb)
          handle_ind(client, sub_stream.state, sub_stream.bytes_left);
      }
      break;
    }
    else if(PB_WT_STRING == wire_type && sns_client_qsocket_msg_resp_tag == tag)
    {
      pb_istream_t sub_stream;
      if(pb_make_string_substream(&stream, &sub_stream))
      {
        handle_resp(client, sub_stream.state, sub_stream.bytes_left);
      }
      break;
    }
    pb_skip_field(&stream, wire_type);
  }
}

/**
 * Lookup a client from its socket descriptor.  Return NULL if no matching
 * client is found.
 *
 * @param[i] remove Remove the client if found
 */
static sns_client*
lookup_client(sns_qsocket_state *state, int const fd, bool remove)
{
  sns_client *last = NULL, *client = state->clients;
  while(NULL != client)
  {
    if(client->fd == fd)
    {
      if(remove)
      {
        if(NULL == last)
          state->clients = client->next;
        else
          last->next = client->next;
      }
      break;
    }
    last = client;
    client = client->next;
  }

  return client;
}

/**
 * Main client thread which waits on incoming messages on open qsocket(s).
 */
static void*
main_thread(void *arg)
{
  sns_qsocket_state *state = (sns_qsocket_state*)arg;

  SNS_LOG(VERBOSE, "main_thread");
  sns_osa_mutex_lock(state->mutex);

  for(;;)
  {
    int err;

    // Don't call qpoll unless we have at least one client
    while(NULL == state->clients)
    {
      sns_osa_mutex_unlock(state->mutex);
      sns_osa_sem_wait(state->sem);
      sns_osa_mutex_lock(state->mutex);
    }

    if(NULL != state->new_fds)
    {
      sns_free(state->fds);
      state->fds = state->new_fds;
      state->fds_len = state->new_fds_len;

      state->new_fds = NULL;
      state->new_fds_len = 0;
    }

    sns_osa_mutex_unlock(state->mutex);
    err = sns_qpoll(state->fds, state->fds_len, -1);
    sns_osa_mutex_lock(state->mutex);

    if(err <= 0)
    {
      SNS_LOG(WARN, "qpoll error %i", err);
      continue;
    }

    for(size_t i = 0; i < state->fds_len; i++)
    {
      if(state->fds[i].revents & QPOLLIN)
      {
        int size;
        void *msg;
        sns_client *client = lookup_client(state, state->fds[i].fd, false);

        if(NULL != client)
        {
          size = sns_qrecv(state->fds[i].fd, NULL, 0, QMSG_PEEK|QMSG_DONTWAIT);
          SNS_ASSERT(size > 0);

          msg = sns_malloc(size);
          SNS_ASSERT(NULL != msg);

          size = sns_qrecv(state->fds[i].fd, msg, size, QMSG_DONTWAIT);
          SNS_ASSERT(size > 0);

          handle_msg(client, msg, size);

          sns_free(msg);
        }
        else
        {
          SNS_LOG(VERBOSE, "Unknown client %i", state->fds[i].fd);
        }
        state->fds[i].revents &= ~(QPOLLIN);
      }
      if(state->fds[i].revents & QPOLLHUP)
      {
        sns_client *client = lookup_client(state, state->fds[i].fd, false);
        SNS_ASSERT(NULL != client);

        if(NULL != client->error_cb)
          client->error_cb(client, SNS_STD_ERROR_INVALID_STATE,
            client->error_cb_data);
        state->fds[i].revents &= ~(QPOLLHUP);
      }
      if(0 != state->fds[i].revents)
      {
        SNS_LOG(WARN, "Unknown poll event %x", state->fds[i].revents);
      }
    }
  }

  return NULL;
}

/**
 * Lookup the Sensor Service address.
 *
 * @return 0 upon success; <0 upon failure
 */
static int
lookup_server_addr(uint32_t timeout, struct qsockaddr_ipcr *srv_addr)
{
  int rv = 0;
  int ctrl_fd, rc = 0;
  struct qpollfd pfd;
  ipcr_name_t name = (ipcr_name_t)
      { .service = QSOCKET_SNS_CLIENT_SVC_ID, .instance = 0 };

  ctrl_fd = sns_qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);
  if(ctrl_fd < 0)
  {
    SNS_LOG(ERROR, "Control FD creation failed rc %i", ctrl_fd);
    rv = -1;
  }
  else
  {
    /* Mark this as a control socket to listen on events on the name server */
    /*rc = qsetsockopt(ctrl_fd, QSOL_IPC_ROUTER, QSO_IPCR_SET_CONTROL_PORT, NULL, 0);
    if(rc)
    {
      SNS_LOG(ERROR, "Setting socket option failed with rc %i", rc);
      rv = -1;
    }
    rc = qsetsockopt(ctrl_fd, QSOL_IPC_ROUTER, QSO_IPCR_SET_SERVICE_PREF,
        &name.service, sizeof(name.service));
    if(rc)
    {
      SNS_LOG(ERROR, "Control FD creation failed rc %i", ctrl_fd);
      rv = -1;
    }*/

    pfd.fd = ctrl_fd;
    pfd.events = QPOLLIN;
    pfd.revents = 0;

    while(0 == rv)
    {
      ipcr_ctrl_msg msg;
      unsigned int num_entries = 1;
      rc = sns_ipcr_find_name(ctrl_fd, &name, srv_addr, NULL,
          &num_entries, IPCR_FLAGS_ANY_INSTANCE);

      if(rc > 0)
      {
        SNS_LOG(ERROR, "Got service with addr <%" PRIu32 ",%" PRIu32 ">",
            srv_addr->address.addr.port_addr.node_id,
            srv_addr->address.addr.port_addr.port_id);
        break;
      }

      rc = sns_qpoll(&pfd, 1, timeout);
      if(rc < 0)
      {
        SNS_LOG(ERROR, "poll returned error %i", rc);
        rv = -1;
        break;
      }
      else if(rc == 0)
      {
        SNS_LOG(ERROR, "poll timed-out");
        rv = -1;
        break;
      }
      else
      {
        while(sns_qrecv(ctrl_fd, &msg, sizeof(msg), QMSG_DONTWAIT) > 0)
        { } /* Do nothing */
      }
    }
    sns_qclose(ctrl_fd);
  }

  return rv;
}

/**
 * Initialize all global data structures.  Assumes single-threaded client
 * at least until sns_qsocket_client_init.
 */
static void
one_time_init(void)
{
  if(NULL == qsocket_state)
  {
    int err;

    qsocket_state = sns_malloc(sizeof(*qsocket_state));
    SNS_ASSERT(NULL != qsocket_state);

    qsocket_state->fds = NULL;
    qsocket_state->fds_len = 0;
    qsocket_state->new_fds = NULL;
    qsocket_state->new_fds_len = 0;

    qsocket_state->clients = NULL;

    err = sns_osa_mutex_create(&qsocket_state->mutex);
    SNS_ASSERT(0 == err);

    err = sns_osa_sem_create(&qsocket_state->sem);
    SNS_ASSERT(0 == err);

    err = sns_osa_thread_create(&qsocket_state->thread, &main_thread, qsocket_state);
    SNS_ASSERT(0 == err);
  }
}

/**
 * Send an already encoded sns_client_qsocket_msg to the connected service, and
 * perform all necessary book-keeping.
 *
 * @return
 *  0 Upon success
 * -2 Upon qsend failure
 */
static int
send_encoded_msg(sns_client *client, void *buffer, size_t buffer_len)
{
  int rv = 0;
  int err;

  for(int i = 0; i < QSOCKET_SEND_RETRY_CNT; i++)
  {
    sns_osa_mutex_lock(qsocket_state->mutex);
    err = sns_qsend(client->fd, buffer, buffer_len, 0);
    if(err < 0)
      SNS_LOG(VERBOSE, "qsend error %i (fd %i), try %d", err, client->fd, i);

    rv = (err < 0) ? -2 : 0;
    sns_osa_mutex_unlock(qsocket_state->mutex);

    if(QEWOULDBLOCK == err || QENOMEM == err)
    {
      uint64_t ticks = uTimetick_CvtToTicks(QSOCKET_SEND_RETRY_WAIT_US, T_USEC);

      // PEND: Island exit or switch to utimer_sleep when available
      timer_sleep(ticks, T_TICK, TRUE);
      continue;
    }
    break;
  }

  if(err < 0)
    SNS_LOG(ERROR, "qsend error %i (fd %i)", err, client->fd);

  return rv;
}

/**
 * Send a no-op request to any existing client.  This request should prompt a
 * response message by SEE, so that the client thread will be awoken to process
 * the response.
 *
 * @note For whatever reason, encoding the message using pb_encode would not
 * work, therefore needed to encode it manually.
 */
static void
send_noop(sns_client *client)
{
  sns_client_qsocket_resp resp = sns_client_qsocket_resp_init_default;
  int err;
  pb_ostream_t stream;
  uint8_t buf[sns_client_qsocket_resp_size + 5];
  sns_request *request;

  stream = pb_ostream_from_buffer(buf, sizeof(buf));
  if(!pb_encode_tag(&stream, PB_WT_STRING, sns_client_qsocket_msg_resp_tag))
    SNS_LOG(ERROR, "Error pb_encode_tag: %s", PB_GET_ERROR(&stream));
  else if(!pb_encode_submessage(&stream, sns_client_qsocket_resp_fields, &resp))
    SNS_LOG(ERROR, "Error pb_encode_submessage: %s", PB_GET_ERROR(&stream));

  request = add_request(client, NULL, NULL);
  err = send_encoded_msg(client, buf, stream.bytes_written);
  if(0 != err)
  {
    SNS_LOG(ERROR, "encode_and_send error %i", err);
    get_request(client, request->request_id);
  }
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

int
sns_client_init(sns_client **client_out,
    uint32_t timeout, sns_client_ind ind_cb, void *ind_cb_data,
    sns_client_error error_cb, void *error_cb_data)
{
  struct qsockaddr_ipcr addr;
  sns_client *client = NULL;
  int err;
  int rv = 0;

  one_time_init();

  err = lookup_server_addr(timeout, &addr);
  if(0 != err)
  {
    SNS_LOG(ERROR, "lookup_server_addr error %i", err);
    rv = -1;
  }
  else
  {
    sns_osa_mutex_lock(qsocket_state->mutex);
    client = sns_malloc(sizeof(*client));
    SNS_ASSERT(NULL != client);

    client->next = qsocket_state->clients;
    qsocket_state->clients = client;

    client->ind_cb = ind_cb;
    client->ind_cb_data = ind_cb_data;
    client->error_cb = error_cb;
    client->error_cb_data = error_cb_data;
    client->requests = NULL;

    client->fd = sns_qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);
    SNS_ASSERT(client->fd >= 0);

    err = sns_qconnect(client->fd, (struct qsockaddr*)&addr, sizeof(addr));
    if(0 != err)
    {
      SNS_LOG(ERROR, "qconnect error %i", err);
      rv = -1;
    }
    else
    {
      struct qpollfd *source_fds = NULL;
      size_t start_len = 0;
      struct qpollfd *new_fds = NULL;

      if(NULL == qsocket_state->new_fds)
      {
        source_fds = qsocket_state->fds;
        start_len = qsocket_state->fds_len;
      }
      else
      {
        source_fds = qsocket_state->new_fds;
        start_len = qsocket_state->new_fds_len;
      }

      new_fds = sns_malloc(sizeof(struct qpollfd) * (start_len + 1));
      SNS_ASSERT(NULL != new_fds);

      if(NULL != source_fds)
        memscpy(new_fds, sizeof(struct qpollfd) * (start_len + 1),
            source_fds, sizeof(struct qpollfd) * start_len);

      new_fds[start_len].fd  = client->fd;
      new_fds[start_len].events = QPOLLIN;
      new_fds[start_len].revents = 0;
      qsocket_state->new_fds_len = start_len + 1;

      sns_free(qsocket_state->new_fds);
      qsocket_state->new_fds = new_fds;

      if(0 == start_len)
        sns_osa_sem_post(qsocket_state->sem);
      else
        send_noop(qsocket_state->clients->next);

      SNS_LOG(VERBOSE, "sns_qsocket_client_init success (fd %i)", client->fd);
    }
    sns_osa_mutex_unlock(qsocket_state->mutex);
  }

  if(0 != rv && NULL != client)
  {
    sns_free(client);
    client = NULL;
  }

  *client_out = client;
  return rv;
}

int
sns_client_deinit(sns_client *client)
{
  int rv = 0;
  int err;
  sns_request *request;
  struct qpollfd *source_fds = NULL;
  size_t start_len = 0;
  struct qpollfd *new_fds = NULL;

  sns_osa_mutex_lock(qsocket_state->mutex);

  if(NULL == qsocket_state->new_fds)
  {
    source_fds = qsocket_state->fds;
    start_len = qsocket_state->fds_len;
  }
  else
  {
    source_fds = qsocket_state->new_fds;
    start_len = qsocket_state->new_fds_len;
  }

  client = lookup_client(qsocket_state, client->fd, true);
  SNS_ASSERT(NULL != client);

  if(start_len > 1)
  {
    new_fds = sns_malloc(sizeof(struct qpollfd) * (start_len - 1));
    SNS_ASSERT(NULL != new_fds);

    if(client->fd != source_fds.fd)
    {
      if(client->fd != qsocket_state->fds[i].fd)
        memscpy(&new_fds[j++], sizeof(struct qpollfd),
            &source_fds[i], sizeof(struct qpollfd));
    }
  }
  qsocket_state->new_fds_len = start_len - 1;
  sns_free(qsocket_state->new_fds);
  qsocket_state->new_fds = new_fds;

  request = client->requests;
  while(NULL != request)
  {
    sns_request *temp = request;
    request = request->next;
    sns_free(temp);
  }
  client->requests = NULL;

  err = sns_qclose(client->fd);
  if(0 != err)
  {
    SNS_LOG(ERROR, "qclose error %i", err);
    rv = -1;
  }

  SNS_LOG(VERBOSE, "qclose complete");
  free(client);

  sns_osa_mutex_unlock(qsocket_state->mutex);
  return rv;
}

int
sns_client_send(sns_client *client, sns_client_request_msg *msg,
    sns_client_resp resp_cb, void *resp_cb_data)
{
  sns_client_qsocket_msg message = sns_client_qsocket_msg_init_default;
  size_t encoded_len;
  int rv = -1;
  sns_request *request;

  request = add_request(client, resp_cb, resp_cb_data);
  message.which_msg = sns_client_qsocket_msg_req_tag;
  message.msg.req.request = *msg;
  message.msg.req.request_id = request->request_id;

  if(!pb_get_encoded_size(&encoded_len,
        sns_client_qsocket_msg_fields, &message))
  {
    SNS_LOG(ERROR, "pb_get_encoded_size error");
  }
  else
  {
    void *encoded_msg = sns_malloc(encoded_len);
    pb_ostream_t stream;
    SNS_ASSERT(NULL != encoded_msg);

    stream = pb_ostream_from_buffer(encoded_msg, encoded_len);
    if(!pb_encode(&stream, sns_client_qsocket_msg_fields, &message))
    {
      SNS_LOG(ERROR, "pb_encode error: %s", PB_GET_ERROR(&stream));
    }
    else
    {
      rv = send_encoded_msg(client, encoded_msg, stream.bytes_written);
    }
    sns_free(encoded_msg);
  }

  if(0 != rv)
    get_request(client, request->request_id);

  return rv;
}
