/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    Diag I/O Wrapper APIs for Interprocessor Communication and I/O

General Description

Copyright (c) 2015-2017 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagcomm_io_udp.c#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
08/29/17    gn     Changes to handle remote socket close
06/21/17    sp     Fix compiler error in function prototype
05/23/16    gn     Fix for DCI_Override issue
05/12/16    ph     SMD references removal and code clean up.
05/03/16    ph     Changed UDP_WRITE_TIMEOUT to infinite.
04/21/16    gn     diag_pfd cleanup on channel close
12/07/15    ph     Exit polling for service if we are unable to get the address
                   after the timeout duration elapsed.
07/20/15    sr     Increased the timeout value to get the server address to
                   200ms
05/29/15    sa     Increased the control channel send timeout to default
                   timeout of 5seconds
04/08/15    ps     Created file to provide DIAG over sockets

===========================================================================*/
#include "diagcomm_io.h"
#include "diagcomm_io_udp.h"
#include "diagcomm_io_socket.h"
#include "assert.h"
#include "msg_diag_service.h"
#include <stdio.h>
#include <stdlib.h>
#include "diag_v.h"
#include "diagcomm_v.h"
#include "stringl.h"
#include "diag_cfg.h"
#include "time_svc.h"
#include "osal.h"
#include "qsocket_ids.h"

#define THREAD_TASK_STACK_SIZE 4*1024 //4 //4k stacksize


void diagcomm_io_udp_receive_data(void);
void diagcomm_io_udp_thread_main(void *data);

#define DIAG_CHANNEL_READ_BUF_SIZE 8*1024
#define DIAG_UDP_TIMEOUT_TIMER_SIG       0x00100000

osal_tcb_t *read_task_tcb;
osal_timer_t read_task_block_timer;


static uint32 diag_pfds_index = 0;
static struct qpollfd diag_pfds[DIAG_MAX_NUM_SOCKETS];
static diagcomm_io_conn_type *diag_sock_conn[DIAG_MAX_NUM_SOCKETS];


/*===========================================================================
FUNCTION diagcomm_io_udp_get_srv_addr

DESCRIPTION
  This API gets the server address based on the port name


PARAMETERS
  name: remote port name
  srv_addr: (output) server address
  timeout: timeout to qpoll to look for the remote address

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  rc=0         : SUCCESS
  rc >0 or <0  : Cannot find remote server

===========================================================================*/
int diagcomm_io_udp_get_srv_addr (ipcr_name_t name, struct qsockaddr_ipcr *srv_addr, int timeout)
{
  int ctrl_fd, rc = 0;
  struct qpollfd pfd;

  /* Create a socket */
  ctrl_fd = qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);

  if(ctrl_fd < 0)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Control FD creation failed rc = %d!\n",ctrl_fd);
    return -1;
  }
  /* Mark this as a control socket to listen on events
   * on the name server */
  rc = qsetsockopt(ctrl_fd, QSOL_IPC_ROUTER, QSO_IPCR_SET_CONTROL_PORT, NULL, 0);
  if(rc)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Setting socket option failed with rc = %d\n", rc);
    qclose(ctrl_fd);
    return -1;
  }

  rc = qsetsockopt(ctrl_fd, QSOL_IPC_ROUTER, QSO_IPCR_SET_SERVICE_PREF, &name.service, sizeof(name.service));
  if(rc)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Setting socket type pref failed with rc= %d\n", rc);
    qclose(ctrl_fd);
    return -1;
  }

  pfd.fd = ctrl_fd;
  pfd.events = QPOLLIN;
  pfd.revents = 0;

  while(1)
  {
    ipcr_ctrl_msg msg;
    unsigned int num_entries = 1;
    rc = ipcr_find_name(ctrl_fd, &name, srv_addr, NULL, &num_entries, 0);
    if(rc > 0)
    {
      MSG_2(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Got service with addr <%u,%u>\n",
                srv_addr->address.addr.port_addr.node_id,
                srv_addr->address.addr.port_addr.port_id);
      rc = 0;
      break;
    }

    rc = qpoll(&pfd, 1, timeout);
    if(rc < 0)
    {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "poll returned error = %d\n", rc);
      break;
    }
    else if(rc == 0)
    {
      MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"poll timedout!\n");
      rc = -1;
      break;
    }
    else
    {
      while(qrecv(ctrl_fd, &msg, sizeof(msg), QMSG_DONTWAIT) > 0)
      {
        /* Do nothing */
      }
    }

  }
  qclose(ctrl_fd);
  return rc;
}


/*===========================================================================
FUNCTION diagcomm_io_udp_open

DESCRIPTION
  Open the socket connection for the connection object.
This function should be called to open up the UDP connection. The same socket will be used to send or receive data
for the channel.

PARAMETERS
  conn - Connection object for the channel.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  DIAG_EOK : SUCCESS
  DIAG_EFAILED: Opening of socket connection failed
===========================================================================*/
int diagcomm_io_udp_open(diagcomm_io_conn_type * conn)
{
   int sock = -1;
   struct qsockaddr_ipcr srv_addr;
   int rc = 0;
   int wr_timeout = UDP_WRITE_TIMEOUT; //5000 milliseconds

  //Open CMD/Data/DCI CMD/DCI Data channels
  if(diag_pfds_index >= DIAG_MAX_NUM_SOCKETS)
  {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_FATAL,"diagcomm_io_udp_open: Number of sockets opened exceeds limit: %d", diag_pfds_index);
     return DIAG_ELIMIT;
  }

  if(! conn->isServer)
  {
      if(diagcomm_io_udp_get_srv_addr(conn->addr.address.addr.port_name, &srv_addr, wr_timeout)) //Timeout 20 ms.
     {
       MSG(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Getting server addr failed!\n");
       return DIAG_EFAILED;
     }
  }

  sock = qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);
  if (sock < 0)
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"Could not create UDP socket - %d", sock);
    return DIAG_EFAILED;
  }

  if(0 == conn->send_flags)
  {
     rc = qsetsockopt(sock, QSOL_SOCKET, QSO_SNDTIMEO, &wr_timeout, sizeof(wr_timeout));
     if(rc)
     {
       MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "Setting socket option failed with rc = %d\n", rc);
       qclose(sock);
       return DIAG_EFAILED;
     }
  }
  if(! conn->isServer)
  {
     rc = qconnect(sock, (struct qsockaddr *)&srv_addr, sizeof(srv_addr));
     if(rc < 0)
     {
       MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Connect returned %d\n", rc);
       return DIAG_EFAILED;
     }
  }
  else
  {
     rc = qbind(sock, (struct qsockaddr *)&conn->addr, sizeof(conn->addr));
     if(rc < 0)
     {
	    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH,"Failed to bind address! Error code = %d\n",rc);
        return DIAG_EFAILED;
     }
  }
  conn->sock_handle = sock;
  conn->connected = TRUE;
  if(conn->isServer)
  {
     conn->channel_connected = TRUE;
  }

  diagcomm_io_conn_changed( conn , TRUE);

  if((NULL == conn->buf) && (0 != conn->buf_length))
  {
     conn->buf = malloc(conn->buf_length);
  }

  if(diag_pfds_index < DIAG_MAX_NUM_SOCKETS)
  {
    diag_pfds[diag_pfds_index].fd = sock;
    diag_pfds[diag_pfds_index].events = QPOLLIN;
    diag_pfds[diag_pfds_index].revents = 0;
    diag_sock_conn[diag_pfds_index] = conn;

    diag_pfds_index++;
  }

  return DIAG_EOK;

}

/*===========================================================================
FUNCTION diagcomm_io_udp_open_init

DESCRIPTION
  Initialize the UDP read thread. If the thread cannot be created we assert since there is no recovery.

PARAMETERS
  conn - Connection object for the channel.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  None
===========================================================================*/
void diagcomm_io_udp_open_init(diagcomm_io_conn_type * conn)
{
   char thread_name[16];
   int return_val = 0;
   (void)snprintf( thread_name, sizeof(thread_name), "diagcomm_udp");

   read_task_tcb = diagcomm_io_socket_create_thread(thread_name, diagcomm_io_udp_thread_main, (void*)conn, THREAD_TASK_STACK_SIZE, READ_TASK_PRIORITY);
   return_val = osal_create_diag_timer(&read_task_block_timer, read_task_tcb , DIAG_UDP_TIMEOUT_TIMER_SIG);

   /* Assert since there is no recovery from this error */
   ASSERT(OSAL_SUCCESS == return_val);
}


/*===========================================================================
FUNCTION diagcomm_io_udp_close

DESCRIPTION
  Close the socket connection

PARAMETERS
  conn - Connection object for the channel.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  None
===========================================================================*/
void diagcomm_io_udp_close(diagcomm_io_conn_type * conn)
{
   int i = 0;
   uint8 size_pfds = 0, size_sock_conn = 0;
   
   (void)qclose(conn->sock_handle);
   for(i=0;i<DIAG_MAX_NUM_SOCKETS;i++)
	{
		if (diag_pfds[i].fd==conn->sock_handle)
		{
			/* Reset the entry for which socket is closed */
			diag_pfds[i].fd = 0;
			diag_pfds[i].events = 0;
			diag_pfds[i].revents = 0;
			diag_sock_conn[i] = NULL;
			diag_pfds_index--;
			
			/* Pull up the entries which are next to current entry */
			size_pfds = sizeof(diag_pfds[0])*(diag_pfds_index-i);
			memscpy(&diag_pfds[i], size_pfds, &diag_pfds[i+1], size_pfds);
			size_sock_conn = sizeof(diag_sock_conn[0])*(diag_pfds_index-i);
			memscpy(&diag_sock_conn[i], size_sock_conn, &diag_sock_conn[i+1], size_sock_conn);
			
			/* Reset the last entry as it is pulled up */
			diag_pfds[diag_pfds_index].fd = 0;
			diag_pfds[diag_pfds_index].events = 0;
			diag_pfds[diag_pfds_index].revents = 0;
			diag_sock_conn[diag_pfds_index] = NULL;
			break;
		}
	}

   /* Reset the connection parameters */	  
   conn->io_type = DIAGCOMM_IO_TYPE_INVALID;
   conn->addr.address.addrtype = IPCR_ADDR_NAME;
   conn->addr.address.addr.port_name.service = QSOCKET_DIAG_ID;
   conn->connected = FALSE;
   conn->channel_connected = FALSE;
   conn->data_pending = FALSE;
   conn->diag_prev_tx_pending = FALSE;
   conn->diag_resend_data = FALSE;
   memset(conn->feature_mask,0,DIAG_INT_FEATURE_MASK_LEN);   
   conn->allow_flow = TRUE;   

   if(conn->port_num == DIAGCOMM_PORT_1 && conn->channel_type == DIAGCOMM_IO_CTRL)
   	{
   		conn->addr.address.addr.port_name.instance = DIAG_SOCKET_INST_ID_CTRL;
   	}
   
   else if(conn->port_num == DIAGCOMM_PORT_1 && conn->channel_type == DIAGCOMM_IO_CMD)
   	{
   		conn->addr.address.addr.port_name.instance = DIAG_SOCKET_INST_ID_CMD;
   	}

   else if(conn->port_num == DIAGCOMM_PORT_1 && conn->channel_type == DIAGCOMM_IO_DATA)
   	{
   		conn->addr.address.addr.port_name.instance = DIAG_SOCKET_INST_ID_DATA;
	}
   
   else if(conn->port_num == DIAGCOMM_PORT_2 && conn->channel_type == DIAGCOMM_IO_CMD)
   	{
   		conn->addr.address.addr.port_name.instance = DIAG_SOCKET_INST_ID_DCI_CMD;
   	}

   else if(conn->port_num == DIAGCOMM_PORT_2 && conn->channel_type == DIAGCOMM_IO_DATA)
   	{
   		conn->addr.address.addr.port_name.instance = DIAG_SOCKET_INST_ID_DCI_DATA;
   	}

   else
   	{
   		MSG_2(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"diagcomm_io_udp_close:: Unsupported port_num: %d and channel_type: %d", conn->port_num, conn->channel_type);
   	}
   

}


/*===========================================================================
FUNCTION diagcomm_io_udp_close

DESCRIPTION
  This function should be called to send data through the previously connected socket associated with
  the diagcomm_io_conn_type.  Data will be written on the calling thread's context and can block if all transmit queues
  are full.   The number of bytes written will be returned.  A negative number would indicate an error condition.
  Currently the send is done in DIAG task context.

PARAMETERS
  conn - Connection object for the channel.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  None
===========================================================================*/
int diagcomm_io_udp_send(diagcomm_io_conn_type * conn,void *buf, unsigned int total_pkt_length)
{
  //The send is done in diag task context.
  int write_bytes = 0;

  ASSERT(total_pkt_length);
  if (conn->io_type == DIAGCOMM_IO_TYPE_IPCR_UDP)
  {
    if(conn->isServer)
    {
      write_bytes = qsendto(conn->sock_handle, buf, total_pkt_length, conn->send_flags, (struct qsockaddr *)&conn->addr, sizeof(conn->addr));

    }
    else
    {
       write_bytes = qsend(conn->sock_handle, buf, total_pkt_length, conn->send_flags);
    }
  }
  else
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"diagcomm_io_udp_send:: Unsupported io_type: %d", conn->io_type);
    ASSERT(0);
  }

  return write_bytes;
}

//This is the main function for the UDP use case
//The main loop will perform a blocking read for data.  When data is received, it will determine
//the source of the data, and notify that data has been received
void diagcomm_io_udp_thread_main(void *data)
{
  diagcomm_io_udp_receive_data();
}


void diagcomm_udp_block_timeout(void)
{
   osal_sigs_t return_sigs = 0;

   osal_set_timer(&read_task_block_timer, 10);
   return_sigs = osal_thread_wait (read_task_tcb, DIAG_UDP_TIMEOUT_TIMER_SIG);
}

//This function performs the actual reading from the socket associated with the udp_data_type.  Since the UDP protocal
//sends and receives packetized data, no further processing is necessary.  All data is guaranteed to be a full DIAG packet.
//If further enhancments related to reliablility need to be implemented, this is the area where it should be implemented.
void diagcomm_io_udp_receive_data(void)
{
  int i, rc = 0;
  int num_recv = -1;
  qsocklen_t socklen;
  diagcomm_io_conn_type *conn;

  while(TRUE)
  {
    rc = qpoll(&diag_pfds[0], diag_pfds_index, 100);
    if(rc < 0)
    {
      MSG_1(MSG_SSID_DIAG, MSG_LEGACY_HIGH, "qpoll returned %d\n", rc);
      break;
    }
    else if(rc == 0)
    {
      MSG_LOW("POll timedout!\n",0,0,0);
      continue;
    }
    else
    {
      for(i=0;i<diag_pfds_index;i++)
      {
        if(diag_pfds[i].revents & QPOLLIN)
        {
           conn = diag_sock_conn[i];
		   if(conn == NULL)
		   	continue;
           socklen = sizeof(conn->addr);

           if(conn->used == 0)
           {
              num_recv = qrecvfrom(diag_pfds[i].fd, conn->buf, conn->buf_length, conn->recv_flags, (struct qsockaddr *)&(conn->addr), &socklen);
              if(num_recv > 0)
              {
                 osal_lock_mutex(&conn->buf_mutex);
                 conn->used = num_recv;
                 osal_unlock_mutex(&conn->buf_mutex);
                 diagcomm_io_notify_rx(conn);
                 continue;
              }
           }
           else
           {
              diagcomm_udp_block_timeout();
              continue;
           }

           if(num_recv < 0)
           {
             MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"qrecvfrom returned an error %d",num_recv);
             break;
           }
           else if(0 == num_recv)
           {
              //Signal writers that they can continue writing more.
             diagcomm_io_enable_flow(conn);
           }
        }
      }
    }
  }
}


// This function will read from the socket for the connection object into the conn->buf buffer
int diagcomm_io_udp_read(diagcomm_io_conn_type *conn)
{
   int num_recv = -1;
   qsocklen_t socklen;

   if(NULL == conn->buf) return DIAG_EPARAM;
   socklen = sizeof(conn->addr);
   /* conn->used should be zero since we will not be reading in diag task context */
   /* If used bytes is not zero, something is wrong and processing is not being done before the next read */
   ASSERT(conn->used == 0);

   num_recv = qrecvfrom(conn->sock_handle, conn->buf, conn->buf_length, conn->recv_flags, (struct qsockaddr *)&(conn->addr), &socklen);
   if(num_recv > 0)
   {
      conn->used = num_recv;
   }
   else if(num_recv < 0)
   {
     MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR,"qrecvfrom returned an error %d",num_recv);
   }
   else if(0 == num_recv)
   {
     //Signal writers that they can continue writing more.
     diagcomm_io_enable_flow(conn);
   }
   return num_recv;
}

