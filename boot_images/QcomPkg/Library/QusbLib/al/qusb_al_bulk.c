/*=======================================================================*//**
 * @file        qusb_al_bulk.c
 * @author:     dpatnaik
 * @date        12-Feb-2011
 *
 * @brief       QUSB (Qualcomm High-Speed USB) AL (Adaptation-Layer) implementation.
 *
 * @details     This file contains the implementation of the APIs to be used by
 *              the boot Sahara downloader.
 *
 * @note        
 *
 *              Copyright 2011 Qualcomm Technologies Incorporated.
 *              All Rights Reserved.
 *              Qualcomm Confidential and Proprietary
 * 
*//*========================================================================*/


// ===========================================================================
// 
//                            EDIT HISTORY FOR FILE
//   This section contains comments describing changes made to the module.
//   Notice that changes are listed in reverse chronological order.
// 
// 
// when         who        what, where, why
// --------   ---        ----------------------------------------------------------
// 03/28/11   niting     Added callback support
// 02/12/11   dpatnaik Initial revision
//
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"           // common defines - basic types as byte word uint32 etc
#include "LoaderUtils.h"      // For memscpy
#include "qusb_al_bulk.h"
#include "qusb_fd.h"
#include "qusb_dcd.h"
#include "qusb_dci_api.h"
#include "qusb_log.h"
#include "qusb_ldr_utils.h"
#include "qusb_dci_common.h"


//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
// -------------------------------------------------------------------------*/

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Static Function Declarations and Definitions
//----------------------------------------------------------------------------

//Maximum Bulk transfer size 
#define QUSB_BULK_LAYER_MAX_TRANSFER_SIZE_IN_BYTES        (1024*1024)
#define QUSB_BULK_ZLP_TIMER_VAL_USEC                      50000  //50 milliseconds 

// iProduct
static char qusb_bulk_str_product[] = "QUSB_BULK";

//For synchronous read within QUSB
//Client does not pass completion callback 
//and rely on QUSB to poll until transfer completion 
typedef struct
{
  uint32 actual_length;
  boolean pending_read;
} qusb_synch_read_ctx;

typedef enum
{
  QUSB_CHECK_FOR_ZLP,
  QUSB_LACKS_ZLP,
  QUSB_SENDS_ZLP
} qusb_zlp_mode_e;

//@TODO During Bring Up Enable CHECK_FOR_ZLP
//static qusb_zlp_mode_e qusb_zlp_mode = QUSB_CHECK_FOR_ZLP;
static qusb_zlp_mode_e qusb_zlp_mode = QUSB_SENDS_ZLP;
static boolean qusb_pending_rx_zlp = FALSE;

static boolean qusb_waiting_for_zlp = FALSE; //Used to indicate that we are waiting for a ZLP 
static uint32 cached_zlp_data_length = 0; // Length of data received from the host instead of the impending ZLP
static uint32 no_zlp_count = 0; //Number of times host sent data instead of ZLP. Used for tracking / debug.

static struct qusb_urb* qusb_al_bulk_tx_urb = NULL;
static struct qusb_dcd_dsc_device* qusb_al_bulk_dev = NULL;
static boolean qusb_enumerated = FALSE;

static qusb_bulk_rx_cb_type al_bulk_rx_cb = NULL; //hold the callback for propagation to upper layer
static qusb_synch_read_ctx qusb_synch_rx_ctx;


__QUSB_DMA_ZI__ static uint8 zlp_read_buf[QUSB_MAX_CTRL_PKT_SIZE];

static boolean qusb_is_disconnected = FALSE;
static boolean qusb_is_rx_in_progress = FALSE;

boolean qusb_al_check_for_fedl(uint32 core_id)
{
  return  qusb_ldr_utils_fedl_check();
}


uint32 qusb_al_bulk_receive_zlp(void)
{
  uint32 err_code = FD_SUCCESS;

  qusb_log(AL_BULK_RECEIVE_ZLP_LOG,0,0);
  //prepare an empty TD to read ZLP 
  err_code = qusb_fd_rx_pkt(zlp_read_buf, QUSB_MAX_CTRL_PKT_SIZE);
  if (err_code != FD_SUCCESS)
  {
    qusb_error_log(AL_BULK_RECEIVE_ZLP_ERROR_LOG, 0, 0);
  }
  else
  {
    if(qusb_zlp_mode == QUSB_CHECK_FOR_ZLP)
    {
      qusb_pending_rx_zlp = TRUE; 
      qusb_timer_start(QUSB_BULK_ZLP_TIMER_VAL_USEC); 
      //return flow control and let the next iteration of qusb_al_bulk_rx_complete handle the ZLP
    }
    //else for optimization don't prime start timer for ZLP
  }
  return err_code;
}

uint32 qusb_al_bulk_handle_rx_zlp_mode(void)
{
  uint32 err_code = FD_SUCCESS;

  qusb_log(AL_BULK_HANDLE_RX_ZLP_MODE_LOG,0,qusb_zlp_mode);
  switch(qusb_zlp_mode)
  {
    case QUSB_CHECK_FOR_ZLP:
    case QUSB_SENDS_ZLP:
      qusb_waiting_for_zlp = TRUE;
      err_code = qusb_al_bulk_receive_zlp();
    break;
    case QUSB_LACKS_ZLP:
      //host is determined to not send ZLP
      break;
    default:
    break;
  }
  return err_code;
}

// ===========================================================================
/**
 * @function    qusb_al_bulk_set_zlp_mode
 * 
 * @brief   Sets the ZLP mode based on host type.
 *
 * @param   [IN] Whether host supports ZLP
 *
 *          
 * @return  None
 * 
 */
 // ===========================================================================
void qusb_al_bulk_set_zlp_mode(boolean host_sends_zlp)
{
  //if this API is not called then by default it is QUSB_CHECK_FOR_ZLP
  // but for now it is QUSB_SENDS_ZLP
  if(TRUE == host_sends_zlp)
  {
    qusb_zlp_mode = QUSB_SENDS_ZLP;
    qusb_uart_log("host sends ZLP", 0);
  }
  else
  {
    qusb_zlp_mode = QUSB_LACKS_ZLP;
    qusb_uart_log("host lacks ZLP", 0);
  }
  qusb_log(AL_BULK_SET_ZLP_MODE_LOG, 0, qusb_zlp_mode);
}

// ===========================================================================
/**
 * @function    qusb_al_bulk_rx_complete
 * 
 * @brief   This function will be a called after each URB receive is complete.
 *
 * @details This function will be a called after each URB receive is complete.
 * 
 * @param   *urb   -  [IN/OUT] URB corresponding to the current reception
 * 
 * @return  None.
 * 
 */
// ===========================================================================
static void qusb_al_bulk_rx_complete(struct qusb_urb* urb)
{
  static uint32 total_bytes_received = 0;
  boolean short_packet_received = FALSE;
  uint32 err_code = FD_SUCCESS;

  if(NULL == urb)
  {
    //this is not possible @rx_complete
    qusb_error_log(AL_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }

  qusb_log(AL_BULK_RX_COMPLETE_LOG,0,0);
  if (QUSB_URB_STATUS_COMPLETE_OK == urb->transfer_status)
  {
    if (urb->actual_length == 0) //ZLP 
    {
      qusb_log(AL_BULK_ZLP_ARRIVED_LOG,0,0);
      qusb_waiting_for_zlp = FALSE;      
      short_packet_received = TRUE; //ZLP coming anytime from the host

      //also look for ZLP that is pending from last iteration of priming buffers to trap ZLP
      if (qusb_pending_rx_zlp)
      {
        //host confirmed to send ZLP, can optimize and remov subsequent ZLP timer activation
        qusb_zlp_mode = QUSB_SENDS_ZLP;
        qusb_pending_rx_zlp = FALSE;
      }
    }
    if ((urb->actual_length % qusb_al_bulk_get_max_packet_size()) > 0) //check for short packet
    { 
      short_packet_received = TRUE; //end of transfer, return to upper layer
      qusb_log(AL_BULK_SHORT_PACKET_RECVD_LOG,0,total_bytes_received);
    }
    total_bytes_received += urb->actual_length;
    qusb_is_rx_in_progress = FALSE;
    qusb_log(TRANSFER_DONE_LOG,0,total_bytes_received);
  }
  else
  {
    //TODO handle other exceptional cases from lower layer 
    //signature of ZLP being missing from the host 
    if (QUSB_URB_STATUS_COMPLETE_ERROR == urb->transfer_status)
    {
      qusb_log(TRANSFER_ERROR_LOG,0,0);
      if (qusb_pending_rx_zlp)
      {
        short_packet_received = TRUE;
        qusb_pending_rx_zlp = FALSE;
      }
    }
  }

  //For short packets, transfer is complete, return the number of bytes transferred to the upper layer.
  //Otherwise, prime buffers to handle ZLP as needed.
  if (urb->transfer_length == urb->actual_length || short_packet_received)
  {

    //We expected a ZLP but received data from the host, then call the callback into client layer
    if (qusb_waiting_for_zlp && (urb->actual_length != 0))
    {
      qusb_log(AL_BULK_DATA_RECVD_INSTEAD_OF_ZLP_LOG,0,urb->actual_length);        
      al_bulk_rx_cb(total_bytes_received - urb->actual_length, BULK_SUCCESS); //Adjust bytes received and callback
      al_bulk_rx_cb = NULL; 
      total_bytes_received = urb->actual_length; //Update bytes received 
      cached_zlp_data_length = urb->actual_length; //Update count of how many bytes we are holding on to
      no_zlp_count++; //Increment count that keeps track of no ZLP condition
      return;
    }
    
    if ((urb->transfer_length == urb->actual_length) &&
      ((urb->transfer_length % qusb_al_bulk_get_max_packet_size()) == 0))
    {
      if (qusb_zlp_mode == QUSB_LACKS_ZLP) // No ZLP is expected
      {
        //transfer is complete, return bytes transferred to upper layer immediately 
        if (al_bulk_rx_cb != NULL)
        {
          qusb_log(AL_BULK_RX_CB_RX_DONE_LOG,0,total_bytes_received);
          al_bulk_rx_cb(total_bytes_received, BULK_SUCCESS);
          al_bulk_rx_cb = NULL; // use only once 
          qusb_is_rx_in_progress = FALSE;

          total_bytes_received = 0;
        }
        else // synchronous read 
        {
          qusb_synch_rx_ctx.actual_length = total_bytes_received;
          qusb_synch_rx_ctx.pending_read = FALSE;

          total_bytes_received = 0;
        }
      }
      else
      { 
        // Handle ZLP only when necessary 
        err_code = qusb_al_bulk_handle_rx_zlp_mode();
        if (FD_SUCCESS != err_code) //error condition 
        {
          //error setting up subsequent priming, return error condition immediately 
          if (al_bulk_rx_cb != NULL)
          {
            al_bulk_rx_cb(total_bytes_received, BULK_ERR_READ);
            al_bulk_rx_cb = NULL; /* use only once */
            total_bytes_received = 0;
          }
          else // synchronous read 
          {
            qusb_synch_rx_ctx.actual_length = total_bytes_received;
            qusb_synch_rx_ctx.pending_read = FALSE;
            total_bytes_received = 0;
          }
        }
      }
      // return and let the next iteration handle ZLP 
    }
    else
    {
      if (al_bulk_rx_cb != NULL)
      {
        qusb_log(AL_BULK_RX_CB_SHORT_PACKET_LOG,0,total_bytes_received);
        al_bulk_rx_cb(total_bytes_received, BULK_SUCCESS);
        al_bulk_rx_cb = NULL; // use only once 

        qusb_is_rx_in_progress = FALSE;

        total_bytes_received = 0;
      }
      else //synchronous read 
      {
        qusb_synch_rx_ctx.actual_length = total_bytes_received;
        qusb_synch_rx_ctx.pending_read = FALSE;

        total_bytes_received = 0;
      }
    }
  }
  else if(urb->actual_length > urb->transfer_length)
  {
    qusb_log(TRANSFER_ERROR_LOG,0,total_bytes_received);
  }
  else // more data to read 
  {
    uint32 remaining_read_size = urb->transfer_length - urb->actual_length;
    uint32 current_read_size = 0;

    // move the iterator to new destination address 
    uint8 *transfer_buffer_iter = (uint8*)(urb->transfer_buffer_ptr);
    qusb_log(AL_BULK_RX_MORE_DATA_LOG,0,remaining_read_size);

    transfer_buffer_iter += urb->actual_length;

    current_read_size = (remaining_read_size <= qusb_al_bulk_get_max_raw_data_size()) 
        ? remaining_read_size : qusb_al_bulk_get_max_raw_data_size();

    err_code = qusb_fd_rx_pkt((void *)transfer_buffer_iter, current_read_size);
    if (FD_SUCCESS != err_code)
    {
      // error setting up subsequent reads, return error condition immediately
      if (al_bulk_rx_cb != NULL)
      {
        al_bulk_rx_cb(total_bytes_received, BULK_SUCCESS);
        al_bulk_rx_cb = NULL; //use only once 
        total_bytes_received = 0;
        qusb_is_rx_in_progress = FALSE;
      }
      else // synchronous read 
      {
        qusb_synch_rx_ctx.actual_length = total_bytes_received;
        qusb_synch_rx_ctx.pending_read = FALSE;
        total_bytes_received = 0;
      }
    }
    else
    {
      // TODO reset the timer if there is more to read 
      // otherwise let the read proceed 
    }
  }
}

// ===========================================================================
/**
 * @function    qusb_al_bulk_tx_complete
 * 
 * @brief   This function will be a called after each URB transmit is complete.
 *
 * @details This function will be a called after each URB transmit is complete.
 * 
 * @param   *urb   -  [IN/OUT] URB corresponding to the current transmission
 * 
 * @return  None.
 * 
 */
// ===========================================================================
static void qusb_al_bulk_tx_complete(struct qusb_urb* urb)
{
  qusb_log(AL_BULK_TX_COMPLETE_LOG,0,0);
  //When TX complete happens, urb can't be NULL, so check for NULL status 
  if(NULL == urb)
  {
    qusb_error_log(AL_NULL_POINTER_ERROR_LOG, 0, 0);
  }
  else
  {
    qusb_al_bulk_tx_urb = urb;
  }
}

// ===========================================================================
/**
 * @function    qusb_al_enum_complete
 * 
 * @brief   This function is registered to be called when enumeration is
 *          complete.
 *
 * @details This function will set a static variable to indicate enumeration has
 *          completed.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
static void qusb_al_enum_complete(boolean enum_flag)
{
  qusb_log(AL_ENUM_COMPLETE_LOG,0,enum_flag);
  qusb_enumerated = enum_flag;
}

// ===========================================================================
/**
 * @function    qusb_al_disconnect_handler
 * 
 * @brief   This function will be called if there is a disconnect or bus reset
 *			during transfer
 *
 * @details This function will set a static variable to indicate the error.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
static void qusb_al_disconnect_handler(void)
{
  qusb_is_disconnected = TRUE;
  qusb_enumerated       = FALSE;

  qusb_uart_log("DISCONNECT", 0);
  qusb_log(AL_DISCONNECT_HANDLER_LOG, 0, 0);
}


//----------------------------------------------------------------------------
// Externalized Function Definitions
//----------------------------------------------------------------------------
// ===========================================================================
/**
 * @function    qusb_al_bulk_init
 * 
 * @brief   This function initializes the USB core and keeps polling until
 *          enumeration is complete.
 *
 * @details This function initializes the USB core and keeps polling until
 *          enumeration is complete.
 * 
 * @param   speed_required   -  [IN] The maximum speed at which USB should be
 *                            configured
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_al_bulk_init(qusb_max_speed_required_t speed_required)
{
  qusb_uart_log("init start", 0);
  qusb_al_bulk_tx_urb = NULL;
  qusb_enumerated = FALSE;

  qusb_synch_rx_ctx.actual_length = 0;
  qusb_synch_rx_ctx.pending_read = FALSE;

  qusb_log(AL_BULK_INIT_ENTRY_LOG,0,speed_required);
  //By default set the core as SNPS core
  qusb_fd_set_core(DCI_SNPS_CORE);

  //set the mode as Internal buffer mode 
  qusb_fd_set_buffer_mode(FD_CLIENT_SUPPLIED_BUFFER_MODE);

  qusb_al_bulk_dev = qusb_fd_init(qusb_al_bulk_rx_complete,
                                qusb_bulk_str_product,
                                qusb_al_bulk_tx_complete,
                                qusb_al_enum_complete,
                                qusb_al_disconnect_handler,
                                USB_PRODUCT_DIAG_ONLY,
                                speed_required);

  while ( FALSE == qusb_al_bulk_dev->qusb_is_configured )
  {
    if (TRUE == qusb_is_disconnected)
    {
      qusb_uart_log("Disconnect no enum", 0);
      qusb_log(AL_BULK_INIT_EXIT_LOG,0,0);
      return;
    }
    
    qusb_al_bulk_poll();
  }
  qusb_enumerated = TRUE;
  qusb_uart_log("ENUM success", 0);

  qusb_log(SNPS_CONTROLLER_ENUM_COMPLETE_LOG,0,0);
  qusb_log(AL_BULK_INIT_EXIT_LOG,0,0);
}

// ===========================================================================
/**
 * @function    qusb_al_bulk_poll
 * 
 * @brief   This function polls the hardware for events.
 *
 * @details This function polls the hardware for events.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_al_bulk_poll(void)
{
  uint32 total_bytes_received = 0x0; 

  qusb_dcd_poll(qusb_al_bulk_dev);

  //Now polling is controlled by application while we receive the data
  //Now if cable is disconnected in the RX path inform the application
  if (TRUE == qusb_is_disconnected)
  {
    if (TRUE == qusb_is_rx_in_progress)
  {
    //call their callback and tell higher layers of a disconnect
    if (al_bulk_rx_cb != NULL)
    {
      al_bulk_rx_cb(total_bytes_received, BULK_ERR_CABLE_DISCONNECT);
      al_bulk_rx_cb = NULL; /* use only once */
    }
    //Clear all the flags used
    qusb_is_rx_in_progress = FALSE;
    qusb_is_disconnected = FALSE;
  }
    else
    {
      // For TX path, return.
      return;
    }
  }
  //determine whether the host will send the ZLP or timeout
  if(qusb_pending_rx_zlp)
  {
    if(qusb_timer_check_for_expiry())
    {
      //now the timer is stopped and cleared at this point
      //handling the time out now
      switch(qusb_zlp_mode)
      {
        case QUSB_CHECK_FOR_ZLP:
          //even after a time out we didn't get a ZLP
          qusb_zlp_mode= QUSB_LACKS_ZLP; 
          qusb_pending_rx_zlp = FALSE; 
          qusb_waiting_for_zlp = FALSE; 
          qusb_dci_cancel_transfer(0x1,QUSB_EP_DIR_RX);
          break;
        case QUSB_SENDS_ZLP:
          //host times out by then 
          qusb_error_log(ZLP_TIMEOUT_ERROR_LOG, 0, 0); 
          break;
        case QUSB_LACKS_ZLP: 
          //shouldn't come here as qusb_pending_rx_zlp would not be set_new_handler
          break; 
        default: 
          break; 
      }
    }
    //allow the polling mechanism to continue as timer has not expired
  }
}
// ===========================================================================
/**
 * @function    qusb_al_bulk_set_shutdown_mode
 * 
 * @brief   This function sets shutdown mode of the hardware.
 *
 * @details This function sets shutdown mode of the hardware.
 * 
 * @param   Shutdown mode.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_al_bulk_set_shutdown_mode(qusb_shutdown_mode_t mode)
{
  qusb_fd_set_shutdown_mode(mode);
}
// ===========================================================================
/**
 * @function    qusb_al_bulk_get_shutdown_mode
 * 
 * @brief   This function reads shutdown mode of the hardware.
 *
 * @details This function reads shutdown mode of the hardware.
 * 
 * @param   None.
 * 
 * @return  Shutdown mode.
 * 
 */
// ===========================================================================
static qusb_shutdown_mode_t  qusb_al_bulk_get_shutdown_mode(void)
{
  return qusb_fd_get_shutdown_mode();
}

// ===========================================================================
/**
 * @function    qusb_al_bulk_shutdown
 * 
 * @brief   This function performs a shutdown of the hardware.
 *
 * @details This function performs a shutdown of the hardware.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_al_bulk_shutdown(void)
{
  qusb_log(AL_BULK_SHUTDOWN_LOG,0,0);

  switch(qusb_al_bulk_get_shutdown_mode())
  {
    case QUSB_SINGLE_ENUM_MODE:
    {
      qusb_dci_write_enum_cookie(QUSB_SINGLE_ENUM_COOKIE);
      qusb_fd_shutdown();
    }
    break;

    case QUSB_COMPLETE_SHUTDOWN_MODE:
    default:
      qusb_al_bulk_snps_shutdown();
  }
}
// ===========================================================================
/**
 * @function    qusb_al_bulk_snps_shutdown
 * 
 * @brief   This function performs a shutdown of the SNPS hardware.
 *
 * @details This function performs a shutdown of the SNPS hardware.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_al_bulk_snps_shutdown(void)
{
  qusb_dci_set_run_stop(FALSE);

  //Delay for spoof disconnect( SNPS QUSB - Table 6-62 )
  qusb_dci_delay_ms(30);
  //@TODO we need to check if the shutdown procedure works 
  //shutdown the stack properly
  qusb_dci_deinit_hs_phy();
  qusb_fd_shutdown();
}

// ===========================================================================
/**
 * @function    qusb_al_bulk_receive
 * 
 * @brief   This function receives a bulk of data upto the input length.
 *
 * @details This function will continue to poll the hardware until the receive
 *          URB is not NULL, upon which the data will be copied to the
 *          input receive buffer.
 * 
 * @param   *rx_buf     -  [IN/OUT] Receive buffer for data
 * @param   len         -  [IN    ] Length of buffer
 * @param   rx_cb       -  [IN    ] Return callback function
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * 
 * @return  Number of bytes receieved.
 * 
 */
uint32 qusb_al_bulk_receive(byte *rx_buf,   // must be 4 byte aligned       
                              uint32 len,   // read buffer size             
                                            // must be multiples of max     
                                            // packet size                  
                              qusb_bulk_rx_cb_type rx_cb, // completion callback 
                              uint32 * err_code)     // error code returned
{
  uint32 current_read_size = 0;

  uint8 *transfer_buffer_iter = (uint8*)(rx_buf);
  uint32 remaining_read_size = len;

  qusb_log(AL_BULK_RECEIVE_LOG,0,len);

  if (err_code == NULL || len == 0)
  {
    return 0;
  }

  if (qusb_waiting_for_zlp) // The ZLP buffer has received data due to host skipping sending ZLP
  {
    qusb_waiting_for_zlp = FALSE; // Clear flag 

    if(len < cached_zlp_data_length) // If FH layer asks for less than what host sent us instead of the ZLP 
    {
      cached_zlp_data_length = len;
    }

    qusb_log(AL_BULK_RX_ZLP_COPY_LOG ,0, cached_zlp_data_length);
    memscpy((void*)(rx_buf), cached_zlp_data_length, (void*)(zlp_read_buf), cached_zlp_data_length);

    if (cached_zlp_data_length < QUSB_MAX_CTRL_PKT_SIZE) // We received a short packet instead of the ZLP so we are done.
    {
      if(rx_cb)
      {
        qusb_log(AL_BULK_RX_CB_NO_ZLP_BUT_SHORT_PACKET_LOG,0,cached_zlp_data_length);
        rx_cb(cached_zlp_data_length, BULK_SUCCESS);
      } 

      al_bulk_rx_cb = NULL; // use only once 
      cached_zlp_data_length = 0;
      return 0 ;
    }

    remaining_read_size = len - cached_zlp_data_length;

    // move the iterator to new destination address 
    transfer_buffer_iter += cached_zlp_data_length;

    cached_zlp_data_length = 0;
  }    

  //enforce read only upto qusb_al_bulk_get_max_raw_data_size( ) 
  current_read_size = ((remaining_read_size <= qusb_al_bulk_get_max_raw_data_size()) ?
  remaining_read_size : qusb_al_bulk_get_max_raw_data_size());

  //setup read 

  qusb_log(AL_BULK_RECEIVE_LOG,0,current_read_size);
  *err_code = qusb_fd_rx_pkt((void *)transfer_buffer_iter, current_read_size);

  if (FD_SUCCESS != *err_code)
  {
    return 0; // error condition 
  }

  if (rx_cb == NULL)
  {
    //Synchronous read when client does not pass callback.
    // Poll in USB layer until completion 

    // safe to set flag here on the assumption that there is no polling in between packet setup and here 
    qusb_synch_rx_ctx.pending_read = TRUE;
    while (qusb_synch_rx_ctx.pending_read)
    {
      qusb_al_bulk_poll();
    }
    return qusb_synch_rx_ctx.actual_length;
  }
  else
  {
    //client layer will perform the polling 
    // transfer completes when client's cb is invoked 
    al_bulk_rx_cb = rx_cb;
    qusb_is_rx_in_progress = TRUE;
    *err_code = BULK_SUCCESS;
    return 0;
  }
}

// ===========================================================================
/**
 * @function    qusb_al_bulk_transmit
 * 
 * @brief   This function receives a bulk of data upto the input length.
 *
 * @details This function will continue to poll the hardware until the transmit
 *          URB transfer is complete.
 * 
 * @param   *tx_buf     -  [IN/OUT] Transmit buffer for data
 * @param   len         -  [IN    ] Length of data to trasmit in bytes
 * @param   tx_cb       -  [IN    ] Transmit complete callback
 * @param   *err_code   -  [IN/OUT] Any error that occurs during transfer
 * 
 * @return  Number of bytes transmitted.
 * 
 */
uint32 qusb_al_bulk_transmit(byte *tx_buf,        // must be 4 byte aligned   
                               uint32 len,        // number of bytes to send  
                               qusb_bulk_tx_cb_type tx_cb, // completion callback 
                               uint32 * err_code)     // returned error code 
{
  uint32 tx_length;
  *err_code = BULK_SUCCESS;

  qusb_log(AL_BULK_TRANSMIT_LOG,0,(uint32)tx_buf);
  qusb_log(TRANSFER_LENGTH_LOG,0,len);

  if (FALSE == qusb_is_disconnected)
  {
    // Check for enumeration to complete before submission ( to cater disconnect usecase ) 
    while ( !qusb_enumerated )
    {
      qusb_al_bulk_poll();
    }
    qusb_fd_tx_pkt(tx_buf, len);
  }

  while (qusb_al_bulk_tx_urb == NULL)
  {  
    if(TRUE == qusb_is_disconnected)
    {
      qusb_is_disconnected = FALSE;
      *err_code = BULK_ERR_CABLE_DISCONNECT;
      //Doesn't really matter how much was transferred, in anycase sahara has to begin 
      //again
      tx_length = 0x0; 
      if ( tx_cb != NULL )
      {
        tx_cb(tx_length, *err_code);
      }
      qusb_fd_shutdown();
      return tx_length;
    }
    qusb_al_bulk_poll();
  }
  tx_length = qusb_al_bulk_tx_urb->actual_length;
  if ( qusb_al_bulk_tx_urb->transfer_status == QUSB_URB_STATUS_COMPLETE_OK )
  {
    qusb_al_bulk_tx_urb = NULL;
  }
  else
  {
    qusb_log(AL_BULK_ERR_WRITE_LOG,0,0);
    qusb_al_bulk_tx_urb = NULL;
    *err_code = BULK_ERR_WRITE;
  }

  if ( tx_cb != NULL )
  {
    tx_cb(tx_length, *err_code);
  }
  return tx_length;
}

// ===========================================================================
/**
 * @function    qusb_al_bulk_get_max_packet_size
 * 
 * @brief   This function returns whether USB spec defined max packet sizes
 *          for FS USB or HS USB
 *
 * @details Correct packet sizes are determined internally and will be
 *          returned to external interface
 *          
 * @return  Max packet sizes
 * 
 */
uint32 qusb_al_bulk_get_max_packet_size(void)
{
  if(NULL != qusb_al_bulk_dev )
  {
    if (qusb_al_bulk_dev->speed == QUSB_SUPER_SPEED)
    {
      return 1024; 
    }
    else
    {
      return 512; 
    }

  }
  return 512;
}

// ===========================================================================
/**
 * @function    qusb_al_bulk_get_max_raw_data_size
 * 
 * @brief   Returns the maximum bulk transfer level size that can be requested
 *          by the client
 *
 * @details Windows only support maximum bulk transfer size of 1MB, whereas
 *          Linux supports a larger maximum 
 *          
 * @return  Max USB bulk transfer level size
 * 
 */
uint32 qusb_al_bulk_get_max_raw_data_size(void)
{
  //Maximum bulk transfer size
  return QUSB_BULK_LAYER_MAX_TRANSFER_SIZE_IN_BYTES;
}

