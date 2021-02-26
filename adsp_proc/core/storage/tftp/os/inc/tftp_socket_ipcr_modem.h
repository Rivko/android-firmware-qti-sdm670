/***********************************************************************
 * tftp_socket_ipcr_modem.h
 *
 * Short description
 * Copyright (c) 2014 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * Verbose description.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/storage/tftp/os/inc/tftp_socket_ipcr_modem.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2014-07-18   rp    tftp and ipc-router integration changes from target.
2014-06-30   nr    Support connected sockets and muti-poll.
2014-06-04   rp    Create

===========================================================================*/

#ifndef __TFTP_SOCKET_IPCR_MODEM_H__
#define __TFTP_SOCKET_IPCR_MODEM_H__

#include "tftp_config_i.h"
#include "qsocket.h"
#include "qsocket_ipcr.h"

struct tftp_modem_socket
{
  int sock_fd;
  int error_num;
};

typedef struct tftp_modem_socket tftp_socket;
typedef struct qsockaddr_ipcr tftp_sockaddr;
typedef qsocklen_t tftp_sockaddr_len_type;
typedef struct qpollfd tftp_socket_pollfd;


#endif /* not __TFTP_SOCKET_IPCR_MODEM_H__ */
