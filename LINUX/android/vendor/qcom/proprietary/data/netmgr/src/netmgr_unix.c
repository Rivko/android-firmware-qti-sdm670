/******************************************************************************

                        N E T M G R _ U N I X . C

******************************************************************************/

/******************************************************************************

  @file    netmgr_unix.c
  @brief   Network Manager UNIX domain sockets implementation file

  DESCRIPTION
  Implementation file for NetMgr UNIX module.

******************************************************************************/
/*===========================================================================

  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include <sys/socket.h>
#include <sys/un.h>
#include <poll.h>

#include "netmgr_util.h"
#include "netmgr_unix.h"

/*===========================================================================
                     LOCAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

/*---------------------------------------------------------------------------
   Constant representing number of file descriptors to poll for
---------------------------------------------------------------------------*/
#define NETMGR_UNIX_NUM_POLL_FDS  1

/*---------------------------------------------------------------------------
   Constant representing maximum packet size to be printed
---------------------------------------------------------------------------*/
#define NETMGR_UTIL_MAX_PKT_BUFFER_SIZE  (2048)

/*===========================================================================
  FUNCTION  netmgr_util_print_packet
===========================================================================*/
/*!
@brief
  Prints a pkt of size pktlen bytes. If pktlen is greater than 2048,
  then the first 2048 bytes are printed. If pktlen is 0 or lesser, or if
  the buffer is NULL, we do not print anything.

@return
  int - NETMGR_SUCCESS on operation success, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
static
void netmgr_util_print_packet
(
  char  *pkt,
  int   pktlen,
  char  *direction
)
{
  char buffer[NETMGR_UTIL_MAX_PKT_BUFFER_SIZE];
  int i = 0;
  int printlen = 0;
  int buffloc = 0;

  if ((pkt == NULL) || (direction == NULL))
  {
    netmgr_log_err("%s(): Input arguments are NULL", __func__);
    return;
  }

  if (pktlen > NETMGR_UTIL_MAX_PKT_BUFFER_SIZE)
  {
    printlen = NETMGR_UTIL_MAX_PKT_BUFFER_SIZE;
  }
  else if (pktlen <= 0)
  {
    netmgr_log_err("%s(): Invalid packet length - %d", __func__, pktlen);
    return;
  }
  else
  {
    printlen = pktlen;
  }

  netmgr_log_low("%s(): Packet length is %d, printing first %d bytes",
                 __func__, pktlen, printlen);

  memset(buffer, 0, sizeof(buffer));
  for (i = 0; i < printlen; i++)
  {
    if ((i%16) == 0)
    {
      netmgr_log_low("[%s] %s", direction, buffer);
      memset(buffer, 0, sizeof(buffer));
      buffloc = 0;
      buffloc += snprintf(&buffer[buffloc], sizeof(buffer)-buffloc, "%04X:", i);
    }

    buffloc += snprintf(&buffer[buffloc], sizeof(buffer)-buffloc, " %04X", pkt[i]);
  }
  netmgr_log_low("[%s] %s", direction, buffer);
}

/*===========================================================================
                     GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/

/*===========================================================================
  FUNCTION  netmgr_unix_ipc_send_msg
===========================================================================*/
/*!
@brief
  Function to send unicast NETLINK messages to netmgr and to receive the
  responses from netmgr. This is interface for clients of netmgr to
  communicate with netmgr

@return
  int - NETMGR_SUCCESS on successful operation, NETMGR_FAILURE otherwise

@note

  - Dependencies
    - None

  - Side Effects
    - None
*/
/*=========================================================================*/
int
netmgr_unix_ipc_send_msg
(
  char           *req_buffer,
  int            req_bufflen,
  char           **resp_buffer,
  unsigned int   *resp_bufflen,
  int            resp_timeout
)
{
  int                 unix_sock_fd = NETMGR_UNIX_API_ERROR;
  int                 len = NETMGR_UNIX_API_ERROR;
  int                 rc = NETMGR_FAILURE;
  int                 str_len = NETMGR_UNIX_API_ERROR;
  struct sockaddr_un  client_socket;
  struct pollfd       pollinfo;

  memset(&pollinfo, 0, sizeof(struct pollfd));

  if ((req_buffer == NULL) || (req_bufflen <= 0) || (resp_buffer == NULL) || (resp_buffer == NULL)
    || (*resp_buffer == NULL) || (resp_bufflen == NULL))
  {
    netmgr_log_err("%s(): Invalid input arguments", __func__);
    goto exit;
  }

  if ((unix_sock_fd = socket(AF_UNIX, SOCK_STREAM | SOCK_CLOEXEC, 0)) == NETMGR_UNIX_API_ERROR)
  {
      netmgr_log_err("%s(): Unable to create UNIX socket - %s", __func__, strerror(errno));
      goto exit;
  }
  netmgr_log_med("%s(): Successfully created UNIX socket", __func__);

  client_socket.sun_family = AF_UNIX;
  str_len = strlcpy(client_socket.sun_path, NETMGR_UNIX_CLIENT_CONN_SOCK_PATH, UNIX_PATH_MAX);
  if ((str_len <= NETMGR_UNIX_API_ERROR) && (str_len > UNIX_PATH_MAX))
  {
    netmgr_log_err("%s(): String truncation occurred", __func__);
    goto close_socket;
  }

  len = strlen(client_socket.sun_path) + sizeof(client_socket.sun_family);
  if (connect(unix_sock_fd, (struct sockaddr *)&client_socket, len) == NETMGR_UNIX_API_ERROR)
  {
    netmgr_log_err("%s(): Unable to connect to UNIX socket - %s", __func__, strerror(errno));
    goto close_socket;
  }
  netmgr_log_med("%s(): Successfully connected to UNIX socket", __func__);

  netmgr_util_print_packet(req_buffer, req_bufflen, "Tx");
  if (send(unix_sock_fd, req_buffer, req_bufflen, MSG_DONTWAIT | MSG_NOSIGNAL ) == NETMGR_UNIX_API_ERROR)
  {
    netmgr_log_err("%s(): Failed to send message to UNIX socket - %s", __func__, strerror(errno));
    goto close_socket;
  }
  netmgr_log_med("%s(): Successfully sent message to UNIX socket", __func__);
  rc = NETMGR_SUCCESS;

poll_socket:
  pollinfo.events = POLLIN | POLLNVAL | POLLERR | POLLHUP;
  pollinfo.fd = unix_sock_fd;
  if (poll(&pollinfo, NETMGR_UNIX_NUM_POLL_FDS, resp_timeout)
    == NETMGR_UNIX_API_ERROR)
  {
    netmgr_log_err("%s(): Error occurred while polling UNIX socket - %s",
                   __func__, strerror(errno));

   if (errno == EINTR)
   {
     netmgr_log_high("%s(): Polling UNIX socket again - %s", __func__, strerror(errno));
     goto poll_socket;
   }

    goto close_socket;
  }
  if (!(pollinfo.revents & POLLIN))
  {
    netmgr_log_err("%s(): Invalid event was received in UNIX socket - %d",
                   __func__, pollinfo.revents);
    goto close_socket;
  }

  *resp_bufflen = recv(unix_sock_fd, *resp_buffer, NETMGR_UNIX_MAX_MESSAGE_SIZE, 0);
  netmgr_log_med("%s(): Successfully received a message from UNIX socket", __func__);
  if(((int) *resp_bufflen) == NETMGR_UNIX_API_ERROR)
  {
    netmgr_log_err("%s(): Error in recv in UNIX socket - %s", __func__, strerror(errno));
    goto close_socket;
  }

  if(*resp_bufflen == 0)
  {
    netmgr_log_err("%s(): Server closed UNIX socket", __func__);
    goto close_socket;
  }
  netmgr_util_print_packet(*resp_buffer, *resp_bufflen, "Rx");

close_socket:
  close(unix_sock_fd);
exit:
  return rc;
}
