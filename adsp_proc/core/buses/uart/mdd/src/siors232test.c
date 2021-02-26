/*===========================================================================

                       S I O R S 2 3 2 T E S T  . C
DESCRIPTION
  This file contains the functionality for echo-packet testing
  
   
  
Copyright (c) 2006-2012 by Qualcomm Technologies Incorporated.  
All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/06/08   rh      Correct the usage of diagdsm.h
08/07/07   rh      Added flow control base on watermark queue
02/15/07   rh      Move the UART logging feature here
05/01/06   rh      File creation - Adapted from USBTEST.C
09/27/06   rh      Add linear and AA/55 RX only test method
                               
===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/



#include "sio.h"
#include "comdef.h"
#include "siors232test.h"
#include "dsm.h"
#include "diagdsm.h"
#include "uart_log.h"

#define UART_TRACE_BUFFER_FILE_ID     10

//  Make the init timeout longer to give everything a chance to
//  start up.

#define TX_SOURCE_INIT_TIMEOUT        200
#define TX_SOURCE_TIMEOUT_INTERVAL    10
#define TX_SOURCE_DATA_PER_SLICE      128

/*===========================================================================
  Info regarding Fusion UART loopback test:
    - basic connectivity test that can be run on Fusion targets to loop
      packets between Modem and MDM UARTs.
    - To enable test:
       1) disable current user (MUX, APR, etc) of UARTs in build.
       2) modify UART SConscript file to include this file in build.
       3) add #define SIORS232TEST_USE_FUSION_LOOPBACK to this file.
       4) if using other than primary UART (7x30 uses UART2DM):
          add appropriate define to this file (ie. SIORS232TEST_USE_AUX_UART).
       5) Add call to siors232test_echo_init() from a rex task:
          - at end of tmc_libraries_init_after_tasks_start(), or
            in any task (only call function once).
===========================================================================*/
// #define SIORS232TEST_USE_FUSION_LOOPBACK
// #define SIORS232TEST_USE_AUX_UART
// #define SIORS232TEST_USE_THIRD_UART

/*===========================================================================
  
        PART 1 - SIO ECHO TEST FUNCTIONS
   
===========================================================================*/


static void siors232test_process_switch(void);

static void siors232test_sio_process_rx_packet(dsm_item_type **dsm_item_ptr);
static void siors232test_transmit_fixed_data(sio_stream_id_type lb_stream_id, char ch, int size);

#ifdef FEATURE_DSM_WM_CB
   static void siors232test_lowater_cb(struct dsm_watermark_type_s *wm_ptr, void *v_ptr);
   static void siors232test_hiwater_cb(struct dsm_watermark_type_s *wm_ptr, void *v_ptr);
#else /* FEATURE_DSM_WM_CB */
   static void siors232test_lowater_cb(void);
   static void siors232test_hiwater_cb(void);
#endif /* FEATURE_DSM_WM_CB */

static void uart_test_tx_source_on_rx( dsm_item_type** ppDsmItem );

static dsm_watermark_type siors232test_wmq;

static q_type siors232test_echo_q;

static sio_open_type siors232test_echo_open_struct =
{
    SIO_NO_STREAM_ID,                         /* Stream ID Type.  Filled in
                                                 by SIO for internal use.    */
    SIO_STREAM_RXTX,                          /* Type of stream being opened.
                                                 Filled in by SIO for 
                                                 internal use only.          */
    SIO_DS_PKT_MODE,                          /* Stream Mode Type            */
    NULL,                                     /* Received Data Queue         */
    &siors232test_wmq,                        /* Transmit Data Queue         */
#ifdef SIORS232TEST_USE_FUSION_LOOPBACK
    SIO_BITRATE_4000000,                      /* Bit-rate for reception      */
    SIO_BITRATE_4000000,                      /* Bit-rate for transmission   */
#else
    SIO_BITRATE_115200,                       /* Bit-rate for reception      */
    SIO_BITRATE_115200,                       /* Bit-rate for transmission   */
#endif
    SIO_PORT_UART_MAIN,                       /* Port which needs to be used */
    FALSE,                                    /* Is tail character used ?    */
    '\0',                                     /* If tail character is used, 
                                                 this is the tail character. */
    siors232test_sio_process_rx_packet,       /* If non-NULL, this function 
                                                 will be called for each 
                                                 packet which is received.   */
    SIO_CTSRFR_AUTO_FCTL,                     /* TX flow control method      */
    SIO_CTSRFR_AUTO_FCTL                      /* RX flow control method      */
};

typedef enum
{
    STATE_RESET,
    STATE_INIT,
    STATE_DONE,
    STATE_CLOSE_INIT,
    STATE_CLOSE_STAGE2,
    STATE_NUMBER_OF
}siors232test_process_state;

typedef enum
{
    SIORS232TEST_LOOPBACK,                      /* Standard loopback test     */
    SIORS232TEST_LINEAR,                        /* Counting from 0x00 to 0xff */
    SIORS232TEST_AA55,                          /* Send 0xAA follow by 0x55   */
    SIORS232TEST_RX_ONLY,                       /* Just count rx bytes        */
    SIORS232TEST_FUSION_LOOPBACK,               /* New loopback test          */
    SIORS232TEST_TX_SOURCE                      /* Transmit continuously      */
}siors232test_test_type;

typedef struct 
{
     siors232test_test_type     test_type;
     uint8                      next_byte;
     uint8                      next_rx_byte;
     uint32                     error_count;
     uint32                     tx_length;
     uint32                     tx_byte_count;
     uint32                     rx_byte_count;
     uint32                     current_rx_count;
     boolean                    use_interrupt_context;
     uint32                     time_slice;
}siors232test_test_state_type;

static siors232test_process_state        current_state = STATE_DONE;
static siors232test_test_state_type      rstest_test_type;

void siors232test_command_dpc(unsigned long cmd)
{
    siors232test_command( (char) cmd);
}

/*===========================================================================

FUNCTION siors232test_process

DESCRIPTION
  Process the state machine changes which initialize the loop back operation

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void siors232test_process_switch ()
{ 
    //static rdm_service_enum_type old_service;

    switch (current_state)
    {
    case STATE_RESET:
        /* Setup port setting variable                                     */
        siors232test_wmq.current_cnt = 0;
        siors232test_wmq.dont_exceed_cnt = 10000;
        siors232test_wmq.hi_watermark = 8000;
        siors232test_wmq.lo_watermark = 3000;
        siors232test_wmq.hiwater_func_ptr = (wm_cb_type) siors232test_hiwater_cb;
        siors232test_wmq.lowater_func_ptr = (wm_cb_type) siors232test_lowater_cb;

        /* Setup queue */
        (void) q_init( &siors232test_echo_q );
        siors232test_wmq.q_ptr = &siors232test_echo_q;

        dsm_queue_init(&siors232test_wmq, /* watermark queue */
           10000, /* dont_exceed_cnt */
           &siors232test_echo_q  /* queue */
           );

           
        /* Note, there is no break statement here                   */
        /* If there is no need to change port, run to the next case */
    case STATE_INIT:
        /* Open the sio port */
        sio_open(&siors232test_echo_open_struct);
        /* We are done */
        current_state = STATE_DONE;
#ifdef SIORS232TEST_USE_FUSION_LOOPBACK
        // set current_rx_count to allow 1 transmit.
        rstest_test_type.current_rx_count = rstest_test_type.tx_length;
        // Start the new loopback by sending one transmit.
        siors232test_command('N');
#endif 
        break;

    case STATE_CLOSE_INIT:
        sio_close(siors232test_echo_open_struct.stream_id, 
           siors232test_process_switch);
        current_state = STATE_CLOSE_STAGE2;
        break;

    case STATE_CLOSE_STAGE2:
        
    case STATE_DONE:
        break;
    default:
        break;
    }
}



/*===========================================================================

FUNCTION siors232test_hiwater_cb

DESCRIPTION
  This callback happens when the watermark queue is too full, 
  the action is to disable inbound flow 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_DSM_WM_CB
   static void siors232test_hiwater_cb(struct dsm_watermark_type_s *wm_ptr, void *v_ptr)
#else /* FEATURE_DSM_WM_CB */
   static void siors232test_hiwater_cb()
#endif /* FEATURE_DSM_WM_CB */
{
    sio_ioctl_param_type ioctl_p;
    sio_ioctl(siors232test_echo_open_struct.stream_id, 
              SIO_IOCTL_INBOUND_FLOW_DISABLE,
              &ioctl_p);
}


/*===========================================================================

FUNCTION siors232test_lowater_cb

DESCRIPTION
  This callback happens when the watermark queue has more room,
  thus the inbound flow can be re-enabled

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
#ifdef FEATURE_DSM_WM_CB
   static void siors232test_lowater_cb(struct dsm_watermark_type_s *wm_ptr, void *v_ptr)
#else /* FEATURE_DSM_WM_CB */
   static void siors232test_lowater_cb()
#endif /* FEATURE_DSM_WM_CB */
{

    sio_ioctl_param_type ioctl_p;
    sio_ioctl(siors232test_echo_open_struct.stream_id, 
              SIO_IOCTL_INBOUND_FLOW_ENABLE,
              &ioctl_p);
}


/*===========================================================================

FUNCTION siors232test_rdm_event_cb

DESCRIPTION
  Process peripheral emulation switch state callback from rdm.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================
static void siors232test_rdm_event_cb(rdm_assign_status_type status,
                                      rdm_service_enum_type service, 
                                      rdm_device_enum_type device)
{
    siors232test_process_switch();
}
*/
/*===========================================================================

FUNCTION siors232test_sio_process_rx_packet

DESCRIPTION
  process_rx packet function for UART

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

NOTE
  When a packet is received, the transmit function is called right away.  
  To simulate a busy system and to cause FIFO overflow, place a breakpoint 
  on this function to delay the execution.

===========================================================================*/

static void siors232test_sio_process_rx_packet(dsm_item_type **dsm_item_ptr)
{
    uint32 i, len;

    UART_LOG_0( TRACE, "+siors232test_sio_process_rx_packet");

    switch (rstest_test_type.test_type) 
    {
    case SIORS232TEST_RX_ONLY:
        rstest_test_type.rx_byte_count += dsm_length_packet(*dsm_item_ptr);
        dsm_free_packet(dsm_item_ptr);
        break;
    case SIORS232TEST_LOOPBACK:
        len = dsm_length_packet(*dsm_item_ptr);
        rstest_test_type.rx_byte_count += len;
        sio_transmit(siors232test_echo_open_struct.stream_id, *dsm_item_ptr);
        rstest_test_type.tx_byte_count += len;
        break;

    case SIORS232TEST_TX_SOURCE:

        uart_test_tx_source_on_rx( dsm_item_ptr );
        break;

    case SIORS232TEST_LINEAR:
        for(i=0;i<(*dsm_item_ptr)->used;i++)
        {
            if((*dsm_item_ptr)->data_ptr[i] != rstest_test_type.next_byte)
            {
                rstest_test_type.error_count++;
            }
            rstest_test_type.next_byte = (rstest_test_type.next_byte+1) & 0x0ff;
        }
        dsm_free_packet(dsm_item_ptr);
        break;
    case SIORS232TEST_AA55:
        for(i=0;i<(*dsm_item_ptr)->used;i++)
        {
            if((*dsm_item_ptr)->data_ptr[i] != rstest_test_type.next_byte)
            {
                rstest_test_type.error_count++;
            }
            if((*dsm_item_ptr)->data_ptr[i] == 0xAA)
            {
                rstest_test_type.next_byte = 0x55;
            }
            else
            {
                rstest_test_type.next_byte = 0xAA;
            }
        }
        dsm_free_packet(dsm_item_ptr);
        break;
    case SIORS232TEST_FUSION_LOOPBACK:
        len = dsm_length_packet(*dsm_item_ptr);
        rstest_test_type.rx_byte_count += len;
        rstest_test_type.current_rx_count += len;
        dsm_free_packet(dsm_item_ptr);
        if (rstest_test_type.current_rx_count >= rstest_test_type.tx_length)
        {
            if (rstest_test_type.use_interrupt_context)
            {
                siors232test_command('N');
            }
        }
        break;
    default:
        /* Free the packet to prevent memory leaks */
        dsm_free_packet(dsm_item_ptr);
        break;
    }

    UART_LOG_0( TRACE, "-siors232test_sio_process_rx_packet");
}  

/*===========================================================================

FUNCTION siors232test_echo_close                        EXTERNALIZED FUNCTION

DESCRIPTION
  Stop echo test process

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Should be called only once.

===========================================================================*/

void siors232test_echo_close(void)
{
    current_state = STATE_CLOSE_INIT;

    /* Call function to inititate closing of ports */
    siors232test_process_switch();
}                                     

/*===========================================================================
 
FUNCTION siors232test_echo_init                         EXTERNALIZED FUNCTION

DESCRIPTION
  Close the existing ports and open a specified port for echoing packets 
  from the host

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Should be called only once.

===========================================================================*/

void siors232test_echo_init(void)
{
    current_state = STATE_RESET;
    rstest_test_type.error_count = 0;
    rstest_test_type.tx_byte_count = 0;
    rstest_test_type.rx_byte_count = 0;
    rstest_test_type.current_rx_count = 0;
    rstest_test_type.next_byte = 0;
    rstest_test_type.tx_length = 64;
    rstest_test_type.test_type = SIORS232TEST_LOOPBACK;
    rstest_test_type.use_interrupt_context = FALSE;

#if defined (SIORS232TEST_USE_AUX_UART)
    siors232test_echo_open_struct.port_id = SIO_PORT_UART_AUX;
    siors232test_rdm_dev = RDM_UART2_DEV;
#elif defined(SIORS232TEST_USE_THIRD_UART) 
    siors232test_echo_open_struct.port_id = SIO_PORT_UART_THIRD;
    siors232test_rdm_dev = RDM_UART3_DEV;
#endif
 
#ifdef SIORS232TEST_USE_FUSION_LOOPBACK
    rstest_test_type.test_type = SIORS232TEST_FUSION_LOOPBACK;
#endif

    /* Call function to inititate closing of ports */
    siors232test_process_switch();
}                                     

/*===========================================================================
 
FUNCTION siors232test_rx_check_init                   EXTERNALIZED FUNCTION

DESCRIPTION
  Close the existing ports and open a specified port for echoing packets 
  from the host
  
PARAMETERS
  Two kind of test are defined, test_type = 1 for linear increment test,
  otherwise AA/55 test is used

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Should be called only once.

===========================================================================*/

void siors232test_rx_check_init(uint32 test_type) 
{
    current_state = STATE_RESET;
    rstest_test_type.error_count = 0;
    if(test_type == 1) {
        rstest_test_type.test_type = SIORS232TEST_LINEAR;
        rstest_test_type.next_byte = 0;
    }
    else {
        rstest_test_type.test_type = SIORS232TEST_AA55;
        rstest_test_type.next_byte = 0xAA;
    }

    /* Call function to inititate closing of ports */
    siors232test_process_switch();
}                                     

/*===========================================================================

FUNCTION siors232test_transmit_fixed_data

DESCRIPTION
  It will allocate the DSM item,fill that dsm item with a character which is
  recieved as parameter and than sent to the sio layer to transfer the data
  over UART by using stream id allocaed for that UART port which is recieved
  as a parameter.

PARAMETERS
  sio_stream_id which is used to call sio_transmit and a character to fill in 
  the tx packet.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void siors232test_transmit_fixed_data(sio_stream_id_type lb_stream_id,
                                            char ch, int size_of_data)
{
    dsm_item_type *item_ptr;
    int i;
     
    item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
    if(item_ptr != NULL)
    {
        size_of_data = (size_of_data <= item_ptr->size) ? (size_of_data) : (item_ptr->size);    
        for(i=0;i<size_of_data;i++)
        {
            item_ptr->data_ptr[i] = ch;
        }
        (item_ptr)->used = i;
        sio_transmit(lb_stream_id, item_ptr);
    }
}
 
     

/*===========================================================================

FUNCTION siors232test_command                        EXTERNALIZED FUNCTION

DESCRIPTION
  Test different IOCTL commands

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void siors232test_command(char cmd)
{
    sio_ioctl_param_type ioctl_p;
    dsm_item_type *item_ptr;
    int data_size=40;
    int i=0;
    switch(cmd)
    {
        case 'A':
            sio_ioctl(siors232test_echo_open_struct.stream_id, 
                      SIO_IOCTL_INBOUND_FLOW_DISABLE,
                      &ioctl_p);
            break;
        case 'B':
            sio_ioctl(siors232test_echo_open_struct.stream_id, 
                      SIO_IOCTL_INBOUND_FLOW_ENABLE,
                      &ioctl_p);
            break;
        case 'C':
            
            siors232test_transmit_fixed_data(siors232test_echo_open_struct.stream_id,'A',data_size);
            siors232test_transmit_fixed_data(siors232test_echo_open_struct.stream_id,'B',data_size);
            sio_ioctl(siors232test_echo_open_struct.stream_id, 
                      SIO_IOCTL_FLUSH_TX,
                      &ioctl_p);
            
            siors232test_transmit_fixed_data(siors232test_echo_open_struct.stream_id,'C',data_size);
            siors232test_transmit_fixed_data(siors232test_echo_open_struct.stream_id,'D',data_size);
            siors232test_transmit_fixed_data(siors232test_echo_open_struct.stream_id,'E',data_size);
            sio_ioctl(siors232test_echo_open_struct.stream_id, 
                      SIO_IOCTL_FLUSH_TX,
                      &ioctl_p);
            break;
        case 'D':
            sio_ioctl(siors232test_echo_open_struct.stream_id, 
                      SIO_IOCTL_POWERDOWN,
                      &ioctl_p);
            break;
        case 'E':
            sio_ioctl(siors232test_echo_open_struct.stream_id, 
                      SIO_IOCTL_WAKEUP,
                      &ioctl_p);
            break;
        case 'F':
            siors232test_transmit_fixed_data(siors232test_echo_open_struct.stream_id,'A',10);
            siors232test_transmit_fixed_data(siors232test_echo_open_struct.stream_id,'B',10);
            siors232test_transmit_fixed_data(siors232test_echo_open_struct.stream_id,'C',10);
            siors232test_transmit_fixed_data(siors232test_echo_open_struct.stream_id,'D',10);
            siors232test_transmit_fixed_data(siors232test_echo_open_struct.stream_id,'E',10);
            break;
        case 'N':        // used with new FUSION_LOOPBACK test
            while (rstest_test_type.current_rx_count >= 
                   rstest_test_type.tx_length)
            {
                rstest_test_type.current_rx_count -= rstest_test_type.tx_length;
                item_ptr = dsm_new_buffer( DSM_DIAG_ITEM_POOL);
                if(item_ptr != NULL)
                {
                    for(i=0;i<rstest_test_type.tx_length;i++)
                    {
                        item_ptr->data_ptr[i] = i & 0xff;
                    }
                    (item_ptr)->used = i;
                    rstest_test_type.tx_byte_count += rstest_test_type.tx_length;
                    sio_transmit(siors232test_echo_open_struct.stream_id, item_ptr);
                }   
            }
            break;
        default:
            break;
    }
}

/*===========================================================================

FUNCTION          uart_test_tx_source_on_rx

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

static void
uart_test_tx_source_on_rx( dsm_item_type** ppDsmItem )
{
   siors232test_test_state_type*   txd = &rstest_test_type;
   uint8*                          ptr;
   uint8*                          plast;
   uint8                           byte;
   dsm_item_type*                  pDsmItem;
   boolean                         first = TRUE;
   uint32                          len;



           
   UART_LOG( TRACE, "+uart_test_tx_source_on_rx");

   if( !( ppDsmItem && *ppDsmItem ))
   {
      UART_LOG( ERROR, "NULL DSM item pointer");
      goto done;
   }

   //  Do some error checking.  Make sure the received data is what's expected.
   
   pDsmItem = *ppDsmItem;

   len   = pDsmItem->used;
   ptr   = pDsmItem->data_ptr;
   plast = ptr + len;

   //  Count RXed bytes.
           
   txd->rx_byte_count += len;

   //  Compare actual RX data with expected RX data.
           
   for( ; ptr < plast; ptr++, txd->next_rx_byte++ )
   {
      byte = *ptr;
      if( byte != txd->next_rx_byte )
      {
         txd->error_count++;
         if( first )
         {
            first = FALSE;
            UART_LOG_2( ERROR, "RX ERROR: expected 0x%02x, got 0x%02x.", txd->next_rx_byte, byte );
         }
      }

      //  The expected result is linear.  If a byte is added or dropped, 
      //  reset the next_rx_byte so we get just the one error instead of 
      //  an error for every byte.

      txd->next_rx_byte = byte;
   }

   //  Just free the packet.  We're not looping back.

   dsm_free_packet( ppDsmItem );

   UART_LOG_1( TRACE, "RXed %d total bytes.", txd->rx_byte_count );
   UART_LOG_1( TRACE, "%d RX errors so far.", txd->error_count );

done:

   UART_LOG( TRACE, "-uart_test_tx_source_on_rx");
}

/*===========================================================================

FUNCTION          uart_test_tx_source_start

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/

void 
uart_test_tx_source_start( void )
{
   siors232test_test_state_type* txd = &rstest_test_type;



   UART_LOG_0( TRACE, "+uart_test_tx_source_run");

   current_state = STATE_RESET;

   memset( txd, 0, sizeof( *txd ));

   txd->test_type             = SIORS232TEST_TX_SOURCE;
   txd->use_interrupt_context = FALSE;
   
   //  Set a timer to fire every TX_SOURCE_TIMEOUT_INTERVAL ms.

  UART_LOG_0( TRACE, "-uart_test_tx_source_run");
}                                     


