#ifndef QHSUSB_CH9_H
#define QHSUSB_CH9_H
/*=======================================================================*/
/*
  @file         qhsusb_ch9.h
 
  @brief        USB Chapter 9 structures
 
  @details      
 
               Copyright (c) 2008 Qualcomm Technologies Incorporated.
               All Rights Reserved.
               Qualcomm Confidential and Proprietary
========================================================================*/ 
//
//  when        who       what, where, why
//  ----------  ---      -------------------------------------------------------
//  2010-08-30  dpatnaik Port QHSUSB to RPM, add ULPI based charger detection
//
//========================================================================
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/

#include "com_dtypes.h"

#define PACKED_POST __attribute__((__packed__)) 

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/**
 * Setup packet
 */
typedef PACKED struct usb_setup_packet {
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
#define USB_SETUP_RT_DIR_POS  (7)     /**< Position */
#define USB_SETUP_RT_DIR_SEED (0x01)  /**< Seed */
#define USB_SETUP_RT_DIR_MASK (USB_SETUP_RT_DIR_SEED << USB_SETUP_RT_DIR_POS) /**< Mask */
#define USB_SETUP_RT_DIR_H2D  (0)     /**< Host to Device */
#define USB_SETUP_RT_DIR_D2H  (1)     /**< Device to Host */
/* @} usb_setup_rt_dir */

/** @defgroup usb_setup_rt_type Type
 *  Bits 6:5
 */
/*@{*/
#define USB_SETUP_RT_TYPE_POS  (5)    /**< Position */
#define USB_SETUP_RT_TYPE_SEED (0x03) /**< Seed */
#define USB_SETUP_RT_TYPE_MASK (USB_SETUP_RT_TYPE_SEED << USB_SETUP_RT_TYPE_POS) /**< Mask */
#define USB_SETUP_RT_TYPE_STD  (0)    /**< Standard */
#define USB_SETUP_RT_TYPE_CLS  (1)    /**< Class */
#define USB_SETUP_RT_TYPE_VND  (2)    /**< Vendor */
#define USB_SETUP_RT_TYPE_RSVD (3)    /**< Reservrd */
/* @} usb_setup_rt_type */

/** @defgroup usb_setup_rt_rcp Recipient
 *  Bits 4:0
 */
/*@{*/
#define USB_SETUP_RT_RCP_POS  (0)     /**< Position */
#define USB_SETUP_RT_RCP_SEED (0x1f)  /**< Seed */
#define USB_SETUP_RT_RCP_MASK (USB_SETUP_RT_RCP_SEED << USB_SETUP_RT_RCP_POS) /**< Mask */
#define USB_SETUP_RT_RCP_DEV  (0)     /**< Device */
#define USB_SETUP_RT_RCP_IFC  (1)     /**< Interface */
#define USB_SETUP_RT_RCP_EP   (2)     /**< Endpoint */
#define USB_SETUP_RT_RCP_OTH  (3)     /**< Other */
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
/* @} usb_desctype */

/**
 * Any descriptor begins with this header
 */
typedef PACKED struct usb_desc_header {
   uint8  bLength;
   uint8  bDescriptorType;
} PACKED_POST usb_desc_header_t;

/**
 * Standard Device descriptor
 */
typedef PACKED struct usb_desc_device {
   uint8  bLength;
   uint8  bDescriptorType;              /**< USB_DESCTYPE_DEVICE */
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
typedef PACKED struct usb_desc_configuration {
   uint8  bLength;
   uint8  bDescriptorType;              /**< USB_DESCTYPE_CONFIGURATION */
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
typedef PACKED struct usb_desc_interface {
   uint8  bLength;
   uint8  bDescriptorType;              /**< USB_DESCTYPE_INTERFACE */
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
typedef PACKED struct usb_desc_endpoint {
   uint8  bLength;
   uint8  bDescriptorType;              /**< USB_DESCTYPE_ENDPOINT */
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
typedef PACKED struct usb_desc_dev_qual {
   uint8  bLength;
   uint8  bDescriptorType;              /**< USB_DESCTYPE_DEV_QUAL */
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
typedef PACKED struct usb_desc_hid {
   uint8  bLength;
   uint8  bDescriptorType;              /**< USB_DESCTYPE_HID */
   uint16 bcdHID;
   uint8  bCountryCode;
   uint8  bNumDescriptor;
   uint8  bDescriptorType1;             /**< 1 to avoid name clash */
   uint16 wDescriptorLen;
} PACKED_POST usb_desc_hid_t;

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

#endif /*QHSUSB_CH9_H*/

