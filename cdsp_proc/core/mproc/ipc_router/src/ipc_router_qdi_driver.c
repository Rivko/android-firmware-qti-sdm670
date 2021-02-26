/*===========================================================================

                    I P C    R O U T E R   Q D I

   This file describes the QDI Driver implementation of IPC Router.

  ---------------------------------------------------------------------------
  Copyright (c) 2012-2016, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.qdsp6/2.1.c4/mproc/ipc_router/src/ipc_router_qdi_driver.c#2 $
$DateTime: 2019/03/05 03:25:43 $
$Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/12/18   rv      Avoiding sending rx buf to user pd if client is closed
05/09/12   aep     Initial Creation
==========================================================================*/

/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include <stddef.h>
#include "qurt_qdi_driver.h"
#include "ipc_router_core.h"
#include "ipc_router_database.h"
#include "ipc_router_internal.h"
#include "ipc_router_qdi_driver.h"
#include "qurt_signal.h"
#include "timer.h"
#include "smem_log.h"
#include "msg.h"

/* TODO Enable when testing out PD restart */
//#define PD_RESTART_SUPPORT

/*===========================================================================
                  CONSTANT / MACRO DACLARATIONS
===========================================================================*/
/* Initialize a waiter structure and add to 
 * global list. waiter_list_lock must be held by caller */
#define INIT_WAITER(WAITER, PFD, NUM, timeout_ms, ADDR) \
do {  \
  (WAITER).pfd = PFD; \
  (WAITER).num = NUM; \
  (WAITER).uid = waiter_next_uid++;  \
  qurt_signal_init(&((WAITER).signal)); \
  LINK_INIT((WAITER).link); \
  (WAITER).addr = ADDR; \
  if((timeout_ms) > 0) \
  { \
    timer_def_osal(&((WAITER).timer), NULL, TIMER_FUNC1_CB_TYPE,  \
                    notify_waiters_timeout, (timer_cb_data_type)((WAITER).uid)); \
    timer_set_64(&((WAITER).timer), timeout_ms, 0, T_MSEC); \
  } \
  LIST_ADD(g_waiter_list, &(WAITER), link); \
} while(0)

/* remove waiter from global list and deinitializes it.
 * waiter_list_lock must be held by the caller */
#define DEINIT_WAITER(waiter, timeout_ms)  \
do {  \
  LIST_REMOVE(g_waiter_list, &(waiter), link);  \
  if((timeout_ms) > 0) \
    timer_undef(&((waiter).timer)); \
  qurt_signal_destroy(&((waiter).signal)); \
} while(0)

/* Used for timer operations */
#define TIMEOUT_SIG    0x80000000

/* Connection status of a port */
#define CONN_STATUS_DISCONNECTED  -1
#define CONN_STATUS_UNCONNECTED    0
#define CONN_STATUS_CONNECTED      1
#define CONN_STATUS_CONNECTED_NAME 2

#ifndef UINT32_MAX
#define UINT32_MAX  (0XFFFFFFFFUL)
#endif

/*===========================================================================
                      FORWARD TYPE DECLARATIONS
===========================================================================*/
typedef struct data_buf_s data_buf_t;
typedef struct event_buf_s event_buf_t;
typedef struct ipc_router_user_ep_s ipc_router_user_ep_t;
typedef struct ipc_router_qdi_handle_s ipc_router_qdi_handle_t;
typedef struct ipc_router_qdi_waiter_s ipc_router_qdi_waiter_t;
typedef struct ipc_router_qdi_name_s   ipc_router_qdi_name_t;

/*===========================================================================
                        TYPE DECLARATIONS
===========================================================================*/

/** Data buffer type (Queue entry) */
struct data_buf_s {
  ipc_router_user_ep_t    *user;
  ipc_router_packet_type  *pkt;
  uint32                  len;
  ipc_router_address_type src_addr;
  uint32                  confirm_rx;
  LINK(data_buf_t, link);
};

/** Event buffer type (Queue entry) */
struct event_buf_s {
  void                       *cookie;
  uint32                     event;
  ipc_router_event_info_type info;
  LINK(event_buf_t, link);
};

typedef struct {
  int local;
  int remote;
} ipc_router_qdi_sig_type;


/** QDI Device handle (One for each client process) */
struct ipc_router_qdi_handle_s 
{
  qurt_qdi_obj_t            obj; /* Has to be the first element */
  int                       qdi_handle;

  ipc_router_os_sem event_q_lock;
  LIST(event_buf_t, event_q);
  ipc_router_os_sem rx_q_lock;
  LIST(data_buf_t, rx_q);

  int recv_timeout;

  struct {
    int valid;
    ipc_router_qdi_sig_type group;
    ipc_router_qdi_sig_type event;
    ipc_router_qdi_sig_type data;
  } signal;

  int                       active_receivers;

  boolean                   closing_client_handles;
  
  ipc_router_os_sem         clients_lock;
  LIST(ipc_router_user_ep_t, clients);
  int                       next_fd;
  LINK(ipc_router_qdi_handle_t, link);
};

/** QDI User Endpoint handle (One for each endpoint (Router client)) */
struct ipc_router_user_ep_s 
{
  ipc_router_client_type  *handle;
  ipc_router_qdi_handle_t *dev;
  int                     fd;
  uint32_t                allowed_flag; /* See *_ALLOWED */

  int send_timeout;
  int ctrl_port;

  struct
  {
    int status; /* See CONN_STATUS_* */
    union
    {
      ipc_router_address_type addr;
      ipc_router_service_type name;
    } val;
  } conn;
  void                    *cookie;
  unsigned int            ref_count;
  LINK(ipc_router_user_ep_t, link);
};

struct ipc_router_qdi_waiter_s
{
  unsigned int            uid;
  LINK(ipc_router_qdi_waiter_t, link);
  ipc_router_qdi_poll_fd  *pfd;
  uint32_t                num;
  timer_type              timer;
  qurt_signal_t           signal;
  ipc_router_address_type *addr;
};


struct ipc_router_qdi_name_s
{
  LINK(ipc_router_qdi_name_t, link);
  ipc_router_address_type     addr;
  char                        name[32];
};

/*===========================================================================
                        GLOBAL VARIABLES
===========================================================================*/
/* To ease debugging with crash dumps */
static ipc_router_os_sem active_handles_lock;
static LIST(ipc_router_qdi_handle_t, g_active_handles);

static LIST(ipc_router_qdi_waiter_t, g_waiter_list);
static ipc_router_os_sem waiter_list_lock;
static unsigned int waiter_next_uid = 1;

static LIST(ipc_router_qdi_name_t, g_name_list);
static ipc_router_os_sem name_list_lock;

/*===========================================================================
                        LOCAL FUNCTIONS
===========================================================================*/


/*===========================================================================
FUNCTION      notify_waiters

DESCRIPTION   Notify all threads waiting on fd

ARGUMENTS     fd - File descriptor (qdi_handle)
              event - Event which occured (IPC_QDI_POLL*)
              addr - if the event is associated with an address, the pointer.

RETURN VALUE  num unblocked waiters

SIDE EFFECTS  None
===========================================================================*/
static int notify_waiters(int fd, int event, ipc_router_address_type *addr)
{
  int num = 0;
  ipc_router_qdi_waiter_t *w;
  ipc_router_os_sem_lock(&waiter_list_lock);
  w = LIST_HEAD(g_waiter_list);
  while(w)
  {
    int i;
    for(i = 0; i < w->num; i++)
    {
      if(w->pfd[i].fd == fd)
      {
        if(w->pfd[i].events & event)
        {
          if(addr && w->addr && memcmp(w->addr, addr, sizeof(*addr)) != 0)
          {
            break;
          }
          qurt_signal_set(&w->signal, event);
          num++;
        }
        break;
      }
    }
    w = w->link.next;
  }
  ipc_router_os_sem_unlock(&waiter_list_lock);
  return num;
}

/*===========================================================================
function      notify_waiters_timeout

description   Notify waiter upon timeout (Called by the timer subsystem)

arguments     data - Cookie passed into timer_def_osal() -- the waiter uid.

return value  none

side effects  none
===========================================================================*/
static void notify_waiters_timeout(timer_cb_data_type data)
{
  ipc_router_qdi_waiter_t *w;
  unsigned int uid = (unsigned int)data;

  ipc_router_os_sem_lock(&waiter_list_lock);
  LIST_FIND(g_waiter_list, w, link, w->uid == uid);
  if(w)
  {
    qurt_signal_set(&w->signal, TIMEOUT_SIG);
  }
  ipc_router_os_sem_unlock(&waiter_list_lock);
}

/*===========================================================================
FUNCTION      Disconnected_users

DESCRIPTION   Return the number of disconnected users

ARGUMENTS     handle - the device handle

RETURN VALUE  Number of disconnected users

SIDE EFFECTS  None
===========================================================================*/
static int disconnected_users(ipc_router_qdi_handle_t *handle)
{
  int cnt = 0;
  ipc_router_user_ep_t *c;

  ipc_router_os_sem_lock(&handle->clients_lock);
  c = LIST_HEAD(handle->clients);
  while(c)
  {
    if(c->conn.status == CONN_STATUS_DISCONNECTED)
    {
      cnt++;
    }
    c = c->link.next;
  }
  ipc_router_os_sem_unlock(&handle->clients_lock);
  return cnt;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_get_obj

DESCRIPTION   Search through the device's (handle) endpoint list and returns
              the object associated with the particular endpoint file 
              descriptor. 

ARGUMENTS     handle - Handle associated with this endpoint
              fd - The endpoint's file descriptor. 

RETURN VALUE  EP Handle if successful (Found) NULL otherwise.

SIDE EFFECTS  The endpoint's reference count is incremented upon success.

FUTURE OPT.    This can be optimized to be a search in a hash table 
              (Rather than a list to reduce the average search time)
===========================================================================*/
static ipc_router_user_ep_t *ipc_router_qdi_get_obj(ipc_router_qdi_handle_t *handle, int fd)
{
  ipc_router_user_ep_t *ep = NULL;
  ipc_router_os_sem_lock(&handle->clients_lock);
  LIST_FIND(handle->clients, ep, link, ep->fd == fd);
  if(ep)
  {
    ep->ref_count++;
  }
  ipc_router_os_sem_unlock(&handle->clients_lock);
  return ep;
}

/*===========================================================================
FUNCTION      cleanup_client_queues

DESCRIPTION   Removes all data and event packets which are destined to a
              particular endpoint.

ARGUMENTS     handle - Handle associated with this endpoint

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void cleanup_client_queues(ipc_router_user_ep_t *handle)
{
  ipc_router_qdi_handle_t *dev = handle->dev;
  ipc_router_address_type addr;
  data_buf_t *data_buf;
  ipc_router_qdi_name_t *entry;

  /* Cleanup all the names associated with the port */
  addr = handle->handle->addr;

  ipc_router_os_sem_lock(&name_list_lock);
  entry = LIST_HEAD(g_name_list);
  while(entry)
  {
    ipc_router_qdi_name_t *to_free = entry;
    entry = entry->link.next;
    if(memcmp(&to_free->addr, &addr, sizeof(addr)) == 0)
    {
      LIST_REMOVE(g_name_list, to_free, link);
      ipc_router_os_free(to_free);
    }
  }
  ipc_router_os_sem_unlock(&name_list_lock);


  /* Remove all queued packets */
  ipc_router_os_sem_lock(&dev->rx_q_lock);
  data_buf = LIST_HEAD(dev->rx_q);
  while(data_buf)
  {
    data_buf_t *to_free = data_buf;
    data_buf = data_buf->link.next;
    if(to_free->user->fd == handle->fd)
    {
      LIST_REMOVE(dev->rx_q, to_free, link);
      ipc_router_packet_free(&to_free->pkt);
      ipc_router_os_free(to_free);
    }
  }
  ipc_router_os_sem_unlock(&dev->rx_q_lock);

  /* Events (in dev->event_q) are not cleaned since we also queue
   * up the CLOSED event. Removing it might cause mem leaks
   * in the user PD */
}

/*===========================================================================
FUNCTION      ipc_router_qdi_put_obj

DESCRIPTION   Decrements the reference count, and releases the endpoint
              if the reference is 0.

ARGUMENTS     fd - The endpoint's handle.

RETURN VALUE  None

SIDE EFFECTS  None

FUTURE OPT.   This can be optimized to be a search in a hash table 
             (Rather than a list to reduce the average search time)
===========================================================================*/
static void ipc_router_qdi_put_obj(ipc_router_user_ep_t *ep)
{
  ipc_router_qdi_handle_t *handle = NULL;

  if(!ep || !ep->dev)
  {
    return;
  }

  handle = ep->dev;

  ipc_router_os_sem_lock(&handle->clients_lock);
  ep->ref_count--;
  
  if(ep->ref_count == 0)
  {
    LIST_REMOVE(handle->clients, ep, link);
    ipc_router_os_sem_unlock(&handle->clients_lock);
    cleanup_client_queues(ep);
    ep->handle = NULL;
    ipc_router_os_free(ep);
  }
  else
  {
    ipc_router_os_sem_unlock(&handle->clients_lock);
  }
}

/*===========================================================================
FUNCTION      enqueue_data_packet

DESCRIPTION   Enqueues a packet to the device's RX queue

ARGUMENTS     user - User handle
              src_addr - Src addr
              pkt - incoming packet
              len - size of the incoming packet
              confirm_rx - does the sender require conf?

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void enqueue_data_packet
(
  ipc_router_user_ep_t *user,
  ipc_router_address_type *src_addr,
  ipc_router_packet_type *pkt,
  uint32 len,
  boolean confirm_rx
)
{
  data_buf_t *buf = ipc_router_os_calloc(1, sizeof(*buf));
  if(!buf)
  {
    /* TODO Log error */
    if(pkt)
    {
      ipc_router_packet_free(&pkt);
    }
    return;
  }

  LINK_INIT(buf->link);
  buf->user = user;
  if(src_addr)
  {
    buf->src_addr = *src_addr;
  }
  else
  {
    buf->src_addr.processor_id = buf->src_addr.port_id = 0xffffffff;
  }
  buf->confirm_rx = (uint32)confirm_rx;
  buf->len = len;
  buf->pkt = pkt;

  ipc_router_os_sem_lock(&user->dev->rx_q_lock);
  LIST_ADD(user->dev->rx_q, buf, link);
  ipc_router_os_sem_unlock(&user->dev->rx_q_lock);

  if(user->dev->signal.valid)
    qurt_qdi_signal_set(user->dev->signal.data.local);
  notify_waiters(user->dev->qdi_handle, IPC_QDI_POLLIN, NULL);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_event_cb

DESCRIPTION   Callback function called by IPC Router Core when there is a
              new event for an endpoint.

ARGUMENTS     unused -the router core client handle (Not used)
              cb_data - the cookie provided on open (the endpoint handle)
              event - the event
              info - the data associated with the event.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_event_cb
(
    ipc_router_client_type *unused,
    void *cb_data,
    ipc_router_event_type event,
    ipc_router_event_info_type *info)
{
  ipc_router_user_ep_t *clnt = (ipc_router_user_ep_t *)cb_data;
  ipc_router_qdi_handle_t *handle = NULL;
  void *cookie = NULL;
  uint32_t allowed_flag = 0;
  int ctrl_port = 0;

  if(!clnt || !clnt->dev)
  {
    /* TODO Log error */
    return;
  }

  /* Cache info */
  handle = clnt->dev;
  cookie = clnt->cookie;
  allowed_flag = clnt->allowed_flag;
  ctrl_port = clnt->ctrl_port;

  switch(event)
  {
    case IPC_ROUTER_EVENT_CLOSED:
      {
        int qdi_handle;
        int active_receivers;
        boolean closing_client_handles;
        uint32 sig_mask =  IPC_QDI_POLLNVAL | IPC_QDI_POLLOUT;
        
        /* Cache info */
        qdi_handle = handle->qdi_handle;
        closing_client_handles = handle->closing_client_handles;
        
        ipc_router_os_sem_lock(&handle->clients_lock);
        active_receivers = --handle->active_receivers;
        
        /* Disable RX and TX */
        clnt->allowed_flag &= ~(DATA_ALLOWED | TRANSMIT_ALLOWED);
        
        ipc_router_os_sem_unlock(&handle->clients_lock);

        /* IPC Router Core releases the reference. reason
         * we cache info at the start since this call might
         * free clnt */
        ipc_router_qdi_put_obj(clnt);

        /* set event POLLOUT so potentially blocked writers to
         * this user can unblock, notice the TRANSMIT_ALLOWED is
         * cleared, and return error while releasing their refcount. 
         * set event POLLIN if this was the last listener on the fd
         * so potential blocked receivers can unblock and return error
         * after observing that the active_receivers has gone to 0 */
        if(active_receivers <= 0)
        {
          sig_mask |= IPC_QDI_POLLIN;
        }
        notify_waiters(qdi_handle, sig_mask, NULL);
        
        if (closing_client_handles == TRUE)
        {
          /* QDI handle might be already freed */
          return;
        }
      }
      break;
    case IPC_ROUTER_EVENT_RESUME_TX:
      if(notify_waiters(handle->qdi_handle, IPC_QDI_POLLOUT, &info->client) == 0)
      {
        if(0 == (allowed_flag & EVENT_ALLOWED))
        {
          /* Events are not allowed on this fd and no active waiters notified. 
             Queue in an 0 length data packet so the user may handle it as a resume */
          enqueue_data_packet(clnt, &info->client, NULL, 0, 0);
          return;
        }
      }
      break;
    case IPC_ROUTER_EVENT_REMOVE_CLIENT:
      if(clnt->conn.status == CONN_STATUS_CONNECTED &&
          memcmp(&info->client, &clnt->conn.val.addr, sizeof(info->client)) == 0)
      {
        memset(&clnt->conn.val.addr, 0, sizeof(info->client));
        clnt->conn.status = CONN_STATUS_DISCONNECTED;
        notify_waiters(handle->qdi_handle, IPC_QDI_POLLOUT | IPC_QDI_POLLIN, &info->client);
        return;
      }
      break;
    case IPC_ROUTER_EVENT_NEW_SERVER:
    case IPC_ROUTER_EVENT_REMOVE_SERVER:
      if(clnt->conn.status == CONN_STATUS_CONNECTED_NAME &&
          clnt->conn.val.name.type != info->server.service.type)
      {
        /* Port is not interested in this event */
        return;
      }
      break;
    default:
      break;
  }

  /* We will either put the packet in the data queue (ctrl port) or the
   * event queue (oob). Check if the port is even interested. */
  if(0 == (allowed_flag & EVENT_ALLOWED))
  {
    return;
  }

  if(!ctrl_port)
  {
    event_buf_t *buf = ipc_router_os_malloc(sizeof(*buf));
    if(!buf)
    {
      return;
    }
    LINK_INIT(buf->link);
    buf->event = (uint32)event;
    buf->cookie= cookie;
    buf->info  = *info;

    /* Queue up the event. */
    ipc_router_os_sem_lock(&handle->event_q_lock);
    LIST_ADD(handle->event_q, buf, link);
    ipc_router_os_sem_unlock(&handle->event_q_lock);

    if(handle->signal.valid)
      qurt_qdi_signal_set(handle->signal.event.local);
  }
  else
  {
    uint32_t ctrl_msg[5] = {0};
    ipc_router_packet_type *pkt = NULL;

    if(event == IPC_ROUTER_EVENT_NEW_SERVER || event == IPC_ROUTER_EVENT_REMOVE_SERVER)
    {
      ctrl_msg[0] = event == IPC_ROUTER_EVENT_NEW_SERVER ? 4 : 5;
      ctrl_msg[1] = info->server.service.type;
      ctrl_msg[2] = info->server.service.instance;
      ctrl_msg[3] = info->server.addr.processor_id;
      ctrl_msg[4] = info->server.addr.port_id;
    }
    else if(event == IPC_ROUTER_EVENT_REMOVE_CLIENT)
    {
      ctrl_msg[0] = 6;
      ctrl_msg[1] = info->client.processor_id;
      ctrl_msg[2] = info->client.port_id;
    }
    else
    {
      /* Do not bother queueing up other events like CLOSED */
      return;
    }
    if(ipc_router_packet_copy_header(&pkt, ctrl_msg, sizeof(ctrl_msg)) != sizeof(ctrl_msg))
    {
      /* TODO Log error */
      return;
    }
    enqueue_data_packet(clnt, NULL, pkt, sizeof(ctrl_msg), 0);
  }
}

/*===========================================================================
FUNCTION      ipc_router_qdi_rx_cb

DESCRIPTION   Callback function called from IPC Router Core when there is a
              packet to be read.

ARGUMENTS     client - The router core client.
              cb_data - the cookie provided on open (endpoint handle).
              msg - IPC router packet  type.
              src_addr - the address of the source.
              len - the length of the packet.
              confirm_rx - flag if true implies that the source is requesting
                           confirmation.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_rx_cb
(
    ipc_router_client_type *client,
    void *cb_data,
    ipc_router_packet_type *msg,
    ipc_router_address_type *src_addr,
    uint32 len,
    boolean confirm_rx)
{
  ipc_router_user_ep_t *clnt = (ipc_router_user_ep_t *)cb_data;

  if(!clnt || !clnt->dev || !(clnt->allowed_flag & DATA_ALLOWED) || clnt->ctrl_port)
  {
    ipc_router_packet_free(&msg);
    return;
  }

  enqueue_data_packet(clnt, src_addr, msg, len, confirm_rx);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_event_read_invocation

DESCRIPTION   Allows an endpoint to read a broadcasted event.

ARGUMENTS     client_handle - The calling client's handle
              handle - Handle for this device
              out_event - out event ID.
              out_info  - Out info of this event.
              out_cookie - The cookie associated with the endpoint.

RETURN VALUE  Negative error code on failure, positive bytes read into
              out_info. 0 if there are no events to be read.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_event_read_invocation
(
    int client_handle,
    ipc_router_qdi_handle_t *handle,
    uint32 *out_event,
    ipc_router_event_info_type *out_info,
    void *out_cookie
)
{
  event_buf_t *buf;
  int rc;

  if(!handle || !out_event || !out_info || !out_cookie)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  ipc_router_os_sem_lock(&handle->event_q_lock);
  buf = LIST_HEAD(handle->event_q);
  if(!buf)
  {
    ipc_router_os_sem_unlock(&handle->event_q_lock);
    return 0;
  }

  rc = qurt_qdi_copy_to_user(client_handle, out_event, &buf->event, sizeof(uint32));
  if(rc)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  rc = qurt_qdi_copy_to_user(client_handle, out_info, &buf->info, sizeof(ipc_router_event_info_type));
  if(rc)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  rc = qurt_qdi_copy_to_user(client_handle, out_cookie, &buf->cookie, sizeof(void *));
  if(rc)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  LIST_REMOVE(handle->event_q, buf, link);
  ipc_router_os_free(buf);
  rc = (int)sizeof(ipc_router_event_info_type);
bail:
  ipc_router_os_sem_unlock(&handle->event_q_lock);

  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_read_invocation

DESCRIPTION   returns a buffer sent to this endpoint.

ARGUMENTS     client_handle - Handle identifying the client
              handle - The handle for this device.
              out_buf - the buffer to place the packet
              in_max_len - the size of the provided buffer.
              out_addr - The source address**
              out_confirm_rx - flag if set requesting the endpoint to 
                              send a confirmation (send_conf)***
              out_len - Length of the buffer read (or in queue if 
                        IPC_ROUTER_QDI_RDFLAG_MSGPEEK set in flags)
              cookie - the cookie associated with this endpoint
                       (Provided by the client on  open)**
              flags - Read flags (See IPC_ROUTER_QDI_RDFLAG_* for more)

 ** - Optional. The value is not returned if the user provides NULL.
 *** - If the user does not provide a pointer to hold the confirm_rx flag,
       and the flag was set, the driver will send the confirmation upon read.
 **** - This function will never block if there are one or more disconnected
        users (Caller must free the handle which is disconnected)

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_read_invocation
(
    int client_handle,
    ipc_router_qdi_handle_t *handle,
    void *out_buf,
    uint32 in_max_len,
    ipc_router_address_type *out_addr,
    uint32 *out_confirm_rx,
    uint32 *out_len,
    void *cookie,
    uint32 flags
)
{
  data_buf_t *buf;
  int rc;
  void *return_buf = NULL;

  if(!handle || !out_len || (in_max_len > 0 && !out_buf))
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  /* Set dontwait if the flags is 0 and the user
     had set recv_timeout as 0 */
  if(flags == 0 && handle->recv_timeout == 0)
    flags = IPC_ROUTER_QDI_RDFLAG_DONTWAIT;

  if(!(flags & IPC_ROUTER_QDI_RDFLAG_DONTWAIT))
  {
    ipc_router_qdi_poll_fd pfd;
    ipc_router_qdi_waiter_t waiter;

    ipc_router_os_sem_lock(&waiter_list_lock);

    pfd.fd = handle->qdi_handle;
    pfd.events = IPC_QDI_POLLIN;
    pfd.revents = 0;

    INIT_WAITER(waiter, &pfd, 1, handle->recv_timeout, NULL);

    /* We have rx queue locks so we dont need this lock */
    ipc_router_os_sem_unlock(&waiter_list_lock);

    while(1)
    {
      uint32_t cnt = 0;
      qurt_signal_clear(&waiter.signal, IPC_QDI_POLLIN);
      ipc_router_os_sem_lock(&handle->rx_q_lock);
      cnt = LIST_CNT(handle->rx_q);
      ipc_router_os_sem_unlock(&handle->rx_q_lock);
      if(cnt > 0 || handle->active_receivers <= 0 || 
          disconnected_users(handle) > 0)
      {
        break;
      }
#ifdef PD_RESTART_SUPPORT
      rc = qurt_signal_wait_cancellable(&waiter.signal, IPC_QDI_POLLIN | TIMEOUT_SIG,
                                          QURT_SIGNAL_ATTR_WAIT_ANY, NULL);
      if(rc == QURT_ECANCEL)
      {
        break;
      }
#else
      qurt_signal_wait_any(&waiter.signal, IPC_QDI_POLLIN | TIMEOUT_SIG);
#endif
      if(qurt_signal_get(&waiter.signal) & TIMEOUT_SIG)
      {
        break;
      }
    }

    ipc_router_os_sem_lock(&waiter_list_lock);
    DEINIT_WAITER(waiter, handle->recv_timeout);
    ipc_router_os_sem_unlock(&waiter_list_lock);
  }

  /* Get write access to out_len */
  rc = qurt_qdi_lock_buffer(client_handle, out_len, sizeof(uint32), QDI_PERM_W, (void **)&out_len);
  if(rc || !out_len)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  ipc_router_os_sem_lock(&handle->rx_q_lock);
  buf = LIST_HEAD(handle->rx_q);

  /* check if the client related to buffer is allowed to receive data or not */
  while(buf && !(buf->user->allowed_flag & DATA_ALLOWED))
  {
    /* Remove the head buffer from the queue */
    LIST_REMOVE(handle->rx_q, buf, link);
    ipc_router_packet_free(&buf->pkt);
    ipc_router_os_free(buf);
    buf = LIST_HEAD(handle->rx_q);
  }

  /* Nothing to be read */
  if(!buf)
  {
    if(handle->active_receivers <= 0)
    {
      rc = IPC_ROUTER_QDI_BADF_ERR;
    }
    else if(disconnected_users(handle) > 0)
    {
      rc = IPC_ROUTER_QDI_HUNGUP_ERR;
    }
    else if(!(flags & IPC_ROUTER_QDI_RDFLAG_DONTWAIT))
    {
      rc = IPC_ROUTER_QDI_TIMEOUT_ERR;
    }
    else 
    {
      rc = IPC_ROUTER_QDI_NO_MSG_ERR;
    }
    goto bail;
  }

  /* Just return the length */
  if(flags & IPC_ROUTER_QDI_RDFLAG_MSGPEEK)
  {
    *out_len = buf->len;
    rc = IPC_ROUTER_STATUS_SUCCESS;
    goto bail;
  }

  if(buf->len > in_max_len && 0 == (flags & IPC_ROUTER_QDI_RDFLAG_TRUNC))
  {
    rc = IPC_ROUTER_QDI_NO_SPACE_ERR;
    goto bail;
  }

  if(in_max_len > 0 && out_buf != NULL)
  {
    rc = qurt_qdi_lock_buffer(client_handle, out_buf, in_max_len, QDI_PERM_W, (void **)&return_buf);
    if(rc || !return_buf)
    {
      rc = IPC_ROUTER_QDI_PERM_ERR;
      goto bail;
    }
  }

  if(out_addr && qurt_qdi_copy_to_user(client_handle, out_addr, &buf->src_addr, sizeof(ipc_router_address_type)) != 0)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  if(cookie && qurt_qdi_copy_to_user(client_handle, cookie, &buf->user->cookie, sizeof(void *)) != 0)
  {
    rc = IPC_ROUTER_QDI_PERM_ERR;
    goto bail;
  }

  if(out_confirm_rx)
  {
    /* If user wants to send the confirmation, let them  */
    rc = qurt_qdi_copy_to_user(client_handle, out_confirm_rx, &buf->confirm_rx, sizeof(uint32));
    if(rc)
    {
      rc = IPC_ROUTER_QDI_PERM_ERR;
      goto bail;
    }
  }
  else if(buf->confirm_rx)
  {
    /* If user chooses not to be the one to send confirmation (Does not receive conf_rx),
     * we should send it on their behalf on read */
    ipc_router_core_send_conf(buf->user->handle, &buf->src_addr);
  }

  *out_len = ipc_router_packet_read(&buf->pkt, return_buf, buf->len > in_max_len ? in_max_len : buf->len);
  /* Normally return the actual length read, but if trunc is specified, return the actual
   * buffer's length */
  if(flags & IPC_ROUTER_QDI_RDFLAG_TRUNC)
    *out_len = buf->len;

  /* Remove the head buffer from the queue */
  LIST_REMOVE(handle->rx_q, buf, link);
  ipc_router_packet_free(&buf->pkt);
  ipc_router_os_free(buf);
  rc = IPC_ROUTER_STATUS_SUCCESS;

bail:
  ipc_router_os_sem_unlock(&handle->rx_q_lock);
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_find_names_invocation

DESCRIPTION   Query the number and location of names of a particular
              name

ARGUMENTS     client_handle - Handle identifying the client
              handle - The handle of this endpoint.
              name - The name whose address we need to find
              name_len - The max length of the name string
              addrs - The return address 
              num_entries - (int) size of addrs, (out) filled
              num_servers - Total number of found services.

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_find_names_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    char *name,
    uint32 name_len,
    ipc_router_address_type *addrs,
    uint32 *num_entries, 
    uint32 *num_servers
)
{
  int rc;
  ipc_router_qdi_name_t *entry;
  uint32 addrs_len = 0;
  uint32 filled = 0;

  if(!num_servers || !name || !name_len || !num_entries)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  if(name_len > 32)
  {
    return IPC_ROUTER_QDI_NOT_SUPP_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, name, name_len, QDI_PERM_R, (void **)&name);
  if(rc || !name)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  /* Check for null termination */
  if(name[name_len - 1] != 0)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, num_entries, sizeof(uint32), QDI_PERM_RW, (void **)&num_entries);
  if(rc || !num_entries)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }
  addrs_len = *num_entries;

  rc = qurt_qdi_lock_buffer(client_handle, num_servers, sizeof(uint32), QDI_PERM_RW, (void **)&num_servers);
  if(rc || !num_servers)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  if(addrs)
  {
    /* uint32 overflow check for (sizeof(*addrs) * addrs_len) */
    if ((UINT32_MAX / sizeof(*addrs)) < addrs_len)
    {
      return IPC_ROUTER_QDI_PARAM_ERR;
    }
    
    rc = qurt_qdi_lock_buffer(client_handle, addrs, sizeof(*addrs) * addrs_len, QDI_PERM_W, (void **)&addrs);
    if(rc || !addrs)
    {
      return IPC_ROUTER_QDI_PERM_ERR;
    }
  }
  else
  {
    addrs_len = 0;
  }

  ipc_router_os_sem_lock(&name_list_lock);
  entry = LIST_HEAD(g_name_list);
  while(entry)
  {
    if(memcmp(name, entry->name, name_len) == 0)
    {
      *num_servers += 1;
      if(filled < addrs_len)
      {
        addrs[filled] = entry->addr;
        filled++;
      }
    }
    entry = entry->link.next;
  }
  ipc_router_os_sem_unlock(&name_list_lock);
  *num_entries = filled;
  return 0;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_find_servers_invocation

DESCRIPTION   Query the number and location of services of a particular
              type with (instance & mask) being the same.

ARGUMENTS     client_handle - Handle identifying the client
              handle - The handle of this endpoint.
              service - The service discretion (ID and Instance).
              servers - (Optional) pointers to place the address of the found servers.
              num_entries - (Optional) In: Number of servers the above array can hold
                                       Out: Total populated servers.
              num_servers - Total number of found services.
              mask - The mask to be used during compare operations on instance ID.
                      (mask = 0xffffffff implies that only services with the same
                      instance ID will be returned).

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_find_servers_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    ipc_router_service_type *service,
    ipc_router_server_type *servers, 
    uint32 *num_entries, 
    uint32 *num_servers,
    uint32 mask
)
{
  ipc_router_client_type *client = NULL;
  int rc;

  if(!num_servers)
    return IPC_ROUTER_QDI_PARAM_ERR;

  if(handle)
  {
    client = handle->handle;
  }

  if((servers && !num_entries) ||
      (!servers && num_entries))
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, service, sizeof(ipc_router_service_type), QDI_PERM_R, (void **)&service);
  if(rc || !service)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, num_servers, sizeof(uint32), QDI_PERM_RW, (void **)&num_servers);
  if(rc || !num_servers)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  if(servers)
  {
    /* If servers is valid, the so will be num_entries */
    rc = qurt_qdi_lock_buffer(client_handle, num_entries, sizeof(uint32), QDI_PERM_RW, (void **)&num_entries);
    if(rc || !num_entries)
    {
      return IPC_ROUTER_QDI_PERM_ERR;
    }
    
    /* uint32 overflow check for (*num_entries * sizeof(ipc_router_server_type)) */
    if ((UINT32_MAX / sizeof(ipc_router_server_type)) < *num_entries)
    {
      return IPC_ROUTER_QDI_PARAM_ERR;
    }
    
    /* num_entries was locked above for read and write, so we can directly access it here 
     * servers might be read by the ipc_router_core function even though it is 
     * strictly an out parameter, just make sure we have permissions for read */
    rc = qurt_qdi_lock_buffer(client_handle, servers, *num_entries * sizeof(ipc_router_server_type),
            QDI_PERM_RW, (void **)&servers);
    if(rc || !servers)
    {
      return IPC_ROUTER_QDI_PERM_ERR;
    }
  }

  return ipc_router_core_find_all_servers(
      client, service, servers, num_entries, num_servers, mask);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_unregister_invocation

DESCRIPTION   Unregisters a previously registered service on this endpoint

ARGUMENTS     client_handle - Handle identifying the client
              handle - client handle for this endpoint
              service - The service discretion (ID and Instance).

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_unregister_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    ipc_router_service_type *service
)
{
  int rc;

  if(!handle)
  {
    return IPC_ROUTER_QDI_BADF_ERR;
  }
  if(!service)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, service, sizeof(ipc_router_service_type), QDI_PERM_R, (void **)&service);
  if(rc || !service)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  return ipc_router_core_dereg_server(handle->handle, service);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_register_invocation

DESCRIPTION   Register a service on this endpoint.

ARGUMENTS     client_handle - Handle identifying the client
              handle - Client handle for this endpoint.
              service - The service discretion (ID and Instance).

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_register_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    ipc_router_service_type *service
)
{
  int rc;

  if(!handle)
  {
    return IPC_ROUTER_QDI_BADF_ERR;
  }
  if(!service)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, service, sizeof(ipc_router_service_type), QDI_PERM_R, (void **)&service);
  if(rc || !service)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  return ipc_router_core_reg_server(handle->handle, service);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_name_register_invocation

DESCRIPTION   Register a service on this endpoint.

ARGUMENTS     client_handle - Handle identifying the client
              handle - Client handle for this endpoint.
              service - The service discretion (ID and Instance).

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_name_register_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    char *name,
    uint32 name_len
)
{
  int rc;
  ipc_router_qdi_name_t *entry, *i;
  if(!name || !name_len)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  if(!handle)
  {
    return IPC_ROUTER_QDI_BADF_ERR;
  }

  if(name_len > 32)
  {
    return IPC_ROUTER_QDI_NOT_SUPP_ERR;
  }

  entry = ipc_router_os_calloc(1, sizeof(*entry));
  if(!entry)
  {
    return IPC_ROUTER_QDI_NO_MEM_ERR;
  }

  rc = qurt_qdi_copy_from_user(client_handle, entry->name, name, name_len);
  if(rc)
  {
    ipc_router_os_free(entry);
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  if(entry->name[name_len - 1] != 0)
  {
    ipc_router_os_free(entry);
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  entry->addr = handle->handle->addr;

  ipc_router_os_sem_lock(&name_list_lock);
  LIST_FIND(g_name_list, i, link, 
       memcmp(&i->addr, &entry->addr, sizeof(i->addr)) == 0 &&
       memcmp(i->name, entry->name, 32) == 0);
  if(i)
  {
    ipc_router_os_free(entry);
    rc = IPC_ROUTER_STATUS_FAILURE;
  }
  else
  {
    LIST_ADD(g_name_list, entry, link);
    rc = 0;
  }
  ipc_router_os_sem_unlock(&name_list_lock);
  
  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_send_conf_invocation

DESCRIPTION   Send a TX confirmation control message to a destination port.

ARGUMENTS     client_handle - Handle identifying the client
              handle - the client handle provided by open,
              addr - Address of the destination

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_send_conf_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    ipc_router_address_type *addr
)
{
  int rc;

  if(!handle)
  {
    return IPC_ROUTER_QDI_BADF_ERR;
  }
  if(!addr)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, addr, sizeof(ipc_router_address_type), QDI_PERM_R, (void **)&addr);
  if(rc || !addr)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  return ipc_router_core_send_conf(handle->handle, addr);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_send_invocation

DESCRIPTION   Invocation to handle client request to send a buffer to a 
              remote client.

ARGUMENTS     client_handle - Handle identifying the client
              handle - Client handle provided on open
              addr - Address of the destination
              user_buf - The buffer to send to the destination,
              len - the length of the buffer
              flags - Send flags (See IPC_ROUTER_QDI_WRFLAG_*)
              addr_local - set if the address is local and does not need
                           to be locked

RETURN VALUE  0 on success, error code on failure. 
              IPC_ROUTER_STATUS_BUSY if the particular port is busy.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_send_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    ipc_router_address_type *addr,
    void *user_buf,
    uint32 len,
    uint32 flags,
    int addr_local
)
{
  int rc;
  ipc_router_address_type *send_addr = NULL;
  ipc_router_qdi_waiter_t waiter;
  ipc_router_qdi_poll_fd pfd;

  if(!user_buf || !len)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }
  if(!handle || 0 == (handle->allowed_flag & TRANSMIT_ALLOWED))
  {
    return IPC_ROUTER_QDI_BADF_ERR;
  }

  if(flags == 0 && handle->send_timeout == 0)
    flags = IPC_ROUTER_QDI_WRFLAG_DONTWAIT;

  switch(handle->conn.status)
  {
    case CONN_STATUS_DISCONNECTED:
      return IPC_ROUTER_QDI_HUNGUP_ERR;
      break;
    case CONN_STATUS_UNCONNECTED:
      if(addr)
      {
        if(!addr_local)
        {
          rc = qurt_qdi_lock_buffer(client_handle, addr, sizeof(ipc_router_address_type), QDI_PERM_R, (void **)&addr);
          if(rc || !addr)
          {
            return IPC_ROUTER_QDI_PERM_ERR;
          }
        }
        send_addr = addr;
      }
      else
      {
        return IPC_ROUTER_QDI_ADDR_REQ_ERR;
      }
      break;
    case CONN_STATUS_CONNECTED:
      send_addr = &handle->conn.val.addr;
      break;
    default:
      return IPC_ROUTER_QDI_INTERNAL_ERR;
  }

  if(qurt_qdi_lock_buffer(client_handle, user_buf, len, QDI_PERM_R, (void **)&user_buf))
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  if(flags & IPC_ROUTER_QDI_WRFLAG_DONTWAIT)
  {
    return ipc_router_core_send(handle->handle, send_addr, user_buf, len);
  }

  ipc_router_os_sem_lock(&waiter_list_lock);
  pfd.fd = handle->dev->qdi_handle;
  pfd.events = IPC_QDI_POLLOUT | IPC_QDI_POLLHUP;
  pfd.revents = 0;
  INIT_WAITER(waiter, &pfd, 1, handle->send_timeout, send_addr);
  ipc_router_os_sem_unlock(&waiter_list_lock);

  while(1)
  {
    qurt_signal_clear(&waiter.signal, IPC_QDI_POLLOUT);
    if(0 == (handle->allowed_flag & TRANSMIT_ALLOWED))
    {
      /* Being terminated */
      rc = IPC_ROUTER_QDI_BADF_ERR;
      break;
    }
    rc = ipc_router_core_send(handle->handle, send_addr, user_buf, len);
    if(rc != IPC_ROUTER_STATUS_BUSY)
    {
      break;
    }

#ifdef PD_RESTART_SUPPORT
    rc = qurt_signal_wait_cancellable(&waiter.signal, IPC_QDI_POLLOUT | TIMEOUT_SIG,
                                 QURT_SIGNAL_ATTR_WAIT_ANY, NULL);
    if(rc == QURT_ECANCEL)
    {
      rc = IPC_ROUTER_QDI_TERM_ERR;
      break;
    }
#else
    qurt_signal_wait_any(&waiter.signal, IPC_QDI_POLLOUT | TIMEOUT_SIG);
#endif
    if(qurt_signal_get(&waiter.signal) & TIMEOUT_SIG)
    {
      rc = IPC_ROUTER_QDI_TIMEOUT_ERR;
      break;
    }
  }

  ipc_router_os_sem_lock(&waiter_list_lock);
  DEINIT_WAITER(waiter, handle->send_timeout);
  ipc_router_os_sem_unlock(&waiter_list_lock);

  return rc;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_send_to_name_invocation

DESCRIPTION   Invocation to handle client request to multicast a buffer to a 
              all endpoints with the given name

ARGUMENTS     client_handle - Handle identifying the client
              handle - Client handle provided on open
              service - Address of the destination
              user_buf - The buffer to send to the destination,
              len - the length of the buffer

RETURN VALUE  0 on success, error code on failure. 
              IPC_ROUTER_STATUS_BUSY if the particular port is busy.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_send_to_name_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    ipc_router_service_type *service,
    void *user_buf,
    uint32 len
)
{
  int rc;

  if(!user_buf || !len)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }
  if(!handle)
  {
    return IPC_ROUTER_QDI_BADF_ERR;
  }
  
  rc = qurt_qdi_lock_buffer(client_handle, service, sizeof(ipc_router_service_type), QDI_PERM_R, (void **)&service);
  if(rc || !service)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, user_buf, len, QDI_PERM_R, (void **)&user_buf);
  if(rc || !user_buf)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  return ipc_router_core_send_to_name(handle->handle, service, user_buf, len);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_send_to_sname_invocation

DESCRIPTION   Invocation to handle client request to multicast a buffer to a 
              all endpoints with the given name

ARGUMENTS     client_handle - Handle identifying the client
              handle - Client handle provided on open
              service - Address of the destination
              user_buf - The buffer to send to the destination,
              len - the length of the buffer

RETURN VALUE  0 on success, error code on failure. 
              IPC_ROUTER_STATUS_BUSY if the particular port is busy.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_send_to_sname_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle,
    char *name,
    uint32 name_len,
    void *user_buf,
    uint32 len
)
{
  int rc = IPC_ROUTER_STATUS_FAILURE;
  ipc_router_address_type dst_addr;
  ipc_router_qdi_name_t *entry = NULL;

  if(!user_buf || !len || !name || !name_len)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }
  if(name_len > 32)
  {
    return IPC_ROUTER_QDI_NOT_SUPP_ERR;
  }
  if(!handle)
  {
    return IPC_ROUTER_QDI_BADF_ERR;
  }
  
  rc = qurt_qdi_lock_buffer(client_handle, name, name_len, QDI_PERM_R, (void **)&name);
  if(rc || !name)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }
  if(name[name_len - 1] != 0)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  ipc_router_os_sem_lock(&name_list_lock);
  LIST_FIND(g_name_list, entry, link, memcmp(name, entry->name, name_len) == 0);
  if(entry)
  {
    dst_addr = entry->addr;
  }
  ipc_router_os_sem_unlock(&name_list_lock);
  if(!entry)
  {
    return IPC_ROUTER_STATUS_FAILURE;
  }

  return ipc_router_qdi_send_invocation(client_handle, handle, &dst_addr, 
                                          user_buf, len, 0, 1);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_close_invocation

DESCRIPTION   handles the request from a client to release a router handle.

ARGUMENTS     client_handle - Handle identifying the client
              handle - handle to release

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  Unlocks the clients_lock
===========================================================================*/
static inline int ipc_router_qdi_close_invocation
(
    int client_handle,
    ipc_router_user_ep_t *handle
)
{
  ipc_router_qdi_handle_t *dev;
  
  if(!handle)
  {
    return IPC_ROUTER_QDI_BADF_ERR;
  }

  if(!handle->handle || !handle->dev)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  dev = handle->dev;

  /* Signal any user thread which might be waiting so, it has a chance to 
   * shutdown. */
  if(dev->signal.valid)
  {
    qurt_qdi_signal_set(dev->signal.event.local);
    qurt_qdi_signal_set(dev->signal.data.local);
  }

  /* Disable RX and TX */
  handle->allowed_flag &= ~(DATA_ALLOWED | TRANSMIT_ALLOWED);

  /* This handle will be freed in the close callback */
  ipc_router_core_close(handle->handle);

  return 0;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_open_invocation

DESCRIPTION   invocation called when the remote client requests for an 
              IPC router handle. 

ARGUMENTS     client_handle - Identify the caller
              handle - The handle to this device.
              out_clnt_handle - The pointer to place the output handle.
              port_id - port ID required.
              cookie - Cookie to be associated with this opening handle.

RETURN VALUE  0 on success, error code on failure

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_qdi_open_invocation
(
  int client_handle,
  ipc_router_qdi_handle_t *handle,
  int *out_clnt_handle,
  uint32 port_id,
  ipc_router_core_options_type *in_options,
  void *cookie
)
{
  int rc;
  ipc_router_user_ep_t *clnt = NULL;
  ipc_router_core_options_type *options = NULL;

  if(!handle || !out_clnt_handle)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  clnt = ipc_router_os_calloc(1, sizeof(*clnt));
  if(!clnt)
  {
    return IPC_ROUTER_QDI_NO_MEM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, out_clnt_handle, sizeof(int), QDI_PERM_W, (void **)&out_clnt_handle);
  if(rc || !out_clnt_handle)
  {
    ipc_router_os_free(clnt);
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  if(in_options)
  {
    rc = qurt_qdi_lock_buffer(client_handle, in_options, sizeof(*options), QDI_PERM_R, (void **)&options);
    if(rc || !options)
    {
      ipc_router_os_free(clnt);
      return IPC_ROUTER_QDI_PERM_ERR;
    }
  }

  clnt->dev = handle;
  clnt->cookie = cookie;
  clnt->ref_count = 1; /* One reference to ipc_router_core */
  /* User has to explicitly ask for events */
  clnt->allowed_flag = DATA_ALLOWED | TRANSMIT_ALLOWED;
  /* Instead of infinity, I will use a comically large timeout
   * but not infinity. Let use set to inf if they chooses */
  clnt->send_timeout = 5000;
  /* Default to data port */
  clnt->ctrl_port = 0;

  rc = ipc_router_core_open_with_options(&clnt->handle, port_id, 
      ipc_router_qdi_rx_cb, clnt, 
      ipc_router_qdi_event_cb, clnt, options);
  if(rc != IPC_ROUTER_STATUS_SUCCESS)
  {
    ipc_router_os_free(clnt);
    return rc;
  }

  ipc_router_os_sem_lock(&handle->clients_lock);
  if(handle->next_fd < 1)
  {
    handle->next_fd = 1;
  }
  *out_clnt_handle = clnt->fd = handle->next_fd++;
  LIST_ADD(handle->clients, clnt, link);
  handle->active_receivers++;
  ipc_router_os_sem_unlock(&handle->clients_lock);

  return 0;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_event_log_invocation

DESCRIPTION   Allows an endpoint to log events

ARGUMENTS     client_handle - The calling client's handle
              handle - Handle for this device
              desc - Log event description
              d1 - d6 - Log data

RETURN VALUE  Negative error code on failure, 0 on success

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_event_log_invocation
(
    int client_handle,
    ipc_router_qdi_handle_t *handle,
    uint32 desc,
    uint32 d1,
    uint32 d2,
    uint32 d3,
    uint32 d4,
    uint32 d5,
    uint32 d6
)
{
  if(!handle)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  SMEM_LOG_EVENT6(desc, d1, d2, d3, d4, d5, d6);

  return 0;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_set_options_invocation

DESCRIPTION   Set port options

ARGUMENTS     client_handle - The calling client's handle
              handle - port handle
              _options - pointer to options structure

RETURN VALUE  0 on success, error code on failure

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_set_options_invocation
(
 int client_handle,
 ipc_router_user_ep_t *handle,
 ipc_router_qdi_port_options *_options
)
{
  int rc;
  if(!handle)
    return IPC_ROUTER_QDI_BADF_ERR;
  if(!_options)
    return IPC_ROUTER_QDI_PARAM_ERR;

  ipc_router_qdi_port_options *options = NULL;
  rc = qurt_qdi_lock_buffer(client_handle, _options, sizeof(*options),
                              QDI_PERM_R, (void **)&options);
  if(rc || !options)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  if(options->allowed_valid)
    handle->allowed_flag = options->allowed_flag;
  if(options->sendtimeo_valid)
    handle->send_timeout = (int)options->sendtimeo;
  if(options->rcvtimeo_valid)
    handle->dev->recv_timeout = (int)options->rcvtimeo;
  if(options->ctrl_port_valid)
  {
    handle->ctrl_port = 1;
    /* Transmit disallowed on ctrl ports */
    handle->allowed_flag &= ~TRANSMIT_ALLOWED;
    handle->allowed_flag |= EVENT_ALLOWED;
  }
  if(options->type_pref_valid)
  {
    /* Not supported on a non-ctrl port */
    if(!handle->ctrl_port)
    {
      return IPC_ROUTER_QDI_NOT_SUPP_ERR;
    }
    handle->conn.status = CONN_STATUS_CONNECTED_NAME;
    handle->conn.val.name.type = options->type_pref;
    handle->conn.val.name.instance = 0xffffffff;
  }

  return 0;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_connect_invocation

DESCRIPTION   Sets a port to be connected to a remote address.
              (Future send need not provide the address, also, the port
              is put in a disconnect state when we detect a REMOVE_CLIENT

ARGUMENTS     client_handle - The calling client's handle
              handle - the port handle
              addr - the address of the remote port.

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static inline int ipc_router_qdi_connect_invocation
(
 int client_handle,
 ipc_router_user_ep_t *handle,
 ipc_router_address_type *addr
)
{
  int rc;

  if(!handle)
    return IPC_ROUTER_QDI_BADF_ERR;
  if(!addr)
    return IPC_ROUTER_QDI_PARAM_ERR;

  if(handle->conn.status != CONN_STATUS_UNCONNECTED)
  {
    return IPC_ROUTER_QDI_NOT_SUPP_ERR;
  }
  rc = qurt_qdi_copy_from_user(client_handle, &handle->conn.val.addr, addr, sizeof(*addr));
  if(rc)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }
  handle->conn.status = CONN_STATUS_CONNECTED;
  return 0;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_poll_invocation

DESCRIPTION   Allow a user to wait on events on multiple device file 
              descriptor (Similar to the standard UNIX poll() method.

ARGUMENTS     client_handle - The calling client's handle
              handle - The device handle (Assumption is that this is also
                the first fd in pfd)
              pfd - Array of pollfd structures (See ipc_router_qdi_poll_fd)
              len - number of elements in the pfd array.
              timeout_ms - Timeout in milliseconds.
                           0 - returns immediately if there are not events.
                           -1 - Wait forever.
                           X > 0 - wait for X milli-seconds for an event

RETURN VALUE  Number of fds on which events are pending. Negative error
              code on failure.

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_qdi_poll_invocation
(
 int client_handle,
 ipc_router_qdi_handle_t *handle,
 ipc_router_qdi_poll_fd *pfd,
 uint32_t len,
 int timeout_ms
)
{
  int rc;
  int ret = 0, i;
  ipc_router_qdi_waiter_t waiter;

  if(!pfd || !len)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  rc = qurt_qdi_lock_buffer(client_handle, pfd, len * sizeof(*pfd), QDI_PERM_RW, (void **)&pfd);
  if(rc || !pfd)
  {
    return IPC_ROUTER_QDI_PERM_ERR;
  }

  ipc_router_os_sem_lock(&waiter_list_lock);
  INIT_WAITER(waiter, pfd, len, timeout_ms, NULL);

  /* Check all ports with lock held */
  while(ret == 0)
  {
    unsigned int wait_events = 0;

    qurt_signal_clear(&waiter.signal, IPC_QDI_POLLIN | IPC_QDI_POLLOUT | IPC_QDI_POLLHUP | IPC_QDI_POLLNVAL);
    
    for(i = 0; i < len; i++)
    {
      ipc_router_qdi_handle_t *h = NULL;

      pfd[i].revents = 0;
      wait_events |= pfd[i].events;

      if(pfd[i].fd < 0)
      {
        continue;
      }

      h = (ipc_router_qdi_handle_t *)qurt_qdi_objref_get_from_handle(QDI_HANDLE_LOCAL_CLIENT, pfd[i].fd);

      if(!h || h->active_receivers <= 0 || LIST_CNT(h->clients) == 0)
      {
        pfd[i].revents = IPC_QDI_POLLNVAL;
        ret = IPC_ROUTER_QDI_BADF_ERR;
        if(h)
          qurt_qdi_objref_release(&h->obj);
        break;
      }

      if(disconnected_users(h) > 0)
      {
        pfd[i].revents |= IPC_QDI_POLLHUP;
      }

      ipc_router_os_sem_lock(&h->rx_q_lock);
      if(pfd[i].events & IPC_QDI_POLLIN)
      {
        if(LIST_CNT(h->rx_q) > 0)
        {
          pfd[i].revents |= IPC_QDI_POLLIN;
        }
      }
      ipc_router_os_sem_unlock(&h->rx_q_lock);

      if(pfd[i].events & IPC_QDI_POLLOUT)
      {
        pfd[i].revents |= IPC_QDI_POLLOUT;
      }
      ret += pfd[i].revents == 0 ? 0 : 1;

      qurt_qdi_objref_release(&h->obj);
    }

    if(ret || timeout_ms == 0)
    {
      break;
    }

    /* Wait without holding the waiter lock */
    ipc_router_os_sem_unlock(&waiter_list_lock);
#ifdef PD_RESTART_SUPPORT
    rc = qurt_signal_wait_cancellable(&waiter.signal, wait_events | TIMEOUT_SIG,
                                       QURT_SIGNAL_ATTR_WAIT_ANY, NULL);
    if(rc == QURT_ECANCEL)
    {
      ret = IPC_ROUTER_QDI_TERM_ERR;
    }
#else
    qurt_signal_wait_any(&waiter.signal, wait_events | TIMEOUT_SIG);
#endif
    ipc_router_os_sem_lock(&waiter_list_lock);

    if(qurt_signal_get(&waiter.signal) & TIMEOUT_SIG)
    {
      /* Timed out, break out of loop with ret = 0 */
      break;
    }
  }
  DEINIT_WAITER(waiter, timeout_ms);
  ipc_router_os_sem_unlock(&waiter_list_lock);

  return ret;
}


/*===========================================================================
FUNCTION      ipc_router_qdi_handle_release

DESCRIPTION   Cleanup everything associated with a handle. Called when a
              process calls deinit or when it exits (Hopefully QuRT closes
              pending handles when a process exits).

ARGUMENTS     handle - The handle associated with this open object.

RETURN VALUE  None

SIDE EFFECTS  All clients will be closed on the behalf of the remote clients.
===========================================================================*/
static inline void ipc_router_qdi_handle_release
(
  ipc_router_qdi_handle_t *handle 
)
{
  data_buf_t *buf, *buf_to_free;
  event_buf_t *event, *event_to_free;
  ipc_router_user_ep_t *i, *head;
  ipc_router_qdi_waiter_t waiter;
  ipc_router_qdi_poll_fd pfd;

  pfd.fd = handle->qdi_handle;
  pfd.events = IPC_QDI_POLLNVAL;
  pfd.revents = 0;

  ipc_router_os_sem_lock(&waiter_list_lock);
  INIT_WAITER(waiter, &pfd, 1, 0, NULL);
  ipc_router_os_sem_unlock(&waiter_list_lock);

  /* Take a reference so they dont get released from
   * underneath us. */
  ipc_router_os_sem_lock(&handle->clients_lock);
  i = head = LIST_HEAD(handle->clients);
  while(i)
  {
    i->ref_count++;
    i = i->link.next;
  }
  i = head;
  ipc_router_os_sem_unlock(&handle->clients_lock);
  
  /* Update flag to indicate client handles started closing, 
   * to avoid QDI handle access in the close event. */
  handle->closing_client_handles = TRUE;

  /* Close each handle */
  while(i)
  {
    ipc_router_core_close(i->handle);
    i = i->link.next;
  }
  i = head;
  
  /* Release the reference we have taken */
  while(i)
  {
    ipc_router_user_ep_t *to_free = i;
    i = i->link.next;
    ipc_router_qdi_put_obj(to_free);
  }

  /* Wait till all clients are freed (reference count of handle
   * goes to 0) so we can continue to destroy it */
  ipc_router_os_sem_lock(&handle->clients_lock);
  while(LIST_CNT(handle->clients) > 0)
  {
    ipc_router_os_sem_unlock(&handle->clients_lock);
    /* I can make this cancellable, but the wait duration
     * is short, so... use the normal wait */
    qurt_signal_wait_any(&waiter.signal, IPC_QDI_POLLNVAL);
    qurt_signal_clear(&waiter.signal, IPC_QDI_POLLNVAL);
    ipc_router_os_sem_lock(&handle->clients_lock);
  }
  ipc_router_os_sem_unlock(&handle->clients_lock);

  ipc_router_os_sem_lock(&waiter_list_lock);
  DEINIT_WAITER(waiter, 0);
  ipc_router_os_sem_unlock(&waiter_list_lock);

  /* Remove all queued buffers */
  ipc_router_os_sem_lock(&handle->rx_q_lock);
  buf = LIST_HEAD(handle->rx_q);
  LIST_INIT(handle->rx_q);
  while(buf)
  {
    ipc_router_packet_free(&buf->pkt);
    buf_to_free = buf;
    buf = buf->link.next;
    ipc_router_os_free(buf_to_free);
  }
  ipc_router_os_sem_unlock(&handle->rx_q_lock);

  /* Remove all queued events */
  ipc_router_os_sem_lock(&handle->event_q_lock);
  event = handle->event_q.head;
  LIST_INIT(handle->event_q);
  while(event)
  {
    event_to_free = event;
    event = event->link.next;
    ipc_router_os_free(event_to_free);
  }
  ipc_router_os_sem_unlock(&handle->event_q_lock);

  /* Deinit all locks */
  ipc_router_os_sem_deinit(&handle->event_q_lock);
  ipc_router_os_sem_deinit(&handle->rx_q_lock);
  ipc_router_os_sem_deinit(&handle->clients_lock);

  ipc_router_os_sem_lock(&active_handles_lock);
  LIST_REMOVE(g_active_handles, handle, link);
  ipc_router_os_sem_unlock(&active_handles_lock);

  if (handle->signal.valid)
  {
    qurt_qdi_close(handle->signal.group.local);
    qurt_qdi_close(handle->signal.event.local);
    qurt_qdi_close(handle->signal.data.local);
    qurt_qdi_close(handle->signal.group.remote);
    qurt_qdi_close(handle->signal.event.remote);
    qurt_qdi_close(handle->signal.data.remote);
    handle->signal.group.local = -1;
    handle->signal.event.local = -1;
    handle->signal.data.local = -1;
    handle->signal.group.remote = -1;
    handle->signal.event.remote = -1;
    handle->signal.data.remote = -1;
  }
  ipc_router_os_free(handle);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_main_invocation

DESCRIPTION   funtion which will be invoked when any calls are made to the
              QDI driver. This is the starting point on handling all 
              router methods.

ARGUMENTS     ptr - Usually the open handle.
              method - The method of the invocation.
              a1 - a8 - The arguments passed in the invocation

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_qdi_main_invocation
(
 int client_handle,
 qurt_qdi_obj_t *ptr,
 int method,
 qurt_qdi_arg_t a1,
 qurt_qdi_arg_t a2,
 qurt_qdi_arg_t a3,
 qurt_qdi_arg_t a4,
 qurt_qdi_arg_t a5,
 qurt_qdi_arg_t a6,
 qurt_qdi_arg_t a7,
 qurt_qdi_arg_t a8,
 qurt_qdi_arg_t a9
)
{
  ipc_router_qdi_handle_t *handle = (ipc_router_qdi_handle_t *)ptr;
  /* For most calls, the first parameter is the client handle */
  ipc_router_user_ep_t *client = NULL;

  int ret = IPC_ROUTER_QDI_INTERNAL_ERR;

  if(!handle)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  switch(method)
  {
    case IPC_ROUTER_QDI_OPEN:
      ret = ipc_router_qdi_open_invocation(client_handle, handle, a1.ptr, (uint32)a2.num, a3.ptr, a4.ptr);
      break;

    case IPC_ROUTER_QDI_CLOSE:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_close_invocation(client_handle, client);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_SET_PORT_OPTIONS:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_set_options_invocation(client_handle, client, a2.ptr);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_CONNECT:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_connect_invocation(client_handle, client, a2.ptr);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_SEND:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_send_invocation(client_handle, client, a2.ptr, a3.ptr, (uint32)a4.num, a5.num, 0);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_SEND_TO_NAME:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_send_to_name_invocation(client_handle, client, a2.ptr, a3.ptr, (uint32)a4.num);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_SEND_CONF:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_send_conf_invocation(client_handle, client, a2.ptr);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_SEND_TO_SNAME:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_send_to_sname_invocation(client_handle, client, a2.ptr, (uint32)a3.num, a4.ptr, (uint32)a5.num);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_REGISTER_SERVER:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_register_invocation(client_handle, client, a2.ptr);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_UNREGISTER_SERVER:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_unregister_invocation(client_handle, client, a2.ptr);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_REGISTER_NAME:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_name_register_invocation(client_handle, client, a2.ptr, (uint32)a3.num);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_FIND_ALL_SERVERS:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_find_servers_invocation(client_handle, client, a2.ptr, a3.ptr, a4.ptr, 
                                                            a5.ptr, (uint32)a6.num);
      ipc_router_qdi_put_obj(client);
      break;

    case IPC_ROUTER_QDI_FIND_NAME:
      client = ipc_router_qdi_get_obj(handle, a1.num);
      ret = ipc_router_qdi_find_names_invocation(client_handle, client, a2.ptr, (uint32)a3.num, a4.ptr,
                                                 a5.ptr, a6.ptr);
      ipc_router_qdi_put_obj(client);
      break;

    /* The following are for the common process reader thread, and hence do not 
     * require the client handle provided */
    case IPC_ROUTER_QDI_READ:
      ret = ipc_router_qdi_read_invocation(client_handle, handle, a1.ptr, (uint32)a2.num, 
                                           a3.ptr, a4.ptr, a5.ptr, a6.ptr, a7.num);
      break;

    case IPC_ROUTER_QDI_EVENT_READ:
      ret = ipc_router_qdi_event_read_invocation(client_handle, handle, a1.ptr, a2.ptr, a3.ptr);
      break;

    case IPC_ROUTER_QDI_LOG:
      ret = ipc_router_qdi_event_log_invocation(client_handle, handle, a1.num, a2.num, a3.num,
                                                a4.num, a5.num, a6.num, a7.num);
      break;
    case IPC_ROUTER_QDI_POLL:
      ret = ipc_router_qdi_poll_invocation(client_handle, handle, a1.ptr, a2.num, a3.num);
      break;

    default:
      ret = qurt_qdi_method_default(client_handle, ptr, method, a1, a2, a3, a4, a5, a6, a7, a8, a9);
      break;
  }

  return ret;
}

/*===========================================================================
FUNCTION      ipc_router_qdi_deinit_invocation

DESCRIPTION   Handles an deinit call (Technically this is the invocation for
              qurt_qdi_close(), but named deinit to avoid confusion with the
              actual IPC Router close call) which initializes a process's
              connection to the QDI driver.

ARGUMENTS     ptr - object to release

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void ipc_router_qdi_deinit_invocation
(
  qurt_qdi_obj_t *ptr
)
{
  ipc_router_qdi_handle_t *handle = (ipc_router_qdi_handle_t *)ptr;

  if(!handle)
  {
    return;
  }

  ipc_router_qdi_handle_release(handle);
}

/*===========================================================================
FUNCTION      ipc_router_qdi_init_invocation

DESCRIPTION   Handles an init call (Technically this is the invocation for
              qurt_qdi_open(), but named init to avoid confusion with the
              actual IPC Router open call) which initializes a process's
              connection to the QDI driver.

ARGUMENTS     client_handle - Handle of the caller
              ptr - unused
              method - this is the method ID.
              a1:a9 - The init arguments.

RETURN VALUE  0 on success, error code on failure.

SIDE EFFECTS  None
===========================================================================*/
static int ipc_router_qdi_init_invocation
(
 int client_handle, 
 qurt_qdi_obj_t *ptr,
 int method, 
 qurt_qdi_arg_t a1, /* Dev name */
 qurt_qdi_arg_t a2, /* Out: Group Sig */
 qurt_qdi_arg_t a3, /* Out: Event Sig */
 qurt_qdi_arg_t a4, /* Out: Data Sig */
 qurt_qdi_arg_t a5,
 qurt_qdi_arg_t a6,
 qurt_qdi_arg_t a7,
 qurt_qdi_arg_t a8,
 qurt_qdi_arg_t a9
)
{
  ipc_router_qdi_handle_t *handle;
  int rc;
  int *remote_group_sig = NULL;
  int *remote_event_sig = NULL;
  int *remote_data_sig = NULL;

  if(method != IPC_ROUTER_QDI_INIT)
  {
    return IPC_ROUTER_QDI_PARAM_ERR;
  }

  handle = ipc_router_os_calloc(1, sizeof(*handle));
  if(!handle)
  {
    return IPC_ROUTER_QDI_NO_MEM_ERR;
  }

  LIST_INIT(handle->rx_q);
  ipc_router_os_sem_init(&handle->rx_q_lock);
  LIST_INIT(handle->event_q);
  ipc_router_os_sem_init(&handle->event_q_lock);
  LIST_INIT(handle->clients);
  ipc_router_os_sem_init(&handle->clients_lock);

  handle->next_fd = 1;

  /* Default, recv will block forever */
  handle->recv_timeout = -1;

  handle->active_receivers = 0;
  handle->closing_client_handles = FALSE;

  handle->obj.invoke = ipc_router_qdi_main_invocation;
  handle->obj.release = ipc_router_qdi_deinit_invocation;
  handle->obj.refcnt = QDI_REFCNT_INIT;

  rc = qurt_qdi_handle_create_from_obj_t(client_handle, (qurt_qdi_obj_t *)handle);
  if(rc < 0)
  {
    rc = IPC_ROUTER_QDI_INTERNAL_ERR;
    goto err_bail;
  }
  handle->qdi_handle = rc;

  /* User passes in 3 pointers to place the output signals. Get permissions to write.
     if pointer for group is not provided, it is assumed that none are required.
     if pointer for group is provided, then it is required for event adn data to be
     provided  */

  if(a2.ptr)
  {
    rc = qurt_qdi_lock_buffer(client_handle, a2.ptr, sizeof(int), QDI_PERM_W, (void **)&remote_group_sig);
    if(rc || !remote_group_sig)
    {
      rc = IPC_ROUTER_QDI_PERM_ERR;
      goto release_handle_bail;
    }
    rc = qurt_qdi_lock_buffer(client_handle, a3.ptr, sizeof(int), QDI_PERM_W, (void **)&remote_event_sig);
    if(rc || !remote_event_sig)
    {
      rc = IPC_ROUTER_QDI_PERM_ERR;
      goto release_handle_bail;
    }

    rc = qurt_qdi_lock_buffer(client_handle, a4.ptr, sizeof(int), QDI_PERM_W, (void **)&remote_data_sig);
    if(rc || !remote_data_sig)
    {
      rc = IPC_ROUTER_QDI_PERM_ERR;
      goto release_handle_bail;
    }

    rc = qurt_qdi_signal_group_create(client_handle, 
             &handle->signal.group.local, &handle->signal.group.remote);
    if(rc)
    {
      rc = IPC_ROUTER_QDI_INTERNAL_ERR;
      goto release_handle_bail;
    }
    *remote_group_sig = handle->signal.group.remote;

    rc = qurt_qdi_signal_create(handle->signal.group.local, &handle->signal.event.local,
                              &handle->signal.event.remote);
    if(rc)
    {
      rc = IPC_ROUTER_QDI_INTERNAL_ERR;
      goto release_handle_bail;
    }
    *remote_event_sig = handle->signal.event.remote;

    rc = qurt_qdi_signal_create(handle->signal.group.local, &handle->signal.data.local,
                              &handle->signal.data.remote);
    if(rc)
    {
      rc = IPC_ROUTER_QDI_INTERNAL_ERR;
      goto release_handle_bail;
    }
    *remote_data_sig = handle->signal.data.remote;
    handle->signal.valid = 1;
  }
  else
  {
    handle->signal.valid = 0;
  }

  ipc_router_os_sem_lock(&active_handles_lock);
  LIST_ADD(g_active_handles, handle, link);
  ipc_router_os_sem_unlock(&active_handles_lock);

  return handle->qdi_handle;

release_handle_bail:
  qurt_qdi_handle_release(client_handle, handle->qdi_handle);
err_bail:
  ipc_router_os_sem_deinit(&handle->clients_lock);
  ipc_router_os_sem_deinit(&handle->event_q_lock);
  ipc_router_os_sem_deinit(&handle->rx_q_lock);
  ipc_router_os_free(handle);
  return rc;
}

/*===========================================================================
                        PUBLIC FUNCTIONS
===========================================================================*/


/*===========================================================================
FUNCTION      ipc_router_qdi_init

DESCRIPTION   Initializes the IPC Router QDI driver

ARGUMENTS     None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void ipc_router_qdi_init(void)
{
  static qurt_qdi_obj_t opener = {
    ipc_router_qdi_init_invocation,
    QDI_REFCNT_PERM,
    NULL
  };

  ipc_router_os_sem_init(&waiter_list_lock);
  ipc_router_os_sem_init(&active_handles_lock);
  ipc_router_os_sem_init(&name_list_lock);

  qurt_qdi_register_devname(IPC_ROUTER_QDI_DEV_NAME, &opener);
}
