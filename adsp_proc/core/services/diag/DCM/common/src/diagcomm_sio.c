/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Diagnostic Communications via SIO

General Description
  The routines in this file handle the communications layer for the
  diagnostic subsystem when communicating via SIO.

Copyright (c) 2001-2017 by QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                               Edit History

 $PVCSPath:  L:/src/asw/MSM5200/common/vcs/diagcomm_sio.c_v   1.10   08 May 2002 14:37:42   ldefauw  $
 $Header: //components/rel/core.qdsp6/2.1/services/diag/DCM/common/src/diagcomm_sio.c#4 $ $DateTime: 2017/12/14 03:17:10 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/06/17   sa      Do not send the registration table if diagID is not set.
02/21/17   ph      Tag DIAG ID header to all the diag packets sent out.
08/29/17   gn      Changes to handle remote socket close
06/21/16   rs      channel_connected is checked for it to be TRUE in diagcomm_status()
05/12/16   ph      SMD references removal and code clean up.
02/01/15   is      Make allowflow not static to allow debugging
10/14/15   nk      Removed un-used diag forwarding task variables from slave peripherals
04/08/15   ps      Diag over sockets
01/21/15   sr      Added changes to remove DSM items
02/27/15   sa      Removed buffering into the SIO TX queue in streaming mode.
10/30/14   xy      Fixed issue that modem not able to send mask centralization
                   control packets to apps
04/10/14   sr      Support for peripheral buffering mode
04/01/14   rs      Added support for build mask centralization.
03/05/14   xy      Fixed KW ASSERTs issues
11/27/13   ph      Updated MSG 1.0 API references to MSG.2.0.
09/27/13   xy      Removed code under FEATURE_WINCE
09/25/13   xy      Handle sending out feature mask on control channel first before
                   sending out cmd reg tables in diag_handle_internal_sigs()
09/13/13   ph      Updated drain logic for TX packets of size greater than 4k
                   to send the current dsm item and then start the chain.
02/11/13   sr      Mainling Diag central routing
12/13/12   rs      Added timeout mechanism to unblock diag in diagbuf_flush.
11/15/12   is      Support for preset masks
07/11/12   hvm     Moved defines for TX/RX Max Chain Size to diagi_v.h
03/04/12   sg      set the signal to forward the regsitration tables to the apps
                   proc only when the ctrl channel is opened on both the sides
10/07/11   is      Support sending log mask per equipment id
09/12/11   is      Master Diag forwards "set mask" requests via CTRL msgs
04/28/11   mad     Fixed featurization for diag_fwd_task access
04/18/11   is      Ensure CTRL msg sizes are constant
03/29/11   sg      Central Routing changes for Q6
03/25/11   is      Fix compilation errors for modem-only configuration
03/04/11   is      Support for Diag over SMD-Lite
02/28/11   sg      Power saving changes
12/20/10   mad     Changes for dual mask
12/16/10   vs      Open SDIO port based on feature
10/20/10   is      Bypass RDM to set default to UART1 for 8960 Apps only bringup
10/14/10   is      Featurize diagcomm_send_dtr_msg() for multi-proc only
10/05/10   mad     Register for DTR only if port open is successful
09/28/10   mad     Included diaglogi.h, eventi.h to fix build error
09/14/10   is      New control channel and central routing features
08/10/10   sg      Close SMD Port when the SIO port on apps is closed
09/13/10   is      Add flow control count and DSM chaining count to Diag Health
09/01/10   sg      Call sio_transmit() with smd stream id on Q6 to put the
                   tx traffic into diagcomm_smd_tx_wmq.
08/06/10   sg      Featurized diagcomm_sio_close_done_cb(),diagcomm_sio_close_cb
                   to fix compiler warnings
07/23/10   mad     Turning on command-filtering before sio_open() call, for
                   on-device diag port
07/21/10   sg      Added chaining support for rx response packets
07/15/10   sg      Cleaning up the setting and resetting of drain timer in
                   diag_dtr_changed_cb()
06/21/10   vs      Added ASSERT to catch dropping of partially filled DSM item.
05/10/10   sg      mainlined setting and clearing of drain timer
06/07/10   is      Removed support for FEATURE_DIAG_MULTI_PORTS, FEATURE_SIO_USB_ETHR
                   FEATURE_SIO_USB_COMPOSITE
05/25/10   JV      New task on master to process slave traffic
05/06/10   as      De-coupled Diag and ondevice modules. Mainlined the code for
                   retry count when the open port fails. Fixed the assign port
                   logic when Diag port is configured to ondevice.
05/03/10   sg      fixing corner case where we can expect triple chaining
                   with 6k logs
04/29/10   is      Removed support for DEBUG_TX_EFS
04/20/10   sg      Set Event Drian Signal After Flow control is turned off
03/25/10   mad     Support for on-device logging for WM7 devices
03/15/10   vs      Fixed unchecked pointer access in diagcomm_outbound_alloc
03/03/10   as      Added support RDM assign port needed for Ondevice retry.
01/04/10   vs      Modifications to support diagbuf_send_pkt_single_proc
12/30/09   sg      Fixed compiler warning
12/28/09   ps      Fixed Klocwork errors & compiler warnings.
12/10/09   sg      Added flow control mechanism for DSM_DIAG_SIO_TX_ITEM_POOL
                   at item level granularity
11/30/09   ps      Fixed compiler warnings.
11/05/09   vs      Mainlined functions diagbuf_outbound_alloc and diagbuf_send
10/29/09   JV      Pass the DSM pool ID to SIO-USB
09/11/09   sg      Made changes to have dedicated pools in each Tx and RX direction
                   for single processor targets
08/11/09   mad     Signal Diag with DIAG_TX_SLAVE_SIG, when DTR changes
08/06/09   sg      Added support for DSM chaining in diagcomm_sio_inbound_pkt_notify(),
                   diagcomm_inbound()
08/05/09   JV      Removed the CUST_H featurization around the inclusion of
                   customer.h.
08/04/09   JV      Diag does not vote for/against sleep based on dtr change events.
07/31/09   JV      Removed inclusion of task.h. Removed warnings.
07/31/09   JV      Merged Q6 diag code back to mainline
06/23/09   JV      Introduced the OS abstraction layer
05/07/09   vk      changed includes from external to internal headers
04/21/09   ps      branch collapse of 6k and 7k DIAG
11/06/08   mad     modified diagcomm_status() to return only the port
                   open/close status. diagcomm_status() is not affected by usb
                   cable detach anymore.
10/03/08   vg      Updated code to use PACK() vs. PACKED
1/28/08    as      Fixed tail char setup for single and modem only builds.
12/16/07   pc      Added support for diag_dtr_enable and diag_dtr_changed_cb.
05/17/07   jc      Proxy tv.  FEATURE_SIO_USB_COMPOSITE
12/15/06   as      Fixed compiler warnings.
10/31/05   as      Fixed lint errors.
09/09/05   sl      Added support for multiple tx/rx queues for the case of
                   FEATURE_DIAG_MULTI_PORTS
05/15/05   as      Added support for FEATURE_DATA_SERIALIZER
03/28/05   sl      Added an array of watermark queues for the multi port feature
03/24/05   as      Added check in UART/USB notify functions to check if the
                   connected accessory detected by AAM is UART and USB
03/05/05   sl      Corrected BT Featurization for FEATURE_DIAG_MULTI_PORTS
02/20/05   sl      Added support for DIAG over BT via RDM under
                   FEATURE_DIAG_MULTI_PORTS
02/12/05   sl      Fixed a bug that caused the close/open port bit masks
                   not being cleared
02/09/05   abu     Added support for Diag over IrDA under FEATURE_IRDA flag
01/25/04   sl      Added support for callback notifications from Accessory
                   Attachment Manager (AAM) for USB and UART cable attach and
                   detach under FEATURE_ACCESSORY_ATTACHMENT_MANAGER.
12/14/04   sl      Added support for DIAG to hold multiple SIO ports
                   controlled by FEATURE_DIAG_MULTI_PORTS
04/20/04   pn      Added support for DIAG over Bluetooth.
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
09/29/03   dna     Get T_QUASAR to compile and work again for MSM6500
08/14/03   vsk     DSM 2.0 changes
04/10/03    wx     Move qcdmg and to_data_mode functions from RDM to here.
05/08/02   lad     Moved dsm_simple_enqueue_isr() call above diagcomm_sio_cb
                   to enqueue data prior to potential context switch in cb.
02/27/02   dwp     Additional changes to use new DSM interface functions for
                   registering/handling memory events.
02/19/02   lad     Made changes to use new DSM interface functions for
                   registering/handling memory events.
01/21/02   as      Changed nv_sio_baud_type to nv_sio_baudrate_type
11/06/01   abp     Added diagcomm_flush_tx() to flush SIO transmit channel
10/30/01   abp     Added diagcomm_sio_close_to_open_cb().
                   In diagcomm_sio_close_done_cb() callback function ptr is
                   checked for NULL.
                   In diagcomm_open() added mechanism to close connection
                   before opening a new connection
10/09/01   ttl     Added second parameter to diagcomm_sio_tx_start_cb() and
                   diagcomm_sio_tx_stop_cb() due the DSM memory pool registration
                   changes.
09/17/01   jal     Fixed diagcomm_close() to notify RDM of completion
                   properly
05/07/01   lad     Cleaned up file.
04/17/01   dwp     Initialize diagcomm_sio_open_params.port_id to NULL. RDM
                   will instruct DIAG as to what port to open.
04/06/01   lad     Introduced types for callback function pointers.
                   Cleaned up FEATURE_RUNTIME_DEVMAP support.
                   Updated watermark settings (this may be changed to save RAM).
02/23/01   lad     Created file.

===========================================================================*/

#include "comdef.h"

#include "customer.h"
#include "target.h" /* This is prob. not needed even on AMSS */


#include "diag_v.h"
#include "diagbuf_v.h"
#include "diagcmd.h"
#include "diagcomm_v.h"
#include "diagi_v.h"
#include "diagpkt.h"
#include "msg.h"
#include "msgi.h" /* For msg_set_all_rt_masks_adv () */
#include "diagtune.h"
#include "diagdiag_v.h"
#include "diag_cfg.h"
#include "diag_mode_v.h"
#include "diagcomm_ctrl.h"
#include "diagcomm_io.h" /* For diagcomm_port_type */
#include "diaglogi.h" /* for log_config_mask */
#include "eventi.h" /* for event_set_all_rt_masks */

#if defined (FEATURE_DIAG_NV)
#include "diagnv_v.h"
#include "nv.h"
#endif

#if defined (DIAG_QDSP6_APPS_PROC)
#include "diagstub.h"
#else
#include "assert.h"
#endif

/*lint -save -e751 */
/* Supress local typedef 'diag_config_comm_enum_type' not referenced */

/* -------------------------------------------------------------------------
** Definitions and Declarataions
** ------------------------------------------------------------------------- */
extern diagcomm_io_conn_type diagcomm_io_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; /* From diagcomm_io.c */
extern diagcomm_io_conn_type diagcomm_io_ctrl_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS]; /* From diagcomm_io.c */


extern osal_tcb_t diag_tcb;
#ifdef FEATURE_DIAG_SWITCH
  extern int diag_on_single_proc;
#endif

/* Diag Health counter - Keeps track of # of times DSM flow ctrl is triggered. */
static uint32 diagcomm_flow_ctrl_count[NUM_PORT_TYPES] = {0};

boolean diagcomm_apps_only_config = FALSE;


/* Configuration information for the diagnostic communications layer.
*/
static struct
{
  void (*inbound_cb) (void);

  /* !!! Note: if using a system with separate threads
     ** for RX and TX, these should be NULL.
   */
  void (*tx_start_cb) (void);
  void (*tx_stop_cb) (void);
}
diagcomm_params;

extern diagcomm_io_conn_type diagcomm_io_cmd_conn[NUM_PORT_TYPES][NUM_REMOTE_PORTS];

/* -------------------------------------------------------------------------
** Externalized routines (declared in diagcomm.h).
** ------------------------------------------------------------------------- */
/*===========================================================================

FUNCTION DIAGCOMM_REGISTER_INBOUND

DESCRIPTION
  This function configures the diagnostic subsystem's communcations layer.

===========================================================================*/
void
diagcomm_register_inbound (void (*inbound_pkt_cb) (void))
{
  diagcomm_params.inbound_cb = inbound_pkt_cb;
}               /* diagcomm_register_inbound */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_OPEN

DESCRIPTION
  Opens the communcications stream.  Returns boolean indicating success.

===========================================================================*/
boolean
diagcomm_sio_open (void)
{
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1];
  diagpkt_filtered_cmd_set = FALSE;

  if((conn->connected) || (conn->channel_connected))
  {
    /*The channel is already connected*/
    return TRUE;
  }

  if (conn->connected)
  {
   /* success or not, consider close operation successful */
    conn->connected = FALSE;
  }

  if( !conn->connected )
  {
    if( diagcomm_io_open(conn) == TRUE )
    {
      conn->connected = TRUE;
    }
    else
    {
      diagpkt_filtered_cmd_set = FALSE;
      MSG (MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_sio_open - Couldn't get stream.");
    }
  }

  return conn->connected;

} /* diagcomm_sio_open */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_CONN_CHANGED

DESCRIPTION

PARAMETERS
  conn - Diag I/O connection handle
  connected - True: SIO data channel has just changed to connected
              False: SIO data channel has just changed to dis-connected

RETURN VALUE
  None

===========================================================================*/
void diagcomm_sio_conn_changed( diagcomm_io_conn_type * conn, boolean connected )
{
  osal_sigs_t return_sigs;
  int return_val = 0;

  if( conn == NULL )
  {
     diag_debug_log(DIAG_DEBUG_LOG_2,0);
     return;
  }

  if (!connected)
  {
    conn->channel_connected = FALSE;

    return_val = osal_set_sigs(&diag_tcb, DIAG_COMM_SIO_TX_FLUSH_SIG , &return_sigs);
    ASSERT(OSAL_SUCCESS == return_val);
  }
  else
  {
    conn->channel_connected = TRUE;
  }
} /* diagcomm_sio_conn_changed */


/*===========================================================================

FUNCTION DIAGCOMM_SIO_CTRL_CONN_CHANGED

DESCRIPTION
  Set the status of channel connected to TRUE when the ctrl port is opened on the
  remote side and set signal to forward the registration tables to apps.

===========================================================================*/
void diagcomm_sio_ctrl_conn_changed( diagcomm_io_conn_type * conn, boolean connected )
{
  if( conn == NULL )
  {
     diag_debug_log(DIAG_DEBUG_LOG_3,0);
     return;
  }

  if (!connected)
  {
    conn->channel_connected = FALSE;
  }
  else
  {
    conn->channel_connected = TRUE;

    /* Check if connection to Master/Remote proc is established first */
    if( diagcomm_ctrl_status(DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1) && diagcomm_ctrl_channel_status(DIAGCOMM_PORT_REMOTE_PROC, DIAGCOMM_PORT_1) )
    {
              /*set feature mask signal first*/
       /* Set signal to send Diag feature flag mask to remote procs */
      (void)diag_set_internal_sigs( DIAG_INT_FEAT_UPDATE_SIG );

    }
  }
} /* diagcomm_sio_ctrl_conn_changed */

/*===========================================================================

FUNCTION DIAGCOMM_CLOSE

DESCRIPTION
  Closes the current connection, if any.

===========================================================================*/
void
diagcomm_close ( void )
{
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1];

  if (conn->connected)
  {
  conn->connected = FALSE;
  diagcomm_io_close( conn );

  /* success or not, consider close operation successful */
  conn->connected = FALSE;
  }
} /* diagcomm_close */

/*===========================================================================

 FUNCTION diagcomm_sio_tx_flush

 DESCRIPTION
   This function flushes outbound water mark queue and turns off all the masks
   to not generate any more traffic

===========================================================================*/
void diagcomm_sio_tx_flush( void )
{
 /*Turn off all message,log,event masks  */
  msg_set_all_rt_masks_adv (0, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  log_config_mask(-1, FALSE, DIAG_STREAM_ALL, DIAG_PRESET_MASK_ALL);
  event_set_all_rt_masks(0, 0);

  /* Clear the diag connected status */
  DIAG_CLR_MASK (diag_cx_state, DIAG_CX_FLUSH_S | DIAG_CX_COMM_S);

} /* diagcomm_sio_tx_flush */


/*===========================================================================

 FUNCTION DIAGCOMM_FLUSH_RX

 DESCRIPTION
   Flushes inbound queue.  This is used when a security timeout occurs,
   though the capability is generic.

===========================================================================*/
void
diagcomm_flush_rx (void (*flush_cb) (void))
{

  if (flush_cb)
  {
    flush_cb ();
  }
} /* diagcomm_flush_rx */

/*===========================================================================

 FUNCTION DIAGCOMM_FLUSH_TX

 DESCRIPTION
   Flushes the transmit channel on current connection.

===========================================================================*/
void
diagcomm_flush_tx (void (*flush_cb) (void), unsigned int timeoutVal)
{
  if (flush_cb)
  {
    flush_cb ();
  }

} /* diagcomm_flush_tx */

/*===========================================================================

FUNCTION DIAGCOMM_IS_CHANNEL_CONNECTED

DESCRIPTION
    This function returns TRUE if the port passed in is open.

===========================================================================*/
boolean
diagcomm_is_channel_connected(diagcomm_enum_port_type port)
{
  return diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][port].connected;
}

/*===========================================================================

FUNCTION DIAGCOMM_STATUS

DESCRIPTION
  This function returns TRUE if the communications layer has an open port.

===========================================================================*/
boolean
diagcomm_status (void)
{
  return (diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].connected \
  && diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].channel_connected);
}

/*===========================================================================

FUNCTION DIAGCOMM_INBOUND

DESCRIPTION
  This function returns a pointer to some inbound data.  If no inbound
  data is available, NULL is returned.

===========================================================================*/
void *
diagcomm_inbound (unsigned int *stream_len, diagcomm_enum_port_type port_num)
{

  ASSERT (stream_len);

  ASSERT (port_num == DIAGCOMM_PORT_1 || port_num == DIAGCOMM_PORT_2);

  *stream_len = diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][port_num].used;
   return (void *)&(diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][port_num].buf);

} /* diagcomm_inbound */
/*===========================================================================

FUNCTION DIAGCOMM_INBOUND_COMPLETE

DESCRIPTION
  This function tells the diag communications layer that the caller is done
  with the buffer returned by diagcomm_send() may now be released.

===========================================================================*/
void
diagcomm_inbound_complete (void *ptr)
{
  ASSERT (ptr != NULL);

  diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].used = 0;

} /* diagcomm_inbound_complete */

/*===========================================================================

FUNCTION DIAGCOMM_CONNECT_STATE

DESCRIPTION

===========================================================================*/
boolean diagcomm_connect_state()
{
  return diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1].channel_connected;
}

/*===========================================================================

FUNCTION DIAGCOMM_SIO_INIT

DESCRIPTION
  This function initializes the diagnostic subsystem's communications layer.

===========================================================================*/
void
diagcomm_sio_init (diagcomm_enum_port_type port_num)
{
  osal_sigs_t return_sigs;
  ASSERT(port_num == DIAGCOMM_PORT_1 || port_num == DIAGCOMM_PORT_2);

  if(DIAGCOMM_PORT_1 == port_num)
  {
  diagpkt_filtered_cmd_set = FALSE;

  /* Init callbacks, etc */
  diagcomm_params.inbound_cb = NULL;
  diagcomm_params.tx_start_cb = NULL;
  diagcomm_params.tx_stop_cb = NULL;

  /* Make sure we are closed */
  diagcomm_close();

  (void)osal_set_sigs(&diag_tcb, DIAG_COMM_OPEN_SIG, &return_sigs);

  }

} /* diagcomm_sio_init */

/*===========================================================================

FUNCTION DIAGCOMM_SEND

DESCRIPTION
  Sends the given data stream if the connection is open.

PARAMETERS
  ptr - Pointer to DSM packet to send
  length - Length of data in DSM packet
  is_cmd_rsp_type - True is DSM contains response packet (has higher priority).
                    Else False.
  port_num - Port number

RETURN VALUE
  None

===========================================================================*/
void
diagcomm_send (void * ptr, unsigned int length,
              boolean is_cmd_rsp_type,
              diagcomm_enum_port_type port_num,
              diag_tx_mode_type *tx_mode_ptr )
{
  diagcomm_io_tx_params_type tx_params;
  uint8 stream_index;
  uint8 stream_id;
  diagcomm_io_conn_type * conn = &diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][port_num];
  diagcomm_io_conn_type * cmd_conn = &diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][port_num];
  diagcomm_io_conn_type * ctrl_conn = &diagcomm_io_ctrl_conn[DIAGCOMM_PORT_REMOTE_PROC][DIAGCOMM_PORT_1];

  ASSERT (ptr);

  if( port_num > DIAGCOMM_PORT_NUM_LAST )
  {
    MSG_1(MSG_SSID_DIAG, MSG_LEGACY_ERROR, "diagcomm_send: Invalid port_num (%d)", port_num);
    return;
  }

  stream_index = DIAG_PORT_TO_INDEX(port_num);
  stream_id = DIAG_INDEX_TO_ID(stream_index);

  //  Are we trying to send something while we are flow controlled?
  // diag_resend_data is set to 1 when we trying to resend the buffer to SMDL signals us--
  // In this case we can send data while we are flow controlled

  if(is_cmd_rsp_type)
  {
    if ((diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][port_num].diag_prev_tx_pending == 1) &&
    (diagcomm_io_cmd_conn[DIAGCOMM_PORT_REMOTE_PROC][port_num].diag_resend_data != 1))
    {
      ASSERT(0);
    }
  }
  else
  {
    if ((diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][port_num].diag_prev_tx_pending == 1) &&
    (diagcomm_io_conn[DIAGCOMM_PORT_REMOTE_PROC][port_num].diag_resend_data != 1))
    {
       ASSERT(0);
    }
  }

    /* Just send immediately if there's no pkt to write */
    tx_params.buffer = ptr;
    tx_params.len = length;
    tx_params.tx_ptr = tx_mode_ptr;

    if (is_cmd_rsp_type)
    {
        if( DIAG_IS_INT_FEATURE_BIT_SET(ctrl_conn->feature_mask, F_DIAG_REQ_RSP_CHANNEL) )
            diagcomm_io_transmit( cmd_conn, &tx_params );
        else
            diagcomm_io_transmit( conn, &tx_params );
    }
    else
    {
    ASSERT(tx_mode_ptr != NULL);
    if(tx_mode_ptr->mode == DIAG_TX_MODE_STREAMING)
        {
            diagcomm_io_transmit( conn, &tx_params );
        }
        else
        {
            /* If data is buffered, check if ready to drain. */
            if( diag_drain_buffer_check(stream_id, tx_mode_ptr) )
            {
                diagcomm_io_transmit( conn, &tx_params );

                /*Check if there is more data to be drained*/
                if (diag_drain_buffer_check(stream_id, tx_mode_ptr))
                {
                    if( stream_id == DIAG_STREAM_1 )
                    {
                        diag_set_internal_sigs( DIAG_INT_DRAIN_BUFFERING_BUFFER_SIG );
                    }
                }
            }
        }
    }
}
 /* diagcomm_send */

/*===========================================================================
FUNCTION DIAGCOMM_RESET_FLOW_CTRL_COUNT

DESCRIPTION
    This function resets the internal variable diagcomm_flow_ctrl_count, which keeps
    track of the # of times flow control is triggered.

===========================================================================*/
void diagcomm_reset_flow_ctrl_count( void )
{
  diagcomm_flow_ctrl_count[DIAGCOMM_PORT_REMOTE_PROC] = 0;

}  /* diagcomm_reset_flow_ctrl_count */


/*===========================================================================
FUNCTION DIAGCOMM_GET_FLOW_CTRL_COUNT

DESCRIPTION
    This function returns the internal variable diagcomm_flow_ctrl_count, which keeps
    track of the # of times flow control is triggered.

===========================================================================*/
uint32 diagcomm_get_flow_ctrl_count( diagcomm_port_type port_type )
{
  return diagcomm_flow_ctrl_count[port_type];

} /* diagcomm_get_flow_ctrl_count */


/*===========================================================================
FUNCTION DIAGCOMM_INCR_FLOW_CTRL_COUNT

DESCRIPTION
    This function increments the internal variable diagcomm_flow_ctrl_count, which keeps
    track of the # of times flow control is triggered.

PARAMETERS
  port_type

RETURN VALUE
  The current flow control count.
===========================================================================*/
uint32 diagcomm_incr_flow_ctrl_count( diagcomm_port_type port_type )
{
  if( diagcomm_flow_ctrl_count[port_type] == MAX_VALUE_UINT32 )
  {
    MSG(MSG_SSID_DIAG, MSG_LEGACY_LOW, "diagcomm_flow_ctrl_count has reached max value");
  }
  else
  {
    diagcomm_flow_ctrl_count[port_type]++;
  }

    return diagcomm_flow_ctrl_count[port_type];

} /* diagcomm_incr_flow_ctrl_count */
