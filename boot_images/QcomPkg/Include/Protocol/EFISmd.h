/** 
  @file  EFISd.h
  @brief Shared Memory Protocol for UEFI.
*/
/*=============================================================================
  Copyright (c) 2011-2015 Qualcomm Technologies, Incorporated.
  All rights reserved.
  Qualcomm Technologies, Confidential and Proprietary.
=============================================================================*/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     ----------------------------------------------------------
 11/02/15   rli     Branched from WP.UEFI.3.2 PW
 11/20/13   llg     (Tech Pubs) Edited function syntax; otherwise, Doxygen will not run
 06/19/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
 01/31/13   bm      Initial version for SMD Lite lib.

=============================================================================*/
#ifndef __EFISMD_H__
#define __EFISMD_H__

/*===========================================================================
  INCLUDE FILES
===========================================================================*/
#include "smd_type.h"

/*===========================================================================
  MACRO DECLARATIONS
===========================================================================*/

/** @ingroup efi_smd_constants
  Protocol version.
*/
#define EFI_SMD_PROTOCOL_REVISION 0x0000000000010001

/*  Protocol GUID definition */
/** @ingroup efi_smd_protocol */
#define EFI_SMD_PROTOCOL_GUID \
  { 0xb98ff495, 0x55ca, 0x403d, { 0x8c, 0x7a, 0x20, 0xe7, 0x42, 0x20, 0x93, 0x19 } }

/** @cond */
/*===========================================================================
  EXTERNAL VARIABLES
===========================================================================*/
/*
  External reference to the PMIC SMD Protocol GUID.
 */
extern EFI_GUID gEfiSMDProtocolGuid;
/** @endcond */

/*===========================================================================
  TYPE DEFINITIONS
===========================================================================*/
/* EFI_SMDL_OPEN */
/** @ingroup efi_smdl_open
  @par Summary
  Opens a port to the specified remote processor.
 
  @param[in]    name     Name for the port.
  @param[in]    edge     Edge, identifying both processors, on which this
                         channel communicates. The edge must include the
                         current processor. See #smd_channel_type for 
                         details.
  @param[in]    flags    Bitfield of flags controlling the behavior of the
                         port. See: \n
                         - #SMDL_OPEN_FLAGS_NONE
                         - #SMDL_OPEN_FLAGS_PRIORITY_DEFAULT
                         - #SMDL_OPEN_FLAGS_PRIORITY_LOW
                         - #SMDL_OPEN_FLAGS_PRIORITY_HIGH
                         - #SMDL_OPEN_FLAGS_PRIORITY_IST
                         - #SMDL_OPEN_FLAGS_MODE_PACKET
                         - #SMDL_OPEN_FLAGS_MODE_STREAMING
                         @tablebulletend 
  @param[in]    fsize    Size of the FIFO. This is the physical
                         size in memory, which is always four bytes greater 
                         than the total capacity of the FIFO. In Packet mode, 
                         the SMD packet header adds 20 bytes per packet, 
                         further reducing the capacity. The FIFO size must be
                         a multiple of 32 and must not be less than 
                         #SMD_MIN_FIFO or greater than #SMD_MAX_FIFO. \n
                         @vertspace{3}
                         If this parameter is 0, the default size 
                         #SMD_STANDARD_FIFO is used.
  @param[in]    notify   Callback that is called when SMD needs to notify the 
                         client of a remote event on the SMD port; see 
                         #smdl_callback_t for details.
  @param[in]    cb_data      Data passed to the notification callback function.
  @param[out]   smdl_handle  Opaque SMD handle for the opened port, 
                             or NULL if an error occurred while opening the 
                             port.
 
  @newpage
  @par Description 
  This function opens the port named <i>name</i> between the processors
  specified by <i>edge</i>. (The current processor must be one of the two
  processors connected by <i>edge</i>.) The function allocates the data
  structures for the ports in local and shared memory and allocates FIFOs
  for receive and transmit, each of size <i>fsize</i>.
  @par 
  Clients may not read from or write to the port until it is opened on both
  ends. When the other processor opens the port and asserts its Data Terminal
  Ready (DTR) signal, the <i>notify</i> callback is called with argument
  #SMDL_EVENT_OPEN. (The DTR signal is asserted by default when the port is
  opened using #EFI_SMDL_OPEN(), but if the other end of the port is opened 
  using the SMD memcpy API or the SIO API, the remote client must call an ioctl 
  to assert its DTR signal.) If the other end is already open and has already
  asserted its DTR signal when this function is called, the open notification
  callback is called immediately, and may be called before this function
  returns.
  @par
  If a client wants to know when the SMD port is fully open, and the other
  end asserts its DTR signal when opening the port, the client waits
  for #SMDL_EVENT_OPEN. If a client wants to know when the SMD port is fully
  open, and the other end does not assert its DTR signal when opening the
  port, the client waits for #SMDL_EVENT_CTS, aware that the
  other end may set and clear its Clear To Send (CTS) signal to enable and
  disable inbound flow during normal operation. If a client does not need to
  know when the port is open, and only wants to transmit data, the client
  sends data after opening the port. If the port is fully open, SMD
  Lite transmits the data immediately. If the port is not fully open, SMD
  Lite notifies the client when data can be written (when the remote
  processor asserts its CTS signal) by sending #SMDL_EVENT_WRITE.
  @par
  Port names uniquely identify the port connecting two processors across a
  single edge. The same name may be used along different edges. The special
  name ``LOOPBACK'' causes the opposite end to echo data back as it is 
  written to the port. Only one loopback port may be opened on a single edge.
  Loopback ports are only intended for testing, not for general use.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
  for additional possible returns.
 
  @sideeffects
  The function allocates the local and shared data structures for the SMD
  port. \n
  The function signals the remote processor.
*/
typedef EFI_STATUS (EFIAPI* EFI_SMDL_OPEN)(
  const char       *name,
  smd_channel_type  edge,
  uint32            flags,
  uint32            fsize,
  smdl_callback_t   notify,
  void             *cb_data,
  smdl_handle_type *smdl_handle
);

/* EFI_SMDL_CLOSE */
/** @ingroup efi_smdl_close
  @par Summary
  Closes the port to the remote processor.
 
  @param[in]    port  Port to close.
  @param[out]   ret   -1 if <i>port</i> is NULL; otherwise, 0.
 
  @par Description
  This function closes the port and signals the remote processor to close the 
  port. No more data may be transferred over the port after it is closed.
  @par
  After calling this function, clients must wait for
  #SMDL_EVENT_CLOSE_COMPLETE before reopening the port.
 
  @return
  EFI_SUCCESS -- Function completed successfully.
 
  @sideeffects
  The function writes to shared memory and signals the remote processor.
*/
typedef EFI_STATUS (EFIAPI* EFI_SMDL_CLOSE)(
  smdl_handle_type port,
  int32           *ret
);

/* EFI_SMDL_RX_PEEK */
/** @ingroup efi_smdl_rx_peek
  @par Summary
  Returns the number of bytes available to be read from the port.
  @par
  Clients may use this function to quickly determine whether to read data from
  the port, and to allocate the correct buffer size.
 
  @param[in]  port  Port to check for data.
  @param[out] ret   Packet mode: The number of bytes to be read from the next 
                    packet in the queue, or 0 if no packets are available. \n
                    Streaming mode: The number of bytes available to be read 
                    from the port. (This is determined by how many bytes have 
                    been written thus far and not yet read.) \n
                    If an error occurred: A negative error code.
                    
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
  for additional possible returns.
*/
typedef EFI_STATUS (EFIAPI* EFI_SMDL_RX_PEEK)(
  smdl_handle_type port,
  int32            *ret
);

/* EFI_SMDL_TX_PEEK */
/** @ingroup efi_smdl_tx_peek
  @par Summary
  Returns the number of bytes that can be written to a port.
  @par
  Clients may use this function to quickly determine whether the packet they
  want to send will fit in the outgoing buffer.
 
  @param[in]  port  Port to check for data.
  @param[out] ret   Length, in bytes, of largest amount of data that can 
                    be written to the port. \n
                    If an error occurred: A negative error code.
 
  @par Description
  When the port is not fully opened, or the receiver has disabled inbound
  flow, this function returns 0 because no data can be written to the port.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
  for additional possible returns.
*/
typedef EFI_STATUS (EFIAPI* EFI_SMDL_TX_PEEK)(
  smdl_handle_type port,
  int32           *ret
);

/* EFI_SMDL_READ */
/** @ingroup efi_smdl_read
  @par Summary
  Reads data from the SMD port into the provided buffer.
 
  @param[in]   port    Port from which to read the data.
  @param[in]   len     The number of bytes available in <i>buffer</i>.
  @param[out]  buffer  Buffer into which the data from the port is to 
                       be copied.
  @param[in]   flags   Bitwise-OR of flags controlling SMD's behavior. See
                       #SMDL_READ_FLAGS_NONE.
  @param[out]  ret    If data was read: The number of bytes read into 
                       <i>buffer</i>. \n
                       If no data was available: 0. \n
                      If an error occurred: A negative error code.
 
  @par Description
  Packet mode: \n
  - This function reads up to <i>len</i> bytes of the next packet in the 
    port into the provided buffer. If the packet is larger than the buffer, 
    as much data as possible is copied to the buffer and the remainder 
    is discarded. Clients concerned about dropping data can call 
    EFI_SMDL_RX_PEEK() to determine the size of the next packet.
  @par
  Streaming mode: \n
  - This function reads up to <i>len</i> bytes from the port into the 
    provided buffer, limited by the value returned by EFI_SMDL_RX_PEEK(). 
    Any remaining bytes in the port are left there for future reads.
  @par
  For both modes: \n
  - This function does not block; it returns 0 immediately if no data is
    available. \n
  - This function may be called from the notification callback function
    registered with EFI_SMDL_OPEN().

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
  for additional possible returns.
 
  @sideeffects
  The function reads from shared memory and signals the remote processor.
*/
typedef EFI_STATUS (EFIAPI *EFI_SMDL_READ)(
  smdl_handle_type port,
  uint32           len,
  void            *buffer,
  uint32           flags,
  int32           *ret
);

/* EFI_SMDL_WRITE */
/** @ingroup efi_smdl_write
  @par Summary
  Writes data from the provided buffer to the SMD port.
 
  @param[in]  port    Port to which to write the data.
  @param[in]  len     The number of bytes to write to the port.
  @param[in]  buffer  Buffer to write to the port.
  @param[in]  flags   Bitwise-OR of flags controlling SMD's behavior. See
                      #SMDL_WRITE_FLAGS_NONE and #SMDL_WRITE_FLAG_NOINT.
  @param[out] ret     Return behavior depends on the transfer mode used. \n
                      - Packet mode: \n
                        -- If the port has sufficient space to hold the 
                           provided packet: The length of the packet written 
                           to the port. \n
                        -- If the port does not have sufficient space to 
                        hold the provided packet: 0.
                      -Streaming mode: The number of bytes that were 
                        successfully written to the port. If truncated, this 
                        is less than <i>len</i>.
                      \n
                      If an error occurred: A negative error code.
 
  @par Description
  Packet mode: \n
  - The buffer is treated as a single packet to be sent to the remote 
    processor. The packet is sent in its entirety if space is available in 
    the outgoing FIFO, or it is dropped (with an appropriate error 
    code) if space is not available. Attempting to send packets larger than 
    the size of the outgoing FIFO always fails.
  @par
  Streaming mode: \n
  - Up to <i>len</i> bytes from <i>buffer</i> are written into the 
    port, limited by the value returned by EFI_SMDL_TX_PEEK(). This function 
    returns the number of successfully written bytes. Any bytes that cannot 
    fit in the outgoing FIFO are truncated.
  @newpage
  @par
  For both modes: \n
  - Writing zero bytes to the port does not change shared memory and does not
    signal the remote processor. \n
  - Clients that want to determine the maximum packet size that can be
    written to the port can call EFI_SMDL_TX_PEEK(). \n
  - This function may be called from the notification callback function
    registered with EFI_SMDL_OPEN().

  @return
  EFI_SUCCESS -- Function completed successfully. \n
  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
  for additional possible returns.
 
  @sideeffects
  The function writes to shared memory and signals the remote processor.
*/
typedef EFI_STATUS (EFIAPI *EFI_SMDL_WRITE)(
  smdl_handle_type port,
  uint32           len,
  const void      *buffer,
  uint32           flags,
  uint32          *ret      
);

/* EFI_SMDL_READV */
/** @ingroup efi_smdl_readv
  @par Summary
  Reads data from the SMD port into the provided vector.
  
  @param[in]   port    Port from which to read the data.
  @param[out]  iovec   Pointer to a linked list of I/O vectors into which 
                       to copy the packet. See #smdl_iovec_struct for details.
  @param[in]   flags   Bitwise-OR of flags controlling SMD's behavior. See
                       #SMDL_READ_FLAGS_NONE and #SMDL_READ_FLAG_LONG.
  @param[out]  ret     If data was read: The total number of bytes read into 
                       <i>iovec</i>. \n
                       If no data was available: 0. \n
                       If #SMDL_READ_FLAG_LONG is specified in Packet mode and
                       the entire packet was not read: #SMDL_CONTINUE. \n
                       If an error occurred: A negative error code.
 
  @par Description
  Packet mode: \n
  - This function reads the next packet in the port into the
    provided buffer or buffers in the linked list of vectors <i>iovec</i>. Each
    element of the I/O vector describes a buffer size and length and a pointer
    to the next buffer in the list. This function copies as much of the
    packet as possible into the first buffer in the chain, then continues
  copying data until either the packet is complete or the buffers are
  exhausted. If the packet is larger than the vector, as much data as
    possible is copied to the vector and the remainder is discarded.
    Clients concerned about dropping data can call EFI_SMDL_RX_PEEK() to
    determine the size of the next packet. \n
  - If the packet to be received is larger than the FIFO (specified when
    opening the port using EFI_SMDL_OPEN()), and the #SMDL_READ_FLAG_LONG flag 
    in <i>flags</i> is not set, this function reads as much of the packet as is
    possible in a single read, then discards the remainder of the packet. If 
    the #SMDL_READ_FLAG_LONG flag is set, this function reads as much data as
    is available at a time, then returns #SMDL_CONTINUE if more data must be
    received before the read is complete. The client is responsible for waiting
  until it receives the #SMDL_EVENT_READ event callback, at which point the
    client must call %EFI_SMDL_READV() again, using the same arguments. The
    function reads as much data as is possible, repeating as many times as
    is necessary to read the entire packet, and ultimately returns the size of
  the entire packet when the entire packet has been read.
  @par
  Streaming mode: \n
  - This function reads as much data as will fit into the provided linked 
    list of vectors <i>iovec</i> from the port into the provided buffer, limited 
    by the value returned by EFI_SMDL_RX_PEEK(). Any remaining bytes in the 
    port are left there for future reads. \n
  - Setting the #SMDL_READ_FLAG_LONG flag when a port has been opened in
    Streaming mode has no effect. This function always read as much data
    as is available and returns the number of bytes read.
  @par
  For both modes: \n
  - This function does not block; it returns 0 immediately if no data is
    available. \n
  - This function may be called from the notification callback function
    registered with EFI_SMDL_OPEN().
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
  for additional possible returns.
 
  @sideeffects  
  The function reads from shared memory and signals the remote processor.
*/
typedef EFI_STATUS (EFIAPI *EFI_SMDL_READV)(
  smdl_handle_type port,
  smdl_iovec_type *iovec,
  uint32           flags,
  uint32          *ret      
);

/* EFI_SMDL_WRITEV */
/** @ingroup efi_smdl_writev
  @par Summary
  Writes data from the provided vector to the SMD port.
 
  @param[in]  port    Port to which to write the data.
  @param[in]  iovec   Pointer to a linked list of I/O vectors to be written
                      to the port. See #smdl_iovec_struct for details.
  @param[in]  flags   Bitwise-OR of flags controlling SMD's behavior. See
                      #SMDL_WRITE_FLAGS_NONE and #SMDL_WRITE_FLAG_NOINT.
  @param[out] ret     Return behavior depends on the transfer mode used. \n
                      - If #SMDL_WRITE_FLAG_LONG is specified: \n
                        -- If the entire message has been written: The size 
                           of the message. \n
                        -- If the entire message has not been written: 
                       #SMDL_CONTINUE.
                      - Packet mode: \n
                        -- If the port has sufficient space to hold the 
                           provided packet: The length of the packet written 
                           to the port. \n
                        -- If the port does not have sufficient space to 
                       hold the provided packet: 0.
                     -Streaming mode: The number of bytes that were 
                        successfully written to the port. If truncated, 
                        this is less than the total amount of data available 
                        in the vector.
                      \n
                     If an error occurred: A negative error code.
 
  @par Description
  Packet mode: \n
  - The vector is treated as a single packet to be sent to the remote 
    processor. By default, the packet is sent in its entirety if space is 
    available in the outgoing FIFO, or it is dropped (with an appropriate 
    error code) if space is not available.
  @newpage
  @par
  Streaming mode: \n
  - This function writes as much of the provided vector as is possible, 
    limited by the value returned by EFI_SMDL_TX_PEEK(). This function 
    returns the number of successfully written bytes. Any bytes that cannot 
    fit in the outgoing FIFO are truncated.
  @par
  For both modes: \n
  - Clients may specify the
    #SMDL_WRITE_FLAG_LONG bit of <i>flags</i> to enable writing messages longer
    than the FIFO. In this mode, %EFI_SMDL_WRITEV() writes as much of the
    message as is possible, then returns #SMDL_CONTINUE. The client is
    responsible for waiting until it receives the #SMDL_EVENT_WRITE event
    callback, at which point the client must call %EFI_SMDL_WRITEV() again,
    using the same arguments. The function again writes as much data as
    can be written. When %EFI_SMDL_WRITEV() has written the entire message,
    it returns the size of the entire message. \n
  - Writing zero bytes to the port does not change shared memory and does not
    signal the remote processor. \n
  - Clients that want to determine the maximum packet size that can be
    written to the port (without specifying #SMDL_WRITE_FLAG_LONG) can call
    EFI_SMDL_TX_PEEK(). \n
  - This function may be called from the notification callback function
    registered with EFI_SMDL_OPEN().
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
  for additional possible returns.
 
  @sideeffects
  The function writes to shared memory and signals the remote processor.
*/
typedef EFI_STATUS (EFIAPI* EFI_SMDL_WRITEV)(
  smdl_handle_type       port,
  const smdl_iovec_type *iovec,
  uint32                 flags,
  uint32                *ret
);

/* EFI_SMDL_SIG_SET */
/** @ingroup efi_smdl_sig_set
  @par Summary
  Sets or clears an RS-232 signal. 
  
  @param[in]    port    Port from which to set or clear the signal.
  @param[in]    sig     Type of signal. This can be DTR/DSR or CTS/RTS.
  @param[in]    value   Must be 1 for set, or 0 for clear. No other values are
                        allowed.
  @param[out]   ret     0 if successful. \n
                        -1 if <i>port</i> is invalid or an error occurred.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
  for additional possible returns.
 
  @sideeffects
  The function writes the signal to shared memory and signals the remote 
                processor.
*/
typedef EFI_STATUS (EFIAPI* EFI_SMDL_SIG_SET)(
  smdl_handle_type port,
  smdl_sig_type    sig,
  boolean          value,
  uint32          *ret  
);

/* EFI_SMDL_SIG_GET */
/** @ingroup efi_smdl_sig_get
  @par Summary
  Queries the value of an RS-232 signal. 
 
  @param[in]    port    Port from which to query the signal.
  @param[in]    sig     Type of signal. This can be DTR/DSR or CTS/RTS.
  @param[out]   ret     Value of the signal (0 or 1) if successful. \n
                        -1 if <i>port</i> is invalid or an error occurred.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
  for additional possible returns.
*/
typedef EFI_STATUS (EFIAPI* EFI_SMDL_SIG_GET)(
  smdl_handle_type port,
  smdl_sig_type    sig,
  uint32          *ret  
);

/* EFI_SMDL_IS_PORT_OPEN */
/** @ingroup efi_smdl_is_port_open
  @par Summary
   Specifies whether the port with the specified name and channel type has 
   already been opened by any processor.
   @par
   This function does not differentiate based on which processor opened the 
   port.  It also works whether or not the local host is coincident with the 
   channel type.
 
   @param[in]  name           Name of the port for which to look.
   @param[in]  channel_type   Channel type of the port. See #smd_channel_type 
                              for details.
   @param[out] ret            1 if the channel is open; otherwise, 0.
 
  @return
  EFI_SUCCESS -- Function completed successfully. \n
  See @xhyperref{hdr:funcRetErrorCodes,Function return error codes} 
  for additional possible returns.
*/
typedef EFI_STATUS (EFIAPI* EFI_SMDL_IS_PORT_OPEN)(
  const char       *name,
  smd_channel_type  channel_type,
  uint32           *ret
);

/*===========================================================================
  PROTOCOL INTERFACE
===========================================================================*/
/** @ingroup efi_smd_protocol
  @par Summary
  Shared Memory Driver (SMD) Protocol interface.

  @par Parameters
  @inputprotoparams{smd_proto_params.tex}
*/
typedef struct _EFI_SMD_PROTOCOL {
  UINT64                Revision;
  EFI_SMDL_OPEN         SmdlOpen;
  EFI_SMDL_CLOSE        SmdlClose;
  EFI_SMDL_RX_PEEK      SmdlRxPeek;
  EFI_SMDL_TX_PEEK      SmdlTxPeek;
  EFI_SMDL_READ         SmdlRead;
  EFI_SMDL_WRITE        SmdlWrite;
  EFI_SMDL_READV        SmdlReadv;
  EFI_SMDL_WRITEV       SmdlWritev;
  EFI_SMDL_SIG_SET      SmdlSigSet;
  EFI_SMDL_SIG_GET      SmdlSigGet;
  EFI_SMDL_IS_PORT_OPEN SmdlIsPortOpen;
}EFI_SMD_PROTOCOL;

#endif /* EFISMD_H */
