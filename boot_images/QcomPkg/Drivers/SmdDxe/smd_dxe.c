/**
 * @file smd_dxe.c
 *
 * DXE driver for shared memory port device in UEFI
 */

/*==============================================================================
     Copyright (c) 2013, 2015 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/19/15   rli     Fixed Copyright banner
01/24/13   bm      Created UEFI support code for SMD
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFISmd.h>
#include "com_dtypes.h"
#include <Library/smd_lite.h>


/*===========================================================================
                        INTERNAL FUNCTION DEFINITIONS
===========================================================================*/

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
 * ends. When the other processor opens the port and asserts its Data Terminal
 * Ready (DTR) signal the \c notify callback will be called with argument
 * #SMDL_EVENT_OPEN. (The DTR signal is asserted by default when the port is
 * opened using smdl_open(), but if the other end of the port is opened using
 * the SMD memcpy API or the SIO API, the remote client must call an ioctl to
 * assert its DTR signal.) If the other end is already open and has already
 * asserted its DTR signal when this function is called, the open notification
 * callback will be called immediately, and may be called before this function
 * returns.
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
 * single edge. The same name may be used along different edges. The special
 * name "LOOPBACK" will cause the opposite end to echo data back as it is 
 * written to the port. Only one loopback port may be opened on a single edge.
 * Loopback ports are only intended for testing, not for general use.
 *
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
 * @param[out]   smdl_handle An opaque SMD handle for the opened port,
 *                           or NULL if an error occured while opening the port
 *
 * @return       EFI_SUCCESS
 *
 * @sideeffects  Allocates the local and shared data structures for the SMD
 *               port. Signals the remote processor.
 */
static EFI_STATUS EFI_smdl_open
(
  const char       *name,
  smd_channel_type  edge,
  uint32            flags,
  uint32            fsize,
  smdl_callback_t   notify,
  void             *cb_data,
  smdl_handle_type *smdl_handle
)
{
  *smdl_handle = smdl_open(name,
                           edge,
                           flags,
                           fsize,
                           notify,
                           cb_data);
  return EFI_SUCCESS;
}

/** 
 * Closes the port to the remote processor.
 *
 * Closes the port and signals the remote processor to close the port. No
 * more data may be transferred over the port after it is closed.
 *
 * After calling this function, clients must wait for
 * #SMDL_EVENT_CLOSE_COMPLETE before reopening the port.
 *
 * @param[in]    port  The port to close
 *
 * @param[out]   ret   Internal SMD Error code.
 *
 * @return       EFI_SUCESS
 *
 * @sideeffects  Writes to shared memory and signals the remote processor.
 */
static EFI_STATUS EFI_smdl_close
(
  smdl_handle_type port,
  int32            *ret
)
{
  *ret = smdl_close(port);
  return EFI_SUCCESS;
}

/** 
 * Returns the number of bytes available to be read from the port.
 *
 * Clients may use this function to quickly determine whether to read data from
 * the port, and to allocate the correct buffer size.
 *
 * @param[in]  port  The port to check for data.
 *
 * @param[out] ret   Packet mode: The number of bytes to be read from the next 
 *                   packet in the queue, or 0 if no packets are available.
 *                   Streaming mode: The number of bytes available to be read 
 *                   from the port.  (Determined by how many bytes have been 
 *                   written so far and not yet read.)
 *                   If an error occurred: A negative error code.
 *                   
 * @return      EFI_SUCCESS
 * 
 * @sideeffects  None
 */
static EFI_STATUS EFI_smdl_rx_peek
(
  smdl_handle_type port,
  int32           *ret
)
{
  *ret = smdl_rx_peek(port);
  return EFI_SUCCESS;
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
 * @param[out] ret   The length, in bytes, of largest amount of data that can 
 *                   be written to the port.
 *                   If an error occurred: A negative error code.
 *
 * @return     EFI_SUCCESS
 *
 * @sideeffects  None
 */
static EFI_STATUS EFI_smdl_tx_peek
(
  smdl_handle_type port,
  int32           *ret
)
{
  *ret = smdl_tx_peek(port);
  return EFI_SUCCESS;
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
 * This function may be called from the notification callback function
 * registered with smdl_open().
 *
 * @param[in]   port    The port to read data from
 * @param[in]   len     The number of bytes available in \c buffer
 * @param[out]  buffer  The buffer into which the data from the port should
 *                      be copied.
 * @param[in]   flags   Bitwise-or of flags controlling SMD's behavior. See
 *                      #SMDL_READ_FLAGS_NONE.
 *
 * @param[out]  ret    If data was read: The number of bytes read into 
 *                     \c buffer
 *                     If no data was available: 0
 *                     If an error occurred: A negative error code.
 *
 * @return      EFI_SUCCESS
 *
 * @sideeffects  Reads from shared memory and signals the remote processor.
 */
static EFI_STATUS EFI_smdl_read
(
  smdl_handle_type port,
  uint32           len,
  void            *buffer,
  uint32           flags,
  int32           *ret
)
{
  *ret = smdl_read(port,
                   len,
                   buffer,
                   flags);
  return EFI_SUCCESS;
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
 * This function may be called from the notification callback function
 * registered with smdl_open().
 *
 * @param[in]  port    The port to write data to
 * @param[in]  len     The number of bytes to write to the port
 * @param[in]  buffer  The buffer to write to the port
 * @param[in]  flags   Bitwise-or of flags controlling SMD's behavior. See
 *                     #SMDL_WRITE_FLAGS_NONE and #SMDL_WRITE_FLAG_NOINT.
 *
 * @param[out] ret     Return behavior depends on the transfer mode used.
 *                     -Packet mode: 
 *                      -If the port has sufficient space to hold the 
 *                       provided packet: The length of the packet written to 
 *                       the port.
 *                      -If the port does not have sufficient space to 
 *                       hold the provided packet: 0.
 *                     -Streaming mode: The number of bytes that were 
 *                      successfully written to the port. If truncated, will 
 *                      be less than \c len.
 *                    If an error occurred: A negative error code.
 *
 * @return     EFI_SUCCESS
 *
 * @sideeffects  Writes to shared memory and signals the remote processor.
 */
static EFI_STATUS EFI_smdl_write
(
  smdl_handle_type port,
  uint32           len,
  const void      *buffer,
  uint32           flags,
  uint32          *ret
)
{
  *ret = smdl_write(port,
                    len,
                    buffer,
                    flags);
  return EFI_SUCCESS;
}

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
 * @param[out]  ret     If data was read: The total number of bytes read into 
 *                      \c iovec.
 *                      If no data was available: 0
 *                      If #SMDL_READ_FLAG_LONG is specified in packet mode and
 *                      the whole packet was not read: #SMDL_CONTINUE
 *                      If an error occurred: A negative error code.
 *
 * @return      EFI_SUCCESS
 *
 * @sideeffects  Reads from shared memory and signals the remote processor.
 */
static EFI_STATUS EFI_smdl_readv
(
  smdl_handle_type port,
  smdl_iovec_type *iovec,
  uint32           flags,
  uint32          *ret  
)
{
  *ret = smdl_readv(port,
                    iovec,
                    flags);
  return EFI_SUCCESS;
}

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
 * This function may be called from the notification callback function
 * registered with smdl_open().
 *
 * @param[in]  port    The port to write data to
 * @param[in]  iovec   Pointer to a linked list of I/O vectors to be written
 *                     to the port.
 * @param[in]  flags   Bitwise-or of flags controlling SMD's behavior. See
 *                     #SMDL_WRITE_FLAGS_NONE and #SMDL_WRITE_FLAG_NOINT.
 *
 * @param[out] ret     Return behavior depends on the transfer mode used.
 *                     -If #SMDL_WRITE_FLAG_LONG is specified:
 *                      -If the whole message has been written: The size of the
 *                       message.
 *                     -If the whole message has not been written: 
 *                      #SMDL_CONTINUE.
 *                    -Packet mode: 
 *                     -If the port has sufficient space to hold the 
 *                      provided packet: The length of the packet written to 
 *                      the port.
 *                     -If the port does not have sufficient space to 
 *                      hold the provided packet: 0.
 *                    -Streaming mode: The number of bytes that were 
 *                     successfully written to the port. If truncated, will
 *                     be less than the total amount of data available in the 
 *                     vector.
 *                    If an error occurred: A negative error code.
 *
 * @return     EFI_SUCCESS
 *
 * @sideeffects  Writes to shared memory and signals the remote processor.
 */
static EFI_STATUS EFI_smdl_writev
(
  smdl_handle_type       port,
  const smdl_iovec_type *iovec,
  uint32                 flags,
  uint32                *ret  
)
{
  *ret = smdl_writev(port,
                     iovec,
                     flags);
  return EFI_SUCCESS;
}

/**
 * Set or clear an RS-232 signal. 
 *
 * @param[in]    port    The port to set or clear the signal from.
 * @param[in]    sig     The type of the signal.  Can be DTR/DSR or CTS/RTS.
 * @param[in]    value   Must be 1 for set, or 0 for clear. No other values are
 *                       allowed.
 *
 * @param[out]   ret     0 if successful.
 *                       -1 if \c port is invalid or an error occurred.
 *
 * @return       EFI_SUCCESS
 * 
 * @sideeffects  Writes the signal to shared memory and signals the remote 
 *               processor.
 */
static EFI_STATUS EFI_smdl_sig_set
(
  smdl_handle_type port,
  smdl_sig_type    sig,
  boolean          value,
  uint32          *ret    
)
{
  *ret = smdl_sig_set(port,
                      sig,
                      value);
  return EFI_SUCCESS;
}

/**
 * Query the value of an RS-232 signal. 
 *
 * @param[in]    port    The port to query the signal from.
 * @param[in]    sig     The type of the signal.  Can be DTR/DSR or CTS/RTS.
 *
 * @param[out]   ret     The value of the signal (0 or 1) if successful.
 *                       -1 if \c port is invalid or an error occurred.
 *
 * @return       EFI_SUCCESS
 */
static EFI_STATUS EFI_smdl_sig_get
(
  smdl_handle_type port,
  smdl_sig_type    sig,
  uint32          *ret    
)
{
  *ret = smdl_sig_get(port,
                      sig);
  return EFI_SUCCESS;
}


/**
 *  Specifies whether the port with the given name and channel_type has already
 *  been opened by any processor.
 *  
 *  This function doesn't differentiate based on which processor(s) opened the 
 *  port.  It also works whether or not the local host is coincident with the 
 *  channel_type.
 *
 *  @param[in]  name           Name of the port to look for.
 *  @param[in]  channel_type   Channel type of the port to look for.
 *  @param[out] *ret           1 If channel is open, 0 otherwise.
 *
 *  @return     EFI_SUCCESS
 */
static EFI_STATUS EFI_smd_is_port_open
(
  const char       *name,
  smd_channel_type  channel_type,
  uint32           *ret
)
{
  *ret = smd_is_port_open(name,
                          channel_type);
  return EFI_SUCCESS;
}

/*===========================================================================
                           INTERNAL DATA STRUCTURES
===========================================================================*/

static EFI_SMD_PROTOCOL SMDProtocol =
{
  EFI_SMD_PROTOCOL_REVISION,
  EFI_smdl_open,
  EFI_smdl_close,
  EFI_smdl_rx_peek,
  EFI_smdl_tx_peek,
  EFI_smdl_read,
  EFI_smdl_write,
  EFI_smdl_readv,
  EFI_smdl_writev,
  EFI_smdl_sig_set,
  EFI_smdl_sig_get,
  EFI_smd_is_port_open
};

/*===========================================================================
                        EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/
extern void smd_initialize_static(void);

/**
 * Entry point for the SMD DXE driver.
 */
EFI_STATUS EFIAPI SmdEntryPoint
(
  EFI_HANDLE            ImageHandle,
  EFI_SYSTEM_TABLE     *SystemTable
)
{
  EFI_HANDLE handle = NULL;
  EFI_STATUS status;

  /* TODO: initialize SMD drivers here */
  smd_initialize_static();

  status = gBS->InstallMultipleProtocolInterfaces(&handle,
      &gEfiSMDProtocolGuid, (void **)&SMDProtocol, NULL, NULL, NULL );

  return status;
}
