/**
 * @file UsbfnIoTest.h
 *
 * Provides test functions to verify the proper operation of the UsbfnIo
 * policy protocol functions.
 *
 * Copyright (c) 2014-2015, QUALCOMM Technologies Inc. All rights reserved.
 */
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 01/20/15   ck      Remove deprecated functions
 02/06/14   ml      Initial revision
=============================================================================*/

#ifndef _USBFN_IO_TEST_H_
#define _USBFN_IO_TEST_H_


#include "UsbfnLoopback.h"


/**
 * Test the SetEndpointPolicy function with all of the supported policies.
 *
 * @param[in]     UsbfnIo        Usbfn protocol instance
 *
 * @retval EFI_SUCCESS           The test finished successfully.
 * @retval other                 The test failed.
 */
EFI_STATUS
TestUsbfnIoSetEndpointPolicy (
  IN  EFI_USBFN_IO_PROTOCOL         *UsbfnIo
  );


/**
 * Test the GetEndpointPolicy function with all of the supported policies.
 *
 * @param[in]     UsbfnIo        Usbfn protocol instance
 *
 * @retval EFI_SUCCESS           The test finished successfully.
 * @retval other                 The test failed.
 */
EFI_STATUS
TestUsbfnIoGetEndpointPolicy (
  IN  EFI_USBFN_IO_PROTOCOL         *UsbfnIo
  );


/**
 * Test the device information query protocol functions.
 *
 * @param[in]     UsbfnIo        Usbfn protocol instance
 *
 * @retval EFI_SUCCESS           The test finished successfully.
 * @retval other                 The test failed.
 */
EFI_STATUS
TestUsbfnIoQueryDeviceInformation (
  IN  EFI_USBFN_IO_PROTOCOL         *UsbfnIo
  );


#endif /* _USBFN_IO_TEST_H_ */
