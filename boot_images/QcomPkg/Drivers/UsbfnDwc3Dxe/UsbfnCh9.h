/**
 * @file UsbfnCh9.h
 *
 * @brief USB Chapter 9 structures
 *
 * @copyright Copyright (C) 2013 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 */
/*=============================================================================
 EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 06/10/13   mliss   Initial rework for Synopsys Usbfn driver
 =============================================================================*/

#ifndef _USBFN_CH9_H_
#define _USBFN_CH9_H_

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

/** transfer direction
 *  Bit 7
 */
#define USB_SETUP_RT_DIR_POS  (7)     /**< Position */
#define USB_SETUP_RT_DIR_SEED (0x01)  /**< Seed */
#define USB_SETUP_RT_DIR_MASK (USB_SETUP_RT_DIR_SEED << USB_SETUP_RT_DIR_POS) /**< Mask */
#define USB_SETUP_RT_DIR_H2D  (0)     /**< Host to Device */
#define USB_SETUP_RT_DIR_D2H  (1)     /**< Device to Host */

/** Type
 *  Bits 6:5
 */
#define USB_SETUP_RT_TYPE_POS  (5)    /**< Position */
#define USB_SETUP_RT_TYPE_SEED (0x03) /**< Seed */
#define USB_SETUP_RT_TYPE_MASK (USB_SETUP_RT_TYPE_SEED << USB_SETUP_RT_TYPE_POS) /**< Mask */
#define USB_SETUP_RT_TYPE_STD  (0)    /**< Standard */
#define USB_SETUP_RT_TYPE_CLS  (1)    /**< Class */
#define USB_SETUP_RT_TYPE_VND  (2)    /**< Vendor */
#define USB_SETUP_RT_TYPE_RSVD (3)    /**< Reservrd */

/** Recipient
 *  Bits 4:0
 */
#define USB_SETUP_RT_RCP_POS  (0)     /**< Position */
#define USB_SETUP_RT_RCP_SEED (0x1f)  /**< Seed */
#define USB_SETUP_RT_RCP_MASK (USB_SETUP_RT_RCP_SEED << USB_SETUP_RT_RCP_POS) /**< Mask */
#define USB_SETUP_RT_RCP_DEV  (0)     /**< Device */
#define USB_SETUP_RT_RCP_IFC  (1)     /**< Interface */
#define USB_SETUP_RT_RCP_EP   (2)     /**< Endpoint */
#define USB_SETUP_RT_RCP_OTH  (3)     /**< Other */

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

typedef PACKED struct usb_desc_bos{
  uint8   bLength;
  uint8   bDescriptorType;
  uint16  wTotalLength;
  uint8   bNumDeviceCaps;
} PACKED_POST usb_desc_bos_t;

typedef PACKED struct usb_desc_dev_cap{
  uint8   bLength;
  uint8   bDescriptorType;
  uint8   bDevCapabilityType;
  uint8   vCapDepField[1];
} PACKED_POST usb_desc_dev_cap_t;

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
 * SuperSpeed Endpoint Companion descriptor
 */

typedef PACKED struct usb_desc_ss_endpoint_companion {
   uint8  bLength;
   uint8  bDescriptorType;              /**< USB_DESCTYPE_SS_ENDPOINT_COMPANION */
   uint8  bMaxBurst;
   PACKED union {
     uint8 u;
     PACKED struct {
       uint8 max_streams :5;
       uint8 reserved1   :3;
     } bulk;
   } bmAttributes;
   uint16 wBytesPerInterval;
} PACKED_POST usb_desc_ss_endpointcompanion_t;

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


#endif /* _USBFN_CH9_H_ */
