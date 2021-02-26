/*---------------------------------------------------------------------------
   SMD.H  - Shared Memory Driver
---------------------------------------------------------------------------*/
/**
  @file smd.h

*/
/* This file contains the SMD external data structure and APIs. */


/* NOTE: For the output PDF generated using Doxygen and Latex, all file and 
         group descriptions are maintained in the SMD_mainpage file. To change
         any of the the file/group text for the PDF, edit the SMD_mainpage 
         file, or contact Tech Pubs.

         The above description for this file is part of the "smd" group
         description in the SMD_mainpage file. 
*/

/*--------------------------------------------------------------------------
Copyright (c) 2005-2011 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
---------------------------------------------------------------------------*/

/*===========================================================================

                           EDIT HISTORY FOR FILE


when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/08/11   bm      Added support for a new ioctl - 
                   SMD_IOCTL_REGISTER_MEMCPY_RESET
07/26/11   tl      Added SMD IST priority
05/03/11   tl      Moved smd_channel_type to smd_type.h
04/01/11   bm      Added  smd_xfrflow_type element to smd_open_param_type 
                   struct.
03/25/11   hwu     Merging 8960 support. 
01/06/11   bt      Removed SMD Block and Data Mover protocols.
12/23/10   hwu     Removed static ports from SMD port list.
10/25/20   bt      Fixed port_name argument bugs in all smd open functions.
08/13/10   esh     Removed unused typedef
08/12/10   sw      (Tech Pubs) Edited/added Doxygen comments and markup.
07/21/10   tl      Added SMD API for SMD priority. Deprecated other open APIs.
06/21/10   rs      SMD Lite V2 8660 target build:                      
                     + Changed ARM7 processor name to DSPS
04/30/10   rs      SMD lite v2 port to ARM7: 
                     + Added new edges for PPSS processor
02/16/10   tl/esh  Added Doxygen tags
12/07/09   rs      CMI updates, featurization removal.
09/29/09   bfc/rs  Added variable sized FIFO API support.
07/17/09   tl      Added CTS/RTS callback
06/10/09   bfc     CMI updates.
04/20/09   bfc     Only declare AU public items here.
01/26/09   bfc     Added new return types and a new dataxfr type.
09/24/08   jlk     Added smd version
08/20/08   bfc     Changed the SMD BLOCK interface.
07/29/08   jlk     added FIRST and LAST to channel types for type checking
05/02/08   jlk     added wm_wait ioctl
02/21/08   bfc     First SMD Block port check in.
01/29/08   hwu     Added smd_get_version.
01/14/08   bfc     Created a SMD base struct.
01/14/08   bfc     Added the public APIS for the block ports.
01/09/08   bfc     Added a SMD API version feature.
10/25/07   hwu     Changed rx/tx/flwctl callback prototype.
10/19/07   hwu     Changed smd_port_id_type to uint32.
                   Added smd_open_memcpy_2.
10/19/07   bfc     Fixed a merged error.
10/18/07   bfc     Added tramsmit and receive abort ioctls
10/08/07   hwu     Move smd_channel_type to smd.h
06/12/07   bfc     Added a ioctl that enables a low level packet layer.
05/19/07   hwu     Added SMD_PORT_LOOPBACK for SMD loopback testing.
                   Also added additional ports for component services.
05/02/07   hwu     Added SMD_NUM_CHANNELS. 
04/26/07   ptm     Use SMD NUM PORTS in place of SMD PORT LAST and SMD PORT MAX
03/14/07   ptm     Delete unused symbol.
02/28/06   ptm     Add IOCTL INVALID symbol and delete unsupported BT IOCTL
                   symbols.
02/06/06   ptm     Increase number of SMD data ports to 20.
01/09/07   bfc     Added SMD using DM
12/18/06   bfc     Moved control port logic to the smd_wm_layer 
11/03/06   ptm     Change SMD_SMD_... symbols to SMD_... symbols and rename
                   dsm_iov to smd_iov_type.
10/09/06   bfc     Removed a SMD Tranport layer to DSM dependency and deleted
                   some structures that are not needed at this layer.
10/05/06   bfc     Added new ioctl's that enable/disable Full Rcv Buffer
                   control and edited some comments
09/28/06   taw     Added symbols that were previously being pulled from
                   sio.h.  These were SIO_* and are now SMD_*.
05/19/06   ptm     Restore IOCTL comment.
04/28/06   ptm     Added flow control callback.
03/27/06   bfc     Modified SMD to use the new sio extended function callback
                   parameters
11/22/05   ptm     Initial version (based on smd_interface.h) API restructured.
===========================================================================*/


/*===========================================================================

                        INCLUDE FILES

===========================================================================*/
#ifndef SMD_H
#define SMD_H

#include "smd_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------
  Defines
-------------------------------------------------------------------------*/


/** @addtogroup smd
@{ */

/** @name SMD Port IDs
  These numbered ports are defined for communication between the modem and
  applications processors from SMD buffer layer. The IDs can be passed as 
  the port_id parameter to smd_open_memcpy(), or they can be returned as 
  the port ID from smd_open_memcpy_2(), smd_open_memcpy_sz(), or 
  smd_open_memcpy_3().
@{ */
#define SMD_PORT_INVALID         0xFFFFFFFF
#define SMD_PORT_FIRST           0
#define SMD_PORT_DS              SMD_PORT_FIRST
#define SMD_PORT_LOOPBACK        1
#define SMD_PORT_RPC_CALL        2
#define SMD_PORT_DUMMY           3    
#define SMD_NUM_PORTS            4

/** @} */ /* end_name_group SMD Port IDs */


/*--------------------------------------------------------------------------
  Enumerated types
---------------------------------------------------------------------------*/
typedef uint32    smd_port_id_type;
  
/* smd_ioctl_cmd_type */
/** IOCTL commands supported by the SMD.
  @sa
  smd_ioctl, smd_ioctl_param_type
*/
typedef enum
{
  SMD_IOCTL_INVALID = -1,         /**< Invalid IOCTL. */
  SMD_IOCTL_CD_ASSERT=0,          /**< Asserts a Carrier Detect indicator. */
  SMD_IOCTL_CD_DEASSERT,          /**< Deasserts a Carrier Detect indicator. */
  SMD_IOCTL_GET_CURRENT_CD,       /**< Indicates whether a Carrier Detect 
                                       signal is asserted. */
  SMD_IOCTL_RI_ASSERT,            /**< Asserts a Ring Indication. */
  SMD_IOCTL_RI_DEASSERT,          /**< Deasserts a Ring Indication. */
  SMD_IOCTL_GET_CURRENT_RI,       /**< Indicates whether a Ring Indication 
                                       signal is asserted. */
  SMD_IOCTL_ENABLE_DTR_EVENT,     /**< Registers a clock callback that 
                                       notifies the caller of the change in 
                                       state of the Data Terminal Ready 
                                       indication. */
  SMD_IOCTL_DISABLE_DTR_EVENT,    /**< Deregisters the Data Terminal 
                                       Ready indication notification clock 
                                       callback. */
  SMD_IOCTL_INBOUND_FLOW_ENABLE,  /**< Enables the inbound flow using the
                                       current flow control method. */
  SMD_IOCTL_INBOUND_FLOW_DISABLE, /**< Disables the inbound flow using the 
                                       current flow control method. */
  SMD_IOCTL_DSR_ASSERT,           /**< Asserts the Data Set Ready indication. */
  SMD_IOCTL_DSR_DEASSERT,         /**< Deasserts the Data Set Ready indication. */
  SMD_IOCTL_DTE_READY_ASSERTED,   /**< Indicates whether the Data Terminal 
                                       Equipment ready signal is asserted. */
  SMD_IOCTL_GET_CURRENT_RTS,      /**< Reads the current status of Request To 
                                       Send from the Data Terminal Equipment 
                                       (i.e., the MSM&tm;&nbsp; DCE CTS pin).*/
  SMD_IOCTL_SET_FLOW_CTL,         /**< Sets the flow control method. */
  SMD_IOCTL_GET_FLOW_CTL,         /**< Gets the flow control method. */
  SMD_IOCTL_ENABLE_AUTODETECT,    /**< Enables AutoDetect. This has no effect 
                                       if AutoDetect is already enabled. */
  SMD_IOCTL_RECORD_ESC_FUNC_PTR,  /**< Registers a clock callback function 
                                       that notifies the caller of the 
                                       arrival of the escape code ( +++ ). */
  SMD_IOCTL_RECORD_PKT_FUNC_PTR,  /**< Registers a clock callback function that 
                                       notifies the caller of the arrival of the 
                                       Point-to-Point Protocol packet. */
  SMD_IOCTL_BEGIN_FAX_IMAGE,      /**< Used by the caller to signal the 
                                       beginning of a fax image. From the 
                                       beginning of the fax image to the end 
                                       of the fax image on a given line, 
                                       software flow control must be ignored 
                                       because the beginning/end may be 
                                       unescaped data bytes. */
  SMD_IOCTL_CHANGE_BAUD_RATE,     /**< Used to change the baud rate of the 
                                       serial port.\n
         @note This is for backward compatability with data services only.\n 
         The change takes place <i>only after</i> a transition from a non-empty 
         Tx FIFO buffer to an empty Tx FIFO buffer. If the Tx FIFO buffer is 
         empty when this IOCTL is received, no baud change occurs until more 
         data is transmitted. */
  SMD_IOCTL_CHANGE_BAUD_NOW,      /**< Used to change the baud rate of the 
                                       serial port. Change takes place 
                                       immediately without regard to any data
                                       in the Tx FIFO buffer. Therefore, it is 
                                       up to the application to flush the Tx 
                                       FIFO buffer beforehand. Otherwise, the 
                                       usage is identical to 
                                       SMD_IOCTL_CHANGE_BAUD_RATE. */
  SMD_IOCTL_FLUSH_TX,             /**< Registers a callback function that 
                                       notifies the caller when all pending
                                       output data has been transmitted.\n 
         @note Unreliable results may occur if data is queued after this 
         function call is made and before the callback function has been 
         called. */
  SMD_IOCTL_CHANGE_MODE,          /**< Changes the serial port's mode (e.g., 
                                       from AUTODETECT to RAWDATA). */
  SMD_IOCTL_REG_TLP_CHANGE_CB,    /**< Registers a callback to be invoked when 
                                       the version or use of the USB Thin-Layer 
                                       Protocol changes. The callback is invoked 
                                       immediately to provide the initial state. */
  SMD_IOCTL_GET_PACKET_SIZE,      /**< Reads the maximum packet size for 
                                       the packet devices. */
  SMD_IOCTL_SET_PACKET_SIZE,      /**< Changes the maximum packet size for 
                                       the packet devices. */
  SMD_IOCTL_GET_BUFFER_SIZE,      /**< Reads the buffer size for devices 
                                       that have buffers. */
  SMD_IOCTL_SET_BUFFER_SIZE,      /**< Changes the buffer size for devices
                                       that have variable buffers. */
  SMD_IOCTL_GET_XFR_MODE,         /**< Reads the transfer mode for a device. */
  SMD_IOCTL_SET_XFR_MODE,         /**< Sets the transfer mode for a device. */
  SMD_IOCTL_GET_XFR_TYPE,         /**< Reads the transfer type for a device. */
  SMD_IOCTL_SET_XFR_TYPE,         /**< Sets the transfer type for a device. */
  SMD_IOCTL_ENABLE_DTR_EVENT_EXT, /**< Registers a callback that notifies the
                                       caller of a change in state of the Data 
                                       Terminal Ready indication. The callback 
                                       gets the port ID and user data as 
                                       arguments. */
  SMD_IOCTL_DISABLE_DTR_EVENT_EXT,/**< Deregisters the Data Terminal Ready
                                       indication notification callback. */
  SMD_IOCTL_ENABLE_CD_EVENT_EXT,  /**< Registers a callback that notifies the
                                       caller of a change in the state of the 
                                       Carrier Detect indication. The callback 
                                       gets the port ID and user data as 
                                       arguments. */
  SMD_IOCTL_DISABLE_CD_EVENT_EXT, /**< Deregisters the Carrier Detect 
                                       indication notification callback. */
  SMD_IOCTL_ENABLE_RI_EVENT_EXT,  /**< Registers a callback that notifies
                                       the caller of a change in the state 
                                       of the Ring Indication. */
  SMD_IOCTL_DISABLE_RI_EVENT_EXT, /**< Deregisters the Ring Indication 
                                       notification callback. */
  SMD_IOCTL_SET_DROP_PEND,        /**< Sets the Drop Pending state. */
  SMD_IOCTL_GET_DROP_PEND,        /**< Gets the Drop Pending state. */
  SMD_IOCTL_SET_FULL_RCV_BUFS,    /**< Sets the Full Receive Buffer state. */
  SMD_IOCTL_GET_FULL_RCV_BUFS,    /**< Gets the Full Receive Buffer state. */
  SMD_IOCTL_SET_PACKET_MODE,      /**< Sets the SMD Packet mode state. */
  SMD_IOCTL_GET_PACKET_MODE,      /**< Gets the SMD Packet mode state. */
  SMD_IOCTL_ABORT_TX,             /**< Stops the pending tranmits at the buffer
                                       level. */
  SMD_IOCTL_ABORT_RX,             /**< Stops the pending tranmits at the buffer
                                       level. */
  SMD_IOCTL_WM_SET_WAIT,          /**< Sets the wm_info->wait flag to the value 
                                       of the wait parameter. If TRUE, this 
                                       signals the watermark layer to stop 
                                       sending data. If FALSE, waiting data is 
                                       transmitted. */
  SMD_IOCTL_WM_GET_WAIT,          /**< Gets the current value of the
                                       wm_info->wait flag. */
  SMD_IOCTL_ENABLE_CTS_EVENT_EXT, /**< Registers a callback that notifies 
                                       the client of a change in the 
                                       Clear-To-Send/Request-To-Send register. */
  SMD_IOCTL_DISABLE_CTS_EVENT_EXT,/**< Deregisters the CTS/RTS notification
                                       callback. */
  SMD_IOCTL_REGISTER_MEMCPY_RESET /**< Register a client callback to be called 
                                       when memcpy stream is reset.  */
} smd_ioctl_cmd_type;

/* smd_flow_ctl_type */
/** SMD flow control types.
*/
typedef enum
{
  SMD_FCTL_BEST = 0,              /**< Best flow control method: the default
                                       or no change. */
  SMD_FCTL_OFF,                   /**< Flow control is disabled. */
  SMD_XONXOFF_STRIP_FCTL_FS,      /**< Use fail-safe XON/XOFF flow control, 
                                       but strip the XON/XOFF characters 
                                       from the stream. */
  SMD_XONXOFF_STRIP_FCTL_NFS,     /**< Use non-failsafe XON/XOFF flow control,
                                       but strip the XON/XOFF 
                                       characters from the stream. */
  SMD_CTSRFR_FCTL,                /**< Use Clear-to-Send (CTS) / Ready for 
                                       Receiving (RFR) flow control. */
  SMD_XONXOFF_NSTRIP_FCTL_FS,     /**< Use fail-safe XON/XOFF flow control 
                                       and leave the XON/XOFF characters in 
                                       the stream. */
  SMD_XONXOFF_NSTRIP_FCTL_NFS,    /**< Use non-failsafe XON/XOFF flow control
                                       and leave the XON/XOFF characters 
                                       in stream. */
  SMD_MAX_FLOW,                   /**< For bounds checking only. */
  SMD_CTSRFR_AUTO_FCTL            /**< Use CTS/RFR flow control with auto Rx
                                       RFR signal generation. */
} smd_flow_ctl_type;

/* smd_dataxfr_mode_type */
/** SMD transfer modes.
*/
typedef enum    
{
  SMD_MEMCPY_MODE,       /**< &nbsp; */
  SMD_INVALID_MODE       /**< &nbsp; */
} smd_dataxfr_mode_type;

/* smd_xfrflow_type */
/** SMD transfer flow modes. 
*/
typedef enum    
{
  SMD_XFRFLOW_FIRST,                     /**< &nbsp; */
  SMD_NO_XFRFLOW = SMD_XFRFLOW_FIRST,    /**< &nbsp; */
  SMD_STREAMING_TYPE,                    /**< &nbsp; */
  SMD_PKTXFR_TYPE,                       /**< &nbsp; */
  SMD_INVALID_TYPE                       /**< &nbsp; */
} smd_xfrflow_type;

/* smd_api_return_type */
/** SMD API return values. 
  @note The values contained in this enumeraton cannot mutate. Add new values
        only to the end of the list. For deprecated values, leave
        them or replace them with place holders; do not remove them.
*/
typedef enum
{
  SMD_UNAVAIL,            /**< &nbsp; */
  SMD_INVALID_INPUT,      /**< &nbsp; */
  SMD_BUSY,               /**< &nbsp; */
  SMD_PENDING,            /**< &nbsp; */
  SMD_NO_RESOURCES,       /**< &nbsp; */
  SMD_NO_DATA,            /**< &nbsp; */
  SMD_DONE,               /**< &nbsp; */
  SMD_NON_CRIT_MISMATCH   /**< &nbsp; */
}smd_api_return_type;

/* smd_vv_func_ptr_type */
/** Function pointer type. It points to a function that takes no arguments.

  @return
  None.
*/
typedef void ( *smd_vv_func_ptr_type )( void );

/* smd_vu_func_ptr_type */
/** Function pointer type. It points to a function that takes a callback data
  pointer.

  @param[in] cb_data   User-provided callback data.

  @return
  None.
*/
typedef void (*smd_vu_func_ptr_type)(void * cb_data);

/** SMD close callback function typedef.

 @param[in] data    User-provided callback data.

 @return
  None.
*/
typedef void ( *smd_close_callback_fn )( void *data );

/** SMD flush callback function typedef.

  @param[in] data    User-provided callback data.

  @return
  None.
*/
typedef void ( *smd_flush_callback_fn )( void *data );

/** SMD abort callback function typedef.

  @param[in] data          User-provided callback data.
  @param[in] bytes_left    Number of bytes left to transmit.

  @return
  None.
*/
typedef void ( *smd_abort_callback_fn )( void *data, uint32 bytes_left );

/** MEMCPY transmit callback function typedef. This is used to request the 
next transmit buffer (if any).

  @param[in]  data     User data provided when the callback was registered.
  @param[out] buffer   Tx data buffer for the next data block (or NULL if none).
  @param[out] size     Number of bytes to send from the buffer.

  @return
  None.
 */
typedef void ( *smd_memcpy_tx_callback_fn )( void              *cb_data,
                                             byte             **buf_ptr,
                                             uint32            *size );

/** MEMCPY receive callback function typedef. This is used to request the next 
  receive buffer.

  @param[in]     data        User data provided when the callback was 
                             registered.
  @param[in]     bytes_read  Number of bytes in the buffer.
  @param[in,out] buffer      Input is the previous receive buffer with data, 
                             and output is the next empty buffer.
  @param[out]    size        Size of the new receive buffer.

  @return
  None.
 */
typedef void ( *smd_memcpy_rx_callback_fn )( void              *cb_data,
                                             uint32             bytes_read,
                                             byte             **buf_ptr,
                                             uint32            *size );

/** MEMCPY receive flow control callback function typedef.

  This is called when rx_callback_fn returns a NULL buffer and flow 
  control is not enabled. The user can then determine how much data is to be 
  thrown away. This is user-defined to allow for packet boundary maintenance.

  @param[in] cb_data      User data provided when the callback was registered.
  @param[in] byte_count   Number of bytes pending.

  @return
  uint32 -- Number of bytes to throw away.
 */
typedef uint32 (*smd_memcpy_rx_flowctl_fn) ( void              *cb_data,
                                             uint32             byte_count );

/** MEMCPY stream reset callback function typedef.

  This is called when MEMCPY stream is reset, and used to convey this 
  information to the client so that it may reset its internal structures
  accordingly.

  @param[in] cb_data      User data provided when the callback was registered.

  @return
  None.
 */
typedef void (*smd_memcpy_reset_callback_fn) ( void        *cb_data );

/*-------------------------------------------------------------------------
  Structure definitions
--------------------------------------------------------------------------*/
/* smd_ioctl_flow_ctl_type */
/** @brief Flow control type.\n
  This structure must specify transmit flow control and receive flow control
  methods separately. Because the SIO interface allows these methods to be 
  set separately, the SMD interface must do so as well.
*/
typedef struct smd_ioctl_flow_ctl 
{
  smd_flow_ctl_type tx_flow;            /**< Transmit flow control setting. */
  smd_flow_ctl_type rx_flow;            /**< Receive flow control setting. */
} smd_ioctl_flow_ctl_type;

/* smd_ioctl_enable_dtr_event_ext_type */
/** @brief Data Terminal Ready callback function and function callback data. 
*/
typedef struct smd_ioctl_enable_dtr_event_ext
{
  smd_vu_func_ptr_type  cb_func;        /**< Callback function. */
  void *                cb_data;        /**< User-defined callback data. */
} smd_ioctl_enable_dtr_event_ext_type;

/* smd_ioctl_abort_ext */
/** @brief Tx abort callback function and function callback data.
*/
typedef struct smd_ioctl_abort_ext
{
  smd_abort_callback_fn  cb_func;       /**< Callback function. */
  void *                 cb_data;       /**< User-defined callback data. */
}smd_ioctl_abort_ext_type;

/* smd_ioctl_register_memcpy_reset_type */
/** @brief Tx abort callback function and function callback data.
*/
typedef struct smd_ioctl_register_memcpy_reset
{
  smd_memcpy_reset_callback_fn  cb_func;       /**< Callback function. */
  void *                        cb_data;       /**< User-defined callback data. */
}smd_ioctl_register_memcpy_reset_type;
/** @} */ /* end_addtogroup smd */
/** @addtogroup smd
@{ */

/** @name Flow Control Callback Typedef Aliases
@{ */
/** Typedef for DTR event callback data. */
typedef struct smd_ioctl_enable_dtr_event_ext \
                                            smd_ioctl_enable_cb_event_ext_type;

/** Typedef for DTR event ring indicator data. */
typedef struct smd_ioctl_enable_dtr_event_ext \
                                            smd_ioctl_enable_ri_event_ext_type;

/** Typedef for DTR event clear-to-send data. */
typedef struct smd_ioctl_enable_dtr_event_ext \
                                           smd_ioctl_enable_cts_event_ext_type;

/** @} */ /* end_name_group test */

/** @} */ /* end_addtogroup smd */
/** @addtogroup smd
@{ */

/*-------------------------------------------------------------------------
   Union definitions
--------------------------------------------------------------------------*/
/* smd_ioctl_param_type */
/** @brief Union of the supported IOCTL parameter types. 
*/
typedef union smd_ioctl_param 
{
  smd_vv_func_ptr_type                  enable_dte_ready_event;            
  boolean                               *dte_ready_asserted;                
  boolean                               *rts_asserted;                      
  boolean                               *cd_asserted;                       
  boolean                               *ri_asserted;                       
  smd_ioctl_flow_ctl_type               flow_ctl;                          
  smd_vv_func_ptr_type                  record_flush_func_ptr;             
  uint32                                packet_size;                       
  uint32                                buffer_size;                       
  boolean                               drop_asserted;
  boolean                               full_bufs_asserted;
  smd_dataxfr_mode_type                 dataxfr_mode;                      
  smd_xfrflow_type                      xfrflow_type;                      
  smd_ioctl_enable_dtr_event_ext_type   enable_dte_ready_event_ext; 
  smd_ioctl_enable_cb_event_ext_type    enable_cd_ready_event_ext;  
  smd_ioctl_enable_ri_event_ext_type    enable_ri_ready_event_ext;
  smd_ioctl_enable_cts_event_ext_type   enable_cts_ready_event_ext;
  smd_ioctl_abort_ext_type              abort_ext;
  smd_ioctl_register_memcpy_reset_type  memcpy_reset;
  void                                  *tx_buffer;
  boolean                               wait;
} smd_ioctl_param_type;

/**
 * SMD port priority type
 *
 * This enumeration lists the available SMD port priorities. The priorities
 * represented here will generally correspond to different OS tasks running at
 * different priorities, but some implementations use only one SMD task and
 * ignore the requested priority. The priorities in this list are sorted from
 * highest to lowest so SMD can signal the priorities in order and guarantee
 * that the highest-priority task will be executed first.
 *
 * #SMD_PRIORITY_IST and #SMD_PRIORITY_COUNT are reserved for internal use and
 * may not be used by clients.
 */
typedef enum
{
  SMD_PRIORITY_HIGH,
  SMD_PRIORITY_DEFAULT,
  SMD_PRIORITY_LOW,
  SMD_PRIORITY_IST,
  SMD_PRIORITY_COUNT
} smd_priority_type;

/**
 * SMD port optional parameters
 *
 * This structure may be used to pass optional parameters to
 * smd_open_memcpy_3() or smd_wm_open_3(). Each parameter represented in this
 * structure has a default value that will be used if an alternate value in
 * not specified.
 *
 * Passing a NULL pointer as the smd_param argument of smd_open_memcpy_3() or
 * smd_wm_open_3() has the same effect as passing a pointer to a valid struct
 * whose fields parameter is 0.
 *
 * The SMD API may be extended by adding additional optional parameters at the
 * end of this structure, and using new bits to represent them, but backward
 * compatability will be maintained by leaving the field ordering in place.
 */
typedef struct {
  /** Bitmask of the values used in this structure.
   *
   * Bits not specified are reserved and must be 0.
   */
  uint32             fields;

  /** The size of the circular buffer allocated in shared memory to transfer
   * data.
   *
   * The FIFO size must be a multiple of 32 and must not be greater than
   * #SMD_MAX_FIFO. Large FIFOs allow more efficient transfer of large amounts
   * of data but consume more shared memory. If the FIFO size is not
   * specified, the default #SMD_STANDARD_FIFO is used.
   *
   * If this field is set, the bit #SMD_OPEN_PARAM_FIFO_SIZE must be set in
   * fields.
   */
  uint32             fifo_size;

  /** The priority requested for the SMD channel.
   *
   * If the priority is not specified, the default #SMD_PRIORITY_DEFAULT is
   * used.
   *
   * If this field is set, the bit #SMD_OPEN_PARAM_PRIORITY must be set in
   * fields.
   */
  smd_priority_type  priority;

  /** The xfrflow type for the SMD channel.
   *
   * If the xfrflow type is not specified, the default #SMD_NO_XFRFLOW is
   * used.
   *
   * If this field is set, the bit #SMD_OPEN_PARAM_XFRFLOW must be set in
   * fields.
   */
  smd_xfrflow_type  xfrflow;

} smd_open_param_type;

/** \name SMD parameter bits 
 *
 * These bits define the bitmask used by #smd_open_param_type.
 *
 * Bits not specified are reserved and must be 0.
 * @{
 */
#define SMD_OPEN_PARAM_FIFO_SIZE 0x00000001
#define SMD_OPEN_PARAM_PRIORITY  0x00000002
#define SMD_OPEN_PARAM_XFRFLOW   0x00000004
/*@}*/

/*--------------------------------------------------------------------------
  Function Declarations
---------------------------------------------------------------------------*/

/** @} */ /* end_addtogroup smd */

/** @addtogroup smd_buffer
@{ */

/*===========================================================================
  FUNCTION  smd_open_memcpy
===========================================================================*/
/**
  Opens an SMD MEMCPY stream for a specified port. It must be called before any
  other operations are performed on that port. 

  @param[in] port_id         SMD port to open.
  @param[in] tx_callback_fn  Called when the previous transmit buffer (if any) 
                             has been copied to shared memory. The callback is 
                             to provide the next buffer (and the buffer size) 
                             to send. If no buffer is provided,
                             smd_notify_write must be called when a new 
                             transmit buffer is available.\n 
                             The call to smd_notify_write causes the SMD task 
                             to call this callback to get the buffer. This 
                             callback is called in the SMD task context and 
                             <i>must not block or wait</i>.
  @param[in] rx_callback_fn  Called when the SMD has finished writing data into
                             the previous receive buffer (if any). It is called 
                             with the number of bytes written into the previous 
                             buffer. The callback is to provide the next read 
                             buffer and the space available in the buffer.\n 
                             If the number of bytes written is zero, there was 
                             no previous buffer. If no buffer is provided, 
                             smd_notify_read must be called when a new receive 
                             buffer is available. The call to smd_notify_read 
                             causes the SMD task to call this callback to get 
                             the buffer. This callback is called in the SMD 
                             task context and <i>must not block or wait</i>.
  @param[in] rx_flowctl_fn   Called when rx_callback_fn returns a NULL buffer 
                             and flow control is not enabled. Flow control 
                             not enabled means that pending data should be 
                             thrown away if there is nowhere to put the data.\n 
                             This function allows the user to specify how much 
                             data to throw way for maintaining packet 
                             synchronization. The function is called with the 
                             number of bytes pending and returns the number of 
                             bytes to skip. For streaming-mode streams, you 
                             typically skip all of the pending bytes.\n
                             This callback is called in the SMD task context 
                             and <i>must not block or wait</i>.

  @return
  None.

  @dependencies
  Shared memory must have been initialized on this processor by calling 
  smem_init().\n 
  The SMD must be up and running. The startup process is platform specific.

@deprecated   This function is deprecated; new code should use
              smd_open_memcpy_3() instead. Existing users of this function
              will continue to be supported indefinately.

  @sideeffects
  Allocates an SMD stream information structure.
*/
/*=========================================================================*/
void smd_open_memcpy
(
  smd_port_id_type          port_id,
  smd_memcpy_tx_callback_fn tx_callback_fn,
  smd_memcpy_rx_callback_fn rx_callback_fn,
  smd_memcpy_rx_flowctl_fn  rx_flowctl_fn
);

/*===========================================================================
  FUNCTION  smd_open_memcpy_2
===========================================================================*/
/**
  Opens a shared memory port identified by the name and direction. It must be 
  called before any other operations are performed on that port. 

  @param[in] port_name         Name of the SMD port. The same name must be used 
                               by both processors for a particular port. The 
                               length of the name is defined as 
                               SMD_CHANNEL_NAME_SIZE_MAX, where the last 
                               character is always null. Do not use 
                               whitespace in port_name.
  @param[in] channel_type      Port connection type; specifies the connection 
                               direction.
  @param[in] tx_callback_fn    Called when the previous transmit buffer 
                               (if any) has been copied to shared memory. The
                               callback should provide the next buffer (and the
                               buffer size) to send. If no buffer is provided, 
                               smd_notify_write must be called when a new 
                               transmit buffer is available.\n 
                               The call to smd_notify_write causes the SMD task 
                               to call this callback to get the buffer. This 
                               callback is called in the SMD task context and 
                               <i>must not block or wait</i>.
  @param[in] tx_callback_data  A pointer to the client tx_callback_fn data.
  @param[in] rx_callback_fn    Called when the SMD has finished writing data 
                               into the previous receive buffer (if any). It is 
                               called with the number of bytes written into the 
                               previous buffer. The callback is to provide the 
                               next read buffer and the space available in the 
                               buffer. If the number of bytes written is zero, 
                               there was no previous buffer. If no buffer is 
                               provided, smd_notify_read must be called when a 
                               new receive buffer is available.\n 
                               The call to smd_notify_read causes the SMD task 
                               to call this callback to get the buffer. This 
                               callback is called in the SMD task context and 
                               <i>must not block or wait</i>.
  @param[in] rx_callback_data  Pointer to the client rx_callback_fn data.
  @param[in] rx_flowctl_fn     Called when rx_callback_fn returns a NULL buffer 
                               and flow control is not enabled. Flow control 
                               not enabled means that pending data should be 
                               thrown away if there is nowhere to put the 
                               data.\n 
                               This function allows the user to specify how 
                               much data to throw way for maintaining packet 
                               synchronization. The function is called with the 
                               number of bytes pending and returns the number 
                               of bytes to skip.\n 
                               For streaming-mode streams, you typically skip 
                               all of the pending bytes.\n
                               This callback is called in the SMD task context 
                               and <i>must not block or wait</i>.
  @param[in] rx_flowctl_data   Pointer to the client rx_flowctl_fn data.

  @return
  SMD port ID of the newly-opened port.

  @dependencies
  Shared memory must have been initialized on this processor by calling 
  smem_init().\n
  The SMD must be up and running. The startup process is platform specific.

@deprecated   This function is deprecated; new code should use
              smd_open_memcpy_3() instead. Existing users of this function
              will continue to be supported indefinately.

  @sideeffects
  Allocates an SMD stream information structure.
*/
/*=========================================================================*/
uint32 smd_open_memcpy_2
(
  const char                *port_name,
  smd_channel_type           channel_type,
  smd_memcpy_tx_callback_fn  tx_callback_fn,
  void                      *tx_callback_data,
  smd_memcpy_rx_callback_fn  rx_callback_fn,
  void                      *rx_callback_data,
  smd_memcpy_rx_flowctl_fn   rx_flowctl_fn,
  void                      *rx_flowctl_data
);

/*===========================================================================
  FUNCTION  smd_open_memcpy_sz
===========================================================================*/
/**
  Opens a shared memory port identified by the name and direction. The client 
  must also specify the SMD FIFO size.
  
  This function must be called before any other operations are performed on 
  that port. 

  @param[in] port_name         Name of the SMD port. The same name must be used 
                               by both processors for a particular port. The 
                               length of the name is defined as 
                               SMD_CHANNEL_NAME_SIZE_MAX, where the last 
                               character is always null. Do not use 
                               whitespace in port_name.
  @param[in] channel_type      Port connection type; specifies the connection 
                               direction.
  @param[in] tx_callback_fn    Called when the previous transmit buffer (if any) 
                               has been copied to shared memory. The callback is 
                               to provide the next buffer (and the buffer size) 
                               to send. If no buffer is provided, 
                               smd_notify_write must be called when a new 
                               transmit buffer is available.\n 
                               The call to smd_notify_write causes the SMD task 
                               to call this callback to get the buffer. This 
                               callback is called in the SMD task context and 
                               <i>must not block or wait</i>.
  @param[in] tx_callback_data  A pointer to the client tx_callback_fn data
  @param[in] rx_callback_fn    Called when the SMD has finished writing data
                               into the previous receive buffer (if any). It is
                               called with the number of bytes written into the
                               previous buffer. The callback should provide the
                               next read buffer and the space available in the
                               buffer. If the number of bytes written is zero, 
                               there was no previous buffer. If no buffer is 
                               provided, smd_notify_read must be called when a
                               new receive buffer is available.\n 
                               The call to smd_notify_read causes the SMD task 
                               to call this callback to get the buffer. This 
                               callback is called in the SMD task context and 
                               <i>must not block or wait</i>.
  @param[in] rx_callback_data  A pointer to the client rx_callback_fn data
  @param[in] rx_flowctl_fn     Called when rx_callback_fn returns a NULL buffer 
                               and flow control is not enabled. Flow control 
                               not enabled means that pending data should be 
                               thrown away if there is nowhere to put the 
                               data.\n 
                               This function allows the user to specify how 
                               much data to throw way for maintaining packet 
                               synchronization. The function is called with the 
                               number of bytes pending and returns the number 
                               of bytes to skip. For streaming-mode streams, 
                               you typically skip all of the pending bytes.\n
                               This callback is called in the SMD task context 
                               and <i>must not block or wait</i>.
  @param[in] rx_flowctl_data   Pointer to the client rx_flowctl_fn data.
  @param[in] fifo_sz           SMD FIFO size. The FIFO size must be a multiple
                               of 32 and must not be greater than
                               #SMD_MAX_FIFO.

  @return
  SMD port ID.

  @dependencies
  Shared memory must have been initialized on this processor by calling 
  smem_init().\n 
  The SMD must be up and running. The startup process is platform specific.

@deprecated   This function is deprecated; new code should use
              smd_open_memcpy_3() instead.

  @sideeffects
  Allocates an SMD stream information structure.
*/
/*=========================================================================*/
uint32 smd_open_memcpy_sz 
(
  const char                *port_name,
  smd_channel_type           channel_type,
  smd_memcpy_tx_callback_fn  tx_callback_fn,
  void                      *tx_callback_data,
  smd_memcpy_rx_callback_fn  rx_callback_fn,
  void                      *rx_callback_data,
  smd_memcpy_rx_flowctl_fn   rx_flowctl_fn,
  void                      *rx_flowctl_data,
  uint32                     fifo_sz
);

/*===========================================================================
  FUNCTION  smd_open_memcpy_3
===========================================================================*/
/*!
@brief
  Opens a shared memory port identified by the name and edge with optional
  parameters.

  This function must be called before any other operations are done on that
  port. 

@param[in]  port_name           The name of the SMD port. The same name must
                                be used by both processors for a particular
                                port. The length of the name is defined as
                                SMD_CHANNEL_NAME_SIZE_MAX, where the last 
                                character is always null. Do not use 
                                whitespace in port_name.

@param[in]  channel_type        The port connection type, specifies the
                                connection direction.  

@param[in]  callback_data       A pointer to the client data passed to all
                                three client callback functions.

@param[in]  tx_callback_fn      This function is called when the previous
                                transmit buffer (if any) has been copied to
                                shared memory. The callback should provide the
                                next buffer(and the buffer size)to send.  If
                                no buffer is provided, then smd_notify_write
                                must be called when a new transmit buffer is
                                available. The call to smd_notify_write will
                                cause the SMD task to call this callback to
                                get the buffer.  This callback is called in
                                the SMD task context and MUST NOT BLOCK OR
                                WAIT.

@param[in]  rx_callback_fn      This function is called when SMD has finished
                                writing data into the previous receive buffer
                                (if any). It is called with the number of
                                bytes written into the previous buffer. The
                                callback should provide the next read buffer
                                and the space available in the buffer.If the
                                number of bytes written is zero, then there
                                was no previous buffer. If no buffer is
                                provided, then smd_notify_read() must be
                                called when a new receive buffer is available.
                                The call to smd_notify_read() will cause the
                                SMD task to call this callback to get the
                                buffer.  This callback is called in the SMD
                                task context and MUST NOT BLOCK OR WAIT.

@param[in]  rx_flowctl_fn       This function is called when rx_callback_fn
                                returns a NULL buffer and flow control is not
                                enabled. Flow control not enabled means that
                                pending data should be throw away if there's
                                nowhere to put the data. rx_flowctl_fn is
                                called with the number of bytes pending and
                                returns the number of bytes to skip.  For
                                packet mode streams, this can be used to keep
                                packet sync. by skipping bytes to the next
                                header.  For stream mode stream, you typically
                                skip all of the pending bytes.  This callback
                                is called in the SMD task context and MUST NOT
                                BLOCK OR WAIT.

@param[in]  smd_param           Optional paramaters used to configure the SMD
                                port. If this pointer is NULL, the default
                                values will be used instead. See \a
                                smd_open_param_type for detailed documentation
                                on using the optional paramaters.

@return       The SMD port id of the newly-opened port

@sideeffect   Allocates an smd stream info structure

@dependencies
              - Shared memory must have been initialized on this processor by
                calling smem_init().
              - The Shared Memory Driver must be up and running. The startup
                process is platform-specific.

@ingroup smd_buffer
*/
/*=========================================================================*/
uint32 smd_open_memcpy_3
(
  const char                *port_name,
  smd_channel_type           channel_type,
  void                      *callback_data,
  smd_memcpy_tx_callback_fn  tx_callback_fn,
  smd_memcpy_rx_callback_fn  rx_callback_fn,
  smd_memcpy_rx_flowctl_fn   rx_flowctl_fn,
  const smd_open_param_type *smd_param
);

/** @} */ /* end_addtogroup smd_buffer */

/** @addtogroup smd_buffer
@{ */

/*===========================================================================
  FUNCTION  smd_close
===========================================================================*/
/**
  Closes a shared memory port. The callback is called when the port has been 
  closed. 

  @param[in] port_id    SMD port ID.
  @param[in] close_cb   Callback that is called when the port is actually closed.
  @param[in] cb_data    User data passed to the close callback routine.
  @param[in] flush      Flushes the port before closing.\n 
                        If TRUE, the close callback is called when all of the 
                        pending data has been  received by the other processor. 
                        The flush operation is not complete until the SMD task 
                        calls tx_callback_fn and does <i>not</i> get another 
                        transmit buffer (i.e., the higher level transmit queue 
                        is also empty). If the other side of the port closes 
                        before the flush is complete, any data pending in the 
                        shared memory pipe is lost.\n 
                        If FALSE, the close callback is called as soon as 
                        possible, and there may be data remaining in the transmit 
                        buffers and shared memory pipe (and in any higher level 
                        queues).

  @return
  None.

  @dependencies
  The port must have been opened by smd_open_memcpy(), smd_open_memcpy_2(), 
  smd_open_memcpy_sz(), or smd_open_memcpy_3().
*/
/*==========================================================================*/
void smd_close
(
  smd_port_id_type      port_id,
  smd_close_callback_fn close_cb,
  void                  *cb_data,
  boolean               flush
);

/*===========================================================================
  FUNCTION  smd_ioctl
===========================================================================*/
/**
  Provides ways to control various features of the SMD interface.
  
  The behavior of this function depends on the command issued. The command 
  parameter is used to control the behavior of the SMD port.

  The following commands are handled immediately:
  - SMD_IOCTL_DTE_READY_ASSERTED -- Retrieves the state of the Data Terminal 
                                    Ready signal.
  - SMD_IOCTL_GET_CURRENT_RTS    -- Retrieves the state of the RTS signal.
  - SMD_IOCTL_GET_FLOW_CTL       -- Retrieves the flow control method for the 
                                    port.
  - SMD_IOCTL_GET_CURRENT_CD     -- Retrieves the state of the Carrier Detect 
                                    signal.
  - SMD_IOCTL_GET_CURRENT_RI     -- Retrieves the state of the Ring Indication 
                                    signal.
  - SMD_IOCTL_GET_BUFFER_SIZE    -- Retrieves the state of the Buffer Size
                                    control.
  - SMD_IOCTL_GET_DROP_PEND      -- Retrieves the state of the drop pending
                                    control.
  - SMD_IOCTL_GET_FULL_RCV_BUFS  -- Retrieves the state of the Full Receive 
                                    Buffers control.

  The following commands are handled by the smd_task:
  - SMD_IOCTL_CD_ASSERT             -- Asserts the Carrier Detect signal.
  - SMD_IOCTL_CD_DEASSERT           -- Deasserts the Carrier Detect signal.
  - SMD_IOCTL_RI_ASSERT             -- Asserts the Ring Indicator signal.
  - SMD_IOCTL_RI_DEASSERT           -- Deasserts the Ring Indicator signal.
  - SMD_IOCTL_ENABLE_DTR_EVENT      -- Registers the DTR-Toggle callback function
  - SMD_IOCTL_DISABLE_DTR_EVENT     -- Deregisters the DTR-Toggle callback 
                                       function.
  - SMD_IOCTL_INBOUND_FLOW_ENABLE   -- Enables the inbound flow.
  - SMD_IOCTL_INBOUND_FLOW_DISABLE  -- Disables the inbound flow.
  - SMD_IOCTL_DSR_ASSERT            -- Asserts the DSR signal.
  - SMD_IOCTL_DSR_DEASSERT          -- Deasserts the DSR signal.
  - SMD_IOCTL_SET_FLOW_CTL          -- Allows the flow control method to be set.
  - SMD_IOCTL_SET_XFR_MODE          -- Sets the transfer type for the device.
  - SMD_IOCTL_ENABLE_DTR_EVENT_EXT  -- Registers the extended DTR-Toggle callback 
                                       function.
  - SMD_IOCTL_DISABLE_DTR_EVENT_EXT -- Deregisters the extended DTR-Toggle 
                                       callback function.
  - SMD_IOCTL_ENABLE_CD_EVENT_EXT   -- Registers the extended CD-Toggle callback 
                                       function.
  - SMD_IOCTL_DISABLE_CD_EVENT_EXT  -- Deregisters the extended CD-Toggle callback 
                                       function.
  - SMD_IOCTL_ENABLE_RI_EVENT_EXT   -- Registers the extended RI-Toggle callback 
                                       function.
  - SMD_IOCTL_DISABLE_RI_EVENT_EXT  -- Deregisters the extended RI-Toggle callback 
                                       function.
  - SMD_IOCTL_ENABLE_CTS_EVENT_EXT  -- Registers the extended CTS-Toggle callback 
                                       function.
  - SMD_IOCTL_DISABLE_CTS_EVENT_EXT -- Deregisters the extended CTS-Toggle 
                                       callback function.
  - SMD_IOCTL_SET_BUFFER_SIZE       -- Sets the state of the Buffer Size control.
  - SMD_IOCTL_SET_DROP_PEND         -- Sets the state of the Drop Pending control.
  - SMD_IOCTL_SET_FULL_RCV_BUFS     -- Sets the state of the Full Receive Buffers 
                                       control.
  - SMD_IOCTL_REGISTER_MEMCPY_RESET -- Register a memcpy stream reset callback.

  @param[in] port_id      SMD port to control.
  @param[in] command      SMD IOCTL command.
  @param[in,out] param    Pointer to a union containing parameters or results for
                          the command. This can be NULL if the command does not 
                          require any parameters.

  @return
  In param, as required by the command.

  @dependencies
  The port must have been opened by smd_open_memcpy(), smd_open_memcpy_2(), 
  smd_open_memcpy_sz(), or smd_open_memcpy_3().
*/
/*==========================================================================*/
void smd_ioctl
(
  smd_port_id_type      port_id,
  smd_ioctl_cmd_type    command,
  smd_ioctl_param_type *param
);

/*===========================================================================
  FUNCTION  smd_flush_tx
===========================================================================*/
/**
  Flushes the pending transmit data to the other processor.
  
  This callback is only called after the other processor has received all of the 
  pending data. The flush operation is not complete until the SMD task calls 
  the tx_callback_fn and does <i>not</i> get a another transmit buffer (i.e., 
  the higher level transmit queue is also empty).

  If the other side of the port closes before the flush is complete, any
  data pending in the shared memory pipe is lost.

  @param[in] port_id    SMD port to flush.
  @param[in] flush_cb   Callback to call when the other processor has received 
                        all pending data.
  @param[in] cb_data    User data passed to the flush callback routine.

  @return
  None.

  @dependencies
  The port must have been opened by smd_open_memcpy(), smd_open_memcpy_2(), 
  smd_open_memcpy_3(), or smd_open_memcpy_sz().
*/
/*==========================================================================*/
void smd_flush_tx
(
  smd_port_id_type      port_id,
  smd_flush_callback_fn flush_cb,
  void                  *cb_data
);

/*===========================================================================
  FUNCTION  smd_notify_read
===========================================================================*/
/**
  Notifies the SMD task that a read buffer is now available for a specified 
  port_id. If data is available to be read in the FIFO buffer, the SMD 
  calls the registered read callback from the SMD task.
  
  @param[in] port_id   SMD port that is to get a read buffer.

  @return
  None.

  @dependencies
  The port must have been opened by smd_open_memcpy(),smd_open_memcpy_2(), 
  smd_open_memcpy_3(), or smd_open_memcpy_sz().
*/
/*==========================================================================*/
void smd_notify_read( smd_port_id_type port_id );

/*===========================================================================
  FUNCTION  smd_notify_write
===========================================================================*/
/**

  Notifies the SMD task that a transmit buffer is now available for a specified 
  port_id. If space is available in the FIFO buffer and the receiver has 
  enabled flow, the SMD calls the registered transmit callback from the SMD
  task.

  @param[in] port_id   SMD port that is to get a transmit buffer.

  @return
  None.

  @dependencies
  The port must have been opened by smd_open_memcpy(), smd_open_memcpy_2(), 
  smd_open_memcpy_3(), or smd_open_memcpy_sz().
*/
/*==========================================================================*/
void smd_notify_write( smd_port_id_type port_id );

/** @} */ /* end_addtogroup smd_buffer */

/** @addtogroup smd
@{ */

/*===========================================================================
  FUNCTION  smd_api_version
===========================================================================*/
/**
  Returns the current version of the SMD API.

  @return
  A 32-bit SMD version ID.

  @dependencies
  None.
*/
/*==========================================================================*/
uint32 smd_api_version(void);

/** @} */ /* end_addtogroup smd */

#ifdef __cplusplus
}
#endif


#endif /* SMD_INTERFACE_H */

