/*===========================================================================

                    Shared Memory Driver Main Routine Source File

This file contains the main routine for the SMD task which is OS independent.

-----------------------------------------------------------------------------
Copyright (c) 2005-2013 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
-----------------------------------------------------------------------------
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/23/13   sm      + Rename string copy function to smd_string_copy. 
                   + KW warning fixes.
03/07/13   bm      KW warning fixes.
09/14/12   bt      Use changed version mask name SMEM_FULL_VERSION_MASK.
06/25/12   pa      Cleaning up warnings.
                   Also, fix a minor issue in smd_handle_ssr_close.
03/22/12   bm      Use new macros to access SMD channel header struct.
01/20/11   bm      Added support for RPM proc
12/12/11   bm      Added ioctl SMD_IOCTL_REGISTER_MEMCPY_RESET in 
                   smd_handle_ioctl().
11/23/11   bt      KW warning: check port_id range in smd_cmd().
08/23/11   bt      Added smd_loopback_server/client_update() in smd_main().
04/12/12   vm      Fixed l4/SAL warnings
01/05/12   hwu     Removed IMAGE_APPS_PROC.
07/25/11   rs      Added support for SSR Phase 2
04/25/11   tl      Include smd_target.h to get SMD_APPS_PROC definition
04/01/11   bm      Modified smd_handle_loopback() to be compatible with new
                   definition of smdi_alloc_channel_info() which now takes
                   smd_xfrflow_type as an argument as well.
04/12/11   tl      Start loopback in SMD Lite mode on apps
02/11/11   bt      Removed old unused SMD profiling code.
01/06/11   bt      Removed SMD Block and Data Mover protocols.
11/08/10   rs      Protect open ports list for standalone SMD Lite
10/21/10   rs      Cleaned up redundancy in function calls when sending and 
                   receiving interrupts
09/27/10   tl/ih   Added SMD_CMD_PROCESS_PORT to process a specific port
08/13/09   rs      Removed unused global variables for standalone SMD Lite
08/09/10   bt      Updated call to smem_version_set for new mask argument
08/13/10   tl/rs   Assign context when opening loopback port
07/15/10   rs      Standalone SMD Lite optimizations: 
                     + Removed SMD comands queue
                     + Removed loopback related functions
07/21/10   tl      Support SMD Lite API with multiple task priorities
06/08/10   tl      Split SMD task into multiple tasks with different priorities
05/28/10   hwu     Moved ioctl SMD_IOCTL_SET_XFR_TYPE from dsm layer to 
                   buffer layer.
04/21/10   tl      Implemented SMD Lite API
04/19/10   tl      Fixed case where SMD is instructed to drop data on a
                   connecting stream
03/30/10   tl      Use SMD memory access macros in smd_strncpy()
12/03/09   tl      Set FIFO size for loopback channel
11/19/09   sa      warning cleanup, sync-up with symbian
09/24/09   rs      Fixed typo.
07/17/09   tl      Added CTS/RTS callback
06/01/09   hwu     Changed smd_packet_tx/rx_header size to accomdate ports
                   opened with named open. 
09/25/08   jlk     added protocol version check
08/20/08   bfc     Changed the SMD BLOCK interface.
06/19/08   bfc     Support variable sized SMD FIFOs.
06/10/08   sa      Smd_target.h cleanup. 
04/08/08   sd      Sending wakeup reasons to arm11 from arm9 and Logging these
                   wakeup reasons in arm11 using Celog
04/17/08   hwu     Fixed the channel_type issue.
03/18/08   hwu     Change the API order to enable loopback test.
03/17/08   bfc     Implemented close and added the block unit test.
02/28/08   bfc     Fixed a wraparound issue.
02/21/08   bfc     First SMD Block port check in.
01/14/08   bfc     Created a SMD base struct.
11/15/07   ih      Moved smd_strncpy to smd_main.c since inline functions are 
                   not supported on WM
11/09/07   bfc     Removed strncpy.
10/25/07   hwu     Added client cb_data for rx/tx/flwctl callback functions.
10/25/07   bfc     fixed missing break statement
10/19/07   hwu     Changed loopback port to use name instead of port id.
10/18/07   bfc     Added tramsmit and receive abort ioctls
10/15/07   jlk     added SMD_PROFILE hooks
10/08/07   hwu     Changed the order to include smd_os.h and smd.h
                   because smd_channel_type moved to smd.h
09/12/07   bfc     smd_main now only processes one event, so the arguement
                   was removed, and the processing combined.
08/02/07   bfc     Moved some packet mode code into this file.
07/18/07   hwu     Moved err.h and assert.h to smd_target.h.
06/14/07   ptm     Update SMD loopback to avoid race condition.
06/12/07   bfc     Added a low level packet mode.
05/17/07   hwu     Added the SMD loopback feature.
04/26/07   ptm     Change "stub" port to SMD PORT DUMMY and delete old shared
                   memory data structure references.
03/20/07   taw     Include smd_target.h instead of customer.h and target.h.
02/21/07   ptm     Fix SET FULL RCV BUFS IOCTL.
01/09/07   bfc     Added SMD using DM
01/04/06   bfc     Broke SMD MEMCPY and DMOV out into components.
01/04/07   hwu     Added new commands to handle remote processor reset.
12/18/06   bfc     Implemented port close logic for control ports
12/06/06   bfc     Fixed the location where we were getting the data_read
                   flag from for transmit operations
11/29/06   taw     Use macros to access shared memory data.
11/17/06   hwu     changed to process signal based on the single
                   smd_event_sig signal and the single smd_open_list.
11/14/06   taw     Remove argument to smd_init() & use smem_alloc for the 
                   location of shared memory data.
11/03/06   ptm     Move command loop from smd_main to OS specific files to
                   support a polling environment.
10/13/06   ptm     Remove include of smd_wm.h.
10/12/06   ptm     Remove sync state machine based on SMSM.
10/05/06   bfc     Added new ioctl's that enable/disable Full Rcv Buffer
                   control and edited some function headers.
10/03/06   ptm     Rename interrupt related symbols and remove apps wakeup code.
09/28/06   taw     Switch to using SMD symbols instead of SIO symbols.
09/27/06   bfc     Added an ioctl to support additional functionality while
                   a port is in the Opening state.
09/05/06   ptm     Initial version - derived from smd_task.c#20
===========================================================================*/


/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smd_os.h"

#include "smem_log.h"
#include "smem.h"

#include "smd.h"
#include "smd_internal.h"
#include "smd_target.h"
#include "smd_os_common.h"
#include "smd_main.h"
#include "smd_memcpy.h"
#include "smd_packet.h"
#include "smd_v.h"

/*===========================================================================
                              LOCAL DATA DECLARATIONS
===========================================================================*/
/*-------------------------------------------------------------------------
 *                    Event logging definitions
 *-------------------------------------------------------------------------*/

/* Turn logging on/off here using define/undef */
#undef SMD_MAIN_LOG_EVENTS

#ifdef SMD_MAIN_LOG_EVENTS
#define SMD_MAIN_LOG_EVENT(id,d1,d2,d3)     SMEM_LOG_EVENT(id,d1,d2,d3)
#else
#define SMD_MAIN_LOG_EVENT(id,d1,d2,d3) 
#endif /* SMD_MAIN_LOG_EVENTS */

/*-------------------------------------------------------------------------
 *                    SMD command queue definitions
 *-------------------------------------------------------------------------*/
typedef struct
{
  q_link_type link;
  smd_cmd_type cmd;
} smdi_cmd_type;

#ifdef FEATURE_SMD
#define SMD_CMD_BUF_CNT    75

static smdi_cmd_type smd_cmd_buf[SMD_CMD_BUF_CNT];
static q_type smd_cmd_free_q;
#endif

#ifdef FEATURE_SMD
/*-------------------------------------------------------------------------
 *                    SMD loopback definitions
 *-------------------------------------------------------------------------*/
#define  SMD_LOOPBACK_BUF_SIZE   512

typedef enum
{
  SMD_LOOPBACK_S_TX,
  SMD_LOOPBACK_S_TX_WAIT,
  SMD_LOOPBACK_S_RX,
  SMD_LOOPBACK_S_RX_WAIT,
}smd_loopback_state;

typedef struct
{
  uint8   tx_rx_buf[SMD_LOOPBACK_BUF_SIZE]; /* tx/rx buff */
  uint32  size;                             /* data to be sent */

  smd_loopback_state    state;
}smd_lp_slave_info_type;

static smd_lp_slave_info_type   smd_lp_slave_info;

/*-------------------------------------------------------------------------
 *                    SMD packet definitions
 *-------------------------------------------------------------------------*/
static byte smd_packet_memcpy_scratch_buf[SMD_PACKET_SCRATCH_SZ];

/* packet_header's must be CACHE aligned for the DMOVer*/
static uint32 smd_packet_rx_header[SMD_NUM_CHANNELS][SMD_PACKET_HEADER_SIZE];
static uint32 smd_packet_tx_header[SMD_NUM_CHANNELS][SMD_PACKET_HEADER_SIZE];
#endif


/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smd_process_update

DESCRIPTION   Handle changes to the interface signals as required. To avoid
              race conditions, the values from shared memory are read only
              once into local variables.

              If DTR has changed state call dtr callback (if any).

              If CD has changed state call cd callback (if any).

              If RTS becomes set start flow (if required).

ARGUMENTS     info - pointer to stream info structure

RETURN VALUE  None.
===========================================================================*/
static void smd_process_update( smd_stream_info_type *info )
{
  uint8 new_dtr_sig;
  uint8 new_cd_sig;
  uint8 new_ri_sig;
  uint8 new_rts_sig;

#ifdef SMD_MAIN_LOG_EVENTS
  uint32 old_sigs;

  old_sigs = info->prev_dtr | (info->prev_rts << 1) | (info->prev_cd << 2)
             | (info->prev_ri <<3);
#endif

  /* If state of DTR bit has changed, call the DTR callback */
  new_dtr_sig = (uint8) SMD_READ_IF_SIG( info, SMD_RX_HDR, dtr_sig );

  if( info->prev_dtr !=  new_dtr_sig )
  {
    info->prev_dtr = new_dtr_sig;

    /* call the stream dtr change function */
    info->stream_dtr( info );

    /* Call the "simple" callback */
    if( info->dtr_callback_fn != NULL )
    {
      (info->dtr_callback_fn)();
    }

    /* Call the "extended" callback */
    if( info->dtr_callback_ext_fn != NULL )
    {
      info->dtr_callback_ext_fn( info->dtr_callback_ext_data );
    }
  }


  /* If the state of the CD bit has changed, call the CD callback */
  new_cd_sig = (uint8) SMD_READ_IF_SIG( info, SMD_RX_HDR, cd_sig );

  if( info->prev_cd !=  new_cd_sig )
  {
    info->prev_cd = new_cd_sig;

    /* Call the "extended" callback */
    if( info->cd_callback_ext_fn != NULL )
    {
      info->cd_callback_ext_fn( info->cd_callback_ext_data );
    }
  }

  new_ri_sig = (uint8) SMD_READ_IF_SIG( info, SMD_RX_HDR, ri_sig );

  if( info->prev_ri !=  new_ri_sig )
  {
    info->prev_ri = new_ri_sig;

    /* Call the "extended" callback */
    if( info->ri_callback_ext_fn != NULL )
    {
      info->ri_callback_ext_fn( info->ri_callback_ext_data );
    }
  }

  /* If flow control is enabled,
  ** and RTS state has changed and gone to flow enabled (on)
  ** start flow again.
  */
  new_rts_sig = (uint8) SMD_READ_IF_SIG( info, SMD_RX_HDR, rts_sig );

  if ( info->prev_rts != new_rts_sig )
  {
    info->prev_rts = new_rts_sig;

    /* Call the "extended" CTS/RTS callback */
    if( info->cts_callback_ext_fn != NULL )
    {
      info->cts_callback_ext_fn( info->cts_callback_ext_data );
    }

    if( info->tx_fctl_enabled && new_rts_sig )
    {
      info->stream_write( info );
    }
  }

  SMD_MAIN_LOG_EVENT( SMEM_LOG_EVENT_SIGS2,
                      info->port_id,
                      old_sigs,
                      new_dtr_sig | (new_rts_sig << 1) | (new_cd_sig << 2 ) |
                      (new_ri_sig << 3) );
} /* smd_process_update */

/*===========================================================================
FUNCTION      smd_handle_flush

DESCRIPTION   This routine handles a flush command for the give port. It saves
              the flush callback information and calls the flush routine
              associated with the dataxfr mode (via the jump table in info).

ARGUMENTS     port_id - SMD port to flush

              flush_cb - callback to call when other processor has received
                         all pending data

              cb_data - user data passed to the flush callback routine

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void smd_handle_flush
(
  smd_port_id_type      port_id,
  smd_flush_callback_fn flush_cb,
  void                  *cb_data
)
{
  smd_stream_info_type *info;

  info = smdi_get_stream_info( port_id );

  if(info == NULL)
    return;

  ASSERT( SMD_STREAM_OPEN(info) );

  if(!SMD_STREAM_OPEN(info))
    return;

  info->flush_cb = flush_cb;
  info->flush_cb_data = cb_data;

  info->stream_flush_tx( info );
} /* smd_handle_flush */

/*===========================================================================
FUNCTION      smd_close_cb

DESCRIPTION   This function is called when a stream is ready to be closed
              either immediately, from smd_handle_close, or when the flush
              associated with a close completes. It sends a CLOSE event to
              the stream state machine.

ARGUMENTS     data - pointer to the info structure associated with the port_id

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void smd_close_cb
(
  void *data
)
{
  smd_stream_info_type *info = (smd_stream_info_type *) data;

  smdi_stream_state_machine( info, SMD_EVENT_CLOSE );
} /* smd_close_cb */

/*===========================================================================
FUNCTION      smd_handle_close

DESCRIPTION   Closes an open stream.

ARGUMENTS     port_id - SMD port to close

              close_cb - callback that is called when the port is actually
                         closed.

              cb_data - user data passed to the close callback routine.

              flush - If TRUE, flush the stream before closing otherwise
                      close the stream immediately.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void smd_handle_close
(
  smd_port_id_type      port_id,
  smd_close_callback_fn close_cb,
  void                  *cb_data,
  boolean               flush
)
{
  smd_stream_info_type *info;

  /* This is the DUMMY port, just call the cb. */
  if( port_id == SMD_PORT_DUMMY )
  {
    if( close_cb != NULL )
    {
      close_cb( cb_data );
    }
    return;
  }

  info = smdi_get_stream_info( port_id );

  if(info == NULL)
    return;

  ASSERT( SMD_STREAM_OPEN(info) );

  if(!SMD_STREAM_OPEN(info))
    return;


  /* Save close callback info */
  info->close_cb = close_cb;
  info->close_cb_data = cb_data;

  if( flush )
  {
    smd_handle_flush( port_id, smd_close_cb, (void *) info );
  }
  else
  {
    smd_close_cb( (void *)info );
  }
} /* smd_handle_close */

#ifdef FEATURE_SMD
/*===========================================================================
FUNCTION      smd_set_flow_ctl

DESCRIPTION   This function will set the flow control method between the DCE
              and the DTE for the given device.

ARGUMENTS     info - pointer to stream info structure

              tx_flow - TX flow control method

              rx_flow - RX flow control method

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void smd_set_flow_ctl
(
  smd_stream_info_type *info,
  smd_flow_ctl_type  tx_flow,
  smd_flow_ctl_type  rx_flow
)
{
  /* Best means no change. */
  if ( tx_flow != SMD_FCTL_BEST)
  {
    info->tx_flow_control_method = tx_flow;
  }

  /* Only hardware flow control (SMD_CTSRFR_FCTL) supported */
  if (info->tx_flow_control_method == SMD_CTSRFR_FCTL)
  {
    info->tx_fctl_enabled = TRUE;
  }
  else
  {
    info->tx_fctl_enabled = FALSE;
  }

  /* Best means no change. */
  if ( rx_flow != SMD_FCTL_BEST)
  {
    info->rx_flow_control_method = rx_flow;
  }

  /* Only hardware flow control (SMD_CTSRFR_FCTL) supported */
  if (info->rx_flow_control_method == SMD_CTSRFR_FCTL)
  {
    info->rx_fctl_enabled = TRUE;
  }
  else
  {
    info->rx_fctl_enabled = FALSE;
    /* Since flow control is disabled, we need to assert CTS so the remote
    ** may send data. Disable flow control means data can flow freely so
    ** CTS must remain set at all time. */
    SMD_WRITE_IF_SIG( info, SMD_TX_HDR, cts_sig, TRUE );
    smdi_event_send_update( info );
  }
} /* smd_set_flow_ctl */

/*===========================================================================
FUNCTION      smd_packet_status_change

DESCRIPTION   This function will set the transfer mode to PACKET
              or STREAMING.

ARGUMENTS     info - pointer to stream info structure

              xfrflow_type - the new transfer mode

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void smd_packet_status_change
( 
  smd_stream_info_type *info,
  smd_xfrflow_type xfrflow
)
{
  if( (xfrflow >= SMD_INVALID_TYPE) )
  {
    ERR_FATAL("Invalid transfer mode=%i for port=%i", 
              xfrflow, info->port_id, 0);
  }

  /* only process a change in flow type */
  if( info->xfrflow != xfrflow )
  {
    info->xfrflow = xfrflow;

    if( info->xfrflow == SMD_PKTXFR_TYPE )
    {
      /* full_rcv_buf_en must be set for the packet layer */
      info->ret_full_rcv_bufs_en = TRUE;
    }

    if( info->dataxfr_mode == SMD_MEMCPY_MODE )
    {
      smd_packet_memcpy_status_change( info,
                                       smd_packet_memcpy_scratch_buf,
                                       smd_packet_rx_header[info->port_id],
                                       smd_packet_tx_header[info->port_id] );
    }
  }
}/* smd_packet_status_change */

/*===========================================================================
FUNCTION      smd_handle_ioctl

DESCRIPTION   This function will set the flow control method between the DCE
              and the DTE for the given device.

ARGUMENTS     port_id - SMD port to control

              command - SMD IOCTL command

              param   - pointer to union containing parameters or results for
                        command - may be NULL if the command does not require
                        any parameters

DEPENDENCIES  Must only be called from smd_handle_cmd because it decrements
              the pending IOCTL count.

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void smd_handle_ioctl
(
  smd_port_id_type      port_id,
  smd_ioctl_cmd_type    cmd,
  smd_ioctl_param_type *param
)
{
  smd_stream_info_type *info;
  uint32 stream_state;

  info = smdi_get_stream_info( port_id );
  switch ( cmd )
  {
    case SMD_IOCTL_CD_ASSERT:         /* Assert the Carrier Detect signal   */
      SMD_WRITE_IF_SIG( info, SMD_TX_HDR, cd_sig, TRUE );
      smdi_event_send_update( info );
      break;

    case SMD_IOCTL_CD_DEASSERT:      /* De-assert the carrier detect signal */
      SMD_WRITE_IF_SIG( info, SMD_TX_HDR, cd_sig, FALSE );
      smdi_event_send_update( info );
      break;

    case SMD_IOCTL_RI_ASSERT:         /* Assert the Ring Indicator signal   */
      SMD_WRITE_IF_SIG( info, SMD_TX_HDR, ri_sig, TRUE );
      smdi_event_send_update( info );
      break;

    case SMD_IOCTL_RI_DEASSERT:      /* De-assert the Ring Indicator signal */
      SMD_WRITE_IF_SIG( info, SMD_TX_HDR, ri_sig, FALSE );
      smdi_event_send_update( info );
      break;

    case SMD_IOCTL_ENABLE_DTR_EVENT:/* Register DTR-Toggle callback function */
      info->dtr_callback_fn = param->enable_dte_ready_event;
      ASSERT(info->dtr_callback_fn);
      break;

    case SMD_IOCTL_DISABLE_DTR_EVENT:/*Deregister the DTR-Toggle callback fn*/
      info->dtr_callback_fn = NULL;
      break;

    case SMD_IOCTL_INBOUND_FLOW_ENABLE: /* Enable inbound flow */
      if ( info->rx_fctl_enabled )
      {
        SMD_WRITE_IF_SIG( info, SMD_TX_HDR, cts_sig, TRUE );
        smdi_event_send_update( info );

        /* Reading was blocked when CTS was de-asserted. So, if the stream is
           open, read now */
        stream_state =
                SMD_READ_CH_HDR( info, SMD_TX_HDR, stream_state );

        if( stream_state == SMD_SS_OPENED ||
            stream_state == SMD_SS_FLUSHING )
        {
          SMD_WRITE_IF_SIG( info, SMD_RX_HDR, data_written, TRUE );
          smd_event_send_smd( port_id );
        }
      }
      break;

    case SMD_IOCTL_INBOUND_FLOW_DISABLE:/* Disable inbound flow */
      if ( info->rx_fctl_enabled )
      {
        SMD_WRITE_IF_SIG( info, SMD_TX_HDR, cts_sig, FALSE );
        smdi_event_send_update( info );
      }
      break;

    case SMD_IOCTL_DSR_ASSERT:
      SMD_WRITE_IF_SIG( info, SMD_TX_HDR, dsr_sig, TRUE );
      smdi_event_send_update( info );
      break;

    case SMD_IOCTL_DSR_DEASSERT:
      SMD_WRITE_IF_SIG( info, SMD_TX_HDR, dsr_sig, FALSE );
      smdi_event_send_update( info );
      break;

    case SMD_IOCTL_SET_FLOW_CTL:
      smd_set_flow_ctl( info,
                        param->flow_ctl.tx_flow,
                        param->flow_ctl.rx_flow);
      break;

    case SMD_IOCTL_SET_BUFFER_SIZE:
      /*************************************************************
       * - 4 because write_index == read_index => buffer is empty
       * - 4 instead of - 1 to try to keep transfers word aligned
       *************************************************************/
      if( param->buffer_size > info->fifo_sz - 4 )
      {
        info->max_queued_data = info->fifo_sz - 4;
      }
      else
      {
        info->max_queued_data = param->buffer_size;
      }
      break;

    case SMD_IOCTL_SET_DROP_PEND:
      info->tx_opening_drop_enabled = param->drop_asserted;
      break;

    case SMD_IOCTL_SET_FULL_RCV_BUFS:
      if( info->xfrflow != SMD_PKTXFR_TYPE )
      {
        info->ret_full_rcv_bufs_en = param->full_bufs_asserted;
      }
      break;

    case SMD_IOCTL_SET_XFR_TYPE:
      if( smdi_set_channel_xfrflow( port_id, param->xfrflow_type ) != SMD_DONE )
      {
        ERR ("SMD: XFRFLOW Mismatch detected port %d\n", port_id, 0, 0);
      }
      break;

    case SMD_IOCTL_SET_XFR_MODE:
      break;

    case SMD_IOCTL_ENABLE_DTR_EVENT_EXT:
      info->dtr_callback_ext_fn = param->enable_dte_ready_event_ext.cb_func;
      info->dtr_callback_ext_data = param->enable_dte_ready_event_ext.cb_data;
      ASSERT(info->dtr_callback_ext_fn);
      break;

    case SMD_IOCTL_DISABLE_DTR_EVENT_EXT:
      info->dtr_callback_ext_fn = NULL;
      info->dtr_callback_ext_data = NULL;
      break;

    case SMD_IOCTL_ENABLE_CD_EVENT_EXT:
      info->cd_callback_ext_fn = param->enable_cd_ready_event_ext.cb_func;
      info->cd_callback_ext_data = param->enable_cd_ready_event_ext.cb_data;
      break;

    case SMD_IOCTL_DISABLE_CD_EVENT_EXT:
      info->cd_callback_ext_fn = NULL;
      info->cd_callback_ext_data = 0;
      break;

    case SMD_IOCTL_ENABLE_RI_EVENT_EXT:
      info->ri_callback_ext_fn = param->enable_ri_ready_event_ext.cb_func;
      info->ri_callback_ext_data = param->enable_ri_ready_event_ext.cb_data;
      break;

    case SMD_IOCTL_DISABLE_RI_EVENT_EXT:
      info->ri_callback_ext_fn = NULL;
      info->ri_callback_ext_fn = 0;
      break;

    case SMD_IOCTL_ENABLE_CTS_EVENT_EXT:
      info->cts_callback_ext_fn = param->enable_cts_ready_event_ext.cb_func;
      info->cts_callback_ext_data = param->enable_cts_ready_event_ext.cb_data;
      break;

    case SMD_IOCTL_DISABLE_CTS_EVENT_EXT:
      info->cts_callback_ext_fn = NULL;
      info->cts_callback_ext_data = 0;
      break;

    case SMD_IOCTL_SET_PACKET_MODE:
      smd_packet_status_change(info, param->xfrflow_type );
      break;

    case SMD_IOCTL_ABORT_TX:
      if( !param->abort_ext.cb_func )
      {
        ERR_FATAL("SMD_IOCTL_ABORT_TX: No clnt callback",0,0,0);
      }

      param->abort_ext.cb_func( param->abort_ext.cb_data,
                                info->stream_tx_abort(info));
      break;

    case SMD_IOCTL_ABORT_RX:
      if( !param->abort_ext.cb_func )
      {
        ERR_FATAL("SMD_IOCTL_ABORT_RX: No clnt callback",0,0,0);
      }

      param->abort_ext.cb_func( param->abort_ext.cb_data,
                                info->stream_rx_abort(info));
      break;
    case SMD_IOCTL_REGISTER_MEMCPY_RESET:
      /* Set the callback in the MEMCPY structure */
      info->mode.memcpy.reset_callback_fn = param->memcpy_reset.cb_func;
      info->mode.memcpy.reset_callback_data = param->memcpy_reset.cb_data;
      break;

    default:
      ASSERT( FALSE );          /* Invalid ioctl */
      break;
  }

  SMD_IOCTL_LOCK();
  info->pending_ioctl_cnt--;
  SMD_IOCTL_UNLOCK();
} /* smd_handle_ioctl */

/*===========================================================================
FUNCTION      smd_handle_remote_reset

DESCRIPTION   This function handles the reset from the other processor
              by transition all the open streams to RESET state and
              calls the dtr callbacks to clean up if necessary. It then
              calls smd_down() to signal the upper layer that SMD is
              down now.

ARGUMENTS     context - SMD task context object

RETURN VALUE  None

SIDE EFFECTS
===========================================================================*/
static void smd_handle_remote_reset( smd_context_type * context )
{
  smd_info_type        *info;
  smd_stream_info_type *sinfo;

  /* for all the open streams, transition to the RESET state */
  for( info = context->open_list; info != NULL; info = info->next )
  {
    if( info->protocol != SMD_STREAMING_BUFFER )
    {
      continue;
    }

    sinfo = &info->info.stream;

    smdi_stream_state_machine(sinfo, SMD_EVENT_REMOTE_RESET);

    /* let the upper layer know DTR is de-asserted */
    if(sinfo->prev_dtr)
    {
        sinfo->prev_dtr = FALSE;

        /* call the stream dtr change function */
        sinfo->stream_dtr( sinfo );

        /* Call the "simple" callback */
        if( sinfo->dtr_callback_fn != NULL )
        {
          (sinfo->dtr_callback_fn)();
        }

        /* Call the "extended" callback */
        if( sinfo->dtr_callback_ext_fn != NULL )
        {
          sinfo->dtr_callback_ext_fn( sinfo->dtr_callback_ext_data );
        }
    }

    /* update the previous state for the remote side to CLOSED */
    sinfo->prev_remote_state = 0;
  }

  /* signal the clients that SMD is down now */
  smd_down();

} /* smd_handle_remote_reset */

/*===========================================================================
FUNCTION      smd_handle_remote_reset_done

DESCRIPTION   This function is called when the remote processor comes
              out of the SMSM_RESET state. It calls smd_up() to signal
              the upper layer that the SMD is up now.

ARGUMENTS     context - SMD task context object

RETURN VALUE  None
===========================================================================*/
static void smd_handle_remote_reset_done( smd_context_type * context )
{
  /* signal the clients that SMD is up now */
  smd_up();
} /* smd_handle_remote_reset_done */

/*===========================================================================
FUNCTION      smd_lp_slave_tx_cb

DESCRIPTION   This routine is called when loopback slave needs a new
              buffer to send either because it does not have a buffer or
              because it is done with the previous buffer.

ARGUMENTS     cb_data - A pointer to the client callback data.

              buf_ptr - the next transmit buffer (output)

              size - the number of bytes in the transmit buffer (output)

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_lp_slave_tx_cb
(
  void              *cb_data,
  byte             **buf_ptr,
  uint32            *size
)
{
  smd_stream_info_type *info = (smd_stream_info_type *)cb_data;

  switch( smd_lp_slave_info.state )
  {
    case SMD_LOOPBACK_S_TX:
      /* transmit what have just read */
      *buf_ptr = smd_lp_slave_info.tx_rx_buf;
      *size    = smd_lp_slave_info.size;

      smd_lp_slave_info.state = SMD_LOOPBACK_S_TX_WAIT;
      break;

    case SMD_LOOPBACK_S_TX_WAIT:
      /* stop transmitting */
      *buf_ptr = NULL;
      *size = 0;

      smd_lp_slave_info.state = SMD_LOOPBACK_S_RX;

      /* poke to see if there is more to be read */
      smd_notify_read(info->port_id);
      break;

    default:
      /* ignore other states */
      *buf_ptr = NULL;
      *size    = 0;
      break;
  }
}/* smd_lp_slave_tx_cb */

/*===========================================================================
FUNCTION      smd_lp_slave_rx_cb

DESCRIPTION   This routine is called when loopbacak slave needs a
              buffer for incoming data either because it does not have a
              buffer or it is done with the one it has.

              If bytes_read is 0, SMD did not have a previous buffer,
              otherwise, bytes_read contains the number of bytes that were
              actually transferred in to the previous buffer.

              It is assumed that SMD may not fill the buffers completely.

ARGUMENTS     cb_data - A pointer to the client callback data.

              bytes_read - the number of bytes read into the previous buffer,
                           0 means there was not a previous buffer.

              buf_ptr - the next read buffer (output)

              size - the size of the read buffer (output)

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  None.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static void smd_lp_slave_rx_cb
(
  void              *cb_data,
  uint32             bytes_read,
  byte             **buf_ptr,
  uint32            *size
)
{
  smd_stream_info_type *info = (smd_stream_info_type *)cb_data;

  switch( smd_lp_slave_info.state )
  {
    case SMD_LOOPBACK_S_RX:
      /* give the full buffer for receiving */
      *buf_ptr = smd_lp_slave_info.tx_rx_buf;
      *size    = SMD_LOOPBACK_BUF_SIZE;

      smd_lp_slave_info.state = SMD_LOOPBACK_S_RX_WAIT;
      break;

    case SMD_LOOPBACK_S_RX_WAIT:
      /* transmit what is read if any */
      if(bytes_read)
      {
        *buf_ptr = NULL;
        *size    = 0;

        /* store the size to be transmitted */
        smd_lp_slave_info.size = bytes_read;
        smd_lp_slave_info.state = SMD_LOOPBACK_S_TX;
        smd_notify_write(info->port_id);
      }
      else
      {
        /* We were waiting for data, but didn't get any */
        ERR_FATAL( "Invalid loopback state/bytes_read count", 0, 0, 0 );
      }
      break;

    default:
      /* ignore other states */
      *buf_ptr = NULL;
      *size    = 0;
      break;
  }
} /* smd_lp_slave_rx_cb */

/*===========================================================================
FUNCTION      smd_lp_flwctl_cb

DESCRIPTION   This routine is called when it did not get a buffer
              and flow control is not enabled. Not implemented for
              loopback port.

ARGUMENTS     cb_data - A pointer to the client callback data.

              byte_count - the number of bytes currently available in the
              stream

DEPENDENCIES  The stream must have been opened.

RETURN VALUE  The number of bytes to skip.

SIDE EFFECTS  Called in SMD context
===========================================================================*/
static uint32 smd_lp_flwctl_cb
(
  void   *cb_data,
  uint32  byte_count
)
{
  ERR_FATAL("smd lp flwctl: byte_count = %d", byte_count, 0, 0);
  return byte_count;
}/* smd_lp_flwctl_cb */
#endif

/*===========================================================================
FUNCTION      smd_handle_stream_event

DESCRIPTION   This function process the stream event signal for a stream port.
              It could be an update, transmit or receive.

ARGUMENTS     info - information about the stream

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
static void smd_handle_stream_event(smd_info_type *info)
{
  uint32 curr_stream_state = 0;
  smd_stream_info_type *sinfo;

  sinfo = &info->info.stream;

  /* buffer stream_state since this is an expensive access */
  curr_stream_state =
      SMD_READ_CH_HDR( sinfo, SMD_TX_HDR, stream_state );

  /* process update */
  if( SMD_READ_IF_SIG( sinfo, SMD_RX_HDR, state_updated ) )
  {
      /* update the state_updated flag first */
      SMD_WRITE_IF_SIG( sinfo, SMD_RX_HDR, state_updated, FALSE );

      /* this is a connected stream */
      if((curr_stream_state == SMD_SS_OPENED) ||
         (curr_stream_state == SMD_SS_FLUSHING ))
      {
          smd_process_update( sinfo );
          smdi_stream_state_update( sinfo );
      }
      /* this is connecting stream */
      else
      {
          smdi_stream_state_update( sinfo );

          /* the connecting stream just connected */
          curr_stream_state =
               SMD_READ_CH_HDR( sinfo, SMD_TX_HDR, stream_state );
          if( (curr_stream_state == SMD_SS_OPENED) ||
              (curr_stream_state == SMD_SS_FLUSHING ))
          {
              smd_process_update(sinfo);
          }
      }
  }

  /* process transmit */
  if( SMD_READ_IF_SIG( sinfo, SMD_RX_HDR, data_read ) )
  {
    /* this is a connected stream */
    if( (curr_stream_state == SMD_SS_OPENED )   ||
        (curr_stream_state == SMD_SS_FLUSHING ))
    {
      SMD_WRITE_IF_SIG( sinfo, SMD_RX_HDR, data_read, FALSE );
      sinfo->stream_write( sinfo );
    }
  }
  else if( sinfo->transmit_pending && sinfo->tx_opening_drop_enabled )
  {
    /* This is a connecting stream, and we've been instructed to drop data,
     * rather than queue it until the port is opened by the remote processor;
     * drop pending data. */
    sinfo->stream_drop( sinfo );
  }

  /* process receive */
  if( SMD_READ_IF_SIG( sinfo, SMD_RX_HDR, data_written ) )
  {
    /* this is a connected stream */
    if( (curr_stream_state == SMD_SS_OPENED )   ||
        (curr_stream_state == SMD_SS_FLUSHING ))
    {
      SMD_WRITE_IF_SIG( sinfo, SMD_RX_HDR, data_written, FALSE );
      sinfo->stream_read( sinfo );
    }
  }
}

#ifdef FEATURE_SMD
/*===========================================================================
FUNCTION      smd_handle_loopback

DESCRIPTION   This function is called when the other processor opens the
              loopback port. This processor opens its loopback port
              accordingly to establish a loopback connection.

ARGUMENTS     None

RETURN VALUE  None
===========================================================================*/
static void smd_handle_loopback
(
  const char       *port_name,
  smd_channel_type  channel_type
)
{
  smd_info_type        *info;
  smd_stream_info_type *sinfo;
  uint32                port_id;

  /* initialize fields of loopback slave info structure */
  smd_lp_slave_info.size  = SMD_LOOPBACK_BUF_SIZE;
  smd_lp_slave_info.state = SMD_LOOPBACK_S_RX;

  /* open the port first */
  port_id = smdi_alloc_channel_info(port_name,
                                    channel_type, 
                                    SMD_STREAMING_BUFFER,
                                    SMD_NO_XFRFLOW);
  if(port_id == SMD_PORT_INVALID)
    return;

  info = smdi_alloc_info( port_id );
  sinfo = &info->info.stream;

  /* udpate the top level info fields */
  smd_string_copy(info->port_name, port_name, SMD_CHANNEL_NAME_SIZE_MAX);
  info->port_id      = port_id;
  info->channel_type = channel_type;
  info->protocol     = SMD_STREAMING_BUFFER;
  info->handle_event = smd_handle_stream_event;

  /* initialize "simple" fields of stream info structure */
  sinfo->port_id      = port_id;
  sinfo->channel_type = channel_type;
  
  sinfo->tx_flow_control_method = SMD_CTSRFR_FCTL;
  sinfo->rx_flow_control_method = SMD_CTSRFR_FCTL;
  sinfo->fifo_sz = SMD_STANDARD_FIFO;

  sinfo->tx_fctl_enabled = TRUE;
  sinfo->rx_fctl_enabled = TRUE;

  sinfo->dataxfr_mode = SMD_MEMCPY_MODE;

  sinfo->mode.memcpy.tx_callback_fn   = smd_lp_slave_tx_cb;
  sinfo->mode.memcpy.tx_callback_data = (void *)sinfo;
  sinfo->mode.memcpy.rx_callback_fn   = smd_lp_slave_rx_cb;
  sinfo->mode.memcpy.rx_callback_data = (void *)sinfo;
  sinfo->mode.memcpy.rx_flowctl_fn    = smd_lp_flwctl_cb;
  sinfo->mode.memcpy.rx_flowctl_data  = (void *)sinfo;
  
  smd_assign_context( info, SMD_PRIORITY_DEFAULT );

  /* Send command to smd task */
  smd_memcpy_open( port_id );

  /* set flow control, dsr assert */
  SMD_WRITE_IF_SIG( sinfo, SMD_TX_HDR, dsr_sig, TRUE );

  SMD_WRITE_IF_SIG( sinfo, SMD_TX_HDR, cts_sig, TRUE );
  smdi_event_send_update( sinfo );

} /* smd_handle_loopback */


/*===========================================================================
FUNCTION      smd_handle_cmd

DESCRIPTION   Dispatch commands queued for the smd task to their handlers

ARGUMENTS     context - SMD task context object

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
void smd_handle_cmd( smd_context_type * context )
{
  smdi_cmd_type *cmdi;
  smd_info_type *info;

  while( (cmdi = (smdi_cmd_type *)q_get(&context->cmd_queue)) != NULL )
  {
    switch( cmdi->cmd.cmd )
    {
      case SMD_CMD_OPEN_MEMCPY:
        smd_memcpy_open( cmdi->cmd.data.open_memcpy.port_id );

        info = smdi_get_info( cmdi->cmd.data.open_memcpy.port_id );

        info->handle_event = smd_handle_stream_event;

        /* if this is loopback port, starts the other side */
        if( cmdi->cmd.data.open_memcpy.is_loopback )
        {
          smd_event_start_loopback();
        }
        break;

      case SMD_CMD_OPEN_SMDLITE:

        info = smdi_get_info( cmdi->cmd.data.open_memcpy.port_id );
        info->handle_event = smd_handle_stream_event;

        smd_lite_open( cmdi->cmd.data.open_memcpy.port_id );

        /* if this is loopback port, starts the other side */
        if( cmdi->cmd.data.open_memcpy.is_loopback )
        {
          smd_event_start_loopback();
        }
        break;

      case SMD_CMD_CLOSE:
        smd_handle_close( cmdi->cmd.data.close.port_id,
                          cmdi->cmd.data.close.close_cb,
                          cmdi->cmd.data.close.cb_data,
                          cmdi->cmd.data.close.flush );
        break;

      case SMD_CMD_FLUSH:
        smd_handle_flush( cmdi->cmd.data.flush.port_id,
                          cmdi->cmd.data.flush.flush_cb,
                          cmdi->cmd.data.flush.cb_data );
        break;

      case SMD_CMD_IOCTL:
        smd_handle_ioctl( cmdi->cmd.data.ioctl.port_id,
                          cmdi->cmd.data.ioctl.cmd,
                          &cmdi->cmd.data.ioctl.param );
        break;

      case SMD_CMD_REMOTE_RESET:
        smd_handle_remote_reset(context);
        break;

      case SMD_CMD_REMOTE_RESET_DONE:
        smd_handle_remote_reset_done(context);
        break;

      case SMD_CMD_LOOPBACK:
        smd_handle_loopback("LOOPBACK", SMD_APPS_MODEM);
        break;

      case SMD_CMD_PROCESS_PORT:
        info = smdi_get_info( cmdi->cmd.data.process_port.port_id );
        info->handle_event(info);
        break;

      default:
        ERR( "Unknown cmd %d ignored", cmdi->cmd.cmd, 0, 0 );
        break;
    }

    q_put( &smd_cmd_free_q, &cmdi->link );
  }
} /* smd_handle_cmd */

#else

/*===========================================================================
FUNCTION      smd_lite_handle_cmd
===========================================================================*/
/**
  Dispatches commands for standalone SMD Lite (which is taskless).
 
  @param[in] cmd         SMD Lite command.

  @return
  NA.
 
  @dependencies
  None.

  @sideeffects
  Interrupts the remote processor.
*/
/*=========================================================================*/
static void smd_lite_handle_cmd( smd_cmd_type *cmd )
{
  smd_info_type    *info;

  switch( cmd->cmd )
  {
    case SMD_CMD_OPEN_SMDLITE:
      info = smdi_get_info( cmd->data.open_memcpy.port_id );
      info->handle_event = smd_handle_stream_event;
      smd_lite_open( cmd->data.open_memcpy.port_id );

#ifdef SMD_APPS_PROC
      /* if this is loopback port, starts the other side */
      if( cmd->data.open_memcpy.is_loopback )
      {
        smd_event_start_loopback();
      }
#endif /* SMD_APPS_PROC */
      break;

    case SMD_CMD_CLOSE:
      smd_handle_close( cmd->data.close.port_id,
                        cmd->data.close.close_cb,
                        cmd->data.close.cb_data,
                        cmd->data.close.flush );
      break;

    default:
      ERR( "Unknown cmd %d ignored", cmd->cmd, 0, 0 );
      break;
  }
}
#endif


/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smd_init

DESCRIPTION   Initialize the SMD. Calls init functions for various subsystems,
              and initializes the command queue.

RETURN VALUE  None
===========================================================================*/
void smd_init( void )
{
#ifdef FEATURE_SMD
  uint32 i;
#endif

  /* Initialize internal structures and wm */
  smdi_init();

#ifdef FEATURE_SMD
  /* Initialize smd command queue */
  (void) q_init( &smd_cmd_free_q );

  for( i = 0; i < SMD_CMD_BUF_CNT; i++ )
  {
    q_put( &smd_cmd_free_q, q_link( &smd_cmd_buf[i], &smd_cmd_buf[i].link ) );
  }
#endif

  if(smem_version_set(SMEM_VERSION_SMD, SMD_PROTOCOL_VERSION,
      SMEM_FULL_VERSION_MASK) == FALSE)
  {
  /* this will only trigger if another process set a version which does not
     match this one */
     ERR_FATAL("smd_init: SMD protocol version (%d) does not match", 
               SMD_PROTOCOL_VERSION, 0, 0);
  }
} /* smd_init */

/*===========================================================================
FUNCTION      smd_cmd

DESCRIPTION   This routine copies the given cmd buffer into a command queue
              structure, puts the command on the SMD command queue and sets
              the command queue signal for the SMD task.

ARGUMENTS     cmd_ptr - pointer of the command buffer to copy

RETURN VALUE  None

SIDE EFFECTS  Queues command, signals SMD task
===========================================================================*/
void smd_cmd( smd_cmd_type *cmd_ptr )
{
  smd_info_type *info;
  smd_port_id_type port_id = SMD_PORT_INVALID;

  switch(cmd_ptr->cmd)
  {
    case SMD_CMD_OPEN_MEMCPY:
    case SMD_CMD_OPEN_SMDLITE:
      port_id = cmd_ptr->data.open_memcpy.port_id;
      break;
    case SMD_CMD_CLOSE:
      port_id = cmd_ptr->data.close.port_id;
      break;
    case SMD_CMD_FLUSH:
      port_id = cmd_ptr->data.flush.port_id;
      break;
    case SMD_CMD_IOCTL:
      port_id = cmd_ptr->data.ioctl.port_id;
      break;
    case SMD_CMD_REMOTE_RESET:
    case SMD_CMD_REMOTE_RESET_DONE:
      /* These commands are sent to all SMD worker tasks at once */
      smd_cmd_broadcast(cmd_ptr);
      break;
    case SMD_CMD_LOOPBACK:
      /* This command is sent to the default SMD task */
      smd_cmd_context(cmd_ptr, smd_context_get_default());
      break;
    case SMD_CMD_PROCESS_PORT:
      port_id = cmd_ptr->data.process_port.port_id;
      break;
    default:
      ERR_FATAL( "Unrecogonized SMD command %d", cmd_ptr->cmd, 0, 0 );
  }

  if(port_id != SMD_PORT_INVALID && port_id < SMD_NUM_CHANNELS)
  {
    info = smdi_get_info(port_id);
    smd_cmd_context(cmd_ptr, info->context);
  }
} /* smd_cmd */

/*===========================================================================
FUNCTION      smd_cmd_context

DESCRIPTION   Sends a command to the particular SMD task context.

ARGUMENTS     cmd_ptr - pointer of the command buffer to copy

              context - pointer to the SMD context

RETURN VALUE  None

SIDE EFFECTS  Queues command, signals SMD task
===========================================================================*/
void smd_cmd_context( smd_cmd_type *cmd_ptr, smd_context_type *context)
{
#ifdef FEATURE_SMD
  smdi_cmd_type *link_ptr;
#endif

  ASSERT(context != NULL);

#ifdef FEATURE_SMD
  if(( link_ptr = (smdi_cmd_type *)q_get( &smd_cmd_free_q )) == NULL )
  {
    ERR_FATAL( "SMD Task command buffer not available", 0, 0, 0 );
  }
  else
  {
    link_ptr->cmd = *cmd_ptr;
    q_put( &context->cmd_queue, &link_ptr->link );
    smd_event_send_smd_context( context, SMD_CMD_SIG );
  }
#else
  /* Lock SMD Lite open port list */
  smd_acquire_open_list_lock(context);

  smd_lite_handle_cmd( cmd_ptr );

  /* Unlock SMD Lite open port list */
  smd_release_open_list_lock(context);
#endif
} /* smd_cmd_context */

/*===========================================================================
FUNCTION      smd_set_stream_closed

DESCRIPTION   This routine forces to transition all the open stream
              state to SMD_SS_CLOSED immediately.

ARGUMENTS     context - SMD task context object

RETURN VALUE  None

SIDE EFFECTS  provided for quick clean up upon errors.
===========================================================================*/
void smd_set_stream_closed( smd_context_type * context )
{
  smd_info_type        *info;
  smd_stream_info_type *sinfo;

  for( info = context->open_list; info != NULL; info = info->next )
  {
    if( info->protocol != SMD_STREAMING_BUFFER )
    {
      continue;
    }

    sinfo = &info->info.stream;

    if( sinfo->tx_shared_info_ptr )
    {
      sinfo->tx_shared_info_ptr->stream_state = SMD_SS_CLOSED;
    }
  }
} /* smd_set_stream_closed */

/*===========================================================================
FUNCTION      smd_main

DESCRIPTION   Shared Memory Driver's main program.

              When a share memory port is opened, it synchronizes the opening
              with the other processor and is then responsible for sending
              and receiving data and control signals.

ARGUMENTS     context - SMD task context object

RETURN VALUE  None
===========================================================================*/
void smd_main( smd_context_type * context )
{
  /* static char test = 0; */

  smd_info_type *info      = NULL;
  smd_info_type *next_info = NULL;

#ifdef FEATURE_SMD
  smd_handle_cmd(context);
#endif

#ifndef FEATURE_SMD
  /* Lock SMD Lite open port list */
  smd_acquire_open_list_lock(context);
#endif

  for( info = context->open_list; info != NULL; info = next_info )
  {
    /*--------------------------------------------------------------------
     * Check update first because it may change the behavior of the read
     * and write operations below
     * Must be careful going down the lists here because sometimes we
     * remove the current item from the list as we go
     *-------------------------------------------------------------------*/
    next_info = info->next;

    info->handle_event(info);
  }
  
  if (context->process_loopback)
  {
#ifdef SMD_NWAY_LOOP_SERVER
    smd_loopback_server_update();
#endif

#ifdef SMD_NWAY_LOOP_CLIENT
    smd_loopback_client_update();
#endif
  }

#ifndef FEATURE_SMD
  /* Unlock SMD Lite open port list */
  smd_release_open_list_lock(context);
#endif

} /* smd_main */

/*===========================================================================
FUNCTION      smd_string_copy

DESCRIPTION   Copies the source string into the destination buffer until 
              size is reached, or until a '\0' is encountered.  If valid,
              the destination string will always be NULL deliminated.
              
PARAMETERS    dst - the destination string, contents will be updated.

              src - the source string.

              size - the maximum copy size (size of dst).

RETURN VALUE  The destination string pointer, dst.
===========================================================================*/
char *smd_string_copy(char *dst, const char *src, uint32 size)
{
  uint32 sz;

  if( !size || !dst || !src )
  {
    return NULL;
  }

  sz = strlen(src) + 1;

  if(sz > size)
  {
    SMD_MEMCPY_FROM_TO_SMEM(dst, src, size-1);
    dst[size-1] = '\0';
  }
  else
  {
    SMD_MEMCPY_FROM_TO_SMEM(dst, src, sz);
  }

  return dst;
}