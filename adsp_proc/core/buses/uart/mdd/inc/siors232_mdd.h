#ifndef SIORS232_MDD_H
#define SIORS232_MDD_H //{
/*===========================================================================

           S I O   D E V I C E   L A Y E R   M D D   H E A D E R

DESCRIPTION
   This is the header file for the common code used by the SIO UART driver. 

Copyright (c) 2008, 2012-2013 Qualcomm Technologies Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.qdsp6/2.1/buses/uart/mdd/inc/siors232_mdd.h#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/21/07   rh      Add IOCTL for RX line state detection
09/24/07   rh      Add new IOCTL function to control TX BREAK event
08/10/07   rh      Init function now initialize all UART ports
07/30/07   rh      Featurize sleep support
02/23/07   rh      Created file.
===========================================================================*/

/*===========================================================================

                        FEATURE FLAGS RESOLUTION 

===========================================================================*/

/*===========================================================================
 
                  Include Files

===========================================================================*/

#include "sio.h"                             /* SIO Server header file     */
#include "timer.h"                             /* Clock services header file */
#include "DalDevice.h"
#include "DALSys.h"
#include "DDITlmm.h"
#include "DDIUart.h"
#include "uart_circ_buffer.h"
#include "uart_compat.h"                 
#include "uart_event_queue.h"
#include "uart_init.h"

/*===========================================================================
                        ENUMERATION TYPE
===========================================================================*/


//------------------------------------------------------------------------------
//  State variable to determine if a land to mobile FAX image is in progress
//------------------------------------------------------------------------------

typedef enum 
{
  SIO_NOT_IN_FAX_IMAGE,
  SIO_IN_FAX_IMAGE,
  SIO_IN_FAX_IMAGE_DLE_FOUND
} sio_fax_image_partition_type;

//------------------------------------------------------------------------------
//  SIO Escape sequence state.  Used in the detection of '+++' escape sequence.
//------------------------------------------------------------------------------

typedef enum
{
  SIO_ESC_SEQ_NULL,                //  no Escape sequencing in process      
  SIO_ESC_SEQ_1ST_GUARDTIME,       //  1st Guardtime occurred              
  SIO_ESC_SEQ_1ST_CHAR,            //  Received 1st delay and 1st char    
  SIO_ESC_SEQ_2ND_CHAR,            //  Received 2nd delay and 2nd char   
  SIO_ESC_SEQ_3RD_CHAR,            //  Received 3rd delay and 3rd char  
  SIO_ESC_SEQ_DETECTED,            //  Escape Sequence detected        
  SIO_ESC_SEQ_MAX                  //  For Bounds checking only       
} sio_esc_seq_enum_type;

//------------------------------------------------------------------------------
//  AT autodetect sequence state.  Used in the detection of AT commands.
//------------------------------------------------------------------------------

typedef enum
{
  SIO_AD_INIT,                     //  Init state.  No chars received.           
  SIO_AD_GOT_A,                    //  Got the 'A' part of the AT command.      
  SIO_AD_GOT_AT,                   //  Got the 'AT' part of the AT command.    
  SIO_AD_GOT_AT_COMMAND,           //  Got the 'ATX' where X is any character.
                                   //  This is assumed to be an AT command.  
  SIO_AD_GOT_ASYNC_FLAG,           //  Got an SIO_SYNC_FLAG char after prepause. 
  SIO_AD_DISCARD                   //  Got an invalid char.  We are discarding. 

} sio_ad_state_type;

typedef enum
{
  SIO_PDR_INVALID,                 /* Invalid powerdown reason.          */
  SIO_PDR_INTERNAL,                /* Internal.                          */
  SIO_PDR_NO_CABLE,                /* Powered down due to cable removal. */
  SIO_PDR_INACTIVITY,              /* Powered down due to inactivity.    */
  SIO_PDR_EXPLICIT                 /* Powered down explictily.           */

} sio_pd_reason_type;

//------------------------------------------------------------------------------
//  TX stream state
//------------------------------------------------------------------------------

typedef enum
{
  //  Stream is open.  Normal operation.

  TX_STREAM_STATE_OPEN,

  //  Stream is closed.  Can't do anything but call open.

  TX_STREAM_STATE_CLOSED,

  //  Stream tx is being flushed.  Waiting for the empty
  //  TX FIFO notification.

  TX_STREAM_STATE_FLUSH_WAIT_FLUSH_TX_FIFO,

  //  Stream tx is being flushed.  It's received the TX 
  //  FIFO notification and is polling for the TX_EMT 
  //  condition.

  TX_STREAM_STATE_FLUSH_WAIT_TX_COMPLETE,

  //  Stream tx is being flushed.  For the very special case 
  //  of the AT+IPR command (change baud rate), the baud rate
  //  change must be delayed until after the command 
  //  acknowledgement (OK) is sent.  It's assumed that this
  //  will occur on the next transmit.

  TX_STREAM_STATE_FLUSH_WAIT_NEXT_TX

} sio_tx_stream_state_type;

//------------------------------------------------------------------------------
//  Workloop events
//------------------------------------------------------------------------------

typedef enum
{
  UART_WORKLOOP_LINE_EVENT = 0,
  UART_WORKLOOP_INACTIVITY_EVENT = 1,
  UART_WORKLOOP_FLUSH_EVENT = 2,

  UART_WORKLOOP_MAX_EVENTS = 3

} sio_workloop_event_type;

/*===========================================================================

                        DATA STRUCTURES 

===========================================================================*/

//  Structure definition for MSM UART variables

typedef struct                 //  Contains variables related to autodetect   
{
                               //  Current state of AD processing.  If no chars
                               //  have been received, state is INIT.        
    sio_ad_state_type          state;

                               //  Semaphores for Autodetect mode valid Packet 
                               //  detection.                               
    boolean                    packetpause_occurred;

                               //  Count of rx characters while in discard state.   
    word                       rx_cnt;

                               //  Clock callbacks struct used for the serial RX 
                               //  line pause checking                              
    timer_type                pausecheck_cb_item;
    timer_type                packetpause_cb_item;

                               //  Stream IDs for Autodetect and 'in a data call'   
    sio_stream_id_type         autodetect_stream_id;
    sio_stream_id_type         ds_incall_stream_id;
    uart_device_id_type        autodetect_device_id;
    uart_device_id_type        ds_incall_device_id;


} sio_mdd_autodetect_var_type;

//------------------------------------------------------------------------------
//  sio_mdd_port_data_type
//------------------------------------------------------------------------------

typedef struct sio_mdd_port_data_type
{
  //  Lock used for both port data and stream data (since
  //  there can only be one stream per port).
  DALSYSSyncHandle             lock_handle;
  DALDEVICEHANDLE              dal_device_handle;
  DALDEVICEID                  dal_device_id;
  uart_device_id_type          device_id; 
  boolean                      is_clock_on;

  uint32                       inactivity_ms;        /* Inactivity timeout, ms */ 
  timer_type                   inactivity_timer;     /* Inactivity timer       */
  sio_pd_reason_type           power_down_reason;

} sio_mdd_port_data_type;

//------------------------------------------------------------------------------
//  sio_mdd_stream_data_type
//
//  Data associated with stream.  Use the port data lock handle to access.
//------------------------------------------------------------------------------

typedef struct sio_mdd_stream_data_type
{
  //  Is stream open?
  boolean                      is_open;

  //  Stream ID at open time.
  sio_stream_id_type           open_stream_id;           

  //  RX, TX or both.
  sio_stream_type              sio_type_of_stream;        

  //  Current RX and TX bit rates    
  sio_bitrate_type             sio_current_bitrate;    
                                                        
  //  Baudrate to switch to when the user calls the 
  //  ioctl used to change baudrate                
  sio_bitrate_type             sio_new_bitrate;          
                                                          
  //  Pointer to task context block which is used in 
  //  signalling task when DTR changes state.  DTR/ 
  //  pin allows dedication of GPIO_INT pin to the 
  //  purpose.                                   
  void                         (*siors_dtr_func_ptr)(void);  
                                                              
} sio_mdd_stream_data_type;

//------------------------------------------------------------------------------
//  sio_mdd_tx_data_type
//
//  Data associated with tx.  Use the lock handle to access.
//------------------------------------------------------------------------------

typedef struct sio_mdd_tx_data_type
{
  //  Use lock to access data.
  DALSYSSyncHandle             lock_handle;

  //  Indicates whether stream is open for TX.
  boolean                      is_open;

  //  Stream state.
  sio_tx_stream_state_type     tx_stream_state;              

  //  Pointer to queue from which SIO will dequeue 
  //  packets to TX.                                
  dsm_watermark_type*          sio_tx_watermark_queue_ptr;

  //  Pointer to packet which contains the item which 
  //  is being transmitted.                        
  dsm_item_type*               sio_tx_current_wm_item_ptr;

  //  Pointer to buffer portion of sio_tx_current_wm_item_ptr.  
  //  This is the actual data that needs to be TX'ed.                
  byte*                        sio_current_tx_ptr;

  //  This is the number of bytes which need to be 
  //  transmitted from current packet.             
  int32                        sio_bytes_to_tx;

  //  Number of times we've polled for tx complete.
  uint32                       num_tx_complete_polls;   
  
  //  Are we sending the TX break condition?
  boolean                      tx_break_state;          

  //  TRUE if tx_in_progress.
  boolean                      tx_in_progress;          

  //  Pointer to function which will be called when 
  //  the last bit of packet waiting transmission 
  //  has been transmitted.                       
  sio_vv_func_ptr_type         sio_stream_close_func_ptr;

  //  Clock Call back item used in determining the 
  //  time at which all of the bytes that need to be 
  //  TX'ed have completely left SIO hardware.     
  timer_type                  sio_stream_close_cb_item;

  //  Flow control variables.
  sio_flow_ctl_type            sio_tx_flow_control_method;
  boolean                      sio_outbound_flow_disabled;

} sio_mdd_tx_data_type;

//------------------------------------------------------------------------------
//  sio_mdd_rx_data_type
//
//  Data associated with rx.  Use the lock handle to access.
//------------------------------------------------------------------------------

typedef struct sio_mdd_rx_data_type
{
  //  Use lock to access data.
  DALSYSSyncHandle             lock_handle;

  //  Indicates whether stream is open for RX.
  boolean                      is_open;

  //  Total number of overruns. 
  uint32                       total_overruns;            

  //  Circular buffer used when reading data. 
  uart_circular_buffer_type    circular_buffer;            

  //  Pointer to queue onto which incoming packets 
  //  should be placed.                             
  dsm_watermark_type*          sio_rx_watermark_queue_ptr;   
                                                            
  //  Function pointer which should be called with 
  //  incoming packet in lieu of enqueuing onto the 
  //  above queue.       
  sio_rx_func_ptr_type         sio_rx_func_ptr;

  // Indicates if sio_rx_dsm_pool is explicitly specified
  // by the client, or if the driver should use
  // an implicit pool computed by the UART driver.
  // TRUE - UART driver uses explicit pool specified by client.
  // FALSE - UART driver uses implicit pool as determined by the
  // function set_dsm_pool_id ().
  boolean                 use_explicit_sio_rx_dsm_pool;

  // The DSM pool to use for incoming packets. Only valid if use_explicit_sio_rx_dsm_pool = TRUE
  dsm_mempool_id_type     sio_rx_dsm_pool;

  //  This boolean indicates whether or not SIO 
  //  needs to look for a tail character.          
  boolean                      sio_tail_char_used;

  //  This is the actual tail character for which SIO 
  //  needs to examine incoming stream.            
  byte                         sio_tail_char;

  //  Mode of current stream.
  sio_stream_mode_type         sio_current_stream_mode;  

  //--------------------------------------------------------
  //  Variables used in '+++' escape code detection. 
  //--------------------------------------------------------

  //  State associated with escape code detection. 
  sio_esc_seq_enum_type        sio_esc_seq_state;

  //  Clock call back structure which is used in 
  //  escape code detection.                       
  timer_type                  sio_esc_seq_cb_item;

  //  Pointer to function which will be called when 
  //  escape code is detected.                     
  sio_vv_func_ptr_type         sio_escape_code_func_ptr;

  //--------------------------------------------------------
  //  Variables used in packet detection. 
  //--------------------------------------------------------

  //  Pointer to function which will be called when 
  //  packet is detected.                          
  sio_vv_func_ptr_type         sio_packet_func_ptr;

  //  Boolean indicating, if TRUE, that timer for 
  //  byte following PPP flag is scheduled but has 
  //  not happened yet. FALSE, timer has expired. 
  boolean                      sio_packet_detection_active;

  //  ????
  boolean                      sio_uart_packet_mode;

  //  Store the callback registered when RX
  //  activity check is required               
  sio_vv_func_ptr_type         rx_activity_cb_func_ptr;
  
  //  Flow control variables.
  sio_flow_ctl_type            sio_rx_flow_control_method;
  boolean                      sio_inbound_flow_enabled;

  //--------------------------------------------------------
  //  Autodetect variables.
  //--------------------------------------------------------

  sio_mdd_autodetect_var_type  adv;   
  
} sio_mdd_rx_data_type;

//------------------------------------------------------------------------------
//  sio_mdd_debug_data_type
//
//  Data associated with debug.  If write access is required, use the port
//  data lock handle to access.
//------------------------------------------------------------------------------

typedef struct
{
  /** TLMM device handle, used to query the state of the UART's GPIO lines. */
  DalDeviceHandle      *tlmm_dal;

  /** All of the GPIO lines used by this device.
   *
   * The GPIO lines are read from the DAL UART configuration file. The order
   * of the GPIO lines does not matter. If four GPIO lines are not used on
   * this platform configuration, the unused lines will be zero. */
  DALGpioSignalType     gpio[4];

} sio_mdd_debug_data_type;

//------------------------------------------------------------------------------
//  sio_mdd_port_type
//
//  Use the lock_handle to access each portion of the data.
//  port_data and stream_data use the same lock (in port_data) 
//  since there can only be one stream per port.
//
//  If multiple locks need to be held concurrently, they should be 
//  acquired in the order:
//
//  1) port
//  2) rx
//  3) tx
//
//  The locks should be released in the inverse order.
//------------------------------------------------------------------------------

typedef struct              
{
  sio_mdd_port_data_type       port_data;
  sio_mdd_stream_data_type     stream_data; 
  sio_mdd_rx_data_type         rx_data;
  sio_mdd_tx_data_type         tx_data;
  sio_mdd_debug_data_type      debug_data;

  //  Workloop related variables.  
  
  DALSYSWorkLoopHandle         workloop_handle;
  DALSYSEventHandle            workloop_event[UART_WORKLOOP_MAX_EVENTS];
  uart_event_queue_type        event_queue;

} sio_mdd_port_type;

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

extern sio_mdd_port_type       siors_mdd_ports[];

uint32                         siors_mdd_to_dal_baud_rate( sio_bitrate_type bitrate );
void                           siors_mdd_dsm_new_buffer_null_cleanup( sio_mdd_port_type* );
void                           siors_mdd_ioctl_disable_dtr_event( sio_mdd_port_type*, sio_ioctl_param_type* );
void                           siors_mdd_detect_rx_activity( sio_mdd_port_type* );
void                           siors_mdd_flush_tx( sio_mdd_port_type*, boolean, sio_flush_func );
void                           siors_mdd_update_flow_control( sio_mdd_port_type* );
dsm_mempool_id_type siors_mdd_set_dsm_pool_id (const sio_open_type *open_ptr, sio_mdd_rx_data_type *mdd_port);
sio_status_type                siors_mdd_powerdown( sio_mdd_port_type*, sio_pd_reason_type reason );
void                           siors_mdd_wakeup( sio_mdd_port_type* );
void                           siors_mdd_on_activity( sio_mdd_port_type* );
void                           siors_mdd_on_inactivity_isr( int32 time_ms, timer_cb_data_type data );
void                           siors_mdd_debug_init( sio_mdd_port_type* );
void                           siors_mdd_debug( sio_mdd_port_type* );

#endif //}

