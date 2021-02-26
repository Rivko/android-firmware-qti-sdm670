/*===========================================================================

           Shared Memory Driver Memcpy Mode Xfr Processing Source File


 Copyright (c) 2005-2011 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ---------------------------------------------------------   
03/22/12   bm      Use new macros to access SMD channel header struct.
11/08/11   bm      Integrated CL 1879286, and added proper SSR handling to 
                   clear Memcpy layer buffer pointers at SSR.
07/25/11   rs      Added support for SSR Phase 2   
07/20/11   hwu     Added smd_rx_peek() to check for data left in receive FIFO.
02/11/11   bt      Removed old unused SMD profiling code.
01/24/11   bt      Remove stream_xfr_finish api, as it is no longer needed.
12/22/10   tl      Fixed corner case in flushing data
12/02/10   rs      Removed unused cross process memory access for WM6
11/26/10   rs      Compile for both SMD and standalone SMD lite
10/25/10   tl      Fixed interrupt-suppression bug in smd_memcpy_process_write()
10/04/10   tl      When receiving data, send an interrupt only when necessary
05/21/09   rs      Added SMD index validation based on FIFO size
05/06/10   rs      Check for RTS not set
04/30/10   rs      Clear compiler warnings
04/30/10   rs      SMD lite v2 port to ARM7: 
                     + Added support SMD Lite v2 (per changelist 1134928)
04/21/10   tl      Implemented SMD Lite API
12/17/09   tl      Make sure all ports are fully closed before opening them
11/19/09   sa      warning cleanup, sync-up with symbian
09/29/09   hwu     Need to update the prev_rts if we have already act upon it.
03/24/09   hwu     Removed the GetCurrentPermissions/SetProcPermissions. 
02/16/09   hwu     Correct a typo.
06/23/08   bfc     Fix for variable sized SMD FIFOs.
06/19/08   bfc     Support variable sized SMD FIFOs.
06/10/08   sa      Smd_target.h cleanup. 
04/08/08   sd      Sending wakeup reasons to arm11 from arm9 and Logging these
                   wakeup reasons in arm11 using Celog
02/21/08   bfc     First SMD Block port check in.
01/14/08   bfc     Created a SMD base struct.
10/25/07   hwu     Changed to pass in client callback data for client
                   registered callback functions.
10/18/07   bfc     Added hooks to support abort ioctls.
10/15/07   jlk     added smd profile hooks
10/08/07   hwu     Changed the order to include smd_os.h and smd.h
                   because smd_channel_type moved to smd.h
09/11/07   bfc     Sending an event when we had a pending ioctl was causing
                   an endless loop.
07/31/07   hwu     Changed the reset the tx/rx buffer when the DTR is
                   de-asserted. Also added more logging.
07/18/07   hwu     Moved err.h and assert.h to smd_target.h.
05/16/07   hwu     Compiled out GetCurrentPermissions/SetProcPermissions for 
                   WINCE boot and kernel.
04/26/07   ptm     Use new channel data structure.
04/17/07   ih      Modified to support memcpy across process boundary on WinMob
03/20/07   taw     Include smd_target.h instead of customer.h and target.h.
01/09/07   bfc     Added SMD using DM
01/04/06   bfc     Broke SMD MEMCPY and DMOV out into components.
12/05/06   bfc     Fixed a corner case in flush
11/29/06   taw     Use macros to access shared memory data.
11/13/06   hwu     combined smd_update_sig, smd_receive_sig and
                   smd_transmit_sig into smd_event_sig.
10/13/06   ptm     Remove include of cache_mmu.h.
10/03/06   bfc     Added capability for the rcv operation to only return full
                   buffers to the buffer layer.
10/02/06   ptm     Changed smd_memcpy_process_read/write to always send
                   interrupt to other processor if read/write pointer is
                   updated.
09/28/06   taw     Remove inclusion of sio.h.                   
09/27/06   bfc     Added new API to drop pending transmit buffers and fixed
                   a bug in smd_memcpy_read_skip.
09/05/06   ptm     Remove references to REX.
09/05/06   ptm     Modify smd_memcpy_process_write to use max_queued_data.
08/28/06   ptm     Update tx_buf_ptr for partial buffer copies.
08/22/06   ptm     Merge WinMobile changes to main line.
06/16/06   ptm     Change smd_memcpy_process_read to retry reading after IOCTL.
05/05/06   ptm     Make interface signals synchronous and add pending IOCTL
                   counter.
04/28/06   ptm     Make interface signals asynchronous.
04/28/06   ptm     Added flow control callback.
04/04/06   ptm     Add '#' to 'define' for SMD_MEMCPY_LOG_EVENT.
01/03/06   ptm     Removed packet handling code.
09/29/05   TMR     Changed smd_memcpy_process_write() to try to get a buffer
                   first if none is currently being processed.
05/18/05   ptm     General simplification of buffer read and write.
02/07/05   adm     Initial version
===========================================================================*/


/*===========================================================================
                              INCLUDE FILES 
===========================================================================*/

#include "smd_os.h"

#include "smem_log.h"

#include "smd.h"
#include "smd_internal.h"
#include "smd_memcpy.h"

/*===========================================================================
                              GLOBAL DATA DECLARATIONS 
===========================================================================*/
/* Turn logging on/off here using define/undef */
#undef SMD_MEMCPY_FILTER_EVENTS  
#undef SMD_MEMCPY_LOG_EVENTS

#ifdef SMD_MEMCPY_FILTER_EVENTS
#define SMD_MEMCPY_LOG_EVENT(pid,id,d1,d2,d3)   \
  do { \
    if( pid == SMD_MEMCPY_FILTER_EVENTS ) \
    { \
      SMEM_LOG_EVENT(id,d1,d2,d3); \
    } \
  } while(0)
#elif defined(SMD_MEMCPY_LOG_EVENTS)
#define SMD_MEMCPY_LOG_EVENT(pid,id,d1,d2,d3)    SMEM_LOG_EVENT(id,d1,d2,d3)
#else
#define SMD_MEMCPY_LOG_EVENT(pid,id,d1,d2,d3)   do{ } while(0)
#endif /* SMD_MEMCPY_LOG_EVENTS */

/*===========================================================================
                    LOCAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smd_memcpy_from_sm

DESCRIPTION   This function copys data from a shared memory buffer to a
              destination buffer. It uses memcpy for performance and handles
              wrap around for the shared memory buffer.

ARGUMENTS     dst - pointer to destination buffer

              src - pointer to shared memory buffer

              count - number of bytes to copy

              read_index - start index in source buffer

              src_size - size of source buffer

RETURN VALUE  The next read index for the shared memory buffer.
===========================================================================*/
uint32 smd_memcpy_from_sm
(
  uint8 *dst,
  const uint8 *src,
  uint32 count,
  uint32 read_index,
  uint32 src_size
)
{
  uint32 bytes;

  if( read_index + count > src_size ) {
    /* must split read of count - copy bytes at end of src first */
    bytes = src_size - read_index;
    SMD_MEMCPY_FROM_SMEM( dst, src + read_index, bytes );
    count -= bytes;
    read_index = 0;
  }
  else {
    bytes = 0;
  }

  SMD_MEMCPY_FROM_SMEM( dst + bytes, src + read_index, count );

  return (read_index + count == src_size) ? 0 : (read_index + count);
} /* smd_memcpy_from_sm */

/*===========================================================================
FUNCTION      smd_memcpy_to_sm

DESCRIPTION   This function copys data from a source buffer to a shared memory
              buffer. It uses memcpy for performance and handles wrap around
              for the shared memory buffer.

ARGUMENTS     dst - pointer to shared memory buffer

              src - pointer to source buffer

              count - number of bytes to copy

              write_index - start index in destination buffer

              dst_size - size of destination buffer

RETURN VALUE  The next write index for the shared memory buffer.
===========================================================================*/
uint32 smd_memcpy_to_sm
(
  uint8 *dst,
  const uint8 *src,
  uint32 count,
  uint32 write_index,
  uint32 dst_size
)
{
  uint32 bytes;

  if( write_index + count > dst_size ) {
    /* must split write of count - copy bytes at end of dst first */
    bytes = dst_size - write_index;
    SMD_MEMCPY_TO_SMEM( dst + write_index, src, bytes );
    count -= bytes;
    write_index = 0;
  }
  else {
    bytes = 0;
  }

  SMD_MEMCPY_TO_SMEM( dst + write_index, src + bytes, count );

  return (write_index + count == dst_size) ? 0 : (write_index + count);
} /* smd_memcpy_to_sm */

#ifdef FEATURE_SMD

/*===========================================================================
FUNCTION      smd_memcpy_read_skip
  
DESCRIPTION   This function is a support function for smd_memcpy_process_read.
              It is called when process_read was unable to get a buffer and
              flow control is not enabled. In this case we want to just
              throw away the data in the shared memory buffer. But, the
              client must be involved so that packet based channels are
              handled correctly (the client doesn't want to lose packet sync).

ARGUMENTS     si - pointer to stream info structure
              rx_info - pointer to shared memory RX info
              mi - pointer to memcpy stream info structure
              read_index - pointer to read index in shared memory buffer
              write_index - pointer to write index in shared memory buffer
              byte_count - pointer to the number of bytes in share memory buffer

DEPENDENCIES  None
               
RETURN VALUE  TRUE if there's data to read, and a buffer to put it in,
              FALSE if there's no data to read.

===========================================================================*/
static boolean smd_memcpy_read_skip
(
  smd_stream_info_type *si,
  volatile smd_shared_stream_info_type *rx_info,
  smd_memcpy_stream_info_type *mi,
  uint32 *read_index,
  uint32 *write_index,
  uint32 *byte_count
)
{
  do {
    /* skip bytes in pipe as required and signal other processor */
    *byte_count = (mi->rx_flowctl_fn)( mi->rx_flowctl_data, *byte_count );

    *read_index += *byte_count;
    if( *read_index >= si->fifo_sz ) {
      *read_index -= si->fifo_sz;
    }

    SMD_WRITE_CH_HDR( si, SMD_RX_HDR, read_index, *read_index );

    /* if there are no more bytes exit */
    *write_index = SMD_READ_CH_HDR( si, SMD_RX_HDR, write_index );
    SMD_VALIDATE_INDEX( *write_index, si->fifo_sz );

    if( *write_index == *read_index ) {
      /* source buffer is empty - nothing to do */
      return FALSE;
    }

    /* There are more bytes, try to get another buffer */
    *byte_count = *write_index - *read_index;

    if( *byte_count > si->fifo_sz ) {
      /* Handle underflow in write_index - read_index calculation */
      *byte_count += si->fifo_sz;
    }

    (mi->rx_callback_fn)( mi->rx_callback_data,
                          0,
                          &mi->rx_buf_ptr,
                          &mi->rx_buf_size );
    
  } while( mi->rx_buf_ptr == NULL );

  return TRUE;
} /* smd_memcpy_read_skip */

/*===========================================================================
FUNCTION      smd_memcpy_init_stream

DESCRIPTION   This function initializes the memcpy specific part of the stream
              info structure and the shared memory structure.

              In future to deal with security related MPU's we may need to make 
              changes to this function.

ARGUMENTS     si - pointer to stream info structure

RETURN VALUE  None
===========================================================================*/
static void smd_memcpy_init_stream( smd_stream_info_type *si )
{
  si->mode.memcpy.tx_buf_ptr = NULL;
  si->mode.memcpy.tx_buf_size = 0;
  
  si->mode.memcpy.rx_buf_ptr = NULL;
  si->mode.memcpy.rx_buf_size = 0;

  SMD_WRITE_CH_HDR( si, SMD_TX_HDR,write_index, 0 );
  SMD_WRITE_CH_HDR( si, SMD_RX_HDR, read_index, 0 );
} /* smd_memcpy_init_stream */

/*===========================================================================
FUNCTION      smd_memcpy_process_read
  
DESCRIPTION   This function copies data out of the rx buffer in shared memory
              to local buffers provided by higher layers. It uses a callback
              to tell the higher layer how much data it read in the previous
              buffer (if any) and to get another buffer.

              If data was read out of shared memory, an transmit interrupt is
              sent to notify the other processor that it may be able to send
              more data.

ARGUMENTS     si - pointer to stream info structure

DEPENDENCIES  None
               
RETURN VALUE  None.
===========================================================================*/
static void smd_memcpy_process_read( smd_stream_info_type *si )
{
  smd_memcpy_stream_info_type *mi;
  volatile smd_shared_stream_info_type *rx_info;
  uint8 *in_buf;
  uint32 read_index;
  uint32 write_index;
  uint32 byte_count;

  SMD_MEMCPY_LOG_EVENT( si->port_id,
                        SMEM_LOG_EVENT_READ, 0, (uint32) si->port_id, 0 );

  mi = &si->mode.memcpy;
  rx_info = si->rx_shared_info_ptr;
  in_buf = (uint8*) si->rx_shared_fifo;

  if(  si->rx_fctl_enabled && 
      !SMD_READ_IF_SIG( si, SMD_TX_HDR, cts_sig ) )
  {
    /* Flow control enabled and CTS low - do nothing */
    return;
  }

  if( si->pending_ioctl_cnt != 0 )
  {
    /* pending IOCTL
     * We only need to set the flag since an event will be signaled 
     * by the client if it has not been already.
     */
    SMD_WRITE_IF_SIG( si, SMD_RX_HDR, data_written, TRUE );

    return;
  }

  read_index = SMD_READ_CH_HDR( si, SMD_RX_HDR, read_index );
  SMD_VALIDATE_INDEX( read_index, si->fifo_sz );

  write_index = SMD_READ_CH_HDR( si, SMD_RX_HDR, write_index );
  SMD_VALIDATE_INDEX( write_index, si->fifo_sz );

  if( write_index == read_index ) {
    /* source buffer is empty - nothing to do */
    SMD_MEMCPY_LOG_EVENT( si->port_id,
                          SMEM_LOG_EVENT_READ, 1, (uint32) si->port_id, 0 );
    return;
  }

  /*************************************************************
   * Once we enter this loop, we may update the read pointer
   * in memory, so, we must guarantee that we send a transmit event
   * to the processor on the other end of the channel when we
   * exit the loop.
   *************************************************************/
  do {
    byte_count = write_index - read_index;
    if( byte_count > si->fifo_sz )
    {
      /* Handle underflow in write_index - read_index calculation */
      byte_count += si->fifo_sz;
    }
    if( mi->rx_buf_ptr == NULL )
    {
      /* no destination buffer - try to get one */ 

      /* Stream mode should always return 0 for size on the first callback
         to distinguish it from packet mode which returns the size of the
         incoming packet
      */
      mi->rx_buf_size = 0;
    
      (mi->rx_callback_fn)( mi->rx_callback_data,
                            0,
                            &mi->rx_buf_ptr,
                            &mi->rx_buf_size );

      if( mi->rx_buf_ptr == NULL )
      {
        /* still no destination buffer - nothing to do */
        if( si->rx_fctl_enabled )
        {
          /* still no destination buffer - nothing to do 
           * We send a transmit event to the other processor just in case
           * we have updated the read index on a previous iteration of this
           * loop. Since this case is rare, sending an extra interrupt should
           * increase overhead less than setting a flag every time through
           * the loop. */
          SMD_MEMCPY_LOG_EVENT
            ( si->port_id, SMEM_LOG_EVENT_READ, 2, (uint32) si->port_id, 0 );
          smdi_event_send_transmit( si );
          return;
        }
        /* unable to get an rx buffer, and no flow control - skip data */
        if( !smd_memcpy_read_skip( si, rx_info, mi,
                                   &read_index, &write_index, &byte_count ) )
        {
          /* smd_memcpy_read_skip always updates the read index so we must
           * send a transmit event to the processor on the other end of the
           * channel */
          SMD_MEMCPY_LOG_EVENT( si->port_id, SMEM_LOG_EVENT_READ,
                                3, (uint32) si->port_id, 0 );
          smdi_event_send_transmit( si );
          return;
        }
      }
      mi->bytes_remaining = mi->rx_buf_size;
    }

    /* byte count is minimum of bytes in shared memory and space in buffer */
    if( byte_count > mi->bytes_remaining ) {
      byte_count = mi->bytes_remaining;
    }

    SMD_MEMCPY_LOG_EVENT( si->port_id,
                          (SMEM_LOG_SMEM_EVENT_BASE + 0x2001),
                          si->port_id,
                          read_index,
                          byte_count );
     
    read_index = smd_memcpy_from_sm( mi->rx_buf_ptr,
                                     in_buf,
                                     byte_count,
                                     read_index, si->fifo_sz );
    /* byte_count number of bytes was read to rcv buffer */
    mi->bytes_remaining -= byte_count;

    /* Advance the rx_buf_ptr */
    mi->rx_buf_ptr += byte_count;

    /* Update the shared memory copy of read_index */
    SMD_WRITE_CH_HDR( si, SMD_RX_HDR, read_index, read_index );

    if( !si->ret_full_rcv_bufs_en || (mi->bytes_remaining == 0) )
    {
       /* let the client know that the buffer has data */
      (mi->rx_callback_fn)( mi->rx_callback_data,
                            mi->rx_buf_size - mi->bytes_remaining,
                            &mi->rx_buf_ptr,
                            &mi->rx_buf_size );

      mi->bytes_remaining = mi->rx_buf_size;
    }

    /* if the client gave us another buffer, try to fill it before returning */
    write_index = SMD_READ_CH_HDR( si, SMD_RX_HDR, write_index );
    SMD_VALIDATE_INDEX( write_index, si->fifo_sz );
    /* If there's more data and flow control is disabled and there are no
       pending IOCTLs attempt to fill next buffer */
  } while( write_index != read_index && (si->pending_ioctl_cnt == 0) );

  if( write_index != read_index && (si->pending_ioctl_cnt != 0) )
  {
    /* exited loop because ioctl pending
     * We only need to set the flag since an event will be signaled 
     * by the client if it has not been already.
     */
    SMD_WRITE_IF_SIG( si, SMD_RX_HDR, data_written, TRUE );
  }

  SMD_MEMCPY_LOG_EVENT( si->port_id,
                        SMEM_LOG_EVENT_READ, 4, (uint32) si->port_id, 0 );

  /* Send transmit interrupt to tell other processor to send more data */
  smdi_event_send_transmit( si );
} /* smd_memcpy_process_read */

/*===========================================================================
FUNCTION      smd_memcpy_process_write

DESCRIPTION   This function writes data from buffers provided by higher layers
              into the tx buffer in shared memory. It uses a callback to get
              a new buffer. When the callback is called, this layer is done
              with the previous buffer (if any).

              If data is written into shared memory a receive interrupt is
              sent to notify the other processor that it may be able to read
              more data.

ARGUMENTS     si - pointer to stream info structure
              
RETURN VALUE  None
===========================================================================*/
static void smd_memcpy_process_write( smd_stream_info_type *si )
{
  smd_memcpy_stream_info_type *mi;
  volatile smd_shared_stream_info_type *tx_info;
  uint8 *out_buf;
  uint32 read_index;
  uint32 write_index;
  uint32 byte_count;

  SMD_MEMCPY_LOG_EVENT( si->port_id,
                        SMEM_LOG_EVENT_WRITE, 0, (uint32) si->port_id, 0 );

  mi = &si->mode.memcpy;
  tx_info = si->tx_shared_info_ptr;
  out_buf = (uint8 *) si->tx_shared_fifo;

  /* If there is not a source buffer currently being 
  ** processed, try to get one
  */
  if( mi->tx_buf_ptr == NULL ) {
    (mi->tx_callback_fn)( mi->tx_callback_data,
                          &mi->tx_buf_ptr,
                          &mi->tx_buf_size );
  }

  /* If the current buffer is empty, continue to try to get one
  ** with some data in it.
  */
  while( (mi->tx_buf_ptr != NULL) && (mi->tx_buf_size == 0) ) {
    /* We're done with this buffer, get the next one */
    (mi->tx_callback_fn)( mi->tx_callback_data,
                          &mi->tx_buf_ptr,
                          &mi->tx_buf_size );
  }

  if( mi->tx_buf_ptr == NULL ) {
    /* still no source buffer - nothing to do */
    if( SMD_STREAM_FLUSHING(si) &&
        ( SMD_READ_CH_HDR( si, SMD_TX_HDR, read_index ) == 
          SMD_READ_CH_HDR( si, SMD_TX_HDR, write_index ) ) )
    {
      /* flush complete - no tx buffers and shared memory buffer empty */
      smdi_stream_state_machine( si, SMD_EVENT_FLUSH_COMPLETE );
    }
    SMD_MEMCPY_LOG_EVENT( si->port_id,
                          SMEM_LOG_EVENT_WRITE, 1, (uint32) si->port_id, 0 );
    return;
  }

  write_index = SMD_READ_CH_HDR( si, SMD_TX_HDR, write_index );
  SMD_VALIDATE_INDEX( write_index, si->fifo_sz );

  read_index = SMD_READ_CH_HDR( si, SMD_TX_HDR, read_index );
  SMD_VALIDATE_INDEX( read_index, si->fifo_sz );

  /* compute bytes used in buffer */
  byte_count = write_index - read_index;

  if( byte_count > si->fifo_sz ) {
    /* Handle underflow */
    byte_count += si->fifo_sz;
  }

  /* Make sure there's room for some data */
  if( byte_count >= si->max_queued_data )
  {
    /* destination buffer is full - nothing to do */
    SMD_MEMCPY_LOG_EVENT( si->port_id,
                          SMEM_LOG_EVENT_WRITE, 3, (uint32) si->port_id, 0 );
    /* If we have more data, clear the suppress-interrupt flag */
    SMD_WRITE_IF_SIG( si, SMD_TX_HDR, mask_recv_intr, 0 );
    return;
  }

  /*************************************************************
   * Once we enter this loop, we will update the write pointer
   * in memory and send a receive event to the processor on the 
   * other end of the channel.
   *************************************************************/
  do {
    /* Need to update the local prev_rts. This is important as it keeps the state
       and the action took upon in sync */
    if( si->tx_fctl_enabled )
    {
      si->prev_rts = SMD_READ_IF_SIG( si, SMD_RX_HDR, rts_sig );
      if( 0 == si->prev_rts ) 
      {
        /* flow control enabled and RTS is not set - wait */
        SMD_MEMCPY_LOG_EVENT( si->port_id,
                              SMEM_LOG_EVENT_WRITE, 2, (uint32) si->port_id, 0 );
        break;
      }
    }

    /* now compute bytes available in buffer */
    byte_count = si->max_queued_data - byte_count;

    if( byte_count > mi->tx_buf_size ) {
        byte_count = mi->tx_buf_size;
    }

    SMD_MEMCPY_LOG_EVENT( si->port_id,
                          (SMEM_LOG_SMEM_EVENT_BASE + 0x2000),
                          si->port_id,
                          write_index,
                          byte_count );

    /* write data buffer */
    write_index = smd_memcpy_to_sm( out_buf, mi->tx_buf_ptr, byte_count,
                                    write_index, si->fifo_sz );
    mi->tx_buf_ptr += byte_count;
    mi->tx_buf_size -= byte_count;

    /* Update the shared memory copy of write_index */
    /* Don't need memory barrier between memcpy and writing write_index,
       because write operations to the same memory are serialized by the HW */
    SMD_WRITE_CH_HDR( si, SMD_TX_HDR, write_index, write_index );

    if( mi->tx_buf_size == 0 ) {
      /* We're done with this buffer, get the next one */
      (mi->tx_callback_fn)( mi->tx_callback_data,
                            &mi->tx_buf_ptr,
                            &mi->tx_buf_size );
    }

    if(mi->tx_buf_size)
    {
      /* If we have more data, clear the suppress-interrupt flag */
      SMD_WRITE_IF_SIG( si, SMD_TX_HDR, mask_recv_intr, 0 );
    }

    read_index = SMD_READ_CH_HDR( si, SMD_TX_HDR, read_index );
    SMD_VALIDATE_INDEX( read_index, si->fifo_sz );

    /* compute bytes used in buffer */
    byte_count = write_index - read_index;

    if( byte_count > si->fifo_sz ) {
      /* Handle underflow */
      byte_count += si->fifo_sz;
    }

    /* If there's room for more data and we have a buffer to send, send another buffer */
  } while( (byte_count < si->max_queued_data) && (mi->tx_buf_ptr != NULL) );

  SMD_MEMCPY_LOG_EVENT( si->port_id,
                        SMEM_LOG_EVENT_WRITE, 4, (uint32) si->port_id, 0 );

  if(SMD_STREAM_FLUSHING(si))
  {
    SMD_WRITE_IF_SIG( si, SMD_TX_HDR,mask_recv_intr, 0 );
  }
  else if(!mi->tx_buf_size)
  {
    /* If we have no pending data, set the suppress-interrupt flag */
    SMD_WRITE_IF_SIG( si, SMD_TX_HDR, mask_recv_intr, 1 );
  }

  /* Send receive interrupt to tell other processor to receive this data */
  smdi_event_send_receive( si );
} /* smd_memcpy_process_write */

/*===========================================================================
FUNCTION      smd_memcpy_flush_tx

DESCRIPTION   If there is no data either in the tx shared memory buffer or
              waiting to be copied into the tx shared memory buffer, call
              the flush callback because the flush is complete.

              Otherwise, set the stream state to FLUSHING. When
              smd_memcpy_process_write sees the above condition (that all
              data has been received by the other processor) and the stream
              is in the FLUSHING state, it calls the flush callback because
              the flush is complete.

ARGUMENTS     si - pointer to stream info structure
              
RETURN VALUE  None
==================================================x=========================*/
static void smd_memcpy_flush_tx( smd_stream_info_type *si )
{
  smd_memcpy_stream_info_type *mi;
  volatile smd_shared_stream_info_type *tx_info;

  mi = &si->mode.memcpy;
  tx_info = si->tx_shared_info_ptr;

  if( ( ( SMD_READ_CH_HDR( si, SMD_TX_HDR, write_index ) ) == 
        ( SMD_READ_CH_HDR( si, SMD_TX_HDR, read_index ) ) ) &&
      ( mi->tx_buf_ptr == NULL ) )
  {
    /* the SMEM buffer is empty and no data is pending at this layer.  Try 
       to get a queue from the client */
    (mi->tx_callback_fn)( mi->tx_callback_data,
                          &mi->tx_buf_ptr,
                          &mi->tx_buf_size );

    if( mi->tx_buf_size != 0 )
    {
      /* Still have data to send, just change state to flushing */
      smdi_stream_state_machine( si, SMD_EVENT_FLUSH );
    }
    else
    {
      /* All data has been received by the other side - flush complete */
      SMD_CALL_FLUSH_CB( si );
    }
  }
  else
  {
    /* Still have data to send, just change state to flushing */
    smdi_stream_state_machine( si, SMD_EVENT_FLUSH );
  }
} /* smd_memcpy_flush_tx */

/*===========================================================================
FUNCTION      smd_memcpy_dtr

DESCRIPTION - This function is called when the DTR state has changed. It
              clears the tx/rx buffer when DTR is de-asserted. It leaves
              the tx/rx buffer intact if DTR is asserted. 

ARGUMENTS     si - pointer to stream info structure
              
RETURN VALUE  None
==================================================x=========================*/
static void smd_memcpy_dtr( smd_stream_info_type *si )
{

} /* smd_memcpy_dtr */

/*===========================================================================
FUNCTION      smd_memcpy_drop

DESCRIPTION   DESCRIPTION - Drops all pending transmit buffers.  This function
                            should only be called when the port_id is in an
                            opening state and the tx_opening_drop_enabled
                            control is set.

ARGUMENTS     si - pointer to stream info structure
              
RETURN VALUE  None
============================================================================*/
static void smd_memcpy_drop( smd_stream_info_type *si )
{
  smd_memcpy_stream_info_type *mi;

  mi = &si->mode.memcpy;

  do{
    (mi->tx_callback_fn)( mi->tx_callback_data,
                          &mi->tx_buf_ptr,
                          &mi->tx_buf_size );
  }while( mi->tx_buf_ptr != NULL );
}/* smd_memcpy_drop */

/*===========================================================================
FUNCTION      smd_memcpy_abort_tx

DESCRIPTION   Aborts any transmits in progress on si->port.

ARGUMENTS     si - pointer to stream info structure
              
RETURN VALUE  The amount of unprocessed bytes in the client tx_buf.
============================================================================*/
static uint32 smd_memcpy_abort_tx( smd_stream_info_type *si )
{
  uint32 bytes_left;
  smd_memcpy_stream_info_type *mi;

  mi = &si->mode.memcpy;

  mi->tx_buf_ptr = NULL;

  bytes_left = mi->tx_buf_size;
  mi->tx_buf_size = 0;

  return bytes_left;
}/* smd_memcpy_abort_tx */

/*===========================================================================
FUNCTION      smd_memcpy_abort_rx

DESCRIPTION   Aborts any receives in progress on si->port.

ARGUMENTS     si - pointer to stream info structure
              
RETURN VALUE  The amount of unprocessed bytes in the client rx_buf.
============================================================================*/
static uint32 smd_memcpy_abort_rx( smd_stream_info_type *si )
{
  uint32 bytes_left;
  smd_memcpy_stream_info_type *mi;

  mi = &si->mode.memcpy;

  mi->rx_buf_ptr = NULL;
  mi->rx_buf_size = 0;

  bytes_left = mi->bytes_remaining;
  mi->bytes_remaining = 0;

  return bytes_left;
}/* smd_memcpy_abort_rx */

/*===========================================================================
  FUNCTION  smd_memcpy_reset
===========================================================================*/
/**
  This function sends the notification to SMD clients when the port is under
  reset conditions.
 
  Upon execution of this function, the DTR of the specified SMD stream
  is de-asserted, and then the DTR callbacks are executed so the client
  knows that the port is down.
  
  Also, the Memcpy layer Rx/Tx buffers are cleared and the client's Stream
  Reset callback is invoked so that it may do its own cleanup.

  @param[in] si           Pointer to the SMD port's stream information.
 
  @return
  None.
 
  @dependencies
  The SMD task should not be allowed to operate when SSR in in progress
  (required for Phase 2, since SSR events is synchronous).

*/
/*=========================================================================*/
static void smd_memcpy_reset( smd_stream_info_type *si )
{
  /* Deassert DTR */
  si->prev_dtr = 0;

  /* Call DTR change function */
  if( si->stream_dtr  )
  {
    si->stream_dtr( si );
  }

  /* Call the DTR callback */
  if( si->dtr_callback_fn )
  {
    si->dtr_callback_fn();
  }

  /* Clear memcpy layer Tx/Rx buffers */
  si->mode.memcpy.tx_buf_ptr = NULL;
  si->mode.memcpy.tx_buf_size = 0;
    
  si->mode.memcpy.rx_buf_ptr = NULL;
  si->mode.memcpy.rx_buf_size = 0;

  /* Call the extended DTR callback */
  if( si->dtr_callback_ext_fn )
  {
    si->dtr_callback_ext_fn( si->dtr_callback_ext_data );
  }

  /* Call the client level Stream Reset callback */
  if( si->mode.memcpy.reset_callback_fn )
  {
    si->mode.memcpy.reset_callback_fn(
                      si->mode.memcpy.reset_callback_data);
  }
}

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
/*===========================================================================
FUNCTION      smd_memcpy_open

DESCRIPTION   This routine handles an open command for a memcpy stream. It
              allocates and initializes a shared memory structure, finishes
              initializing the stream info structure and sends the stream
              state machine an OPEN event.

ARGUMENTS     port_id - SMD port to open

RETURN VALUE  This function will always return SMD_DONE

SIDE EFFECTS  None
===========================================================================*/
smd_api_return_type smd_memcpy_open( smd_port_id_type port_id )
{
  smd_stream_info_type             *info;

  info = smdi_get_stream_info( port_id );

  /* Initialize jump table with memcpy entry points */
  info->stream_init = smd_memcpy_init_stream;
  info->stream_read = smd_memcpy_process_read;
  info->stream_write = smd_memcpy_process_write;
  info->stream_flush_tx = smd_memcpy_flush_tx;
  info->stream_dtr = smd_memcpy_dtr;
  info->stream_drop = smd_memcpy_drop;
  info->stream_tx_abort = smd_memcpy_abort_tx;
  info->stream_rx_abort = smd_memcpy_abort_rx;
  info->stream_close = NULL;
  info->stream_reset = smd_memcpy_reset;

  smdi_allocate_stream_channel( port_id, info );

  /* Make sure the channel is closed before trying to open it. This should
   * address an unfortunate edge case in Q6 restart. */
  SMD_WRITE_CH_HDR( info, SMD_TX_HDR, stream_state, SMD_SS_CLOSED );
  SMD_CLR_IF_SIGS( info, SMD_TX_HDR );

  smdi_stream_state_machine( info, SMD_EVENT_OPEN );

  return SMD_DONE;
}/* smd_memcpy_open */

/** 
 * Returns the number of bytes available in the receive FIFO.
 *
 *
 * @param[in]    port_id  port id
 *
 * @return       The number of bytes available in receive FIFO. (Determined by how 
 *               many bytes have been written so far and not yet read.)
 *
 * @sideeffects  None
 */
uint32 smd_rx_peek
(
  smd_port_id_type  port_id
)
{
  smd_info_type                        *info;
  smd_stream_info_type                 *sinfo;
  uint32                                read_index;
  uint32                                write_index;
  uint32                                byte_count;
  
  info  = smdi_get_info(port_id);
  sinfo = &info->info.stream;
 
  read_index = SMD_READ_CH_HDR( sinfo, SMD_RX_HDR, read_index );
  SMD_VALIDATE_INDEX(read_index, sinfo->fifo_sz);

  write_index = SMD_READ_CH_HDR( sinfo, SMD_RX_HDR, write_index );
  SMD_VALIDATE_INDEX(write_index, sinfo->fifo_sz);
    
  byte_count = write_index - read_index;
  if( write_index < read_index )
  {
    /* FIFO wrapped */
    byte_count += sinfo->fifo_sz;
  }

  return byte_count;
}

#endif /* FEATURE_SMD */
