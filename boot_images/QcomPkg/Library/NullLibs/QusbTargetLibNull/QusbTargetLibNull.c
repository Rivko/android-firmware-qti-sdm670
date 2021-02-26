/** @file QusbTargetLibNull.c
  
  Stub functions for QusbTargetLib

  Copyright (c) 2015, Qualcomm Technologies, Inc. All rights reserved.
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/05/15   plc     Initial revision

=============================================================================*/


/*==========================================================================

                               INCLUDE FILES

===========================================================================*/
#include "qusb_log.h"
#include "qusb_dci_private.h"
#include "qusb_dci_api.h"
#include "qusb_urb.h"
#include "qusb_dcd.h"

/*===========================================================================
                      FUNCTION DECLARATIONS
===========================================================================*/ 
#define QUSB_DLOAD_INFO_ADDR_IN_IMEM              (SHARED_IMEM_USB_BASE)
//============================================================================
/**
* @function  qusb_timer_start
*
* @brief Call the API to start the timer 
*
* @Note : These timers are based on QTIMERS in APPS. This is a count down timer
*
* @param timer_value in uSec. 
*
* @return None
*
*/
//============================================================================
void qusb_timer_start(uint32 timer_value)
{
  return; 
}

//============================================================================
/**
* @function  qusb_timer_check_for_expiry
*
* @brief Call the API to stop the timer 
*
* @Note : These timers are based on QTIMERS in APPS. This is a count down timer
*
* @param  None 
*
* @return boolean: TRUE if expired else FALSE
*
*/
//============================================================================
boolean qusb_timer_check_for_expiry(void)
{
  return FALSE; 
}

// ===========================================================================
/**
 * @function    qusb_dci_cancel_transfer
 * 
 * @brief       This function terminates an active transfers
 * 
 * @details     
 * 
 * @param core_id    
 *
 * @param ep    
 *          endpoint number
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qusb_dci_shutdown
 * 
 * @ref SNPS Spec
 */
// ===========================================================================
uint32 qusb_dci_cancel_transfer(uint8 ep, uint8 dir)
{
  return(DCI_OK);
}

// ===========================================================================
/**
 * @function    qusb_dci_write_enum_cookie
 * 
 * @brief   API is used to write the single enumeration Cookie
 * 
 * @note    This API makes a Write to QSRAM register to store the 
 *          Single Enumeration Cookie 
 *  
 * @param   uint32 : The Cookie Value to be written 
 *
 */
// ===========================================================================
void qusb_dci_write_enum_cookie(uint32 val)
{
  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_set_run_stop
 * 
 * @brief   API used to set or clear Run Stop Bit.
 * 
 * @param  Set
 *               Value for setting or clearing run stop
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_set_run_stop(boolean set)
{
  return;
}

/**
* @function  qusb_dci_delay_ms
*
* @brief Perform delay in milliseconds.
*
* @Note : The USB Timers can not be used before the core is initialized.
*
* @param milliseconds
*
* @return none
*
*/
//============================================================================
void qusb_dci_delay_ms(uint32 msecs)
{  
  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_deinit_hs_phy
 * 
 * @brief   API used to de-initialize the High Speed PHY.
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_deinit_hs_phy(void)
{
  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_disable_usb30_clocks
 * 
 * @brief   This function will be used to turn OFF the USB3.0 clocks
 * 
 * @param   None
 * 
 * @return  TRUE or FALSE depending on success or failure.
 * 
 */
// ===========================================================================
boolean qusb_dci_disable_usb30_clocks(void)
{
  boolean var = TRUE; 
  return (var);
}

// ===========================================================================
/**
 * @function    dci_skip_enumeration
 * 
 * @brief   Determine whether enumeration in subsequent stages can be skipped
 * 
 * @note    Wrapper function to lookup whether enumeration in subsequent stages can be skipped          
 *  
 * @param   TRUE signifies enumeration is skipped
 *
 */
// ===========================================================================
boolean qusb_dci_skip_enumeration(void)
{  
  return TRUE;
}

// ===========================================================================
/**
 * @function    qusb_dci_unconfig_endpoint
 * 
 * @brief       This function de-configures the endpoint
 * 
 * @details     
 * 
 * @note        This function should be called on "Set Configuration" USB Standard Command.
 * 
 * @note        caution: if one endpoint direction is enabled and the paired endpoint of opposite
 *              direction is disabled
 *              then the unused direction type must be changed from the default controltype
 *              to any other type (ie. bulktype).
 *              leaving an unconfigured endpoint control will cause undefined behavior 
 *              for the data pid tracking on the active endpoint/direction.
 * 
 * @param ep    
 *          endpoint number
 * 
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qusb_dci_config_endpoint
 * 
 * @ref SNPS spec 
 *        
 */
// ===========================================================================
uint32 qusb_dci_unconfig_endpoint( uint8 ep, uint8 dir)
{
  uint32 status=DCI_OK;
  return status;
}

//============================================================================
/**
 * @function  qusb_dci_get_speed
 *
 * @brief Report the USB connection speed.
 *
 * @Note : Although the device is high-speed, the Host might be full-speed.
 *
 * @Note : The USB HW can be forced to Full-Speed.
 *
 * @param None 
 * @return Speed
 *              QUSB_SPEED_FULL        
 *              QUSB_SPEED_LOW         
 *              QUSB_SPEED_HIGH        
 *              QUSB_SPEED_FULL
 *              QUSB_SPEED_UNKNOWN     
 *
 * @ref SNPS spec 
 *       Chapter 6.3.1.4 DSTS ( bits 0-2)
 *
 */
//============================================================================
uint32 qusb_dci_get_speed(void)
{
  uint32 speed=0;
  return(speed);
}

//============================================================================
/**
 * @function  qusb_dci_set_max_packet_size
 *
 * @brief Sets the maximum packet size of EP0 to be used
 *
 * @param max_packet_size  
 *
 * @return None  
 *
 * @ref SNPS spec 
 *       
 */
//============================================================================
void qusb_dci_set_max_packet_size(uint32 max_packet_size)
{
  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_transfer_request
 * 
 * @brief       This function ques data transfer according to URB.
 * 
 * @details     
 *              This function calls transfer_data_request() and update the URB.
 * 
 * @note        The USB is a Master-Slave protocol where the Device is the Slave.
 *              The Device's data is actually sent/received on the USB bus 
 *              only when the Host send SETUP/IN/OUT token.
 *              Therefore, the caller only start a REQUEST.
 *              A callback is called upon complete/error detection.
 *              The caller May poll the status in the URB.
 * 
 * @param urb_ptr
 *              A pointer to URB structure.
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see transfer_data_request
 * 
 */
// ===========================================================================
uint32 qusb_dci_transfer_request(qusb_urb_t* urb_ptr)
{
  uint32  status=0;
  return status;
}

// ===========================================================================
/**
 * @function    qusb_dci_init
 * 
 * @brief   This function will be called by the DCD to initialize the core
 * 
 * @param   * dcd_context_ptr   - DCD context pointer. It is not NULL 
 *  
 * @param   core_id             - Core ID of the controller
 *  
 *  
 * @param   *callback_table_ptr - Function Call back Pointer. It is not NULL 
 * 
 * @param 	*callback_table_ptr - Ch9 callback
 * 
 * @return  None.
 * 
 */
// ===========================================================================
uint32 qusb_dci_init(uint32 core_id, struct qusb_dcd_dsc_device* dcd_context_ptr, 
                       qusb_dci_callback_table_t *callback_table_ptr)
{
   return DCI_OK;
}

// ===========================================================================
/**
 * @function    qusb_dci_shutdown
 * 
 * @brief       This function cancel any active transfer.
 * 
 * @note        This function is called at the end of the DLOAD program before jumping to ARMPROG.
 *              Since ep#1 rx is still active , it need to be canceled.
 * 
 * @param 
 *
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qusb_dci_cancel_transfer
 * 
 * @ref SNPS spec.
 *      Chapter 8.1.8  Device Initiated disconnect
 */
// ===========================================================================
uint32 qusb_dci_shutdown(void)
{
  return(DCI_OK);
}

//============================================================================
/**
 * @function qusb_dci_poll_events
 * @brief   Check for any pending events (interrupts are disabled) and call the relevant callbacks.
 *
 *   When an event occurs within the core, hardware checks the enable bit that corresponds to the event 
 *   to decide whether the event will be written to the Event Buffer or not. 
 *   The Event Buffer contains one of the following types of information:
 *      -  Endpoint-Specific Event (DEPEVT)
 *      -  Device-Specific Event (DEVT)
 *   
 *   Number of events ( interrupters)  -  1
 *   Event buffer size                         -  1024 bytes
 *   Number entries in each buffer      -  256 (each event is 4 bytes long ) 
 *   initial event count                        -  0
 *   
 *   Logic : 
 *   check for the event count. If the event count is greater than zero, check for the event type ( EP or Dev ) and handle 
 *   the corresponding event. Update the HW about the number of events ( in bytes) SW has processed. 
 *   Move to the next 4 bytes (i.e. next event) in the current event and repeat it until there are no pending events.
 *               
 * @return  none.
 *
 * @ref SNPS spec 
 *      Chapter 8.2.2 , 6.2.7.3 ,  6.2.8
 *
 */
//============================================================================
void qusb_dci_poll_events(void)
{
  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_set_address
 * 
 * @brief       This function set the device address
 * 
 * @details     
 *       
 * @param address
 *      New Device Address.
 * 
 * @return  none.
 *
 * @ref 
 */
// ===========================================================================
void qusb_dci_set_address(uint8 address)
{
  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_handle_set_select
 * 
 * @brief   This function will be used for specifying the system exit latency
 *          in USB. Currently Power management is not supported in SBL driver
 *          so it is safe to set all these values to 0x0. 
 * 
 * @param   None
 * 
 * @return  None
 * 
 */
// ===========================================================================
void qusb_dci_handle_set_select(void)
{
  return;	
}

void qusb_dci_handle_u1_enable(struct qusb_dcd_dsc_device* dev, boolean enable)
{
  return;
}

void qusb_dci_handle_u2_enable(struct qusb_dcd_dsc_device* dev, boolean enable)
{
  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_stall_endpoint
 * 
 * @brief       This function stall an endpoint
 * 
 * @details     
 *      There are two occasions where the device controller may need to return to the host a STALL.
 *  
 *      The first occasion is the FUNCTIONAL stall, 
 *      which is a condition set by the DCD as described in the USB 2.0 device framework (chapter 9). 
 *      A functional stall is only used on noncontrol endpoints and can be enabled in the device
 *      controller by setting the endpoint stall bit in the ENDPTCTRLx register 
 *      associated with the given endpoint and the given direction. 
 *      
 *      In a functional stall condition, the device controller will continue to return STALL responses to all
 *      transactions occurring on the respective endpoint and direction until the endpoint stall bit is cleared by the DCD.
 *      
 *      A PROTOCOL stall, unlike a function stall, is used on control endpoints is automatically cleared by the device
 *      controller at the start of a new control transaction (setup phase). 
 *      When enabling a protocol stall, the DCD should enable the stall bits (both directions) as a pair. 
 *
 *  
 * @param ep    
 *          endpoint number
 * 
 * @param dir    
 *          endpoint direction (1=TX=IN,0=RX=OUT)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @ref SNPS spec 
 *   Chapter 8.2.7 Page 579
 *   Chapter 6.3.2.5.4 Page 512
 */
// ===========================================================================
uint32 qusb_dci_stall_endpoint(uint8 ep, uint8 dir)
{
  uint32 status=DCI_OK;
  return(status);
}

// ===========================================================================
/**
 * @function    qusb_dci_unstall_endpoint
 * 
 * @brief       This function unstall a stalled endpoint
 * 
 * @details
 *      There are two occasions where the device controller may need to return to the host a STALL.
 *  
 *      The first occasion is the FUNCTIONAL stall, 
 *      which is a condition set by the DCD as described in the USB 2.0 device framework (chapter 9). 
 *      A functional stall is only used on noncontrol endpoints and can be enabled in the device
 *      controller by setting the endpoint stall bit in the ENDPTCTRLx register 
 *      associated with the given endpoint and the given direction. 
 *      
 *      In a functional stall condition, the device controller will continue to return STALL responses to all
 *      transactions occurring on the respective endpoint and direction until the endpoint stall bit is cleared by the DCD.
 *      
 *      A PROTOCOL stall, unlike a function stall, is used on control endpoints is automatically cleared by the device
 *      controller at the start of a new control transaction (setup phase). 
 *      When enabling a protocol stall, the DCD should enable the stall bits (both directions) as a pair. 
 *      A single write to the ENDPTCTRLx register can ensure that both stall bits are set at the same instant.
 *  
 * @note : Any write to the ENDPTCTRLx register during operational mode must preserve the endpoint type field (i.e. perform a readmodifywrite).
 *  
 * @note : Reset Data-Toggle as mentioned in Chapter 8.3.3.1 Data Toggle Reset
 *    
 *
 * @param ep    
 *          endpoint number
 * 
 * @param dir    
 *          endpoint direction (1=TX=IN,0=RX=OUT)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qusb_dci_stall_endpoint
 *
 * @ref SNPS spec 
 *   Chapter 8.2.7 Page 579
 *   Chapter 6.3.2.5.4 Page 512
 */
// ===========================================================================
uint32 qusb_dci_unstall_endpoint(uint8 ep, uint8 dir)
{
  uint32 status=DCI_OK;
  return(status);
}

//============================================================================
/**
 * @function    qusb_get_shared_imem_address
 * 
 * @brief   	Gets the shared imem address based on the hardware version
 * 
 * @note    	None          
 *  
 * @param   	None
 *
 * @return 		uint32 A 32-Bit Address
 *
 * @ref 
 *
 */
// ===========================================================================
uint64 qusb_get_shared_imem_address()
{
  return QUSB_DLOAD_INFO_ADDR_IN_IMEM;
}

// ===========================================================================
/**
 * @function    qusb_dci_config_endpoint
 * 
 * @brief       This function configures the endpoint
 * 
 * @details     
 * 
 * @note        This function should be called on "Set Configuration" USB Standard Command.
 * 
 *
 * @param ep    
 *          endpoint number
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * @param ep_type 
 *          endpoint type:    
 *          QUSB_EP_TYPE_CONTROL      0
 *          QUSB_EP_TYPE_ISO          1
 *          QUSB_EP_TYPE_BULK         2    
 *          QUSB_EP_TYPE_INTERRUPT    3
 * @param max_packet_length    
 *          maximum packet length - depends on the connection speed and the Hardware FIFOs.
 * @param zero_length_transfer    
 *          if set to TRUE, and the transfer length is a multiply of the max_packet_length
 *          then a zero lenfth packet is added.
 * 
 * @return None
 * 
 * @ref SNPS spec
 *       
 */
// ===========================================================================
void qusb_dci_config_endpoint(uint8 ep, uint8 dir, 
                                  uint8 ep_type, uint16 max_packet_length, 
                                  boolean zero_length_transfer)
{
  return;
}

// ===========================================================================
/**
 * @function    qusb_dci_handle_u1_u2_enable
 * 
 * @brief   This function will enable link transition to U1 or U2 state.
 * 
 * @param   Device context.
 * @param   Type of event.
 * @param   Enable/Disable.
 * 
 * @return  None.
 *
 * 
 */
// ===========================================================================
void qusb_dci_handle_u1_u2_enable(struct qusb_dcd_dsc_device* dev, qusb_dci_u1_u2_event_type event, boolean enable)
{
  return;
}