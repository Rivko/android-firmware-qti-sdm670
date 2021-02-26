#ifndef QUSB_CH9_H
#define QUSB_CH9_H
/*=======================================================================*/
/*
  @file         qusb_ch9.h
 
  @brief        USB Chapter 9 structures
 
  @details      
 
               Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
========================================================================*/ 
//
//  when        who       what, where, why
//  ----------  ---      -------------------------------------------------------
//  2010-08-30  dpatnaik Port QUSB to RPM, add ULPI based charger detection
//
//========================================================================
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/


#include "comdef.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/


/**
*  Enumeration for Port Test Mode. USB2.0 Table 9-2
**/
typedef enum {
  USB_PORT_TEST_DISABLE,
  USB_PORT_TEST_J_STATE,
  USB_PORT_TEST_K_STATE,
  USB_PORT_TEST_SE0_NAK,
  USB_PORT_TEST_PACKET,
  USB_PORT_TEST_FORCE_ENABLE,
} QCOM_USB_PORT_TEST_MODE;


/**
 * Setup packet
 */
PACKED typedef struct usb_setup_packet {
   uint8  bmRequestType;
   uint8  bRequest;
   uint16 wValue;
   uint16 wIndex;
   uint16 wLength;
} PACKED_POST usb_setup_packet_t ;

/** @defgroup usb_setup_rt Request type sub-fields
 *  Bit fields for bmRequestType
 */
/*@{*/

/** @defgroup usb_setup_rt_dir Data transfer direction
 *  Bit 7
 */
/*@{*/
#define USB_SETUP_RT_DIR_POS  (7)     // Position
#define USB_SETUP_RT_DIR_SEED (0x01)  // Seed
#define USB_SETUP_RT_DIR_MASK (USB_SETUP_RT_DIR_SEED << USB_SETUP_RT_DIR_POS) //Mask 
#define USB_SETUP_RT_DIR_H2D  (0)     //Host to Device 
#define USB_SETUP_RT_DIR_D2H  (1)     //Device to Host 
/* @} usb_setup_rt_dir */

/** @defgroup usb_setup_rt_type Type
 *  Bits 6:5
 */
/*@{*/
#define USB_SETUP_RT_TYPE_POS  (5)    // Position 
#define USB_SETUP_RT_TYPE_SEED (0x03) // Seed 
#define USB_SETUP_RT_TYPE_MASK (USB_SETUP_RT_TYPE_SEED << USB_SETUP_RT_TYPE_POS) // Mask 
#define USB_SETUP_RT_TYPE_STD  (0)    // Standard 
#define USB_SETUP_RT_TYPE_CLS  (1)    // Class 
#define USB_SETUP_RT_TYPE_VND  (2)    // Vendor 
#define USB_SETUP_RT_TYPE_RSVD (3)    //Reserved 
/* @} usb_setup_rt_type */

/** @defgroup usb_setup_rt_rcp Recipient
 *  Bits 4:0
 */
/*@{*/
#define USB_SETUP_RT_RCP_POS  (0)     // Position 
#define USB_SETUP_RT_RCP_SEED (0x1f)  // Seed 
#define USB_SETUP_RT_RCP_MASK (USB_SETUP_RT_RCP_SEED << USB_SETUP_RT_RCP_POS) // Mask 
#define USB_SETUP_RT_RCP_DEV  (0)     // Device 
#define USB_SETUP_RT_RCP_IFC  (1)     // Interface 
#define USB_SETUP_RT_RCP_EP   (2)     // Endpoint 
#define USB_SETUP_RT_RCP_OTH  (3)     // Other 
/* @} usb_setup_rt_rcp */

/* @} usb_setup_rt */

/** @defgroup usb_setup_req Requests
 *  values for bRequest
 */
/*@{*/
#define USB_SETUP_REQ_GET_STATUS         (0)
#define USB_SETUP_REQ_CLEAR_FEATURE      (1)
#define USB_SETUP_REQ_SET_FEATURE        (3)
#define USB_SETUP_REQ_SET_ADDRESS        (5)
#define USB_SETUP_REQ_GET_DESCRIPTOR     (6)
#define USB_SETUP_REQ_SET_DESCRIPTOR     (7)
#define USB_SETUP_REQ_GET_CONFIGURATION  (8)
#define USB_SETUP_REQ_SET_CONFIGURATION  (9)
#define USB_SETUP_REQ_GET_INTERFACE     (10)
#define USB_SETUP_REQ_SET_INTERFACE     (11)
#define USB_SETUP_REQ_SYNCH_FRAME       (12)
#define USB_SETUP_REQ_SET_SEL           (48)
#define USB_SETUP_REQ_SET_ISOCH_DELAY   (49)


/* @} usb_setup_req */

/** @defgroup usb_desctype Descriptor Types
 *  values for bDescriptorType
 */
/*@{*/
#define USB_DESCTYPE_DEVICE         (1)
#define USB_DESCTYPE_CONFIGURATION  (2)
#define USB_DESCTYPE_STRING         (3)
#define USB_DESCTYPE_INTERFACE      (4)
#define USB_DESCTYPE_ENDPOINT       (5)
#define USB_DESCTYPE_DEV_QUAL       (6)
#define USB_DESCTYPE_OTH_SPEED      (7)
#define USB_DESCTYPE_IFC_POWER      (8)
#define USB_DESCTYPE_EP_COMP        (0x30)
#define USB_DESCTYPE_BOS            (0xf)

#define USB_DEVICE_CAPABILITY	    (0x10)

/**
 * @ref HID 1.11 clause 7.1
 */
#define USB_DESCTYPE_HID           (33)
#define USB_DESCTYPE_HID_REPORT    (34)
#define USB_DESCTYPE_HID_PHYS      (35)
/* @} usb_desctype */

/** @defgroup usb_fsel Feature Selectors
 *  values for wValue in {SET|CLEAR}_FEATURE requests
 */
/*@{*/
#define USB_FSEL_EP_HALT            (0)
#define USB_FSEL_DEV_REMOTE_WAKEUP  (1)
#define USB_FSEL_DEV_TEST_MODE      (2)

#define USB_FSEL_IFACE_FUNC_SUSP    (0)

#define USB_FSEL_U1_ENABLE          (48)
#define USB_FSEL_U2_ENABLE          (49)
#define USB_FSEL_LTM_ENABLE         (50)

/* USB Feature Status bits */
#define DEV_BUS_PWRD            (0)
#define DEV_SELF_PWRD           (1)
#define DEV_REMOTE_WAKEUP       (1 << 1)
#define U1_ENABLE               (1 << 2)
#define U2_ENABLE               (1 << 3)

#define FUNC_REMOTE_WAKEUP_CAP  (1)
#define FUNC_REMOTE_WAKEUP      (1 << 1)



/* @} usb_desctype */

/**
 * Any descriptor begins with this header
 */
PACKED typedef struct usb_desc_header {
  uint8  bLength;
  uint8  bDescriptorType;
} PACKED_POST usb_desc_header_t;

/**
 * Standard Device descriptor
 */
PACKED typedef struct usb_desc_device {
  uint8  bLength;
  uint8  bDescriptorType;              //USB_DESCTYPE_DEVICE 
  uint16 bcdUSB;
  uint8  bDeviceClass;
  uint8  bDeviceSubClass;
  uint8  bDeviceProtocol;
  uint8  bMaxPacketSize0;
  uint16 idVendor;
  #define USB_VENDOR_ID_QUALCOMM      (0x05C6)
  uint16 idProduct;
  uint16 bcdDevice;
  uint8  iManufacturer;
  uint8  iProduct;
  uint8  iSerialNumber;
  uint8  bNumConfigurations;
} PACKED_POST usb_desc_device_t;

/**
 * Standard Configuration descriptor 
 * Same structure used for USB_DESCTYPE_OTH_SPEED 
 */
PACKED typedef struct usb_desc_configuration {
  uint8  bLength;
  uint8  bDescriptorType;              // USB_DESCTYPE_CONFIGURATION 
  uint16 wTotalLength;
  uint8  bNumInterfaces;
  uint8  bConfigurationValue;
  uint8  iConfiguration;
  uint8  bmAttributes;
  uint8  bMaxPower;
} PACKED_POST usb_desc_configuration_t;

/**
 * Standard Interface descriptor
 */
PACKED typedef struct usb_desc_interface {
  uint8  bLength;
  uint8  bDescriptorType;              // USB_DESCTYPE_INTERFACE 
  uint8  bInterfaceNumber;
  uint8  bAlternateSetting;
  uint8  bNumEndpoints;
  uint8  bInterfaceClass;
  uint8  bInterfaceSubClass;
  uint8  bInterfaceProtocol;
  uint8  iInterface;
} PACKED_POST usb_desc_interface_t;

/**
 * Standard Endpoint descriptor
 */
PACKED typedef struct usb_desc_endpoint {
  uint8  bLength;
  uint8  bDescriptorType;              // USB_DESCTYPE_ENDPOINT 
  uint8  bEndpointAddress;
#define USB_EP_ADDR_OUT                (0)
#define USB_EP_ADDR_IN              (0x80)
  uint8  bmAttributes;
#define USB_EP_ATTR_CNTRL              (0)
#define USB_EP_ATTR_ISOCH              (1)
#define USB_EP_ATTR_BULK               (2)
#define USB_EP_ATTR_INTR               (3)
  uint16 wMaxPacketSize;
  uint8  bInterval;
} PACKED_POST usb_desc_endpoint_t;

/**
 * Standard Device Qualifier descriptor
 */
PACKED typedef struct usb_desc_dev_qual {
  uint8  bLength;
  uint8  bDescriptorType;              //USB_DESCTYPE_DEV_QUAL
  uint16 bcdUSB;
  uint8  bDeviceClass;
  uint8  bDeviceSubClass;
  uint8  bDeviceProtocol;
  uint8  bMaxPacketSize0;
  uint8  bNumConfigurations;
  uint8  bReserved;
} PACKED_POST usb_desc_dev_qual_t;

/**
 * HID class descriptor 
 */
PACKED typedef struct usb_desc_hid {
 uint8  bLength;
 uint8  bDescriptorType;              // USB_DESCTYPE_HID
 uint16 bcdHID;
 uint8  bCountryCode;
 uint8  bNumDescriptor;
 uint8  bDescriptorType1;             //1 to avoid name clash
 uint16 wDescriptorLen;
} PACKED_POST usb_desc_hid_t;

/* *
 * SuperSpeed Endpoint Companion descriptor
 */
PACKED typedef struct usb_ep_comp_desc {
  uint8  bLength;
  uint8  bDescriptorType;
  uint8  bMaxBurst;
  uint8  bmAttributes;
  uint16 wBytesPerInterval;
} PACKED_POST usb_ep_comp_desc_t;


/* *
*  BOS: group of device-level capabilities 
*/
PACKED typedef struct usb_bos_desc {
  uint8  bLength;
  uint8  bDescriptorType;
  uint16 wTotalLength;
  uint8  bNumDeviceCaps;
} PACKED_POST usb_bos_desc_t;

#define USB_BOS_DESC_SIZE           (5)


/* *
* Device capability Header
*/
PACKED typedef struct usb_dev_cap_header {
  uint8  bLength;
  uint8  bDescriptorType;
  uint8  bDevCapabilityType;
} PACKED_POST usb_dev_cap_header_t;


#define USB_LOW_SPEED_SUPPORT       (1)
#define USB_FULL_SPEED_SUPPORT      (1 << 1) 
#define USB_HIGH_SPEED_SUPPORT      (1 << 2) 
#define USB_SUPER_SPEED_SUPPORT     (1 << 3) 

#define         USB_SS_CAP_TYPE                  3
#define         USB_USB2_EXTN_CAP_TYPE           2
#define         USB_USB2_EXTN_LPM_CAP            2



/**
 * SuperSpeed USB Capability descriptor: Set of SuperSpeed USB
 * specific device level capabilities
 */
PACKED typedef struct usb_ss_cap_desc {
  uint8  bLength;
  uint8  bDescriptorType;
  uint8  bDevCapabilityType;
  uint8  bmAttributes;
  uint16 wSpeedSupported;
  uint8  bFunctionalitySupport;
  uint8  bU1devExitLat;
  uint16 bU2DevExitLat;
} PACKED_POST usb_ss_cap_desc_t;

/**
 * USB2 Extention USB Capability descriptor: Set of SuperSpeed USB
 * specific device level capabilities
 */
PACKED typedef struct usb_extn_cap_desc {
  uint8  bLength;
  uint8  bDescriptorType;
  uint8  bDevCapabilityType;
  uint32  bmAttributes;
} PACKED_POST usb_extn_cap_desc_t;


//
// Tracking the control transfer stage
//
typedef enum qusb_ctrl_xfer_state{
  EP0_STATE_MIN = 0,
  EP0_STATE_SETUP_TRB,
  EP0_STATE_INTRPT_SETUP,
  EP0_STATE_STATUS_2,
  EP0_STATE_DATA,
  EP0_STATE_DATA_IN_PROGRESS,
  EP0_STATE_STATUS_3,
  EP0_STATE_MAX
} qusb_ctrl_xfer_state_t;

typedef enum qusb_link_state{
  LS_U0 = 0,  // HS on state 
  LS_U1,
  LS_U2,      // HS L1 state (sleep)  
  LS_U3,      // HS L2 state (suspend) 
  LS_SS_DIS,  // HS Disconnect state
  LS_RX_DET,
  LS_SS_INACT,
  LS_POLL,
  LS_RECOV,   // Remote Wakeup for HS
  LS_HRESET,
  LS_CMPLY,
  LS_LPBK,
  LS_RESUME_RESET = 0xF
} qusb_link_state_t;

typedef enum qusb_device_connect_speed{
  QUSB_HIGH_SPEED = 0,
  QUSB_FULL_SPEED  = 1,
  QUSB_LOW_SPEED   = 2,
  QUSB_FULL_SPEED_48 = 3,
  QUSB_SUPER_SPEED = 4,
  QUSB_UNKNOWN_SPEED = 0xF
} qusb_device_connect_speed_t;

typedef enum qusb_device_bcd_USB{
  QUSB_BCD_USB2       = 0x200,
  QUSB_BCD_USB2_EXT   = 0x210,
  QUSB_BCD_USB3       = 0x310,    
} qusb_device_bcd_USB_t; 

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#endif /*QUSB_CH9_H*/

