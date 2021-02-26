/******************************************************************************
  @file    qsocket_ipcr.c
  @brief   Socket interface into IPC Router

  DESCRIPTION
  IPC Router is a connection-less datagram protocol covering the Network
  and Transport layers of the OSI model. It provides a packet delimited
  end-to-end flow control. Any streaming functionalities provided by
  IPC Router are implemented on top of the datagram methods.

  IPC Router also provides a distributed name service in order to provide
  location transparency to it's clients

  This source implements a OS agnostic sockets interface to IPC Router
  when a direct function call API is available.

  ---------------------------------------------------------------------------
  Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
  ---------------------------------------------------------------------------

 *******************************************************************************/

/*===========================================================================
                      INCLUDES
===========================================================================*/
#include "qsocket_ipcr.h"
#include "qsocket_os.h"
#include "ipc_router_core.h"
#include "ipc_router_internal.h"

/*===========================================================================
                      MACROS AND DEFINES
===========================================================================*/

/* Timeout when waiting for flow control */
#define WRITE_TIMEOUT 5000

#define ACTIVE_RX  1
#define ACTIVE_TX  2

#define QSOCK_CTRL  0xffff0000


/*===========================================================================
                      FORWARD DECLARATIONS
===========================================================================*/

typedef struct ipcr_sock_s ipcr_sock_t;
typedef struct ipcr_buf_s ipcr_buf_t;
typedef struct ipcr_waiter_s ipcr_waiter_t;

/*===========================================================================
                     PRIVATE TYPE DECLARATIONS
===========================================================================*/

typedef struct qsockaddr_ipcr ipcr_addr_t;

/* Waiter element type */
struct ipcr_waiter_s
{
  LINK(ipcr_waiter_t, link);
  ipcr_sock_os_sig_t      signal;
  struct qpollfd          *pfd;
  qnfds_t                 num;
  ipc_router_address_type *addr;
};

/* Buffer entry type */
struct ipcr_buf_s
{
  LINK(ipcr_buf_t, link);
  ipc_router_packet_type  *pkt;
  uint32_t                len;
  ipc_router_address_type addr;
  int                confirm_rx;
};

/* Internal socket representation */
struct ipcr_sock_s
{
  LINK(ipcr_sock_t, link);
  int                    fd;
  int                    type;
  ipc_router_client_type *handle;
  int                    ref;
  int                    write_timeout;
  int                    read_timeout;

  LIST(ipcr_buf_t, rx_q);
  ipc_router_os_sem rx_q_lock;

  /* Bit 0: RX 1 active, 0 - down
   * Bit 1: TX 1 active, 0 - down */
  uint8_t active;

  /* 0 unconnected, 1 connected, -1 disconnected */
  int                     connected;
  union
  {
    ipc_router_address_type addr;
    ipc_router_service_type name;
  } conn;
};

/*===========================================================================
                      GLOBALS
===========================================================================*/

/* List of sockets. Can be optimized to be a hash
 * if number of users increase beyond a threshold */
static LIST(ipcr_sock_t, g_sock_list);
static ipc_router_os_sem sock_list_lock;
static int g_next_sock = 1;

/* List of waiters */
static LIST(ipcr_waiter_t, g_waiters_list);
static ipc_router_os_sem waiters_list_lock;

#define IPCR_ADDR_PORT_SUPP   0x1
#define IPCR_ADDR_NAME_SUPP   0x2
#define IPCR_ADDR_SNAME_SUPP  0x4

/*===========================================================================
                      PRIVATE METHODS
===========================================================================*/
static int validate_addr(ipcr_addr_t *addr, qsocklen_t addrlen,
                         unsigned int supported_addrtypes)
{
  if(addr)
  {
    if(addrlen != sizeof(*addr))
    {
      return QEINVAL;
    }
    if(addr->sa_family != AF_IPC_ROUTER)
    {
      return QEAFNOSUPPORT;
    }

    switch(addr->address.addrtype)
    {
      case IPCR_ADDR_PORT:
        if(!(supported_addrtypes & IPCR_ADDR_PORT_SUPP))
        {
          return QEOPNOTSUPP;
        }
        break;

      case IPCR_ADDR_NAME:
        if(!(supported_addrtypes & IPCR_ADDR_NAME_SUPP))
        {
          return QEOPNOTSUPP;
        }
        break;

      default:
        return QEOPNOTSUPP;
    }
  }
  else
  {
    if(addrlen > 0)
    {
      return QEINVAL;
    }
  }
  return 0;
}

/*===========================================================================
  FUNCTION  err_ipcr_to_socket
===========================================================================*/
/*!
@brief

  Converts IPC Router errors to socket error codes

@note
  None
*/
/*=========================================================================*/
static int err_ipcr_to_sock(int ipcr_err)
{
  int ret;
  switch(ipcr_err)
  {
    case IPC_ROUTER_STATUS_SUCCESS:
      ret = 0;
      break;
    case IPC_ROUTER_STATUS_NO_MEM:
      ret = QENOMEM;
      break;
    case IPC_ROUTER_STATUS_BUSY:
      ret = QEAGAIN;
      break;
    case IPC_ROUTER_STATUS_INVALID_ENDPOINT:
      ret = QECONNRESET;
      break;
    default:
      ret = QEFAULT;
      break;
  }
  return ret;
}

/*===========================================================================
  FUNCTION  convert_event
===========================================================================*/
/*!
@brief

  Converts an IPC Router event into an event type for the ctrl socket.

@note
  None
*/
/*=========================================================================*/
static int convert_event(ipc_router_event_type event)
{
  int ret;
  switch(event)
  {
    case IPC_ROUTER_EVENT_NEW_SERVER:
      ret = IPCR_CTRL_NEW_SERVICE;
      break;
    case IPC_ROUTER_EVENT_REMOVE_SERVER:
      ret = IPCR_CTRL_REMOVE_SERVICE;
      break;
    case IPC_ROUTER_EVENT_REMOVE_CLIENT:
      ret = IPCR_CTRL_REMOVE_PORT;
      break;
    default:
      ret = 0;
      break;
  }
  return ret;
}

/*===========================================================================
  FUNCTION  unblock_event
===========================================================================*/
/*!
@brief

  Unblocks all waiters who are waiting for the specific event on the
  particular socket.

@note
  None
*/
/*=========================================================================*/
static int unblock_event(int fd, int event, ipc_router_address_type *addr)
{
  ipcr_waiter_t *w;
  int ret = 0;

  ipc_router_os_sem_lock(&waiters_list_lock);
  w = LIST_HEAD(g_waiters_list);
  while(w)
  {
    int i, set = 0;;
    for(i = 0; i < w->num; i++)
    {
      if((w->pfd[i].fd == fd) &&
         (event & (w->pfd[i].events | QPOLLHUP | QPOLLNVAL)))
      {
        if(!addr || !w->addr || memcmp(w->addr, addr, sizeof(*addr)) == 0)
        {
          set = 1;
          IPCR_OS_SIGNAL_SET(&w->signal);
        }
        break;
      }
    }
    ret += set;
    w = w->link.next;
  }
  ipc_router_os_sem_unlock(&waiters_list_lock);
  return ret;
}


/*==========================================================================
  FUNCTION  get_sock
===========================================================================*/
/*!
@brief

  Looks up a socket descriptor, takes a reference and returns the structure

@note
  None
*/
/*=========================================================================*/
static ipcr_sock_t *get_sock(int fd)
{
  ipcr_sock_t *sock;
  ipc_router_os_sem_lock(&sock_list_lock);
  LIST_FIND(g_sock_list, sock, link, sock->fd == fd);
  if(sock)
  {
    sock->ref++;
  }
  ipc_router_os_sem_unlock(&sock_list_lock);
  return sock;
}

/*===========================================================================
  FUNCTION  put_sock
===========================================================================*/
/*!
@brief

  Returns the reference taken from get_sock. Frees if reference goes to 0

@note
  None
*/
/*=========================================================================*/
static void put_sock(ipcr_sock_t *sock)
{
  int ref;
  int sig_fd = -1;
  ipc_router_os_sem_lock(&sock_list_lock);
  ref = --sock->ref;
  if(ref == 0)
  {
    ipcr_buf_t *buf;
    LIST_REMOVE(g_sock_list, sock, link);
    ipc_router_os_sem_deinit(&sock->rx_q_lock);
    while(NULL != (buf = LIST_HEAD(sock->rx_q)))
    {
      LIST_REMOVE(sock->rx_q, buf, link);
      ipc_router_packet_free(&buf->pkt);
      ipc_router_os_free(buf);
    }
    sig_fd = sock->fd;
    ipc_router_os_free(sock);
  }
  ipc_router_os_sem_unlock(&sock_list_lock);

  if(sig_fd > 0)
  {
    unblock_event(sig_fd, QPOLLNVAL, NULL);
  }
}


/*===========================================================================
  FUNCTION  sock_event_cb
===========================================================================*/
/*!
@brief

  Event callback which will be called by IPC Router core upon events

@note
  None
*/
/*=========================================================================*/
static void sock_event_cb(ipc_router_client_type *client,
                     void *cb_data,
                     ipc_router_event_type event,
                     ipc_router_event_info_type *info)
{
  ipcr_sock_t *sock = (ipcr_sock_t *)cb_data;

  if(!sock || (!(sock->active & ACTIVE_RX) && event != IPC_ROUTER_EVENT_CLOSED))
  {
    return;
  }

  switch(event)
  {
    case IPC_ROUTER_EVENT_RESUME_TX:
      if(unblock_event(sock->fd, QPOLLOUT, &info->client) == 0)
      {
        /* If no one is waiting, then we have to somehow
         * notify the user, so queue in an empty packet */
        ipcr_buf_t *buf = ipc_router_os_calloc(1, sizeof(*buf));
        if(buf)
        {
          buf->pkt = NULL;
          buf->len = 0;
          buf->addr = info->client;
          LINK_INIT(buf->link);
          ipc_router_os_sem_lock(&sock->rx_q_lock);
          LIST_ADD(sock->rx_q, buf, link);
          ipc_router_os_sem_unlock(&sock->rx_q_lock);
          unblock_event(sock->fd, QPOLLIN, NULL);
        }
      }
      break;
    case IPC_ROUTER_EVENT_NEW_SERVER:
    case IPC_ROUTER_EVENT_REMOVE_SERVER:
    case IPC_ROUTER_EVENT_REMOVE_CLIENT:
      if(sock->type == QSOCK_CTRL)
      {
        ipcr_ctrl_msg msg = {0};
        ipcr_buf_t *buf;
        msg.msg_type = convert_event(event);
        if(event == IPC_ROUTER_EVENT_NEW_SERVER || event == IPC_ROUTER_EVENT_REMOVE_SERVER)
        {
          if(sock->connected == 1 && sock->conn.name.type != info->server.service.type)
          {
            /* Break if the control socket is not interested in this
             * name */
            break;
          }
          msg.srv.service = (uint32_t)info->server.service.type;
          msg.srv.instance = (uint32_t)info->server.service.instance;
          msg.srv.node_id = (uint32_t)info->server.addr.processor_id;
          msg.srv.port_id = (uint32_t)info->server.addr.port_id;
        }
        else
        {
          msg.cli.node_id = (uint32_t)info->client.processor_id;
          msg.cli.port_id = (uint32_t)info->client.port_id;
        }
        buf = ipc_router_os_calloc(1, sizeof(*buf));
        if(!buf)
        {
          break;
        }
        buf->pkt = ipc_router_packet_new(0, sizeof(msg));
        if(!buf->pkt)
        {
          ipc_router_os_free(buf);
          break;
        }
        if(ipc_router_packet_copy_payload(&buf->pkt, (unsigned char *)&msg, sizeof(msg)) != sizeof(msg))
        {
          ipc_router_packet_free(&buf->pkt);
          ipc_router_os_free(buf);
          break;
        }
        buf->len = sizeof(msg);
        buf->addr.processor_id = 0xffffffff;
        buf->addr.port_id = 0xffffffff;
        buf->confirm_rx = 0;
        LINK_INIT(buf->link);

        ipc_router_os_sem_lock(&sock->rx_q_lock);
        LIST_ADD(sock->rx_q, buf, link);
        ipc_router_os_sem_unlock(&sock->rx_q_lock);
        unblock_event(sock->fd, QPOLLIN, NULL);
      }
      else if(sock->connected && event == IPC_ROUTER_EVENT_REMOVE_CLIENT &&
              info->client.processor_id == sock->conn.addr.processor_id &&
              info->client.port_id == sock->conn.addr.port_id)
      {
        ipcr_buf_t *buf;
        sock->connected = -1;
        memset(&sock->conn.addr, 0, sizeof(sock->conn.addr));
        ipc_router_os_sem_lock(&sock->rx_q_lock);
        /* TODO Should we allow the user to read?? */
        while(NULL != (buf = LIST_HEAD(sock->rx_q)))
        {
          LIST_REMOVE(sock->rx_q, buf, link);
          ipc_router_packet_free(&buf->pkt);
          ipc_router_os_free(buf);
        }
        ipc_router_os_sem_unlock(&sock->rx_q_lock);
        unblock_event(sock->fd, QPOLLIN | QPOLLHUP, NULL);
      }
      break;
    case IPC_ROUTER_EVENT_CLOSED:
      put_sock(sock);
      break;
    default:
      break;
  }
}

/*===========================================================================
  FUNCTION  sock_rx_cb
===========================================================================*/
/*!
@brief

  Data callback which will be called by IPC Router core upon received
  data packets

@note
  None
*/
/*=========================================================================*/
static void sock_rx_cb(ipc_router_client_type     *client,
                       void                       *cb_data,
                       ipc_router_packet_type     *msg,
                       ipc_router_address_type    *src_addr,
                       uint32                      len,
                       boolean                     confirm_rx)
{
  ipcr_sock_t *sock = (ipcr_sock_t *)cb_data;
  ipcr_buf_t *buf;

  if(!sock || sock->connected < 0 || !(sock->active & ACTIVE_RX) ||
     (sock->type != QSOCK_DGRAM && sock->type != QSOCK_STREAM))
  {
    ipc_router_packet_free(&msg);
    return;
  }

  if(sock->connected == 1 && (src_addr->processor_id != sock->conn.addr.processor_id ||
                         src_addr->port_id != sock->conn.addr.port_id))
  {
    /* Message from a source we are not connected to,
     * discard */
    if(confirm_rx)
    {
      ipc_router_core_send_conf(client, src_addr);
    }
    ipc_router_packet_free(&msg);
    /* TODO Log  */
    return;
  }

  buf = ipc_router_os_calloc(1, sizeof(*buf));
  if(!buf)
  {
    /* LOG ERROR */
    return;
  }
  buf->pkt = msg;
  buf->len = len;
  buf->addr = *src_addr;
  buf->confirm_rx = confirm_rx;
  LINK_INIT(buf->link);

  ipc_router_os_sem_lock(&sock->rx_q_lock);
  LIST_ADD(sock->rx_q, buf, link);
  ipc_router_os_sem_unlock(&sock->rx_q_lock);
  unblock_event(sock->fd, QPOLLIN, NULL);
}

/*===========================================================================
  FUNCTION  ipcr_mcast
===========================================================================*/
/*!
@brief
  Sends a multi-cast message to a name

@note
  None
*/
/*=========================================================================*/
static int ipcr_mcast(ipcr_sock_t *sock, ipcr_addr_t *name, void *buf, size_t len)
{
  int rc = -1;
  ipc_router_service_type service;

  /* Multicast is allowed only on datagram type connections */
  if(sock->type != QSOCK_DGRAM)
  {
    return QENOTSUPP;
  }

  service.type = (uint32)name->address.addr.port_name.service;
  service.instance = (uint32)name->address.addr.port_name.instance;

  rc = ipc_router_core_send_to_name(sock->handle, &service, 
                                    buf, len);
  if(rc != IPC_ROUTER_STATUS_SUCCESS)
  {
    rc = err_ipcr_to_sock(rc);
  }
  else
  {
    rc = (int)len;
  }
  return rc;
}

/*===========================================================================
                          PUBLIC METHODS
===========================================================================*/
void qsocket_init(void)
{
  static int inited = 0;
  if(inited == 0)
  {
    inited = 1;
    ipc_router_os_sem_init(&sock_list_lock);
    ipc_router_os_sem_init(&waiters_list_lock);
#ifdef QSOCKET_OS_SPECIFIC_INIT
    QSOCKET_OS_SPECIFIC_INIT();
#endif
  }
}

/*===========================================================================
  FUNCTION  qsocket
===========================================================================*/
int qsocket(int domain, int type, int protocol)
{
  ipcr_sock_t *sock;
  int rc;

  if(domain != AF_IPC_ROUTER)
  {
    return QEAFNOTSUPP;
  }

  if(type != QSOCK_DGRAM && type != QSOCK_STREAM)
  {
    return QEOPNOTSUPP;
  }

  sock = ipc_router_os_calloc(1, sizeof(*sock));
  if(!sock)
  {
    return QENOMEM;
  }

  /* Member Init */
  LIST_INIT(sock->rx_q);
  ipc_router_os_sem_init(&sock->rx_q_lock);

  /* Get a global sock FD */
  ipc_router_os_sem_lock(&sock_list_lock);
  sock->type = type;
  sock->fd = g_next_sock++;
  sock->ref = 1;
  sock->active = ACTIVE_RX | ACTIVE_TX;
  sock->read_timeout = -1;
  /* Default to a sane 5 sec instead of inf. */
  sock->write_timeout = WRITE_TIMEOUT;

  LIST_ADD(g_sock_list, sock, link);
  ipc_router_os_sem_unlock(&sock_list_lock);

  rc = ipc_router_core_open(&sock->handle, 0, sock_rx_cb,
                            sock, sock_event_cb, sock);
  if(rc != IPC_ROUTER_STATUS_SUCCESS)
  {
    ipc_router_os_sem_lock(&sock_list_lock);
    LIST_REMOVE(g_sock_list, sock, link);
    ipc_router_os_sem_unlock(&sock_list_lock);
    ipc_router_os_sem_deinit(&sock->rx_q_lock);
    ipc_router_os_free(sock);
    rc = err_ipcr_to_sock(rc);
  }
  else
  {
    rc = (int)sock->fd;
  }
  return rc;
}

/*===========================================================================
  FUNCTION  ipcr_connect
===========================================================================*/
int qconnect(int fd, struct qsockaddr *addr, qsocklen_t addrlen)
{
  ipcr_sock_t *sock;
  ipcr_addr_t *dest = (ipcr_addr_t *)addr;
  int rc;

  if(0 != (rc = validate_addr(dest, addrlen, IPCR_ADDR_PORT_SUPP)))
  {
    return rc;
  }

  sock = get_sock(fd);
  if(!sock)
  {
    return QEBADF;
  }

  if(sock->type != QSOCK_DGRAM && sock->type != QSOCK_STREAM)
  {
    put_sock(sock);
    return QEOPNOTSUPP;
  }

  if(sock->connected == 0)
  {
    sock->conn.addr.processor_id = (uint32)dest->address.addr.port_addr.node_id;
    sock->conn.addr.port_id = (uint32)dest->address.addr.port_addr.port_id;
    sock->connected = 1;
    /* TODO Check if the dest is valid with router core */
    rc = 0;
  }
  else
  {
    rc = QEISCONN;
  }
  put_sock(sock);
  return rc;
}

/*===========================================================================
  FUNCTION  qshutdown
===========================================================================*/
int qshutdown(int fd, int how)
{
  ipcr_sock_t *sock;
  ipc_router_client_type *handle;

  sock = get_sock(fd);
  if(!sock)
  {
    return QEBADF;
  }

  if(how == QSHUT_RD || how == QSHUT_RDWR)
  {
    sock->active &= ~ACTIVE_RX;
  }
  if(how == QSHUT_WR || how == QSHUT_RDWR)
  {
    sock->active &= ~ACTIVE_TX;
  }

  if(!sock->active)
  {
    handle = sock->handle;
    sock->handle = NULL;
    ipc_router_core_close(handle);
  }

  put_sock(sock);
  return 0;
}
/*===========================================================================
  FUNCTION  qclose
===========================================================================*/
int qclose(int fd)
{
  return qshutdown(fd, QSHUT_RDWR);
}

/*===========================================================================
  FUNCTION  qsendto
===========================================================================*/
int qsendto(int fd, void *buf, size_t len, int flags, 
              struct qsockaddr *_addr, qsocklen_t addrlen)
{
  ipcr_addr_t *addr = (ipcr_addr_t *)_addr;
  ipcr_sock_t *sock;
  int rc;
  ipc_router_address_type iaddr;
  ipcr_waiter_t waiter;
  int timeout = 0;
  struct qpollfd pfd;

  if(!buf || !len)
  {
    return QEINVAL;
  }
  if(0 != (rc = validate_addr(addr, addrlen, IPCR_ADDR_PORT_SUPP | IPCR_ADDR_NAME_SUPP)))
  {
    return rc;
  }

  sock = get_sock(fd);
  if(!sock)
  {
    return QEBADF;
  }

  if(addr && addr->address.addrtype == IPCR_ADDR_NAME)
  {
    rc = ipcr_mcast(sock, addr, buf, len);
    put_sock(sock);
    return rc;
  }

  timeout = sock->write_timeout;
  if(timeout == 0 && flags == 0)
  {
    flags |= QMSG_DONTWAIT;
  }

  if(sock->type != QSOCK_DGRAM && sock->type != QSOCK_STREAM)
  {
    put_sock(sock);
    return QEOPNOTSUPP;
  }

  if(addr)
  {
    iaddr.processor_id = (uint32)addr->address.addr.port_addr.node_id;
    iaddr.port_id = (uint32)addr->address.addr.port_addr.port_id;
  }
  else if(sock->connected == 1)
  {
    iaddr = sock->conn.addr;
  }
  else
  {
    put_sock(sock);
    return QENOTCONN;
  }

  if(!(sock->active & ACTIVE_TX))
  {
    put_sock(sock);
    return QEOPNOTSUPP;
  }

  if(flags & QMSG_DONTWAIT)
  {
    rc = ipc_router_core_send(sock->handle, &iaddr, buf, len);
    put_sock(sock);
    rc = rc == IPC_ROUTER_STATUS_SUCCESS ? (int)len : err_ipcr_to_sock(rc);
    if(rc == QECONNRESET && sock->connected == 0)
    {
      rc = QEHOSTUNREACH;
    }
    return rc;
  }

  ipc_router_os_sem_lock(&waiters_list_lock);
  LINK_INIT(waiter.link);
  pfd.fd = sock->fd;
  pfd.revents = 0;
  pfd.events = QPOLLOUT;
  waiter.pfd = &pfd;
  waiter.num = 1;
  waiter.addr = &iaddr;
  IPCR_OS_SIGNAL_INIT(&waiter.signal, timeout);
  LIST_ADD(g_waiters_list, &waiter, link);
  ipc_router_os_sem_unlock(&waiters_list_lock);

  while(1)
  {
    IPCR_OS_SIGNAL_CLEAR(&waiter.signal);
    rc = ipc_router_core_send(sock->handle, &iaddr, buf, len);
    if(rc == IPC_ROUTER_STATUS_SUCCESS)
    {
      rc = (int)len;
      break;
    }
    if(rc != IPC_ROUTER_STATUS_BUSY)
    {
      rc = err_ipcr_to_sock(rc);
      if(rc == QECONNRESET && sock->connected == 0)
      {
        rc = QEHOSTUNREACH;
      }
      break;
    }

    IPCR_OS_SIGNAL_WAIT(&waiter.signal, timeout);
    if(IPCR_OS_SIGNAL_TIMED_OUT(&waiter.signal))
    {
      rc = QETIMEDOUT;
      break;
    }
  }

  ipc_router_os_sem_lock(&waiters_list_lock);
  LIST_REMOVE(g_waiters_list, &waiter, link);
  IPCR_OS_SIGNAL_DEINIT(&waiter.signal);
  ipc_router_os_sem_unlock(&waiters_list_lock);

  put_sock(sock);
  return rc;
}

/*===========================================================================
  FUNCTION  qsend
===========================================================================*/
int qsend(int fd, void *buf, size_t len, int flags)
{
  return qsendto(fd, buf, len, flags, NULL, 0);
}

/*===========================================================================
  FUNCTION  qrecvfrom
===========================================================================*/
int qrecvfrom(int fd, void *buffer, size_t len, int flags, 
               struct qsockaddr *_addr, qsocklen_t *addrlen)
{
  ipcr_addr_t *addr = (ipcr_addr_t *)_addr;
  ipcr_sock_t *sock;
  struct qpollfd pfd;
  ipcr_waiter_t waiter;
  int timeout; 
  int rc = 0;

  if((len > 0 && !buffer) ||
     (addr && !addrlen) ||
     (addrlen && *addrlen < sizeof(ipcr_addr_t)))
  {
    return QEINVAL;
  }
  if(addrlen)
  {
    *addrlen = sizeof(ipcr_addr_t);
  }

  sock = get_sock(fd);
  if(!sock)
  {
    return QEBADF;
  }

  timeout = sock->read_timeout;
  if(timeout == 0 && flags == 0)
  {
    flags |= QMSG_DONTWAIT;
  }

  if(!(sock->active & ACTIVE_RX))
  {
    put_sock(sock);
    return QEOPNOTSUPP;
  }

  /* Wait for packets if required */
  if(!(flags & QMSG_DONTWAIT))
  {
    ipc_router_os_sem_lock(&waiters_list_lock);
    LINK_INIT(waiter.link);
    pfd.fd = sock->fd;
    pfd.events = QPOLLIN;
    pfd.revents = 0;
    waiter.pfd = &pfd;
    waiter.num = 1;
    waiter.addr = NULL;
    IPCR_OS_SIGNAL_INIT(&waiter.signal, timeout);
    LIST_ADD(g_waiters_list, &waiter, link);
    ipc_router_os_sem_unlock(&waiters_list_lock);

    while(1)
    {
      uint32_t cnt;
      IPCR_OS_SIGNAL_CLEAR(&waiter.signal);
      ipc_router_os_sem_lock(&sock->rx_q_lock);
      cnt = LIST_CNT(sock->rx_q);
      ipc_router_os_sem_unlock(&sock->rx_q_lock);
      if(cnt > 0 || sock->connected == -1)
      {
        break;
      }
      IPCR_OS_SIGNAL_WAIT(&waiter.signal, timeout);
      if(IPCR_OS_SIGNAL_TIMED_OUT(&waiter.signal))
      {
        break;
      }
    }

    ipc_router_os_sem_lock(&waiters_list_lock);
    LIST_REMOVE(g_waiters_list, &waiter, link);
    IPCR_OS_SIGNAL_DEINIT(&waiter.signal);
    ipc_router_os_sem_unlock(&waiters_list_lock);
  }

  ipc_router_os_sem_lock(&sock->rx_q_lock);
  if(LIST_CNT(sock->rx_q) > 0)
  {
    ipcr_buf_t *buf = LIST_HEAD(sock->rx_q);
    if(flags & QMSG_PEEK)
    {
      rc = (int)buf->len;
    }
    else if(len < buf->len && (flags & QMSG_TRUNC) == 0)
    {
      rc = QEMSGSIZE;
    }
    else
    {
      len = buf->len < len ? buf->len : len;
      /* Recv into provided buffer */
      ipc_router_packet_read(&buf->pkt, buffer, len);
      ipc_router_packet_free(&buf->pkt);

      /* Send conf if required */
      if(buf->confirm_rx)
      {
        ipc_router_core_send_conf(sock->handle, &buf->addr);
      }

      /* Fill up sender's address */
      if(addr)
      {
        addr->sa_family = AF_IPC_ROUTER;
        addr->address.addrtype = IPCR_ADDR_PORT;
        addr->address.addr.port_addr.node_id = (uint32_t)buf->addr.processor_id;
        addr->address.addr.port_addr.port_id = (uint32_t)buf->addr.port_id;
      }

      rc = (int)buf->len;
      LIST_REMOVE(sock->rx_q, buf, link);
      ipc_router_os_free(buf);
    }
  }
  else
  {
    if(sock->connected < 0)
    {
      rc = QECONNRESET;
    }
    else if(!(flags & QMSG_DONTWAIT))
    {
      rc = QETIMEDOUT;
    }
    else
    {
      rc = QENOMSG;
    }
  }
  ipc_router_os_sem_unlock(&sock->rx_q_lock);

  put_sock(sock);
  return rc;
}

/*===========================================================================
  FUNCTION  qrecv
===========================================================================*/
int qrecv(int fd, void *buf, size_t len, int flags)
{
  return qrecvfrom(fd, buf, len, flags, NULL, NULL);
}

/*===========================================================================
  FUNCTION  qbind
===========================================================================*/
int qbind(int fd, struct qsockaddr *_addr, qsocklen_t addrlen)
{
  ipcr_addr_t *addr = (ipcr_addr_t *)_addr;
  ipcr_sock_t *sock;
  ipc_router_service_type service;
  int rc;

  if(0 != (rc = validate_addr(addr, addrlen, IPCR_ADDR_NAME_SUPP)))
  {
    return rc;
  }

  sock = get_sock(fd);
  if(!sock)
  {
    return QEBADF;
  }
  service.type = addr->address.addr.port_name.service;
  service.instance = addr->address.addr.port_name.instance;
  if(sock->type != QSOCK_DGRAM && sock->type != QSOCK_STREAM)
  {
    put_sock(sock);
    return QEOPNOTSUPP;
  }
  rc = ipc_router_core_reg_server(sock->handle, &service);
  put_sock(sock);
  return err_ipcr_to_sock(rc);
}

/*===========================================================================
  FUNCTION  qpoll
===========================================================================*/
int qpoll(struct qpollfd *pfd, qnfds_t num, int timeout_ms)
{
  ipcr_waiter_t waiter;
  int ret = 0;
  size_t i;

  if(!pfd || !num)
  {
    return QEINVAL;
  }

  /* Add to waiters list so there is no
   * race to "check" vs wait */
  ipc_router_os_sem_lock(&waiters_list_lock);
  LINK_INIT(waiter.link);
  waiter.pfd = pfd;
  waiter.num = num;
  waiter.addr = NULL;
  IPCR_OS_SIGNAL_INIT(&waiter.signal, timeout_ms);
  LIST_ADD(g_waiters_list, &waiter, link);

  while(ret == 0)
  {
    IPCR_OS_SIGNAL_CLEAR(&waiter.signal);
    for(i = 0; i < num; i++)
    {
      ipcr_sock_t *sock;

      pfd[i].revents = 0;
      if(pfd[i].fd < 0)
      {
        continue;
      }

      sock = get_sock(pfd[i].fd);
      if(sock == NULL)
      {
        pfd[i].revents |= QPOLLNVAL;
        ret = QEBADF;
        break;
      }

      if(sock->connected == -1)
      {
        pfd[i].revents |= QPOLLHUP;
        ret += 1;
        put_sock(sock);
        continue;
      }

      if(pfd[i].events & QPOLLIN)
      {
        ipc_router_os_sem_lock(&sock->rx_q_lock);
        if(LIST_CNT(sock->rx_q) > 0)
        {
          pfd[i].revents |= QPOLLIN;
        }
        ipc_router_os_sem_unlock(&sock->rx_q_lock);
      }

      if(pfd[i].events & QPOLLOUT)
      {
        if(sock->connected == 1)
        {
          /* TODO Check write status for sock->conn.addr */
        }
        else
        {
          /* We always allow writes for unconnected
           * sockets */
          pfd[i].revents |= QPOLLOUT;
        }
      }
      ret += pfd[i].revents == 0 ? 0 : 1;
      put_sock(sock);
    }
    if(ret || timeout_ms == 0)
    {
      break;
    }

    /* Wait without holding the waiter lock */
    ipc_router_os_sem_unlock(&waiters_list_lock);
    IPCR_OS_SIGNAL_WAIT(&waiter.signal, timeout_ms);
    ipc_router_os_sem_lock(&waiters_list_lock);
    if(IPCR_OS_SIGNAL_TIMED_OUT(&waiter.signal))
    {
      break;
    }
  }

  LIST_REMOVE(g_waiters_list, &waiter, link);
  IPCR_OS_SIGNAL_DEINIT(&waiter.signal);
  ipc_router_os_sem_unlock(&waiters_list_lock);
  return ret;
}


int qgetsockopt(int fd, int level, int optname, 
                  void *optval, qsocklen_t *optlen)
{
  return QEOPNOTSUPP;
}


int qsetsockopt(int fd, int level, int optname, 
                 void *optval, qsocklen_t optlen)
{
  ipcr_sock_t *sock;
  int rc = QEOPNOTSUPP;

  sock = get_sock(fd);
  if(!sock)
  {
    return QEBADF;
  }

  switch(level)
  {
    case QSOL_SOCKET:
      switch(optname)
      {
        case QSO_SNDTIMEO:
          if(!optval || optlen < sizeof(int))
          {
            rc = QEINVAL;
            break;
          }
          sock->write_timeout = *((int *)optval);
          rc = 0;
          break;
        case QSO_RCVTIMEO:
          if(!optval || optlen < sizeof(int))
          {
            rc = QEINVAL;
            break;
          }
          sock->read_timeout = *((int *)optval);
          rc = 0;
          break;
        default:
          rc = QEOPNOTSUPP;
          break;
      }
      break;
    case QSOL_IPC_ROUTER:
      switch(optname)
      {
        case QSO_IPCR_SET_CONTROL_PORT:
          if(sock->connected == 0 && 
              LIST_CNT(sock->rx_q) == 0)
          {
            sock->active = ACTIVE_RX;
            sock->type = QSOCK_CTRL;
            rc = 0;
          }
          break;
        case QSO_IPCR_SET_SERVICE_PREF:
          if(sock->type != QSOCK_CTRL)
          {
            rc = QEOPNOTSUPP;
            break;
          }
          if(!optval || optlen < sizeof(uint32_t))
          {
            rc = QEINVAL;
            break;
          }
          sock->conn.name.type = *((uint32 *)optval);
          sock->conn.name.instance = 0xffffffff;
          sock->connected = 1;
          rc = 0;
          break;
        default:
          rc = QEOPNOTSUPP;
          break;
      }
      break;
    default:
      rc = QEOPNOTSUPP;
      break;
  }
  put_sock(sock);
  return rc;
}

/*===========================================================================
  FUNCTION  ipcr_find_name
===========================================================================*/
int ipcr_find_name(int fd, ipcr_name_t *name, struct qsockaddr_ipcr *addrs, 
                    unsigned int *instances, unsigned int *io_num_entries, 
                    unsigned int flags)
{
  int rc;
  uint32 instance_mask = (flags & IPCR_FLAGS_ANY_INSTANCE) ? 0 : 0xffffffff;
  ipc_router_service_type service;
  uint32 num_entries = 0;
  uint32 num_servers = 0;
  uint32 i;
  ipc_router_server_type *servers = NULL;

  if(!name)
  {
    return QEINVAL;
  }
  if(io_num_entries)
  {
    num_entries = (unsigned int)*io_num_entries;
    if(num_entries > 0 && !addrs)
    {
      return QEINVAL;
    }
  }

  service.type = (uint32)name->service;
  service.instance = (uint32)name->instance;

  if(num_entries > 0)
  {
    servers = ipc_router_os_malloc(num_entries * sizeof(*servers));
    if(!servers)
    {
      return QENOMEM;
    }
  }

  rc = ipc_router_core_find_all_servers(NULL, &service, servers, &num_entries,
                            &num_servers, instance_mask);

  if(rc == IPC_ROUTER_STATUS_SUCCESS)
  {
    rc = (int)num_servers;

    if(servers)
    {
      for(i = 0; i < num_entries; i++)
      {
        addrs[i].sa_family = AF_IPC_ROUTER;
        addrs[i].address.addrtype = IPCR_ADDR_PORT;
        addrs[i].address.addr.port_addr.node_id = (uint32_t)servers[i].addr.processor_id;
        addrs[i].address.addr.port_addr.port_id = (uint32_t)servers[i].addr.port_id;
        if(instances)
          instances[i] = servers[i].service.instance;
      }
    }
    else
    {
      num_entries = 0;
    }
  }
  else
  {
    rc = 0;
  }

  if(io_num_entries)
  {
    *io_num_entries = (unsigned int)num_entries;
  }

  if(servers)
  {
    ipc_router_os_free(servers);
  }

  return rc;
}

int ipcr_find_sname(int fd, char *name, struct qsockaddr_ipcr *addrs,
                    unsigned int *num_entries, unsigned int flags)
{
  return QEOPNOTSUPP;
}

