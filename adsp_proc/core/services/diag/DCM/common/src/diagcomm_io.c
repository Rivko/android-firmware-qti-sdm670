/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diag I/O Wrapper APIs for Interprocessor Communication and I/O

General Description

Copyright (c) 2011-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                              Edit History

 $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagcomm_io.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
08/29/17   gn      Changes to handle remote socket close
06/01/16   nk      Fixed KW errors
05/12/16   ph      SMD references removal and code clean up.
01/05/16   ph      Flow control handled for buffering mode switch.
12/22/15   is      Fix flow control race condition
12/01/15   ph      Flow control handled properly over control channel.
09/01/15   xy      Removed check for diag_prev_tx_pending in diagcomm_io_enable_flow()
08/21/15   as      Resolved Diag signal conflicts
04/08/15   ps      Diag over sockets
01/21/15   sr      Added changes to remove DSM items
04/10/14   sr      Support for peripheral buffering mode
09/27/13   xy      Removed code under FEATURE_WINCE
08/30/13   ph      Resetting the signal DIAG_BLOCK_SIG before we call sio_flush_tx().
08/29/13   sr      Fixed the missing break line issue in switch statements
02/11/13   sr      Mainling Diag central routing
01/16/13   rh      Changed DIAG_TX_WMQ_SIG to DIAG_SMDL_WRITE_SIG
01/03/13   sg      Command Response Channel separation
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/15/12   is      Support for preset masks
05/11/12   hvm     Support t-put improvements with SMDL
05/11/12   hvm     Added api diagcomm_io_get_tx_wmq_cnt()
03/04/12   sg      set the signal to forward the regsitration tables to the apps
                   proc only when the ctrl channel is opened on both the sides
03/04/12   sg      Set DTR using sio_control_ioctl for control channel
10/07/11   is      Support sending log mask per equipment id
09/19/11   is      Optimized sending F3 "set mask" requests from Master Diag
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
08/09/11   wjg     Modified to initialize new data_pending member
04/28/11   is      Resolve compilation issues
04/27/11   is      Resolve compiler warnings
03/25/11   is      Fix compilation errors for modem-only configuration
03/04/11   is      Support for Diag over SMD-Lite

===========================================================================*/

#include "diagcomm_io.h"
#include "diagcomm_ctrl.h"        /* For NUM_PORT_TYPES */
#include "diag_v.h"               /* For diag_block() */
#include "diagi_v.h"              /* For DIAG_UPDATE_MASK_NONE, etc */
#include "err.h"                  /* For ERR_FATAL() */
#include "assert.h"               /* For ASSERT() */
#include "diagtarget.h"           /* For diag_tcb */


#include "diagcomm_io_socket.h"
#include "time_svc.h"
#include "stringl.h"

extern osal_tcb_t diag_tcb;

extern osal_timer_t diag_sio_tout_timer;
extern osal_mutex_arg_t diag_flow_control_mutex;

/* -------------------------------------------------------------------------
** Definitions and Declarations
** ------------------------------------------------------------------------- */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];
extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];

uint32 diag_send_buf_threshold = DIAG_MAX_SMDL_DATA_BLOCK_SIZE; /*Buffer up data upto this threshold value */
uint32 diag_max_send_buf_len = DIAG_MAX_TX_PKT_SIZ; /*Maximum send size*/
extern volatile diag_tx_mode_type diag_tx_mode[DIAG_MAX_STREAM_ID][DIAG_SEND_BUF_CNT];

/*===========================================================================

FUNCTION DIAGCOMM_IO_OPEN

DESCRIPTION
  Opens a connection between two processors or I/O

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  True - If connection opened successfully
  False - If connection was not opened

===========================================================================*/
boolean diagcomm_io_open( diagcomm_io_conn_type * conn )
{
  int ret;
  conn->connected = FALSE;

  switch( conn->io_type )
  {
    case DIAGCOMM_IO_TYPE_IPCR_UDP:
    {
      ret = diagcomm_io_socket_open(conn);
      ASSERT(DIAG_EOK == ret);
    }

    default:
      break;
  }

  return conn->connected;

} /* diagcomm_io_open */


/*===========================================================================

FUNCTION DIAGCOMM_IO_CLOSE

DESCRIPTION
  Closes a connection between two processors or I/O

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_close( diagcomm_io_conn_type * conn )
{
  /* Success or not, consider connection closed */
  conn->connected = FALSE;

  switch( conn->io_type )
  {
    case DIAGCOMM_IO_TYPE_IPCR_UDP:
    {
      diagcomm_io_socket_close(conn);
	  /* If command channel is closed, deregister all the local registration tables */
	  if(conn->channel_type==DIAGCOMM_IO_CMD)
	  	diagpkt_tbl_dereg(NULL);
    }


    default:
      break;
  }

} /* diagcomm_io_close */

/*===========================================================================

FUNCTION DIAGCOMM_IO_TRANSMIT

DESCRIPTION
  Transmits data between two processors or I/O

PARAMETERS
  conn - Diag I/O connection handle
  tx_param - Tx data containing buffer to transmit

RETURN VALUE
  DIAG_EOK : Transmit successful
  DIAG_EFAILED: Transmit failed over the connection channel
  DIAG_EPARAM: Input paramater validation failed
  DIAG_ERETRY: Flow is disabled, retry later

===========================================================================*/
int diagcomm_io_transmit( diagcomm_io_conn_type * conn, diagcomm_io_tx_params_type * tx_param )
{
  int num_write = 0;
  int ret = DIAG_EOK;

  switch( conn->io_type )
  {
        case DIAGCOMM_IO_TYPE_IPCR_UDP:
          if(FALSE == conn->allow_flow)
          {
             return DIAG_ERETRY;
          }
        if(0 == tx_param->len)
          {
             return DIAG_EPARAM;
          }

          osal_lock_mutex(&diag_flow_control_mutex);
        num_write = diagcomm_io_socket_send(conn, tx_param->buffer, tx_param->len);

          //Check if the previous send succeeded. If not, disable the flow.
        if( num_write == tx_param->len )
           {
               /*If we are sending data enable flow*/
               conn->allow_flow = TRUE;
               conn->diag_prev_tx_pending = FALSE; /* Packet successfully sent */
               diagbuf_clear_index(conn, tx_param);
               ret = DIAG_EOK;
           }
           else if(num_write == QEAGAIN)
           {
              conn->diag_prev_tx_pending = TRUE;  /* FIFO is full; come back later */
              conn->allow_flow = FALSE;
            diagcomm_incr_flow_ctrl_count(DIAGCOMM_PORT_REMOTE_PROC);
              ret = DIAG_EFAILED;
           }
		   else if((num_write == QECONNRESET) || (num_write == QEBADF) || (num_write == QEHOSTUNREACH) || (num_write == QENOTCONN))
		   {	   
		      /* Remote socket for this channel closed abruptly. Close the local socket for the channel to prevent further data transmision */
		      diagcomm_io_close(conn);
           }
           else
           {
              conn->last_error = num_write;
              ERR_FATAL ("Error seen when sending data: %d",num_write,0,0);
           }
           osal_unlock_mutex(&diag_flow_control_mutex);
           break;

    default:
      break;
  }

  return ret;
} /* diagcomm_io_transmit */

/*===========================================================================

FUNCTION DIAGCOMM_IO_ENABLE_FLOW

DESCRIPTION
  Enables flow control for channel

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_enable_flow( diagcomm_io_conn_type * conn )
{

  switch( conn->io_type )
  {
    case DIAGCOMM_IO_TYPE_IPCR_UDP:
    {
      osal_lock_mutex(&diag_flow_control_mutex);
      conn->diag_resend_data = TRUE;

      if(conn->diag_prev_tx_pending)
      {
          if((DIAGCOMM_IO_DATA == conn->channel_type) && (DIAGCOMM_PORT_REMOTE_PROC == conn->port_type))
          {
            diag_set_internal_sigs(DIAG_INT_RESEND_DATA_SIG);
          }
          else if ((DIAGCOMM_IO_CMD == conn->channel_type) && (DIAGCOMM_PORT_REMOTE_PROC == conn->port_type))
          {
          /*We should not get flow controlled on command channel - how do we handle this case?*/
            diag_set_internal_sigs(DIAG_INT_RESEND_CMD_SIG);
          }
          else if ((DIAGCOMM_IO_CTRL == conn->channel_type) && (DIAGCOMM_PORT_REMOTE_PROC == conn->port_type))
          {
            diag_set_internal_sigs(DIAG_INT_RESEND_CTRL_SIG);
          }
      }
      osal_unlock_mutex(&diag_flow_control_mutex);
      break;
    }

    default:
      break;
  }
} /* diagcomm_io_enable_flow */

/*===========================================================================

FUNCTION DIAGCOMM_IO_STATUS

DESCRIPTION
  This function returns TRUE if the communications layer has an open port.

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  True - If the communications layer has an open port
  False - Otherwise

===========================================================================*/
boolean diagcomm_io_status( diagcomm_io_conn_type * conn )
{
  ASSERT( conn != NULL );

  if( (conn->connected == FALSE) || (conn->channel_connected == FALSE) )
    return FALSE;

  switch( conn->io_type )
  {

    case DIAGCOMM_IO_TYPE_IPCR_UDP:
    {
       return (conn->connected && conn->channel_connected);
    }

    default:
      break;
  }

  return FALSE;

} /* diagcomm_io_status */

/*===========================================================================

FUNCTION DIAGCOMM_IO_CONN_CHANGED

DESCRIPTION
  This function is a wrapper for handling connection status changes.

PARAMETERS
  conn - Diag I/O connection handle
  connected - True if connection is opened.  False if connection is closed.


RETURN VALUE

===========================================================================*/
void diagcomm_io_conn_changed( diagcomm_io_conn_type * conn , boolean connected )
{
  //ToDo: Make this more generic
  if( (DIAGCOMM_PORT_REMOTE_PROC == conn->port_type) && (DIAGCOMM_IO_DATA == conn->channel_type) )
  {
    diagcomm_sio_conn_changed( conn, connected );
  }
  else if( (DIAGCOMM_PORT_REMOTE_PROC == conn->port_type) && (DIAGCOMM_IO_CTRL == conn->channel_type) )
  {
    diagcomm_sio_ctrl_conn_changed( conn, connected );
  }

} /* diagcomm_io_conn_changed */

/*===========================================================================

FUNCTION DIAGCOMM_IO_HAS_TX_DATA

DESCRIPTION
  This function checks whether the port has pending data to transmit.

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  TRUE - If there is data available in the given channel
  FALSE - otherwise

===========================================================================*/
boolean diagcomm_io_has_tx_data( diagcomm_io_conn_type * conn )
{
 if( conn == NULL )
   return FALSE;

  if( (conn->connected == FALSE) || (conn->channel_connected == FALSE) )
    return FALSE;

  switch( conn->io_type )
  {
    default:
      return FALSE;
  }

} /* diagcomm_io_has_tx_data */

/*===========================================================================

FUNCTION DIAGCOMM_IO_NOTIFY_RX

DESCRIPTION
  This function is used to notify diag task of data being available on a channel.
  Depending on the connection type, the corresponding notify function is called.

PARAMETERS
  conn - Diag I/O connection handle

RETURN VALUE
  None

===========================================================================*/
void diagcomm_io_notify_rx(diagcomm_io_conn_type *conn)
{
  osal_sigs_t return_sigs;
  int return_val = 0;

   if( (DIAGCOMM_IO_CMD == conn->channel_type) && (DIAGCOMM_PORT_REMOTE_PROC == conn->port_type) )
   {
     if( conn->port_num == DIAGCOMM_PORT_1 )
     {
       diag_rx_notify();
     }
   }
   else if( DIAGCOMM_IO_CTRL == conn->channel_type )
   {
     /* Set signal to diag task to process ctrl traffic from slave */
      return_val = osal_set_sigs(&diag_tcb, DIAG_CTRL_MSG_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
   }
   else if( (DIAGCOMM_IO_DATA == conn->channel_type) && (DIAGCOMM_PORT_REMOTE_PROC == conn->port_type) )
   {
     diag_rx_notify();
   }
}

void diagcomm_io_change_connection(diagcomm_io_type io_type_to)
{
   int return_val;
   osal_sigs_t return_sigs;
   uint8 buf_index = 0;

   if(diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].io_type != io_type_to)
   {
      diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].change_to_type = io_type_to;
      diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].change_to_type = io_type_to;
      diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].change_to_type = io_type_to;
      return_val = osal_set_sigs(&diag_tcb, DIAG_CHANGE_CONN_SIG, &return_sigs);
      ASSERT(OSAL_SUCCESS == return_val);
   }

   if (io_type_to == DIAGCOMM_IO_TYPE_IPCR_UDP)
   {
      diag_send_buf_threshold = DIAG_MAX_SOCKETS_DATA_BLOCK_SIZE;
      diag_max_send_buf_len = DIAG_MAX_SOCKETS_DATA_BLOCK_SIZE;
      for (buf_index = 0; buf_index < DIAG_SEND_BUF_CNT; buf_index++)
      {
        diag_tx_mode[DIAG_STREAM_1 - 1][buf_index].curr_threshold = diag_max_send_buf_len;
      }
   }
}
