/*=======================================================================*//**
  @file         qusb_fd.c

  @brief        Function Driver for  USB custom function

  @details      
                This FD implements HDLC streaming protocol.
                It can use one of the following USB interfaces, depending on
                value of global variable qusb_hdlc_use_cdcacm:

                - 0 : use vendor specific class with 2 bulk EP's
                - 1 : use CDC/ACM class with 2 bulk and 1 interrupt EP's

                HDLC implementation: For H->D direction, double buffers used
                to maximize throughput. Upon packet receive, the user supplied
                callback executed. This callback should handle the data arrived.
                For D->H direction, byte oriented interface provided. Data
                accumulated in internal buffer till it is full or 
                HDLC terminator (0x7e) submitted. In this case, packet get 
                transmitted to the USB bus.

                CDC/ACM implementation: FD supports ACM class specific commands:

                - SET_LINE_CODING
                - GET_LINE_CODING
                - SET_CONTROL_LINE_STATE

                Immediately after configuration (SET_CONFIG), FD sends 
                SERIAL_STATE notification indicating DCD and DSR

                CDC/ACM uses 2 interfaces, thus it requires 
                QUSB_HAVE_MULTI_FUNCTIONS to be defined.

                To use CDC/ACM, set qusb_hdlc_use_cdcacm to 1 prior to
                QUSB stack initialization

                When the QUSB_SINGLE_BOOT_ENUM_MAGIC is passed via the 
                HS-USB scratch register HDLC will not issue a USB enumeration.
                Instead, it will rely on the existing USB enumeration. This is
                done for having a single USB enumeration during SW download.
 
                Copyright (c) 2008-2009 Qualcomm Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
*//*========================================================================*/

/*============================================================================
  EDIT HISTORY FOR MODULE


  when        who       what, where, why
  ----------  ---       ------------------------------------------------------
  2013-09-17  shreyasr	Removed the CDC ACM support and changes for SNPS controller
  2011-07-22  dpatnaik  For EMMCBLD/EHOSTDL dont update serial number string
  2011-06-10  dpatnaik  Added support for reading PID, Serial string from IMEM
  2011-05-18  kimd      Added support for unframed RAM-dump (tx packet feature)
  2009-04-17  dhaval    Fixed vid/pid read for cdc/acm device.
  2009-04-16  dhaval    Implemented API for retrieving vid/pid
  2009-04-09  dhaval    Corrected iSerialNumber
  2009-04-06  dhaval    Added API qusb_fd_rx_completed to fix defect
                        when usb transfer > 4K.
  2009-03-17  dhaval    Added API for updating vid/pid if vid/pid fuses blown 
  2009-03-17  dhaval    Removed compiler warnings.
  2008-12-17  vkondrat  CDC/ACM support
  2008-08-14  vkondrat  redesign - use simple 1 buffer mode, Rx callback
  2008-08-10  vkondrat  initial version

============================================================================*/

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "qusb_fd.h"
#include "qusb_dcd.h"
#include "qusb_dci_api.h"
#include "qusb_log.h"
#include "qusb_cookie.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define RX_BUF_SIZE (1*1024) // size of each (of 2) Rx buffers 
#define TX_BUF_SIZE (1*1024) // size of  Tx buffer 
#define RX_BUF_SIZE_FIXED (2*1024) // for DMSS and Streaming Download Protocols Only 

#define CUSTOMIZE NULL

#define USB_INTERFACE_PROTOCOL_SAHARA (0x10)

// STRING DESCRIPTOR - USB 2.0 specification Table 9-16
// 1 byte:  bLength         - size of this desc in bytes
// 1 byte:  bDescriptorType - STRING Descriptor Type
// N bytes: bString         - UNICODE encoded string
// UNICODE string scriptor is not NULL-terminated.  The string length is
// substracting two from the value of the first type of the descriptor

// bLength's max value is 255.  2 bytes are used for bLength and 
// bDescriptorType fields
// This leaves 255-2 = 253 for UNICODE string size in bytes.
// Since UNICODE byte size has to be even number, max UNICODE ARRAY byte size 
// is 252 bytes.
#define QUSB_STRING_DESC_UNICODE_SIZE_MAX         (252)

// Size of bLength and bDescriptor field in STRING DESCRIPTOR in bytes
#define QUSB_STRING_DESC_LEN_AND_DESC_FIELD_SIZE  (2)

// Maximum size for the STRING DESCRIPTOR, must be even number
#define QUSB_STRING_DESC_SIZE_MAX    \
  (QUSB_STRING_DESC_UNICODE_SIZE_MAX \
  + QUSB_STRING_DESC_LEN_AND_DESC_FIELD_SIZE)

#define QUSB_DLOAD_SIZE           (35)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
 uint32 qusb_dload_info[QUSB_DLOAD_SIZE];  //This is the static dload structure 

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

static void qusb_notify_cb(struct qusb_dcd_dsc_device* dev, qusb_notify_events_t event);
//Functions for internal buffer mode
static void qusb_init_rx_int_buf_mode(const struct qusb_dcd_dsc_device* dev);
static void qusb_rx_callback_int_buf_mode(struct qusb_urb* urb);

/*****hardware descriptors*****/
/*****OBEX ********************/
static usb_desc_device_t qusb_usb_device_obex = {
  sizeof(usb_desc_device_t),        //  uint8  bLength;
  USB_DESCTYPE_DEVICE,              //  uint8  bDescriptorType;
  QUSB_BCD_USB2,                    //  uint16 bcdUSB;
  0,                                //  uint8  bDeviceClass;
  0,                                //  uint8  bDeviceSubClass;
  0,                                //  uint8  bDeviceProtocol;
  QUSB_EP0_MAX_PACKET_SIZE_HIGH,    //  uint8  bMaxPacketSize0;
  USB_VENDOR_ID_QUALCOMM,           //  uint16 idVendor;
#ifndef FEATURE_QUSB_MS
  USB_PRODUCT_DIAG_ONLY,            //  uint16 idProduct;
#else /* FEATURE_QUSB_MS */
  USB_PRODUCT_DIAG_MS,
#endif /*FEATURE_QUSB_MS*/
  0x0,                              //  uint16 bcdDevice;
  1,                                //  uint8  iManufacturer;
  2,                                //  uint8  iProduct;
  0,                                //  uint8  iSerialNumber;
  1                                 //  uint8  bNumConfigurations;
};

PACKED struct qusb_bos_struct_obex qusb_usb_bos_obex = {
  {
    sizeof(usb_bos_desc_t),         //  uint8 bLength;
    0xF,                            //  uint8 bDescriptorType;
    sizeof(struct qusb_bos_struct_obex),    //  uint16 wTotalLength;
    0x2                             //  uint8 bNumDeviceCaps;
  },
  {
    sizeof(usb_ss_cap_desc_t),      // uint8 bLength; 
    0x10,                           // uint8 bDescriptorType
    0x3,                            // uint8 bDevCapabilityType
    0x0,                            // uint8 bmAttributes
    0xE,                            // uint16 wSpeedSupported -- Full,High and Super Speed
    0x1,                            // uint8 bFunctionalitySupport
    0xA,                            // uint8 bU1devExitLat
    0x7FF                           // uint16 bU2DevExitLat
  },
  {
    sizeof(usb_extn_cap_desc_t),    // uint8 bLength; 
    0x10,                           // uint8 bDescriptorType
    0x2,                            // uint8 bDevCapabilityType
#if FEATURE_QUSB_USBCV_MODE
    0x6                             // uint32 bmAttributes
#else
    0x2                             // uint32 bmAttributes
#endif
  }
};

PACKED struct usb_desc_dev_qual qusb_desc_dev_qual = {
  sizeof(qusb_desc_dev_qual), USB_DESCTYPE_DEV_QUAL,
  QUSB_BCD_USB2,                  //  uint16 bcdUSB;
  0,                              //  uint8  bDeviceClass;
  0,                              //  uint8  bDeviceSubClass;
  0,                              //  uint8  bDeviceProtocol;
  64,                             //  uint8  bMaxPacketSize0;
  1                               //  uint8  bNumConfigurations;
};

PACKED struct qusb_config_struct_obex qusb_usb_config_obex_fs = {
  {                                     // conf1
    sizeof(usb_desc_configuration_t),  //      uint8  bLength;
    USB_DESCTYPE_CONFIGURATION,        //      uint8  bDescriptorType;
    sizeof(struct qusb_config_struct_obex), // uint16 wTotalLength;
    1,                                 //      uint8  bNumInterfaces;
    1,                                 //      uint8  bConfigurationValue;
    0,                                 //      uint8  iConfiguration;
    0xA0,                              //      uint8  bmAttributes;bit5-Remote Wakeup
    1,                                 //      uint8  bMaxPower;
  },
  {                                     // ifc_data
    sizeof(usb_desc_interface_t),      //      uint8  bLength;
    USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
    0,                                 //      uint8  bInterfaceNumber;
    0,                                 //      uint8  bAlternateSetting;
    2,                                 //      uint8  bNumEndpoints;
    0xff,                              //      uint8  bInterfaceClass;
    0xff,                              //      uint8  bInterfaceSubClass;
    USB_INTERFACE_PROTOCOL_SAHARA,     //      uint8  bInterfaceProtocol;
    0,                                 //      uint8  iInterface;
  },
  {
    sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
    USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
    1 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
    USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
    64, // FS=64                       //      uint16 wMaxPacketSize;
    0,                                 //      uint8  bInterval;
  },
  {
    sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
    USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
    1 | USB_EP_ADDR_OUT,               //      uint8  bEndpointAddress;
    USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
    64, // FS=64                       //      uint16 wMaxPacketSize;
    0,                                 //      uint8  bInterval;
  }
 };

PACKED struct qusb_config_struct_obex qusb_usb_config_obex_hs = {
  {                                    // conf1
    sizeof(usb_desc_configuration_t),  //      uint8  bLength;
    USB_DESCTYPE_CONFIGURATION,        //      uint8  bDescriptorType;
    sizeof(struct qusb_config_struct_obex), // uint16 wTotalLength;
    1,                                 //      uint8  bNumInterfaces;
    1,                                 //      uint8  bConfigurationValue;
    0,                                 //      uint8  iConfiguration;
    0xA0,                              //      uint8  bmAttributes;bit5-Remote Wakeup
    1,                                 //      uint8  bMaxPower;
  },
  {                                    // ifc_data
    sizeof(usb_desc_interface_t),      //      uint8  bLength;
    USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
    0,                                 //      uint8  bInterfaceNumber;
    0,                                 //      uint8  bAlternateSetting;
    2,                                 //      uint8  bNumEndpoints;
    0xff,                              //      uint8  bInterfaceClass;
    0xff,                              //      uint8  bInterfaceSubClass;
    USB_INTERFACE_PROTOCOL_SAHARA,     //      uint8  bInterfaceProtocol;
    0,                                 //      uint8  iInterface;
  },
  {
    sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
    USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
    1 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
    USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
    512, // HS=512                                      //      uint16 wMaxPacketSize;
    0,                                 //      uint8  bInterval;
  },
  {
    sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
    USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
    1 | USB_EP_ADDR_OUT,               //      uint8  bEndpointAddress;
    USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
    512, // HS=512                                      //      uint16 wMaxPacketSize;
    0,                                 //      uint8  bInterval;
  }
 };

PACKED struct qusb_config_struct_obex_ss qusb_usb_config_obex_ss = {
  {                                     // conf1
    sizeof(usb_desc_configuration_t),  //      uint8  bLength;
    USB_DESCTYPE_CONFIGURATION,        //      uint8  bDescriptorType;
    sizeof(struct qusb_config_struct_obex_ss), // uint16 wTotalLength;
    1,                                 //      uint8  bNumInterfaces;
    1,                                 //      uint8  bConfigurationValue;
    0,                                 //      uint8  iConfiguration;
    0xA0,                              //      uint8  bmAttributes;;bit5-Remote Wakeup
    1,                                 //      uint8  bMaxPower;
  },
  {                                     // ifc_data
    sizeof(usb_desc_interface_t),      //      uint8  bLength;
    USB_DESCTYPE_INTERFACE,            //      uint8  bDescriptorType;
    0,                                 //      uint8  bInterfaceNumber;
    0,                                 //      uint8  bAlternateSetting;
    2,                                 //      uint8  bNumEndpoints;
    0xff,                              //      uint8  bInterfaceClass;
    0xff,                              //      uint8  bInterfaceSubClass;
    USB_INTERFACE_PROTOCOL_SAHARA,     //      uint8  bInterfaceProtocol;
    0,                                 //      uint8  iInterface;
  },
  {
    sizeof(usb_desc_endpoint_t),       //      uint8  bLength;
    USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
    1 | USB_EP_ADDR_IN,                //      uint8  bEndpointAddress;
    USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
    1024,                              //      uint16 wMaxPacketSize;
    0,                                 //      uint8  bInterval;
  },
  {
    sizeof(usb_ep_comp_desc_t),         //      uint8  bLength;
    USB_DESCTYPE_EP_COMP,               //      uint8  bDescriptorType;
    QUSB_BULK_IN_BURST_SIZE,            //      uint8  bMaxBurst
    0,                                  //      uint8  bmAttributes;
    0,                                  //      uint16  wBytesPerInterval
  },	
  {
    sizeof(usb_desc_endpoint_t),        //      uint8  bLength;
    USB_DESCTYPE_ENDPOINT,             //      uint8  bDescriptorType;
    1 | USB_EP_ADDR_OUT,               //      uint8  bEndpointAddress;
    USB_EP_ATTR_BULK,                  //      uint8  bmAttributes;
    1024,                               //      uint16 wMaxPacketSize;
    0,                                 //      uint8  bInterval;
  },
  {
    sizeof(usb_ep_comp_desc_t),       //      uint8  bLength;
    USB_DESCTYPE_EP_COMP,             //      uint8  bDescriptorType;
    QUSB_BULK_OUT_BURST_SIZE,         //      uint8  bMaxBurst
    0,                                //      uint8  bmAttributes;
    0,                                //     uint16  wBytesPerInterval
  },
 };

static uint8 str0[4] = {
  sizeof(str0),USB_DESCTYPE_STRING,
  0x09,0x04,                            // langid : US_EN
};

static uint8 str_manuf[62] = {                       // iManufacturer
  sizeof(str_manuf),USB_DESCTYPE_STRING,
  'Q',0,
  'u',0,
  'a',0,
  'l',0,
  'c',0,
  'o',0,
  'm',0,
  'm',0,
  ' ',0,
  'C',0,
  'D',0,
  'M',0,
  'A',0,
  ' ',0,
  'T',0,
  'e',0,
  'c',0,
  'h',0,
  'n',0,
  'o',0,
  'l',0,
  'o',0,
  'g',0,
  'i',0,
  'e',0,
  's',0,
  ' ',0,
  'M',0,
  'S',0,
  'M',0,
};


static const uint8 str_serial[34] = {                       // iSerialNumber
  sizeof(str_serial),USB_DESCTYPE_STRING,
  '1',0,
  '2',0,
  '3',0,
  '4',0,
  '5',0,
  '6',0,
  '7',0,
  '8',0,
  '9',0,
  '0',0,
  'A',0,
  'B',0,
  'C',0,
  'D',0,
  'E',0,
  'F',0,
};

//serial string details.
static uint8 serial_number_string[QUSB_STRING_DESC_SIZE_MAX] = {
  QUSB_STRING_DESC_LEN_AND_DESC_FIELD_SIZE, USB_DESCTYPE_STRING, 0};

/*****software descriptors*****/
static struct qusb_dcd_dsc_device sw_device;
static struct qusb_dcd_dsc_configuration sw_config;
static struct qusb_dcd_dsc_configuration sw_config_other_speed;
static struct qusb_dcd_dsc_interface sw_data_ifc;
static struct qusb_dcd_dsc_bos sw_bos; 
static struct qusb_dcd_dsc_qual sw_qual;
static struct qusb_dcd_dsc_ss_cap sw_ss_cap; 

static struct qusb_dcd_dsc_string sw_strings[] = {
 {
  &sw_strings[1],                //   struct qusb_dcd_dsc_string* next;
  (struct usb_desc_header*)str0, //   struct usb_desc_header* descriptor;
  0,                             //   uint16 langid;
  0,                             //   uint8 index;
 },
 {
  &sw_strings[2],                //   struct qusb_dcd_dsc_string* next;
  (struct usb_desc_header*)str_manuf, //   struct usb_desc_header* descriptor;
  0x0409,                        //   uint16 langid;
  1,                             //   uint8 index;
 },
 {
  &sw_strings[3],                //   struct qusb_dcd_dsc_string* next;
  NULL,                          //   struct usb_desc_header* descriptor; updated during fd_init
  0x0409,                        //   uint16 langid;
  2,                             //   uint8 index;
 },
 {
  NULL,                          //   struct qusb_dcd_dsc_string* next;
  (struct usb_desc_header*)str_serial, //   struct usb_desc_header* descriptor;
  0x0409,                        //   uint16 langid;
  3,                             //   uint8 index;
 },
};

static struct qusb_dcd_dsc_endpoint sw_data_eps[2] = {
  {  // IN
    &sw_data_eps[1],         //   struct qusb_dcd_dsc_endpoint* next;
    CUSTOMIZE,               //   struct usb_desc_endpoint* descriptor;
    &sw_data_ifc,            //   struct qusb_dcd_dsc_interface* interface;
    NULL,                    //   int (*setup)(...);
  },
  {  // OUT
    NULL,                    //   struct qusb_dcd_dsc_endpoint* next;
    CUSTOMIZE,               //   struct usb_desc_endpoint* descriptor;
    &sw_data_ifc,            //   struct qusb_dcd_dsc_interface* interface;
    NULL,                    //   int (*setup)(...);
  },
};

static struct qusb_dcd_dsc_interface sw_data_ifc = {
  NULL,                        //  struct qusb_dcd_dsc_interface* next;
  CUSTOMIZE,                   //  struct usb_desc_interface* descriptor;
  &sw_config,                  //  struct qusb_dcd_dsc_configuration* configuration;
  sw_data_eps,                 //  struct qusb_dcd_dsc_endpoint* endpoints;
  NULL,                        //  int (*setup)(...);
};

static struct qusb_dcd_dsc_qual sw_qual = {
  NULL,                       // struct qusb_dcd_dsc_qual* next;
  CUSTOMIZE,                  // struct usb_desc_dev_qual* descriptor; 
};


static struct qusb_dcd_dsc_bos sw_bos = {
  NULL,                       //  struct qusb_dcd_dsc_bos* next;
  CUSTOMIZE,                  //  struct usb_bos_desc* descriptor; 
  CUSTOMIZE,                  //  struct qusb_dcd_dsc_ss_cap* ss_capability;
};

static struct qusb_dcd_dsc_ss_cap sw_ss_cap = {
  NULL,                       //  struct qusb_dcd_dsc_ss_cap* next; 
  CUSTOMIZE,                  //  struct usb_ss_cap_desc* descriptor
};

static struct qusb_dcd_dsc_configuration sw_config = {
  NULL,                        //  struct qusb_dcd_dsc_configuration* next;
  CUSTOMIZE,                   //  struct usb_desc_configuration* descriptor;
  &sw_device,                  //  struct qusb_dcd_dsc_device* device;
  CUSTOMIZE,                   //  struct qusb_dcd_dsc_interface* interfaces;
};

static struct qusb_dcd_dsc_configuration sw_config_other_speed = {
  NULL,                        //  struct qusb_dcd_dsc_configuration* next;
  CUSTOMIZE,                   //  struct usb_desc_configuration* descriptor;
  &sw_device,                  //  struct qusb_dcd_dsc_device* device;
  CUSTOMIZE,                   //  struct qusb_dcd_dsc_interface* interfaces;
};


static struct qusb_dcd_dsc_device sw_device = {
  CUSTOMIZE,                   //  struct usb_desc_device* descriptor;
  &sw_config,                  //  struct qusb_dcd_dsc_configuration* configurations;
  sw_strings,                  //  struct qusb_dcd_dsc_string* strings;
  &sw_bos,                     //  struct qusb_dcd_dsc_bos* bos_data
  &sw_config_other_speed,      //  struct qusb_dcd_dsc_configuration* configurations;
  &sw_qual,                    //  struct qusb_dcd_dsc_qual sw_qual;
  0,                           //  uint32 core_id; Default is HS1 core.
  qusb_notify_cb,              //  void (*notify)(struct qusb_dcd_dsc_device* dev, uint32 event);
  NULL,                        //  int (*setup)(...);
};

/************* Buffers for Tx/Rx *************/

__QUSB_DMA_ZI__ static uint8 qusb_tx_buffer[TX_BUF_SIZE];
static struct qusb_urb qusb_tx_urb;
static struct qusb_urb qusb_rx_urb;
static void (*user_tx_callback)(struct qusb_urb* urb);
static void (*user_rx_callback)(struct qusb_urb* urb);
static void (*user_enum_complete)(boolean enum_flag);
static void (*user_error_handler)(void);
static boolean qusb_cancel_transfer = FALSE;

// Initialize with default
static uint16 user_product_id = USB_PRODUCT_DIAG_ONLY;
static uint8 user_serial_number = 0x0;
static uint8* user_str_product = NULL;

//Keep the default buffer mode as FD_CLIENT_SUPPLIED_BUFFER_MODE
static qusb_transfer_mode qusb_buffer_mode = FD_CLIENT_SUPPLIED_BUFFER_MODE; 

/* for DMSS and DLOAD protocols fixed sized buffer from the USB layer */
static struct qusb_urb qusb_rx_urb_fixed[2];
__QUSB_DMA_ZI__ static uint8 qusb_rx_buffer_fixed[2][RX_BUF_SIZE_FIXED];

#define QUSB_FD_PRODUCT_STRING_DESC_LENGTH 128

static char qusb_product_str_descriptors[QUSB_FD_PRODUCT_STRING_DESC_LENGTH];


/** 
 * API is used to read the PID and serial number 
 * data from the shared IMEM and update in SBL. 
 * This is done to ensure that we have the same PID
 * and Serial string as being used in HLOS.
 * 
 * @param None
 *
 * @return None
 */ 
static void qusb_fd_update_pid_and_serial_number_string(uint16 product_id, uint8 serial_number)
{
  //Offset to serial number string in the String Descriptor array
  int i = QUSB_STRING_DESC_LEN_AND_DESC_FIELD_SIZE; 
  int j = 0; //index to access serial number string from IMEM
  int k = 0; 
  uint8* str_ptr = NULL; 
  uint32 *info_ptr; 
  
  qusb_dload_info_type* dload_info = qusb_boot_dload_get_dload_info_ptr(); 
  if (NULL != dload_info)
  {
    //typecast to 32 bit to read the dload_info
    info_ptr = (uint32*) dload_info; 
  }
  else
  {
    //We will never hit this case as we will always get the address
    //If for some reason it is NULL then we will use the default PID
    //and serial string and port hopping would happen
    qusb_error_log(HLOS_PASSED_NULL_BUFFER_PORT_HOP_LOG, 0, 0x0);
    return; 
  }
  for(k= 0; k < QUSB_DLOAD_SIZE; k++)
  {
    *(qusb_dload_info + k) = *(info_ptr + k); 
  }
  if(qusb_boot_dload_is_dload_info_pid_valid((qusb_dload_info_type*)qusb_dload_info))
  {
    qusb_usb_device_obex.idProduct = (uint16) qusb_dload_info[0];
  }
  else //if the data is not valid
  {
    qusb_usb_device_obex.idProduct = product_id;
  }
  if(qusb_boot_dload_is_dload_info_serial_num_valid((qusb_dload_info_type*)qusb_dload_info))
  {
    qusb_usb_device_obex.iSerialNumber = 0x3;
    //Read the serial number from IMEM and convert to Unicode string
    str_ptr = (uint8*)qusb_dload_info; 
    //Since first 4 bytes are Product ID in the structure increment pointer 
    str_ptr = str_ptr + 4; 
    // "i" starts from index 2 in STRING DESCRIPTOR where UNICODE string starts
    //while loop checks for the following:
    // (1) NULL Termination (2) Source length  (3) destination buffer length
    while(str_ptr[j] && (j < QUSB_SERIAL_NUMBER_LEN) && (i < QUSB_STRING_DESC_SIZE_MAX))
    {
      serial_number_string[i++] = str_ptr[j]; 
      serial_number_string[i++] = 0x0; 
      j++; 
    }
    //Set the bLength field to value of i which is the sum of following: 
    //bLength field + bDescriptorType field + Unicode string length
    serial_number_string[0] = i; 
    //Update the descriptor structure to point to non-default serial number string 
    sw_device.strings[3].descriptor = (struct usb_desc_header*)(void*)serial_number_string;  
  }
  else
  { 
    //We will never hit this case as we will always get the address
    //If for some reason it is NULL then we will use the default PID
    //and serial string and port hopping would happen
    qusb_error_log(HLOS_PASSED_NULL_BUFFER_PORT_HOP_LOG, 0, 0x0);
    qusb_usb_device_obex.iSerialNumber = serial_number; 
  }
}

static char* qusb_fd_speed_to_str(qusb_device_connect_speed_t speed)
{
  switch(speed)
  {
    case QUSB_HIGH_SPEED:           return "HIGH";
    case QUSB_FULL_SPEED:           return "FULL";
    case QUSB_LOW_SPEED:            return "LOW";
    case QUSB_FULL_SPEED_48:        return "FULL-48";
    case QUSB_SUPER_SPEED:          return "SUPER";
    case QUSB_UNKNOWN_SPEED:
    default:                        return "UNKNOWN";
  }
}

static void qusb_fd_update_descriptor(qusb_device_connect_speed_t speed)
{
  /**
  * Depending on speed, set bulk EP max. packet size 
  *  
  * USB 2.0 #5.8.3 specifies bulk max. packet size 
  *  - for low speed - none, bulk EP not supported 
  *  - for full speed one of {8,16,32,64}
  *  - for high speed - 512
  *  - for Super Speed - 1024 
  */

  /* Allocate descriptors based on connection speed */
  switch(speed)
  {
    case QUSB_SUPER_SPEED:
    {
      sw_data_eps[0].descriptor = &qusb_usb_config_obex_ss.ep_in;
      sw_data_eps[1].descriptor = &qusb_usb_config_obex_ss.ep_out;
      sw_data_ifc.descriptor = &qusb_usb_config_obex_ss.ifc_data;
      sw_config.interfaces = &sw_data_ifc;
      sw_config.descriptor = &qusb_usb_config_obex_ss.conf1;
      sw_device.descriptor = &qusb_usb_device_obex;
      sw_device.descriptor->bcdUSB = QUSB_BCD_USB3;
      sw_device.descriptor->bMaxPacketSize0 = QUSB_EP0_MAX_PACKET_SIZE_SUPER_BMAX0;

      // SS specific : BOS and SS Capability descriptors
      sw_ss_cap.descriptor = &qusb_usb_bos_obex.ss_cap_desc;
      sw_bos.descriptor = &qusb_usb_bos_obex.bos_desc;
      sw_bos.ss_capability = &sw_ss_cap;
    }
    break;

    case QUSB_HIGH_SPEED:
    {
      sw_data_eps[0].descriptor = &qusb_usb_config_obex_hs.ep_in;
      sw_data_eps[1].descriptor = &qusb_usb_config_obex_hs.ep_out;
      sw_data_ifc.descriptor = &qusb_usb_config_obex_hs.ifc_data;
      sw_config.interfaces = &sw_data_ifc;
      sw_config.descriptor = &qusb_usb_config_obex_hs.conf1;
      sw_device.descriptor = &qusb_usb_device_obex;
      sw_device.descriptor->bMaxPacketSize0 = QUSB_EP0_MAX_PACKET_SIZE_HIGH;

      // HS specific : Device Qualifier and Other Speed Configuration Descriptors
      sw_qual.descriptor = &qusb_desc_dev_qual;     
      sw_config_other_speed.descriptor = &qusb_usb_config_obex_fs.conf1;

      // Reset the descriptor types based on current speed
      sw_config.descriptor->bDescriptorType = USB_DESCTYPE_CONFIGURATION;
      sw_config_other_speed.descriptor->bDescriptorType = USB_DESCTYPE_OTH_SPEED;

      // If MaxSpeed is SS advertise as USB 2.1 device.
      if(sw_device.speed_required == QUSB_MAX_SPEED_SUPER)
      {
        sw_device.descriptor->bcdUSB = QUSB_BCD_USB2_EXT;
        sw_qual.descriptor->bcdUSB = QUSB_BCD_USB2_EXT;
        sw_bos.descriptor = &qusb_usb_bos_obex.bos_desc;
      }
    }
    break;

    default:
    {
      sw_data_eps[0].descriptor = &qusb_usb_config_obex_fs.ep_in;
      sw_data_eps[1].descriptor = &qusb_usb_config_obex_fs.ep_out;
      sw_data_ifc.descriptor = &qusb_usb_config_obex_fs.ifc_data;
      sw_config.interfaces = &sw_data_ifc;
      sw_config.descriptor = &qusb_usb_config_obex_fs.conf1;
      sw_device.descriptor = &qusb_usb_device_obex;
      sw_device.descriptor->bcdUSB = (sw_device.speed_required == QUSB_MAX_SPEED_HIGH) ? QUSB_BCD_USB2 : QUSB_BCD_USB2_EXT;
      sw_device.descriptor->bMaxPacketSize0 = QUSB_EP0_MAX_PACKET_SIZE_FULL;

      // Other descriptors
      sw_qual.descriptor = &qusb_desc_dev_qual;
      sw_config_other_speed.descriptor = &qusb_usb_config_obex_hs.conf1;

      // Reset the descriptor types based on current speed
      sw_config.descriptor->bDescriptorType = USB_DESCTYPE_CONFIGURATION;
      sw_config_other_speed.descriptor->bDescriptorType = USB_DESCTYPE_OTH_SPEED;

      if(sw_device.speed_required == QUSB_MAX_SPEED_SUPER)
      {
        sw_device.descriptor->bcdUSB = QUSB_BCD_USB2_EXT;
        sw_qual.descriptor->bcdUSB = QUSB_BCD_USB2_EXT;
        sw_bos.descriptor = &qusb_usb_bos_obex.bos_desc;
      }
    }
  }

  sw_device.strings[2].descriptor = (struct usb_desc_header*)(void*)user_str_product;
  qusb_usb_device_obex.idProduct = user_product_id;

  if(user_product_id == USB_PRODUCT_DIAG_ONLY)
  {
    qusb_fd_update_pid_and_serial_number_string(user_product_id, user_serial_number);
  }
  qusb_uart_log(qusb_fd_speed_to_str(speed), qusb_usb_device_obex.idProduct);

}

/**
 * At the end of Tx transfer, mark packet as done
 * 
 * @param urb
 */
static void qusb_tx_callback(struct qusb_urb* urb)
{
  qusb_log(HDLC_TX_CALLBACK_LOG,0,0);
  if(NULL == urb)
  {
    qusb_error_log(FD_NULL_POINTER_ERROR_LOG, 0, 0);
    return; 
  }
  urb->transfer_length = 0;
  if (user_tx_callback != NULL)
  {
    user_tx_callback(urb);
  }
}

/**
 * As soon as Rx completed,
 * Rx is idle and it is possible to submit another URB 
 *  
 * After this, (possibly time consuming process) user callback 
 * called. 
 *  
 * @note may block 
 *  
 * @param urb
 */
static void qusb_rx_callback(struct qusb_urb* urb)
{
  if(NULL != urb)
  {
    qusb_log(HDLC_RX_CALLBACK_ACTUAL_LENGTH_LOG,0,urb->actual_length);
    qusb_log(HDLC_RX_CALLBACK_TRANSFER_LENGTH_LOG,0,urb->transfer_length);
    user_rx_callback(urb);
  }
  else
  {
    qusb_error_log(FD_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }
}

static void qusb_fd_init_urb(void)
{

  static struct qusb_urb* urb;

  /* Data buffers & URB's */
  /* tx */
  urb                      = &qusb_tx_urb;
  urb->usb_device          = &sw_device;
  urb->endpoint_address    = sw_data_eps[0].descriptor->bEndpointAddress;
  urb->transfer_buffer_ptr = qusb_tx_buffer;
  urb->transfer_length     = 0;
  urb->transfer_status     = 0;
  urb->send_zlp            = TRUE;
  urb->complete_callback   = qusb_tx_callback;
    
  if (qusb_buffer_mode == FD_CLIENT_SUPPLIED_BUFFER_MODE) /* Client Supplied Buffer Mode */
  {
    /* rx */
    urb                      = &qusb_rx_urb;
    urb->usb_device          = &sw_device;
    urb->endpoint_address    = sw_data_eps[1].descriptor->bEndpointAddress;
    urb->transfer_buffer_ptr = NULL; /* to be filled by al layer */
    urb->transfer_length     = 0; /* to be filled by al layer */
    urb->send_zlp            = FALSE;
    urb->complete_callback   = qusb_rx_callback;
    urb->actual_length       = 0;
  }
  else /* Internal Buffer Mode */
  {
    uint8 i = 0;
    for (i = 0; i < 2; i++) 
    {
      urb                      = &qusb_rx_urb_fixed[i];
      urb->usb_device          = &sw_device;
      urb->endpoint_address    = sw_data_eps[1].descriptor->bEndpointAddress;
      urb->transfer_buffer_ptr = qusb_rx_buffer_fixed[i];
      urb->transfer_length     = RX_BUF_SIZE_FIXED;
      urb->send_zlp            = FALSE;
      urb->complete_callback   = qusb_rx_callback_int_buf_mode;
      urb->actual_length       = 0;
    }
  }
}

static void qusb_notify_cb(struct qusb_dcd_dsc_device* dev, qusb_notify_events_t event)
{
  if(NULL == dev)
  {
    qusb_error_log(FD_NULL_POINTER_ERROR_LOG, 0, __LINE__);
    return;
  }
  qusb_log(HDLC_NOTIFY_CB_LOG,0,0);
  switch (event) 
  {
    case QUSB_DCD_EVT_CONFIG:
      qusb_log(NOTIFY_EVT_CFG_LOG,0,1);
      qusb_fd_init_urb();
      if(qusb_buffer_mode == FD_USB_INTERNAL_BUFFER_MODE)
      {
        qusb_init_rx_int_buf_mode(dev);
      }
      if (user_enum_complete != NULL)
      {
        user_enum_complete(TRUE);
      }
    break;

    case QUSB_DCD_EVT_UNCONFIG:
      qusb_log(NOTIFY_EVT_CFG_LOG,0,0);
      if (user_enum_complete != NULL)
      {
        user_enum_complete(FALSE);
      }
    break;

    case QUSB_DCD_EVT_SPEED:
    {
      qusb_log(NOTIFY_EVT_SPEED_LOG,0,dev->speed);
      qusb_fd_update_descriptor(dev->speed);
    }
    break;

    case QUSB_DCD_EVT_DISCONNECT: 
    {
      qusb_log(NOTIFY_EVT_DISCONNECT_LOG,0,0);
      /*Now a disconnect event has occured during the transfer*/
      if(NULL != user_error_handler)
      {
        qusb_cancel_transfer = TRUE; 
        qusb_tx_urb.transfer_status = QUSB_URB_STATUS_COMPLETE_ERROR;
        qusb_rx_urb.transfer_status = QUSB_URB_STATUS_COMPLETE_ERROR;
        user_error_handler();
      }
    }
    break;
    case QUSB_DCD_EVT_SUSPEND:
    {
      //FixMe: Sequence to be followed on Suspend
      qusb_log(NOTIFY_EVT_SUSPEND_LOG,0,0);

    }
    break;

    default:
    break;
  }
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
void qusb_fd_set_core(int core_id)
{
  qusb_log(FD_HDLC_SET_CORE_LOG,0,core_id);
  //for synopsys controller, make sure that the correct core_id is set
  if (core_id != DCI_SNPS_CORE)
  {
    qusb_error_log(FD_CORE_ID_WRONG_LOG, 0, __LINE__); 
  }
  else
  {
    sw_device.core_id = core_id;
  }
}

struct qusb_dcd_dsc_device* qusb_fd_init(void (*rx_callback)(struct qusb_urb* urb),
                                             char* str_product,
                                             void (*tx_callback)(struct qusb_urb* urb),
                                             void (*enum_complete)(boolean enum_flag),
                                             void (*error_handler)(void),
                                             uint16 product_id,
                                             qusb_max_speed_required_t speed_required)
{
  qusb_log(FD_HDLC_INIT_LOG,0,0);

  if(rx_callback)
  {
    user_rx_callback = rx_callback;
  }
  if(tx_callback)
  {
    user_tx_callback = tx_callback;
  }
  if(enum_complete)
  {
    user_enum_complete = enum_complete;
  }
  if(error_handler)
  {
    user_error_handler = error_handler;
  }
  if(str_product)
  {
    if (0 != qusb_get_product_str_desc_with_chip_serial(
                str_product,
                (char *)&qusb_product_str_descriptors,
                QUSB_FD_PRODUCT_STRING_DESC_LENGTH))
    {
      user_str_product = (uint8*)qusb_product_str_descriptors;
    }  
  }
  if(product_id)
  {
    user_product_id = product_id;
  }

  sw_device.speed_required = QUSB_MAX_SPEED_SUPER;
  qusb_dcd_init(&sw_device);
  return &sw_device;
}

void qusb_fd_set_shutdown_mode(qusb_shutdown_mode_t mode)
{
  sw_device.shutdown_mode = mode;
}

qusb_shutdown_mode_t qusb_fd_get_shutdown_mode(void)
{
  return sw_device.shutdown_mode;
}

void qusb_fd_shutdown(void)
{
   qusb_log(FD_HDLC_SHUTDOWN_LOG,0,sw_device.shutdown_mode);
   qusb_dcd_shutdown(&sw_device);
}

void qusb_fd_tx_byte(unsigned char chr)
{
  struct qusb_urb* urb = &qusb_tx_urb; 
  qusb_log(FD_TX_BYTE_LOG, 0, chr); 
  //check for the current transfer buffer status
  //ensure that the transfer completes 
  while( QUSB_URB_STATUS_ACTIVE == urb->transfer_status)
  {
    qusb_dcd_poll(&sw_device);
  }
  ((uint8*)(urb->transfer_buffer_ptr))[urb->transfer_length++] = chr;
  if(((0x7E == chr) && (urb->transfer_length > 1)) || 
    (urb->transfer_length >= TX_BUF_SIZE))
  {
    //send the data
    qusb_submit_urb(urb);
  }
}

void qusb_fd_tx_pkt(unsigned char *pkt, uint32 len)
{
  struct qusb_urb* urb = &qusb_tx_urb;

  qusb_log(FD_HDLC_TX_PKT_LOG,0,len);
  while ( QUSB_URB_STATUS_ACTIVE == urb->transfer_status ) 
  {
    qusb_dcd_poll(&sw_device);
    if (TRUE == qusb_cancel_transfer)
    {
      qusb_cancel_transfer= FALSE;
      break;
    }
  }

  // in most case, we should be able avoid memcpy 
  urb->transfer_buffer_ptr = pkt;
  urb->transfer_length = len;

  qusb_log(TRANSFER_LENGTH_LOG,0,urb->transfer_length);
  // send urb 
  qusb_submit_urb(urb);
  //wait here until transfer is finished 
  while ( QUSB_URB_STATUS_ACTIVE == urb->transfer_status )
  {
    if (TRUE == qusb_cancel_transfer)
    {
      qusb_cancel_transfer= FALSE;
      break;
    }
    qusb_dcd_poll(&sw_device);
  }
  // restore tx urb buffer 
  urb->transfer_buffer_ptr = qusb_tx_buffer;
}

int qusb_fd_is_tx_active(void)
{
  if ( ( QUSB_URB_STATUS_ACTIVE == qusb_tx_urb.transfer_status ) || 
  ( QUSB_URB_STATUS_ACTIVE == sw_device.setup_tx_urb.transfer_status ) ) 
  {
    qusb_log(FD_HDLC_IS_TX_ACTIVE_LOG,0,1);
    return 1;
  }
  qusb_log(FD_HDLC_IS_TX_ACTIVE_LOG,0,0);
  return 0;
}

uint32 qusb_fd_rx_pkt(void *buff_ptr, uint32 len)
{
  struct qusb_urb* urb = &qusb_rx_urb;
  qusb_log(FD_HDLC_RX_PKT_LOG,0,len);
  // handle previously active RX transfers 
  if (QUSB_URB_STATUS_ACTIVE == qusb_rx_urb.transfer_status)
  {
    while (QUSB_URB_STATUS_ACTIVE == qusb_rx_urb.transfer_status)
    {
      qusb_dcd_poll(&sw_device); /* finish the previous transfer internally before proceeding */
      if (TRUE == qusb_cancel_transfer)
      {
        qusb_cancel_transfer= FALSE;
        break;
      }
    }
  }

  //avoid memcpy for all cases except destination address 0 
  if ((NULL == buff_ptr) && (len > 0))
  {
    // TODO give hardware software allocated memory 
    urb->transfer_buffer_ptr = buff_ptr;
  }
  else
  {
    urb->transfer_buffer_ptr = buff_ptr;
  }
  urb->transfer_length = len;
  qusb_log(SUBMIT_RX_URB_LOG,0,len); 
  // send urb 
  qusb_submit_urb(urb);

  return FD_SUCCESS;
}

void qusb_fd_rx_completed(struct qusb_urb* urb)
{
  if(NULL != urb)
  {
    qusb_log(FD_HDLC_RX_COMPLETED_LOG,0,urb->actual_length);
    urb->actual_length = 0;
  }
  else
  {
    qusb_error_log(FD_NULL_POINTER_ERROR_LOG, 0, __LINE__);
  }
}

void qusb_fd_set_buffer_mode(qusb_transfer_mode buffer_mode)
{
  qusb_log(FD_SET_BUFFER_MODE_LOG, 0,buffer_mode); 
  qusb_buffer_mode = buffer_mode; 
  return; 
}

/**
 * RX complete for internal buffer mode
 */
static void qusb_rx_callback_int_buf_mode(struct qusb_urb* urb)
{
  qusb_log(RX_CALLBACK_INT_BUF_MODE_LOG, 0, 0);
  if(NULL != urb)
  {
    qusb_init_rx_int_buf_mode(urb->usb_device);
    user_rx_callback(urb);
  }
  else
  {
    qusb_error_log(FD_NULL_POINTER_ERROR_LOG, 0, __LINE__);
  }
  return; 
}

void qusb_rx_completed_int_buf_mode(struct qusb_urb* urb)
{
  if(NULL != urb)
  {
    qusb_log(RX_COMPLETED_INT_BUF_MODE_LOG, 0, urb->actual_length);
    urb->actual_length = 0x0; 
    qusb_init_rx_int_buf_mode(urb->usb_device);
  }
  else
  {
    qusb_error_log(FD_NULL_POINTER_ERROR_LOG, 0, __LINE__);
  }
  return;
}

/** Internal Buffer Mode
 *
 * Try to submit the Rx URB
 * No more then 1 URB may be active at a time, thus check for Rx
 * idle. If no Rx is in progress, find first available URB that 
 * is not currently used, i.e. actual_length =  0x0
 * 
 * @Note May block waiting while Rx URB is successfully
 *       submitted
 *
 */ 
static void qusb_init_rx_int_buf_mode(const struct qusb_dcd_dsc_device* dev)
{
  if((QUSB_URB_STATUS_ACTIVE != qusb_rx_urb_fixed[0].transfer_status) &&
    (QUSB_URB_STATUS_ACTIVE != qusb_rx_urb_fixed[1].transfer_status))
  {
    int i; 
    for(i=0; i<2; i++)
    {
      struct qusb_urb* urb = &qusb_rx_urb_fixed[i];
      if(0 == urb->actual_length)
      {
        while(0 != qusb_submit_urb(urb))
        {
          qusb_dcd_poll(&sw_device);
          if(0 == dev->config_value)
          {
            return; 
          }
        }
        return;
      }
    }
  }
}




