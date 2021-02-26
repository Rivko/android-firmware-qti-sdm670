/** @file I2Capp.h

  I2C Application Header

             Copyright (c) 2014 Qualcomm Technologies Incorporated.
                              All Rights Reserved.
                     Qualcomm Confidential and Proprietary

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 08/21/14   unr     Created

=============================================================================*/

#ifndef _I2C_APP_H_
#define _I2C_APP_H_

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Library/QcomLib.h>
#include <Library/DebugLib.h>

#include "i2c_api.h"

#define I2C_DEFAULT_BUS_FREQ 400

i2c_status i2c_test_eeprom (VOID);
i2c_status i2c_test_generic(UINTN argc, CHAR8 **argv);

VOID i2c_test_hex_dump (UINT8 *buffer, UINT32 length);

#endif /* _I2C_APP_H_ */
