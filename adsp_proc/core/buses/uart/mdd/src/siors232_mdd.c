/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             S I O R S 2 3 2  M D D

            SIO Media Dependent Driver layer for RS-232 communication

GENERAL DESCRIPTION

  This module contain the code shared by different serial driver.  It calls
  seperate hardware dependent driver code to access the actual UART hardware.


EXTERNALIZED FUNCTIONS

  siors_mdd_api_dev_init()
    Initialize device.

  siors_mdd_api_open_stream()
    Allocate SIO resources and do necessary book-keeping.

  siors_mdd_api_close_stream()
    Deallocate SIO resources and do necessary book-keeping.

  siors_mdd_api_dev_transmit()
    Transmit given data over MSM's RS-232 UART.

  siors_mdd_api_flush_tx()
    Flushes the TX.

  siors_mdd_api_disable_device()
    Disables given device.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  siors_mdd_api_dev_init() must be called prior to any of the other functions of
  this module.

Copyright (c) 1999-2000, 2007-2008, 2012-2014 Qualcomm Technologies Incorporated.
All Rights Reserved.  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1/buses/uart/mdd/src/siors232_mdd.c#1 $ $DateTime: 2017/07/21 22:10:47 $ $Author: pwbldsvc $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/19/09   snb     Send response to +IPR command at original baud rate.
08/12/08   cpm     Add data support back in
04/03/08   rh      Use a common HW type definition
03/24/08   rh      Enable the use of LARGE_DSM_ITEM pool
03/18/08   rh      Add missing featurization for FEATURE_FIRST_UART
02/19/08   rh      Use a busy while loop to wait for TX empty
01/22/08   rh      Featurize the call to sleep_set_mclk_restriction
01/10/08   rh      Disable TX flush if TX transmit is called
01/09/08   rh      Disable cable check timer if all ports are in sleep state
12/17/07   rh      Ensure port is fully closed when sio_close is called
12/04/07   rh      Add ARM HALT workaround support
11/21/07   rh      Add IOCTL for RX line state detection
10/18/07   rh      Add debug trace for the received characters
09/24/07   rh      Add new IOCTL function to control TX BREAK event
08/10/07   rh      Added per UART configuration
08/09/07   rh      Integrate changes required by the UARTDM-PD driver
07/27/07   rh      Remove some unused code
                   Added ability to handle receive function that returns
                   no valid data
04/13/07   rh      Created file from siors232.c
===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/

#include "DDIUart.h"
#include "DALDeviceId.h"

#include "diagdsm.h"
#include "dsm.h"
#include "siors232_mdd_defs.h"
#include "siors232_mdd_data.h"
#include "uart_compat.h"
#include "uart_log.h"


//  If there is is at least one UART.
#if( defined(FEATURE_FIRST_UART)  || \
     defined(FEATURE_SECOND_UART) || \
     defined(FEATURE_THIRD_UART))              //{

//--------------------------------------------------------------------------
//  File id for buffer logging.
//--------------------------------------------------------------------------

#define  UART_TRACE_BUFFER_FILE_ID  1

//  The following array contains the port IDs of open streams.  If a stream is
//  not open, the value in location is undefined.

static sio_port_id_type    siors_mdd_stream_port[SIO_MAX_STREAM];

//  The following array contains the port structure.  All the required
//  information/variable is stored in this structure.

sio_mdd_port_type          siors_mdd_ports[UART_NUM_DEVICES];

//  This variable controls the log level that the UART driver registers with
//  the DAL log. Log messages at the INFO level can be useful, but in some
//  cases they take a significant amount of time, and should be disabled for
//  time-sensitive applications. This value can be specified during
//  compilation by setting the preprocessor definition UART_DAL_LOG_LEVEL, and
//  can overridden from the debugger at runtime.

//  Valid values are:

//  DALSYS_LOGEVENT_FATAL_ERROR
//  DALSYS_LOGEVENT_ERROR
//  DALSYS_LOGEVENT_WARNING
//  DALSYS_LOGEVENT_INFO

#ifndef UART_DAL_LOG_LEVEL
#define UART_DAL_LOG_LEVEL DALSYS_LOGEVENT_INFO
#endif /* !UART_DAL_LOG_LEVEL */

uint32                     uart_dal_log_level = UART_DAL_LOG_LEVEL;

//  The function pointer to the next DAL log event handler

static DALSYS_LogEventFncPtr siors_mdd_dal_log_func;

//  Timer group used by the non-deferrable timers in the UART driver. The
//  flush timer should not be deferrable, or the CPU may power down while the
//  UART driver is waiting to power down the UART hardware.

static timer_group_type    uart_timer_group;

//--------------------------------------------------------------------------
//  Function prototypes
//--------------------------------------------------------------------------

static void                handle_line_event_isr( sio_mdd_port_type*, uint32 );
static void                handle_line_event( sio_mdd_port_type* );
static DALResult           workloop_handle_line_event( DALSYSEventHandle, void* );
static DALResult           workloop_handle_inactivity_event( DALSYSEventHandle, void* );
static DALResult           workloop_handle_flush_event( DALSYSEventHandle, void* );
static void                initialize_device_state( uart_device_id_type );
static void                initialize_device( uart_device_id_type );
static void                do_transmit( sio_mdd_port_type* );

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------

typedef void               (*DALNotifyFunc)( void* );
typedef void               (*DALLineEventFunc)( void*, uint32 );

#define   MAX_TX_COMPLETE_POLLS     30
#define   TX_COMPLETE_TIMEOUT       1
#define   UART_WORKLOOP_PRIORITY    0   // Use the default workloop priority

/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static inline boolean
can_transmit_now( sio_mdd_port_type* mdd_port )
{
  //  Can't transmit if we're flowed off.

  if( mdd_port->tx_data.sio_outbound_flow_disabled )
  {
    return( FALSE );
  }

  //  Can't transmit if a transfer is already in
  //  progress.

  if( mdd_port->tx_data.tx_in_progress )
  {
    return( FALSE );
  }

  //  Otherwise, good to go.

  return( TRUE );
}

/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static inline boolean
more_data_to_tx( sio_mdd_port_type* mdd_port )
{
  sio_mdd_tx_data_type*        txd = &mdd_port->tx_data;
  boolean                      more_data = FALSE;


  if( txd->tx_in_progress )
  {
    more_data = TRUE;
  }
  else if( txd->sio_bytes_to_tx > 0 )
  {
    more_data = TRUE;
  }
  else if( txd->sio_tx_current_wm_item_ptr != NULL )
  {
    more_data = TRUE;
  }
  else if( txd->sio_tx_watermark_queue_ptr->current_cnt > 0 )
  {
    more_data = TRUE;
  }

  return( more_data );
}

/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static void
poll_tx_complete_isr( int32 timeMs, timer_cb_data_type data )
{
  sio_mdd_port_type*           mdd_port = (sio_mdd_port_type*) data;
  sio_mdd_tx_data_type*        txd = &mdd_port->tx_data;
  uint32                       is_tx_complete = FALSE;
  boolean                      deregister_timer = FALSE;
  uint32                       device_id;



  device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +poll_tx_complete_isr", device_id );

  DALSYS_SyncEnter( txd->lock_handle );

  //  Determine what to do -- either do nothing, poll again,
  //  or tx is complete and we're done.

  if( txd->tx_stream_state != TX_STREAM_STATE_FLUSH_WAIT_TX_COMPLETE )
  {
    //  Something has changed while we were waiting for the
    //  tx complete.  The condition we were wating for no longer
    //  applies.  Do nothing.
  }
  else if( txd->num_tx_complete_polls > MAX_TX_COMPLETE_POLLS )
  {
    //  Do something.  Can't just lock up, let's call it complete.

    UART_LOG_1( TRACE, "[%d] Max polls exceeded.", device_id );
    deregister_timer = TRUE;
    is_tx_complete   = TRUE;
  }
  else
  {
    //  Query tx complete.

    DalUart_TxComplete( mdd_port->port_data.dal_device_handle, &is_tx_complete );

    if( is_tx_complete )
    {
      deregister_timer = TRUE;
    }
    else
    {
      txd->num_tx_complete_polls++;
    }
  }

  //  Done.

  if( deregister_timer )
  {
    timer_clr_64( &txd->sio_stream_close_cb_item, T_NONE );
  }
  //  Reset the state.

  if( is_tx_complete )
  {
    txd->tx_stream_state = TX_STREAM_STATE_OPEN;

    UART_LOG_1( TRACE, "[%d] TX complete, triggering flush event.", device_id );

    DALSYS_EventCtrl( mdd_port->workloop_event[UART_WORKLOOP_FLUSH_EVENT],
                      DALSYS_EVENT_CTRL_TRIGGER );
  }

  DALSYS_SyncLeave( txd->lock_handle );

  UART_LOG_1( TRACE, "[%d] -poll_tx_complete_isr", device_id );
}

/*===========================================================================

FUNCTION WORKLOOP_HANDLE_FLUSH_EVENT

DESCRIPTION
  Call the user callback to handle the tx complete (flush complete) event.
  This function is executed in workloop, instead of timer, context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static DALResult
workloop_handle_flush_event( DALSYSEventHandle event_handle, void* context )
{
  sio_mdd_port_type*    mdd_port = (sio_mdd_port_type*)context;
  sio_mdd_tx_data_type* txd = &mdd_port->tx_data;
  sio_flush_func        on_flush_complete;

  UART_LOG( TRACE, "+workloop_handle_flush_event");

  DALSYS_SyncEnter( txd->lock_handle );

  on_flush_complete = txd->sio_stream_close_func_ptr;

  DALSYS_SyncLeave( txd->lock_handle );

  UART_LOG( TRACE, "TX complete, invoking callback.");

  if(on_flush_complete)
  {
    on_flush_complete();
  }

  UART_LOG( TRACE, "-workloop_handle_flush_event");

  return DAL_SUCCESS;
}

/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static void
poll_tx_complete( sio_mdd_port_type* mdd_port )
{
  int           device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +poll_tx_complete", device_id );

  //  Set state.

  mdd_port->tx_data.tx_stream_state       = TX_STREAM_STATE_FLUSH_WAIT_TX_COMPLETE;
  mdd_port->tx_data.num_tx_complete_polls = 0;

  timer_reg( &mdd_port->tx_data.sio_stream_close_cb_item,
             poll_tx_complete_isr,
             (timer_cb_data_type) mdd_port,
             0,
             TX_COMPLETE_TIMEOUT );

  UART_LOG_1( TRACE, "[%d] -poll_tx_complete", device_id );
}

/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static void
cancel_flush_tx( sio_mdd_port_type* mdd_port )
{
  int           device_id = mdd_port->port_data.device_id;


  UART_LOG_1( TRACE, "[%d] +cancel_flush_tx", device_id );

  switch( mdd_port->tx_data.tx_stream_state )
  {
    case TX_STREAM_STATE_OPEN:
    case TX_STREAM_STATE_CLOSED:
    case TX_STREAM_STATE_FLUSH_WAIT_FLUSH_TX_FIFO:
    case TX_STREAM_STATE_FLUSH_WAIT_NEXT_TX:

      //  Nothing to do.
      break;

    case TX_STREAM_STATE_FLUSH_WAIT_TX_COMPLETE:

      timer_clr_64( &mdd_port->tx_data.sio_stream_close_cb_item, T_NONE );
      break;

    default:

      UART_LOG_1( FATAL, "[%d] Unhandled case in switch.", device_id );
      break;
  }

  UART_LOG_1( TRACE, "[%d] -cancel_flush_tx", device_id );
}

/*===========================================================================

FUNCTION

DESCRIPTION

PARAMETERS
  immediate - TRUE for an immediate flush;
              FALSE for a delayed flush.

              The immediate flush will allow the tx FIFO to drain and call
              the callback function when TX_EMT is set.

              The delayed flush will wait until the next transmit, then act
              like the immediate flush.  This is needed for the very special
              case of the AT+IPR command (change baud rate).  The FIFO must
              be drained before changing the baud rate.  But, the response
              to this command (OK) must be transmitted before the baud rate
              change.

DEPENDENCIES  Called with the port locked.

RETURN VALUE

SIDE EFFECTS  Releases lock on the port.

===========================================================================*/

void
siors_mdd_flush_tx( sio_mdd_port_type* mdd_port, boolean immediate, sio_flush_func on_flush_complete )
{
  sio_mdd_tx_data_type*            txd = &mdd_port->tx_data;
  boolean                          invoke_callback = FALSE;
  int                              device_id;




  device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +siors_mdd_flush_tx", device_id );

  switch( txd->tx_stream_state )
  {
    case TX_STREAM_STATE_OPEN:

      txd->sio_stream_close_func_ptr = on_flush_complete;

      if( !immediate )
      {
        //  Delayed flush.  Wait until the next TX, then flush.
        txd->tx_stream_state = TX_STREAM_STATE_FLUSH_WAIT_NEXT_TX;
      }
      else if( more_data_to_tx( mdd_port ))
      {
        //  Wait until the FIFO empties.
        txd->tx_stream_state = TX_STREAM_STATE_FLUSH_WAIT_FLUSH_TX_FIFO;
      }
      else
      {
        //  Wait until the shift register empties.
        poll_tx_complete( mdd_port );
      }
      break;

    case TX_STREAM_STATE_CLOSED:

      if( immediate )
      {
        //  Weird case.  Old code called the callback.
        UART_LOG_1( TRACE, "[%d] Calling flush on device.", device_id );
        invoke_callback = TRUE;
      }
      else
      {
        //  Stream is closed, we can't wait for the next tx.  Ignore.
        UART_LOG_1( TRACE, "[%d] Calling delayed flush on a closed device.", device_id );
      }
      break;

    case TX_STREAM_STATE_FLUSH_WAIT_NEXT_TX:
    case TX_STREAM_STATE_FLUSH_WAIT_FLUSH_TX_FIFO:
    case TX_STREAM_STATE_FLUSH_WAIT_TX_COMPLETE:

      //  Calling flush after already flushing?  Presumably
      //  it won't happen.  If it does, print a message and
      //  ignore.  Just let the first flush complete.

      UART_LOG_1( TRACE, "[%d] Flush called while waiting for flush on device.", device_id );
      break;

    default:

      UART_LOG_1( FATAL, "[%d] Unhandled case in switch.", device_id );
      break;
  }

  //  Assumes that the TX has been locked when we enter.
  //  Release the lock before invoking the callback.  (The
  //  callback may make calls on the same port.)

  DALSYS_SyncLeave( txd->lock_handle );

  if( invoke_callback )
  {
    on_flush_complete();
  }

  UART_LOG_1( TRACE, "[%d] -siors_mdd_flush_tx", device_id );
}

/*===========================================================================

FUNCTION

  process_rx_dsm_item

DESCRIPTION

  Release control of the input DSM item.  The DSM item may be freed
  or ownership passed to the RX queue.

DEPENDENCIES

  None.

RETURN VALUE

  None.

SIDE EFFECTS

  The input DSM item is released.  It may be enqueued on the destination
  RX queue.

===========================================================================*/

static void
process_rx_dsm_item( sio_mdd_port_type* mdd_port, dsm_item_type* dsm_item_ptr )
{
  sio_mdd_rx_data_type*            rxd  = &mdd_port->rx_data;
  int                              device_id = mdd_port->port_data.device_id;



  UART_LOG_1( TRACE, "[%d] +siors_mdd_process_rx_dsm_item", device_id );

  if( NULL == dsm_item_ptr )
  {
    UART_LOG_1( TRACE, "[%d] DSM item ptr is null.", device_id );
    goto DONE;
  }

  //  Add the kind field for these two modes.

  if(( rxd->sio_current_stream_mode == SIO_DS_AUTODETECT_MODE ) ||
     ( rxd->sio_current_stream_mode == SIO_DS_RAWDATA_MODE ))
  {
    dsm_item_ptr->kind = DSM_PS_RAW_APP_PAYLOAD;
  }

  //  Do something with the DSM item.

  if( 0 == dsm_item_ptr->used )
  {
    dsm_free_buffer( dsm_item_ptr );
  }
  else if( rxd->sio_rx_func_ptr )
  {
    //  Read callback function called in ISR context!
    rxd->sio_rx_func_ptr( &dsm_item_ptr );
  }
  else if( rxd->sio_rx_watermark_queue_ptr )
  {
    dsm_simple_enqueue_isr( rxd->sio_rx_watermark_queue_ptr, &dsm_item_ptr );
  }
  else
  {
    dsm_free_buffer( dsm_item_ptr );
  }

DONE:;

  UART_LOG_1( TRACE, "[%d] -siors_mdd_process_rx_dsm_item", device_id );
}

/*===========================================================================

FUNCTION                                                    INTERNAL FUNCTION

  siors_mdd_to_dal_baud_rate

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

uint32
siors_mdd_to_dal_baud_rate( sio_bitrate_type bitrate )
{
  uint32      baud_rate;


  baud_rate = 0;

  switch( bitrate )
  {
    case SIO_BITRATE_300:     baud_rate =     300; break;
    case SIO_BITRATE_600:     baud_rate =     600; break;
    case SIO_BITRATE_1200:    baud_rate =    1200; break;
    case SIO_BITRATE_2400:    baud_rate =    2400; break;
    case SIO_BITRATE_4800:    baud_rate =    4800; break;
    case SIO_BITRATE_9600:    baud_rate =    9600; break;
    case SIO_BITRATE_14400:   baud_rate =   14400; break;
    case SIO_BITRATE_19200:   baud_rate =   19200; break;
    case SIO_BITRATE_38400:   baud_rate =   38400; break;
    case SIO_BITRATE_57600:   baud_rate =   57600; break;
    case SIO_BITRATE_115200:  baud_rate =  115200; break;
    case SIO_BITRATE_230400:  baud_rate =  230400; break;
    case SIO_BITRATE_460800:  baud_rate =  460800; break;
    case SIO_BITRATE_921600:  baud_rate =  921600; break;
    case SIO_BITRATE_1200000: baud_rate = 1200000; break;
    case SIO_BITRATE_1209677: baud_rate = 1209677; break;
    case SIO_BITRATE_1250000: baud_rate = 1250000; break;
    case SIO_BITRATE_1293103: baud_rate = 1293103; break;
    case SIO_BITRATE_1339286: baud_rate = 1339286; break;
    case SIO_BITRATE_1388889: baud_rate = 1388889; break;
    case SIO_BITRATE_1442308: baud_rate = 1442308; break;
    case SIO_BITRATE_1500000: baud_rate = 1500000; break;
    case SIO_BITRATE_1562500: baud_rate = 1562500; break;
    case SIO_BITRATE_1630435: baud_rate = 1630435; break;
    case SIO_BITRATE_1704545: baud_rate = 1704545; break;
    case SIO_BITRATE_1785714: baud_rate = 1785714; break;
    case SIO_BITRATE_2000000: baud_rate = 2000000; break;
    case SIO_BITRATE_2900000: baud_rate = 2900000; break;
    case SIO_BITRATE_3000000: baud_rate = 3000000; break;
    case SIO_BITRATE_3200000: baud_rate = 3200000; break;
    case SIO_BITRATE_3686400: baud_rate = 3686400; break;
    case SIO_BITRATE_4000000: baud_rate = 4000000; break;
    default: break;
  }

  return( baud_rate );
}

/*===========================================================================

FUNCTION                                                    INTERNAL FUNCTION

  uart_dal_logger

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static void
uart_dal_logger( DALDEVICEID dalDeviceId, uint32 level, const char* msg )
{
  sio_mdd_port_type*       mdd_port;
  uart_device_id_type      device_id;
  uart_device_id_type      i;


  //  This routine will get called for lots of DAL based devices.  We
  //  only care when it's called for UART devices.

  switch( dalDeviceId )
  {
    case DALDEVICEID_UARTLG_DEVICE_1:
    case DALDEVICEID_UARTLG_DEVICE_2:
    case DALDEVICEID_UARTLG_DEVICE_3:
    case DALDEVICEID_UARTDM_DEVICE_1:
    case DALDEVICEID_UARTDM_DEVICE_2:
    case DALDEVICEID_UARTGSBI_DEVICE_0:
    case DALDEVICEID_UARTGSBI_DEVICE_1:
    case DALDEVICEID_UARTGSBI_DEVICE_2:
    case DALDEVICEID_UARTGSBI_DEVICE_3:
    case DALDEVICEID_UARTGSBI_DEVICE_4:
    case DALDEVICEID_UARTGSBI_DEVICE_5:
    case DALDEVICEID_UARTGSBI_DEVICE_6:
    case DALDEVICEID_UARTGSBI_DEVICE_7:
    case DALDEVICEID_UARTGSBI_DEVICE_8:
    case DALDEVICEID_UARTGSBI_DEVICE_9:
    case DALDEVICEID_UARTGSBI_DEVICE_10:
    case DALDEVICEID_UARTGSBI_DEVICE_11:
    case DALDEVICEID_UARTGSBI_DEVICE_12:
    case DALDEVICEID_UARTBAM_DEVICE_1:
    case DALDEVICEID_UARTBAM_DEVICE_2:
    case DALDEVICEID_UARTBAM_DEVICE_3:
    case DALDEVICEID_UARTBAM_DEVICE_4:
    case DALDEVICEID_UARTBAM_DEVICE_5:
    case DALDEVICEID_UARTBAM_DEVICE_6:
    case DALDEVICEID_UARTBAM_DEVICE_7:
    case DALDEVICEID_UARTBAM_DEVICE_8:
    case DALDEVICEID_UARTBAM_DEVICE_9:
    case DALDEVICEID_UARTBAM_DEVICE_10:
    case DALDEVICEID_UARTBAM_DEVICE_11:
    case DALDEVICEID_UARTBAM_DEVICE_12:
    case DALDEVICEID_UARTCXM:
      device_id = UART_DEVICE_INVALID;
      for( i = UART_DEVICE_FIRST; i < UART_DEVICE_LAST; i++ )
      {
        mdd_port = &siors_mdd_ports[i];
        if( mdd_port->port_data.dal_device_id == dalDeviceId )
        {
          device_id = mdd_port->port_data.device_id;
          break;
        }
      }

      //  Log to the trace buffer.
      UART_DAL_MSG_LOG_3( msg, "[%d] DAL(0x%08x,%d): ", device_id, dalDeviceId, level );
      break;

    default:
      break;
  }

  /* Pass the log event through to the next log handler in the chain, if one
   * exists. */

  if(siors_mdd_dal_log_func)
  {
    siors_mdd_dal_log_func(dalDeviceId, level, msg);
  }
}

/*===========================================================================

FUNCTION                                                    INTERNAL FUNCTION
  tx_complete

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static void
tx_complete( sio_mdd_port_type* mdd_port )
{
  mdd_port->tx_data.tx_in_progress = FALSE;
}

/*===========================================================================

FUNCTION                                                    INTERNAL FUNCTION

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static boolean
create_lock( DALSYSSyncHandle* handle )
{
  DALResult    dal_result;
  boolean      created = FALSE;



  if( *handle )
  {
    //  Previous instance didn't get cleaned up.
    UART_LOG( FATAL, "Previous lock was not properly cleaned up.");
    goto DONE;
  }

  dal_result = DALSYS_SyncCreate( DALSYS_SYNC_ATTR_RESOURCE, handle, NULL );
  if( dal_result != DAL_SUCCESS )
  {
    UART_LOG_1( TRACE, "DALSYS_SyncCreate = %d.", dal_result );
    *handle = NULL;
    goto DONE;
  }

  created = TRUE;

DONE:;

  return( created );
}

/*===========================================================================

FUNCTION                                                    INTERNAL FUNCTION

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static void
destroy_lock( DALSYSSyncHandle* handle )
{
  if( handle && *handle )
  {
    DALSYS_DestroyObject( *handle );
    *handle = NULL;
  }
}

/*===========================================================================

FUNCTION SIORS_MDD_UPDATE_FLOW_CONTROL                   INTERNAL

DESCRIPTION
  Update the hardware flow control setting using the stored setting
  in the port structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void
siors_mdd_update_flow_control( sio_mdd_port_type* mdd_port )
{
  sio_mdd_rx_data_type*               rxd = &mdd_port->rx_data;
  sio_mdd_tx_data_type*               txd = &mdd_port->tx_data;
  uint32                              device_id = mdd_port->port_data.device_id;



  UART_LOG_1( TRACE, "[%d] +siors_mdd_update_flow_control", device_id );

  //  Acquire locks.

  DALSYS_SyncEnter( rxd->lock_handle );
  DALSYS_SyncEnter( txd->lock_handle );

  //  Always check state after acquiring lock.

  if( rxd->is_open )
  {
    //  Set rx flow control.

    if(( rxd->sio_rx_flow_control_method == SIO_FCTL_OFF ) ||
       ( rxd->sio_rx_flow_control_method == SIO_CTSRFR_FCTL ))
    {
      DalUart_SetRtsControl( mdd_port->port_data.dal_device_handle, RTS_DEASSERT );
    }
    else if( rxd->sio_rx_flow_control_method == SIO_CTSRFR_AUTO_FCTL )
    {
      DalUart_SetRtsControl( mdd_port->port_data.dal_device_handle, RTS_AUTO );
    }

    if( rxd->sio_inbound_flow_enabled )
    {
      DalUart_SetRtsControl( mdd_port->port_data.dal_device_handle, RTS_ASSERT );
    }
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the recieve path", device_id );
  }

  if( txd->is_open )
  {
    //  Set tx flow control.

    if( txd->sio_tx_flow_control_method == SIO_FCTL_OFF )
    {
      DalUart_SetCtsControl( mdd_port->port_data.dal_device_handle, CTS_DISABLE );
    }
    else
    {
      DalUart_SetCtsControl( mdd_port->port_data.dal_device_handle, CTS_ENABLE );
    }
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the transmit path", device_id );
  }

  DALSYS_SyncLeave( txd->lock_handle );
  DALSYS_SyncLeave( rxd->lock_handle );

  UART_LOG_1( TRACE, "[%d] -siors_mdd_update_flow_control", device_id );
}

/*===========================================================================

FUNCTION RX_WAKEUP

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void
rx_wakeup( sio_mdd_port_type* mdd_port )
{
  //  On modem processor, there is no need to use the proxy thread.
  //  Enable the UART directly. Only restart the UART if the port
  //  is actually powered down.

  //  TODO: replace all of this with a workloop event.

  siors_mdd_wakeup( mdd_port );
}

/*===========================================================================

FUNCTION SIORS_MDD_GET_MORE_DATA_FOR_TX

DESCRIPTION
  This function will try to determine whether or not there is more data for
  transmission.  If so, interrupt is enabled.  If not, interrupt is disabled.

DEPENDENCIES
  Current transmit buffers should be empty when this function is called.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
get_more_data_for_tx( sio_mdd_port_type* mdd_port )
{
  sio_mdd_tx_data_type*            txd = &mdd_port->tx_data;
  int                              device_id = mdd_port->port_data.device_id;



  UART_LOG_1( TRACE, "[%d] +get_more_data_for_tx", device_id );

  if( txd->sio_bytes_to_tx > 0 )
  {
    goto DONE;
  }

  //  If current item has no data, try to traverse packet chain first.

  txd->sio_tx_current_wm_item_ptr =
    dsm_free_buffer( txd->sio_tx_current_wm_item_ptr );

  if( txd->sio_tx_current_wm_item_ptr != NULL )
  {
    UART_LOG_1( TRACE, "[%d] Got item from packet chain.", device_id );
  }

  //  Traverse the transmit queue if packet chain is empty.

  if( txd->sio_tx_current_wm_item_ptr == NULL )
  {
    txd->sio_tx_current_wm_item_ptr =
      dsm_dequeue( txd->sio_tx_watermark_queue_ptr);

    if( txd->sio_tx_current_wm_item_ptr != NULL )
    {
      UART_LOG_1( TRACE, "[%d] Got item from queue.", device_id );
    }
  }

  if( txd->sio_tx_current_wm_item_ptr != NULL )
  {
    //  If there is another packet in the transmit queue or packet chain,
    //  do the necessary bookkeeping.

    txd->sio_current_tx_ptr =
      txd->sio_tx_current_wm_item_ptr->data_ptr;

    txd->sio_bytes_to_tx =
      txd->sio_tx_current_wm_item_ptr->used;

    ASSERT( txd->sio_bytes_to_tx > 0 );
  }

DONE:;

  UART_LOG_1( TRACE, "[%d] -get_more_data_for_tx", device_id );
}

/*===========================================================================

FUNCTION
  watermark_tx_isr

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
watermark_tx_isr( sio_mdd_port_type* mdd_port )
{
  int device_id = mdd_port->port_data.device_id;



  UART_LOG_1( TRACE, "[%d] +watermark_tx_isr", device_id );

  //  Ensure the transmit is done in a single unit.  If not,
  //  tx_pkt_done might happen and cause the value of bytes_to_tx and
  //  current_tx_ptr to change.


  //  Transmit any queued up data.

  do_transmit( mdd_port );

  UART_LOG_1( TRACE, "[%d] -watermark_tx_isr", device_id );
}

/*===========================================================================

FUNCTION

DESCRIPTION
  This ISR is called when a TX interrupt is generated.  A TX interrupt is
  enabled when there is data that needs to be transmitted.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
tx_isr( sio_mdd_port_type* mdd_port )
{
  sio_mdd_tx_data_type*            txd = &mdd_port->tx_data;
  uint32                           device_id = mdd_port->port_data.device_id;
  


  UART_LOG_1( TRACE, "[%d] +tx_isr", device_id );

  DALSYS_SyncEnter( txd->lock_handle );

  //  Check state after acquiring lock.

  if( !txd->is_open )
  {
    UART_LOG_1( TRACE, "[%d] tx_isr on closed stream.", device_id );
    goto DONE;
  }

  tx_complete( mdd_port );

  watermark_tx_isr( mdd_port );

DONE:;

  DALSYS_SyncLeave( txd->lock_handle );

  UART_LOG_1( TRACE, "[%d] -tx_isr", device_id );
}

/*===========================================================================

FUNCTION
  tail_char_rx_isr

DESCRIPTION
  Break the received data in the circular buffer into tail char
  delimited packets.  A packet ends with the tail char.  The tail
  char is included in the packet.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static boolean
tail_char_rx_isr( sio_mdd_port_type* mdd_port )
{
  uart_circular_buffer_type*      cb_ptr;
  sio_mdd_rx_data_type*           rxd;
  dsm_item_type*                  dsm_item_ptr;
  int                             i;
  uint8*                          buffer_start_ptr;
  uint8*                          buffer_end_ptr;
  uint8*                          read_ptr;
  uint8*                          dest_ptr;
  uint32                          total_copied;
  uint32                          num_bytes;
  boolean                         send_item;
  boolean                         success;
  int                             device_id;



  device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +tail_char_rx_isr", device_id );

  rxd          = &mdd_port->rx_data;
  cb_ptr       = &rxd->circular_buffer;
  success      = TRUE;
  dsm_item_ptr = NULL;
  dest_ptr     = NULL;

  //  Loop over the data in the circular buffer and transfer it to
  //  the DSM item.  Each DSM item will contain a packet.  A packet
  //  ends with the tail_char character.  The tail_char is included
  //  in the packet.

  buffer_start_ptr = uart_circ_buffer_get_buffer_start_ptr( cb_ptr );
  buffer_end_ptr   = uart_circ_buffer_get_buffer_end_ptr( cb_ptr );
  read_ptr         = uart_circ_buffer_get_read_ptr( cb_ptr );
  num_bytes        = uart_circ_buffer_get_num_bytes( cb_ptr );
  total_copied     = 0;

  for( i = 0; i < num_bytes; i++ )
  {
    if( NULL == dsm_item_ptr )
    {
      dsm_item_ptr = dsm_new_buffer( rxd->sio_rx_dsm_pool );
      if( NULL == dsm_item_ptr )
      {
        siors_mdd_dsm_new_buffer_null_cleanup( mdd_port );
        success = FALSE;
        break;
      }

      dest_ptr           = dsm_item_ptr->data_ptr;
      dsm_item_ptr->used = 0;
    }

    //  If we're about to copy the tail_char character, we can
    //  send the packet.

    send_item = ( *read_ptr == rxd->sio_tail_char );

    //  Copy the next byte into the DSM item.

    *dest_ptr++ = *read_ptr++;
    dsm_item_ptr->used++;
    total_copied++;

    //  Has the pointer wrapped around the end of the circular
    //  buffer?

    if( read_ptr >= buffer_end_ptr )
    {
      read_ptr = buffer_start_ptr;
    }

    //  How to handle the case where the gap between tail chars is bigger
    //  than a DSM item?  Send whatever you have so you can flush the data
    //  and continue reading.  The application reading the data may complain,
    //  but at least it won't back everything up.

    send_item = send_item || ( dsm_item_ptr->used >= dsm_item_ptr->size );

    //  Process the DSM item.

    if( send_item )
    {
      process_rx_dsm_item( mdd_port, dsm_item_ptr );
      dsm_item_ptr = NULL;
    }
  }

  //  Update the read pointer in the circular buffer.  Update it with the
  //  actual number of bytes copied to DSM items.  This is the variable
  //  total_copied.  Be sure to subtract the bytes in the last (partial) DSM
  //  item that wasn't sent.  The data that was in the partial packet
  //  will be left in the circular buffer for next time.  The DSM item
  //  will be discarded.

  if( dsm_item_ptr != NULL )
  {
    if( total_copied >= uart_circ_buffer_get_size( cb_ptr ))
    {
      //  We've copied the entire contents of the circular buffer
      //  into the DSM item and still haven't seen the tail char.
      //  Send the DSM item or we'll be stuck in this state forever.

      process_rx_dsm_item( mdd_port, dsm_item_ptr );
    }
    else
    {
      total_copied -= dsm_item_ptr->used;
      dsm_free_buffer( dsm_item_ptr );
    }
  }

  uart_circ_buffer_read_data( cb_ptr, total_copied );

  UART_LOG_1( TRACE, "[%d] -tail_char_rx_isr", device_id );

  return( success );
}

/*===========================================================================

FUNCTION
  buffered_rx_isr

DESCRIPTION
  Read input into the circular buffer for further processing.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void
buffered_rx_isr( sio_mdd_port_type* mdd_port )
{
  uart_circular_buffer_type*  cb_ptr;
  sio_mdd_rx_data_type*       rxd;
  int                         i;
  uint32                      num_overruns;
  uint8*                      buffer_ptr;
  uint32                      buffer_len;
  boolean                     more_data;
  uint32                      bytes_read;
  int                         device_id;




  device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +buffered_rx_isr", device_id );

  //  Read the PDD buffers into a circular buffer for further
  //  processing by the more specific ISR.

  rxd       = &mdd_port->rx_data;
  cb_ptr    = &rxd->circular_buffer;
  more_data = FALSE;
  do
  {
    //  Read from the PDD buffers into the local circular buffer.
    //  Since the buffer is circular, it takes up to 2 reads to
    //  fill it up.  (Current to end, start to current.)

    for( i = 0; i < 2; i++ )
    {
      //  Get write location within buffer.

      buffer_ptr = uart_circ_buffer_get_write_loc( cb_ptr, &buffer_len );
      if (buffer_len == 0)
      {
         break;
      }

      DalUart_Read( mdd_port->port_data.dal_device_handle,
                    buffer_ptr,
                    buffer_len,
                    &bytes_read,
                    &num_overruns );

      UART_BUFFER_LOG_2( buffer_ptr, bytes_read, "[%d] RX %d bytes:", device_id, bytes_read );

      //  Advance the buffer pointers.

      uart_circ_buffer_wrote_data( cb_ptr, bytes_read );

      rxd->total_overruns += num_overruns;

      more_data = ( bytes_read == buffer_len );
      if( !more_data )
      {
        break;
      }
    }

    //  Now that we've read data into the circular buffer,
    //  process it.

    if( FALSE == tail_char_rx_isr( mdd_port ))
    {
      more_data = FALSE;
    }
  }
  while( more_data );

  UART_LOG_1( TRACE, "[%d] -buffered_rx_isr", device_id );
}

/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
unbuffered_rx_isr( sio_mdd_port_type* mdd_port )
{
  sio_mdd_rx_data_type*            rxd;
  dsm_item_type*                   dsm_item_ptr;
  uint32                           num_overruns;
  uint8*                           buffer_ptr;
  uint32                           buffer_len;
  uint32                           bytes_read;
  int                              device_id;



  device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +unbuffered_rx_isr", device_id );

  //  The generic_isr can copy directly from the PDD buffers
  //  into DSM items.

  rxd = &mdd_port->rx_data;

  do
  {
    dsm_item_ptr = dsm_new_buffer( rxd->sio_rx_dsm_pool );
    if( NULL == dsm_item_ptr )
    {
      siors_mdd_dsm_new_buffer_null_cleanup( mdd_port );
      break;
    }

    //  Read data from PDD buffers into the DSM item.

    buffer_ptr = dsm_item_ptr->data_ptr;
    buffer_len = dsm_item_ptr->size;
    bytes_read = 0;

    DalUart_Read( mdd_port->port_data.dal_device_handle,
                  buffer_ptr,
                  buffer_len,
                  &bytes_read,
                  &num_overruns );

    UART_BUFFER_LOG_2( buffer_ptr, bytes_read, "[%d] RX %d bytes:", device_id, bytes_read );

    dsm_item_ptr->used = bytes_read;

    rxd->total_overruns += num_overruns;

    //  Scan the input data.  Currently, this is only done if for
    //  SIO_DS_RAWDATA_MODE (to scan for escape sequences) and
    //  SIO_DS_AUTODETECT_MOE (to scan for AT commands).

    if( SIO_DS_RAWDATA_MODE == rxd->sio_current_stream_mode )
    {
      siors_mdd_scan_data_rawdata_mode_rx_isr( mdd_port, dsm_item_ptr );
    }
    else if( SIO_DS_AUTODETECT_MODE == rxd->sio_current_stream_mode )
    {
      siors_mdd_scan_data_autodetect_mode_rx_isr( mdd_port, dsm_item_ptr );
    }

    //  Consume the DSM item we just filled.

    dsm_item_ptr->used = bytes_read;

    process_rx_dsm_item( mdd_port, dsm_item_ptr );
  }
  while( bytes_read == buffer_len );

  UART_LOG_1( TRACE, "[%d] -unbuffered_rx_isr", device_id );
}

/*===========================================================================

FUNCTION
  is_buffered_read

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static boolean
is_buffered_read( sio_mdd_port_type* mdd_port )
{
  return( mdd_port->rx_data.sio_tail_char_used );
}

/*===========================================================================

FUNCTION
  rx_isr

DESCRIPTION

  This is the ISR that is called when there is an UART RX interrupt.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
rx_isr( sio_mdd_port_type* mdd_port )
{
  sio_mdd_rx_data_type*            rxd = &mdd_port->rx_data;
  uint32                           device_id = mdd_port->port_data.device_id;



  UART_LOG_1( TRACE, "[%d] +rx_isr", device_id );

  DALSYS_SyncEnter( rxd->lock_handle );

  if ( !rxd->is_open )
  {
    UART_LOG_1( TRACE, "[%d] rx_isr on closed stream.", device_id );
    goto DONE;
  }

  //  Buffered vs unbuffered processing.
  //
  //  For unbuffered processing, the data can be read from the PDD buffers
  //  and put directly into the DSM items.  All data from the PDD buffers
  //  is copied to the DSM items and the DSM item enqueued.
  //
  //  For buffered processing, the data from the PDD buffers must be read
  //  into a temporary (circular) buffer for later processing.
  //
  //  For instance, in the tail char case, it's expected that each DSM item
  //  will contain a tail char delimited packet.  If we've read some data,
  //  but haven't seen a tail char, we have to save what's been read so far
  //  and complete the packet on a subsequent read.

  if( is_buffered_read( mdd_port ))
  {
    buffered_rx_isr( mdd_port );
  }
  else
  {
    unbuffered_rx_isr( mdd_port );
  }

DONE:;

  DALSYS_SyncLeave( rxd->lock_handle );

  UART_LOG_1( TRACE, "[%d] -rx_isr", device_id );
}

/*===========================================================================

FUNCTION SIORS_MDD_INITIALIZE_DEVICE_STATE                  INTERNAL FUNCTION

DESCRIPTION
  Initialize state variables for the given device.

DEPENDENCIES
  None


RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
initialize_device_state( uart_device_id_type device_id )
{
  sio_mdd_port_type*           mdd_port;



  UART_LOG_1( LOW, "[%d] +initialize_device_state", device_id );

  mdd_port = &siors_mdd_ports[device_id];

  mdd_port->port_data.lock_handle               = NULL;
  mdd_port->port_data.device_id                 = device_id;
  mdd_port->port_data.dal_device_id             = 0;
  mdd_port->port_data.dal_device_handle         = NULL;
  mdd_port->port_data.is_clock_on               = FALSE;
  mdd_port->port_data.inactivity_ms             = 0;
  mdd_port->port_data.power_down_reason         = SIO_PDR_INVALID;

  mdd_port->stream_data.is_open                 = FALSE;
  mdd_port->stream_data.sio_current_bitrate     = SIO_BITRATE_19200;
  mdd_port->stream_data.sio_type_of_stream      = SIO_STREAM_RXTX;

  mdd_port->tx_data.tx_stream_state             = TX_STREAM_STATE_CLOSED;
  mdd_port->tx_data.is_open                     = FALSE;
  mdd_port->tx_data.sio_tx_current_wm_item_ptr  = NULL;
  mdd_port->tx_data.sio_current_tx_ptr          = NULL;
  mdd_port->tx_data.sio_bytes_to_tx             = DSM_DS_SMALL_ITEM_SIZ;
  mdd_port->tx_data.sio_stream_close_func_ptr   = NULL;
  mdd_port->tx_data.num_tx_complete_polls       = 0;
  mdd_port->tx_data.tx_in_progress              = FALSE;
  mdd_port->tx_data.sio_tx_watermark_queue_ptr  = NULL;
  mdd_port->tx_data.sio_tx_flow_control_method  = SIO_CTSRFR_FCTL;
  mdd_port->tx_data.sio_outbound_flow_disabled  = FALSE;

  mdd_port->rx_data.sio_tail_char_used          = FALSE;
  mdd_port->rx_data.sio_escape_code_func_ptr    = NULL;
  mdd_port->rx_data.sio_packet_func_ptr         = NULL;
  mdd_port->rx_data.sio_packet_detection_active = FALSE;
  mdd_port->rx_data.sio_uart_packet_mode        = FALSE;
  mdd_port->rx_data.sio_rx_func_ptr             = NULL;
  mdd_port->rx_data.use_explicit_sio_rx_dsm_pool = FALSE;
  mdd_port->rx_data.sio_rx_dsm_pool             = 0;
  mdd_port->rx_data.rx_activity_cb_func_ptr     = NULL;
  mdd_port->rx_data.is_open                     = FALSE;
  mdd_port->rx_data.sio_rx_watermark_queue_ptr  = NULL;
  mdd_port->rx_data.sio_rx_flow_control_method  = SIO_CTSRFR_FCTL;
  mdd_port->rx_data.sio_inbound_flow_enabled    = FALSE;
  mdd_port->rx_data.sio_current_stream_mode     = SIO_GENERIC_MODE;
  mdd_port->rx_data.adv.ds_incall_device_id     = UART_DEVICE_INVALID;
  mdd_port->rx_data.adv.autodetect_device_id    = UART_DEVICE_INVALID;
  mdd_port->rx_data.adv.state                   = SIO_AD_INIT;
  mdd_port->rx_data.adv.packetpause_occurred    = FALSE;

  //  Define call back items.  In mc.c, clk_init is called after sio_init.
  //  However, these calls do not require clk initialzation.  It would be
  //  an error to do clk_reg and clk_dereg calls here, however.

  timer_def2( &mdd_port->tx_data.sio_stream_close_cb_item, &uart_timer_group );
  timer_def2( &mdd_port->rx_data.sio_esc_seq_cb_item, NULL );
  timer_def2( &mdd_port->port_data.inactivity_timer, &uart_timer_group );

  UART_LOG_1( LOW, "[%d] -initialize_device_state", device_id );
}

/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static boolean
initialize_workloop( uart_device_id_type device_id )
{
  sio_mdd_port_type*           mdd_port;
  DALResult                    dal_result;
  int                          i;
  DALSYSWorkLoopExecute        workloop_func = NULL;




  UART_LOG_1( HIGH, "[%d] +initialize_workloop", device_id );

  mdd_port = &siors_mdd_ports[device_id];

  //

  if( !uart_event_queue_init( &mdd_port->event_queue ))
  {
     UART_LOG_1( ERROR, "[%d] Error creating event queue.", device_id );
     goto FAILURE;
  }

  //  Create workloop.

  mdd_port->workloop_handle = NULL;
  dal_result = DALSYS_RegisterWorkLoop( UART_WORKLOOP_PRIORITY,
                                        UART_WORKLOOP_MAX_EVENTS,
                                        &mdd_port->workloop_handle,
                                        NULL );

  if( dal_result != DAL_SUCCESS )
  {
    UART_LOG_2( ERROR, "[%d] Error %d creating workloop.", device_id, dal_result );
    goto FAILURE;
  }

  for( i = 0; i < UART_WORKLOOP_MAX_EVENTS; i++ )
  {
    dal_result = DALSYS_EventCreate( DALSYS_EVENT_ATTR_WORKLOOP_EVENT,
                                     &mdd_port->workloop_event[i],
                                     NULL );

    if( dal_result != DAL_SUCCESS )
    {
      UART_LOG_2( HIGH, "[%d] Error %d creating event.", device_id, dal_result );
      goto FAILURE;
    }

    switch(i)
    {
      case UART_WORKLOOP_LINE_EVENT:

        workloop_func = workloop_handle_line_event;
        break;

      case UART_WORKLOOP_INACTIVITY_EVENT:

        workloop_func = workloop_handle_inactivity_event;
        break;

      case UART_WORKLOOP_FLUSH_EVENT:

        workloop_func = workloop_handle_flush_event;
        break;

      default:

        UART_LOG_1( HIGH, "[%d] Internal error.", device_id );
        goto FAILURE;
    }

    dal_result = DALSYS_AddEventToWorkLoop( mdd_port->workloop_handle,
                                            workloop_func,
                                            mdd_port,
                                            mdd_port->workloop_event[i],
                                            NULL );

    if( dal_result != DAL_SUCCESS )
    {
      UART_LOG_2( HIGH, "[%d] Error %d creating adding event to workloop.", device_id, dal_result );
      goto FAILURE;
    }
  }

  UART_LOG_1( HIGH, "[%d] -initialize_workloop", device_id );

  return( TRUE );

FAILURE:;

  UART_LOG_1( HIGH, "[%d] -initialize_workloop -- FAILED", device_id );

  return( FALSE );
}

/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
initialize_device( uart_device_id_type device_id )
{
  uart_device_info             device_info;
  UartInitConfig               initConfig;
  sio_mdd_port_type*           mdd_port;
  DALDEVICEHANDLE              dal_device_handle;




  UART_LOG_1( HIGH, "[%d] +initialize_device", device_id );

  //  Basic initialization so that cleanup on error works.

  mdd_port = &siors_mdd_ports[device_id];

  mdd_port->port_data.lock_handle = NULL;

  initialize_device_state( device_id );

  dal_device_handle = NULL;

  //  Get device info.

  if( !uart_get_device_info( device_id, &device_info ))
  {
    UART_LOG_1( ERROR, "[%d] UART device id is invalid.", device_id );
    goto FAILURE;
  }

  if( !device_info.is_present )
  {
    //  Not an error.  This routine is called unconditionally for all devices.
    UART_LOG_1( LOW, "[%d] UART device is not present.", device_id );
    goto FAILURE;
  }

  if( !device_info.is_valid )
  {
    UART_LOG_1( ERROR, "[%d] UART device is not configured correctly.", device_id );
    goto FAILURE;
  }

  mdd_port->port_data.lock_handle = NULL;
  if( !create_lock( &mdd_port->port_data.lock_handle ))
  {
    UART_LOG_1( ERROR, "[%d] Error creating lock.", device_id );
    goto FAILURE;
  }

  //  Attach to device.

  mdd_port->port_data.dal_device_id = device_info.dal_device_id;

  if( DAL_DeviceAttach( device_info.dal_device_id, &dal_device_handle ) != DAL_SUCCESS )
  {
    UART_LOG_2( ERROR, "[%d] Error attaching to DAL device, dal_device_id = 0x%08x..", device_id, device_info.dal_device_id );
    goto FAILURE;
  }

  if( !initialize_workloop( device_id ))
  {
    UART_LOG_1( ERROR, "[%d] Error initializing workloop.", device_id );
    goto FAILURE;
  }

  //  PostInit phase.

  initConfig.Ctxt               = mdd_port;
  initConfig.enable_dma         = device_info.enable_dma;
  initConfig.uart_irq           = 0;  // Read from property file.  Don't override here.
  initConfig.rx_irq             = 0;  // Read from property file.  Don't override here.
  initConfig.LineEventNotif     = (DALLineEventFunc) handle_line_event_isr;
  initConfig.TransmitReadyNotif = (DALNotifyFunc)    tx_isr; 
  initConfig.ReceiveReadyNotif  = (DALNotifyFunc)    rx_isr;

  if( initConfig.enable_dma )
  {
    UART_LOG_1( TRACE, "[%d] Configure device to use DMA.", device_id );
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] Configure device to use PIO.", device_id );
  }

  if( DalUart_PostInit( dal_device_handle, &initConfig ) != DAL_SUCCESS )
  {
    UART_LOG_1( ERROR, "[%d] DalUart_PostInit failed.", device_id );
    goto FAILURE;
  }

  //  Success.

  mdd_port->port_data.dal_device_handle = dal_device_handle;

  UART_LOG_1( HIGH, "[%d] -initialize_device", device_id );

  return;

FAILURE:;

  //  Clean up, log it, return.

  if( mdd_port->port_data.lock_handle )
  {
    destroy_lock( &mdd_port->port_data.lock_handle );
  }

  if( dal_device_handle != NULL )
  {
    DAL_DeviceDetach( dal_device_handle );
  }

  UART_LOG_1( HIGH, "[%d] -initialize_device", device_id );
}

/*===========================================================================

FUNCTION
  siors_mdd_set_dsm_pool_id

DESCRIPTION
  Sets the DSM pool id to use for RX data.
  open_ptr == NULL implies this function is invoked from an ioctl.

DEPENDENCIES
  stream_mode must be set in mdd_port before calling this function.

RETURN VALUE
  The dsm pool id to use

SIDE EFFECTS
  None

===========================================================================*/
dsm_mempool_id_type
siors_mdd_set_dsm_pool_id (const sio_open_type *open_ptr, sio_mdd_rx_data_type *rxd_ptr)
{
  dsm_mempool_id_type dsm_pool_id = 0;

  /* If coming through an ioctl (no open_ptr), and if an explicit pool is specified
   * by the client, then don't change the pool.
   */
  if ((open_ptr == NULL) && rxd_ptr->use_explicit_sio_rx_dsm_pool)
  {
    return rxd_ptr->sio_rx_dsm_pool;
  }

  /* ioctl with no explicit pool specified, or open stream cases */
  if ((open_ptr == NULL) || (open_ptr->open_param == NULL) ||
      (open_ptr->open_param->dsm_pool_param.dsm_mempool_id == 0))
  {
    switch (rxd_ptr->sio_current_stream_mode)
    {
      case SIO_DS_RAWDATA_MODE:
      case SIO_DS_AUTODETECT_MODE:
      case SIO_GENERIC_MODE:

        dsm_pool_id = DSM_DS_SMALL_ITEM_POOL;
        break;

      case SIO_DS_PKT_MODE:
        if (rxd_ptr->sio_uart_packet_mode)
        {
          #ifdef FEATURE_DSM_LARGE_ITEMS
          dsm_pool_id = DSM_DS_LARGE_ITEM_POOL;
          #else
          dsm_pool_id = DSM_DS_SMALL_ITEM_POOL;
          #endif

        }
        else
        {
          dsm_pool_id = DSM_DS_SMALL_ITEM_POOL;
        }
        break;

      case SIO_DM_MODE:
        dsm_pool_id = DSM_DIAG_ITEM_POOL;
        break;

      default:
        dsm_pool_id = 0;
        break;
    }
    rxd_ptr->use_explicit_sio_rx_dsm_pool = FALSE;
  }
  else
  {
    dsm_pool_id = open_ptr->open_param->dsm_pool_param.dsm_mempool_id;
    rxd_ptr->use_explicit_sio_rx_dsm_pool = TRUE;
  }

  rxd_ptr->sio_rx_dsm_pool = dsm_pool_id;
  return dsm_pool_id;
}

/*===========================================================================

FUNCTION                                                    INTERNAL FUNCTION
  alloc_stream_rx

DESCRIPTION
  This function allocates RX.  If all available RX streams are allocated
  (can be just one), this routine returns SIO_UNAVAIL_S.

DEPENDENCIES
  Should only be called from sio_open.  open_ptr should be the same pointer
  which was passed to sio_open.

RETURN VALUE
  SIO_DONE_S: RX was successfully allocated.
  SIO_UNAVAIL_S: RX could not be allocated.

SIDE EFFECTS
  None

===========================================================================*/

static sio_status_type
alloc_stream_rx( sio_stream_id_type stream_id, sio_mdd_port_type* mdd_port, const sio_open_type* open_ptr )
{
  int                              device_id = mdd_port->port_data.device_id;
  sio_mdd_rx_data_type*            rxd = &mdd_port->rx_data;
  sio_status_type                  return_val;

  UART_LOG_1( TRACE, "[%d] +alloc_stream_rx", device_id );

  if( rxd->is_open )
  {
    //  If rx resource is already allocated.
    UART_LOG_1( TRACE, "[%d] Device already allocated for rx.", device_id );
    return_val = SIO_UNAVAIL_S;
    goto DONE;
  }

  if( !is_rx_stream( open_ptr->stream_type ))
  {
    return_val = SIO_DONE_S;
    goto DONE;
  }

  uart_circ_buffer_init( &rxd->circular_buffer );

  rxd->is_open = TRUE;

  //  If tail character is defined, SIO will always
  //  end a packet with the unescaped tail character,
  //  if it sees a tail character.

  rxd->sio_tail_char_used = open_ptr->tail_char_used;
  rxd->sio_tail_char      = open_ptr->tail_char;

  //  These are parameters for backwards compatible
  //  functions, which are handled with extra calls
  //  that only specific streams are allowed to use.
  //  For now, reset these values.

  rxd->sio_escape_code_func_ptr   = NULL;
  rxd->sio_packet_func_ptr        = NULL;

  rxd->sio_rx_watermark_queue_ptr = open_ptr->rx_queue;
  rxd->sio_rx_func_ptr            = open_ptr->rx_func_ptr;

  rxd->total_overruns             = 0;
  rxd->sio_rx_flow_control_method = open_ptr->rx_flow;

  rxd->sio_current_stream_mode = open_ptr->stream_mode;

  if (siors_mdd_set_dsm_pool_id (open_ptr, rxd) == 0)
  {
    UART_LOG_2( ERROR, "[%d] Invalid dsm pool detected for stream mode: %d.", device_id, open_ptr->stream_mode);
    return_val = SIO_UNAVAIL_S;
    goto DONE;
  }

#ifdef SIORS_HAS_AUTODETECT  //{
  {
    sio_mdd_autodetect_var_type*     padv;

    padv = &mdd_port->rx_data.adv;

    if(( rxd->sio_current_stream_mode == SIO_DS_RAWDATA_MODE ) ||
       ( rxd->sio_current_stream_mode == SIO_DS_PKT_MODE ))
    {
      //  Check to insure that RAW_DATA mode is not already
      //  active on either UART. If so kill the phone as
      //  this is a pathological error that should never
      //  exit lab testing.

      if( padv->ds_incall_device_id != UART_DEVICE_INVALID )
      {
        UART_LOG_1( FATAL, "[%d] Multiple UARTS in 'data call' mode.", device_id );
        return_val = SIO_UNAVAIL_S;
        goto DONE;
      }

      //  Assign Port and Stream IDs for
      //  DS-in-call mode operation

      /* TODO:  Fix the DS incall */
      padv->ds_incall_device_id = mdd_port->port_data.device_id;
      padv->ds_incall_stream_id = stream_id;

      if( rxd->sio_current_stream_mode == SIO_DS_RAWDATA_MODE )
      {
        //  Transition to RawData stream involves connecting
        //  a call, which takes many seconds.  Therefore,
        //  for all practical purposes, the very first
        //  guard time has happened.

        rxd->sio_esc_seq_state = SIO_ESC_SEQ_1ST_GUARDTIME;
      }
    }
    else if( rxd->sio_current_stream_mode == SIO_DS_AUTODETECT_MODE )
    {
      //  Perform Run-time Mode check then Setup UART, if all is proper.

      //  Assign Port and Stream IDs for Autodetect mode operation.

      padv->autodetect_stream_id = stream_id;
      padv->autodetect_device_id = mdd_port->port_data.device_id;

      siors_mdd_setup_uart_autodetect( mdd_port );
    }
  }
#endif //}

  return_val = SIO_DONE_S;

DONE:;

  UART_LOG_1( TRACE, "[%d] -alloc_stream_rx", device_id );

  return( return_val );
}

/*===========================================================================

FUNCTION                                                    INTERNAL FUNCTION
  dealloc_stream_rx

DESCRIPTION
  This function deallocates RX.

DEPENDENCIES
  Should only be called from sio_open or sio_close.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
dealloc_stream_rx( sio_stream_id_type stream_id, sio_mdd_port_type* mdd_port )
{
  int                              device_id = mdd_port->port_data.device_id;
  sio_mdd_rx_data_type*            rxd = &mdd_port->rx_data;
  sio_mdd_autodetect_var_type*     padv;

  UART_LOG_1( TRACE, "[%d] +dealloc_stream_rx", device_id );

  if( FALSE == rxd->is_open )
  {
    goto DONE;
  }

  rxd->is_open                    = FALSE;
  rxd->sio_rx_watermark_queue_ptr = NULL;
  rxd->sio_rx_func_ptr            = NULL;

  rxd->use_explicit_sio_rx_dsm_pool = FALSE;
  rxd->sio_rx_dsm_pool = 0;

  padv = &mdd_port->rx_data.adv;

  //  Do book-keeping necessary for just SIO_RAWDATA_STREAM.

  if( rxd->sio_current_stream_mode == SIO_DS_RAWDATA_MODE ||
      rxd->sio_current_stream_mode == SIO_DS_PKT_MODE)
  {

    //  Clear DS-in-call IDs

    padv->ds_incall_device_id = UART_DEVICE_INVALID;
    padv->ds_incall_stream_id = SIO_NO_STREAM_ID;

    padv->autodetect_device_id = UART_DEVICE_INVALID;
    padv->autodetect_stream_id = SIO_NO_STREAM_ID;

    if( rxd->sio_current_stream_mode == SIO_DS_RAWDATA_MODE )
    {
      //  De register the clock call-back function used by
      //  SIO_RAWDATA_STREAM. Clear the DS-in-call mode
      //  IDs

      timer_clr_64( &rxd->sio_esc_seq_cb_item, T_NONE );
    }
  }
  else if( rxd->sio_current_stream_mode == SIO_DS_AUTODETECT_MODE )
  {
    //  De-register the discard_recovery_cb_isr, reset discard_active flag

    timer_clr_64( &padv->pausecheck_cb_item, T_NONE );
    padv->state = SIO_AD_INIT;

    //  De-register RX Break callback. Clear Autodetect mode
    //  IDs.

    // padv->autodetect_stream_id = SIO_NO_STREAM_ID;
  }

DONE:;

  UART_LOG_1( TRACE, "[%d] -dealloc_stream_rx", device_id );
}

/*===========================================================================

FUNCTION                                                    INTERNAL FUNCTION
  alloc_stream_tx

DESCRIPTION
  This function allocates TX.  If all available TX streams are allocated
  (can be just one), this routine returns SIO_UNAVAIL_S.

DEPENDENCIES
  Should only be called from sio_open.  open_ptr should be the same pointer
  which was passed to sio_open.

RETURN VALUE
  SIO_DONE_S: TX was successfully allocated.
  SIO_UNAVAIL_S: TX could not be allocated.

SIDE EFFECTS
  None

===========================================================================*/

static sio_status_type
alloc_stream_tx( sio_stream_id_type stream_id,
                 sio_mdd_port_type* mdd_port,
                 const sio_open_type* open_ptr )
{
  int                         device_id = mdd_port->port_data.device_id;
  sio_mdd_tx_data_type*       txd = &mdd_port->tx_data;
  sio_status_type             return_val;



  UART_LOG_1( TRACE, "[%d] +alloc_stream_tx", device_id );

  if( txd->is_open )
  {
    //  If tx resource is already allocated.
    UART_LOG_1( TRACE, "[%d] Device already allocated for tx.", device_id );
    return_val = SIO_UNAVAIL_S;
    goto DONE;
  }

  if( !is_tx_stream( open_ptr->stream_type ))
  {
    return_val = SIO_DONE_S;
    goto DONE;
  }

  //  Reset all of the tx variables upon opening of each
  //  stream.

  txd->is_open                    = TRUE;
  txd->sio_tx_watermark_queue_ptr = open_ptr->tx_queue;
  txd->sio_tx_current_wm_item_ptr = NULL;
  txd->sio_bytes_to_tx            = 0;
  txd->tx_break_state             = FALSE;
  txd->tx_in_progress             = FALSE;
  txd->sio_tx_flow_control_method = open_ptr->tx_flow;

  return_val = SIO_DONE_S;

DONE:;

  UART_LOG_1( TRACE, "[%d] -alloc_stream_tx", device_id );

  return( return_val );
}

/*===========================================================================

FUNCTION                                                    INTERNAL FUNCTION
  dealloc_stream_tx

DESCRIPTION
  This function deallocates TX.

DEPENDENCIES
  Should only be called from sio_open or sio_close.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
dealloc_stream_tx( sio_stream_id_type stream_id, sio_mdd_port_type* mdd_port )
{
  int                          device_id = mdd_port->port_data.device_id;
  sio_mdd_tx_data_type*        txd = &mdd_port->tx_data;
  dsm_item_type*               item_ptr;



  UART_LOG_1( HIGH, "[%d] +dealloc_stream_tx", device_id );

  if( FALSE == txd->is_open )
    goto DONE;

  txd->is_open = FALSE;

  //  The pending tx should be completed / flushed before this
  //  function is called.

  dsm_free_packet( &txd->sio_tx_current_wm_item_ptr );
  txd->sio_tx_current_wm_item_ptr = NULL;
  txd->sio_bytes_to_tx            = 0;

  if(( item_ptr = dsm_dequeue( txd->sio_tx_watermark_queue_ptr )) != NULL )
  {
    UART_LOG_1( ERROR, "[%d] dealloc_stream_tx called with non-emtpy tx queue.", device_id );
    dsm_free_buffer( item_ptr);
    dsm_empty_queue( txd->sio_tx_watermark_queue_ptr );
  }

  txd->sio_tx_watermark_queue_ptr = NULL;

DONE:;

  UART_LOG_1( HIGH, "[%d] -dealloc_stream_tx", device_id );
}

/*===========================================================================

FUNCTION
  do_transmit

DESCRIPTION
  Transmit some data.

DEPENDENCIES
  This function does not do any locking.  It's assumed the locking is
  done at a higher level and will only be called from within a locked
  context.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

static void
do_transmit( sio_mdd_port_type* mdd_port )
{
  int                          device_id = mdd_port->port_data.device_id;
  sio_mdd_tx_data_type*        txd = &mdd_port->tx_data;
  uint32                       byte_count;
  uint32                       bytes_written;
  DALResult                    dal_result;




  UART_LOG_1( TRACE, "[%d] +do_transmit", device_id );

  get_more_data_for_tx( mdd_port );

  if( txd->sio_bytes_to_tx <= 0 )
    goto DONE;

  UART_LOG_2( TRACE, "[%d] sio_bytes_to_tx = %d.", device_id, txd->sio_bytes_to_tx );

    byte_count = txd->sio_bytes_to_tx;

    dal_result = DalUart_Write( mdd_port->port_data.dal_device_handle,
                                txd->sio_current_tx_ptr,
                                byte_count,
                                &bytes_written );

  if( dal_result != DAL_SUCCESS )
  {
    UART_LOG_2( TRACE, "[%d] Error - DalUart_Write() = %d.", device_id, dal_result );
    bytes_written = 0;
  }
  else
  {
    //  Transfer in progress.

    txd->tx_in_progress = TRUE;
  }

  //  Log it.

  UART_BUFFER_LOG_2( txd->sio_current_tx_ptr, bytes_written, "[%d] TX %d bytes:", device_id, bytes_written );

  //  Update state for bytes just written out.

  txd->sio_bytes_to_tx    -= bytes_written;
  txd->sio_current_tx_ptr += bytes_written;

DONE:;

  UART_LOG_1( TRACE, "[%d] -do_transmit", device_id );
}

/*===========================================================================

FUNCTION RX_ACTIVITY

DESCRIPTION
  Called when the PDD layer reports a detection of the RX activity
  The function need to decide if wakeup is required, or RX activity
  callback is required

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
rx_activity( sio_mdd_port_type* mdd_port )
{
  uint32                      device_id = mdd_port->port_data.device_id;
  sio_mdd_rx_data_type*       rxd = &mdd_port->rx_data;
  sio_vv_func_ptr_type        on_rx_activity;
  boolean                     rx_locked = FALSE;



  UART_LOG_1( TRACE, "[%d] +rx_activity", device_id );

  //  Get lock.

  DALSYS_SyncEnter( rxd->lock_handle );
  rx_locked = TRUE;

  //  Always check state after acquiring lock.

  if( !rxd->is_open )
  {
    UART_LOG_1( TRACE, "[%d] Port not open, can't wake up.", device_id );
    goto DONE;
  }

  //  Generate a callback if one is required.  Unlock the
  //  port before calling the callback.

  on_rx_activity = rxd->rx_activity_cb_func_ptr;
  rxd->rx_activity_cb_func_ptr = NULL;

  DALSYS_SyncLeave( rxd->lock_handle );
  rx_locked = FALSE;

  //  Decide if the port needs to be reopened.

  rx_wakeup( mdd_port );

  if( on_rx_activity )
  {
    on_rx_activity();
  }

DONE:;

  if( rx_locked )
  {
    DALSYS_SyncLeave( rxd->lock_handle );
  }

  UART_LOG_1( TRACE, "[%d] -rx_activity", device_id );
}

/*===========================================================================

FUNCTION BREAK_START

DESCRIPTION
  Called when the PDD layer reports detection of a break start condition.
  This function will power down the UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
break_start( sio_mdd_port_type* mdd_port )
{
  uart_device_info         device_info;
  uart_device_id_type      device_id = mdd_port->port_data.device_id;



  UART_LOG_1( TRACE, "[%d] +break_start", device_id );

  if( !uart_get_device_info( device_id, &device_info ))
  {
    UART_LOG_1( ERROR, "[%d] UART device id is invalid.", device_id );
    goto DONE;
  }

  siors_mdd_powerdown( mdd_port, SIO_PDR_NO_CABLE );

DONE:;

  UART_LOG_1( TRACE, "[%d] -break_start", device_id );
}

/*===========================================================================

FUNCTION BREAK_END

DESCRIPTION
  Called when the PDD layer reports detection of a break end condition.
  This function will power up the UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
break_end( sio_mdd_port_type* mdd_port )
{
  uart_device_info         device_info;
  uart_device_id_type      device_id = mdd_port->port_data.device_id;



  UART_LOG_1( TRACE, "[%d] +break_end", device_id );

  if( !uart_get_device_info( device_id, &device_info ))
  {
    UART_LOG_1( ERROR, "[%d] UART device id is invalid.", device_id );
    goto DONE;
  }

  siors_mdd_wakeup( mdd_port );

DONE:;

  UART_LOG_1( TRACE, "[%d] -break_end", device_id );
}

/*===========================================================================

FUNCTION HANDLE_LINE_EVENT_ISR

DESCRIPTION
  This is the function that gets called in ISR context.  Don't do anything
  except signal the workloop handler.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
handle_line_event_isr( sio_mdd_port_type* mdd_port, uint32 line_events )
{
  int device_id = mdd_port->port_data.device_id;


  UART_LOG_1( TRACE, "[%d] +handle_line_event_isr", device_id );

  UART_LOG_2( TRACE, "[%d] adding line events 0x%08x to queue.", device_id, line_events );

  uart_event_queue_add_item( &mdd_port->event_queue, line_events );

  DALSYS_EventCtrl( mdd_port->workloop_event[UART_WORKLOOP_LINE_EVENT],
                    DALSYS_EVENT_CTRL_TRIGGER );

  UART_LOG_1( TRACE, "[%d] -handle_line_event_isr", device_id );
}

/*===========================================================================

FUNCTION WORKLOOP_HANDLE_LINE_EVENT

DESCRIPTION
  Call the function to handle line events.  This function is executed
  in workloop, instead of ISR, context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static DALResult
workloop_handle_line_event( DALSYSEventHandle event_handle, void* context )
{
  sio_mdd_port_type* mdd_port = (sio_mdd_port_type*)context;
  int                device_id;


  device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +workloop_handle_line_event", device_id );

  handle_line_event( mdd_port );

  UART_LOG_1( TRACE, "[%d] -workloop_handle_line_event", device_id );

  return DAL_SUCCESS;
}

/*===========================================================================

FUNCTION HANDLE_LINE_EVENT

DESCRIPTION
  Do the real processing for handle_line_event.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static void
handle_line_event( sio_mdd_port_type* mdd_port )
{
  sio_mdd_tx_data_type*       txd = &mdd_port->tx_data;
  int                         device_id = mdd_port->port_data.device_id;
  uint32                      all_line_events[UART_EVENT_QUEUE_SIZE];
  uint32                      line_events;
  int                         num_items;
  int                         i;



  UART_LOG_1( TRACE, "[%d] +handle_line_event", device_id );

  //  Get all of the events from the queue at once so
  //  we can process them without holding the lock on
  //  the event queue.

  num_items = UART_EVENT_QUEUE_SIZE;
  uart_event_queue_remove_all_items( &mdd_port->event_queue, all_line_events, &num_items );

  UART_LOG_2( TRACE, "[%d] %d events in queue", device_id, num_items );

  for( i = 0; i < num_items; i++ )
  {
    line_events = all_line_events[i];

    UART_LOG_2( TRACE, "[%d] Processing line_events 0x%08x", device_id, line_events );

    //  Each item is a bitfield that can multiple events set.

    //  EVENT_TXEMPTY

    if( line_events & EVENT_TXEMPTY )
    {
      DALSYS_SyncEnter( txd->lock_handle );

      if( !txd->is_open )
      {
        //  Do nothing.  Port state has changed
        //  underneath us.
      }
      else if( TX_STREAM_STATE_FLUSH_WAIT_FLUSH_TX_FIFO == txd->tx_stream_state )
      {
        //  May have started another transmission by the time this notification
        //  is received.  Check state.

        if( !more_data_to_tx( mdd_port ))
          poll_tx_complete( mdd_port );
      }

      DALSYS_SyncLeave( txd->lock_handle );
    }

    //  EVENT_RX_WAKEUP

    if( line_events & EVENT_RX_WAKEUP )
    {
      //  Detected RX activity while powered down.

      rx_activity( mdd_port );
    }

    //  EVENT_BREAK

    if( line_events & EVENT_BREAK )
    {
      //  Detected break start.  This, in theory, corresponds
      //  to cable removal.

      break_start( mdd_port );
    }

    //  EVENT_BREAK_END

    if( line_events & EVENT_BREAK_END )
    {
      //  Detected break end.  This, in theory, corresponds
      //  to cable insertion.

      break_end( mdd_port );
    }
  }

  UART_LOG_1( TRACE, "[%d] -handle_line_event", device_id );
}

/*===========================================================================

FUNCTION SIORS_MDD_ON_INACTIVITY_ISR

DESCRIPTION
  Called in timer context on inactivity timer expiration to power down UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void
siors_mdd_on_inactivity_isr( int32 time_ms, timer_cb_data_type data )
{
   sio_mdd_port_type*      mdd_port = (sio_mdd_port_type*) data;
   int                     device_id;


   device_id = mdd_port->port_data.device_id;

   UART_LOG_1( TRACE, "[%d] +siors_mdd_on_inactivity_isr", device_id );

   DALSYS_EventCtrl( mdd_port->workloop_event[UART_WORKLOOP_INACTIVITY_EVENT],
                     DALSYS_EVENT_CTRL_TRIGGER );

   UART_LOG_1( TRACE, "[%d] -siors_mdd_on_inactivity_isr", device_id );
}

/*===========================================================================

FUNCTION WORKLOOP_HANDLE_INACTIVITY_EVENT

DESCRIPTION
  Call the function to handle inactivity event.  This function is executed
  in workloop, instead of timer context.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

static DALResult
workloop_handle_inactivity_event( DALSYSEventHandle event_handle, void* context )
{
  sio_mdd_port_type* mdd_port = (sio_mdd_port_type*)context;
  int device_id;
  uint32 rx_active;


  device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +workloop_handle_inactivity_event", device_id );

  DALSYS_SyncEnter( mdd_port->port_data.lock_handle );

  if( !mdd_port->port_data.is_clock_on )
  {
    UART_LOG_1( TRACE, "[%d] Clock is already off, skipping powerdown.", device_id );
    goto DONE;
  }

  //Check for active RX transfer
  DalUart_RxActive(mdd_port->port_data.dal_device_handle, &rx_active);

  if (rx_active)
  {
   
    UART_LOG_2(TRACE, "[%d] RX path is active. Resetting inactivity timer to %d ms.",
               device_id,
               mdd_port->port_data.inactivity_ms );

    if(mdd_port->port_data.inactivity_ms > 0)
    {

      timer_reg( &mdd_port->port_data.inactivity_timer,
                 siors_mdd_on_inactivity_isr,
                 (timer_cb_data_type) mdd_port,
                 mdd_port->port_data.inactivity_ms,
                 0 );
    }
  }
  else
  {
    siors_mdd_powerdown( mdd_port, SIO_PDR_INACTIVITY ); 
  }

DONE:
  DALSYS_SyncLeave( mdd_port->port_data.lock_handle );

  UART_LOG_1( TRACE, "[%d] -workloop_handle_inactivity_event", device_id );

  return DAL_SUCCESS;
}

/*===========================================================================

FUNCTION SIORS_MDD_POWERDOWN

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

sio_status_type
siors_mdd_powerdown( sio_mdd_port_type* mdd_port, sio_pd_reason_type reason )
{
  DALResult            dal_result;
  UartPowerType        power_level;
  uart_device_info     device_info;
  int                  device_id;
  boolean              tx_locked = FALSE;
  sio_status_type      return_val;

  device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +siors_mdd_powerdown", device_id );

  DALSYS_SyncEnter( mdd_port->port_data.lock_handle );

  if (!mdd_port->stream_data.is_open)
  {
    UART_LOG_1( TRACE, "[%d] Port is already closed, skipping powerdown.", device_id );
    return_val = SIO_DONE_S;
    goto DONE;
  }

  if( !mdd_port->port_data.is_clock_on )
  {
    UART_LOG_1( TRACE, "[%d] Clock is already off, skipping powerdown.", device_id );
    return_val = SIO_DONE_S;
    goto DONE;
  }
  
  DALSYS_SyncEnter( mdd_port->tx_data.lock_handle );
  tx_locked = TRUE;

  // Don't powerdown if a transfer is in progress unless the reason is break detection.
  if( mdd_port->tx_data.tx_in_progress && (reason != SIO_PDR_NO_CABLE))
  {
    UART_LOG_1( TRACE, "[%d] Transmit in progress, skipping powerdown.", device_id );
    return_val = SIO_BADP_S;
    goto DONE;
  }

  DALSYS_SyncLeave( mdd_port->tx_data.lock_handle );
  tx_locked = FALSE;
  
  if( !uart_get_device_info( mdd_port->port_data.device_id, &device_info ))
  {
    UART_LOG_1( ERROR, "[%d] Error getting device info.", device_id );
    return_val = SIO_BADP_S;
    goto DONE;
  }

  timer_clr_64( &mdd_port->port_data.inactivity_timer, T_NONE );
  mdd_port->port_data.power_down_reason = reason;

  //  The DalUart_PowerSet function will register the wakeup interrupt at POWER_025.
  //  At POWER_OFF it goes to sleep and doesn't set the wakeup interrupt.
  //  For break condition (SIO_PDR_NO_CABLE) the wakeup interrupt will be rising edge 
  //  interrupt which will be already registered by the break handler in DAL layer.
  if (reason == SIO_PDR_NO_CABLE)
  {
    power_level = POWER_OFF;
  }
  else
  {
     power_level = (device_info.can_wake_on_rx_event ? POWER_025 : POWER_OFF); 
  }
  dal_result = DalUart_PowerSet( mdd_port->port_data.dal_device_handle, power_level );
  if( DAL_SUCCESS == dal_result )
  {
    mdd_port->port_data.is_clock_on = FALSE;
    return_val = SIO_DONE_S;
  }
  else
  {
    UART_LOG_2( TRACE, "[%d] Error, DalUart_PowerSet() = %d.", device_id, dal_result );
    return_val = SIO_BADP_S;
  }

DONE:
  if( tx_locked )
  {
    DALSYS_SyncLeave( mdd_port->tx_data.lock_handle );
  }

  DALSYS_SyncLeave( mdd_port->port_data.lock_handle );

  UART_LOG_1( TRACE, "[%d] -siors_mdd_powerdown", device_id );
  return ( return_val );
}

/*===========================================================================

FUNCTION

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void
siors_mdd_wakeup( sio_mdd_port_type* mdd_port )
{
  int              device_id = mdd_port->port_data.device_id;
  DALResult        dal_result;

  UART_LOG_1( TRACE, "[%d] +siors_mdd_wakeup", device_id );

  DALSYS_SyncEnter( mdd_port->port_data.lock_handle );

  if (!mdd_port->stream_data.is_open)
  {
    UART_LOG_1( TRACE, "[%d] Port is already closed, skipping wakeup.", device_id );
    goto DONE;
  }

  if( mdd_port->port_data.is_clock_on )
  {
    UART_LOG_1( TRACE, "[%d] Clock is already on, skipping wakeup.", device_id );
    goto DONE;
  }

  dal_result = DalUart_PowerSet( mdd_port->port_data.dal_device_handle, POWER_FULL );
  if( DAL_SUCCESS == dal_result )
  {
    mdd_port->port_data.is_clock_on = TRUE;
    siors_mdd_update_flow_control( mdd_port );
  }
  else
  {
    UART_LOG_2( TRACE, "[%d] Wakeup failed, error = %d", device_id, dal_result );
  }

  mdd_port->port_data.power_down_reason = SIO_PDR_INVALID;
  if( mdd_port->port_data.inactivity_ms > 0 )
  {
     timer_reg( &mdd_port->port_data.inactivity_timer,
                siors_mdd_on_inactivity_isr,
                (timer_cb_data_type) mdd_port,
                mdd_port->port_data.inactivity_ms,
                0 );
  }

DONE:
  DALSYS_SyncLeave( mdd_port->port_data.lock_handle );

  UART_LOG_1( TRACE, "[%d] -siors_mdd_wakeup", device_id );
}

/*===========================================================================

FUNCTION SIORS_MDD_ON_UART_ACTIVITY

DESCRIPTION
  Call this function when there's some kind of UART activity that qualifies
  as "activity".  The inactivity timer will be reset.
  If UART driver has been shutdown UART due to inactivity then it also re-activates it.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void
siors_mdd_on_activity( sio_mdd_port_type* mdd_port )
{
  int device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +siors_mdd_on_activity", device_id );

  DALSYS_SyncEnter( mdd_port->port_data.lock_handle );

  if( mdd_port->port_data.is_clock_on )
  {
     UART_LOG_1( TRACE, "[%d] clock is on.", device_id  );
  }
  else
  {
     UART_LOG_1( TRACE, "[%d] clock is off.", device_id  );
  }

  //  Wake up the UART iff the UART was put to sleep due
  //  to inactivity.  This means the semantics of the UART
  //  with inactivity timer is the same as before the
  //  inactivity timer.

  if( !mdd_port->port_data.is_clock_on )
  {
     if( SIO_PDR_INACTIVITY == mdd_port->port_data.power_down_reason )
     {
        UART_LOG_1( TRACE, "[%d] implicit wakeup.", device_id );
        siors_mdd_wakeup( mdd_port );
     }
  }

  //  If the UART now powered up, start (or reset)
  //  the inactivity timer.
  if( mdd_port->port_data.is_clock_on )
  {
     if( mdd_port->port_data.inactivity_ms > 0 )
     {
        UART_LOG_2( TRACE, "[%d] reset inactivity timer to %d ms.",
                    device_id,
                    mdd_port->port_data.inactivity_ms );
         timer_reg( &mdd_port->port_data.inactivity_timer,
                   siors_mdd_on_inactivity_isr,
                   (timer_cb_data_type) mdd_port,
                   mdd_port->port_data.inactivity_ms,
                   0 );
     }
     else
     {
        UART_LOG_1( TRACE, "[%d] clear inactivity timer.", device_id );
        timer_clr_64( &mdd_port->port_data.inactivity_timer, T_NONE );
     }
  }

  DALSYS_SyncLeave( mdd_port->port_data.lock_handle );

  UART_LOG_1( TRACE, "[%d] -siors_mdd_on_activity", device_id );
}

/*===========================================================================

FUNCTION SIORS_MDD_DSM_NEW_BUFFER_NULL_CLEANUP

DESCRIPTION
  Cleans up when dsm_new_buffer() returns a NULL meaning that no buffer
  is available. Flushes Rx FIFO and sets up for recovery.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void
siors_mdd_dsm_new_buffer_null_cleanup ( sio_mdd_port_type* mdd_port )
{
  int                             device_id = mdd_port->port_data.device_id;
  uart_circular_buffer_type*      cb_ptr;
  sio_mdd_rx_data_type*           rxd = &mdd_port->rx_data;
  uint32                          num_overruns;
  uint32                          bytes_read;
  uint8*                          buffer_ptr;
  uint32                          buffer_len;




  UART_LOG_1( TRACE, "[%d] +siors_mdd_dsm_new_buffer_null_cleanup", device_id );

  cb_ptr = &rxd->circular_buffer;

  //  Clear the RX channel.  We used to call DalUart_PurgeRx.  That call chain
  //  ends up calling malloc, which causes an ASSERT failure if executed
  //  in ISR context.  This function is always executed in ISR context.

  if( is_buffered_read( mdd_port ))
  {
    //  We've already read from the PDD buffers into the
    //  circular buffer.  Dump the data in the circular buffer.

    uart_circ_buffer_clear( cb_ptr );
  }
  else
  {
    //  We haven't used the circular buffer as part of normal
    //  processing.  Use it as a handy destination buffer
    //  for reading.

    //  Make sure the circular buffer is initialized.

    uart_circ_buffer_init( cb_ptr );
  }

  //  Read and discard all data in the PDD buffers.

  buffer_ptr = uart_circ_buffer_get_buffer_start_ptr( cb_ptr );
  buffer_len = uart_circ_buffer_get_size( cb_ptr );

  do
  {
    //  Read data.

    DalUart_Read( mdd_port->port_data.dal_device_handle,
                  buffer_ptr,
                  buffer_len,
                  &bytes_read,
                  &num_overruns );

    //  Discard it.

    uart_circ_buffer_clear( &rxd->circular_buffer );
  }
  while( bytes_read == buffer_len );

  UART_LOG_1( TRACE, "[%d] -siors_mdd_dsm_new_buffer_null_cleanup", device_id );
}

/*===========================================================================

FUNCTION                                                EXTERNALIZED FUNCTION
  siors_mdd_api_dev_init

DESCRIPTION
  Initialize the MSM UART serial interface for operation and initialize
  data structures.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void
siors_mdd_api_dev_init( void )
{
  uart_device_id_type          device_id;



  //  Do DAL initialization first -- before initializing
  //  locking, before makig any UART_LOG logging calls.

  DALSYS_InitMod( NULL );

  UART_LOG( HIGH, "+siors_mdd_api_dev_init");

  siors_mdd_dal_log_func = DALSYS_SetLogCfg( uart_dal_log_level,
      uart_dal_logger );

  // Make sure the UART timer group is non-deferrable
  timer_group_set_deferrable(&uart_timer_group, FALSE);

  for( device_id = UART_DEVICE_FIRST; device_id < UART_DEVICE_LAST; device_id++ )
  {
    initialize_device( device_id );
  }

  UART_LOG( HIGH, "-siors_mdd_api_dev_init");
}

/*===========================================================================

FUNCTION                                                EXTERNALIZED FUNCTION
  siors_mdd_api_open_stream

DESCRIPTION
  This function allocates stream as indicated in open_ptr.

DEPENDENCIES
  None

RETURN VALUE
  SIO_DONE_S: TX was successfully allocated.
  SIO_UNAVAIL_S: TX could not be allocated.

SIDE EFFECTS
  None

===========================================================================*/

sio_status_type
siors_mdd_api_open_stream( sio_open_type* open_ptr )
{
  sio_status_type          return_val;
  sio_stream_id_type       stream_id;
  sio_port_id_type         port_id;
  sio_mdd_port_type*       mdd_port;
  uart_device_id_type      device_id;
  uart_device_info         device_info;
  DALResult                dal_result;
  UartOpenConfig           open_config;
  boolean                  rx_allocated;
  boolean                  tx_allocated;
  boolean                  port_locked;
  boolean                  kill_locks;






  return_val   = SIO_DONE_S;
  rx_allocated = FALSE;
  tx_allocated = FALSE;
  port_locked  = FALSE;
  kill_locks   = FALSE;
  mdd_port     = NULL;
  stream_id    = -1;

  // Reset the DAL logging level in case another compenent has set a higher
  // logging level

  DALSYS_SetLogCfg( uart_dal_log_level, NULL );

  //  Verify input parameters.

  if( NULL == open_ptr )
  {
    UART_LOG( ERROR, "open_ptr is NULL.");
    return( SIO_BADP_S );
  }

  port_id   = open_ptr->port_id;
  device_id = uart_sio_port_id_to_device_id( port_id );

  if( UART_DEVICE_INVALID == device_id )
  {
    UART_LOG_1( ERROR, "UART port id %d is invalid.", port_id );
    return( SIO_BADP_S );
  }

  UART_LOG_1( HIGH, "[%d] +siors_mdd_api_open_stream", device_id );

  if( open_ptr->rx_bitrate != open_ptr->tx_bitrate )
  {
    UART_LOG_1( HIGH, "[%d] rx bitrate used, tx bitrate ignored.", device_id );
  }

  if( !uart_get_device_info( device_id, &device_info ))
  {
    UART_LOG_1( ERROR, "[%d] UART device id is invalid.", device_id );
    return_val = SIO_BADP_S;
    goto FAILURE;
  }

  if( !( device_info.is_present && device_info.is_valid ))
  {
    UART_LOG_1( ERROR, "[%d] UART device id is invalid.", device_id );
    return_val = SIO_BADP_S;
    goto FAILURE;
  }

  //  Get the port.

  stream_id = open_ptr->stream_id;
  mdd_port  = &siors_mdd_ports[device_id];

  //  Can this device be opened?

  if( NULL == mdd_port->port_data.lock_handle )
  {
    UART_LOG_1( HIGH, "[%d] Device not initialized (NULL lock handle).", device_id );
    //  Not much choice for return codes.
    return_val = SIO_BADP_S;
    goto FAILURE;
  }

  DALSYS_SyncEnter( mdd_port->port_data.lock_handle );
  port_locked = TRUE;

  if( NULL == mdd_port->port_data.dal_device_handle )
  {
    UART_LOG_1( HIGH, "[%d] Device not initialized (NULL dal handle).", device_id );
    //  Not much choice for return codes.
    return_val = SIO_BADP_S;
    goto FAILURE;
  }

  if( TRUE == mdd_port->stream_data.is_open )
  {
    UART_LOG_1( TRACE, "[%d] Calling open on open device.", device_id );
    return_val = SIO_BADP_S;
    goto FAILURE;
  }

  //  From here, if we hit a failure, destroy the locks.

  kill_locks = TRUE;
  mdd_port->rx_data.lock_handle = NULL;
  mdd_port->tx_data.lock_handle = NULL;

  //  Initialize rx structures.
  if( !create_lock( &mdd_port->rx_data.lock_handle ))
  {
    return_val = (sio_status_type) SIO_NO_STREAM_ID;
    goto FAILURE;
  }
  if( is_rx_stream( open_ptr->stream_type ))
  {
    return_val = alloc_stream_rx( stream_id, mdd_port, open_ptr );

    if( return_val != SIO_DONE_S )
    {
      return_val = (sio_status_type) SIO_NO_STREAM_ID;
      goto FAILURE;
    }

    rx_allocated = TRUE;
  }

  //  Initialize tx structures.
  if( !create_lock( &mdd_port->tx_data.lock_handle ))
  {
    return_val = (sio_status_type) SIO_NO_STREAM_ID;
    goto FAILURE;
  }
  if( is_tx_stream( open_ptr->stream_type ))
  {

    return_val = alloc_stream_tx( stream_id, mdd_port, open_ptr );

    if( return_val != SIO_DONE_S )
    {
      return_val = (sio_status_type) SIO_NO_STREAM_ID;
      goto FAILURE;
    }

    mdd_port->tx_data.tx_stream_state = TX_STREAM_STATE_OPEN;

    tx_allocated = TRUE;
  }

  //  Open the device.

  if( is_rx_stream( open_ptr->stream_type ))
  {
    open_config.baud_rate               = siors_mdd_to_dal_baud_rate( open_ptr->rx_bitrate );
  }
  else if( is_tx_stream( open_ptr->stream_type ))
  {
    open_config.baud_rate               = siors_mdd_to_dal_baud_rate( open_ptr->tx_bitrate );
  }
  open_config.char_format.ParityMode  = UART_NO_PARITY;
  open_config.char_format.BitsPerChar = UART_8_BITS_PER_CHAR;
  open_config.char_format.NumStopBits = UART_1_0_STOP_BITS;
  open_config.rts_control             = RTS_AUTO;
  open_config.cts_control             = CTS_ENABLE;

  dal_result = DalUart_OpenEx( mdd_port->port_data.dal_device_handle, &open_config );
  if( dal_result != DAL_SUCCESS )
  {
    UART_LOG_2( ERROR, "[%d] DalDevice_Open failed: %d.", device_id, dal_result );
    return_val = (sio_status_type) SIO_NO_STREAM_ID;
    goto FAILURE;
  }

  //  Set port state.

  mdd_port->port_data.is_clock_on  = TRUE;
  siors_mdd_stream_port[stream_id] = port_id;

  //  Update open structures.

  mdd_port->stream_data.sio_type_of_stream = open_ptr->stream_type;
  mdd_port->stream_data.open_stream_id     = stream_id;

  //  Just one bitrate = rx.  Get rid of the other one.
  //  DAL does not support different rx, tx bitrates.

  mdd_port->stream_data.sio_current_bitrate = open_ptr->rx_bitrate;

  //  Everything was successful.  Now the device is open.

  mdd_port->stream_data.is_open = TRUE;

  siors_mdd_debug_init( mdd_port );

  DALSYS_SyncLeave( mdd_port->port_data.lock_handle );

  UART_LOG_2( HIGH, "[%d] -siors_mdd_api_open_stream = %d", device_id, SIO_DONE_S );

  return( SIO_DONE_S );

FAILURE:;

  if( rx_allocated )
  {
    dealloc_stream_rx( stream_id, mdd_port );
  }

  if( tx_allocated )
  {
    dealloc_stream_tx( stream_id, mdd_port );
  }

  if( kill_locks )
  {
    destroy_lock( &mdd_port->tx_data.lock_handle );
    destroy_lock( &mdd_port->rx_data.lock_handle );
  }

  if( port_locked )
  {
    DALSYS_SyncLeave( mdd_port->port_data.lock_handle );
  }

  UART_LOG_2( HIGH, "[%d] -siors_mdd_api_open_stream = %d", device_id, return_val );

  return( return_val );
}

/*===========================================================================

FUNCTION                                                EXTERNALIZED FUNCTION
  siors_mdd_api_close_stream

DESCRIPTION
  This function deallocates given stream.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void
siors_mdd_api_close_stream( sio_stream_id_type stream_id, sio_close_func close_func )
{
  sio_port_id_type         port_id;
  uart_device_id_type      device_id;
  sio_mdd_port_type*       mdd_port;
  boolean                  port_locked;
  boolean                  port_already_closed;
  boolean                  tx_locked;
  sio_mdd_tx_data_type*    txd;
  sio_mdd_rx_data_type*    rxd;




  port_already_closed = FALSE;
  port_locked         = FALSE;
  tx_locked           = FALSE;
  mdd_port            = NULL;

  if( !(( 0 <= stream_id ) && ( stream_id < SIO_MAX_STREAM )))
  {
    UART_LOG_1( ERROR, "Invalid stream id %d.", stream_id );
    return;
  }

  port_id   = siors_mdd_stream_port[stream_id];
  device_id = uart_sio_port_id_to_device_id( port_id );
  if( UART_DEVICE_INVALID == device_id )
  {
    UART_LOG_1( ERROR, "Invalid device id %d.", device_id );
    return;
  }

  UART_LOG_1( HIGH, "[%d] +siors_mdd_api_close_stream", device_id );

  //  Close the device.

  mdd_port = &siors_mdd_ports[device_id];
  txd      = &mdd_port->tx_data;
  rxd      = &mdd_port->rx_data;

  //  Force the device closed -- no matter what state it's
  //  in.  Make it so that if it already is closed, closing
  //  it again is a no-op.


  //  Check if device is actually open.

  DALSYS_SyncEnter( mdd_port->port_data.lock_handle );
  port_locked = TRUE;

  if( !mdd_port->stream_data.is_open )
  {
    UART_LOG_2( TRACE, "[%d] Closing closed port %d -- ignoring.", device_id, port_id );
    port_already_closed = TRUE;
    goto DONE;
  }

  //  Check the tx state to see if we should be closing.

  DALSYS_SyncEnter( txd->lock_handle );
  tx_locked = TRUE;

  if( txd->is_open )
  {
    switch( txd->tx_stream_state )
    {
      case TX_STREAM_STATE_OPEN:

        //  Continue.  This state is OK.
        break;

      case TX_STREAM_STATE_CLOSED:

        UART_LOG_1( TRACE, "[%d] Closing closed device - ignoring.", device_id );
        goto DONE;

      case TX_STREAM_STATE_FLUSH_WAIT_FLUSH_TX_FIFO:
      case TX_STREAM_STATE_FLUSH_WAIT_TX_COMPLETE:
      case TX_STREAM_STATE_FLUSH_WAIT_NEXT_TX:

        UART_LOG_1( TRACE, "[%d] Closing device that is flushing.", device_id );
        cancel_flush_tx( mdd_port );
        break;

      default:

        UART_LOG_1( FATAL, "[%d] Unhandled case in switch.", device_id );
        break;
    }
  }

  txd->tx_stream_state = TX_STREAM_STATE_CLOSED;

  DALSYS_SyncLeave( txd->lock_handle );
  tx_locked = FALSE;

  //  Mark stream/port as closed.

  mdd_port->stream_data.is_open = FALSE;

  //  If this is the MAIN UART then clear the siors_dtr_func_ptr.

  if( SIO_PORT_UART_MAIN == port_id )
    siors_mdd_ioctl_disable_dtr_event( mdd_port, NULL );

  //  Release resources for this open instance.

  DalDevice_Close( mdd_port->port_data.dal_device_handle );

  mdd_port->port_data.is_clock_on = FALSE;

  //  Release RX, TX resources AFTER calling DalDevice_Close.

  //  Done with TX resources.

  DALSYS_SyncEnter( txd->lock_handle );
  if( txd->is_open )
  {
    dealloc_stream_tx( stream_id, mdd_port );
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the transmit path", device_id );
  }
  DALSYS_SyncLeave( txd->lock_handle );

  //  Done with RX resources.

  DALSYS_SyncEnter( rxd->lock_handle );
  if( rxd->is_open )
  {
    dealloc_stream_rx( stream_id, mdd_port );
  }
  else
  {
    UART_LOG_1( TRACE, "[%d] This port won't support the recieve path", device_id );
  }
  DALSYS_SyncLeave( rxd->lock_handle );

  timer_clr_64( &mdd_port->port_data.inactivity_timer, T_NONE );

DONE:;

  if( tx_locked )
  {
    DALSYS_SyncLeave( mdd_port->tx_data.lock_handle );
  }

  if( !port_already_closed )
  {
    destroy_lock( &mdd_port->tx_data.lock_handle );
    destroy_lock( &mdd_port->rx_data.lock_handle );
  }

  if( port_locked )
  {
    DALSYS_SyncLeave( mdd_port->port_data.lock_handle );
  }

  if( close_func )
  {
    close_func();
  }

  UART_LOG_1( HIGH, "[%d] -siors_mdd_api_close_stream", device_id );
}

/*===========================================================================

FUNCTION                                                EXTERNALIZED FUNCTION
  siors_mdd_api_dev_transmit

DESCRIPTION
  This function transmits over MSM's RS-232 UART.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void
siors_mdd_api_dev_transmit ( sio_stream_id_type stream_id, sio_port_id_type port_id, dsm_item_type* tx_ptr )
{
  sio_mdd_tx_data_type*            txd;
  sio_mdd_port_type*               mdd_port;
  uart_device_id_type              device_id;
  boolean                          bad_state;
  boolean                          port_locked;
  boolean                          tx_locked;
  boolean                          free_packet;






  port_locked = FALSE;
  tx_locked   = FALSE;
  bad_state   = FALSE;
  free_packet = FALSE;
  mdd_port    = NULL;

  //  Check to see if the port is valid and assign
  //  interval variables for easy access.

  device_id = uart_sio_port_id_to_device_id( port_id );
  if( UART_DEVICE_INVALID == device_id )
  {
    UART_LOG_1( ERROR, "Invalid port id %d.", port_id );
    goto DONE;
  }

  mdd_port = &siors_mdd_ports[device_id];
  txd      = &mdd_port->tx_data;

  UART_LOG_1( TRACE, "[%d] +siors_mdd_api_dev_transmit", device_id );

  siors_mdd_on_activity( mdd_port );

  DALSYS_SyncEnter( mdd_port->port_data.lock_handle );
  port_locked = TRUE;

  if( !mdd_port->port_data.is_clock_on )
  {
    UART_LOG_1( TRACE, "[%d] Clock is turned off.", device_id );
    free_packet = TRUE;
    goto DONE;
  }

  DALSYS_SyncEnter( txd->lock_handle );
  tx_locked = TRUE;

  //  Always check state after acquiring lock.

  if( !txd->is_open )
  {
    UART_LOG_1( TRACE, "[%d] tx on device not open for tx.", device_id );
    free_packet = TRUE;
    goto DONE;
  }

  //  Check state.

  switch( txd->tx_stream_state )
  {
    case TX_STREAM_STATE_OPEN:

      //  OK.
      break;

    case TX_STREAM_STATE_FLUSH_WAIT_NEXT_TX:

      //  Got the next tx that we were waiting for as part of
      //  the delayed flush.  Bump the state.

      txd->tx_stream_state = TX_STREAM_STATE_FLUSH_WAIT_FLUSH_TX_FIFO;
      break;

    case TX_STREAM_STATE_FLUSH_WAIT_FLUSH_TX_FIFO:
    case TX_STREAM_STATE_FLUSH_WAIT_TX_COMPLETE:

      //  Flushed, then transmitted before flush callback.  Shouldn't
      //  do that.  Let's ignore this request.

      UART_LOG_1( TRACE, "[%d] tx while flushing.", device_id );
      bad_state = TRUE;
      break;

    case TX_STREAM_STATE_CLOSED:

      UART_LOG_1( TRACE, "[%d] tx on closed device.", device_id );
      bad_state = TRUE;
      break;

    default:

      UART_LOG_1( FATAL, "[%d] Unhandled case in switch.", device_id );
      break;
  }

  if( bad_state )
  {
    free_packet = TRUE;
    goto DONE;
  }

  //  Queue the item.  If we're going to send it now,
  //  we'll immediately take it back off the queue.

  dsm_enqueue( txd->sio_tx_watermark_queue_ptr, &tx_ptr );

  if( can_transmit_now( mdd_port ))
  {
    //  Transmit packet now.

    UART_LOG_1( TRACE, "[%d] Transmitting data now.", device_id );

    do_transmit( mdd_port );
  }
  else
  {
    //  Need to enqueue the given packet.  Some packet is already in the process
    //  of being transmitted or the link is flow controlled.

    UART_LOG_1( TRACE, "[%d] Enqueue packet.", device_id );
  }

DONE:;

  if( free_packet )
  {
    dsm_free_packet( &tx_ptr );
  }

  if( tx_locked )
  {
    DALSYS_SyncLeave( mdd_port->tx_data.lock_handle );
  }

  if( port_locked )
  {
    DALSYS_SyncLeave( mdd_port->port_data.lock_handle );
  }

  UART_LOG_1( TRACE, "[%d] -siors_mdd_api_dev_transmit", device_id );
}

/*===========================================================================

FUNCTION                                                EXTERNALIZED FUNCTION
  siors_mdd_api_flush_tx

DESCRIPTION
  This function flushes the TX.

DEPENDENCIES
 User should not queue more data until after the registered callback
 is executed.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void
siors_mdd_api_flush_tx( sio_stream_id_type stream_id, sio_port_id_type port_id, sio_flush_func flush_func )
{
  sio_mdd_port_type*       mdd_port;
  uart_device_id_type      device_id;
  boolean                  port_locked;
  boolean                  tx_locked;




  port_locked = FALSE;
  tx_locked   = FALSE;
  mdd_port    = NULL;

  if( NULL == flush_func )
  {
    UART_LOG( ERROR, "NULL flush function pointer.");
    return;
  }

  device_id = uart_sio_port_id_to_device_id( port_id );
  if( UART_DEVICE_INVALID == device_id )
  {
    UART_LOG_1( ERROR, "Invalid port id %d.", port_id );
    return;
  }

  UART_LOG_1( TRACE, "[%d] +siors_mdd_api_flush_tx", device_id );

  mdd_port = &siors_mdd_ports[device_id];

  siors_mdd_on_activity( mdd_port );

  DALSYS_SyncEnter( mdd_port->port_data.lock_handle );
  port_locked = TRUE;

  if( !mdd_port->stream_data.is_open )
  {
    UART_LOG_2( TRACE, "[%d] Flushing closed port %d -- ignoring.", device_id, port_id );
    goto DONE;
  }

  if( !mdd_port->port_data.is_clock_on )
  {
    UART_LOG_1( TRACE, "[%d] Clock is turned off.", device_id );
    goto DONE;
  }

  //  Lock just the TX side.

  DALSYS_SyncLeave( mdd_port->port_data.lock_handle );
  port_locked = FALSE;

  DALSYS_SyncEnter( mdd_port->tx_data.lock_handle );
  tx_locked = TRUE;

  if( !mdd_port->tx_data.is_open )
  {
    UART_LOG_1( TRACE, "[%d] TX state changed.", device_id );
    goto DONE;
  }

  //  siors_mdd_flush_tx assumes lock is held when it's
  //  called and releases the lock.  Why?  It releases the
  //  lock before invoking callback.  We don't want to be
  //  holding any locks when invoking the callback.  It
  //  may call UART functions.

  siors_mdd_flush_tx( mdd_port, TRUE, flush_func );
  tx_locked = FALSE;

DONE:;

  if( tx_locked )
  {
    DALSYS_SyncLeave( mdd_port->tx_data.lock_handle );
  }

  if( port_locked )
  {
    DALSYS_SyncLeave( mdd_port->port_data.lock_handle );
  }

  UART_LOG_1( TRACE, "[%d] -siors_mdd_api_flush_tx", device_id );
}

/*===========================================================================

FUNCTION SIO_RS232_PURGE_TX

DESCRIPTION
  This function cancels the active transfer and frees up the queued DSM items.

DEPENDENCIES
  Should not be called during flush.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

sio_status_type
siors_mdd_api_purge_tx( sio_stream_id_type stream_id, sio_port_id_type port_id )
{
  sio_mdd_port_type*       mdd_port;
  sio_mdd_tx_data_type*    txd;
  uart_device_id_type      device_id;
  boolean                  port_locked;
  boolean                  tx_locked;
  sio_status_type          return_value;




  port_locked = FALSE;
  tx_locked   = FALSE;
  mdd_port    = NULL;


  device_id = uart_sio_port_id_to_device_id( port_id );
  if( UART_DEVICE_INVALID == device_id )
  {
    UART_LOG_1( ERROR, "Invalid port id %d.", port_id );
    return SIO_BADP_S;
  }

  UART_LOG_1( TRACE, "[%d] +siors_mdd_api_purge_tx", device_id );

  mdd_port = &siors_mdd_ports[device_id];
  txd = &mdd_port->tx_data;

  siors_mdd_on_activity( mdd_port );

  DALSYS_SyncEnter( mdd_port->port_data.lock_handle );
  port_locked = TRUE;

  if( !mdd_port->stream_data.is_open )
  {
    UART_LOG_2( TRACE, "[%d] Purging closed port %d -- ignoring.", device_id, port_id );
    return_value = SIO_CLOSED_PORT_S;
    goto DONE;
  }

  if( !mdd_port->port_data.is_clock_on )
  {
    UART_LOG_1( TRACE, "[%d] Clock is turned off.", device_id );
    return_value = SIO_PORT_POWERED_DOWN_S;
    goto DONE;
  }

  //  Lock just the TX side.

  DALSYS_SyncLeave( mdd_port->port_data.lock_handle );
  port_locked = FALSE;

  DALSYS_SyncEnter( txd->lock_handle );
  tx_locked = TRUE;

  if( !txd->is_open )
  {
    UART_LOG_1( TRACE, "[%d] TX state changed.", device_id );
    return_value = SIO_TX_NOT_OPEN_S;
    goto DONE;
  }

  // Free the current item ptr.
  dsm_free_packet(&txd->sio_tx_current_wm_item_ptr);

  // Free all the items in the watermark queue.
  dsm_empty_queue( txd->sio_tx_watermark_queue_ptr );
  txd->sio_tx_current_wm_item_ptr = NULL;
  txd->sio_bytes_to_tx            = 0;

  DalUart_PurgeTx(mdd_port->port_data.dal_device_handle);
  txd->tx_in_progress = FALSE;
  txd->tx_stream_state = TX_STREAM_STATE_OPEN;
  return_value = SIO_DONE_S;

DONE:;

  if( tx_locked )
  {
    DALSYS_SyncLeave( txd->lock_handle );
  }

  if( port_locked )
  {
    DALSYS_SyncLeave( mdd_port->port_data.lock_handle );
  }

  UART_LOG_1( TRACE, "[%d] -siors_mdd_api_purge_tx", device_id );
  return return_value;
}

/*===========================================================================

FUNCTION SIO_MDD_API_DISABLE_DEVICE                  EXTERNALIZED FUNCTION

DESCRIPTION
  This routine disables given device, including turning off clock to save
  power.  This is different from UART powerdown feature which attempts to
  save power with minimal loss of data.  Instead, this function is meant
  for customers who may not want to use both UARTs.

  Effects of this function may be reversed by calling sio_rs232_dev_init.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void
siors_mdd_api_disable_device( sio_port_id_type port_id )
{
  sio_mdd_port_type*       mdd_port;
  uart_device_id_type      device_id;
  boolean                  port_locked;




  port_locked = FALSE;
  mdd_port    = NULL;

  device_id = uart_sio_port_id_to_device_id( port_id );
  if( UART_DEVICE_INVALID == device_id )
  {
    UART_LOG_1( ERROR, "Invalid port id %d.", port_id );
    goto DONE;
  }

  UART_LOG_1( HIGH, "[%d] +siors_mdd_api_disable_device", device_id );

  mdd_port = &siors_mdd_ports[device_id];

  DALSYS_SyncEnter( mdd_port->port_data.lock_handle );
  port_locked = TRUE;

  if( !mdd_port->stream_data.is_open )
  {
    UART_LOG_2( TRACE, "[%d] Disabling closed port %d -- ignoring.", device_id, port_id );
    goto DONE;
  }

  if( !mdd_port->port_data.is_clock_on )
  {
    UART_LOG_1( TRACE, "[%d] Clock is turned off.", device_id );
    goto DONE;
  }

  DalDevice_Close( mdd_port->port_data.dal_device_handle );

  mdd_port->port_data.is_clock_on = FALSE;

DONE:;

  if( port_locked )
  {
    DALSYS_SyncLeave( mdd_port->port_data.lock_handle );
  }

  UART_LOG_1( HIGH, "[%d] -siors_mdd_api_disable_device", device_id );
}

//-----------------------------------------------------------------------------------
//  End of file
//-----------------------------------------------------------------------------------

#endif //}


