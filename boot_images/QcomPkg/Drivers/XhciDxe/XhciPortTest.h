/** @file XhciPortTest.h

  QCOM_USB_PORT_TEST_PROTOCOL driver.

  Copyright (c) 2011 - 2013, QUALCOMM Technologies Inc. All rights reserved.

**/
#ifndef _EFI_XHCI_PORT_TEST_H_
#define _EFI_XHCI_PORT_TEST_H_


#include <Protocol/EFIUsbPortTest.h>


#define USB_PORT_TEST_VAR_UNDEFINED     -1


/**
  The global UsbPortTest protocol instance for Xhci driver.
**/
extern QCOM_USB_PORT_TEST_PROTOCOL gXhciUsbPortTestTemplate;


#endif
