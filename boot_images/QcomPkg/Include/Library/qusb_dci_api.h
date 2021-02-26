#ifndef QUSB_DCI_API_H
#define QUSB_DCI_API_H

/*=======================================================================*//**
 * @file        qusb_dci_api.h
 * @author:     amirs
 * @date        14-July-2008
 *
 * @brief       QUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) exported API.
 *
 * @details     The QUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) 
 *              acts like a HAL (Hardware Abtraction Layer) for Chip-Idea USB Hardware.
 *              The DCI should supply generic API that hides USB Hardware registers access from the upper layers.
 *              It should not be aware of Chapter-9 or specific function drivers (Mass-Storage , CDC , HID etc).
 *              It should be simple without any management algorithms.
 *              
 *              The USB Device Core Driver (DCD) shall use the DCI to implement
 *              the USB protocol chapter 9 enumeration,
 *              and handling of special BUS signaling : Reset / Suspend / Resume.
 * 
 * @note        This DCI is implementing the ChipIdea hardware core.
 *              The DCI depends on the USB Core (a.k.a LINK) connection to the PHY:
 *              When a 3-wired PHY is connected (Full-Speed only) then the LINK
 *              can not control the VBUS D+ wire via the Run/Stop bit.
 * 
 * @ref         ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *
 *              Copyright (c) 2008-2015, 2017 Qualcomm Technologies Incorporated.
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
// when       who      what, where, why
// --------   ---      ----------------------------------------------------------
// 02/01/17   pm       Added USBSharedLib
// 09/17/13   shreyasr Added support for SNPS controller 
// 04/27/11   dpatnaik Added support for 8960 FLCB
// 04/27/11   dpatnaik Merged important FLCB related changes from 2.2 branch
// 08/30/10   dpatnaik Port QUSB to RPM, add ULPI based charger detection
// 07/14/08   amirs    First Draft
// 08/08/08   amirs    First working DLOAD process
// 08/11/08   amirs    Add files and functions headers according to coding standard.
// 
// ===========================================================================

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "comdef.h"         // basic types as byte word uint32 etc
#include "qusb_dcd.h"       // URB structure
#include "qusb_dci_private.h"
#include "UsbSharedLib.h"


//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------
#define QUSB_SINGLE_BOOT_ENUM_MAGIC_DL          QUSB_SINGLE_ENUM_COOKIE
#define QUSB_SINGLE_ENUM_MAGIC_RAM_OFFSET       QUSB_SINGLE_BOOT_ENUM_ADDR

// Endpoint Direction
#define QUSB_EP_DIR_RX   0  // OUT Token
#define QUSB_EP_DIR_TX   1  // IN Token

// Endpoint Type  see Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
#define QUSB_EP_TYPE_CONTROL      0
#define QUSB_EP_TYPE_ISO          1
#define QUSB_EP_TYPE_BULK         2    
#define QUSB_EP_TYPE_INTERRUPT    3

// DCI Error Codes
#define QUSB_OK                    0

// Valid Resource Index returned from HW, can range from [0, 0x7F], so we can only use 0xFF as a
// special value for resource index not being set
#define QUSB_RSC_IDX_NOT_SET                    (0xFF)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
#define MAX_SYSTEM_EXIT_LATENCY_US 			125

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

struct qusb_dcd_dsc_device;

typedef struct
{
  void (*usb_reset_callback_fn)(struct qusb_dcd_dsc_device* dcd_context_ptr);
  void (*setup_callback_fn)(struct qusb_dcd_dsc_device* dcd_context_ptr, void *buffer_ptr);
  void (*port_status_changed_callback_fn)(struct qusb_dcd_dsc_device* dcd_context_ptr);
  void (*usb_disconnect_callback_fn)(struct qusb_dcd_dsc_device* dcd_context_ptr);
  void (*usb_suspend_callback_fn)(struct qusb_dcd_dsc_device* dcd_context_ptr);
  //=================================================================
  // NOTE: Trnasfer complete ok/error callback is specified in the URB.
  //=================================================================
} qusb_dci_callback_table_t;

//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------
//============================================================================
/**
 * @function  qusb_dci_get_hw_revision
 *
 * @brief Reads HW revision from TCSR register
 *
 *
 * @param None.
 *
 * @return HW revision type.
 *
 */
//============================================================================
qusb_hw_rev qusb_dci_get_hw_rev_major(void);

//============================================================================
/**
 * @function  qusb_dci_dcache_flush
 *
 * @brief Flush Data-Cache
 *
 *
 * @param address
 * @param length
 *
 * @return none
 *
 */
//============================================================================
void qusb_dci_dcache_flush(void* addr, unsigned int length);

//============================================================================
/**
 * @function  qusb_dci_dcache_inval
 *
 * @brief Flush Data-Cache
 *
 *
 * @param address
 * @param length
 *
 * @return none
 *
 */
//============================================================================
void qusb_dci_dcache_inval(void* addr, unsigned int length);
// ===========================================================================
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
uint64 qusb_get_shared_imem_address(void);

// ===========================================================================
/**
 * @function    dci_is_usb_cable_connected
 * 
 * @brief   	This API determines whether USB cable is still connected
 * 
 * @note    	This API will access pmic to get the USB connection status        
 *  
 * @param   	TRUE signifies it is still connected.
 *
 */
// ===========================================================================
boolean dci_is_usb_cable_connected(void);

// ===========================================================================
/**
 * QUSB SNPS Controller for USB3.0 
 *  
 * Hardware abstraction layer for SNPS controller 
 */
// ===========================================================================

// ===========================================================================
/**
 * @function    qusb_dci_enable_usb30_clocks
 * 
 * @brief   This function will be used to turn ON the USB3.0 clocks
 * 
 * @param   None
 * 
 * @return  TRUE or FALSE depending on success or failure.
 * 
 */
// ===========================================================================
boolean qusb_dci_enable_usb30_clocks(void);

// ===========================================================================
/**
 * @function    qusb_dci_get_hardware_id
 * 
 * @brief   This function will be used for getting the SNPS h/w id.
 * 
 * @param   None
 * 
 * @return  uint32 - hardware id. 
 * 
 */
// ===========================================================================
uint32 qusb_dci_get_hardware_id(void);

// ===========================================================================
/**
 * @function    qusb_dci_init
 * 
 * @brief   This function will be called by the DCD to initialize the core
 * 
 * @param   * dcd_context_ptr   - DCD context pointer 
 *  
 * @param   core_id             - Core ID of the controller
 *  
 *  
 * @param   *callback_table_ptr - Function Call back Pointer 
 * 
 * @param 	*callback_table_ptr - Ch9 callback
 * 
 * @return  None.
 * 
 */
// ===========================================================================
uint32 qusb_dci_init(uint32 core_id, struct qusb_dcd_dsc_device* dcd_context_ptr, 
                       qusb_dci_callback_table_t *callback_table_ptr);

// ===========================================================================
/**
 * @function    qusb_dci_handle_suspend_entry
 * 
 * @brief       This function handles USB suspend entry event
 * 
 * @details     
 *  This event provides a notification that the link has gone to a suspend state (L2, U3, or L1). This event is
 *  generated when hibernation mode is disabled. The existing Link State Change event (3) provides the same
 *  information, but is generated for every link state change. 
 *
 *  Note: Enable suspend event, Sec. 6.3.1.3 ( set bit 6,  DEVTEN registers )
 * 
 * @param       None    
 *
 * @return  none.
 *
 * @see qusb_dci_handle_dev_event
 *
 * @ref SNPS spec 
 *   Sec. 6.3.1.3 , 
 *   Table 6-60 ( Pg. 500) - DSTS ( bits 21-18)
 *   
 */
// ===========================================================================
void qusb_dci_handle_suspend_entry(void);

//============================================================================
/**
 * @function qusb_dci_ep0_start_xfer
 * @brief   Start a new ep0 transfer for receiving setup.
 *
 * @param qusb_dcd_dsc_device, pointer to device structure which is not NULL    
 *
 * @return  None.
 *
 * @ref SNPS spec 
 *
 */
//============================================================================
void qusb_dci_ep0_start_xfer(struct qusb_dcd_dsc_device *dcd_dev);

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
void qusb_dci_set_run_stop(boolean set);

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
 *   Number of events ( interrupters)  -  2 
 *   Event buffer size                         -  4096 bytes
 *   Number entries in each buffer      -  1024 bytes  (each event is 4 bytes long ) 
 *   initial event count                        -  0
 *   
 *   Logic : 
 *   Start with the First event Buffer and check fo the event count. if there are no events in the current buffer
 *   move to the next buffer. If the event count is greater than zero, check for the event type ( EP or Dev ) and handle 
 *   the corresponding event. Update the HW about the number of events ( in bytes) SW has processed. 
 *   Move to the next 4 bytes in the current event and repeat it until there are no pending events.
 *               
 * @return  none.
 *
 * @ref SNPS spec 
 *      Chapter 8.2.2 , 6.2.7.3 ,  6.2.8
 *
 */
//============================================================================
void qusb_dci_poll_events(void);

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
void qusb_dci_set_address(uint8 address);
// ===========================================================================
/**
 * @function    qusb_dci_get_address
 * 
 * @brief   Read adress of the device from register.
 * 
 * @note    Function to read set address from device register.
 *  
 * @param   None.
 *
 */
// ===========================================================================
uint8 qusb_dci_get_address(void);
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
uint32 qusb_dci_stall_endpoint(uint8 ep, uint8 dir);

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
uint32 qusb_dci_unstall_endpoint(uint8 ep, uint8 dir);

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
 * @return none
 * 
 * @ref SNPS spec
 *       
 */
// ===========================================================================
void qusb_dci_config_endpoint(uint8 ep, uint8 dir, 
                                  uint8 ep_type, uint16 max_packet_length, 
                                  boolean zero_length_transfer); 
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
uint32 qusb_dci_unconfig_endpoint( uint8 ep, uint8 dir);

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
uint32 qusb_dci_transfer_request(qusb_urb_t* urb_ptr);

// ===========================================================================
/**
 * @function    qusb_dci_cancel_ep0
 * 
 * @brief       This function terminates an active transfers on EP0
 * 
 * @details     
 * 
 * @see qusb_dci_shutdown
 * 
 * @ref SNPS Spec
 */
// ===========================================================================
uint32 qusb_dci_cancel_ep0(void);

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
uint32 qusb_dci_cancel_transfer(uint8 ep, uint8 dir);

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
uint32 qusb_dci_shutdown(void);

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
uint32 qusb_dci_get_speed(void);

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
void qusb_dci_set_max_packet_size(uint32 max_packet_size);

// ===========================================================================
/**
 * @function    qusb_ss_phy_init
 * 
 * @brief   API used for initializing the SS PHY 
 *  
 * @details This is used for initializing the SNPS controller PHY and QMP PHY 
 * 			Configure the QMP PHY as per Table 4-1 of the QMP PHY HPG 
 * 
 * @param   None.
 * 
 * @return  TRUE if successful else FALSE.
 * 
 */
// ===========================================================================
boolean qusb_ss_phy_init(void);
// ===========================================================================
/**
 * @function    qusb_dci_enable_usb30_power
 * 
 * @brief   This function enables the power domain for SNPS controller
 *
 * @param   TRUE: It enables the s/w Power Collapse 
 *          FALSE:  IT disables the s/w Power Collapse
 * 
 * @details  Used for enabling the power domain for SNPS controller
 * 
 * @return  TRUE or FALSE depending on success or failure.
 * 
 */
// ===========================================================================
void qusb_dci_enable_usb30_power_collapse(boolean enable);
// ===========================================================================
/**
 * @function    qusb_dci_is_usb30_sw_collapsed
 * 
 * @brief   This function reads back if  the power domain for SNPS 
            controller is power collapsed.
 * 
 * @details Used fr cheking if USB core was initialized in PBL.
 *          If not then single enumeration cannot continue, 
 *          USB core needs to be re-enumerated.
 * 
 * @return  TRUE or FALSE depending on status of SW collapse bit.
 * 
 */
// ===========================================================================
boolean qusb_dci_is_usb30_sw_collapsed(void);
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
boolean qusb_dci_disable_usb30_clocks(void);
// ===========================================================================
/**
 * @function    qusb_dci_handle_set_select
 * 
 * @brief   This function will be used for generating the delay
 * 
 * @param   None
 * 
 * @return  None
 * 
 */
// ===========================================================================
void qusb_dci_handle_set_select(void);
//============================================================================
/**
 * @function  qusb_dci_core_init
 *
 * @brief
 *    This function initializes the USB device controller according to the SNPS spec:
 *    1.
 *  @Note : Endpoint 0 is designed as a control endpoint only and does not need to be
 *      configured using ENDPTCTRL0 register.
 *      It is also not necessary to initially prime Endpoint 0
 *      because the first packet received will always be a setup packet. 
 *      The contents of the first setup packet will require a response
 *      in accordance with USB device framework (Chapter 9) command set.
 *
 * @param dcd_context_ptr    
 *              A pointer to a DCD initernal context
 *
 * @return status
 *              0 = ok , otherwise error.
 *
 * @ref SNPS spec "":
 *      Chapter 8.1.1 
 *      HPG 4.4. 2 ,  steps 18 -19
 *      
 *
 */
//============================================================================
void qusb_dci_core_init(struct qusb_dcd_dsc_device* dev);
// ===========================================================================
/**
 * @function    qusb_dci_skip_enumeration
 * 
 * @brief   checks for whether Enumeration can be skipped
 * 
 * @note   
 *  
 * @param   
 *
 * @return 
 *
 * @ref 
 *
 */
// ===========================================================================
boolean qusb_dci_skip_enumeration(void);
// ===========================================================================
/**
 * @function    qusb_dci_handle_test_mode
 * 
 * @brief   Enables the USB test mode.
 * 
 * @note   
 *  
 * @param   
 *
 * @return 
 *
 * @ref 
 *
 */
// ===========================================================================
void qusb_dci_handle_test_mode(struct qusb_dcd_dsc_device* dev);
// ===========================================================================
/**
 * @function    qusb_dci_enable_vbus_valid
 * 
 * @brief       API used to enable VBUS using s/w control
 * 
 * @param   None.
 * 
 * @return  None.
 * 
 */
// ===========================================================================
void qusb_dci_enable_vbus_valid(qusb_max_speed_required_t speed_required);
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
void qusb_dci_deinit_hs_phy(void);
// ===========================================================================
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
void qusb_timer_start(uint32 timer_value);
//============================================================================
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
void qusb_dci_write_enum_cookie(uint32 val);
// ===========================================================================
/**
* @function  qusb_timer_stop
*
* @brief Call the API to stop the timer 
*
* @Note : These timers are based on QTIMERS in APPS. This is a count down timer
*
* @param  None 
*
* @return None
*
*/
//============================================================================
void qusb_timer_stop(void);
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
boolean qusb_timer_check_for_expiry(void);
// ===========================================================================
/**
 * @function    qusb_dci_read_enum_cookie
 * 
 * @brief   API is used to read the single enumeration Cookie
 * 
 * @note    This API makes a read from QSRAM register to get the 
 *          Single Enumeration Cookie 
 *  
 * @param   uint32 : The Cookie Value to be read 
 *
 */
// ===========================================================================
void qusb_dci_read_enum_cookie(uint32 *cookie); 
// ===========================================================================
/**
 * @function    qusb_dci_write_evt_buff_cur_pos_cookie
 * 
 * @brief   API is used to write current position of the event buffer
 * 
 * @note    This API makes a write to QSRAM register to save 
 *          current position of event buffer to a Cookie.
 *  
 * @param   uint32 : Cookie Value to write 
 *
 */
// ===========================================================================
void qusb_dci_write_evt_buff_cur_pos_cookie(uint32 val);
// ===========================================================================
/**
 * @function    qusb_dci_read_evt_buff_cur_pos_cookie
 * 
 * @brief   API is used to read current position of the event buffer
 * 
 * @note    This API makes a read from QSRAM register to get the 
 *          current position of event buffer from a Cookie.
 *  
 * @param   uint32 : The Cookie Value to be read 
 *
 */
// ===========================================================================
void qusb_dci_read_evt_buff_cur_pos_cookie(uint32 *cookie);
//============================================================================
/**
 * @function  qusb_dci_event_buffer_restore
 *
 * @brief
 *    This function restores the USB device controller event buffer.
 *    To be used during single enumeration.
 *    
 * @param None
 *
 * @return None
 *
 *      
 *
 */
//============================================================================
void qusb_dci_event_buffer_restore(void);
// ===========================================================================
/**
 * @function    qusb_dci_check_ss_phy_present
 * 
 * @brief   This function will check if 0.9V analog supply to SS PHY is on.
 * 
 * @param   None
 * 
 * @return  TRUE:  If analog supply to SS PHY on.
 *          FALSE: If analog supply to SS PHY off.
 * 
 */
// ===========================================================================
boolean qusb_dci_ss_phy_present(void);
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
void qusb_dci_handle_u1_u2_enable(struct qusb_dcd_dsc_device* dev, qusb_dci_u1_u2_event_type event, boolean enable);


// ===========================================================================
/**
 * @function    qusb_dci_typec_lane_detect
 *
 * @brief        This API will toggle the GPIO for selecting right SS PHY lane based on CC logic.
 *
 *               
 *
 * @param   None
 *
 * @return  None
 *
 */
// ===========================================================================
void qusb_dci_typec_lane_detect(void);

//============================================================================
/**
* @function  usb_dci_get_chip_serial_number
*
* @brief Call API to uint32 unique MSM serial string
*
* @Note : None
*
* @param  None 
*
* @return uint32: Chip serial number
*
*/
//============================================================================
uint32 qusb_dci_get_chip_serial_number(void);

//============================================================================
/** 
 * API is used to add chip serial number to the string and return 
 * Product String Descriptor
 * 
 * @param pointer to product string
 *              Pointer to the buffer to be updated with Product string desc
 *              Length of the buffer
 *
 * @return Appended string length
 */ 
 //============================================================================
uint32 qusb_get_product_str_desc_with_chip_serial(char* product_str, char* prod_str_desc, uint32 desc_len);


#endif // QUSB_DCI_API_H
