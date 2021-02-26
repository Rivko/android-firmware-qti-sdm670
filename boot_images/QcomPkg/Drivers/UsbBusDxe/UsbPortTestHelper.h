/**
  @file UsbPortTestHelper.h

  QCOM_USB_PORT_TEST_PROTOCOL implementation.

  Copyright (c) 2011 Qualcomm Technologies Inc. All rights reserved.

**/

/*=============================================================================
                              EDIT HISTORY

 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 06/13/13   mliss    Initial implementation.
=============================================================================*/
#ifndef _USB_PORT_TEST_HELPER_H_
#define _USB_PORT_TEST_HELPER_H_


#define USB_PORT_TEST_HELPER_SIGNATURE      SIGNATURE_32 ('U', 'P', 'T', 'H')

#define PORT_TEST_HELPER_FROM_THIS(this) \
          CR(this, QCOM_USB_PORT_TEST_HELPER_PRIVATE, PortTestHelper, USB_PORT_TEST_HELPER_SIGNATURE)


/**
 * Port test helper private data to associate each instance with its usb bus.
 */
typedef struct QCOM_USB_PORT_TEST_HELPER_PRIVATE {
  UINTN                               Signature;
  USB_BUS                             *UsbBus;
  QCOM_USB_PORT_TEST_HELPER_PROTOCOL  PortTestHelper;
} QCOM_USB_PORT_TEST_HELPER_PRIVATE;


/**
 * @brief Build an UsbPortTestHelperProtocol instance
 *
 * Allocate memory for and initialize an instance of the UsbPortTestHelper protocol.
 */
EFI_STATUS
UsbPortTestHelperBuildProtocol (
  IN  USB_BUS                             *UsbBus,
  OUT QCOM_USB_PORT_TEST_HELPER_PROTOCOL  **PortTestHelper
  );


#endif /* _USB_PORT_TEST_HELPER_H_ */
