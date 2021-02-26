/** @file

    This file defines global variables used for USB host mode electrical testing.

Portions copyright (c) 2010 - 2013, 2015 , 2018 by Qualcomm Technologies, Inc. All Rights Reserved
Copyright (c) 2007 - 2010, Intel Corporation. All rights reserved.<BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

/*=============================================================================
                              EDIT HISTORY

 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 04/27/15   ck       Remove unused p4 tags
 06/14/13   mliss    updated to support providing PortTest protocol
 12/08/11   wufan    initial draft

=============================================================================*/

#ifndef _EFI_EHCI_PORT_TEST_H_
#define _EFI_EHCI_PORT_TEST_H_


#include <Protocol/EFIUsbPortTest.h>


//
// The control variable for HS USB host mode electrical testing.
// Search HSECT in EhciUrb.c for more details.
//
#define USB_ET_QTD_INDEX_NAME      L"USB_ET_QTD_INDEX"

//
// Usb port test variable
//
#define USB_PORT_TEST_VAR_UNDEFINED  -1

/**
  Check the gloabl variable USB_ET_QTD_INDEX_NAME for USB port testing.

  @retval  USB_PORT_TEST_VAR_UNDEFINED if failed to retrieve the variable,
           otherwise the value of the variable.

**/
INTN EhcCheckPortTestVariable ( USB2_HC_DEV *Ehc );


/**
  The global UsbPortTest protocol instance for Ehci driver.
**/
extern QCOM_USB_PORT_TEST_PROTOCOL gEhciUsbPortTestTemplate;


#endif