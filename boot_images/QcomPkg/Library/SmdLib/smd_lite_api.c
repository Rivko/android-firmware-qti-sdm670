/**
 * @file smd_lite_api.c
 *
 * Implementation for the lightweight shared memory driver API, including the
 * task/state machine.
 */

/*==============================================================================
     Copyright (c) 2010-2015 Qualcomm Technologies, Inc. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
07/15/14   bm      Invoke SMDL_EVENT_OPEN to client in stream_open callback
                   instead of DTR.
04/23/13   sm      + Rename string copy function to smd_string_copy. 
                   + KW warning fixes.
03/07/13   bm      KW warning fixes.
02/14/13   bm      + Clear out RX read and TX write indexes in RESET function 
                   + Add smd_is_port_open API.
05/11/12   bm      Warning cleanup.
03/22/12   bm      Use new macros to access SMD channel header struct.
01/18/12   bm      Adding SMDL API to support RPM: smdl_get_buffered_msgs(),
                   smdl_free_buffered_msgs() and initialization of RPM memory
                   in smdl_init()
11/30/11   bt      Reworked mutexes for locking in SMDL interrupt context, 
                   mainly to accomodate requirements on RIVA.
11/09/11   rs      Deassert previous DTR as part of the stream reset function
04/10/12   vm      Fixed l4/SAL warnings
07/25/11   rs      Added support for SSR Phase 2
07/26/11   tl      Added SMD Lite IST priority
06/28/11   tl      Protect against low-memory conditions from smd_cs_create()
05/18/11   tl      Validate FIFO size, edge, and port name in smdl_open()
05/04/11   bm      Added support for DSPS loopback
05/02/11   tl      Added SMDL_EVENT_CLOSE_COMPLETE and
                   SMDL_EVENT_REMOTE_CLOSE.
04/18/11   tl      Clarified documentation on SMDL_EVENT_OPEN
04/08/11   tl      Unlock SMD Lite mutexes before calling client callbacks
04/05/11   tl      Include smd.h to get SMD data types
04/01/11   bm      Modified smdl_open() to be compatible with new
                   definition of smdi_alloc_channel_info() which 
                   now take smd_xfrflow_type as an argument as well.
03/25/11   bt      Added CD and RI signal callback functions, set them on open.
03/23/11   tl      Fixed signaling when receiving very large packets
03/10/11   bt      Compiler warning resolved for packet_len in smdl_read.
02/22/11   tl      Set transfer mode to streaming when opening in streaming mode
02/16/11   rs      Set the default transfer mode to PACKET when allocating 
                   SMD Lite ports 
02/14/11   tl      Added support for reading long packets
01/25/11   bt      Added RS-232 related signals, events, and functions.
01/24/11   tl      Made smdl_read_packet_header() available for vector read/
                   write
01/24/11   bt      Remove stream_xfr_finish api, as it is no longer needed.
01/12/11   bt      Implemented streaming mode for smdl_open, _write, _read, 
                   _rx_peek, _tx_peek, and smd_lite_process_read().
01/04/11   bt      Fix bug so that SMD Lite always leaves 4 buffer bytes empty.
10/12/10   tl      Set and clear mask_recv_intr where appropriate, letting the
                   receiver know whether interrupts will be ignored
10/07/10   tl      Return 0 from smdl_tx_peek() when port is not open
07/21/10   tl      Added SMD Lite API for SMD priority.
07/12/10   hwu     Merged in the smd fifo index checking.  
04/30/10   rs      SMD lite v2 port to ARM7: 
                     + Added target initialization
04/21/10   tl      Initial release of SMD Lite v2
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "smd_os.h"

#include "smd_lite.h"
#include "smd_internal.h"
#include "smd_os_common.h"
#include "smd_packet.h"
#include "smd_main.h"
#include "smd_target.h"
#include "smdl_profile.h"

#include "smdl_debug.h"
#include "smdl_echo.h" /* for smdl_open_loopback */

/*===========================================================================
                    LOCAL MACRO DECLARATIONS
===========================================================================*/

/**
 * The maximum packet size that can be read from the FIFO.
 *
 * This is the FIFO size minus 4, since the sending SMD will always leave four
 * bytes unused.
 */
#define SMDL_MAX_RX(port) (port->fifo_sz - 4)

#define SMDL_READ_AVAILABLE(port, pkt_len, bytes_avail) \
          (((bytes_avail) + (port)->mode.lite.continue_read >= (pkt_len)) || \
           ((bytes_avail) >= SMDL_MAX_RX(port)))

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Reads the SMD packet header from the FIFO and updates the read_index. If a
 * full packet header could be read, and the full packet is available in the
 * FIFO (or if the packet is too big to fit in the FIFO), returns the number
 * of bytes in the packet payload.
 *
 * This function must be called with the channel's receive mutex held.
 *
 * @param[in]    si    Pointer to the channel stream info structure
 *
 * @return  If a full packet header was read from the shared memory FIFO, the
 *          number of bytes in the packet. If no packet is available, or the
 *          full packet header was not available, 0.
 */
uint32 smdl_read_packet_header( smd_stream_info_type *si )
{
  uint32 read_index;
  uint32 write_index;
  uint32 byte_count;

  read_index = SMD_READ_CH_HDR( si, SMD_RX_HDR, read_index );
  SMD_VALIDATE_INDEX(read_index, si->fifo_sz);

  write_index = SMD_READ_CH_HDR( si, SMD_RX_HDR, write_index );
  SMD_VALIDATE_INDEX(write_index, si->fifo_sz);

  if( write_index != read_index )
  {
    /* Try to read a packet header */
    if(write_index >= read_index)
    {
      byte_count = write_index - read_index;
    }
    else
    {
      /* FIFO wrapped */
      byte_count = si->fifo_sz + write_index - read_index;
    }

    ASSERT(byte_count > 0);
    ASSERT(byte_count < si->fifo_sz);

    /* Skip bytes left over at the end of a packet we had to truncate */
    if(si->mode.lite.skip)
    {
      if(si->mode.lite.skip >= byte_count)
      {
        SMDL_DEBUG("smdl_read_packet_header(%p): "
            "Skipping %d bytes, %d bytes remain to skip",
            si, byte_count, si->mode.lite.skip - byte_count);

        read_index = write_index;
        si->mode.lite.skip -= byte_count;
        byte_count = 0;

        SMD_WRITE_CH_HDR( si, SMD_RX_HDR, read_index, read_index );
        smdi_event_send_transmit( si );
      }
      else
      {
        SMDL_DEBUG("smdl_read_packet_header(%p): "
            "Skipping %d bytes, %d bytes remain to read",
            si, si->mode.lite.skip, byte_count - si->mode.lite.skip);

        read_index += si->mode.lite.skip;
        if(read_index >= si->fifo_sz)
        {
          read_index -= si->fifo_sz;
        }
        byte_count -= si->mode.lite.skip;
        si->mode.lite.skip = 0;
      }
    }

    if( byte_count >
        SMD_PACKET_HEADER_BYTE_SIZE - si->mode.lite.packet_header_len )
    {
      byte_count =
        SMD_PACKET_HEADER_BYTE_SIZE - si->mode.lite.packet_header_len;
    }

    if(byte_count)
    {
      read_index = smd_memcpy_from_sm( (uint8 *)(si->mode.lite.packet_header) +
                                                si->mode.lite.packet_header_len,
                                       (uint8 *)si->rx_shared_fifo,
                                       byte_count,
                                       read_index, si->fifo_sz );

      SMD_WRITE_CH_HDR( si, SMD_RX_HDR, read_index, read_index );
      smdi_event_send_transmit( si );
    }

    si->mode.lite.packet_header_len += byte_count;
  }

  if(si->mode.lite.packet_header_len == SMD_PACKET_HEADER_BYTE_SIZE)
  {
    uint32 packet_len = SMD_GET_PACKET_SIZE(si->mode.lite.packet_header);

    /* Is the full packet available in the FIFO? */
    if(write_index >= read_index)
    {
      byte_count = write_index - read_index;
    }
    else
    {
      /* FIFO wrapped */
      byte_count = si->fifo_sz + write_index - read_index;
    }

    SMDL_DEBUG("smdl_read_packet_header(%p): "
        "Packet header: %d byte payload, %d bytes available",
        si, packet_len, byte_count);

    /* Take into account bytes we've already read when making the decision
     * whether we have enough data to signal the client */
    if(byte_count + si->mode.lite.continue_read >= packet_len)
    {
      /* The full packet is available on the FIFO. */
      return packet_len;
    }
    else if(byte_count == SMDL_MAX_RX(si))
    {
      /* The packet is too big to fit into the FIFO all at once. smdl_read()
       * will truncate it, or smdl_readv() will read as much as is possible. */
      return packet_len;
    }
    else
    {
      /* The full packet is not yet available, or the FIFO is not yet full. */
      return 0;
    }
  }
  else
  {
    return 0;
  }
}

/**
 * This function initializes the memcpy specific part of the stream info
 * structure and the shared memory structure.
 *
 * @param[in]    si    Pointer to the channel stream info structure
 *
 * @return       None
 */
static void smd_lite_init_stream( smd_stream_info_type *si )
{
  SMD_WRITE_CH_HDR( si, SMD_TX_HDR, write_index, 0 );
  SMD_WRITE_CH_HDR( si, SMD_RX_HDR, read_index, 0 );
}

/**
 * When data is received on the FIFO, this function checks whether:
 *   ->Packet mode: a full packet is available to be read.  OR
 *   ->Streaming mode: any data is available to be read ( > 0 bytes).
 * If the condition for the current mode is met, it calls the client 
 * notification callback.
 *
 * @param[in]    si    Pointer to the channel stream info structure
 *
 * @return       None
 *
 * @sideeffects  If data was read out of shared memory, an transmit interrupt
 *               is sent to notify the other processor that it may be able to
 *               send more data.
 */
static void smd_lite_process_read( smd_stream_info_type *si )
{
  uint32 bytes_avail;
  uint32 pkt_len;
  smdl_callback_t notify;
  void *data;

  if( (si == NULL) || (si->mode.lite.notify == NULL))
  {
    return;
  }

  notify = si->mode.lite.notify;
  data = si->mode.lite.cb_data;

  smdl_rx_peek_ext(si, &pkt_len, &bytes_avail);

  if( bytes_avail != 0 )
  {
    /* Note: To be compatible with the current SMDL, signal SMDL_EVENT_READ 
     * when the FIFO is full, regardless if this is a partial packet or not.
     * This must stay consistent with the check in smdl_rx_peek() and 
     * smdl_read(). */
    if (SMDL_READ_AVAILABLE(si, pkt_len, bytes_avail))
    {
      /* Full packet in the FIFO or FIFO is full. */
      notify(si, SMDL_EVENT_READ, data);
    }
    else
    {
      /* Partial packet in the FIFO. Notify the client. */
      notify(si, SMDL_EVENT_READ_PARTIAL, data);
    }
  }
}

/**
 * When data is read from the outgoing FIFO, checks whether more data is
 * available to be written.
 *
 * Clients write directly to the FIFO from their task context. The only
 * thing we need to do here is notify the client that they can write, if it
 * happens that they've set the "notify-on-write" bit.
 *
 * @param[in]    si    Pointer to the channel stream info structure
 *
 * @return       None
 */
static void smd_lite_process_write( smd_stream_info_type *si )
{
  smdl_callback_t notify = NULL;
  void *data = NULL;

  smd_acquire_cs_lock(si->mode.lite.tx_mutex);
  if( si->mode.lite.notify_on_write && si->mode.lite.notify != NULL )
  {
    notify = si->mode.lite.notify;
    data = si->mode.lite.cb_data;
  }
  smd_release_cs_lock(si->mode.lite.tx_mutex);

  if(notify != NULL)
  {
    notify(si, SMDL_EVENT_WRITE, data);
  }
}

/**
 * Handles flushing of the SMD channel.
 *
 * This function does nothing for SMD Lite.
 *
 * @param[in]    si    Pointer to the channel stream info structure
 *
 * @return       None
 */
static void smd_lite_flush_tx( smd_stream_info_type *si )
{
  UNREFERENCED_PARAMETER(si);
}

/**
 * Handles a DTR update.
 *
 * When DTR is asserted, the connection is now open. Note that
 * smd_process_update() updates prev_dtr to the _current_ DTR _before_ calling
 * this function.
 *
 * @param[in]    si    Pointer to the channel stream info structure
 *
 * @return       None
 */
static void smd_lite_dtr( smd_stream_info_type *si )
{
  /* The SMDL_EVENT_OPEN would now be called in stream_open
     callback instead of DTR. See smd_lite_stream_open */
}

/**
 * Handles a drop event.
 *
 * This function does nothing for SMD Lite.
 *
 * @param[in]    si    Pointer to the channel stream info structure
 *
 * @return       None
 */
static void smd_lite_drop( smd_stream_info_type *si )
{
  UNREFERENCED_PARAMETER(si);
}

/**
 * Handles an aborted transfer event.
 *
 * This function does nothing for SMD Lite.
 *
 * @param[in]    si    Pointer to the channel stream info structure
 *
 * @return       None
 */
static uint32 smd_lite_abort_tx( smd_stream_info_type *si )
{
  UNREFERENCED_PARAMETER(si);
  return 0;
}

/**
 * Handles an aborted receive event.
 *
 * This function does nothing for SMD Lite.
 *
 * @param[in]    si    Pointer to the channel stream info structure
 *
 * @return       None
 */
static uint32 smd_lite_abort_rx( smd_stream_info_type *si )
{
  UNREFERENCED_PARAMETER(si);
  return 0;
}

/**
* Stream open callback handler for SMD Lite
*
* When SMD receives a open event from the other end of the channel, it will
* call this callback, which will call the client's SMD Lite notification
* callback with event=SMDL_EVENT_OPEN, if one was provided when the
* channel was opened.
*
* @param[in]    data  Pointer to the port structure
*
* @return       None
*
* @sideeffects  Calls the client's notification callback
*/
static void smd_lite_stream_open
(
  smd_stream_info_type *port
)
{
  if (port->mode.lite.notify)
  {
    smdl_callback_t notify = port->mode.lite.notify;

    notify(port, SMDL_EVENT_OPEN, port->mode.lite.cb_data);
  }
}

/**
 * Stream close callback handler for SMD Lite
 *
 * When SMD receives a close event from the other end of the channel, it will
 * call this callback, which will call the client's SMD Lite notification
 * callback with event=SMDL_EVENT_REMOTE_CLOSE, if one was provided when the
 * channel was opened.
 *
 * @param[in]    data  Pointer to the port structure
 *
 * @return       None
 *
 * @sideeffects  Calls the client's notification callback
 */
static void smd_lite_stream_close
(
  smd_stream_info_type *port
)
{
  if(port->mode.lite.notify)
  {
    smdl_callback_t notify = port->mode.lite.notify;

    notify(port, SMDL_EVENT_REMOTE_CLOSE, port->mode.lite.cb_data);
  }
}

/**
 * Stream reset callback handler for SMD Lite
 *
 * When SMD receives a reset event from the other end of the channel or
 * from an external entity like subsystem restart, it will
 * execute this callback, which will invoke the client's SMD 
 * Lite notification callback with 
 * event=SMDL_EVENT_REMOTE_CLOSE, if one was provided when the 
 * channel was opened. It will also deassert prev DTR to allow 
 * SMDL_EVENT_OPEN to be sent to the client once the port 
 * has been re-opened. 
 *
 * @param[in]    data  Pointer to the port structure
 *
 * @return       None
 *
 * @sideeffects  Calls the client's notification callback
 */
static void smd_lite_stream_reset
(
  smd_stream_info_type *port
)
{
  /* Block asynchronous reads, deassert DTR and clear RX FIFO */
  smd_acquire_cs_lock(port->mode.lite.rx_mutex);
  port->prev_dtr = FALSE;
  SMD_WRITE_CH_HDR( port, SMD_RX_HDR, read_index, 0 );
  smd_release_cs_lock(port->mode.lite.rx_mutex );

  /* Block writes, and clear TX FIFO */
  smd_acquire_cs_lock(port->mode.lite.tx_mutex);
  SMD_WRITE_CH_HDR( port, SMD_TX_HDR, write_index, 0 );
  smd_release_cs_lock(port->mode.lite.tx_mutex );

  if(port->mode.lite.notify)
  {
    smdl_callback_t notify = port->mode.lite.notify;

    notify(port, SMDL_EVENT_REMOTE_CLOSE, port->mode.lite.cb_data);
  }
}


/** 
 * Close callback wrapper for SMD Lite
 *
 * When SMD deems the channel to be closed, it will call this callback, which
 * will call the client's SMD Lite notification callback with
 * event=SMDL_EVENT_CLOSE_COMPLETE, if one was provided when the channel was
 * opened. Once the client is notified that the port is closed, the client may
 * immediately reopen the port.
 *
 * @param[in]    data  Pointer to the port structure
 *
 * @return       None
 */
static void smdl_close_cb
(
  void *data
)
{
  smdl_handle_type port = (smdl_handle_type)data;

  smd_cs_destroy(port->mode.lite.rx_mutex);
  smd_cs_destroy(port->mode.lite.tx_mutex);

  if(port->mode.lite.notify)
  {
    smdl_callback_t notify = port->mode.lite.notify;

    notify(port, SMDL_EVENT_CLOSE_COMPLETE, port->mode.lite.cb_data);
  }
}

/**
 * Wrapper for local extended DTR/DSR Callback.
 * 
 * Whenever the host detects that the remote DTR/DSR signal has changed, it 
 * will run smd_lite_dtr as well as this callback wrapper.  The argument 
 * \c data is set in smdl_open() as the local port structure.
 * 
 * @param[in]     data  Pointer to the port structure.
 * 
 * @return        None
 */
static void smdl_dtr_cb
(
  void *data
)
{
  smdl_handle_type port = (smdl_handle_type)data;

  smdl_callback_t notify = NULL;
  void *cbdata;
  
  smd_acquire_cs_lock(port->mode.lite.rx_mutex);
  notify = port->mode.lite.notify;
  cbdata = port->mode.lite.cb_data;
  smd_release_cs_lock(port->mode.lite.rx_mutex);

  if(notify != NULL)
  {
    notify(port, SMDL_EVENT_DSR, cbdata);
  }
}

/**
 * Wrapper for local extended CTS/RTS Callback.
 * 
 * Whenever the host detects that the remote CTS/RTS signal has changed, it 
 * will run this callback wrapper.  The argument \c data is set in smdl_open() 
 * as the local port structure.
 * 
 * @param[in]     data  Pointer to the port structure.
 * 
 * @return        None
 */
static void smdl_cts_cb
(
  void *data
)
{
  smdl_handle_type port = (smdl_handle_type)data;

  smdl_callback_t notify;
  void *cbdata;

  smd_acquire_cs_lock(port->mode.lite.rx_mutex);
  notify = port->mode.lite.notify;
  cbdata = port->mode.lite.cb_data;
  smd_release_cs_lock(port->mode.lite.rx_mutex);

  if(notify != NULL)
  {
    notify(port, SMDL_EVENT_CTS, cbdata);
  }
}

/**
 * Wrapper for the local CD Callback.
 * 
 * Whenever the host detects that the remote CD signal has changed, it 
 * will run this callback wrapper.  The argument \c data is set in smdl_open() 
 * as the local port structure.
 * 
 * @param[in]     data  Pointer to the port structure.
 * 
 * @return        None
 */
static void smdl_cd_cb
(
  void *data
)
{
  smdl_handle_type port = (smdl_handle_type)data;

  smdl_callback_t notify = NULL;
  void *cb_data;

  smd_acquire_cs_lock(port->mode.lite.rx_mutex);
  notify = port->mode.lite.notify;
  cb_data = port->mode.lite.cb_data;
  smd_release_cs_lock(port->mode.lite.rx_mutex);

  if (notify != NULL)
  {
    notify(port, SMDL_EVENT_CD, cb_data);
  }
}

/**
 * Wrapper for the local RI Callback.
 * 
 * Whenever the host detects that the remote RI signal has changed, it 
 * will run this callback wrapper.  The argument \c data is set in smdl_open() 
 * as the local port structure.
 * 
 * @param[in]     data  Pointer to the port structure.
 * 
 * @return        None
 */
static void smdl_ri_cb
(
  void *data
)
{
  smdl_handle_type port = (smdl_handle_type)data;

  smdl_callback_t notify;
  void *cb_data;

  smd_acquire_cs_lock(port->mode.lite.rx_mutex);
  notify = port->mode.lite.notify;
  cb_data = port->mode.lite.cb_data;
  smd_release_cs_lock(port->mode.lite.rx_mutex);

  if (notify != NULL)
  {
    notify(port, SMDL_EVENT_RI, cb_data);
  }
}

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/** 
 * Initializes the SMD Lite system.
 *
 * This function must be called by the system startup code before any other SMD
 * Lite functions are called.
 *
 * This function initializes local and shared data structures for SMD Lite and
 * registers the necessary interrupt handlers.
 *
 * @param[in]    reserved  Must be 0
 *
 * @return       0 if successful
 * @return       A negative error code if an error occurred.
 *
 * @sideeffects  Registers interrupt handlers, writes to shared memory, and
 *               signals the remote processor.
 */
int32 smdl_init
(
  uint32 reserved
)
{
  UNREFERENCED_PARAMETER(reserved);
#ifndef FEATURE_SMD
  smd_target_init();
  smd_init();
#endif

/* Call the SMDL Loopback Client or Server if Loopback Test flag is enabled */
#ifdef FEATURE_SMDL_LOOPBACK_TEST
#ifdef FEATURE_SMD
  return smdl_profile_init(0);
#else
#ifdef SMD_RIVA_PROC
  smdl_open_loopback("LOOPBACK", SMD_APPS_RIVA, SMD_STANDARD_FIFO);
#endif /* SMD_RIVA_PROC */

#ifdef SMD_Q6FW_PROC
  smdl_open_loopback("LOOPBACK", SMD_MODEM_Q6FW, SMD_STANDARD_FIFO);
#endif /* SMD_Q6FW_PROC */

#ifdef SMD_DSPS_PROC
/* Enable loopback test on DSPS only during bringup because of memory
 * constraints */
  smdl_open_loopback("LOOPBACK", SMD_APPS_DSPS, SMD_STANDARD_FIFO);
#endif /* SMD_DSPS_PROC */
#endif /* FEATURE_SMD */
#endif /* FEATURE_SMDL_LOOPBACK_TEST */

#ifdef SMD_RPM_PROC
//  smdl_open_loopback("BLOOPBACK", SMD_MODEM_RPM, 1024);
#endif /* SMD_Q6FW_PROC */

  return 0;
}

/** 
 * Shuts down the SMD Lite system.
 *
 * This function is provided to terminate the SMD Lite system in test
 * environments. It is not intended to be used on production systems. After
 * this function is called, all handles will be invalidated and clients must
 * call smdl_init() to initialize the system.
 *
 * This function MUST NOT be called if a call to smdl_init() is outstanding in
 * another thread.
 *
 * @param[in]    reserved  Must be 0
 *
 * @return       0 if successful
 * @return       A negative error code if an error occurred.
 */
int32 smdl_term
(
  uint32 reserved
)
{
  UNREFERENCED_PARAMETER(reserved);
  /* Nothing to do when running along side SMD */

  return 0;
}

/** 
 * Opens a port to the specified remote processor.
 *
 * Opens the port named \c name between the processors specified by \c edge.
 * (The current processor must be one of the two processors connected by \c
 * edge.) This function allocates the data structures for the ports in local
 * and shared memory and allocates FIFOs for receive and transmit, each of
 * size \c fsize.
 *
 * Clients may not read from or write to the port until it is opened on both
 * ends. When the other processor opens the port the \c notify callback will 
 * be called with argument #SMDL_EVENT_OPEN. If the other end is already open 
 * when this function is called, the open notification callback will be called 
 * immediately, and may be called before this function returns.
 *
 * If a client wishes to know when the SMD port is fully open, and the other
 * end asserts its DTR signal when opening the port, the client should wait
 * for #SMDL_EVENT_OPEN. If a client wishes to know when the SMD port is fully
 * open, and the other end does not assert its DTR signal when opening the
 * port, the client should wait for #SMDL_EVENT_CTS, taking care that the
 * other end may set and clear its Clear To Send (CTS) signal to enable and
 * disable inbound flow during normal operation. If a client doesn't need to
 * know when the port is open, and merely wishes to transmit data, the client
 * should send data after opening the port. If the port is fully open, SMD
 * Lite will transmit the data immediately. If the port is not fully open, SMD
 * Lite will notify the client when data can be written (when the remote
 * processor asserts its CTS signal) by sending #SMDL_EVENT_WRITE.
 *
 * Port names uniquely identify the port connecting two processors across a
 * single edge. The same name may be used along different edges.
 *
 * @param[in]    name     The name for the port.
 * @param[in]    edge     The edge, identifying both processors, on which this
 *                        channel communicates. The edge must include the
 *                        current processor.
 * @param[in]    flags    A bitfield of flags controlling the behavior of the
 *                        port. See #SMDL_OPEN_FLAGS_NONE,
 *                        #SMDL_OPEN_FLAGS_PRIORITY_DEFAULT,
 *                        #SMDL_OPEN_FLAGS_PRIORITY_LOW,
 *                        #SMDL_OPEN_FLAGS_PRIORITY_HIGH,
 *                        #SMDL_OPEN_FLAGS_PRIORITY_IST,
 *                        #SMDL_OPEN_FLAGS_MODE_PACKET, and
 *                        #SMDL_OPEN_FLAGS_MODE_STREAMING.
 * @param[in]    fsize    The size of the FIFO. This is the physical
 *                        size in memory, which is always four bytes greater 
 *                        than the total capacity of the FIFO. In packet mode, 
 *                        the SMD packet header adds 20 bytes per packet, 
 *                        further reducing the capacity. The FIFO size must be 
 *                        a multiple of 32 and must not be less than 
 *                        SMD_MIN_FIFO or greater than SMD_MAX_FIFO. If this 
 *                        parameter is 0, the default size (SMD_STANDARD_FIFO) 
 *                        will be used.
 * @param[in]    notify   The callback called when SMD needs to notify the
 *                        client of a remote event on the SMD port.
 * @param[in]    cb_data  The data passed to the notify function.
 *
 * @return       An opaque SMD handle for the opened port, or NULL if an
 *               error occured while opening the port.
 *
 * @sideeffects  Allocates the local and shared data structures for the SMD
 *               port. Signals the remote processor.
 */
smdl_handle_type smdl_open
(
  const char       *name,
  smd_channel_type  edge,
  uint32            flags,
  uint32            fsize,
  smdl_callback_t   notify,
  void             *cb_data
)
{
  smd_cmd_type          cmd;
  smd_info_type        *info;
  smd_stream_info_type *sinfo;
  uint32                port_id;
  smd_priority_type     priority = SMD_PRIORITY_DEFAULT;

  if((name == NULL) || (name[0] == '\0'))
  {
    return NULL;
  }

  if((uint32)edge >= (uint32)SMD_CHANNEL_TYPE_LAST)
  {
    return NULL;
  }

  /* Validate size, if specified: multiple of 32, and within the min and max
   * sizes */
  if((fsize != 0) &&
      ( (fsize & 0x1F) != 0 ||
        (fsize < SMD_MIN_FIFO) ||
        (fsize > SMD_MAX_FIFO) )
      )
  {
    return NULL;
  }

  /* allocate the channel info */
  
  if ( (flags & SMDL_OPEN_FLAGS_MODE_MASK) == SMDL_OPEN_FLAGS_MODE_STREAMING )
  {
    /* When the port is opened in streaming mode, set the transfer flow type
     * to streaming */
    port_id = smdi_alloc_channel_info(name, 
                                      edge,
                                      SMD_STREAMING_BUFFER,
                                      SMD_STREAMING_TYPE);
  }
  else
  {
    /* When the port is opened in packet mode, set the transfer flow type to
     * packet */
    port_id = smdi_alloc_channel_info(name,
                                      edge,
                                      SMD_STREAMING_BUFFER,
                                      SMD_PKTXFR_TYPE);
  }

  if (port_id == SMD_PORT_INVALID)
    return NULL;

  info = smdi_alloc_info(port_id);

  if(info == NULL)
    return NULL;
 
  /* Initialize the fields of the smd info structure */
  smd_string_copy(info->port_name, name, SMD_CHANNEL_NAME_SIZE_MAX);
  info->protocol = SMD_STREAMING_BUFFER;
  info->channel_type = edge;

  /* Initialize "simple" fields of stream info structure */   
  sinfo = &info->info.stream;
  sinfo->port_id = port_id;
  sinfo->channel_type = edge;

  sinfo->tx_flow_control_method = SMD_CTSRFR_FCTL;
  sinfo->rx_flow_control_method = SMD_CTSRFR_FCTL;

  sinfo->tx_fctl_enabled = TRUE;
  sinfo->rx_fctl_enabled = TRUE;

  sinfo->dataxfr_mode = SMD_MEMCPY_MODE;
  if(fsize == 0)
  {
    sinfo->fifo_sz = SMD_STANDARD_FIFO;
  }
  else
  {
    sinfo->fifo_sz = fsize;
  }
  SMDL_DEBUG("FIFO size is %d", sinfo->fifo_sz, 0, 0);

  smdi_allocate_stream_channel( port_id, sinfo );

  /* Initialize mutexes */
  sinfo->mode.lite.rx_mutex = smd_cs_create();
  sinfo->mode.lite.tx_mutex = smd_cs_create();
  sinfo->mode.lite.notify = notify;
  sinfo->mode.lite.cb_data = cb_data;
  sinfo->mode.lite.notify_on_write = FALSE;

  if((sinfo->mode.lite.rx_mutex == NULL) || (sinfo->mode.lite.tx_mutex == NULL))
  {
    if(sinfo->mode.lite.rx_mutex != NULL)
    {
      smd_cs_destroy(sinfo->mode.lite.rx_mutex);
    }

    if(sinfo->mode.lite.tx_mutex != NULL)
    {
      smd_cs_destroy(sinfo->mode.lite.tx_mutex);
    }

    smdi_free_info(info);

    return NULL;
  }
  
  /* Initialize signaling callbacks. */
  sinfo->dtr_callback_ext_fn = smdl_dtr_cb;
  sinfo->dtr_callback_ext_data = sinfo;
  sinfo->cts_callback_ext_fn = smdl_cts_cb;
  sinfo->cts_callback_ext_data = sinfo;
  sinfo->cd_callback_ext_fn = smdl_cd_cb;
  sinfo->cd_callback_ext_data = sinfo;
  sinfo->ri_callback_ext_fn = smdl_ri_cb;
  sinfo->ri_callback_ext_data = sinfo;

  /* Assert DSR */
  SMD_WRITE_IF_SIG( sinfo, SMD_TX_HDR, dsr_sig, TRUE );
  /* Assert carrier detect */
  SMD_WRITE_IF_SIG( sinfo, SMD_TX_HDR, cd_sig, TRUE );
  /* Enable inbound flow */
  SMD_WRITE_IF_SIG( sinfo, SMD_TX_HDR, cts_sig, TRUE );

  switch(flags & SMDL_OPEN_FLAGS_PRIORITY_MASK)
  {
    case SMDL_OPEN_FLAGS_PRIORITY_DEFAULT:
      priority = SMD_PRIORITY_DEFAULT;
      break;
    case SMDL_OPEN_FLAGS_PRIORITY_LOW:
      priority = SMD_PRIORITY_LOW;
      break;
    case SMDL_OPEN_FLAGS_PRIORITY_HIGH:
      priority = SMD_PRIORITY_HIGH;
      break;
    case SMDL_OPEN_FLAGS_PRIORITY_IST:
      priority = SMD_PRIORITY_IST;
      break;
  }
  if ( (flags & SMDL_OPEN_FLAGS_MODE_MASK) == 
       SMDL_OPEN_FLAGS_MODE_STREAMING )
  {
    sinfo->mode.lite.transfer_mode = SMDL_MODE_STREAMING;
  }
  else
  {
    sinfo->mode.lite.transfer_mode = SMDL_MODE_PACKET;
  }

  smd_assign_context(info, priority);

  /* Send command to smd task */
  cmd.cmd = SMD_CMD_OPEN_SMDLITE;
  cmd.data.open_memcpy.port_id = port_id;

  /* This is a special loopback port on the apps-modem edge. Real n-way
   * loopback support is not yet widely available. */
  if((strcmp(name, "LOOPBACK") == 0) && (edge == SMD_APPS_MODEM))
  {
    cmd.data.open_memcpy.is_loopback = TRUE;
  }
  else
  {
    cmd.data.open_memcpy.is_loopback = FALSE;
  }

  smd_cmd(&cmd);

  return &info->info.stream;
}

/** 
 * Closes the port to the remote processor.
 *
 * Closes the port and signals the remote processor to close the port. No
 * more data may be transferred over the port after it is closed.
 *
 * @param[in]    port  The port to close
 *
 * @return       0 if the port is closed
 * @return       A negative error code if an error occurred.
 *
 * @sideeffects  Writes to shared memory and signals the remote processor.
 */
int32 smdl_close
(
  smdl_handle_type port
)
{
  smd_cmd_type cmd;

  if(port == NULL)
  {
    return -1;
  }

  cmd.cmd = SMD_CMD_CLOSE;
  cmd.data.close.port_id = port->port_id;
  cmd.data.close.close_cb = smdl_close_cb;
  cmd.data.close.cb_data = port;
  cmd.data.close.flush = FALSE;

  smd_cmd( &cmd );

  return 0;
}

/** 
 * Peek the FIFO to see if there is data or packet pending.
 *
 * Clients may use this function to quickly determine whether to read data from
 * the port, and to allocate the correct buffer size.
 * 
 * Note, if the port is not sufficiently opened to perform a read, the values 
 * pointed by pkt_len and bytes_avail will both be set to 0.  Errors, however, 
 * will not modify these values.
 *
 * @param[in]  port         The port to check for data.
 * @param[out] pkt_len      Pointer to the packet length.
 * @param[out] bytes_avail  Packet Mode: Pointer to the number of bytes of the next 
 *                                       packet that is available in the FIFO.
 *                          Stream Mode: Pointer to the number of bytes available to 
 *                                       be read from the FIFO. 
 *
 * @return       Return 0 if success. Otherwise, return a negative error code. 
 *
 * @sideeffects  None
 */
int32 smdl_rx_peek_ext
(
  smdl_handle_type port,
  uint32          *pkt_len,
  uint32          *bytes_avail
)
{
  uint32 byte_count = 0;
  uint32 packet_length = 0;
  uint32 local_state, remote_state;

  if(port == NULL)
  {
    return SMD_STATUS_INVALID_PARAM;
  }

  if((pkt_len == NULL) && (bytes_avail == NULL))
  {
    return SMD_STATUS_INVALID_PARAM;
  }

  smd_acquire_cs_lock(port->mode.lite.rx_mutex);

  local_state = SMD_READ_CH_HDR( port, SMD_TX_HDR, stream_state );
  remote_state = SMD_READ_CH_HDR( port, SMD_RX_HDR, stream_state );
  
  if ( remote_state == SMD_SS_OPENED && (local_state == SMD_SS_OPENED || 
                                         local_state == SMD_SS_OPENING) )
  {
    if (port->mode.lite.transfer_mode == SMDL_MODE_PACKET)
    {
      /* If in Packet mode, read bytes available */
      byte_count = smdl_read_packet_header(port);

      /* Get the packet size if the header is in the FIFO */
      if(byte_count != 0)
      {
        packet_length = SMD_GET_PACKET_SIZE(port->mode.lite.packet_header);
      }
    }
    else /* port->mode.lite.transfer_mode == SMDL_MODE_STREAMING */
    {
      uint32 read_index;
      uint32 write_index;

      /* If in Streaming mode, calculate size from read/write indices. */
      read_index = SMD_READ_CH_HDR( port, SMD_RX_HDR, read_index );
      write_index = SMD_READ_CH_HDR( port, SMD_RX_HDR, write_index );

      SMD_VALIDATE_INDEX(read_index, port->fifo_sz);
      SMD_VALIDATE_INDEX(write_index, port->fifo_sz);

      /* Calculate the number of bytes available to read from the FIFO. */
      byte_count = write_index - read_index;
      if( write_index < read_index )
      {
        /* FIFO wrapped.  Correct by adding full FIFO size. */
        byte_count += port->fifo_sz;
      }
    }
  }

  /* return packet length if client wants to know */
  if(pkt_len)
  {
    *pkt_len = packet_length;
  }

  /* return bytes available if client wants to know */
  if(bytes_avail)
  {
    *bytes_avail = byte_count;
  }

  smd_release_cs_lock(port->mode.lite.rx_mutex);

  return SMD_STATUS_SUCCESS;
}

/** 
 * Returns the number of bytes available to be read from the port.
 *
 * Clients may use this function to quickly determine whether to read data from
 * the port, and to allocate the correct buffer size.
 *
 * @param[in]  port  The port to check for data.
 *
 * @return       Packet mode: The number of bytes to be read from the next 
 *               packet in the queue, or 0 if no packets are available.
 *               Streaming mode: The number of bytes available to be read from 
 *               this port.  (Determined by how many bytes have been written 
 *               so far and not yet read.)
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  None
 */
int32 smdl_rx_peek
(
  smdl_handle_type port
)
{
  uint32 byte_count;

  if(port == NULL)
  {
    return -1;
  }

  smd_acquire_cs_lock(port->mode.lite.rx_mutex);
  
  if (port->mode.lite.transfer_mode == SMDL_MODE_PACKET)
  {
    /* If in Packet mode, read header to get size. */
    byte_count = smdl_read_packet_header(port);
  }
  else /* port->mode.lite.transfer_mode == SMDL_MODE_STREAMING */
  {
    uint32 read_index;
    uint32 write_index;

    /* If in Streaming mode, calculate size from read/write indices. */
    read_index = SMD_READ_CH_HDR( port, SMD_RX_HDR, read_index );
    SMD_VALIDATE_INDEX(read_index, port->fifo_sz);

    write_index = SMD_READ_CH_HDR( port, SMD_RX_HDR, write_index );
    SMD_VALIDATE_INDEX(write_index, port->fifo_sz);
    byte_count = write_index - read_index;
    if( write_index < read_index )
    {
      /* FIFO wrapped */
      byte_count += port->fifo_sz;
    }
  }
  smd_release_cs_lock(port->mode.lite.rx_mutex);

  return byte_count;
}

/** 
 * Returns the number of bytes that can be written to a port.
 *
 * Clients may use this function to quickly determine whether the packet they
 * wish to send will fit in the outgoing buffer.
 *
 * When the port is not fully opened, or the receiver has disabled inbound
 * flow, this function will return 0, as no data can be written to the port.
 *
 * @param[in]  port  The port to check for data.
 *
 * @return       The length, in bytes, of largest amount of data that can be 
 *               written to the port.
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  None
 */
int32 smdl_tx_peek
(
  smdl_handle_type port
)
{
  uint32 read_index;
  uint32 write_index;
  uint32 byte_count;

  if(port == NULL)
  {
    return -1;
  }

  smd_acquire_cs_lock(port->mode.lite.tx_mutex);

  /* Verify that this end of the port is up and running */
  if( SMD_READ_CH_HDR( port, SMD_TX_HDR, stream_state ) != SMD_SS_OPENED )
  {
    /* If the port is not opened, no data can be transmitted on the port. */
    smd_release_cs_lock(port->mode.lite.tx_mutex);
    return 0;
  }

  /* Check flow control on the other end of this channel */
  if( !SMD_READ_IF_SIG( port, SMD_RX_HDR, rts_sig) )
  {
    /* If the receiver has disabled inbound flow, no data can be transmitted
     * on the port. */
    smd_release_cs_lock(port->mode.lite.tx_mutex);
    return 0;
  }

  write_index = SMD_READ_CH_HDR( port, SMD_TX_HDR, write_index );
  SMD_VALIDATE_INDEX(write_index, port->fifo_sz);

  read_index = SMD_READ_CH_HDR( port, SMD_TX_HDR, read_index );
  SMD_VALIDATE_INDEX(read_index, port->fifo_sz);

  /* compute bytes used in buffer */
  byte_count = read_index - write_index - 4;
  if(read_index <= write_index)
  {
    byte_count += port->fifo_sz;
  }

  /* Only account for packet header size if in Packet mode. */
  if (port->mode.lite.transfer_mode == SMDL_MODE_PACKET)
  {
    if( byte_count > SMD_PACKET_HEADER_BYTE_SIZE )
    {
      byte_count -= SMD_PACKET_HEADER_BYTE_SIZE;
    }
    else
    {
      byte_count = 0;
    }
  }

  smd_release_cs_lock(port->mode.lite.tx_mutex);

  return byte_count;
}

/** 
 * Reads data from the SMD port into the provided buffer.
 *
 * Packet mode: This function will read up to \c len bytes of the next packet 
 * in the port into the provided buffer. If the packet is larger than the 
 * buffer, as much data as possible will be copied to the buffer and remainder 
 * will be discarded. Clients concerned about dropping data should call 
 * smdl_rx_peek() to determine the size of the next packet.
 *
 * Streaming mode: This function will read up to \c len bytes from the port 
 * into the provided buffer, limited by the value returned by smdl_rx_peek(). 
 * Any remaining bytes in the port will be left there for future reads.
 *
 * This function does not block; it will return 0 immediately if no data is
 * available.
 *
 * @param[in]   port    The port to read data from
 * @param[in]   len     The number of bytes available in \c buffer
 * @param[out]  buffer  The buffer into which the data from the port should
 *                      be copied.
 * @param[in]   flags   Bitwise-or of flags controlling SMD's behavior. See
 *                      #SMDL_READ_FLAGS_NONE.
 *
 * @return       If data was read: The number of bytes read into \c buffer
 * @return       If no data was available: 0
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  Reads from shared memory and signals the remote processor.
 */
int32 smdl_read
(
  smdl_handle_type port,
  uint32           len,
  void            *buffer,
  uint32           flags
)
{
  uint32 read_index;
  uint32 write_index;
  uint32 byte_count;
  uint32 packet_len = 0;

  UNREFERENCED_PARAMETER(flags);

  if(port == NULL)
  {
    return -1;
  }

  if((len == 0) || (buffer == NULL))
  {
    return -1;
  }

  smd_acquire_cs_lock(port->mode.lite.rx_mutex);
  
  /* If in Packet mode, first read the header to get the packet length. */
  if (port->mode.lite.transfer_mode == SMDL_MODE_PACKET)
  {
    packet_len = smdl_read_packet_header(port);
    if(packet_len == 0)
    {
      /* The whole packet header was not yet read */
      smd_release_cs_lock(port->mode.lite.rx_mutex);
      return 0;
    }
  }

  read_index = SMD_READ_CH_HDR( port, SMD_RX_HDR, read_index );
  SMD_VALIDATE_INDEX(read_index, port->fifo_sz);

  write_index = SMD_READ_CH_HDR( port, SMD_RX_HDR, write_index );
  SMD_VALIDATE_INDEX(write_index, port->fifo_sz);

  byte_count = write_index - read_index;
  if( write_index < read_index )
  {
    /* FIFO wrapped */
    byte_count += port->fifo_sz;
  }
  
  if (port->mode.lite.transfer_mode == SMDL_MODE_PACKET)
  {
    /* Procedure to read in Packet mode. */
    /* If the packet to be read is larger than the FIFO, truncate it and read
     * only that much as is available. */
    if(packet_len > SMDL_MAX_RX(port))
    {
      port->mode.lite.skip = packet_len - SMDL_MAX_RX(port);
      SMDL_DEBUG("smdl_read(): Skipping %d bytes (packet_len=%d, max=%d)",
          port->mode.lite.skip, packet_len, SMDL_MAX_RX(port));
      packet_len = SMDL_MAX_RX(port);
    }

    if(byte_count >= packet_len)
    {
      byte_count = packet_len;
      if(byte_count > len)
      {
        byte_count = len;
      }

      read_index = smd_memcpy_from_sm( buffer,
                                       (uint8 *)port->rx_shared_fifo,
                                       byte_count,
                                       read_index, port->fifo_sz );

      if(packet_len > len)
      {
        read_index += packet_len - len;
        if(read_index >= port->fifo_sz)
        {
          read_index -= port->fifo_sz;
        }
      }
      SMD_WRITE_CH_HDR( port, SMD_RX_HDR, read_index, read_index );
      smdi_event_send_transmit( port );

      port->mode.lite.packet_header_len = 0;
    }
    else
    {
      byte_count = 0;
    }
  }
  else /* port->mode.lite.transfer_mode == SMDL_MODE_STREAMING */
  {
    if (byte_count == 0){
      /* No data is available for a streaming read. */
      smd_release_cs_lock(port->mode.lite.rx_mutex);
      return 0;
    }
    
    /* Procedure to read in Streaming mode. */
    /* Read as data into buffer as possible, leaving additional data in the 
     * SMD FIFO buffer for additional reads.
     */
    if (byte_count >= len)
    {
      byte_count = len;
    }
    read_index = smd_memcpy_from_sm( buffer, (uint8 *)port->rx_shared_fifo, 
                                     byte_count, read_index, port->fifo_sz );
    SMD_WRITE_CH_HDR( port, SMD_RX_HDR, read_index, read_index );
    smdi_event_send_transmit( port );
  }
  

  smd_release_cs_lock(port->mode.lite.rx_mutex);

  SMDL_DEBUG("smdl_read(%p): tx_read=%d tx_write=%d", port,
    SMD_READ_CH_HDR( port, SMD_TX_HDR, read_index ),
    SMD_READ_CH_HDR( port, SMD_TX_HDR, write_index ));
  SMDL_DEBUG("smdl_read(%p): rx_read=%d rx_write=%d", port,
    SMD_READ_CH_HDR( port, SMD_RX_HDR, read_index ),
    SMD_READ_CH_HDR( port, SMD_RX_HDR, write_index ));

  return byte_count;
}

/** 
 * Writes data from the provided buffer to the SMD port.
 *
 * Packet mode: The buffer is treated as a single packet to be sent to the 
 * remote processor. The packet will be sent in its entirety if space is 
 * available in the outgoing FIFO or will be dropped (with an appropriate error 
 * code) if space is not available. Attempting to send packets larger than the 
 * size of the outgoing FIFO will always fail.
 * 
 * Streaming mode: Up to \c len bytes from \c buffer will be written into the 
 * port, limited by the value returned by smdl_tx_peek(). This function will 
 * return the number of successfully written bytes. Any bytes that cannot fit 
 * in the outgoing FIFO will be truncated.
 *
 * Writing 0 bytes to the port will not change shared memory and will not
 * signal the remote processor.
 *
 * Clients interested in determining the maximum packet size that can be
 * written to the port may call smdl_tx_peek().
 *
 * @param[in]  port    The port to write data to
 * @param[in]  len     The number of bytes to write to the port
 * @param[in]  buffer  The buffer to write to the port
 * @param[in]  flags   Bitwise-or of flags controlling SMD's behavior. See
 *                     #SMDL_WRITE_FLAGS_NONE and #SMDL_WRITE_FLAG_NOINT.
 *
 * @return       Return behavior depends on the transfer mode used.
 *               -Packet mode: 
 *                 -If the port has sufficient space to hold the 
 *                  provided packet: The length of the packet written to the 
 *                  port.
 *                 -If the port does not have sufficient space to 
 *                  hold the provided packet: 0.
 *               -Streaming mode: The number of bytes that were successfully 
 *                written to the port. If truncated, will be less than \c len.
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  Writes to shared memory and signals the remote processor.
 */
int32 smdl_write
(
  smdl_handle_type port,
  uint32           len,
  const void      *buffer,
  uint32           flags
)
{
  uint32 read_index;
  uint32 write_index;
  uint32 byte_count;
  uint32 len_written;

  if(port == NULL)
  {
    return -1;
  }

  if(buffer == NULL)
  {
    return -1;
  }

  if(len == 0)
  {
    return 0;
  }

  smd_acquire_cs_lock(port->mode.lite.tx_mutex);

  /* Verify that this end of the port is up and running */
  if( SMD_READ_CH_HDR( port, SMD_TX_HDR, stream_state ) != SMD_SS_OPENED )
  {
    port->mode.lite.notify_on_write = TRUE;
    smd_release_cs_lock(port->mode.lite.tx_mutex);
    SMDL_DEBUG("smdl_write(%p): Stream state %d",
      port, SMD_READ_CH_HDR(port, SMD_TX_HDR, stream_state ), 0);
    return 0;
  }

  /* Check flow control on the other end of this channel */
  if( !SMD_READ_IF_SIG( port, SMD_RX_HDR, rts_sig ) )
  {
    port->mode.lite.notify_on_write = TRUE;
    smd_release_cs_lock(port->mode.lite.tx_mutex);
    SMDL_DEBUG("smdl_write(%p): DSR=%d RTS=%d", port,
        SMD_READ_IF_SIG( port, SMD_RX_HDR,rts_sig ),
        SMD_READ_IF_SIG( port, SMD_RX_HDR,dsr_sig )
        );
    return 0;
  }

  /* Clear the mask_recv_intr flag while testing whether there is enough room
   * in the tx FIFO. This may result in more transmit-event interrupts than
   * strictly necessary but eliminates potential race conditions when space is
   * made available after checking but before setting the flag. */
  SMD_WRITE_IF_SIG(  port, SMD_TX_HDR, mask_recv_intr, 0 );
  SMD_MEMORY_BARRIER();

  write_index = SMD_READ_CH_HDR(port, SMD_TX_HDR, write_index );
  SMD_VALIDATE_INDEX(write_index, port->fifo_sz);

  read_index = SMD_READ_CH_HDR(port, SMD_TX_HDR, read_index );
  SMD_VALIDATE_INDEX(read_index, port->fifo_sz);

  /* compute bytes used in buffer */
  byte_count = read_index - write_index - 4;
  if(read_index <= write_index)
  {
    byte_count += port->fifo_sz;
  }

  if (port->mode.lite.transfer_mode == SMDL_MODE_PACKET)
  {
    uint32 packet_header[SMD_PACKET_HEADER_SIZE];
    
    /* Write procedure for Packet mode. */
    /* Make sure there's room for some data */
    if( byte_count < len + SMD_PACKET_HEADER_BYTE_SIZE )
    {
      /* destination buffer is full - nothing to do */
      port->mode.lite.notify_on_write = TRUE;
      smd_release_cs_lock(port->mode.lite.tx_mutex);
      if(len + SMD_PACKET_HEADER_BYTE_SIZE > SMDL_MAX_RX(port))
      {
        /* Packet is too big to be sent through FIFO. We will never be able to
         * send this packet. Return failure. */
        return -1;
      }
      else
      {
        /* FIFO has data. We may be able to send this packet later, once data 
         * is read from the FIFO. */
        return 0;
      }
    }

    memset(&packet_header, 0, sizeof(packet_header));
    SMD_SET_PACKET_SIZE(packet_header, len);

    write_index = smd_memcpy_to_sm( (uint8 *)port->tx_shared_fifo,
                                    (uint8 *)packet_header,
                                    sizeof(packet_header), write_index,
                                    port->fifo_sz );
    write_index = smd_memcpy_to_sm( (uint8 *)port->tx_shared_fifo, buffer,
                                    len, write_index, port->fifo_sz );

    SMD_WRITE_CH_HDR(port, SMD_TX_HDR, write_index, write_index );

    /* We've successfully written to the channel. Clear the notify-on-write
     * flag. */
    port->mode.lite.notify_on_write = FALSE;
    SMD_WRITE_IF_SIG( port, SMD_TX_HDR, mask_recv_intr, 1 );
    len_written = len;
  }
  else /* port->mode.lite.transfer_mode == SMDL_MODE_STREAMING */
  {
    /* Write procedure for Streaming mode. */
    /* Check room available vs. the size of buffer we wish to write. */ 
    if (byte_count < len)
    {
      /* There isn't enough room for len bytes in the transfer FIFO 
       * buffer, so just transfer as much as is available and set 
       * notify_on_write, so that the client receives an interrupt when more 
       * room becomes available. */
      port->mode.lite.notify_on_write = TRUE;
      write_index = smd_memcpy_to_sm( (uint8 *)port->tx_shared_fifo, buffer, 
                                      byte_count, write_index, port->fifo_sz );
      SMD_WRITE_CH_HDR( port, SMD_TX_HDR, write_index, write_index );
      /* Leave the mask_recv_intr mask cleared, so an interrupt can be received
       * when a read occurs and more room becomes available.
       */
      len_written = byte_count;
    }
    else
    {
      /* We have room to write all len bytes to the transfer FIFO buffer. */
      write_index = smd_memcpy_to_sm( (uint8 *)port->tx_shared_fifo, buffer, 
                                      len, write_index, port->fifo_sz );
      SMD_WRITE_CH_HDR( port, SMD_TX_HDR, write_index, write_index );

      /* We've successfully written everything to the channel. Clear the 
       * notify-on-write flag. */
      port->mode.lite.notify_on_write = FALSE;
      SMD_WRITE_IF_SIG( port, SMD_TX_HDR, mask_recv_intr, 1 );
      len_written = len;
    }
  }
  smd_release_cs_lock(port->mode.lite.tx_mutex);

  if((flags & SMDL_WRITE_FLAG_NOINT) != SMDL_WRITE_FLAG_NOINT)
  {
    smdi_event_send_receive( port );
  }

  SMDL_DEBUG("smdl_write(%p): tx_read=%d tx_write=%d", port,
    SMD_READ_CH_HDR( port, SMD_TX_HDR, read_index ),
    SMD_READ_CH_HDR( port, SMD_TX_HDR, write_index ));
  SMDL_DEBUG("smdl_write(%p): rx_read=%d rx_write=%d", port,
    SMD_READ_CH_HDR( port, SMD_RX_HDR, read_index ),
    SMD_READ_CH_HDR( port, SMD_RX_HDR, write_index ));

  return len_written;
}

/**
 * This routine handles an open command for a SMD Lite stream.
 *
 * This function allocates and initializes a shared memory structure, finishes
 * initializing the stream info structure and sends the stream state machine
 * an OPEN event.
 *
 * @param[in]    port_id   SMD port to open
 *
 * @return       This function will always return SMD_DONE
 *
 * @sideeffects  None
 */
smd_api_return_type smd_lite_open( smd_port_id_type port_id )
{
  smd_stream_info_type             *info;

  info = smdi_get_stream_info( port_id );

  /* Initialize jump table with SMD Lite entry points */
  info->stream_init = smd_lite_init_stream;
  info->stream_read = smd_lite_process_read;
  info->stream_write = smd_lite_process_write;
  info->stream_flush_tx = smd_lite_flush_tx;
  info->stream_dtr = smd_lite_dtr;
  info->stream_drop = smd_lite_drop;
  info->stream_tx_abort = smd_lite_abort_tx;
  info->stream_rx_abort = smd_lite_abort_rx;
  info->stream_open = smd_lite_stream_open;
  info->stream_close = smd_lite_stream_close;
  info->stream_reset = smd_lite_stream_reset;

  smdi_stream_state_machine( info, SMD_EVENT_OPEN );

  return SMD_DONE;
}

/*===========================================================================
  FUNCTION  smd_is_port_open
===========================================================================*/
/**
  Specifies whether the port with the given name and channel_type has already
  been opened by any processor.
  
  This function doesn't differentiate based on which processor(s) opened the 
  port.  It also works whether or not the local host is coincident with the 
  channel_type.

  @param[in] name           Name of the port to look for.
  @param[in] channel_type   Channel type of the port to look for.

  @return
  TRUE if the port has already been opened.
  FALSE if the port has never been opened.
*/
/*=========================================================================*/
boolean smd_is_port_open
(
  const char       *name,
  smd_channel_type  channel_type
)
{
  if (smdi_get_channel_id(name, channel_type) < 0)
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}
