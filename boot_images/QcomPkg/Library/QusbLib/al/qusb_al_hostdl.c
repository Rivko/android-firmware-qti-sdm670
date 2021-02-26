/*=======================================================================*//**
  @file         qusb_al_hostdl.c

  @brief        ADAPTATION LAYER FOR ARMPRG

  @details      
                Armprog require dispatch table, defined as global structuse
                instance, to be filled.

               EXTERNALIZED FUNCTIONS (through dispatch table)
                  usb_drain
                    To drain the USB transmit buffer.
               
                  usb_init
                     Initializes the USB for transmit and receive.
                     
                  usb_active
                     Determines if activity on any endpoint
                     
                  do_usb_check
                     Determines if any data is available and calls function to process it
               
                  usb_transmit_byte
                     Transmits a single byte to the USB
               
               INITIALIZATION AND SEQUENCING REQUIREMENTS
                 All necessary initialization for normal CPU operation must have
                 been performed before entering this module.  usb_init() should
                 be called before any other functions.

 
                Copyright (c) 2011 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.


  when        who       what, where, why
  ----------  ---      -------------------------------------------------------
  2014-06-06  shreyasr Added this driver for MDM9x45
  2011-03-23  aperel   update for MSM8660 
  2008-08-14  vkondrat redesign - use simple 1 buffer mode, Rx callback
  2008-08-10  vkondrat pass full download flow
  2008-07-17  vkondrat Initial version with empty stubs

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "comdef.h"
#include "flash_prog.h"
#include "qusb_fd.h"
#include "qusb_dci_api.h"
#include "qusb_dcd.h"
#include "qusb_common.h"
#include "qusb_dci_common.h"
#include "qusb_dci_private.h"
#include "qusb_log.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

#define QUSB_HOSTDL_CONNECTION_TIMEOUT 200

#ifndef QUSB_HOSTDL_ENUM_PID
#define QUSB_HOSTDL_ENUM_PID (0x9008)
#endif
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static struct qusb_dcd_dsc_device* dev = NULL;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static boolean usb_init(void);
static boolean usb_active(void);
static void usb_drain(void);
static void do_usb_check(void);
static void usb_transmit_byte(byte chr);
static void usb_deinit(void); 

// iProduct
static char qusb_str_product[] = "QUSB_ARMPRG";

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
/* 
 *  USB function dispatch table
 */ 
DISP usb_dispatch_table = {
  usb_init,
  usb_active,
  usb_drain,
  do_usb_check,
  usb_transmit_byte,
  usb_deinit
};

#if defined(FEATURE_QUSB_ARMPROG_USE_BYTE_HANDLER)
extern void packet_handle_incoming(int);
/* if we have no external packet handling routine, provide one */
static inline void packet_handle_incoming_buf(byte* buf, int len)
{
   while ( len-- >0 ) {
      packet_handle_incoming(*buf++);
   }
}
#else /* use packet handler */
extern void packet_handle_incoming_buf(byte* buf, int len);
#endif

/**
 * This function checks for the assertion of any enabled
 * USB function controller interrupt.
 * 
 * @note The watchdog is reset.
 */
static void do_usb_check(void)
{
  qusb_dcd_poll(dev);
}


/**
 * This function buffers a byte into USB tx buffer.
 * If the end of message (0x7E) is received from the
 * packet module, then this function will send the data.
 * 
 * @note May block
 * @note The watchdog may be reset.
 * @param chr    Byte to transmit
 */
static void usb_transmit_byte(byte chr)
{
  qusb_log(HOSTDL_USB_TRANSMIT_BYTE_LOG, 0, chr);
  qusb_fd_tx_byte(chr);
}

/**
 * This function waits for the IN transfer to complete.
 * This allows the caller to be sure that all
 * characters are transmitted.
 * 
 * @note The watchdog may be reset.
 */
static void usb_drain(void)
{
  qusb_log(HOSTDL_USB_DRAIN_LOG, 0, 0);
  /* Wait for transfer to complete */
  while ( qusb_fd_is_tx_active() != 0 ) 
  {
    qusb_dcd_poll(dev);
  }
}

/**
 * 
 * @return False : The USB is not active
 *         True : USB is active
 */
static boolean usb_active(void)
{
  /* If we called usb_init() and returned TRUE, which is the only way we
  * call this function, we are guaranteed to have data, so just return
  * TRUE.
  */
  qusb_log(HOSTDL_USB_ACTIVE_LOG, 0 , 1);
  return TRUE;
}

static void ap_rx_complete(struct qusb_urb* urb)
{
  qusb_log(HOSTDL_AP_RX_COMPLETE_LOG, 0, 0);
  if ( QUSB_URB_STATUS_COMPLETE_OK == urb->transfer_status ) 
  {
    packet_handle_incoming_buf(urb->transfer_buffer_ptr,(int)urb->actual_length);
    /* mark it done */
    qusb_rx_completed_int_buf_mode(urb);
  }
}

/**
 * This function initializes the MSM USB device controller
 * with no interrupts enabled but both receiver and 
 * transmitter running.
 * 
 * @return True if init succeeded
 */
static boolean usb_init(void)
{
  uint32 t_end = 0;
  qusb_log(HOSTDL_USB_INIT_LOG, 0, 0);

  qusb_fd_set_core(0); 

  qusb_fd_set_buffer_mode(FD_USB_INTERNAL_BUFFER_MODE);
  
  dev = qusb_fd_init(ap_rx_complete,
                     qusb_str_product,
                     NULL,
                     NULL,
                     NULL,
                     USB_PRODUCT_DIAG_ONLY,
                     QUSB_MAX_SPEED_HIGH);

  if ( NULL != dev ) 
  {
    while ( t_end != QUSB_HOSTDL_CONNECTION_TIMEOUT ) 
    {
      qusb_dcd_poll(dev);
      if ( QUSB_UNKNOWN_SPEED != dev->speed ) //i.e. if speed is detected then SOF's are working etc.
      {
        qusb_log(HOSTDL_USB_INIT_SUCCESS_LOG, 0, 1);
        return TRUE;
      }
      qusb_dci_delay_ms(1); 
      t_end++; 
    }
  }
  qusb_error_log(HOSTDL_USB_INIT_FAILED_LOG, 0 ,0);
  return FALSE;
}
/** This function clears the single enumeration cookie in PBL, 
 * disconnects from the Host
 * shuts down the USB stack
 * 
 * @return None
 */ 
static void usb_deinit(void)
{
  qusb_log(HOSTDL_USB_DEINIT_LOG, 0, 0);
  //clear the single enumeration cookie
  qusb_dci_write_enum_cookie(0x0);
  //Clear the run-stop bit
  qusb_dci_set_run_stop(FALSE);
  qusb_fd_shutdown(); 
  return; 
}

