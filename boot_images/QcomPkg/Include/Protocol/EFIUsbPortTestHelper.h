/** @file UsbPortTestHelper.h
   
  QCOM_USB_PORT_TEST_HELPER_PROTOCOL definition. 
  This protocol helps HS USB host mode electrical testing at transfer level. 

  Copyright (c) 2010 - 2011, Qualcomm Technologies,, Inc. All rights reserved.

**/


/*=============================================================================
                              EDIT HISTORY


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 12/08/11   wufan    Initial draft.
 
=============================================================================*/

#ifndef _USB_PORT_TEST_HELPER_PRO_H_
#define _USB_PORT_TEST_HELPER_PRO_H_

///
/// Forward reference for pure ANSI compatibility.
///
typedef struct _QCOM_USB_PORT_TEST_HELPER_PROTOCOL QCOM_USB_PORT_TEST_HELPER_PROTOCOL;


/**
  Get the device number that of the first device connected to the root hub, 
  and the parent port number for the device. 
  There should be only one device connected to the root hub for USB 
  host mode electrical testing. 
   
  @param DeviceNumber     Device number
  @param PortNumber       Port number in root hub. 

  @retval EFI_SUCCESS        Success
  @retval EFI_DEVICE_ERROR   Unsupported TestMode. 
  @retval EFI_NOT_FOUND      no USB Bus protocol instance is found

**/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_PORT_TEST_HELPER_PROTOCOL_GET_DEV_INFO) (
  IN  QCOM_USB_PORT_TEST_HELPER_PROTOCOL  *This,
  OUT UINT8                               *DeviceNumber, 
  OUT UINT8                               *PortNumber
  );


/**
  Trigger a control transfer to get device descriptor. 
  Used in Host Control Packet Parameters testing. 

  @retval EFI_SUCCESS             Success
  @retval EFI_DEVICE_ERROR        Failed to trigger the transfer. 
  @retval EFI_INVALID_PARAMETER   Can't convert This to USB_BUS.  
  @retval EFI_NOT_FOUND           no USB Bus protocol instance is found

**/
typedef
EFI_STATUS
(EFIAPI *QCOM_USB_PORT_TEST_HELPER_PROTOCOL_GET_DEV_DESC) (
  IN  QCOM_USB_PORT_TEST_HELPER_PROTOCOL        *This 
  );



#define QCOM_USB_PORT_TEST_HELPER_PROTOCOL_REVISION  0x00010000

///
/// QCOM_USB_PORT_TEST_HELPER_PROTOCOL
///
struct _QCOM_USB_PORT_TEST_HELPER_PROTOCOL {
  UINT32                                               Revision;
  QCOM_USB_PORT_TEST_HELPER_PROTOCOL_GET_DEV_INFO      GetDeviceInfo;  
  QCOM_USB_PORT_TEST_HELPER_PROTOCOL_GET_DEV_DESC      GetDeviceDescriptor;
};

extern EFI_GUID gQcomUsbPortTestHelperProtocolGuid;

#endif


