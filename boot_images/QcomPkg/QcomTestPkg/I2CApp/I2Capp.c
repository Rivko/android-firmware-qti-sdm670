/** @file I2Capp.c

  I2C Application

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

#include "I2Capp.h"

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

 **/
EFI_STATUS
EFIAPI I2CTestMain (IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  UINTN  argc;
  CHAR8  **argv = NULL;

  EFI_STATUS  status = EFI_DEVICE_ERROR;
  i2c_status istatus = I2C_SUCCESS;

  TEST_START("I2C Test");

  status = GetCmdLineArgs (ImageHandle, &argc, &argv);
  if (status != EFI_SUCCESS)
  {
    goto error;
  }

  if(argc == 0)
  {
    istatus = i2c_test_eeprom ();
  }
  else
  {
    istatus = i2c_test_generic (argc, argv);
  }

error:

  if (istatus != I2C_SUCCESS)
  {
    status = EFI_DEVICE_ERROR;
  }

  TestStatus("I2C TEST", status);
  TEST_STOP ("I2C Test");

  return status;
}
