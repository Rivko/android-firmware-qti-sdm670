/** @file I2Ceeprom.c

  I2C EEPROM Application

         Copyright (c) 2010 - 2017 Qualcomm Technologies Incorporated.
                              All Rights Reserved.
                     Qualcomm Confidential and Proprietary

**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 04/10/17	ag		Updated API's to QupV3
 08/21/14   unr     Refactored

=============================================================================*/

#include "I2Capp.h"

#define DATA_SIZE 4

uint8 bbuf [DATA_SIZE] = {0, 0, 0, 0};
uint8 rbuf [DATA_SIZE] = {0, 0, 0, 0};
uint8 wbuf [DATA_SIZE] = {1, 2, 3, 4};

i2c_status i2c_test_eeprom (VOID)
{
  i2c_status istatus = I2C_SUCCESS;

  VOID      *i2c_handle      = NULL;
  UINT32    bytes_read       = 0;
  UINT32    bytes_written    = 0;
  UINT32    i                = 0;
  UINT16    offset           = 0;

  i2c_slave_config cfg;

  /* Initialize the I2C Core for eeprom */
  istatus = i2c_open((i2c_instance) (PcdGet32(EEPROMI2CCore) - 1), &i2c_handle);
  if (I2C_SUCCESS != istatus)
  {
    DEBUG((EFI_D_ERROR, "Failed to initialize I2C %d\n", istatus));
    goto error;
  }

  cfg.bus_frequency_khz = I2C_DEFAULT_BUS_FREQ;
  cfg.slave_address = PcdGet32(EEPROMSlaveAddress);
  cfg.mode = I2C;
  cfg.slave_max_clock_stretch_us = 500;
  cfg.core_configuration1 = 0;
  cfg.core_configuration2 = 0;

  istatus = i2c_read (i2c_handle, &cfg, offset, 2, bbuf, DATA_SIZE, &bytes_read, 2500);
  if (I2C_SUCCESS != istatus)
  {
    DEBUG((EFI_D_ERROR, "EEPROM Backup Failed %d\n", (uint32) istatus));
    goto error;
  }
  i2c_test_hex_dump (bbuf, DATA_SIZE);
 
  istatus = i2c_write (i2c_handle, &cfg, offset, 2, wbuf, DATA_SIZE, &bytes_written, 2500);
  if (I2C_SUCCESS != istatus)
  {
    DEBUG((EFI_D_ERROR, "EEPROM Write Failed %d\n", (uint32) istatus));
    goto error;
  }
  i2c_test_hex_dump (wbuf, DATA_SIZE);

  gBS->Stall (PcdGet32(EEPROMWriteCycleTime));

  istatus = i2c_read (i2c_handle, &cfg, offset, 2, rbuf, DATA_SIZE, &bytes_read, 2500);
  if (I2C_SUCCESS != istatus)
  {
    DEBUG((EFI_D_ERROR, "EEPROM Read Failed %d\n", (uint32) istatus));
    goto error;
  }
  i2c_test_hex_dump (rbuf, DATA_SIZE);
 
  istatus = i2c_write (i2c_handle, &cfg, offset, 2, bbuf, DATA_SIZE, &bytes_written, 2500);
  if (I2C_SUCCESS != istatus)
  {
    DEBUG((EFI_D_ERROR, "EEPROM Restore Failed %d\n", (uint32) istatus));
    goto error;
  }
  i2c_test_hex_dump (bbuf, DATA_SIZE);

  gBS->Stall (PcdGet32(EEPROMWriteCycleTime));

  /* Compare what we wrote to what we read */
  for (i = 0; i < DATA_SIZE; i++)
  {
    if (rbuf[i] != wbuf[i])
    {
      DEBUG((EFI_D_ERROR, "EEPROM Data Mismatch \n"));
      break;
    }
  }

error:

  if (i2c_handle != NULL)
  {
    if (I2C_SUCCESS != i2c_close(i2c_handle))
    {
      DEBUG((EFI_D_ERROR, "Failed to DeInitialize I2C %d\n", istatus));
    }
  }

  return istatus;
}
