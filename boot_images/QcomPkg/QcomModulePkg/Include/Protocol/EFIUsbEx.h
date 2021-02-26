/**
 * @file UsbEx.h
 *
 * Adding type definitions to facilitate the support of USB 2.x and 3.x
 *
 * Defines various USB related constants and data structure.
 *
 * Copyright (c) 2016 Qualcomm Technologies, Inc. All rights reserved.
 *
 */
 
#ifndef __USB_EX_H__
#define __USB_EX_H__

#pragma pack(1)
/// Standard Device Qualifier Descriptor
/// USB 2.0 spec, Section 9.6.2
///
typedef struct {
  UINT8           Length;
  UINT8           DescriptorType;
  UINT16          BcdUSB;
  UINT8           DeviceClass;
  UINT8           DeviceSubClass;
  UINT8           DeviceProtocol;
  UINT8           MaxPacketSize0;
  UINT8           NumConfigurations;
  UINT8           Reserved;
} USB_DEVICE_QUALIFIER_DESCRIPTOR;

///
/// Standard Binary Object Store Descriptor
/// USB 3.0 spec, Section 9.6.2
///
typedef struct {
  UINT8           Length;
  UINT8           DescriptorType;
  UINT16          TotalLength;
  UINT8           NumDeviceCaps;
} USB_BOS_DESCRIPTOR;

///
/// Standard Device Capability Descriptor
/// USB 3.0 spec, Section 9.6.2
///
typedef struct {
  UINT8           Length;
  UINT8           DescriptorType;
  UINT8           DevCapabilityType;
  UINT8           DevCapabilityData[1];
} USB_DEVICE_CAPABILITY_DESCRIPTOR;

///
/// Standard Device Capability Descriptor, USB 2.0 Extension
/// USB 3.0 spec, Section 9.6.2.1
///
typedef struct {
  UINT8           Length;
  UINT8           DescriptorType;
  UINT8           DevCapabilityType;
  UINT32          Attributes;
} USB_USB_20_EXTENSION_DESCRIPTOR;

///
/// Standard Device Capability Descriptor, SuperSpeed USB
/// USB 3.0 spec, Section 9.6.2.2
///
typedef struct {
  UINT8           Length;
  UINT8           DescriptorType;
  UINT8           DevCapabilityType;
  UINT8           Attributes;
  UINT16          SpeedsSupported;
  UINT8           FunctionalitySupport;
  UINT8           U1DevExitLat;
  UINT16          U2DevExitLat;
} USB_SUPERSPEED_USB_DESCRIPTOR;

///
/// Standard Device Capability Descriptor, Container ID
/// USB 3.0 spec, Section 9.6.2.3
///
typedef struct {
  UINT8           Length;
  UINT8           DescriptorType;
  UINT8           DevCapabilityType;
  UINT8           Reserved;
  UINT8           UUID[16];
} USB_CONTAINER_ID_DESCRIPTOR;

///
/// Standard Interface Association Descriptor
/// USB 3.0 spec, Section 9.6.4
///
typedef struct {
  UINT8           Length;
  UINT8           DescriptorType;
  UINT8           FirstInterface;
  UINT8           InterfaceCount;
  UINT8           FunctionClass;
  UINT8           FunctionSubClass;
  UINT8           FunctionProtocol;
  UINT8           Function;
} USB_INTERFACE_ASSOCIATION_DESCRIPTOR;

///
/// Standard SuperSpeed Endpoint Companion Descriptor
/// USB 3.0 spec, Section 9.6.7
///
typedef struct {
  UINT8           Length;
  UINT8           DescriptorType;
  UINT8           MaxBurst;
  UINT8           Attributes;
  UINT16          BytesPerInterval;
} USB_SS_ENDPOINT_COMPANION_DESCRIPTOR;
#pragma pack()

typedef enum {
  //
  // Standard control transfer request type, or the value
  // to fill in EFI_USB_DEVICE_REQUEST.Request
  //
  USB_REQ_SET_SEL         = 0x30,
  USB_REQ_SET_ISOCH_DELAY = 0x31,

  //
  // USB Descriptor types
  //
  USB_DESC_TYPE_DEVICE_QUALIFIER          = 0x06,
  USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION = 0x07,
  USB_DESC_TYPE_INTERFACE_POWER           = 0x08,
  USB_DESC_TYPE_OTG                       = 0x09,
  USB_DESC_TYPE_DEBUG                     = 0x0A,
  USB_DESC_TYPE_INTERFACE_ASSOCIATION     = 0x0B,
  USB_DESC_TYPE_BOS                       = 0x0F,
  USB_DESC_TYPE_DEVICE_CAPABILITY         = 0x10,
  USB_DESC_TYPE_SS_ENDPOINT_COMPANION     = 0x30,

  //
  // USB Device Capability types
  //
  USB_DEV_CAP_TYPE_WIRELESS_USB           = 0x01,
  USB_DEV_CAP_TYPE_USB_20_EXTENSION       = 0x02,
  USB_DEV_CAP_TYPE_SUPERSPEED_USB         = 0x03,
  USB_DEV_CAP_TYPE_CONTAINER_ID           = 0x04,

  //
  // Features to be cleared by CLEAR_FEATURE requests
  //
  USB_FEATURE_FUNCTION_SUSPEND  = 0,    // Recipient interface
  USB_FEATURE_U1_ENABLE         = 0x30, // Recipient device
  USB_FEATURE_U2_ENABLE         = 0x31, // Recipient device
  USB_FEATURE_LTM_ENABLE        = 0x32, // Recipient device
  USB_FEATURE_HS_TEST_MODE      = 0x2,  // Recipient device
  USB_FEATURE_DEV_REMOTE_WAKEUP = 0x1,  // Recipient device
} USB_TYPES_DEFINITION_EXT;

/** USB Binary Object Store descriptor. **/
typedef USB_BOS_DESCRIPTOR                    EFI_USB_BOS_DESCRIPTOR;
/** USB Generic Device Capability descriptor. **/
typedef USB_DEVICE_CAPABILITY_DESCRIPTOR      EFI_USB_DEVICE_CAPABILITY_DESCRIPTOR;
/** USB 2.0 Extension Device Capability descriptor. **/
typedef USB_USB_20_EXTENSION_DESCRIPTOR       EFI_USB_USB_20_EXTENSION_DESCRIPTOR;
/** USB Super-speed USB Device Capability descriptor. **/
typedef USB_SUPERSPEED_USB_DESCRIPTOR         EFI_USB_SUPERSPEED_USB_DESCRIPTOR;
/** USB Container ID Device Capability descriptor. **/
typedef USB_CONTAINER_ID_DESCRIPTOR           EFI_USB_CONTAINER_ID_DESCRIPTOR;
/** USB Interface Association descriptor. **/
typedef USB_INTERFACE_ASSOCIATION_DESCRIPTOR  EFI_USB_INTERFACE_ASSOCIATION_DESCRIPTOR;
/** USB Super-speed Endpoint Companion descriptor **/
typedef USB_SS_ENDPOINT_COMPANION_DESCRIPTOR  EFI_USB_SS_ENDPOINT_COMPANION_DESCRIPTOR;
/** USB device qualifier descriptor **/
typedef USB_DEVICE_QUALIFIER_DESCRIPTOR       EFI_USB_DEVICE_QUALIFIER_DESCRIPTOR;

#endif
