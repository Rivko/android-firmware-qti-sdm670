/**
 * @file smd_lite_writev.c
 *
 * Vector write implementation for SMD Lite.
 *
 * This API function is implemented in a separate source file so that it can
 * be easily discarded by the linker if it is not used by clients.
 */

/*==============================================================================
     Copyright (c) 2011 Qualcomm Technologies Incorporated. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/22/12   bm      Use new macros to access SMD channel header struct.
11/30/11   bt      Removed documentation statement that is not always true.
04/12/12   vm      Fixed l4/SAL warnings
05/25/11   tl      Changed info-level log message to debug-level log message
03/25/11   tl      Always write at least one byte of payload when transmitting
                   large packets
03/10/11   bt      Compiler warning resolved for len_written in smdl_writev.
02/08/11   tl      Added support for writing long packets
01/27/11   tl      Implemented SMD Lite vector write
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include "smd_os.h"

#include "smd_lite.h"
#include "smd_internal.h"
#include "smd_os_common.h"
#include "smd_packet.h"

#include "smdl_debug.h"

/*===========================================================================
                    EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/**
 * Writes data from the provided vector to the SMD port.
 *
 * Packet mode: The vector is treated as a single packet to be sent to the
 * remote processor. By default, the packet will be sent in its entirety if
 * space is available in the outgoing FIFO or will be dropped (with an
 * appropriate error code) if space is not available.
 *
 * Streaming mode: This function will write as much of the provided vector as
 * is possible, limited by the value returned by smdl_tx_peek(). This function
 * will return the number of successfully written bytes. Any bytes that cannot
 * fit in the outgoing FIFO will be truncated.
 *
 * In both streaming and packet mode, clients may specify the
 * #SMDL_WRITE_FLAG_LONG bit of \c flags to enable writing messages longer
 * than the FIFO. In this mode, smdl_writev() will write as much of the
 * message as is possible, then return #SMDL_CONTINUE. It is the
 * responsibility of the client to wait until it receives #SMDL_EVENT_WRITE,
 * then call smdl_writev() with the same arguments. smdl_writev() will
 * again write as much data as can be written. Once smdl_writev() has written
 * the entire message, it will return the size of the whole message.
 *
 * Writing 0 bytes to the port will not change shared memory and will not
 * signal the remote processor.
 *
 * Clients interested in determining the maximum packet size that can be
 * written to the port (without specifying #SMDL_WRITE_FLAG_LONG) may call
 * smdl_tx_peek().
 *
 * @param[in]  port    The port to write data to
 * @param[in]  iovec   Pointer to a linked list of I/O vectors to be written
 *                     to the port.
 * @param[in]  flags   Bitwise-or of flags controlling SMD's behavior. See
 *                     #SMDL_WRITE_FLAGS_NONE and #SMDL_WRITE_FLAG_NOINT.
 *
 * @return       Return behavior depends on the transfer mode used.
 *               -If #SMDL_WRITE_FLAG_LONG is specified:
 *                 -If the whole message has been written: The size of the
 *                  message.
 *                 -If the whole message has not been written: #SMDL_CONTINUE.
 *               -Packet mode: 
 *                 -If the port has sufficient space to hold the 
 *                  provided packet: The length of the packet written to the 
 *                  port.
 *                 -If the port does not have sufficient space to 
 *                  hold the provided packet: 0.
 *               -Streaming mode: The number of bytes that were successfully 
 *                written to the port. If truncated, will be less than the
 *                total amount of data available in the vector.
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  Writes to shared memory and signals the remote processor.
 */
int32 smdl_writev
(
  smdl_handle_type       port,
  const smdl_iovec_type *iovec,
  uint32                 flags
)
{
  const smdl_iovec_type *iovec_iter;
  uint32 read_index;
  uint32 write_index;
  uint32 byte_count;
  uint32 skip;
  uint32 bytes_remaining;
  int32 len_written;
  uint32 len;

  if(port == NULL)
  {
    return -1;
  }

  if(iovec == NULL)
  {
    return -1;
  }

  /* Iterate through the linked list of vectors to determine how much data we
   * have to send */
  len = 0;
  for(iovec_iter = iovec; iovec_iter; iovec_iter = iovec_iter->next)
  {
    len += iovec_iter->length;
  }

  if(len == 0)
  {
    /* Don't send zero-length messages */
    return 0;
  }

  SMDL_DEBUG("smdl_writev(%p): Ready to write packet: length=%d continue=%d",
      port, len, port->mode.lite.continue_write);

  smd_acquire_cs_lock(port->mode.lite.tx_mutex);

  if(port->mode.lite.continue_write && !(flags & SMDL_WRITE_FLAG_LONG))
  {
    /* A continued-write is in progress but the continued-write flag was not
     * specified. This is probably an unrecoverable error. */
    smd_release_cs_lock(port->mode.lite.tx_mutex);
    SMDL_ERROR("smdl_writev(%p): Expecting continued write (%d) but flags=%x",
        port, port->mode.lite.continue_write, flags);
    return -1;
  }

  /* Verify that this end of the port is up and running */
  if( SMD_READ_CH_HDR( port, SMD_TX_HDR, stream_state ) != SMD_SS_OPENED )
  {
    port->mode.lite.notify_on_write = TRUE;
    smd_release_cs_lock(port->mode.lite.tx_mutex);
    SMDL_DEBUG("smdl_writev(%p): Stream state %d",
      port, SMD_READ_CH_HDR( port, SMD_TX_HDR, stream_state ), 0);
    return 0;
  }

  /* Check flow control on the other end of this channel */
  if( !SMD_READ_IF_SIG( port, SMD_RX_HDR, rts_sig ) )
  {
    port->mode.lite.notify_on_write = TRUE;
    smd_release_cs_lock(port->mode.lite.tx_mutex);
    SMDL_DEBUG("smdl_writev(%p): DSR=%d RTS=%d", port,
      SMD_READ_IF_SIG( port, SMD_RX_HDR, rts_sig ),
      SMD_READ_IF_SIG( port, SMD_RX_HDR, dsr_sig )
     );
    return 0;
  }

  /* Clear the mask_recv_intr flag while testing whether there is enough room
   * in the tx FIFO. This may result in more transmit-event interrupts than
   * strictly necessary but eliminates potential race conditions when space is
   * made available after checking but before setting the flag. */
  SMD_WRITE_IF_SIG( port, SMD_TX_HDR, mask_recv_intr, 0 );
  SMD_MEMORY_BARRIER();

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

  if ((port->mode.lite.transfer_mode == SMDL_MODE_PACKET)
      && (port->mode.lite.continue_write == 0))
  {
    uint32 packet_header[SMD_PACKET_HEADER_SIZE];
    uint32 space_needed;

    if(flags & SMDL_WRITE_FLAG_LONG)
    {
      /* In continued-write mode, make sure there's enough space for at least
       * the packet header and one byte of payload */
      space_needed = SMD_PACKET_HEADER_BYTE_SIZE + 1;
    }
    else
    {
      /* Make sure there's enough space for the entire packet */
      space_needed = len + SMD_PACKET_HEADER_BYTE_SIZE;
    }

    /* In packet mode, make sure there's enough room for the entire vector,
     * unless the continued-write flag is set */
    if( byte_count < space_needed)
    {
      /* destination buffer is full - nothing to do */
      port->mode.lite.notify_on_write = TRUE;
      smd_release_cs_lock(port->mode.lite.tx_mutex);
      if(space_needed > port->max_queued_data)
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
    
    /* Write the packet mode header */
    SMDL_DEBUG("smdl_writev(%p): Writing packet header at write_index=%d",
        port, write_index, 0);

    memset(&packet_header, 0, sizeof(packet_header));
    SMD_SET_PACKET_SIZE(packet_header, len);

    write_index = smd_memcpy_to_sm( (uint8 *)port->tx_shared_fifo,
                                    (uint8 *)packet_header,
                                    sizeof(packet_header), write_index,
                                    port->fifo_sz );

    byte_count -= SMD_PACKET_HEADER_BYTE_SIZE;
  }

  /* Write the body of the message. It doesn't really matter whether we're in
   * packet or streaming mode at this point. */

  /* skip is a running tally of the bytes that we need to skip because we've
   * already written them. It's decremented as we skip data while iterating
   * through the vectors with data to be transmitted. */
  skip = port->mode.lite.continue_write;

  /* bytes_remaining is the running tally of the number of bytes that will be
   * written this cycle. If the whole message can be written at once, it's the
   * total length of the message. If only a partial message can be written,
   * it's the number of bytes that will be written. */
  bytes_remaining = MIN(byte_count, len - skip);

  for(iovec_iter = iovec; iovec_iter; iovec_iter = iovec_iter->next)
  {
    uint32 bytes_this_vector;

    if((iovec_iter->length == 0) || (iovec_iter->buffer == NULL))
    {
      continue;
    }

    if(skip)
    {
      if(skip >= iovec_iter->length)
      {
        skip -= iovec_iter->length;
        continue;
      }
    }

    bytes_this_vector = MIN(bytes_remaining, iovec_iter->length - skip);

    if(skip)
    {
      SMDL_DEBUG("smdl_writev(%p): Skipping %d bytes from %p",
          port, skip, iovec_iter->buffer);
    }
    SMDL_DEBUG("smdl_writev(%p): Writing %d bytes from %p",
        port, bytes_this_vector, iovec_iter->buffer);

    write_index = smd_memcpy_to_sm( (uint8 *)port->tx_shared_fifo,
                                    (uint8 *)(iovec_iter->buffer) + skip,
                                    bytes_this_vector,
                                    write_index, port->fifo_sz );

    skip = 0;
    bytes_remaining -= bytes_this_vector;
    if(bytes_remaining == 0)
    {
      /* No more data can be written. No need to iterate through the remaining
       * vectors. */
      break;
    }
  }
  SMD_WRITE_CH_HDR( port, SMD_TX_HDR, write_index, write_index );

  if (byte_count + port->mode.lite.continue_write < len)
  {
    /* We were not able to transfer the entire vector (in streaming mode or
     * long packet mode). Set notify_on_write so that the client receives an
     * event when more room becomes available. */
    port->mode.lite.notify_on_write = TRUE;
    if(flags & SMDL_WRITE_FLAG_LONG)
    {
      /* Long packet mode: Return SMDL_CONTINUE and save the number of bytes
       * we actually wrote so we can continue in the right place. */
      port->mode.lite.continue_write += byte_count;
      len_written = SMDL_CONTINUE;
    }
    else
    {
      /* Streaming mode: Return the number of bytes actually written */
      len_written = byte_count;
    }
  }
  else
  {
    /* We were able to transfer the entire vector (in either packet or
     * streaming mode). */
    port->mode.lite.notify_on_write = FALSE;
    port->mode.lite.continue_write = 0;
    SMD_WRITE_IF_SIG( port, SMD_TX_HDR, mask_recv_intr, 1 );
    len_written = len;
  }

  smd_release_cs_lock(port->mode.lite.tx_mutex);

  if((flags & SMDL_WRITE_FLAG_NOINT) != SMDL_WRITE_FLAG_NOINT)
  {
    smdi_event_send_receive( port );
  }

  SMDL_DEBUG("smdl_writev(%p): tx_read=%d tx_write=%d", port,
    SMD_READ_CH_HDR( port, SMD_TX_HDR, read_index ),
    SMD_READ_CH_HDR( port, SMD_TX_HDR, write_index ));
  SMDL_DEBUG("smdl_writev(%p): rx_read=%d rx_write=%d", port,
    SMD_READ_CH_HDR( port, SMD_RX_HDR, read_index ),
    SMD_READ_CH_HDR( port, SMD_RX_HDR, write_index ));

  return len_written;
}
