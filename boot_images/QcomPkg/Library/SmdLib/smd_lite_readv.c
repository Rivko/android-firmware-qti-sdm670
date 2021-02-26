/**
 * @file smd_lite_readv.c
 *
 * Vector read implementation for SMD Lite.
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
10/24/12   bt      Update to use control struct macros based on edge.
09/14/12   bt      Return early if there is nothing available to read.
04/12/12   vm	     Fixed l4/SAL warnings
04/05/11   tl      Include smd.h to get SMD data types
03/23/11   tl      Truncate packets larger than the provided vector
03/10/11   bt      Compiler warnings resolved in smdl_readv.
02/14/11   tl      Added support for streaming mode and reading long packets
01/24/11   tl      Implemented SMD Lite vector read
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
 * Reads data from the SMD port into the provided vector.
 *
 * Packet mode: This function will read the next packet in the port into the
 * provided buffer or buffers in the linked list of vectors \c iovec. Each
 * element of the I/O vector describes a buffer size and length, and a pointer
 * to the next buffer in the list. This function will copy as much of the
 * packet as possible into the first buffer in the chain, then continue
 * copying data until either the packet is complete or the buffers are
 * exhausted. If the packet is larger than the vector, as much data as
 * possible will be copied to the vector and remainder will be discarded.
 * Clients concerned about dropping data should call smdl_rx_peek() to
 * determine the size of the next packet.
 *
 * If the packet to be received is larger than the FIFO (specified when
 * opening the port using smdl_open()), and the #SMDL_READ_FLAG_LONG flag in
 * \c flags is not set, this function will read as much of the packet as is
 * possible in a single read, then discard the remainder of the packet. If the
 * #SMDL_READ_FLAG_LONG flag is set, this function will read as much data as
 * is available at a time, then return #SMDL_CONTINUE if more data must be
 * received before the read is complete. The caller is responsible for waiting
 * until it receives the #SMDL_EVENT_READ event callback, at which point the
 * caller must call smdl_readv() again, using the same arguments. This
 * function will read as much data as is possible, repeating as many times as
 * is necessary to read the entire packet, and ultimately return the size of
 * the entire packet when the entire packet has been read.
 *
 * Streaming mode: This function will read as much data as will fit into the
 * provided linked list of vector \c iovec from the port into the provided
 * buffer, limited by the value returned by smdl_rx_peek().  Any remaining
 * bytes in the port will be left there for future reads.
 *
 * Setting the #SMDL_READ_FLAG_LONG flag when a port has been opened in
 * streaming mode has no effect. This function will always read as much data
 * as is available and return the number of bytes read.
 *
 * This function does not block; it will return 0 immediately if no data is
 * available.
 *
 * This function may be called from the notification callback function
 * registered with smdl_open().
 *
 * @param[in]   port    The port to read data from
 * @param[out]  iovec   Pointer to a linked list of I/O vectors for the packet
 *                      to be copied into.
 * @param[in]   flags   Bitwise-or of flags controlling SMD's behavior. See
 *                      #SMDL_READ_FLAGS_NONE and #SMDL_READ_FLAG_LONG.
 *
 * @return       If data was read: The total number of bytes read into \c iovec
 * @return       If no data was available: 0
 * @return       If #SMDL_READ_FLAG_LONG is specified in packet mode and the
 *               whole packet was not read: #SMDL_CONTINUE
 * @return       If an error occurred: A negative error code.
 *
 * @sideeffects  Reads from shared memory and signals the remote processor.
 */
int32 smdl_readv
(
  smdl_handle_type port,
  smdl_iovec_type *iovec,
  uint32           flags
)
{
  uint32 read_index;
  uint32 write_index;
  uint32 byte_count;
  int32 bytes_read;
  uint32 packet_len = 0;
  uint32 skip;

  if(port == NULL)
  {
    return -1;
  }

  if(iovec == NULL)
  {
    return -1;
  }

  smd_acquire_cs_lock(port->mode.lite.rx_mutex);

  if(port->mode.lite.continue_read && !(flags & SMDL_READ_FLAG_LONG))
  {
    /* A continued-read is in progress but the continued-read flag was not
     * specified. This is probably an unrecoverable error. */
    smd_release_cs_lock(port->mode.lite.rx_mutex);
    SMDL_ERROR("smdl_readv(%p): Expecting continued read (%d) but flags=%x",
        port, port->mode.lite.continue_read, flags);
    return -1;
  }

  if(port->mode.lite.transfer_mode == SMDL_MODE_PACKET)
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
  if( read_index > write_index )
  {
    /* FIFO wrapped */
    byte_count += port->fifo_sz;
  }

  if(port->mode.lite.transfer_mode == SMDL_MODE_STREAMING)
  {
    packet_len = byte_count;
  }

  /* If the packet to be read is larger than the FIFO, truncate it and read
   * only that much as is available. */
  if((packet_len > port->max_queued_data) && !(flags & SMDL_READ_FLAG_LONG))
  {
    port->mode.lite.skip = packet_len - port->max_queued_data;
    SMDL_DEBUG("smdl_readv(): Skipping %d bytes (packet_len=%d, max=%d)",
        port->mode.lite.skip, packet_len, port->max_queued_data);
    packet_len = port->max_queued_data;
  }

  /* skip is a running tally of the bytes that we need to skip because we've
   * already read them. It's decremented as we skip data while iterating
   * through the vectors with data to be received. */
  skip = port->mode.lite.continue_read;

  if(byte_count == 0)
  {
    /* There is no further data to read yet.  This is either Streaming mode, or
     * Packet mode after the packet header has been read.  Return early here
     * to avoid unnecessary processing time and sending any interrupts. */
    smd_acquire_cs_lock(port->mode.lite.rx_mutex);
    return 0;
  }
  else if((byte_count >= packet_len) || (flags & SMDL_READ_FLAG_LONG))
  {
    /* bytes_remaining is now the number of bytes available to be read from
     * the message. */
    uint32 bytes_remaining = MIN(byte_count, packet_len - skip);
    bytes_read = 0;

    /* Iterate through the provided vector, reading data from the FIFO into
     * the buffers */
    for(; iovec; iovec = iovec->next)
    {
      uint32 bytes_to_read;

      if((iovec->length == 0) || (iovec->buffer == NULL))
      {
        continue;
      }

      if(skip)
      {
        if(skip >= iovec->length)
        {
          skip -= iovec->length;
          continue;
        }
        SMDL_DEBUG("smdl_readv(%p): Skipping %d bytes from %p",
            port, skip, iovec->buffer);
      }

      bytes_to_read = MIN(iovec->length - skip, bytes_remaining);

      SMDL_DEBUG("smdl_readv(%p): Reading %d bytes into %p",
          port, bytes_to_read, iovec->buffer);

      read_index = smd_memcpy_from_sm( (uint8 *)iovec->buffer + skip,
                                       (uint8 *)port->rx_shared_fifo,
                                       bytes_to_read,
                                       read_index, port->fifo_sz );

      skip = 0;

      bytes_read += bytes_to_read;
      bytes_remaining -= bytes_to_read;
      if(bytes_remaining == 0)
      {
        /* No more data can be written. No need to iterate through the remaining
         * vectors. */
        break;
      }
    }

    if( (flags & SMDL_READ_FLAG_LONG) &&
        (bytes_read + port->mode.lite.continue_read < packet_len) &&
        iovec)
    {
      /* We were not able to read the entire packet. The client must retry
       * when more data is available. */
      port->mode.lite.continue_read += bytes_read;
      bytes_read = SMDL_CONTINUE;
    }
    else
    {
      /* If the buffer provided by the client was shorter than the packet in
       * the fifo, advance the read index to skip the unread portion of the
       * packet.
       */
      if(packet_len + port->mode.lite.skip >
          bytes_read + port->mode.lite.continue_read)
      {
        read_index += byte_count - bytes_read;
        if(read_index >= port->fifo_sz)
        {
          read_index -= port->fifo_sz;
        }
        port->mode.lite.skip +=
          packet_len - port->mode.lite.continue_read - byte_count;
      }

      /* We were able to read the entire vector in packet mode, or we read as
       * much data as was available in streaming mode. Return the total length
       * read to the client and reset the continued-read state. */

      bytes_read += port->mode.lite.continue_read;
      port->mode.lite.continue_read = 0;
      port->mode.lite.packet_header_len = 0;
    }

    SMD_WRITE_CH_HDR( port, SMD_RX_HDR, read_index, read_index );
    smdi_event_send_transmit( port );
  }
  else
  {
    /* In packet mode, the entire packet could not be written. */
    bytes_read = 0;
  }

  smd_release_cs_lock(port->mode.lite.rx_mutex);

  SMDL_DEBUG("smdl_readv(%p): tx_read=%d tx_write=%d", port,
      SMD_READ_CH_HDR( port, SMD_TX_HDR, read_index ),
      SMD_READ_CH_HDR( port, SMD_TX_HDR, write_index ));
  SMDL_DEBUG("smdl_readv(%p): rx_read=%d rx_write=%d", port,
      SMD_READ_CH_HDR( port, SMD_RX_HDR, read_index ),
      SMD_READ_CH_HDR( port, SMD_RX_HDR, write_index ));

  return bytes_read;
}
