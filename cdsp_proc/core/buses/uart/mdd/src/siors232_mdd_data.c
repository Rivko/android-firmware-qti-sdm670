/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             S I O R S 2 3 2  M D D  D A T A
                             
            SIO Media Dependent Driver layer for RS-232 communication

GENERAL DESCRIPTION

  This module contain the code shared by different serial driver.  It calls
  seperate hardware dependent driver code to access the actual UART hardware.
  

EXTERNALIZED FUNCTIONS
    
INITIALIZATION AND SEQUENCING REQUIREMENTS

  sio_rs232_dev_init() must be called prior to any of the other functions of
  this module.

Copyright (c) 1999-2000, 2007-2008, 2012-2013 Qualcomm Technologies Incorporated.
All Rights Reserved.  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $Header: //components/rel/core.qdsp6/2.1.c4/buses/uart/mdd/src/siors232_mdd_data.c#1 $

when         who       what, where, why
-----------  --------  ------------------------------------------------------
21-Jul-2008  cmihalik  Initial revision.

===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "siors232_mdd_defs.h"
#include "siors232_mdd_data.h"
#include "uart_log.h"

//  If there is is at least one UART.
#if( defined(FEATURE_FIRST_UART)  || \
     defined(FEATURE_SECOND_UART) || \
     defined(FEATURE_THIRD_UART))              //{

/*===========================================================================

===========================================================================*/

#define UART_TRACE_BUFFER_FILE_ID             2

/*===========================================================================
                       FUNCTIONs, ISRs and MACROs
===========================================================================*/

static void      siors_mdd_ad_setup_discard_recovery( sio_mdd_port_type* );

/*===========================================================================

FUNCTION SIORS_MDD_ESC_SEQ_GUARDTIME_ISR

DESCRIPTION
  This function will monitor the incoming serial RX stream to determine if
  the Escape Sequence is being received.  At the first guard-time, before
  the Escape characters, the guardtime is long enough that the Modem's fifo
  will be completely drained.  So, the first character that will be seen in
  the fifo will be a Escape character.  When this routine is invoke after
  the last Escape character, the fifo should be completely empty.

  This routine is also invoked after the first and second Escape characters.
  This is because the escape sequence should be presented to the modem in a
  finite amount of time and because the guartime after the first or the
  second escape character may be the beginning of the true escape sequence.

DEPENDENCIES
  This function must only be called while in RAWDATA or PKT modes, and must
  only be invoked from a Clock Services callback function.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void 
siors_mdd_esc_seq_guardtime_isr( int32 timeMs, timer_cb_data_type data )
{
  int                          device_id;
  sio_mdd_port_type*           mdd_port;
  sio_mdd_rx_data_type*        rxd;





  mdd_port = (sio_mdd_port_type*) data;
  rxd      = &mdd_port->rx_data;
  device_id = mdd_port->port_data.device_id;

  UART_LOG_1( TRACE, "[%d] +siors_mdd_escape_sequence_guardtime_isr", device_id );

  //  Use the escape sequence state to begin checking the progress 
  //  of the escape sequence.

  switch ( rxd->sio_esc_seq_state )
  {
    case SIO_ESC_SEQ_NULL:

      rxd->sio_esc_seq_state = SIO_ESC_SEQ_1ST_GUARDTIME;
      break;

    case SIO_ESC_SEQ_1ST_CHAR:
    case SIO_ESC_SEQ_2ND_CHAR:

      //  Guard time expired after the first character, but before 
      //  receiving the second character.  (Or after receiving the 
      //  second character, but before receiving the third character.)
      //  Since each character of the escape sequence must be received
      //  within the guard time, this is not an escape sequence.
      //
      //  Reset the state machine.  The guard time that just expired 
      //  could be the beginning of a new escape sequence.  The correct 
      //  state here is SIO_ESC_SEQ_1ST_GUARDTIME, not SIO_ESC_SEQ_NULL.  

      rxd->sio_esc_seq_state = SIO_ESC_SEQ_1ST_GUARDTIME;
      break;

    case SIO_ESC_SEQ_3RD_CHAR:

      //  Post-escape bytes guardtime has expired.  Escape sequence 
      //  received. Signal task.
      
      if( rxd->sio_escape_code_func_ptr )
        rxd->sio_escape_code_func_ptr();

      //  Reset the state machine.
      //
      //  The guard time that just expired could be the beginning of a 
      //  new escape sequence.  The correct state here is 
      //  SIO_ESC_SEQ_1ST_GUARDTIME, not SIO_ESC_SEQ_NULL.

      rxd->sio_esc_seq_state = SIO_ESC_SEQ_1ST_GUARDTIME;
      break;

    default:
      break;
  } 

  UART_LOG_1( TRACE, "[%d] -siors_mdd_escape_sequence_guardtime_isr", device_id );
}

/*===========================================================================

FUNCTION SIORS_MDD_SCAN_RX_DATA_RAWDATA_MODE_ISR

DESCRIPTION
  This is the RX ISR for RawData mode with hardware flow control and escape 
  code detection is required.  Escape code detection is needed if guard-time 
  has expired, which means that character by character comparision is required.

DEPENDENCIES
  Should only be called from siors_rx_isr.  Flow control method should be
  hardware flow control.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void
siors_mdd_scan_data_rawdata_mode_rx_isr( sio_mdd_port_type* mdd_port, dsm_item_type* dsm_item_ptr )
{
  int                         device_id = mdd_port->port_data.device_id;
  sio_mdd_rx_data_type*       rxd;
  uint8*                      cur_ptr;
  uint8*                      last_ptr;         
  uint8                       rx_char; 




  UART_LOG_1( TRACE, "[%d] +siors_mdd_scan_data_rawdata_mode_rx_isr", device_id );

  //  Check input stream for an escape sequence.  The escape sequence
  //  is 1 second of no activity (guard time), follwed by the sequence
  //  of characters "+++", followed by 1 second of no activity.  Each 
  //  '+' of the escape sequence must be received within 1 ms (guard
  //  time) of the the previous escape sequence character.

  cur_ptr  = dsm_item_ptr->data_ptr;
  last_ptr = dsm_item_ptr->data_ptr + dsm_item_ptr->used;
  rxd      = &mdd_port->rx_data;

  while( cur_ptr < last_ptr )
  {
    rx_char = *cur_ptr++;

    switch( rxd->sio_esc_seq_state )
    {
      case SIO_ESC_SEQ_NULL:

        //  Done.  We'll schedule the guard time check and exit.
        break;

      case SIO_ESC_SEQ_1ST_GUARDTIME:
      case SIO_ESC_SEQ_1ST_CHAR:
      case SIO_ESC_SEQ_2ND_CHAR:

        //  Expecting to get an SIO_ESC_CHAR here.  If we get one,
        //  keep going.  Otherwise, go to NULL state.

        if( SIO_ESC_CHAR == rx_char )
          rxd->sio_esc_seq_state++;
        else
          rxd->sio_esc_seq_state = SIO_ESC_SEQ_NULL;
        break;

      case SIO_ESC_SEQ_3RD_CHAR:

        //  This means that a third escape character was read and then
        //  another character (escape character or not) was read before
        //  the guard time.  Otherwise, if the rx fifo is empty, an 
        //  RXSTALE interrupt would not have happened.

        rxd->sio_esc_seq_state = SIO_ESC_SEQ_NULL;
        break;

      default:
        break;
    }
      
    if( SIO_ESC_SEQ_NULL == rxd->sio_esc_seq_state )
      break;
  }

  //  Schedule a guard time check (if needed).

  switch( rxd->sio_esc_seq_state )
  {
    case SIO_ESC_SEQ_NULL:

      //  Schedule a guard time check ISR (for '+++' escape code detection).
      //  Look for guard time before first character of escape sequence.

      timer_reg( &rxd->sio_esc_seq_cb_item,
                 siors_mdd_esc_seq_guardtime_isr,
                 (timer_cb_data_type) mdd_port,
                 SIO_ESC_GUARD_TIME,
                 0 );
      break;

    case SIO_ESC_SEQ_1ST_CHAR:
    case SIO_ESC_SEQ_2ND_CHAR:

      //  The guard time is also the maximum time that can elapse between 
      //  each successive escape character.  

      timer_reg( &rxd->sio_esc_seq_cb_item,
                 siors_mdd_esc_seq_guardtime_isr,
                 (timer_cb_data_type) mdd_port,
                 SIO_ESC_GUARD_TIME,
                 0 );
      break;

    case SIO_ESC_SEQ_3RD_CHAR:

      //  Schedule a guard time check for guard time following escape
      //  sequence.

      timer_reg( &rxd->sio_esc_seq_cb_item,
                 siors_mdd_esc_seq_guardtime_isr,
                 (timer_cb_data_type) mdd_port,
                 SIO_ESC_GUARD_TIME,
                 0 );
      break;

    default:
      break;
  }

  UART_LOG_1( TRACE, "[%d] -siors_mdd_scan_data_rawdata_mode_rx_isr", device_id );
}

/*===========================================================================

FUNCTION SIORS_MDD_SETUP_UART_AUTODETECT

DESCRIPTION
   Setup the UART for Autodetect processing. This function is called for 
   every new AT cmd line. 

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void 
siors_mdd_setup_uart_autodetect( sio_mdd_port_type* mdd_port )
{
#ifdef SIORS_HAS_AUTODETECT //{

  //  Set UART interrupt mask conditions for Autodetect mode. 
  //  Leave bit rate as already set. 

  //  Set Autodetect mode pause variables to default states then register 
  //  clock callback to set check for Pre-pause interval (delay before bytes 
  //  arrive).

  mdd_port->rx_data.adv.packetpause_occurred = FALSE;
  mdd_port->rx_data.adv.state                = SIO_AD_INIT;

#endif //}
} 

/*===========================================================================

FUNCTION SIORS_MDD_AD_DISCARD_RECOVERY_ISR

DESCRIPTION
  This function is a callback ISR that will determine if the data services 
  serial port has stopped receiving bytes and if so then this function will
  re-initialize the port for data services Autodetect mode operation. If the
  port is still receiving bytes then this callback will register another
  invocation to check again later.

DEPENDENCIES
  - This function must be run in ISR context to insure no context switches
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void 
siors_mdd_ad_discard_recovery_isr( int32 time_ms, timer_cb_data_type cb_data )
{
  sio_mdd_port_type*           mdd_port = (sio_mdd_port_type*) cb_data;
  sio_mdd_rx_data_type*        rxd = &mdd_port->rx_data;
  int                          device_id = mdd_port->port_data.device_id;



  UART_LOG_1( TRACE, "[%d] +siors_mdd_ad_discard_recovery_isr", device_id );

  DALSYS_SyncEnter( rxd->lock_handle );

  if( !rxd->is_open )
  {
    //  Do nothing.  Port has changed underneath us.
  }
  else if( 0 == rxd->adv.rx_cnt )
  {
    //  The line is now Pausing. Setup for Autodetect mode processing

    UART_LOG_1( TRACE, "[%d] Ending SIO Discard", device_id );
    siors_mdd_setup_uart_autodetect(mdd_port);
  }
  else 
  {
    //  The line is still active. Store the saved count again and
    //  re-register this callback to check again after the pause
    //  interval expires

    siors_mdd_ad_setup_discard_recovery( mdd_port );
  }

  DALSYS_SyncLeave( rxd->lock_handle );

  UART_LOG_1( TRACE, "[%d] -siors_mdd_ad_discard_recovery_isr", device_id );
} 

/*===========================================================================

FUNCTION SIORS_MDD_AD_SETUP_DISCARD_RECOVERY

DESCRIPTION
  This function will setup for the Autodetect mode RX ISR to recover from 
  Discard mode. A callback will be setup to monitor the RX line. Once the RX 
  line goes stale (no bytes received) for the the pre-pause interval then the
  callback will prepare the serial port for Autodetect processing.
  
  Autodetect mode is restricted to the main UART channel.

DEPENDENCIES
  This function must be run in ISR context to insure no context switches
  
RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void 
siors_mdd_ad_setup_discard_recovery( sio_mdd_port_type* mdd_port )
{
  int                          device_id = mdd_port->port_data.device_id;


  UART_LOG_1( TRACE, "[%d] +siors_mdd_ad_setup_discard_recovery", device_id );

  //  If for some reason we make it here, but the stream is not in 
  //  AUTODETECT throw an error message and return out. 

  if( mdd_port->rx_data.sio_current_stream_mode != SIO_DS_AUTODETECT_MODE )
  {
    UART_LOG_1( TRACE, "[%d] AD discard while not in AD mode", device_id );
    return;
  }

  //  Save away the current RX count for the callback to check later, set
  //  RXLEV up high to lighten CPU load during Discarding (but low enough 
  //  so that RX interrupts will occur at least once between each discard
  //  callback, then register callback that will re-initialize Autodetect 
  //  mode once the line pauses.

  if( mdd_port->rx_data.adv.state != SIO_AD_DISCARD )
  {
    UART_LOG_1( TRACE, "[%d] SIO Discard Active", device_id );
  }

  mdd_port->rx_data.adv.state  = SIO_AD_DISCARD;
  mdd_port->rx_data.adv.rx_cnt = 0;

  timer_reg( &mdd_port->rx_data.adv.pausecheck_cb_item, 
             siors_mdd_ad_discard_recovery_isr,
             (timer_cb_data_type) mdd_port,
             DISCARD_CHECK_INTERVAL, 
             0 );

  UART_LOG_1( TRACE, "[%d] -siors_mdd_ad_setup_discard_recovery", device_id );
} 

/*===========================================================================

FUNCTION SIORS_MDD_AD_PACKETPAUSE_CHECK_ISR

DESCRIPTION
  This function is a callback ISR that set a semaphore that indicates that
  no bytes have been received since the callback was registered. This is used
  for Packet detection in Autodetect mode.
  
  Autodetect mode is restricted to the main UART channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void 
siors_mdd_ad_packetpause_check_isr( int32 time_ms, timer_cb_data_type cb_data )
{
  sio_mdd_port_type* mdd_port = (sio_mdd_port_type *)cb_data;
  //  Set Semaphore indicating that the Packet Pause interval has elapsed

  mdd_port->rx_data.adv.packetpause_occurred = TRUE;
} 

/*===========================================================================

FUNCTION SIORS_MDD_NEW_VALID_PACKET

DESCRIPTION
  This procedure will signal packet detection to the registered task and
  setup the associated Autodetect mode UART for Packet data flow operation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

static void 
siors_mdd_new_valid_packet( sio_mdd_port_type* mdd_port )
{
  //  Valid packet incoming. Set the UART for Packet mode processing, and signal 
  //  task, if registered.  

  mdd_port->rx_data.adv.state = SIO_AD_INIT;   /* reset for next time */
  
  if( mdd_port->rx_data.sio_packet_func_ptr != NULL)
    mdd_port->rx_data.sio_packet_func_ptr();
} 

/*===========================================================================

FUNCTION SIORS_MDD_SCAN_RXDATA_AUTODETECT_MODE_ISR

DESCRIPTION

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

void
siors_mdd_scan_data_autodetect_mode_rx_isr( sio_mdd_port_type* mdd_port, dsm_item_type* dsm_item_ptr )
{
  int                             device_id = mdd_port->port_data.device_id;
  uint8*                          cur_ptr;
  uint8*                          last_ptr;         
  uint8                           rx_char; 
  uint32                          num_chars;
  sio_mdd_autodetect_var_type*    padv;





  UART_LOG_1( TRACE, "[%d] +siors_mdd_scan_data_autodetect_mode_rx_isr", device_id );

  dsm_item_ptr->kind = DSM_PS_RAW_APP_PAYLOAD;

  padv = &mdd_port->rx_data.adv;

  //  Examine input for AT commands.

  cur_ptr  = dsm_item_ptr->data_ptr;
  last_ptr = dsm_item_ptr->data_ptr + dsm_item_ptr->used;

  while( cur_ptr < last_ptr )
  {
    rx_char = *cur_ptr;

    padv->rx_cnt++;

    switch( padv->state )
    {
      //  No characters received yet.
         
      case SIO_AD_INIT:
        {
          switch( rx_char )
          {
            case 'a':
            case 'A':

              padv->state = SIO_AD_GOT_A;
              break;

            case '\r':
            case '\n':

              //  Ignore \r, \n between AT commands.
              break;

            case SIO_ASYNC_FLAG: 

              //  xxx:  Needed?  If you're in AD mode, why would you be 
              //  getting PPP packets?  Not sure if this is handled correctly
              //  in the old code (which is copied here).  The old code does
              //  not process the DSM item in this case.  That doesn't sound
              //  right.  I wonder if we ever hit this case.

              //  Received an SIO_ASYNC_FLAG after the pre-pause interval,
              //  which means that this could be the beginning of a packet.

              //  xxx:  See comment above.  Put log message here if we ever
              //  hit this case.

              dsm_item_ptr->used = 0;

              num_chars = last_ptr - cur_ptr;
              if( num_chars > 0 )
              {
                //  There is at least 1 more byte in the RX fifo, 
                //  indicating that a valid start of packet has been rx'd.

                siors_mdd_new_valid_packet( mdd_port );
              }
              else
              {
                //  Flag byte received, but no more bytes now available.
                //  Register the 2 ms packet detection clock callback 
                //  that will set a timeout semaphore if the delay period 
                //  is reached.
              
                padv->state                = SIO_AD_GOT_ASYNC_FLAG;
                padv->packetpause_occurred = FALSE;

                timer_reg ( &padv->pausecheck_cb_item,
                            siors_mdd_ad_packetpause_check_isr,
                            (timer_cb_data_type) mdd_port,
                            SIO_PACKET_DETECTION_TIMEOUT,
                            0 );

              }
              break;

            default:

              padv->state = SIO_AD_DISCARD;
              break;
          }
        }
        break;

      //  Already have an 'a' or 'A', the beginning of an AT command.

      case SIO_AD_GOT_A:
        {
          switch( rx_char )
          {
            case 'a':
            case 'A':

              //  Do nothing.  Multiple A's OK.
              break;

            case 't':
            case 'T':
            case '/':

              padv->state = SIO_AD_GOT_AT;
              break;

            default:

              padv->state = SIO_AD_DISCARD;
              break;
          }
        }
        break;

      //  Got an AT plus at least one more character.  Call it an AT command.
         
      case SIO_AD_GOT_AT:

        padv->state = SIO_AD_GOT_AT_COMMAND;
        break;

      //  Got UART_PPP_ASYNC_FLAG.  This should only happen if the UART_PPP_ASYNC_FLAG
      //  was the last character in the buffer.  (If it wasn't, we could have
      //  processed it already.)

      case SIO_AD_GOT_ASYNC_FLAG:
        {
          //  xxx:  In old code, if the DSM item contains a packet it's
          //  not processed.  Is that right?

          dsm_item_ptr->used = 0;

          if( FALSE == padv->packetpause_occurred )
          {
            //  packet timeout timer didn't expire.
            timer_clr_64( &padv->packetpause_cb_item, T_NONE );
            siors_mdd_new_valid_packet( mdd_port );
          }
          else
          {
            padv->state = SIO_AD_DISCARD;
          }
        }
        break;

      default:
        break;
    }

    if(( padv->state == SIO_AD_DISCARD ) ||
       ( padv->state == SIO_AD_GOT_AT_COMMAND ))
    {
      break;
    }

    cur_ptr++;
  }

  //  Discard all items in current buffer.

  if( SIO_AD_DISCARD == padv->state )
  {
    dsm_item_ptr->used = 0;
    siors_mdd_ad_setup_discard_recovery( mdd_port );
  }

  UART_LOG_1( TRACE, "[%d] -siors_mdd_scan_data_autodetect_mode_rx_isr", device_id );
}

#endif //} 

