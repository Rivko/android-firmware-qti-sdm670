/** @file UsbfnLib.h
 *
 *   @brief       QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface) exported API.
 *
 *   @details     The QHSUSB (Qualcomm High-Speed USB) DCI (Device-Controller-Interface)
 *                acts like a HAL (Hardware Abtraction Layer) for Chip-Idea USB Hardware.
 *                The DCI should supply generic API that hides USB Hardware registers access from the upper layers.
 *                It should not be aware of Chapter-9 or specific function drivers (Mass-Storage , CDC , HID etc).
 *                It should be simple without any management algorithms.
 *              
 *                The USB Device Core Driver (DCD) shall use the DCI to implement
 *                the USB protocol chapter 9 enumeration,
 *                and handling of special BUS signaling : Reset / Suspend / Resume.
 * 
 *   @note        This DCI is implementing the ChipIdea hardware core.
 *                The DCI depends on the USB Core (a.k.a LINK) connection to the PHY:
 *                When a 3-wired PHY is connected (Full-Speed only) then the LINK
 *                can not control the VBUS D+ wire via the Run/Stop bit.
 * 
 *   @ref         ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *
 * Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/24/11   cching  Port QHSUSB to UEFI
 08/17/11   sbryksin AHB/GEN_CONFIG settings changes for 8960
=============================================================================*/

#ifndef USB_FN_LIB_H
#define USB_FN_LIB_H

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------
#include "UsbUrb.h"     // URB structure
#include "com_dtypes.h"         // basic types as byte word uint32 etc



//----------------------------------------------------------------------------
// Preprocessor Definitions and Constants
//----------------------------------------------------------------------------

#ifdef FEATURE_QHSUSB_HDLC_CDCACM 
    #define FEATURE_QHSUSB_USE_SCRATCH_ON_TIMERS
#endif // FEATURE_QHSUSB_HDLC_CDCACM

#define QHSUSB_SINGLE_BOOT_ENUM_MAGIC  0xABCDEF

#define QHUSB_MAX_ENDPOINTS 3

// Endpoint Direction
#define QHSUSB_EP_DIR_RX   0  // OUT Token
#define QHSUSB_EP_DIR_TX   1  // IN Token

// Endpoint Type  see Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
#define QHSUSB_EP_TYPE_CONTROL      0
#define QHSUSB_EP_TYPE_ISO          1
#define QHSUSB_EP_TYPE_BULK         2    
#define QHSUSB_EP_TYPE_INTERRUPT    3

// Device Speed
// REference 3.6.16 PORTSCx field PSPD
#define QHSUSB_DEVICE_SPEED_FULL        0
#define QHSUSB_DEVICE_SPEED_LOW         1
#define QHSUSB_DEVICE_SPEED_HIGH        2
#define QHSUSB_DEVICE_SPEED_UNKNOWN     3

// DCI Error Codes
#define QHSUSB_OK                    0

#define QHSUSB_ERR_NOT_SUPPORTED     (1)
#define QHSUSB_ERR_HW_FAILURE        (2)
#define QHSUSB_ERR_BAD_PARAM         (3)
#define QHSUSB_ERR_URB_SIGNATURE     (4)
#define QHSUSB_ERR_SETUP_PENDING     (5)
#define QHSUSB_ERR_TIMER_IS_RUNNING  (6)
#define QHSUSB_ERR_INVALID_OPERATION (7)
#define QHSUSB_ERR_OUT_OF_RESOURCES  (8)

// Timers
// Timer#0 shall be free-run (repaet mode) - restart countdown when reaching zero.
// Timer#1 shall be one-shot (one-shot mode) - stop countdown when reaching zero.
#define QHSUSB_TIMER_0_FREE_RUN  0
#define QHSUSB_TIMER_1_ONE_SHOT  1
#define QHSUSB_TIMER_MAX_USEC   0xFFFFFF   

// Core-Id 
#define QHSUSB_CORE_ID_FS       0x00
#define QHSUSB_CORE_ID_HS       0x01
#define QHSUSB_CORE_ID_FSIC     0x02
#define QHSUSB_CORE_ID_HSIC     0x03

/* Suspend Current for Standard Downstream Port */
#define QHSUSB_CHG_ISUSP              (0)

/* Iunit in BC 1.1: Unit load current (mA).  The is typically amount of current 
 * allowed on USB when USB is not configured */
#define QHSUSB_CHG_IUNIT              (100)

/* Maximum current for Standard Downstream Port */
#define QHSUSB_CHG_IDEV_SDP           (500)

/* Idev_dchg in BC 1.1: Portable Device Current from Dedicated Charging Port */
#define QHSUSB_CHG_IDEV_DCHG          (1500)

/* Idev_cdp_hs in BC 1.1: Portable Device Current (mA) from Charging
 * Downstream Port at High Speed */
#define QHSUSB_CHG_IDEV_CDP_HS        (1500)

/* Idev_cdp_lfs in BC 1.1: Portable Device Current (mA) from Charging 
 * Downstream Port at Low and Full Speed */
#define QHSUSB_CHG_IDEV_CDP_LFS       (1500)

/* Idev_cdp_chrp in BC 1.1: Portable Device Current (mA) from Charging 
 * Downstream Port during chirp */
#define QHSUSB_CHG_IDEV_CDP_CHRP      (560)

/* Spoof disconnect delay in ms */
#define QHSUSB_DISCONNECT_DELAY_MS    (1)

/* Charger detection loop count */
#define QHSUSB_CHG_DETECTION_LOOP_COUNT (100)

/* ULPI loop retry count */
#define QHSUSB_UPLI_LOOP_COUNT          (500)
/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
#define LINESTATE_POLL_COUNT 6

typedef enum
{
    QHSUSB_CHG_ST_START,
    QHSUSB_CHG_ST_BSESS_VALID,
    QHSUSB_CHG_ST_WAIT_FOR_DCD,
    QHSUSB_CHG_ST_WAIT_FOR_DP_LOW,
    QHSUSB_CHG_ST_SE1_ON_BSESS_VALID,
    QHSUSB_CHG_ST_DCD_SUCCESS,
    QHSUSB_CHG_ST_DCD_FAILURE,
    QHSUSB_CHG_ST_CHARGING_PORT_DETECTED,
    QHSUSB_CHG_ST_ACA_PORT_DETECTED,
    QHSUSB_CHG_ST_SDP,
    QHSUSB_CHG_ST_CDP,
    QHSUSB_CHG_ST_DCP,
    QHSUSB_CHG_ST_ACA_DOCK,
    QHSUSB_CHG_ST_ACA_A,
    QHSUSB_CHG_ST_ACA_B,
    QHSUSB_CHG_ST_ACA_C,
    QHSUSB_CHG_ST_INVALID_CHARGER, 
    QHSUSB_CHG_ST_DONE,
    QHSUSB_CHG_ST_END
} qhsusb_chg_state_type;

// Refer to Battery Charging Specification Revision 1.2
// for the definition of the following port types.

typedef enum
{
    QHSUSB_CHG_PORT_SDP,
    QHSUSB_CHG_PORT_CDP,
    QHSUSB_CHG_PORT_DCP,
    QHSUSB_CHG_PORT_ACA_DOCK,
    QHSUSB_CHG_PORT_ACA_A,
    QHSUSB_CHG_PORT_ACA_B,
    QHSUSB_CHG_PORT_ACA_C,
    QHSUSB_CHG_PORT_INVALID, 
    QHSUSB_CHG_PORT_UNKNOWN 
} qhsusb_chg_port_type;

typedef enum
{
    QHSUSB_CHG_LS_SE0,
    QHSUSB_CHG_LS_K,
    QHSUSB_CHG_LS_J,
    QHSUSB_CHG_LS_SE1,
} qhsusb_line_state_type;

typedef enum
{
    QHSUSB_CHG_RID_FLOAT,
    QHSUSB_CHG_RID_GND,
    QHSUSB_CHG_RID_A,
    QHSUSB_CHG_RID_B,
    QHSUSB_CHG_RID_C
} qhsusb_id_pin_state;

//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

struct qhsusb_dcd_dsc_device;

typedef struct
{
    void (*usb_reset_callback_fn)(struct qhsusb_dcd_dsc_device* dcd_context_ptr);
    void (*setup_callback_fn)(struct qhsusb_dcd_dsc_device* dcd_context_ptr, void *buffer_ptr);
    void (*port_status_changed_callback_fn)(struct qhsusb_dcd_dsc_device* dcd_context_ptr);
    void (*usb_cable_status_change_callback_fn)(struct qhsusb_dcd_dsc_device* dcd_context_ptr, boolean is_cable_attached);
    //=================================================================
    // NOTE: Trnasfer complete ok/error callback is specified in the URB.
    //=================================================================
} qhsusb_dci_callback_table_t;

//----------------------------------------------------------------------------
// Function Declarations and Documentation
//----------------------------------------------------------------------------

//============================================================================

/**
 * @function  qhsusb_dci_init
 *
 * @brief
 *    This function initializes the USB device controller according to the ChipIdea spec:
 *    1. Set Controller Mode in the USBMODE register to device mode.
 *    2. Allocate and Initialize device queue heads in system memory.
 *    3. Configure ENDPOINTLISTADDR Pointer.
 *    4. Enable the microprocessor interrupt associated with the USBHS
 *    5. Set Run/Stop bit to Run Mode.
 *
 *  @Note : Endpoint 0 is designed as a control endpoint only and does not need to be
 *      configured using ENDPTCTRL0 register.
 *      It is also not necessary to initially prime Endpoint 0
 *      because the first packet received will always be a setup packet. 
 *      The contents of the first setup packet will require a response
 *      in accordance with USB device framework (Chapter 9) command set.
 *
 * @param callback_table_ptr
 *              A pointer to a callback table
 * @return status
 *              0 = ok , otherwise error.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 8.1 "Device Controller Initialization"
 *      Chapter 8.4.3.1.1 Setup Packet Handling (Pre2.3 hardware)
 *
 */
//============================================================================
uint32 qhsusb_dci_init(uint32 core_id, struct qhsusb_dcd_dsc_device* dcd_context_ptr, 
                       qhsusb_dci_callback_table_t *callback_table_ptr);

//============================================================================

/**
 * @function  qhsusb_dci_init_platform_specific
 *
 * @brief Platform specific initialization
 *
 *
 * @param core_id
 *              Core ID
 * @return status
 *              0 = ok , otherwise error.
 *
 */
//============================================================================
void qhsusb_dci_init_platform_specific(uint32 core_id);


//============================================================================
/**
 * @function qhsusb_dci_poll_events
 * @brief   Check for any pending events (interrupts are disabled) and call the relevant callbacks.
 *
 * @param   none.
 *
 * @return  none.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 8.4.3.1.2 Setup Packet Handling (2.3 hardware and later)
 *      Chapter 3.6.2 USBSTS
 *
 */
//============================================================================
void qhsusb_dci_poll_events(uint32 core_id);

// ===========================================================================
/**
 * @function    qhsusb_dci_set_address
 * 
 * @brief       This function set the device address
 * 
 * @details     
 *      USBADRA Device Address Advance. Default=0. 
 *      When this bit is ‘0’, any writes to USBADR are instantaneous. 
 *      When this bit is written to a ‘1’ at the same time or before USBADR is written, 
 *      the write to the USBADR field is staged and held in a hidden register. 
 *      After an IN occurs on endpoint 0 and is ACKed, USBADR will be loaded from the holding register.
 *      
 *      Hardware will automatically clear this bit on the following conditions:
 *      1) IN is ACKed to endpoint 0. (USBADR is updated from staging register).
 *      2) OUT/SETUP occur to endpoint 0. (USBADR is not updated).
 *      3) Device Reset occurs (USBADR is reset to 0).
 *      
 *      Note: After the status phase of the SET_ADDRESS descriptor, the DCD has 2 ms to program the USBADR field. 
 *      This mechanism will ensure this specification is met when the DCD can not write of the device address within 2ms from the SET_ADDRESS status
 *      phase. If the DCD writes the USBADR with USBADRA=1 after the SET_ADDRESS data phase (before the prime of the status phase), 
 *      the USBADR will be programmed instantly at the correct time and meet the 2ms USB requirement
 * 
 * @param address
 *      New Device Address.
 * 
 * @return  none.
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 3.6.6.2 Device Controller (USB DEVICEADDR)
 */
// ===========================================================================
void qhsusb_dci_set_address(uint32 core_id, uint8 address);

// ===========================================================================
/**
 * @function    qhsusb_dci_shutdown
 * 
 * @brief       This function cancel any active transfer.
 * 
 * @note        This function is called at the end of the DLOAD program before jumping to ARMPROG.
 *              Since ep#1 rx is still active , it need to be canceled.
 * 
 * @param       None.
 * 
 * @return status
 *              
 * 
 * @see qhsusb_dci_cancel_transfer
 * 
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 8.6.5 Flushing/Depriming an Endpoint
 */
// ===========================================================================
EFI_STATUS qhsusb_dci_shutdown(uint32 core_id);

// ===========================================================================
/**
 * @function    qhsusb_dci_config_endpoint
 * 
 * @brief       This function configures the endpoint
 * 
 * @details     This function set the ENDPTCTRLX register and the relevant dQH.
 * 
 * @note        This function should be called on "Set Configuration" USB Standard Command.
 * 
 * @param ep    
 *          endpoint number
 * 
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * 
 * @param ep_type 
 *          endpoint type:    
 *          QHSUSB_EP_TYPE_CONTROL      0
 *          QHSUSB_EP_TYPE_ISO          1
 *          QHSUSB_EP_TYPE_BULK         2    
 *          QHSUSB_EP_TYPE_INTERRUPT    3
 * @param max_packet_length    
 *          maximum packet length - depends on the connection speed and the Hardware FIFOs.
 * @param zero_length_transfer    
 *          if set to TRUE, and the transfer length is a multiply of the max_packet_length
 *          then a zero lenfth packet is added.
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qhsusb_dci_unconfig_endpoint
 * 
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *        Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *        Chapter 8.3.1 Endpoint Initialization
 */
// ===========================================================================
uint32 qhsusb_dci_config_endpoint(uint32 core_id, uint8 endpoint, uint8 dir, 
                                  uint8 ep_type, uint16 max_packet_length, 
                                  boolean zero_length_transfer);

// ===========================================================================
/**
 * @function    qhsusb_dci_unconfig_endpoint
 * 
 * @brief       This function de-configures the endpoint
 * 
 * @details     This function set the dQH with Max-Packet-Size=0,
 *              which results bus error when the host send IN/OUT tokens
 * 
 * @note        This function should be called on "Set Configuration" USB Standard Command.
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
 * @see qhsusb_dci_config_endpoint
 * 
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *        Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *        Chapter 8.3.1 Endpoint Initialization
 */
// ===========================================================================
uint32 qhsusb_dci_unconfig_endpoint(uint32 core_id, uint8 ep, uint8 dir);


// ===========================================================================
/**
 * @function    qhsusb_dci_stall_endpoint
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
 *      A single write to the ENDPTCTRLx register can ensure that both stall bits are set at the same instant.
 *  
 * @note : Any write to the ENDPTCTRLx register during operational mode must preserve the endpoint type field (i.e. perform a readmodifywrite).
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
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *   Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *   Chapter 8.3.2 Stalling
 */
// ===========================================================================
uint32 qhsusb_dci_stall_endpoint(uint32 core_id, uint8 endpoint, uint8 dir);

// ===========================================================================
/**
 * @function    qhsusb_dci_unstall_endpoint
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
 * @param ep    
 *          endpoint number
 * 
 * @param dir    
 *          endpoint direction (1=TX=IN,0=RX=OUT)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qhsusb_dci_stall_endpoint
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *   Chapter 3.6.24 ENDPTCTRL1 – ENDPTCTRL15
 *   Chapter 8.3.2 Stalling
 *   Chapter 8.3.3.1 Data Toggle Reset
 */
// ===========================================================================
uint32 qhsusb_dci_unstall_endpoint(uint32 core_id, uint8 endpoint, uint8 dir);

boolean qhsusb_dci_get_endpoint_stall_state(uint32 core_id, uint8 ep, uint8 dir);

// ===========================================================================
/**
 * @function    qhsusb_dci_transfer_request
 * 
 * @brief       This function initate data transfer according to URB.
 * 
 * @details     
 *              This function calls transfer_data_request() and updtae the URB.
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
uint32 qhsusb_dci_transfer_request(qhsusb_urb_t* urb_ptr);

// ===========================================================================
/**
 * @function    qhsusb_dci_cancel_transfer
 * 
 * @brief       This function terminates an active transfers
 * 
 * @details     
 * 
 * @param ep    
 *          endpoint number
 * @param dir    
 *          endpoint direction (1=TX,0=RX)
 * 
 * @return status
 *              0 = ok , otherwise error.
 * 
 * @see qhsusb_dci_shutdown
 * 
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core".
 *      Chapter 8.6.5 Flushing/Depriming an Endpoint
 */
// ===========================================================================
uint32 qhsusb_dci_cancel_transfer(uint32 core_id,uint8 endpoint,uint8 dir);

//============================================================================
/**
 * @function  qhsusb_dci_get_speed
 *
 * @brief Report the USB connection speed.
 *
 * @Note : Although the device is high-speed, the Host might be full-speed.
 *          
 * @Note : The USB HW can be forced to Full-Speed.
 *
 * @param core_id    
 *
 * @return Speed
 *              QHSUSB_DEVICE_SPEED_FULL        
 *              QHSUSB_DEVICE_SPEED_LOW         
 *              QHSUSB_DEVICE_SPEED_HIGH        
 *              QHSUSB_DEVICE_SPEED_UNKNOWN     
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *       Chapter 3.6.16 PORTSCx
 *
 */
//============================================================================
uint32  qhsusb_dci_get_speed(uint32 core_id); 

//============================================================================
/**
 * @function  qhsusb_dci_is_attached
 *
 * @brief Report if a USB cable is attached to the device.
 *
 * @Note :  Although attached, the host communicates with the device only after 
 *          the device is *connected* to the bus (D+ pull-up).
 *
 * @Note : In Device Mode:
 *      1=Attached. 0=Not Attached. Default=0. 
 *      A one indicates that the device successfully attached
 *      and is operating in either high speed or full speed as indicated by the High Speed Port bit in
 *      this register. 
 *      A zero indicates that the device did not attach successfully or was forcibly
 *      disconnected by the software writing a zero to the Run bit in the USBCMD register. 
 *      It does not state the device being disconnected or suspended.
 *
 * @param core_id    
 *
 * @return boolean ( TRUE / FALSE )
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.16 PORTSCx , CCS bit.
 *
 */
//============================================================================
boolean qhsusb_dci_is_attached(uint32 core_id);

//============================================================================
/**
 * @function  qhsusb_dci_is_wall_charger
 *
 * @brief check if Wall-Charger detected.
 *
 * @Note :  It is done via PMIC on ULC.
 *          It can be done via the USB core when using ULPI PHY.
 *
 * @param core_id    
 *
 * @return boolean ( TRUE / FALSE )
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.16 PORTSCx
 *
 */
//============================================================================
boolean qhsusb_dci_is_wall_charger(uint32 core_id);

//============================================================================
/**
 * @function  qhsusb_dci_is_connected
 *
 * @brief Report if the device is connected to the bus.
 *
 * @Note :  Connect/Disconnect is done via PMIC on ULC.
 *          It can be done via the USB core when using ULPI PHY.
 *
 * @param core_id    
 *
 * @return boolean ( TRUE / FALSE )
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.1 USBCMD
 *
 */
//============================================================================
boolean qhsusb_dci_is_connected(uint32 core_id);

//============================================================================
/**
 * @function  qhsusb_dci_is_suspended
 *
 * @brief report if the device is in suspend mode
 *
 * @param core_id    
 *
 * @return is_suspended
 *         1=Suspended,0=Not-Suspended
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *       Chapter 3.6.16 PORTSCx
 *
 */
//============================================================================
boolean qhsusb_dci_is_suspended(uint32 core_id);

//============================================================================
/**
 * @function  qhsusb_dci_connect
 *
 * @brief Connect the device to the USB bus.
 *
 * @Note :  Connect/Disconnect is done via PMIC on ULC.
 *          It can be done via the USB core when using ULPI PHY.
 *
 * @param core_id    
 *
 * @return none
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.1 USBCMD
 *
 */
//============================================================================
void    qhsusb_dci_connect(uint32 core_id);

//============================================================================
/**
 * @function  qhsusb_dci_disconnect
 *
 * @brief Disconnect the device from the USB bus.
 *
 * @Note :  Connect/Disconnect is done via PMIC on ULC.
 *          It can be done via the USB core when using ULPI PHY.
 *
 * @param core_id    
 *
 * @return none
 *
 * @ref ChipIdea spec "CI13612a HighSpeed USB OnTheGo Controller Core":
 *      Chapter 3.6.1 USBCMD
 */
//============================================================================
void    qhsusb_dci_disconnect(uint32 core_id);

// ===========================================================================
/**
 * @function    qhsusb_dci_start_timer
 * 
 * @brief   Start one of the USB Core general-purpose timers
 * 
 * @note    Time#0 is for free run, Timer#1 is for short delays.
 * 
 * @param core_id    
 *
 * @param timer    
 *          QHSUSB_TIMER_0_FREE_RUN
 *          QHSUSB_TIMER_1_ONE_SHOT
 *
 * @param repeat    
 *          1=Repeat , 0=One-Shot
 * 
 * @return status
 *              0 = ok , otherwise error.
 */
// ===========================================================================
uint32  qhsusb_dci_start_timer(uint32 core_id,uint8 timer, boolean repeat);

// ===========================================================================
/**
 * @function    qhsusb_dci_stop_timer
 * 
 * @brief   Stop one of the USB Core general-purpose timers
 * 
 * @note    Time#0 is for free run, Timer#1 is for short delays.
 * 
 * @param core_id    
 *
 * @param timer    
 *          QHSUSB_TIMER_0_FREE_RUN
 *          QHSUSB_TIMER_1_ONE_SHOT
 *
 * @return status
 *              0 = ok , otherwise error.
 */
// ===========================================================================
uint32  qhsusb_dci_stop_timer(uint32 core_id,uint8 timer); 
// ===========================================================================
/**
 * @function    qhsusb_dci_get_time_usec
 * 
 * @brief   Get the elapsed time in micro seconds (usec)
 * 
 * @note    If the timer is in repeat mode it is the caller responsibility to handle rollover situation.
 * 
 * @param core_id    
 *
 * @param timer    
 *          QHSUSB_TIMER_0_FREE_RUN
 *          QHSUSB_TIMER_1_ONE_SHOT
 *
 * @return  time
 *          elapsed time in micro seconds
 */
// ===========================================================================
uint32  qhsusb_dci_get_time_usec(uint32 core_id,uint8 timer); 

// ===========================================================================
/**
 * @function    qhsusb_dci_ulpi_write
 * 
 * @brief   Writes to UPLI view port register
 * 
 * 
 * @param core_id    
 *
 * @param ulpi_reg    
 *          ULPI register name
 *
 * @param data    
 *          Data to write to the ulpi register
 *
 * @param mask    
 *          Mask to be used with specified data
 *
 * @return  none
 *          
 */
// ===========================================================================
void qhsusb_dci_ulpi_write(uint32 core_id, uint32 ulpi_reg, uint8 data, uint8 mask);
//============================================================================
/**
 * @function  qhsusb_dci_ulpi_read
 *
 * @brief write to the ulpi register
 *
 * @Note : For performing tasks that require ULPI reads eg. charger detection
 *          
 * @param ulpi_reg 
 *           ULPI register from which to read
 *
 * @return uint8 ULPI register value
 *
 * @ref 
 *      
 *
 */
//============================================================================
uint32 qhsusb_dci_ulpi_read(uint32 core_id, uint32 ulpi_reg);
//============================================================================
/**
 * @function  qhsusb_dci_get_line_state
 *
 * @brief Read line state from portsc register
 *
 *          
 * @param core_id 
 *           USB core id
 *
 * @return uint8 
 *           line state
 *
 * @ref 
 *      
 *
 */
//============================================================================
uint32 qhsusb_dci_get_line_state(uint32 core_id);
//============================================================================
/**
 * @function  qhsusb_dci_set_runstop_bit
 *
 * @brief Set the Run Stop bit in the USB controller
 *
 *          
 * @param core_id 
 *           USB core id
 *
 * @return none
 *           
 *
 * @ref 
 *      
 */
//===========================================================================
void qhsusb_dci_set_runstop_bit(uint32 core_id);
//===========================================================================
/**
* @function  qhsusb_dci_delay_ms
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
void qhsusb_dci_delay_ms(uint32 msecs);
//============================================================================
/**
* @function  qhsusb_dci_delay_us
*
* @brief Perform delay in microseconds.
*
* @Note : The USB Timers can not be used before the core is initialized.
*
* @param microseconds
*
* @return none
*
*/
//============================================================================
void qhsusb_dci_delay_us(uint32 usecs);
//=============================================================================
/**
 * @function  qhsusb_dci_detect_charger_type
 *
 * Detects charger port type
 * 
 * Should be called after usb stack initialization
 * 
 * @param core_id
 *        
 *        
 *  
 * @return Charger port type.
 */
//=============================================================================
qhsusb_chg_port_type qhsusb_dci_detect_charger_type(uint32 core_id);
//=============================================================================
/**
 * @function  qhsusb_dci_get_charger_type
 *
 * return the detected charger port type
 *
 * Should be called after qhsusb_dci_detect_charger_type
 *
 * @param core_id
 *
 *
 *
 * @return Charger port type.
 */
//=============================================================================
qhsusb_chg_port_type qhsusb_dci_get_charger_type(uint32 core_id);
//=============================================================================
/**
 * @function  qhsusb_dci_set_enum_flag
 *
 * Stores whether R/S bit should be set during dci_init sequence
 * 
 * 
 * 
 * @param true or false
 *        
 *        
 *  
 * @return None
 */
//=============================================================================
void qhsusb_dci_set_enum_flag(boolean start_enum);

//=============================================================================
/**
 *
 * @function  qhsusb_dci_get_enum_flag
 *
 * Returns whether R/S bit should be set during dci_init sequence
 * 
 * 
 * @param None
 *        
 *        
 *  
 * @return boolean
 */
//=============================================================================
boolean qhsusb_dci_get_enum_flag(void);

//=============================================================================
/**
 * @function    qhsusb_dci_write_scratch
 * 
 * @brief   Write scratch data on USB_OTG_HS_GPTIMER0LD and USB_OTG_HS_GPTIMER1LD
 * 
 * @note    The max value of GPTIMER0LD and GPTIMER1LD is 0xffffff (24 bits).
 * 
 * @param core_id    
 * @param data0    
 * @param data1    
 *
 */
// ===========================================================================
void qhsusb_dci_write_scratch(uint32 core_id, uint32 data0, uint32 data1);


// ===========================================================================
/**
 * @function    qhsusb_dci_read_scratch
 * 
 * @brief   Read scratch data on USB_OTG_HS_GPTIMER0LD and USB_OTG_HS_GPTIMER1LD
 * 
 * @note    The max value of GPTIMER0LD and GPTIMER1LD is 0xffffff.
 * 
 * @param core_id    
 *
 * @param data0_ptr - data0 pointer (24 bits) - to return value   
 *
 * @param data1_ptr - data1 pointer (24 bits) - to return value    
 *
 */
// ===========================================================================
void qhsusb_dci_read_scratch(uint32 core_id, uint32* data0_ptr, uint32* data1_ptr);

#endif // USB_FN_LIB_H
