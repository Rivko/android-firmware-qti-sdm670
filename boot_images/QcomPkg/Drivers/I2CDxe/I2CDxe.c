/** @file I2CDxe.c
   
  I2C UEFI Driver code

  Copyright (c) 2010-2011, Qualcomm Technologies Inc. All rights reserved.
   
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/27/14   unr     Removed local data structures and fixed 64 bit issues
 02/01/13   unr     Fixed number of bytes read
 12/23/10   unr     Initial revision of UEFI I2C Driver

=============================================================================*/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>

/**
  I2C PROTOCOL interfaces
 */
#include <Protocol/EFII2C.h>
#include "DALSys.h"

/**
  I2C UEFI Protocol implementation
 */
EFI_QCOM_I2C_PROTOCOL I2cProtocolImplementation = 
{
  I2C_DXE_REVISION,
  i2c_open,
  i2c_read,
  i2c_write,
  i2c_transfer,
  i2c_close,
};

/**
  I2C DXE Driver Entry Point
 */
EFI_STATUS
I2CProtocolInit (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable)
{
  EFI_STATUS Status;

  DALSYS_InitMod(NULL);

  Status = gBS->InstallMultipleProtocolInterfaces(
	  &ImageHandle,
	  &gQcomI2CProtocolGuid,
	  &I2cProtocolImplementation,
	  NULL);

  return Status;
}

