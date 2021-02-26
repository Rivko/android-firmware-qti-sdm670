/***********************************************************************
 * tftp_socket_ipcr_modem.c
 *
 * NHLOS Abstraction for Socket APIs.
 *
 * Copyright (c) 2014-2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1.c4/storage/tftp/os/src/tftp_socket_ipcr_modem.c#1 $ $DateTime: 2018/07/30 01:21:36 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-01-06   vm    Compile TFTP server on TN Apps.
2014-10-14   rp    Use asserts for control-logic, debug-asserts for data-logic
2014-10-14   dks   Move OS specific arguments out of tftp_server.
2014-07-28   rp    Incorporate ipc-router api changes.
2014-07-18   rp    tftp and ipc-router integration changes from target.
2014-06-30   nr    Support connected sockets and muti-poll.
2014-06-11   rp    Renamed DEBUG_ASSERT as TFTP_DEBUG_ASSERT
2014-06-04   rp    Create

===========================================================================*/

#include "tftp_config_i.h"
#include "tftp_socket.h"
#include "tftp_socket_ipcr_modem.h"
#include "qsocket.h"
#include "qsocket_ipcr.h"
#include "tftp_assert.h"
#include "tftp_log.h"
#include "tftp_os_modem.h"
#include <string.h>


/* Default flags to use while sendto/recvfrom */
#define TFTP_QSOCKET_IPCR_FLAGS_DEFAULT  0

enum tftp_socket_result_type
tftp_socket_open (tftp_socket *socket_handle, int do_bind)
{
  int sock_fd;
  enum tftp_socket_result_type sock_res;

  (void) do_bind;

  sock_fd = qsocket (AF_IPC_ROUTER, QSOCK_DGRAM, 0);

  if (sock_fd >= 0)
  {
    socket_handle->sock_fd = sock_fd;
    sock_res = TFTP_SOCKET_RESULT_SUCCESS;
  }
  else
  {
    socket_handle->sock_fd = -1;
    socket_handle->error_num = -sock_fd;
    sock_res = TFTP_SOCKET_RESULT_FAILED;
  }

  return sock_res;
}


enum tftp_socket_result_type
tftp_socket_close (tftp_socket *socket_handle)
{
  enum tftp_socket_result_type sock_res = TFTP_SOCKET_RESULT_FAILED;
  int result;

  result = qclose (socket_handle->sock_fd);
  if (result >= 0)
  {
    socket_handle->sock_fd = -1;
    sock_res = TFTP_SOCKET_RESULT_SUCCESS;
  }
  else
  {
    socket_handle->sock_fd = -1;
    socket_handle->error_num = -result;
    sock_res = TFTP_SOCKET_RESULT_FAILED;
  }
  return sock_res;
}

int tftp_socket_get_last_errno (tftp_socket *socket_handle)
{
  return socket_handle->error_num;
}

enum tftp_socket_result_type
tftp_socket_set_addr_by_name (tftp_sockaddr *sockaddr,
                              uint32 service_id, uint32 instance_id)
{
  memset(sockaddr, 0, sizeof(tftp_sockaddr));
  sockaddr->sa_family = AF_IPC_ROUTER;
  sockaddr->address.addrtype = IPCR_ADDR_NAME;
  sockaddr->address.addr.port_name.service = service_id;
  sockaddr->address.addr.port_name.instance = instance_id;
  return TFTP_SOCKET_RESULT_SUCCESS;
}

enum tftp_socket_result_type
tftp_socket_set_addr_by_port (tftp_sockaddr *sockaddr,
                              uint32 proc_id, uint32 port_id)
{
  memset (sockaddr, 0, sizeof(tftp_sockaddr));
  sockaddr->sa_family = AF_IPC_ROUTER;
  sockaddr->address.addrtype = IPCR_ADDR_PORT;
  sockaddr->address.addr.port_addr.node_id = proc_id;
  sockaddr->address.addr.port_addr.port_id = port_id;
  return TFTP_SOCKET_RESULT_SUCCESS;
}

enum tftp_socket_result_type
tftp_socket_get_addr_by_port (tftp_socket *socket_handle,
                              tftp_sockaddr *sockaddr,
                              uint32 *proc_id, uint32 *port_id)
{
  (void) socket_handle;

  TFTP_ASSERT (sockaddr != NULL);
  TFTP_ASSERT (proc_id != NULL);
  TFTP_ASSERT (port_id != NULL);

  if ((socket_handle == NULL) ||
      (sockaddr == NULL) ||
      (proc_id == NULL) ||
      (port_id == NULL))
  {
    return TFTP_SOCKET_RESULT_FAILED;
  }

  TFTP_ASSERT (sockaddr->sa_family == AF_IPC_ROUTER);
  TFTP_ASSERT (sockaddr->address.addrtype == IPCR_ADDR_PORT);
  *proc_id = sockaddr->address.addr.port_addr.node_id;
  *port_id = sockaddr->address.addr.port_addr.port_id;

  return TFTP_SOCKET_RESULT_SUCCESS;
}

enum tftp_socket_result_type
tftp_socket_connect(tftp_socket *socket_handle, tftp_sockaddr *remote_addr)
{
  enum tftp_socket_result_type sock_res;
  int32 result;

  result = qconnect (socket_handle->sock_fd, (struct qsockaddr *)remote_addr,
                            sizeof (tftp_sockaddr));
  if (result >= 0)
  {
    sock_res = TFTP_SOCKET_RESULT_SUCCESS;
  }
  else
  {
    socket_handle->error_num = -result;
    sock_res = TFTP_SOCKET_RESULT_FAILED;
  }

  return sock_res;
}

enum tftp_socket_result_type
tftp_socket_bind (tftp_socket *socket_handle, tftp_sockaddr *local_addr)
{
  enum tftp_socket_result_type sock_res = TFTP_SOCKET_RESULT_FAILED;
  int result;

  result = qbind (socket_handle->sock_fd, (struct qsockaddr *)local_addr,
                  sizeof(tftp_sockaddr));
  if (result == 0)
  {
    sock_res = TFTP_SOCKET_RESULT_SUCCESS;
  }

  return sock_res;
}

enum tftp_socket_result_type
tftp_socket_sendto (tftp_socket *socket_handle, void *buffer,
                       int32 buffer_size, tftp_sockaddr *remote_addr,
                       int32 *sent_size)
{
  enum tftp_socket_result_type sock_res;
  struct qsockaddr_ipcr resolved_addr;
  ipcr_name_t sockaddr_name;
  unsigned int num_entries;
  int32 result;

  TFTP_ASSERT (remote_addr->address.addrtype == IPCR_ADDR_NAME);

  num_entries = 1;
  sockaddr_name.service = remote_addr->address.addr.port_name.service;
  sockaddr_name.instance = remote_addr->address.addr.port_name.instance;
  result = ipcr_find_name (socket_handle->sock_fd, &sockaddr_name,
                           &resolved_addr, NULL, &num_entries, 0);
  if (result <= 0)
  {
    if (result == 0)
    {
      TFTP_LOG_ERR ("could not resolve remote host");
      *sent_size = buffer_size;
      sock_res = TFTP_SOCKET_RESULT_SUCCESS;
    }
    else
    {
      sock_res = TFTP_SOCKET_RESULT_FAILED;
      socket_handle->error_num = -result;
    }
    goto End;
  }

  TFTP_ASSERT (resolved_addr.address.addrtype == IPCR_ADDR_PORT);

  result = qsendto (socket_handle->sock_fd, buffer, buffer_size,
                    TFTP_QSOCKET_IPCR_FLAGS_DEFAULT,
                    (struct qsockaddr *)&resolved_addr,
                    sizeof (tftp_sockaddr));

  if (result < 0)
  {
    socket_handle->error_num = -result;
    sock_res = TFTP_SOCKET_RESULT_FAILED;
  }
  else
  {
    *sent_size = result;
    sock_res = TFTP_SOCKET_RESULT_SUCCESS;
  }

 End:
  return sock_res;
}

enum tftp_socket_result_type
tftp_socket_send (tftp_socket *socket_handle, void *buffer, int32 buffer_size,
                  int32 *sent_size)
{
  enum tftp_socket_result_type sock_res;
  int32 result;

  result = qsend (socket_handle->sock_fd, buffer, buffer_size,
                    TFTP_QSOCKET_IPCR_FLAGS_DEFAULT);
  if (result < 0)
  {
    socket_handle->error_num = -result;
    sock_res = TFTP_SOCKET_RESULT_FAILED;
  }
  else
  {
    *sent_size = result;
    sock_res = TFTP_SOCKET_RESULT_SUCCESS;
  }

  return sock_res;
}

int tftp_socket_poll(tftp_socket_pollfd *pfd, uint32 pfd_count,
                     int32 timeout_in_ms)
{
  int poll_res;
  uint32 i;

  TFTP_ASSERT (pfd != NULL);
  TFTP_ASSERT (pfd_count > 0);
  if ((pfd == NULL) ||
      (pfd_count == 0))
  {
    return -(EINVAL);
  }

  for (i = 0; i < pfd_count; i++)
  {
    pfd[i].events = QPOLLIN;
    pfd[i].revents = 0;
  }

  poll_res = qpoll (pfd, pfd_count, timeout_in_ms);
  return poll_res;
}

enum tftp_socket_result_type
tftp_socket_interpret_poll_result(tftp_socket_pollfd *pfd, int poll_res)
{
  enum tftp_socket_result_type sock_res;

  if(poll_res < 0)
  {
    sock_res = TFTP_SOCKET_RESULT_FAILED;
  }
  else if(poll_res == 0)
  {
    sock_res = TFTP_SOCKET_RESULT_TIMEOUT;
  }
  else if (pfd->revents & QPOLLIN)
  {
    sock_res = TFTP_SOCKET_RESULT_SUCCESS;
  }
  else
  {
    sock_res = TFTP_SOCKET_RESULT_HANGUP;
  }

  return sock_res;
}

static enum tftp_socket_result_type
tftp_socket_wait(tftp_socket *socket_handle, int32 timeout_in_ms)
{
  tftp_socket_pollfd pfd[1];
  enum tftp_socket_result_type sock_res;
  int poll_res;

  pfd[0].fd = socket_handle->sock_fd;

  poll_res = tftp_socket_poll (pfd, 1, timeout_in_ms);
  sock_res = tftp_socket_interpret_poll_result (&pfd[0], poll_res);
  if (sock_res == TFTP_SOCKET_RESULT_FAILED)
  {
    TFTP_LOG_DBG ("poll failed : %d", -poll_res);
    socket_handle->error_num = -poll_res;
  }

  if (sock_res != TFTP_SOCKET_RESULT_SUCCESS)
  {
    switch (sock_res)
    {
      case TFTP_SOCKET_RESULT_FAILED:
      {
        socket_handle->error_num = -(poll_res);
        break;
      }
      case TFTP_SOCKET_RESULT_TIMEOUT:
      {
        socket_handle->error_num = ETIMEDOUT;
        break;
      }
      case TFTP_SOCKET_RESULT_HANGUP:
      {
        socket_handle->error_num = ENODEV;
        break;
      }
      default:
      {
        TFTP_ASSERT (0);
      }
    }
  }

  return sock_res;
}


enum tftp_socket_result_type
tftp_socket_recvfrom (tftp_socket *socket_handle, void *buffer,
                       int32 buffer_size, tftp_sockaddr *remote_addr,
                       uint32 timeout_in_ms, int32 *recd_size)
{
  enum tftp_socket_result_type sock_res;
  int32 result;
  tftp_sockaddr_len_type size_of_addr = sizeof (tftp_sockaddr);

  if (timeout_in_ms > 0)
  {
    sock_res = tftp_socket_wait (socket_handle, timeout_in_ms);
    if (sock_res != TFTP_SOCKET_RESULT_SUCCESS)
    {
      goto End;
    }
  }

  result = qrecvfrom (socket_handle->sock_fd, buffer, buffer_size,
                      TFTP_QSOCKET_IPCR_FLAGS_DEFAULT,
                      (struct qsockaddr *)remote_addr, &size_of_addr);
  if (result < 0)
  {
    socket_handle->error_num = -result;
    sock_res = TFTP_SOCKET_RESULT_FAILED;
  }
  else
  {
    *recd_size = result;
    sock_res = TFTP_SOCKET_RESULT_SUCCESS;
  }

End:
  return sock_res;
}

enum tftp_socket_result_type
tftp_socket_recv (tftp_socket *socket_handle, void *buffer, int32 buffer_size,
                  uint32 timeout_in_ms, int32 *recd_size)
{
  enum tftp_socket_result_type sock_res;
  int32 result;

  if (timeout_in_ms > 0)
  {
    sock_res = tftp_socket_wait(socket_handle, timeout_in_ms);
    if (sock_res != TFTP_SOCKET_RESULT_SUCCESS)
    {
      goto End;
    }
  }

  result = qrecv(socket_handle->sock_fd, buffer, buffer_size,
                  TFTP_QSOCKET_IPCR_FLAGS_DEFAULT);
  if (result < 0)
  {
    socket_handle->error_num = -result;
    sock_res = TFTP_SOCKET_RESULT_FAILED;
  }
  else
  {
    *recd_size = result;
    sock_res = TFTP_SOCKET_RESULT_SUCCESS;
  }

End:
  return sock_res;
}

int
tftp_socket_get_socket_desc (tftp_socket *socket_handle)
{
  TFTP_ASSERT (socket_handle != NULL);
  return socket_handle->sock_fd;
}

enum tftp_socket_result_type
tftp_socket_invalidate_socket_handle (tftp_socket *socket_handle)
{
  TFTP_ASSERT (socket_handle != NULL);
  socket_handle->sock_fd = -1;
  socket_handle->error_num = 0;
  return TFTP_SOCKET_RESULT_SUCCESS;
}


