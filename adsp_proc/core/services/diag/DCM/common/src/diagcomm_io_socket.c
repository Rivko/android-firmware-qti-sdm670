/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

    Diag wrapper APIs for I/O over socket interface 

General Description

Copyright (c) 2015-2016 by Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagcomm_io_socket.c#1 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
05/03/16    ph     Changed UDP_WRITE_TIMEOUT to infinite.
05/12/16    ph     SMD references removal and code clean up.
04/08/15    ps     Created file for Diag over sockets. 
===========================================================================*/


/* Include files */
#include "customer.h"
#include "diag_cfg.h"
#include "diagcomm_io_socket.h"
#include "diagcomm_io.h"
#include "osal.h"
#include "diag.h"
#include "diagpkt.h"
#include "stdio.h"
#include <stdlib.h>
#include "diagi_v.h"
#include "assert.h"
#include "diagcomm_io_udp.h"


/* Static and global variables */
int diag_socket_error;
static boolean isSocketsThreadInitialized;


/* Forward declarations */


/*===========================================================================
FUNCTION diagcomm_io_socket_check_support

DESCRIPTION
  This function checks if sockets is supported on the APPS processor.
  
PARAMETERS
  conn - Connection object for CTRL channel

DEPENDENCIES
  None

RETURN VALUE
  TRUE - Sockets supported on APPS
  FALSE - Sockets not supported on APPS
===========================================================================*/
boolean diagcomm_io_socket_check_support(diagcomm_io_conn_type *conn)
{
   struct qsockaddr_ipcr srv_addr;

   //5 milliseconds time out assuming the apps starts first and has already called qbind for the ctrl socket
   if(0 == diagcomm_io_udp_get_srv_addr(conn->addr.address.addr.port_name, &srv_addr, UDP_WRITE_TIMEOUT)) 
   {
     //Sockets supported
     return TRUE;
   }
   return FALSE;
}

/*===========================================================================
FUNCTION diagcomm_io_socket_open

DESCRIPTION
  Open the socket connection for the connection object.
  Additionally start the read thread if not already initialized.

PARAMETERS
  conn - Connection object for the channel.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  DIAG_EUNREACH - The destination port is unreachable
  DIAG_EPARAM - Arguments are invalid
  DIAG_EOK - Operation completed successfully.
===========================================================================*/
int diagcomm_io_socket_open(diagcomm_io_conn_type * conn)
{
   int ret;
   
   if(NULL == conn) 
   {
     return DIAG_EPARAM;
   }

   ret = diagcomm_io_udp_open(conn);
   if(DIAG_EOK != ret)
   {
      return ret;
   }

   /* Check if read thread already initialized */
   if(!isSocketsThreadInitialized)
   {
     /* We start the read thread after we open at least one connection in order to make sure we do not
             create a thread that is not used at all in case there is no socket support on the remote side */
      ret = diagcomm_io_socket_init(conn);
      return ret;
   }
   return ret;
}

/*===========================================================================
FUNCTION diagcomm_io_socket_close

DESCRIPTION
  Close the socket connection for the connection object.

PARAMETERS
  conn - Connection object for the channel.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
   None

===========================================================================*/
void diagcomm_io_socket_close(diagcomm_io_conn_type *conn)
{
   diagcomm_io_udp_close(conn);
}

/*===========================================================================
FUNCTION diagcomm_io_socket_create_thread

DESCRIPTION
  Create a read thread for the socket. 

PARAMETERS
  name - name of the thread.
  main_func - thread start function
  arg - any Argument to be passed to the start function
  stacksize - Size of stack to be allocated for the thread
  priority - priority of thread. 

DEPENDENCIES
  None

RETURN VALUE
   None

===========================================================================*/
void* diagcomm_io_socket_create_thread
(
   const char *name,
   void *main_func,
   void *arg,
   int stacksize,
   int prio
   )
{
  int task_pri_array[32] = { -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, 0, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1 };
  osal_thread_attr_t task_attr;
  osal_tcb_t *task_tcb;
  void *task_stack;

  task_stack = malloc(stacksize);
  if (task_stack == NULL)
  {
    ERR_FATAL("Create task failed! Out of memory! stacksize requested: %d", stacksize, 0, 0);
    return (void *)NULL;
  }

  task_tcb = (osal_tcb_t *)malloc(sizeof(osal_tcb_t));
  if (task_tcb == NULL)
  {
    ERR_FATAL("Create task failed! Out of memory! task_tcb size requested: %d", sizeof(osal_tcb_t), 0, 0);
    return (void *)NULL;
  }

  /* Start the task */
  /* stack is of type unsigned long long' */
  task_attr.stack_size = stacksize;
  task_attr.start_func = (void (*)(uint32))main_func;
  task_attr.arg = arg;
  task_attr.name = (void *)name;
  task_attr.priority = prio;
  task_attr.stack_address = task_stack;
  task_attr.p_tskname = (char *)name;
  task_attr.suspended = FALSE;
  task_attr.dog_report_val = 0;

  if ((osal_create_thread(task_tcb, &task_attr, task_pri_array)) != 0)
  {
    /* Error fatal here since DIAG will not work if the thread cannot be created */
    ERR_FATAL("Create task failed!", 0, 0, 0);
  }
  return (void *)task_tcb;
}

/*===========================================================================
FUNCTION diagcomm_io_socket_send

DESCRIPTION
  Close the socket connection for the connection object.

PARAMETERS
  conn - Connection object for the channel.
  buf - Buffer to be sent
  length -length of data to be sent.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
   DIAG_EPARAM - Arguments are invalid
   DIAG_EOK - Operation completed successfully.


===========================================================================*/
int diagcomm_io_socket_send(diagcomm_io_conn_type *conn, void *buf, uint32 length )
{
  int writev_ret = 0;

  /* Sanity check for conn */
  ASSERT(conn != NULL && buf != NULL);

  writev_ret = diagcomm_io_udp_send(conn, buf, length);

  if (writev_ret != length)
  {
    diag_socket_error = writev_ret; //Store the last error
  }

  return writev_ret;
}


/*===========================================================================
FUNCTION diagcomm_io_socket_init

DESCRIPTION
  Open the socket connection for the connection object.
  Additionally start the read thread if not already initialized.

PARAMETERS
  conn - Connection object for the channel.

DEPENDENCIES
  The connection object should contain the parameters required to open the socket

RETURN VALUE
  DIAG_EUNREACH - The destination port is unreachable
  DIAG_EINVALID - Arguments are invalid

===========================================================================*/

int diagcomm_io_socket_init(diagcomm_io_conn_type *ctrl_conn)
{
   if(NULL == ctrl_conn) 
   {
     return DIAG_EINVALID;
   }

   if(DIAGCOMM_IO_CTRL != ctrl_conn->channel_type) 
   {
     return DIAG_EPARAM;
   }

   if(NULL == ctrl_conn->buf) 
   {
     ASSERT(NULL != ctrl_conn->buf);
     return DIAG_EINVALID;
   }

   diagcomm_io_udp_open_init(ctrl_conn);
   isSocketsThreadInitialized = TRUE;
   return DIAG_EOK;
}
