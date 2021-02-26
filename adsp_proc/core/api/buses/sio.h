#ifndef SIO_H
#define SIO_H

/*===========================================================================

                S E R I A L    I / O    S E R V I C E S

                       H E A D E R    F I L E

===========================================================================*/

/**
  @file sio.h 
  @brief 
      This file contains the definitions of the constants, data 
  structures, and interfaces that comprise the SIO API. 
  Only the interfaces declared shall be used by the client for
  accessing the SIO API. 
  
*/ 


/*
Copyright (c) 2013 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary

*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
11/04/13   rc     Ported from mainline

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.                 */
#include "queue.h"      /* Queue declaration package.                       */
#include "dsm.h"        /* Data service buffer definition.                  */





/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/
/** This macro encapsulates the major and minor number and minor number 
    of device and returns device_id/port_id. */
#define DEVICE_ID(major, minor) ((major << 8) | (minor & 0x00ff))

/** This macro retrieves the major number from device id/port id.*/
#define SIO_MAJOR_NUM_OF_DEVICE(device_id) (((device_id & 0xff00) >> 8 ))

/** This macro retrieve the minor number from device id/port id. */
#define SIO_MINOR_NUM_OF_DEVICE(device_id) ((device_id & 0x00ff))

/*---------------------------------------------------------------------------*/
/** Identifies SIO flow control types for devices. It is used to configure 
    flow control for SIO stream. */
/*---------------------------------------------------------------------------*/

typedef enum
{
  SIO_FCTL_BEST = 0,                   /**< -- Best Flow control method.  Can
                                                mean default or no change. */
  SIO_FCTL_OFF,                        /**< -- Flow control disabled */
  SIO_XONXOFF_STRIP_FCTL_FS,           /**< -- Use fail-safe XON/XOFF flow 
                                                control but strip XON/XOFF 
                                                characters from stream */
  SIO_XONXOFF_STRIP_FCTL_NFS,          /**< -- Use non-failsafe XON/XOFF flow
                                                control but strip XON/XOFF 
                                                characters from stream  */
  SIO_CTSRFR_FCTL,                     /**< -- Use CTS/RFR flow control*/
  SIO_XONXOFF_NSTRIP_FCTL_FS,          /**< -- Use fail-safe XON/XOFF flow 
                                                control and leave in stream */
  SIO_XONXOFF_NSTRIP_FCTL_NFS,         /**< -- Use non-failsafe XON/XOFF flow 
                                                control and leave in stream */                                        
  SIO_MAX_FLOW,                        /**< -- For bounds checking only  */
  SIO_CTSRFR_AUTO_FCTL                 /**< -- Use CTS/RFR flow control with
                                                auto RX RFR signal generation  */
} sio_flow_ctl_type;

/*---------------------------------------------------------------------------*/
/** SIO Stream Identifier type.  */
/*---------------------------------------------------------------------------*/
typedef int2 sio_stream_id_type;

/*---------------------------------------------------------------------------*/
/** Identifier for stream which is opened. */
/*---------------------------------------------------------------------------*/
typedef int2 stream_id_type;


/*---------------------------------------------------------------------------*/
/** SIO_NO_STREAM_ID is returned when sio_open or sio_control_open fails. */
/*---------------------------------------------------------------------------*/
#define SIO_NO_STREAM_ID -1


/*---------------------------------------------------------------------------*/
/** Identifies SIO Stream mode type. */
/*---------------------------------------------------------------------------*/
typedef enum    
{
  SIO_DS_AUTODETECT_MODE,          /**< -- AutoDetect Mode (Backwards comp.)    */
  SIO_DS_RAWDATA_MODE,             /**< -- RawData Mode (Backwards compatible)  */
  SIO_DS_PKT_MODE,                 /**< -- Packet mode                          */
  SIO_DM_MODE,                     /**< -- DM mode                              */
  SIO_GENERIC_MODE,                /**< -- Generic Mode.  Most streams should 
                                            use this mode.                      */
  SIO_MAX_MODE                     /**< -- Maximum Mode Value.                  */
} sio_stream_mode_type;

/*---------------------------------------------------------------------------*/
/** Identifies the SMD data transfer mode type.*/
/*---------------------------------------------------------------------------*/
typedef enum    
{
  SIO_SMD_MEMCPY_MODE,          /**< -- SMD memory copy mode */
  SIO_SMD_DM_MODE,              /**< -- SMD Data mover mode */
  SIO_SMD_INVALID_MODE          /**< -- SMD invalid stream mode */
} sio_dataxfr_mode_type;

/*---------------------------------------------------------------------------*/
/** Identifies the SMD transfer flow type.*/
/*---------------------------------------------------------------------------*/
typedef enum    
{
  SIO_SMD_STREAMING_TYPE,        /**< -- SMD stream transfer flow type */
  SIO_SMD_PKTXFR_TYPE,           /**< -- SMD packet transfer flow type */ 
  SIO_SMD_INVALID_TYPE           /**< -- SMD invalid transfer flow type */
} sio_xfrflow_type;

/*---------------------------------------------------------------------------*/
/** Identifies SIO bitrate type.  The unit of measure is bits per second.
 The "ILLEGAL" entries are to retain compile time range checking 
 while keeping the actual values invariant for NVRAM consistency.*/
/*---------------------------------------------------------------------------*/
/* !!!! Must keep siors232.c:sio_is_baud_valid() in sync with this. !!!! */
typedef enum     
{
  SIO_BITRATE_ILLEGAL_1,            /**<-- Illegal bit-rate 1*/
  SIO_BITRATE_ILLEGAL_3,            /**<-- Illegal bit-rate 2*/
  SIO_BITRATE_ILLEGAL_4,            /**<-- Illegal bit-rate 4*/
  SIO_BITRATE_ILLEGAL_5,            /**<-- Illegal bit-rate 5*/
  SIO_BITRATE_ILLEGAL_6,            /**<-- Illegal bit-rate 6*/
  SIO_BITRATE_ILLEGAL_7,            /**<-- Illegal bit-rate 7*/
  SIO_BITRATE_300,                  /**<-- 300  bit-rate     */
  SIO_BITRATE_600,                  /**<-- 600  bit-rate     */
  SIO_BITRATE_1200,                 /**<-- 1200 bit-rate     */
  SIO_BITRATE_2400,                 /**<-- 2400 bit-rate     */
  SIO_BITRATE_4800,                 /**<-- 4800 bit-rate     */
  SIO_BITRATE_9600,                 /**<-- 9600 bit-rate     */
  SIO_BITRATE_14400,                /**<-- 14400 bit-rate    */
  SIO_BITRATE_ILLEGAL_8 = SIO_BITRATE_14400,
  SIO_BITRATE_19200,                /**<-- 19200  bit-rate   */
  SIO_BITRATE_38400,                /**<-- 38400  bit-rate   */
  SIO_BITRATE_57600,                /**<-- 57600  bit-rate   */
  SIO_BITRATE_76800,                /**<-- 76800  bit-rate   */
  SIO_BITRATE_ILLEGAL_9 = SIO_BITRATE_76800,
  SIO_BITRATE_115200,               /**<-- 115200 bit-rate   */
  SIO_BITRATE_230400,               /**<-- 230400 bit-rate   */
  SIO_BITRATE_ILLEGAL_2 = SIO_BITRATE_230400,
  SIO_BITRATE_460800,               /**<-- 460800 bit-rate   */
  SIO_BITRATE_ILLEGAL_10 = SIO_BITRATE_460800,
  SIO_BITRATE_921600,               /**<-- 921600 bit-rate   */
  SIO_BITRATE_1200000,              /**<-- 1200000 bit-rate  */
  SIO_BITRATE_1209677,              /**<-- 1209677 bit-rate  */
  SIO_BITRATE_1250000,              /**<-- 1250000 bit-rate  */
  SIO_BITRATE_1293103,              /**<-- 1293103 bit-rate  */
  SIO_BITRATE_1339286,              /**<-- 1339286 bit-rate  */
  SIO_BITRATE_1388889,              /**<-- 1388889 bit-rate  */
  SIO_BITRATE_1442308,              /**<-- 1442308 bit-rate  */
  SIO_BITRATE_1500000,              /**<-- 1500000 bit-rate  */
  SIO_BITRATE_1562500,              /**<-- 1562500 bit-rate  */
  SIO_BITRATE_1630435,              /**<-- 1630435 bit-rate  */
  SIO_BITRATE_1704545,              /**<-- 1704545 bit-rate  */
  SIO_BITRATE_1785714,              /**<-- 1785714 bit-rate  */
  SIO_BITRATE_2000000,              /**<-- 2000000 bit-rate  */
  SIO_BITRATE_2900000,              /**<-- 2900000 bit-rate  */
  SIO_BITRATE_3000000,              /**<-- 3000000 bit-rate  */
  SIO_BITRATE_3200000,              /**<-- 3200000 bit-rate  */
  SIO_BITRATE_3686400,              /**<-- 3686400 bit-rate  */
  SIO_BITRATE_4000000,              /**<-- 4000000 bit-rate  */
  SIO_BITRATE_HS_CUSTOM,            /**<-- HS custom bit-rate*/      
  SIO_BITRATE_ILLEGAL_11 = SIO_BITRATE_HS_CUSTOM,
  SIO_BITRATE_BEST = 0x7FFE,  /**< -- Best bitrate (default, fastest, etc) */
  SIO_BITRATE_MAX = 0x7FFF    /**< -- For bounds checking only             */
} sio_bitrate_type;



/*---------------------------------------------------------------------------*/
/** Maximum number of streams allowed.*/
/*---------------------------------------------------------------------------*/
#define SIO_MAX_STREAM 128

/*---------------------------------------------------------------------------*/
/**
@name SIO Major numbers
  The following constants are grouped as SIO major numbers. Sio major numbers
  are assigned to each unique driver-interface.
  @{
*/
/*---------------------------------------------------------------------------*/
 
#define SIO_MAJOR_LEGACY        0       /**< SIO Major for Legacy device driver */ 
#define SIO_MAJOR_UART          1       /**< SIO Major for UART driver */ 
#define SIO_NULL_DRIVER         2      /**< SIO Major for NULL driver */

/** 
   @}
*/


/*---------------------------------------------------------------------------*/
/**
@name SIO Minor numbers
  The following constants are grouped as SIO Minor numbers. 
  SIO minor number indemnifies the logical device for device driver.
  Maximum minor numbers allowed per major number is 64.
  @{
*/
/*---------------------------------------------------------------------------*/

/* SIO_LEGACY_MINOR */
#define SIO_MINOR_LEGACY_NULL   0       /**< SIO Minor for NULL Driver */
 
/* UART DRIVER */
#define SIO_MINOR_UART_1        1       /**< SIO Minor for UART 1 */
#define SIO_MINOR_UART_2        2       /**< SIO Minor for UART 2 */
#define SIO_MINOR_UART_3        3       /**< SIO Minor for UART 3 */
#define SIO_MINOR_UART_CXM      4       /**< SIO Minor for UART_CXM */
#define SIO_PORT_MAX            5
/** 
   @}
*/

#define SIO_MAC_ADDR_STR_SIZE 13        

/*---------------------------------------------------------------------------*/
/** sio_port_id_type is used to identify various SIO devices.
    Sio port id is 16 bit number, which comprises SIO_MAJOR_NUMBER and
    SIO_MINOR_NUMBER. (SIO_PORT_ID = DEVICE_ID(SIO_MAJOR_NUMBER,
    SIO_MINOR_NUMBER). Upper byte indicates the major number while lower byte
    indicates the minor number. Clients can use SIO port id to communicate with
    given logical port of a particular driver, where logical port is identified
    by minor number and the driver is identified by major number. SIO major
    number is assigned to each unique driver interface. Each Major number can
    support certain number of minor numbers (logical ports), maximum minor
    number is limited to 64. In other words, drivers can support as many as 64
    logical ports with one major number.  */
/*---------------------------------------------------------------------------*/

typedef enum    
{
    /** Non-existent/NULL SIO Port.*/
  SIO_PORT_NULL = DEVICE_ID(SIO_MAJOR_LEGACY, SIO_MINOR_LEGACY_NULL ), 

  /*--------------------- UART DRIVER --------------------- */

    /** -- SIO Port id for UART 1/main device */
  SIO_PORT_UART_MAIN = DEVICE_ID(SIO_MAJOR_UART , SIO_MINOR_UART_1 ),
    /** -- SIO Port id for UART 2/AUX device */
  SIO_PORT_UART_AUX = DEVICE_ID(SIO_MAJOR_UART , SIO_MINOR_UART_2 ), 
    /** -- SIO Port id for UART 3 device */
  SIO_PORT_UART_THIRD = DEVICE_ID(SIO_MAJOR_UART , SIO_MINOR_UART_3 ), 
    /** -- SIO Port id for UART_CXM device */
  SIO_PORT_UART_CXM = DEVICE_ID(SIO_MAJOR_UART , SIO_MINOR_UART_CXM ), 
} sio_port_id_type;


/*---------------------------------------------------------------------------*/
/** Resolve the maximum port number for a particular device group. */
/*---------------------------------------------------------------------------*/
  #define SIO_PORT_UART_MAX   SIO_PORT_UART_THIRD

/*---------------------------------------------------------------------------*/
/** Identifies the stream type for device.*/
/*---------------------------------------------------------------------------*/

typedef enum
{
  SIO_STREAM_RX_ONLY = 0,                      /**< -- Stream only used for RX  */
  SIO_STREAM_TX_ONLY,                          /**< -- Stream only used for TX  */
  SIO_STREAM_RXTX,                             /**< -- Stream used for RX & TX  */
  SIO_STREAM_MAX                               /**< -- Used for bounds checking */
} sio_stream_type;

/*---------------------------------------------------------------------------*/
/** Function pointer type used by device driver to pass incoming data
 packaged in DSM item to Client.*/
/*---------------------------------------------------------------------------*/

typedef void (*sio_rx_func_ptr_type)( dsm_item_type **);

/*---------------------------------------------------------------------------*/
/** Identifies the usb mode type for SPS subsystem. */
/*---------------------------------------------------------------------------*/

typedef enum
{
  SIO_USB_MODE_LEGACY         = 0,/**< -- Data goes through legacy path */
  SIO_USB_MODE_SPS_USB_BAM    = 1,/**< -- Data goes to USB BAM from another 
                                     BAM,for use with A2 or DMA BAMs */
  SIO_USB_MODE_SPS_USB_SYSTEM = 2,/**< -- Data goes to USB with a CPU setting
                                     up the USB BAM in System mode, for 
                                     use with NAT and possibly Diag */
}sio_usb_mode_enum_type;

/*---------------------------------------------------------------------------*/
/** Identifies the usb endpoint info type. */
/*---------------------------------------------------------------------------*/
typedef struct{
    sio_usb_mode_enum_type mode;
}sio_usb_end_point_info_type;

/*---------------------------------------------------------------------------*/
/** Used by sio client to pass DSM pool information to driver.*/
/*---------------------------------------------------------------------------*/
typedef struct {
    dsm_mempool_id_enum_type dsm_mempool_id;    /**< DSM pool id type. */
    uint32                   mempool_set;       /**< Mempool set. */
    sio_usb_mode_enum_type   usb_mode;          /**< USB mode type. */
    uint32                   smd_fifo_size;     /**< Fifo size for SMD. */
}sio_dsm_pool_type;

/*---------------------------------------------------------------------------*/
/** Identifies the special param open Parameters : to pass special parameter to driver in
 sio_open() and sio_control_open() function calls. */
/*---------------------------------------------------------------------------*/

typedef union sio_open_param{
    sio_dsm_pool_type           dsm_pool_param;     /**< Dsm pool information. */
    sio_usb_end_point_info_type usb_end_point_info; /**< To be set appropriately for 
                                                       data SIO ports (RmNET or PPP). */
}sio_open_param_type;


/*---------------------------------------------------------------------------*/
/** A variable of this type needs to be passed to sio_open.
 sio_open will not modify any of the parameters (hence declaration of
 parameter to sio_open as "const").*/
/*---------------------------------------------------------------------------*/

typedef struct
{
  sio_stream_id_type    stream_id;          /**< Stream ID Type.  Filled in
                                               by SIO for internal use.    */
  sio_stream_type       stream_type;        /**< Type of stream being opened.
                                               Filled in by SIO for 
                                               internal use only.          */
  sio_stream_mode_type  stream_mode;        /**< Stream Mode Type.           */
  dsm_watermark_type   *rx_queue;           /**< Received Data Queue.         */
  dsm_watermark_type   *tx_queue;           /**< Transmit Data Queue.         */
  sio_bitrate_type      rx_bitrate;         /**< Bit-rate for reception.      */
  sio_bitrate_type      tx_bitrate;         /**< Bit-rate for transmission.   */
  sio_port_id_type      port_id;            /**< Port which needs to be used. */
  boolean               tail_char_used;     /**< True, if tail character is used*/
  
  byte                  tail_char;          /**< If tail character is used,
                                               this is the tail character. */

  sio_rx_func_ptr_type  rx_func_ptr;        /**< If non-NULL, this function 
                                               will be called for each. 
                                               packet which is received.   */
  sio_flow_ctl_type     tx_flow;            /**< TX flow control method.      */
  sio_flow_ctl_type     rx_flow;            /**< RX flow control method.      */

  struct bt_spp_open_struct*  bt_open_ptr;  /**< Additional data to open Bluetooth port via SIO.   */
  sio_open_param_type   *open_param;        /**< special open parameter for driver. */
  uint32                forward_bridge_id;  /**< used only for smd_bridge by SIO (not for client use). */
} sio_open_type;

/*---------------------------------------------------------------------------*/
/** Identifies the different return values of SIO operation.*/
/*---------------------------------------------------------------------------*/

typedef enum
{
  SIO_DONE_S=0,                    /**< -- SIO done status, for successful case. */
  SIO_BADP_S,                      /**< -- Bad parameter.                        */
  SIO_UNAVAIL_S,                   /**< -- No more streams can be allocated.    */
  SIO_BUSY_S,                      /**< -- Command is still being processed.    */
  SIO_RX_NOT_OPEN_S,               /**< -- RX was not opened for this stream.    */
  SIO_TX_NOT_OPEN_S,               /**< -- TX was not opened for this stream.    */
  SIO_BITRATE_CHANGE_FAILED_S,     /**< -- Attempt to change bitrate failed.     */
  SIO_CLOSED_PORT_S,               /**< -- Accessing a closed port               */
  SIO_PORT_POWERED_DOWN_S,         /**< -- Accessing a powered down port         */
  SIO_MAX_S                        /**< -- For bounds checking only.             */
} sio_status_type;

/*---------------------------------------------------------------------------*/
/** These are the commands which can be carried out by sio_ioctl.
    These are to be used in conjunction with the above command type.*/
/*---------------------------------------------------------------------------*/

typedef enum
{
  SIO_IOCTL_CD_ASSERT=0,             /**< -- Assert Carrier Detection Indicator   */
  SIO_IOCTL_CD_DEASSERT,             /**< -- Deassert Carrier Detection Indicator */
  SIO_IOCTL_GET_CURRENT_CD,          /**< -- Return a boolean (in parameter) which
                                        indicates whether or not the CD
                                        signal is asserted                   */
  SIO_IOCTL_RI_ASSERT,               /**< -- Assert Ring Indication               */
  SIO_IOCTL_RI_DEASSERT,             /**< -- Dassert Ring Indication              */
  SIO_IOCTL_GET_CURRENT_RI,          /**< -- Return a boolean (in parameter) which
                                        indicates whether or not the RI
                                        signal is asserted                   */
  SIO_IOCTL_ENABLE_DTR_EVENT,        /**< -- Register clock call-back which 
                                        informs caller of change in state of
                                        data terminal ready indication       */  
  SIO_IOCTL_DISABLE_DTR_EVENT,       /**< -- De-register clock call-back for
                                        informing of change in state of 
                                        data terminal ready indication       */
  SIO_IOCTL_INBOUND_FLOW_ENABLE,     /**< -- Enable inbound flow using current
                                        flow control method                  */
  SIO_IOCTL_INBOUND_FLOW_DISABLE,    /**< -- Disable inbound fow using current
                                        flow control method                  */
  SIO_IOCTL_DSR_ASSERT,              /**< -- Assert Data Set Ready Indication     */
  SIO_IOCTL_DSR_DEASSERT,            /**< -- Deassert Data Set Ready Indication   */
  SIO_IOCTL_DTE_READY_ASSERTED,      /**< -- Return a boolean (in parameter) which
                                        indicates whether or not DTE ready
                                        signal is asserted                   */
  SIO_IOCTL_GET_CURRENT_RTS,         /**< -- Read the current status of RTS from
                                        the PC(DTE),i.e. the MSM(DCE) CTS pin*/
  SIO_IOCTL_SET_FLOW_CTL,            /**< -- Allows setting of flow control 
                                        method                               */
  SIO_IOCTL_GET_FLOW_CTL,            /**< -- Allows getting the flow control
                                          methods                            */
  SIO_IOCTL_ENABLE_AUTODETECT,       /**< -- Enable AutoDetect.  Has no effect if
                                        AutoDetect already enabled.          */
  SIO_IOCTL_RECORD_ESC_FUNC_PTR,     /**< -- Register clock call-back function
                                        which informs caller of arrival of
                                        escape code ( +++ )                  */
  SIO_IOCTL_RECORD_PKT_FUNC_PTR,     /**< -- Register clock call-back function
                                        which informs caller of arrival of
                                        PPP packet                           */
  SIO_IOCTL_BEGIN_FAX_IMAGE,         /**< -- Used by caller to signal beginning
                                        of fax image.  From the beginning of
                                        fax image to the end of fax image on
                                        given line, software flow control 
                                        needs to be ignored because they 
                                        may be unescaped data bytes          */
  SIO_IOCTL_CHANGE_BAUD_RATE,        /**< -- Used to change the baud rate of the  
                                        serial port.  THIS IS FOR BACKWARD
                                        COMPATIBILITY WITH DATA SERVICES ONLY.
                                        The change takes place _only_  _after_
                                        a transition from a non-empty TX FIFO
                                        to an empty TX FIFO. If the TX FIFO
                                        is empty when this ioctl is received, 
                                        no baud change occurs until more data 
                                        is Tx'd.                             */
  SIO_IOCTL_CHANGE_BAUD_NOW,         /**< -- Used to change the baud rate of the
                                        serial port.  Change takes place 
                                        immediately without regard to any data
                                        in the TX FIFO. Hence it is up to the 
                                        app to flush the tx beforehand. The 
                                        usage is otherwise identical to 
                                        SIO_IOCTL_CHANGE_BAUD_RATE.          */
  SIO_IOCTL_FLUSH_TX,                /**< -- Register call-back function which
                                        informs caller when all pending
                                        output data has been transmitted
                                        WARNING: Unreliable results may occur
                                        if data is queued after this function
                                        call is made and before the callback
                                        function has been called.            */
  SIO_IOCTL_CHANGE_MODE,             /**< -- Change the serial port's mode, i.e.,
                                        from AUTODETECT to RAWDATA for
                                        example                              */
  SIO_IOCTL_REG_TLP_CHANGE_CB,       /**< -- Register a callback to be invoked when
                                        the version and/or use of the USB TLP
                                        changes.  Callback invoked immediately
                                        to provide initial state.            */
  SIO_IOCTL_GET_PACKET_SIZE,         /**< -- Read the maximum packet size for
                                        packet devices */
  SIO_IOCTL_SET_PACKET_SIZE,         /**< -- Change the maximum packet size for
                                        packet devices */
  SIO_IOCTL_GET_BUFFER_SIZE,         /**< -- Read the buffer size for devices that
                                        have buffers */
  SIO_IOCTL_SET_BUFFER_SIZE,         /**< -- Change the buffer size for devices
                                        that have variable buffers */
  SIO_IOCTL_GET_XFR_MODE,            /**< -- Read the transfer mode for device */
  SIO_IOCTL_SET_XFR_MODE,            /**< -- Set the transfer mode for device */
  SIO_IOCTL_GET_XFR_TYPE,            /**< -- Read the transfer type for device */
  SIO_IOCTL_SET_XFR_TYPE,            /**< -- Set the transfer type for device */
  SIO_IOCTL_ENABLE_DTR_EVENT_EXT,    /**< -- Register callback which informs caller
                                        of change in state of data terminal
                                        ready indication - callback gets
                                        port_id and user data as args */  
  SIO_IOCTL_DISABLE_DTR_EVENT_EXT,   /**< -- De-register call-back for informing of
                                        change in state of data terminal ready
                                        indication       */
  SIO_IOCTL_ENABLE_CD_EVENT_EXT,     /**< -- Register callback which informs caller
                                        of change in state of carrier detect
                                        indication - callback gets port_id 
                                        and user data as args */  
  SIO_IOCTL_DISABLE_CD_EVENT_EXT,    /**< -- De-register call-back for informing of
                                        change in state of carrier detect
                                        indication       */
  SIO_IOCTL_ENABLE_RI_EVENT_EXT,     /**< -- Register callback for informing of
                                        change in state of ring indication  */
  SIO_IOCTL_DISABLE_RI_EVENT_EXT,    /**< -- De-register callback for informing of
                                        change in state of ring indication  */
  SIO_IOCTL_SET_DROP_PEND,           /**< -- Set the Drop Pending state  */
  SIO_IOCTL_GET_DROP_PEND,           /**< -- Get the Drop Pending state  */
  SIO_IOCTL_BT_CONFIGURE,            /**< -- Configure Bluetooth port.            */
  SIO_IOCTL_BT_DISCONNECT,           /**< -- Disconnect Bluetooth port.           */
  SIO_IOCTL_BT_GET_STATUS,           /**< -- Get Bluetooth status.                */
  SIO_IOCTL_POWERDOWN,               /**< -- Power down the port                  */
  SIO_IOCTL_WAKEUP,                  /**< -- Wake up the port                     */
  SIO_IOCTL_TX_START_BREAK,          /**< -- Start a break event on the TX side   */
  SIO_IOCTL_TX_END_BREAK,            /**< -- End a break event on the TX side     */
  SIO_IOCTL_RX_HIGH_EVENT,           /**< -- Register a RX line high event CB     */
  SIO_IOCTL_CHECK_RX_STATE,          /**< -- Get the current state of RX line     */ 
  SIO_IOCTL_WM_SET_WAIT,             /**< -- Sets the wait flag in SMD            */
  SIO_IOCTL_WM_GET_WAIT,              /**< -- Gets the wait flag in SMD            */
  SIO_IOCTL_NETWORK_CONNECTION_NOTIF, /**< -- used to notify the host whether the physical 
                                         link is up. Triggers sending NETWORK_CONNECTION 
                                         notification on the ECM control pipe*/
  SIO_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF, /**< -- Triggers sending the CONNECTION_SPEED_CHANGE 
                                         notification to the host on the ECM control pipe.
                                         This notification is sent to the device by the host 
                                         in order to notify the host upon change in the 
                                         upstream/downstream speed connection */
  SIO_IOCTL_ENABLE_SET_PACKET_FILTER_EVENT, /**< -- This IOCTL is used by the application to provide 
                                         the AL with a cb to be invoked once SET_PACKET_FILTER
                                         notification is received from the host on the ECM
                                         control pipe */
  SIO_IOCTL_GET_MAC_ADDRESS,           /**< -- Used by the application to retrieve the MAC 
                                          address from the ECM AL */
  SIO_IOCTL_IS_STANDARD_ECM,           /**< -- Used by the application to get ECM operational mode:
                                          standard or proprietary */
  SIO_IOCTL_DIAG_CONFIG,               /**< -- Mask-configuration change in On-device Diag */
  SIO_IOCTL_DIAG_QUERY,                /**< -- Query On-device Diag parameters */
  SIO_IOCTL_INACTIVITY_TIMER,          /**< -- Set an inactivity timer for the device.  The 
                                               device will power down if there's no activity
                                               for the timer duration.  Set the inactivity
                                               timer to 0 to clear it (turn it off).
                                               Not all devices support inactivity timer.
                                         */
  SIO_IOCTL_DATA_NOTIFY,               /**< -- Notify the device that data is
                                               available, if the device is
                                               aggregating data. */
  SIO_IOCTL_GET_STATUS,                /**< -- Get status */
  SIO_IOCTL_SET_LOOPBACK_MODE,         /**< -- Enable or disable loopback mode */
  SIO_IOCTL_SET_CXM,                   /**< -- Enable or disable two-wire
                                               LTE/ISM coexistence */
  SIO_IOCTL_GET_CXM_TX,                /**< -- Returns the 802.11 TX state */
  SIO_IOCTL_CXM_TX_DIRECT_CHAR,         /**< -- Send a char using direct option (CXM only) w/o using SW TX FIFO */
  SIO_IOCTL_CANCEL_FLUSH_TX,           /**< -- Cancel a pending flush operation */
  SIO_IOCTL_PURGE_TX,                  /**< -- Cancels any active transfer and frees up the queued DSM items*/
} sio_ioctl_cmd_type;


/*---------------------------------------------------------------------------*/
/** Default parameter to be used with ioctl command.  Should
  only be used as place-holder for operations which do not
  require parameters. */
/*---------------------------------------------------------------------------*/
typedef word sio_ioctl_default_param_type;

/*---------------------------------------------------------------------------*/
/** It is used to specify TX flow and RX flow
    control methods. Because standard allows setting of these
    methods separately, SIO interface has to do so as well.    */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_flow_ctl {
  sio_flow_ctl_type tx_flow;    /**< Transmit flow control type. */
  sio_flow_ctl_type rx_flow;    /**< Receive flow control type. */
} sio_ioctl_flow_ctl_type;

/*---------------------------------------------------------------------------*/
/** Structure with current TLP settings to be passed to callback
    registered via SIO_IOCTL_REG_TLP_CHANGE_CB ioctl. */
/*---------------------------------------------------------------------------*/
typedef struct
{
  uint8    version;             /**< Version for TLP settings. */
  boolean  downlink_tlp_on;     /**< Downlink TLP on/off. */
  boolean  uplink_tlp_on;       /**< Uplink TLP on/off. */
} sio_ioctl_usb_tlp_settings_type;

/*---------------------------------------------------------------------------*/
/** Prototype for callback registered via SIO_IOCTL_REG_TLP_CHANGE_CB ioctl. */
/*---------------------------------------------------------------------------*/
typedef void (* sio_usb_tlp_change_cb_f_type)
(
  sio_ioctl_usb_tlp_settings_type,
  void *
);

/*---------------------------------------------------------------------------*/
/** Structure to be passed as SIO_IOCTL_REG_TLP_CHANGE_CB ioctl parameter. */
/*---------------------------------------------------------------------------*/
typedef struct
{
  sio_usb_tlp_change_cb_f_type  callback;    /**< Function to call when TLP
                                                settings change.            */
  void *                        user_data;   /**< will be passed to callback. */
} sio_ioctl_usb_reg_tlp_change_cb_type;

/*---------------------------------------------------------------------------*/
/** Prototype for callback registered via
    SIO_IOCTL_ENABLE_SET_PACKET_FILTER_EVENT ioctl.  */
/*---------------------------------------------------------------------------*/
typedef void (* sio_enable_set_packet_filter_event_func_ptr_type)
(
  uint16 packet_filter_bitmask,  /**< Packet filter bitmask.*/
  void *cb_data                  /**< Pointer to callback data.*/
);

/*---------------------------------------------------------------------------*/
/** Structure to be passed as
    SIO_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF ioctl parameter. */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_connection_speed_change_notif
{
  uint32                         upstream_bitrate;   /**< Upstream bit rate.*/
  uint32                         downstream_bitrate; /**< Downstream bit rate.*/
}sio_ioctl_connection_speed_change_notif_type;

/*---------------------------------------------------------------------------*/
/** Structure to be passed as
    SIO_IOCTL_ENABLE_SET_PACKET_FILTER_EVENT ioctl parameter.  */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_enable_set_packet_filter_event
{
  sio_enable_set_packet_filter_event_func_ptr_type cb_func;
  void *                cb_data;
} sio_ioctl_enable_set_packet_filter_event_type;

/*---------------------------------------------------------------------------*/
/** Function pointer type which points to a function that takes no arguments
 and returns nothing (i.e. void). */
/*---------------------------------------------------------------------------*/

typedef void (*sio_vv_func_ptr_type)( void);

/*---------------------------------------------------------------------------*/
/** Function pointer type which points to a function that takes a uint32 as 
 arguments and returns nothing (i.e void). This is meant to be
 used for callbacks.*/
/*---------------------------------------------------------------------------*/
typedef void (*sio_vpu_func_ptr_type)(void * cb_data);


/*---------------------------------------------------------------------------*/
/** Structure to be passed as SIO_IOCTL_ENABLE_DTR_EVENT_EXT ioctl parameter. */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_enable_dtr_event_ext{
  sio_vpu_func_ptr_type cb_func;
  void *                cb_data;
} sio_ioctl_enable_dtr_event_ext_type;

/*---------------------------------------------------------------------------*/
/** Structure to be passed as SIO_IOCTL_ENABLE_CD_EVENT_EXT ioctl parameter.  */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_enable_dtr_event_ext sio_ioctl_enable_cb_event_ext_type;

/*---------------------------------------------------------------------------*/
/** Structure to be passed as SIO_IOCTL_ENABLE_RI_EVENT_EXT ioctl parameter.  */
/*---------------------------------------------------------------------------*/
typedef struct sio_ioctl_enable_dtr_event_ext sio_ioctl_enable_ri_event_ext_type;

/*---------------------------------------------------------------------------*/
/** Structure to get UART status information with SIO_IOCTL_GET_STATUS        */
/*---------------------------------------------------------------------------*/
typedef struct
{
   boolean                 is_clock_on;        // True = clock is on, False = off
   // Add new status fields here
} uart_status_type;

/*---------------------------------------------------------------------------*/
/** Structure to configure two-wire coexistence with SIO_IOCTL_SET_CXM       */
/*---------------------------------------------------------------------------*/
typedef struct
{
  /** Enable or disable two-wire coexistence protocol */
  boolean               enable_cxm;

  /** The two-bit station address included in certain CXM messages */
  uint32                sam;
} uart_cxm_type;

/*---------------------------------------------------------------------------*/
/** Structure to get the 802.11 sticky transmit status when using two-wire
 * coexistence with SIO_IOCTL_GET_CXM_TX                                     */
/*---------------------------------------------------------------------------*/
typedef struct
{
  /** Output: the state of the sticky flag */
  uint32                tx_sticky;

  /** Input: whether to clear the sticky bit */
  boolean               clear;
} uart_cxm_tx_type;

/*---------------------------------------------------------------------------*/
/** Structure to send a char in direct mode for CXM UART when using
 * coexistence with SIO_IOCTL_CXM_TX_DIRECT_CHAR (Bypass SW TX FIFO          */
/*---------------------------------------------------------------------------*/
typedef struct
{
  /** Output: the state of the sticky flag */
  uint8                character;
} uart_cxm_tx_direct_char_type;

/*---------------------------------------------------------------------------*/
/** Aggregate SIO IOCTL parameter type. It is used to pass additional parameter
    in sio_ioctl() or sio_control_ioctl(). */
/*---------------------------------------------------------------------------*/

typedef union sio_ioctl_param {
  sio_ioctl_default_param_type   carrier_detect_assert;     /**< Carrier detect assert */
  sio_ioctl_default_param_type   carrier_detect_deassert;   /**< Carrier detect deassert */
  sio_ioctl_default_param_type   ring_indicator_assert;     /**< Ring indicator assert */
  sio_ioctl_default_param_type   ring_indicator_deassert;   /**< Ring indicator deassert */
  sio_vv_func_ptr_type           enable_dte_ready_event;    /**< Enable dte ready event */
  sio_ioctl_default_param_type   disable_dte_ready_event;   /**< Disable dte ready event */
  sio_ioctl_default_param_type   inbound_flow_enable;       /**< Inbound flow enable */
  sio_ioctl_default_param_type   inbound_flow_disable;      /**< Inbound flow disable */
  sio_ioctl_default_param_type   data_set_ready_assert;     /**< Data set ready assert */
  sio_ioctl_default_param_type   data_set_ready_deassert;   /**< Data set ready deassert */
  boolean                       *dte_ready_asserted;        /**< DTE ready asserted */
  boolean                       *rts_asserted;              /**< RTS asserted */
  boolean                       *cd_asserted;               /**< Carrier detect asserted */
  boolean                       *ri_asserted;               /**< Ring indicator asserted */
  sio_ioctl_flow_ctl_type        flow_ctl;                  /**< Flow control type */
  sio_ioctl_default_param_type   rearm_autobaud;            /**< Rearm Auto baudrate */
  sio_vv_func_ptr_type           record_escape_func_ptr;    /**< Record escape function pointer */
  sio_vv_func_ptr_type           record_pkt_func_ptr;       /**< Record packet function pointer */
  sio_ioctl_default_param_type   begin_fax_image;           /**< Begin fax image */
  sio_bitrate_type               bitrate;                   /**< Bitrate type */
  sio_stream_mode_type           stream_mode;               /**< Stream mode type */
  sio_open_type                 *open_ptr;                  /**< Sio Open type */
  sio_vv_func_ptr_type           record_flush_func_ptr;     /**< Record flush function pointer */
  uint32                         packet_size;               /**< Packet size */
  uint32                         buffer_size;               /**< Buffer size */
  boolean                        drop_asserted;             /**< Drop asserted*/
  sio_dataxfr_mode_type          dataxfr_mode;              /**< Data transfer mode */
  sio_xfrflow_type               xfrflow_type;              /**< Transfer flow type*/
  sio_ioctl_enable_dtr_event_ext_type   enable_dte_ready_event_ext; /**< Enable DTE ready event ext */
  sio_ioctl_enable_cb_event_ext_type    enable_cd_ready_event_ext;  /**< Enable CD ready event ext */
  sio_ioctl_enable_ri_event_ext_type    enable_ri_ready_event_ext;  /**< Enable RI ready event ext */
  sio_ioctl_usb_reg_tlp_change_cb_type  tlp_change_cb;      /**< TLP change callback */        
  struct bt_spp_config_struct*   bt_spp_config_ptr;         /**< Bluetooth_SPP config pointer */
  struct bt_spp_status_struct*   bt_spp_status_ptr;         /**< Bluetooth SPP status pointer*/
  sio_vv_func_ptr_type           rx_high_event_func_ptr;    /**< RX high event function pointer*/
  uint32                         inactivity_ms;             /**< Inactivity timer */
  boolean                       *rx_line_state;             /**< RX line state */
  boolean                        wm_wait;                   /**< WM wait*/
  boolean                        link_up;                   /**< Link up*/
  sio_ioctl_connection_speed_change_notif_type connection_speed_change_notif;   /**< Connection speed change notification*/
  sio_ioctl_enable_set_packet_filter_event_type set_packet_filter_event;        /**< Set packet filter event*/
  uint8                          mac_address[SIO_MAC_ADDR_STR_SIZE];  /**< Mac address*/
  boolean                        is_standard_ecm;                     /**< Standard ECM boolean*/
  void                          *status_ptr;                /**< Status pointer */
  boolean                        enable_loopback_mode;      /**< Enable loopback mode */
  uart_cxm_type                  set_cxm;                   /**< Enable coexistence */
  uart_cxm_tx_type               cxm_tx;                    /**< Get 802.11 tx flag */
  uart_cxm_tx_direct_char_type   cxm_tx_direct_char;        /**< Tx char in direct mode (CXM UART)*/
} sio_ioctl_param_type;


  /*~ PARTIAL sio_ioctl_param */
  /*~ CASE SIO_IOCTL_CHANGE_BAUD_RATE sio_ioctl_param.rearm_autobaud */
  /*~ CASE SIO_IOCTL_CHANGE_BAUD_NOW sio_ioctl_param.rearm_autobaud */
  /*~ CASE SIO_IOCTL_NETWORK_CONNECTION_NOTIF sio_ioctl_param.link_up */
  /*~ CASE SIO_IOCTL_IS_STANDARD_ECM sio_ioctl_param.is_standard_ecm */
  /*~ CASE SIO_IOCTL_GET_MAC_ADDRESS sio_ioctl_param.mac_address */
  /*~ CASE SIO_IOCTL_CONNECTION_SPEED_CHANGE_NOTIF sio_ioctl_param.connection_speed_change_notif */
  /*~ DEFAULT sio_ioctl_param.void */

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*=========================================================================*/
/** 
@brief 
  This function opens a stream across the serial link. 

  This function calls the driver's open api associated with port_id being
  passed by client in *open_ptr. This function creates a stream connection 
  between the calling application and the specified hardware port. Assuming 
  a successful completion, the stream_id returned is used to specify this 
  stream in future SIO API calls.

@param[in]  *open_ptr -- pointer to sio_open_type structure

        Client defined members in *open_ptr:
        <ul>
        <li>stream_mode -- The mode specifies the internal SIO behavior required 
                       of this stream. Most new applications should consider 
                       using the SIO_GENERIC_MODE.
        -- SIO_DS_AUTODETECT_MODE and SIO_DS_RAWDATA_MODE support backward 
           compatibility for existing data applications.
        -- SIO_DS_PKT_MODE specifies Packet Data mode.
        -- SIO_DM_MODE supports the DIAG application for communication with 
            the external Diagnostic Module program.

        <li>rx_queue,tx_queue -- These members contain pointers to 
                             dsm_watermark_type structures that describe 
                             the Rx and Tx watermark queues

        <li>rx_bitrate,tx_bitrate -- For UART ports these will determine the 
                        actual baud rate used. Currently, the Rx bit rate 
                        and Tx bit rate must be the same.
        <li>port_id -- The port_id specifies which hardware device to open
                   for the calling application.

        <li>tail_char_used -- This is a Boolean variable, which specifies
                          whether or not a tail character is used to delimit 
                          packets in the data stream.

        <li>tail_char -- If the tail_char_used flag above is nonzero, 
                    the tail character must be specified here.

        <li>rx_func_ptr -- If this pointer is non-NULL, it is assumed to point
                       to a function to be called for each packet received.

        <li>tx_flow, rx_flow -- These specify the Tx and Rx flow control 
                            methods to be used on this port.
        </ul>


@dependencies 
  sio_init must have been called.  sio_init needs to be called by just one
  task and only once upon power-up.

@sideeffects 
  None

@return 
  If successful, a nonnegative stream identifier (that is, a handle) will be 
  returned. This identifier will be used in subsequent SIO requests to specify
   this port.

  If the request could not be completed, the value SIO_NO_STREAM_ID will be 
  returned to indicate the failure, as indicated below.

@sa
  sio_open_type
  sio_stream_id_type
*/ 
/*===========================================================================*/

extern sio_stream_id_type sio_open
( 
  sio_open_type *open_ptr                      /* Configurations of stream */
);

/*==========================================================================*/
/**
@brief
  This procedure does all the book-keeping necessary to close a stream.
  Optional tcb_ptr and sigs can be passed to this function, if task wants
  to be signalled when the last bit of pending transmission leaves phone.

  This function will call the close apis of device driver associated with
  stream_id. Closes a serial stream and optionally notifies the caller 
  when all pending output data has been transmitted via the callback 
  function pointer.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@param[in]  *close_func_ptr -- Pointer to the function to be invoked when
      all pending output data has been transmitted. Use NULL if no
      callback is specified.

@dependencies
  None

@sideeffects
  close_func_ptr will be called when all pending data has left
  transmitter.

@return
  None

@sa
  None
*/
/*=====================================================================*/

extern void sio_close
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  void             (*close_func_ptr)(void)    /* Function to call when 
                                                 transmission is complete. */
);

/*=========================================================================*/
/** 
@brief
  This function will transmit a packet or if transmitter is busy, schedule
  the transmission of a packet.

  This function will call driver's transmit api associated with stream_id.
  Device driver will queue the specified data for transmitting.
  The SIO subsystem(driver) will free the dsm item when the data has been sent.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@dependencies
  The specified stream_id must be open and owned by the caller.
  The pointer parameter tx_ptr must point to a valid dsm item.
  
@sideeffects
  None

@return
  None

@sa
  None
*/
/*=========================================================================*/

extern void sio_transmit
( 
  sio_stream_id_type stream_id,    /* Stream ID                            */
  dsm_item_type *tx_ptr            /* Packet which needs to be transmitted */
);

/*=========================================================================*/
/** 
@brief
  This function will transmit any queued data and then call the registered
  callback.

  This function will call driver's flush api, if supported, associated
  with stream_id. callback_function is called by driver when flush is
  completed.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.

@dependencies
  The specified stream_id must be open and owned by the caller.
  
@sideeffects
  None

@return
  None

@sa
  None
*/  
/*=========================================================================*/

extern void sio_flush_tx
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  void                   (*close_func_ptr)(void) /* Function to call when 
                                                 transmission is complete. */    
);


/*=========================================================================*/
/**
@brief
  Allows control of open stream.

  The I/O control operation specified by the cmd and param parameters is performed.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@param[in]  cmd      --  Type of control operation (command) being requested.
@param[in/out] *param -- Extra parameter depending on cmd_type.

@dependencies
  The specified stream_id must be open and owned by the caller.
  The cmd and param parameters must be valid.

@sideeffects
  None

@return
  None (*param may return some value depending on cmd)

@sa
  sio_ioctl_cmd_type
  sio_ioctl_param_type
*/
/*=========================================================================*/

void sio_ioctl
(
  sio_stream_id_type    stream_id, /* Stream ID                            */ 
  sio_ioctl_cmd_type    cmd,       /* IOCTL command                        */
  sio_ioctl_param_type *param      /* Parameter to be used for command     */
);

/*=========================================================================*/
/** 
@brief
  Allows control of open stream.

  The I/O control operation specified by the cmd and param parameters is performed.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@param[in]  cmd      --  Type of control operation (command) being requested.
@param[in/out] *param -- Extra parameter depending on cmd_type.

@dependencies
  The specified stream_id must be open and owned by the caller.
  The cmd and param parameters must be valid.

@sideeffects
  None

@return
  None (*param may return some value depending on cmd)

@sa
  sio_ioctl_cmd_type
  sio_ioctl_param_type
*/
/*=========================================================================*/

void sio_control_ioctl
(
  sio_stream_id_type    stream_id, /* Stream ID                            */ 
  sio_ioctl_cmd_type    cmd,       /* IOCTL command                        */
  sio_ioctl_param_type *param      /* Parameter to be used for command     */
);

/*=========================================================================*/
/** 
@brief
  This routine checks if the specified baud rate is valid under the 
  current feature set.  

  The primary purpose of this utility is to validate baud rate values 
  recovered from NVRAM, which may not be valid in the current feature set.  
  For example, SIO_BITRATE_AUTO is only valid when FEATURE_AUTOBAUD is 
  defined.  

@param[in] check_baud -- Baud rate being checked 

@dependencies
  None
                
@sideeffects
  None

@return
  TRUE if the baud rate is valid under the current feature set.
  FALSE otherwise.

@sa
  None
*/
/*=========================================================================*/

extern boolean sio_is_baud_valid
(
  sio_bitrate_type   check_baud              /* SIO baud rate              */
);

/*=========================================================================*/
/** 
@brief 
  This function opens a control stream across the serial link. 

  This function calls driver's control_open api associated with port_id being
  passed by client in *open_ptr. This function creates a stream connection 
  between the calling application and the specified hardware port. Assuming 
  a successful completion, the stream_id returned is used to specify this 
  stream in future SIO API calls.

@param[in]  *open_ptr -- pointer to sio_open_type structure

        Client defined members in *open_ptr:
        <ul>
        <li>stream_mode -- The mode specifies the internal SIO behavior required 
                       of this stream. Most new applications should consider 
                       using the SIO_GENERIC_MODE.
        -- SIO_DS_AUTODETECT_MODE and SIO_DS_RAWDATA_MODE support backward 
           compatibility for existing data applications.
        -- SIO_DS_PKT_MODE specifies Packet Data mode.
        -- SIO_DM_MODE supports the DIAG application for communication with 
            the external Diagnostic Module program.

        <li>rx_queue,tx_queue -- These members contain pointers to 
                             dsm_watermark_type structures that describe 
                             the Rx and Tx watermark queues

        <li>rx_bitrate,tx_bitrate -- For UART ports these will determine the 
                        actual baud rate used. Currently, the Rx bit rate 
                        and Tx bit rate must be the same.
        <li>port_id -- The port_id specifies which hardware device to open
                   for the calling application.

        <li>tail_char_used -- This is a Boolean variable, which specifies
                          whether or not a tail character is used to delimit 
                          packets in the data stream.

        <li>tail_char -- If the tail_char_used flag above is nonzero, 
                    the tail character must be specified here.

        <li>rx_func_ptr -- If this pointer is non-NULL, it is assumed to point
                       to a function to be called for each packet received.

        <li>tx_flow, rx_flow -- These specify the Tx and Rx flow control 
                            methods to be used on this port.
        </ul>


@dependencies 
  sio_init must have been called.  sio_init needs to be called by just one
  task and only once upon power-up.

@sideeffects 
  None

@return 
  If successful, a nonnegative stream identifier (that is, a handle) will be 
  returned. This identifier will be used in subsequent SIO requests to specify
   this port.

  If the request could not be completed, the value SIO_NO_STREAM_ID will be 
  returned to indicate the failure, as indicated below.

@sa
  sio_open_type
  sio_stream_id_type
*/ 
/*=========================================================================*/
sio_stream_id_type sio_control_open
( 
  sio_open_type *open_ptr                       /* SIO Open structure      */
);

/*=========================================================================*/
/**                                                                             
@brief
  This procedure does all the book-keeping necessary to close a stream.
  Optional tcb_ptr and sigs can be passed to this function, if task wants
  to be signalled when the last bit of pending transmission leaves phone.

  This function will call the control_close apis of device driver associated 
  with stream_id.  Closes a serial stream and optionally notifies the caller 
  when all pending output data has been transmitted via the callback 
  function pointer.  

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@param[in]  *close_func_ptr -- Pointer to the function to be invoked when
      all pending output data has been transmitted. Use NULL if no
      callback is specified.

@dependencies
  None

@sideeffects
  close_func_ptr will be called when all pending data has left
  transmitter.

@return
  None

@sa
  None
*/
/*=========================================================================*/

void sio_control_close
( 
  sio_stream_id_type stream_id,             /* Stream ID                   */
  void             (*close_func_ptr)(void)  /* Function to call when 
                                               transmission is complete.   */
);


/*=========================================================================*/
/**
@brief
  This function will transmit a packet or if transmitter is busy, schedule
  the transmission of a packet.

  This function will call driver's control_transmit api associated with 
  stream_id. Device driver will queue the specified data for transmitting.
  The SIO subsystem(driver) will free the dsm item when the data has been sent.

@param[in]  stream_id -- Identifier of the specified serial stream. 
                         This is the handle returned by the corresponding 
                         sio_open( ) call.
@dependencies
  The specified stream_id must be open and owned by the caller.
  The pointer parameter tx_ptr must point to a valid dsm item.
  
@sideeffects
  None

@return
  None

@sa
  None
*/
/*=========================================================================*/

void sio_control_transmit
( 
  sio_stream_id_type stream_id,           /* Stream ID                     */
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
);

/*---------------------------------------------------------------------------*/
/** Structure used to register Driver side interface. This structure is 
    populated by device driver with corresponding driver's api and is 
    registered with SIO for specific SIO Port owned by device driver.*/
/*---------------------------------------------------------------------------*/

typedef struct
{
    /** *HW_open api to be called when sio_open() is called by client */
    sio_status_type (*HW_open) (sio_open_type *open_ptr);
    /** *HW_close api to be called when sio_close() is called by client */
    void (*HW_close) (sio_stream_id_type stream_id,
                      sio_port_id_type      port_id,
                        void (*close_func_ptr)(void));
    /** *HW_ioctl api to be called when sio_ioclt() is called by client */
    void (*HW_ioctl) (sio_stream_id_type stream_id,
                      sio_port_id_type      port_id,
                      sio_ioctl_cmd_type    cmd,
                      sio_ioctl_param_type *param);
    /** *HW_transmit api to be called when sio_transmit() is called by client */
    void (*HW_transmit) ( sio_stream_id_type stream_id,
                      sio_port_id_type   port_id,
                      dsm_item_type     *tx_ptr);
    /** *HW_flush_tx api to be called when sio_flush_tx() is called by client */
    void (*HW_flush_tx) (sio_stream_id_type stream_id,
                         sio_port_id_type   port_id,
                         void (*flush_func_ptr)(void));
    /** *HW_disable_device api to be called when sio_disable_device()
     is called by client */
    void (*HW_disable_device) (sio_port_id_type   port_id);
    
    /** *HW_control_open api to be called when sio_control_open()
    is called by client */
    sio_status_type (*HW_control_open) (sio_open_type *open_ptr);
    /** *HW_control_close api to be called when sio_control_close()
     is called by client */
    void (*HW_control_close) (sio_stream_id_type stream_id,
                              sio_port_id_type      port_id,
                              void (*close_func_ptr)(void));
    /** *HW_control_ioctl api to be called when sio_control_ioctl() is
    called by client */
    void (*HW_control_ioctl) (sio_stream_id_type stream_id,
                             sio_port_id_type      port_id,
                             sio_ioctl_cmd_type    cmd,
                             sio_ioctl_param_type *param);
    /** *HW_control_transmit api to be called when sio_control_transmit()
    is called by client */
    void (*HW_control_transmit) (sio_stream_id_type stream_id,
                                 sio_port_id_type      port_id,
                                 dsm_item_type     *tx_ptr);

    boolean is_registered;    /** Used by sio layer to indicated if driver is 
                                 registered or not */

}sio_driver_interface;

/*---------------------------------------------------------------------------*/
/** SIO device interface.
    Structure used to register Driver side interface. This structure is 
    populated by device driver with corresponding driver's api and is 
    registered with SIO for specific SIO Port owned by device driver.*/
/*---------------------------------------------------------------------------*/

typedef struct
{
    uint32      magic_num;          /**< Magic number for sanity check */
    /* Data related to the device being used */
    uint32      device_id;          /**< Port_id/Device_id being used for 
                                         device driver registration */
    boolean     forward_port;       /**< Non zero = forward this port  */
    boolean     driver_available;   /**< Non zero = No driver available */
    sio_port_id_type forward_port_id;  /**< identifies which bridge to use.
                                            use default value SIO_PORT_NULL
                                            for legacy bridge (2-proc targets) */
    sio_driver_interface * driver_interface;  /**< Pointer to a driver_interface
                                                   structure.*/
}sio_device_interafce;

/*---------------------------------------------------------------------------*/
/** Prototype for SIO register function callback*/
/*---------------------------------------------------------------------------*/
typedef uint32 (*sio_register_device_driver_cb_type)(sio_device_interafce * device_iface_ptr);

/*---------------------------------------------------------------------------*/
/** Prototype for Driver init function */
/*---------------------------------------------------------------------------*/
typedef uint32 (*sio_driver_init_func_type)(sio_register_device_driver_cb_type register_driver_cb_fn);

/*=========================================================================*/
/**
@brief
  Registers driver interface in driver_list for given major number

@param[in]  major_num -- major number associated with device driver.
@param[in]  *driver_iface_ptr -- pointer to driver's interface structure

@dependencies
  None

@sideeffects
  None

@return
  1 -- if successful
  0 -- if not successful

@sa
  None
*/ 
/*=========================================================================*/

uint32 sio_register_driver
(
    uint32 major_num,  /* Major number of driver */
    sio_driver_interface * driver_iface_ptr  /* Pointer to driver interface */
); 

/*=========================================================================*/
/**
@brief
  Registers devices of driver with SIO 

@param[in]  *device_iface_ptr -- pointer to driver's interface structure
      
@dependencies
  None
                                                       
@sideeffects
  None

@return
  0 -- Failed to register the SIO device with SIO service
  1 -- Success

@sa
  None
*/
/*=========================================================================*/

uint32 sio_register_device_driver(sio_device_interafce * device_iface_ptr);


#endif /* SIO_H */

