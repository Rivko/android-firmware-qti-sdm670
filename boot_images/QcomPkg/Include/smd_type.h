#ifndef SMD_TYPE_H
#define SMD_TYPE_H

/**
 * @file smd_type.h
 *
 * Public data types and definitions used by SMD
 */

/*==============================================================================
     Copyright (c) 2011-2014 Qualcomm Technologies, Inc. 
     All rights reserved.
     Qualcomm Confidential and Proprietary
==============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/19/13   llg     (Tech Pubs) Edited/added Doxygen comments and markup.
02/11/13   bm      Move type definitions to smd_type.h
08/30/12   pa      Added SMD_STATUS_SIZE_MISMATCH.
08/22/12   bt      Move SMD error codes to smd_type.h.
01/20/12   bm      Add PROC(n)_RPM edges.
10/31/11   bm      Moved SMD_CHANNEL_NAME_SIZE_MAX to smd_type.h to 
                   expose it as a public macro.
05/03/11   tl      Created separate header file for SMD data types
===========================================================================*/

#include "com_dtypes.h"
/*-------------------------------------------------------------------------
  Defines
-------------------------------------------------------------------------*/

/** @cond */
/** SMD_CHANNEL_NAME_SIZE_MAX must be a multiple of 4 for structure
   alignment in shared memory. */ 
#define SMD_CHANNEL_NAME_SIZE_MAX    20
/** @endcond */

/** @addtogroup efi_smd_constants 
@{ */
/** Standard size of a small FIFO buffer (1 KB). */
#define SMD_MIN_FIFO       (1 * 1024) 

/** Default FIFO size for an SMD buffer (8 KB). */
#define SMD_STANDARD_FIFO  (8 * 1024)

/** Standard size of a large FIFO buffer (128 KB). This is the largest size 
  permitted by SMD. */
#define SMD_MAX_FIFO       (128 * 1024)

/** @name Function return error codes
    @xreflabel{hdr:funcRetErrorCodes}
  These are the error codes returned by some SMD public functions. The client 
  can use these error codes to determine the success of the function or what 
  went wrong.
@{ */
/** Function succeeded. */
#define SMD_STATUS_SUCCESS        0
/** Generic error. */
#define SMD_STATUS_ERROR          -1
/** Invalid argument was passed in. */
#define SMD_STATUS_INVALID_PARAM  -2
/** SMD has not been initialized enough to handle this command. */
#define SMD_STATUS_NOT_INIT       -3
/** Requested port size does not match existing port size. */
#define SMD_STATUS_SIZE_MISMATCH  -4
/** @} */ /* end_name_group Function return error codes */
/** @} */ /* end_addtogroup efi_smd_constants */

/*--------------------------------------------------------------------------
  Enumerated types
---------------------------------------------------------------------------*/

/* smd_channel_type */
/** @ingroup efi_smd_data_types
  SMD channel connection direction.
*/
typedef enum
{
  /** @cond */
  SMD_CHANNEL_TYPE_FIRST,
  /** @endcond */
  SMD_APPS_MODEM = SMD_CHANNEL_TYPE_FIRST,
  /**< Applications to modem. */
  SMD_APPS_QDSP,
  /**< Applications to Hexagon@tm DSP. */
  SMD_MODEM_QDSP,
  /**< Modem to Hexagon DSP. */
  SMD_APPS_DSPS,
  /**< Applications to Dedicated Sensors Processor Subsystem (DSPS). */
  SMD_MODEM_DSPS,
  /**< Modem to DSPS. */
  SMD_QDSP_DSPS,
  /**< Hexagon DSP to DSPS. */
  SMD_APPS_RIVA,
  /**< Applications to RIVA. */
  SMD_MODEM_RIVA,
  /**< Modem to RIVA. */
  SMD_QDSP_RIVA,
  /**< Hexagon DSP to RIVA. */
  SMD_DSPS_RIVA,
  /**< DSPS to RIVA. */
  SMD_APPS_Q6FW,
  /**< Applications to Hexagon firmware. */
  SMD_MODEM_Q6FW,
  /**< Modem to Hexagon firmware. */
  SMD_QDSP_Q6FW,
  /**< Hexagon DSP to Hexagon firmware. */
  SMD_DSPS_Q6FW,
  /**< DSPS to Hexagon firmware. */
  SMD_RIVA_Q6FW,
  /**< RIVA to Hexagon firmware. */
  SMD_APPS_RPM,
  /**< Applications to Resource Power Manager (RPM). */
  SMD_MODEM_RPM,
  /**< Modem to RPM. */
  SMD_QDSP_RPM,
  /**< Hexagon DSP to RPM. */
  SMD_RIVA_RPM,
  /**< RIVA to RPM. */
  /** @cond */
  SMD_CHANNEL_TYPE_LAST,
  /** @endcond */
  SMD_INVALID_CHANNEL_TYPE
  /**< Invalid channel type. @newpage */
} smd_channel_type;

/** @cond */

/*===========================================================================
                      MACRO DECLARATIONS
===========================================================================*/

/**
  This preprocessor symbol can be used to test for the minor differences
  between SMD Lite API v1 and SMD Lite API v2 at compile time. The only
  difference is smdl_open(): In v2, the argument smd_channel_type edge has
  been added.
 */
#define SMD_LITE_API_V2

/*===========================================================================
                      TYPE DECLARATIONS
===========================================================================*/

/**
  Internal data structure for SMD Lite ports.
 
  This structure should not be used directly by clients; they should treat
  smdl_handle_type as an opaque handle.
 */
struct smd_stream_info_struct;

/**
  Opaque handle pointing to the internal data structures.
 
  Clients must not take advantage of the internal structure of this handle.
 */
typedef struct smd_stream_info_struct * smdl_handle_type;
/** @endcond */

/** @ingroup efi_smd_data_types
  I/O vector type.  Clients using the vector read and write functions, 
  EFI_SMDL_READV() and EFI_SMDL_WRITEV(), must create a structure of this 
  type for each buffer that makes up the vector.
 */
typedef struct smdl_iovec_struct {
  /** Pointer to the next I/O vector structure, or NULL if this is the last
      buffer in the chain. */
  struct smdl_iovec_struct *next;

  /** Length, in bytes, of this element of the vector. */
  uint32  length;

  /** Data pointer of this element of the vector. */
  void   *buffer;
} smdl_iovec_type;

/** @ingroup efi_smd_data_types
  Notification events that clients receive from SMD. */
typedef enum
{
  /** No event. */
  SMDL_EVENT_NONE = 0,

  /** Called when the remote processor asserts its Data Terminal Ready (DTR)
      signal, which normally means that the port is open. See EFI_SMDL_OPEN(). */
  SMDL_EVENT_OPEN,

  /** Called when data is available to be read from the FIFO. \n
      \vertspace{4}
      SMD Lite sends this event to the client when there is new data
      available to be read, but does not explicitly notify the client for each
      packet received. When clients receive this event, they must read as much
      data as is available by calling %EFI_SMDL_READ() or %EFI_SMDL_READV() in 
      a loop to avoid leaving data in the receive FIFO when multiple packets 
      are received at the same time. \n
      \vertspace{4}
      See EFI_SMDL_READ(), EFI_SMDL_READV(), and EFI_SMDL_RX_PEEK(). */
  SMDL_EVENT_READ,

  /** Called when space is available in the FIFO after a failed write.
      See EFI_SMDL_WRITE() and EFI_SMDL_WRITEV(). */
  SMDL_EVENT_WRITE,

  /** Called when SMD Lite has finished closing the port after the client
      calls %EFI_SMDL_CLOSE(). After the client receives this event, the port 
      can be immediately reopened. See EFI_SMDL_CLOSE(). */
  SMDL_EVENT_CLOSE_COMPLETE,

  /** Called when SMD Lite has finished closing the port after the client
      calls %EFI_SMDL_CLOSE(). After the client receives this event, the port 
      can be immediately reopened. This event is provided for backward 
      compatability with existing code. See EFI_SMDL_CLOSE(). */
  SMDL_EVENT_CLOSE = SMDL_EVENT_CLOSE_COMPLETE,
  
  /** Event generated when the remote processor changes its DTR signal. \n
      \vertspace{4}
      Most SMD clients use the DTR signal to indicate that they have opened 
      the port and are ready to transmit and receive data. EFI_SMDL_OPEN() 
      asserts the DTR signal when the port is opened, but clients using the 
      SMD memcpy API or the SIO API must call an ioctl to assert the DTR 
      signal. \n
      \vertspace{4}
      When receiving this event, clients may use %EFI_SMDL_SIG_GET() to query 
      the current state of the remote processor's DTR signal. Reading the 
      remote processor's DTR signal is considered equivalent to reading the 
      channel's Data Set Ready (DSR) signal. \n
      \vertspace{4}
      See EFI_SMDL_SIG_GET() and EFI_SMDL_SIG_SET(). */
  SMDL_EVENT_DSR,
  
  /** Event generated when the remote processor changes its Clear To Send
      (CTS) signal. \n
      \vertspace{4}
      Before SMD can transmit data on a channel, the remote processor must
      assert its CTS signal, which is referred to as ``enabling inbound flow.''
      EFI_SMDL_OPEN() asserts the CTS signal when the port is opened, but 
      clients using the SMD memcpy API or the SIO API must call an ioctl to 
      assert the CTS signal. \n
      \vertspace{4}
      When receiving this event, clients may use %EFI_SMDL_SIG_GET() to query 
      the current state of the remote processor's CTS signal. Reading the 
      remote processor's CTS signal is considered equivalent to reading the 
      channel's Ready To Send (RTS) signal. \n
      \vertspace{4}
      See EFI_SMDL_SIG_GET() and EFI_SMDL_SIG_SET(). @newpage */
  SMDL_EVENT_CTS,
  
  /** Event generated when the remote processor changes its Carrier Detect
      (CD) signal. \n
      \vertspace{4}
      When receiving this event, clients may use %EFI_SMDL_SIG_GET() to query 
      the current state of the remote processor's CD signal. \n
      \vertspace{4}
      SMD does not use the CD signal but does allow clients to use the legacy
      RS-232 signal. EFI_SMDL_OPEN() asserts the CD signal when the port is 
      opened, but clients using the SMD memcpy API or the SIO API must call 
      an ioctl to assert the CD signal. \n
      \vertspace{4}
      See EFI_SMDL_SIG_GET() and EFI_SMDL_SIG_SET(). */
  SMDL_EVENT_CD,
  
  /** Event generated when the remote processor changes its Ring Indicator
      (RI) signal. \n
      \vertspace{4}
      When receiving this event, clients may use %EFI_SMDL_SIG_GET() to query 
      the current state of the remote processor's RI signal. \n
      \vertspace{4}
      SMD does not use the RI signal but does allow clients to use the legacy
      RS-232 signal. \n
      \vertspace{4}
      See EFI_SMDL_SIG_GET() and EFI_SMDL_SIG_SET(). */
  SMDL_EVENT_RI,

  /** Event generated when the remote processor closes the port. After the
      remote processor closes the port, no further data can be transferred 
      on the port. Clients can leave the port half-opened and wait for the 
      remote processor to reconnect, at which point SMD Lite generates
      #SMDL_EVENT_OPEN. */
  SMDL_EVENT_REMOTE_CLOSE,

    /** Called in packet mode, when partial packet is available to be read.
   *
   * SMD Lite will send this event to the client when there is partial data 
   * available in packet mode. When clients receive this event, they must read as 
   * much data as is available by calling smdl_readv() in a loop to read data. 
   * See smdl_read(), smdl_readv(), and smdl_rx_peek(). */
  SMDL_EVENT_READ_PARTIAL,

  /** @cond */
  /** Last event in the enumeration. */
  SMDL_EVENT_LAST = SMDL_EVENT_REMOTE_CLOSE,
  /** @endcond */
} smdl_event_type;

/** @cond */
/** The type of RS-232 signal to set, clear, or query. */
typedef enum {
  /** DTR/DSR signals. */
  SMDL_SIG_DTR_DSR,
  
  /** RTS/CTS signals. */
  SMDL_SIG_RTS_CTS,
  
  /** CD signal. */
  SMDL_SIG_CD,
  
  /** RI signal. */
  SMDL_SIG_RI,
} smdl_sig_type;
/** @endcond */

/** @addtogroup efi_smd_constants 
@{ */

/** @name Open flags
  These flags can be used to control the behavior of the port opened by
  EFI_SMDL_OPEN().
@{ */
/** Empty flags definition for EFI_SMDL_OPEN(). This can be passed as the 
  flags argument of %EFI_SMDL_OPEN() to indicate no special operations are 
  requested.
 */
#define SMDL_OPEN_FLAGS_NONE    0x00000000

/** Bitmask of the EFI_SMDL_OPEN() flags for specifying priority.
  \n
  How the priority parameters are interpreted varies depending on the
  implementation of SMD Lite. If SMD Lite is running on a processor with SMD, 
  SMD Lite ports are processed from the SMD tasks. If SMD is implemented 
  using multiple tasks for ports of different priorities, the port priority 
  requested here controls which SMD task the SMD Lite port uses. High-priority 
  ports are processed from the high-priority SMD task, default-priority ports 
  are processed from the default-priority SMD task, and low-priority ports are 
  processed from the low-priority SMD task.
  \n
  If SMD Lite is running on a processor with SMD, but without multiple SMD
  tasks, the priority is ignored and all SMD Lite ports are processed from 
  the one SMD task.
  \n
  If SMD Lite is running on a processor without SMD, the priority is ignored, 
  and all SMD Lite ports are processed from the SMD Lite ISR.
 */
#define SMDL_OPEN_FLAGS_PRIORITY_MASK           0x00000003

/** SMD Lite open flag for default priority. */
#define SMDL_OPEN_FLAGS_PRIORITY_DEFAULT        0x00000000

/** SMD Lite open flag for low priority. */
#define SMDL_OPEN_FLAGS_PRIORITY_LOW            0x00000001

/** SMD Lite open flag for high priority. */
#define SMDL_OPEN_FLAGS_PRIORITY_HIGH           0x00000002

/** SMD Lite open flag for IST priority. */
#define SMDL_OPEN_FLAGS_PRIORITY_IST            0x00000003

/** Bitmask of the EFI_SMDL_OPEN() flags for specifying the transfer mode.
  The mode can be selected as either Packet or Streaming mode. Packet mode  
  is the default.
 */
#define SMDL_OPEN_FLAGS_MODE_MASK               0x00000004

/** Open the port in Packet mode. This is the default mode.
  Data is sent in packets of no more than a maximum size, each preceded by a 
  packet header.
 */
#define SMDL_OPEN_FLAGS_MODE_PACKET             0x00000000

/** Open the port in Streaming mode. This allows an arbitrary amount of data 
  to be read or written without packet headers, while message boundaries 
  are not preserved.
 */
#define SMDL_OPEN_FLAGS_MODE_STREAMING          0x00000004
/** @} */ /* end_name_group Open flags */

/** @name Read flags
  These flags can be used to control the behavior of EFI_SMDL_READ(). No flags
  are defined at this time; clients can pass 0 to the flags argument or use
  the empty-flag value SMDL_READ_FLAGS_NONE.
@{ */
/** Empty flags definition for EFI_SMDL_READ(). This can be passed as the 
  flags argument of %EFI_SMDL_READ() to indicate no special operations are 
  requested.
 */
#define SMDL_READ_FLAGS_NONE    0x00000000

/** Allows EFI_SMDL_READV() to read more data than will fit in the FIFO. 
  This flag can be bitwise-ORed together with other read flags.
 */
#define SMDL_READ_FLAG_LONG     0x00000002
/** @} */ /* end_name_group Read flags */

/** @name Write flags
  These flags can be used to control the behavior of EFI_SMDL_WRITE().
@{ */
/** Empty flags definition for EFI_SMDL_WRITE(). This can be passed as the 
  flags argument of %EFI_SMDL_WRITE() to indicate no special operations are 
  requested.
 */
#define SMDL_WRITE_FLAGS_NONE   0x00000000

/** Does not send an interrupt to the remote processor on write.
  \n
  Clients can use this flag when they are writing a long sequence of packets
  and they want to avoid waking up the remote processor until all the data is
  written. No interrupt is sent when this flag is set, but the remote
  processor may read data from the channel.
  \n
  This flag can be bitwise-ORed together with other write flags.
 */
#define SMDL_WRITE_FLAG_NOINT   0x00000001

/** Allows EFI_SMDL_WRITEV() to write more data than will fit in the FIFO.
  This flag can be bitwise-ORed together with other write flags.
 */
#define SMDL_WRITE_FLAG_LONG    0x00000002
/** @} */ /* end_name_group Write flags */

/** Return code that signals continued read or write.
  \n
  This value is returned by EFI_SMDL_WRITEV() or EFI_SMDL_READV() when the 
  entire message cannot be written to or read from shared memory and the 
  client must retry the write or read after receiving the appropriate event.
 */
#define SMDL_CONTINUE           (int32)(0x80000000)
/** @} */ /* end_addtogroup efi_smd_constants */

/** @ingroup efi_smdl_callback_functions
  @par Summary
  Notification callback when SMD needs to communicate asynchronously with
  the client.

  @param[in] port   Port on which the event occurred.
  @param[in] event  Event that occurred; see #smdl_event_type for details.
  @param[in] data   User-supplied data provided to EFI_SMDL_OPEN().

  @par Description
  This callback function can be called from interrupt context; clients must
  not block or call any functions that block. Clients can call any of the
  following SMD Lite functions from the callback:
  - EFI_SMDL_READ()
  - EFI_SMDL_READV()
  - EFI_SMDL_WRITE()
  - EFI_SMDL_WRITEV()
  - EFI_SMDL_RX_PEEK()
  - EFI_SMDL_TX_PEEK()
  - EFI_SMDL_SIG_SET()
  - EFI_SMDL_SIG_GET()
  - EFI_SMDL_CLOSE()

  @return
  None.
 */
typedef void (*smdl_callback_t) (
                                  smdl_handle_type port,
                                  smdl_event_type  event,
                                  void            *data
                                );

/** @cond */
/** Type for passing parameters to smdl_open_ex */
typedef struct
{
  /* Name of the port. */
  const char       *name;

  /* The edge, identifying both processors, on which this channel communicates.
   * The edge must include the current processor. */
  smd_channel_type  edge;

  /* A bitfield of flags controlling the behavior of the port.
   * #SMDL_OPEN_FLAGS_NONE
   * #SMDL_OPEN_FLAGS_PRIORITY_DEFAULT
   * #SMDL_OPEN_FLAGS_PRIORITY_LOW
   * #SMDL_OPEN_FLAGS_PRIORITY_HIGH
   * #SMDL_OPEN_FLAGS_PRIORITY_IST
   * #SMDL_OPEN_FLAGS_MODE_PACKET
   * #SMDL_OPEN_FLAGS_MODE_STREAMING */
  uint32            flags;

  /* The size of the FIFO. This is the physical size in memory, which is always
   * four bytes greater than the total capacity of the FIFO. In packet mode, the
   * SMD packet header adds 20 bytes per packet, further reducing the capacity.
   * The FIFO size must be a multiple of 32 and must not be less than
   * SMD_MIN_FIFO or greater than SMD_MAX_FIFO.
   * If this parameter is 0, the default size (SMD_STANDARD_FIFO) will be used.
   */
  uint32            fsize;

  /* The callback function called when SMD needs to notify the client of a
   * remote event on the SMD port. */
  smdl_callback_t   notify;

  /* The data passed to the notify function. */
  void             *cb_data;

  /* Handle returned to client after the port is allocated. */
  smdl_handle_type  handle;

  /* Extended status information in case of failure.
   * When SMD_STATUS_SIZE_MISMATCH is returned, this field contains the
   * existing fifo size. */
  uint32            ext_status;

} smdl_open_params_type;
/** @endcond */

#endif /* SMD_TYPE_H */

