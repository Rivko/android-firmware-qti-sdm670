/** @file TsFwI2CTest.c
   
  Touch Screen I2C Test App

  Copyright (c) 2012, 2019 Qualcomm Technologies, Inc.  All Rights Reserved.
  Copyright (c) 2016, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
  
**/

/*=============================================================================
                              EDIT HISTORY


 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 05/29/12   sr      Initial version .

=============================================================================*/


#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/TestInterface.h>
#include <Protocol/EFIPmicVreg.h>
#include <Protocol/EFII2C.h>
#include <Protocol/EFITlmm.h>
#include <Protocol/EFIPmicGpio.h>

#define I2C_TS_FREQ 400

#define DATA_SIZE 4

UINT8  BackBuf[DATA_SIZE+1];
UINT8  ReadBuf[DATA_SIZE+1];

/* Test Status */
typedef enum
{
  E_SUCCESS,
  E_BACKUP_FAILED,
  E_READ_FAILED,
  E_WRITE_FAILED,
  E_RESTORE_FAILED,
  E_DATA_MISMATCH
} TS_TEST_STATUS;

/**
  Prints the error strings on console

  @param[in] error     Error code defined in EFI_QCOM_I2C_STATUS_CODES

  @retval none

 */
VOID I2cErrorPrint(EFI_QCOM_I2C_STATUS_CODES error)
{
  switch (error)
  {
    case EFI_QCOM_I2C_ERROR_OPERATION_FAILED:
      gST->ConOut->OutputString(gST->ConOut, L"OPERATION_FAILED \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_BUS_NOT_READY:
      gST->ConOut->OutputString(gST->ConOut, L"BUS_NOT_READY \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_WRITE_BUFFER_FULL:
      gST->ConOut->OutputString(gST->ConOut, L"WRITE_BUFFER_FULL \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_READ_BUFFER_EMPTY:
      gST->ConOut->OutputString(gST->ConOut, L"READ_BUFFER_EMPTY \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_BUS_NOT_IN_IDLE_STATE:
      gST->ConOut->OutputString(gST->ConOut, L"BUS_NOT_IN_IDLE_STATE \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_BUS_NOT_IN_TX_STATE:
      gST->ConOut->OutputString(gST->ConOut, L"BUS_NOT_IN_TX_STATE \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_BUS_NOT_IN_TX_OR_IDLE_STATE:
      gST->ConOut->OutputString(gST->ConOut, L"BUS_NOT_IN_TX_OR_IDLE_STATE \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_BUS_ACQUISITION_TIMEOUT:
      gST->ConOut->OutputString(gST->ConOut, L"BUS_ACQUISITION_TIMEOUT \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_BYTE_TRANSFER_TIMEOUT:
      gST->ConOut->OutputString(gST->ConOut, L"BYTE_TRANSFER_TIMEOUT \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_INVALID_ACCESS:
      gST->ConOut->OutputString(gST->ConOut, L"INVALID_ACCESS \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_INVALID_DATA_LEN:
      gST->ConOut->OutputString(gST->ConOut, L"INVALID_DATA_LEN \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_INVALID_PARAMETER:
      gST->ConOut->OutputString(gST->ConOut, L"INVALID_PARAMETER \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_INVALID_BUS_CLOCK:
      gST->ConOut->OutputString(gST->ConOut, L"INVALID_BUS_CLOCK \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_INVALID_BYTE_TRANSFER_TIME_USEC:
      gST->ConOut->OutputString(gST->ConOut, L"INVALID_BYTE_TRANSFER_TIME_USEC \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_INVALID_INTERFACE:
      gST->ConOut->OutputString(gST->ConOut, L"INVALID_INTERFACE \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_INVALID_BUS_ID:
      gST->ConOut->OutputString(gST->ConOut, L"INVALID_BUS_ID \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_DAL_ERROR:
      gST->ConOut->OutputString(gST->ConOut, L"DAL_ERROR \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_SLAVE_NACK:
      gST->ConOut->OutputString(gST->ConOut, L"SLAVE_NACK \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_BUS_POWER_STATE_NOT_ACTIVE:
      gST->ConOut->OutputString(gST->ConOut, L"BUS_POWER_STATE_NOT_ACTIVE \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_POWER_EVENT_FAILED:
      gST->ConOut->OutputString(gST->ConOut, L"POWER_EVENT_FAILED \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_PROPERTIES_INVALID:
      gST->ConOut->OutputString(gST->ConOut, L"PROPERTIES_INVALID \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_BUS_IDLE_WAIT_TIMEOUT:
      gST->ConOut->OutputString(gST->ConOut, L"BUS_IDLE_WAIT_TIMEOUT \n\r");
      break;
    case EFI_QCOM_I2C_ERROR_LOCK_NOT_ACQUIRED:
      gST->ConOut->OutputString(gST->ConOut, L"LOCK_NOT_ACQUIRED \n\r");
      break;
    default:
      break;
  }
}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

 **/

EFI_STATUS
EFIAPI
TSI2CTestMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
    )
{
  EFI_STATUS Status = EFI_SUCCESS;
  EFI_QCOM_I2C_STATUS_CODES I2cStatus = EFI_QCOM_I2C_ERROR_OPERATION_FAILED;
  EFI_QCOM_I2C_PROTOCOL *I2cIo = NULL;
  EFI_QCOM_I2C_BUS_CONFIG I2cBusConfig;
  EFI_QCOM_I2C_SLAVE_CONFIG I2cSlaveConfig;
  EFI_QCOM_I2C_CONFIG I2cConfig;
  TS_TEST_STATUS TestSts = E_SUCCESS;
  EFI_TLMM_PROTOCOL *TLMMProtocol = NULL;
  VOID  *I2cHandle = NULL;
  UINT32 BytesRead = 0;
  UINT32 BytesWritten = 0;
  UINT32 i = 0;
  UINT32 config;
  UINT32 config1;
  UINT32 value = GPIO_HIGH_VALUE;
  UINT32 counter=0;
 
  TEST_START("TsFwI2CTest");
 

 /* Locate the TLMM protocol & then configure the GPIO 50 */
 
  if(EFI_SUCCESS != (Status = gBS->LocateProtocol( &gEfiTLMMProtocolGuid, NULL, (void**)&TLMMProtocol)))
  {
      AsciiPrint ("TsFwI2CTest: Locate TLMM Protocol Failed!\n");
  } 
  

   /*Configure MSM GPIO 50 & Enable */
  if (Status == EFI_SUCCESS)
  {
    config = EFI_GPIO_CFG( 50, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA );
    Status = TLMMProtocol->ConfigGpio((UINT32)config, TLMM_GPIO_ENABLE);
    if (Status != EFI_SUCCESS)
    {
        AsciiPrint ("TsFwI2CTest: Unable to Configure MSM GPIO 50 !!\n");
    }
  }

  /* To power on the TSC , drive the line HIGH */
  Status = TLMMProtocol->GpioOut(config, GPIO_HIGH_VALUE);
  gBS->Stall(2000); // 2 ms .   


  /* Locate the I2C Protocol */
  Status = gBS->LocateProtocol( &gQcomI2CProtocolGuid, 
                                                NULL, 
						      (VOID **) &I2cIo );

  if (NULL == I2cIo)
  {
    TestStatus("TsFwI2CTest",EFI_DEVICE_ERROR);
    TEST_STOP("TsFwI2CTest");
    return EFI_DEVICE_ERROR;
  }


  /* After power up MXT1386 takes 90ms before it is ready to start communications .
      there RESET_N (MSM 8960 GPIO 52 ) need to be set HIGH ('1') 90ms after power rails
      are stable */
  
  gBS->Stall(90000); // 90 ms .

  /* To put the device in bootloader mode , toggling the reset line 10 times  */

  config1 =    EFI_GPIO_CFG( 52, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA );

  Status = TLMMProtocol->ConfigGpio((UINT32)config1,TLMM_GPIO_ENABLE);
	
  if (Status != EFI_SUCCESS)
  {
       AsciiPrint ("TsFwI2CTest: Unable to Configure MSM GPIO 52 ENABLE !!\n");
  }
  else
  {
      for(i=0;i<10;i++)
      {
             Status = TLMMProtocol->GpioOut(config1, GPIO_LOW_VALUE);
	      gBS->Stall(5000); // 5 ms .
	      Status = TLMMProtocol->GpioOut(config1, GPIO_HIGH_VALUE);
	      gBS->Stall(60000); // 60 ms .
       }
  }
  if (EFI_SUCCESS != Status)
  {
    TestStatus("TsFwI2CTest",Status); 
    TEST_STOP("TsFwI2CTest");
    return Status;
  }

  /* Bus & Slave ( TSC ) Configuration . Device Type should be DEFAULT
      as we write directly without using the reg-address , otherwise Read/Write 
      I2C calls will not work in boot loader mode if Device Type is MEMORY */


  /* Bus Config */
  I2cBusConfig.Bus_Frequency_KHz = I2C_TS_FREQ;

  /* Slave Config */
  I2cSlaveConfig.Slave_Address = 
  PcdGet32(TSSlaveAddress);
  I2cSlaveConfig.Byte_Transfer_Wait_Time_Usec = 
  PcdGet32(TSI2CByteTransferWaitTime);

  I2cSlaveConfig.Slave_Address_Length = EFI_QCOM_I2C_7_BIT_SLAVE_ADDRESS;
  I2cSlaveConfig.Slave_Device_Type = EFI_QCOM_I2C_DEFAULT_ADDRESS_DEVICE;
  I2cSlaveConfig.Read_Options = EFI_QCOM_I2C_GEN_START_BEFORE_READ;

  I2cConfig.Bus_Config = &I2cBusConfig;
  I2cConfig.Slave_Config = &I2cSlaveConfig;

  /* Initialize the I2C Core for Touch screen controller device */
  Status = I2cIo->Init((EFI_QCOM_I2C_CORE) (PcdGet32(TSI2CCore)-1), // touch sits on GSBI 3 
                                  &I2cHandle);
  if (EFI_SUCCESS != Status)
  {
    TestStatus("TsFwI2CTest",Status);
    TEST_STOP("TsFwI2CTest");
    return Status;
  }

  /* Configure bus and slave parameters */
  I2cStatus = I2cIo->Configure(I2cHandle, &I2cConfig);
  if (EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
  {
     I2cErrorPrint(I2cStatus);
  }
  else
  {
     AsciiPrint("\n Bus & Slave configuration success");
  }

   /* Read the first byte to confirm if the device is in bootloader mode or not */
   
   I2cStatus = I2cIo->Read(I2cHandle, 0, BackBuf, 1 , &BytesRead);
   if (EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
   {
      TestSts= E_BACKUP_FAILED;
      gST->ConOut->OutputString(gST->ConOut, L"TsFwI2CTest Backup Failed \n\r");
      I2cErrorPrint(I2cStatus);
      return TestSts;
   }
   else
   {
	     AsciiPrint(" \n I2C Initial Read in boot mode :Suceess with buffer value %d:",*BackBuf);
	     AsciiPrint(" \n I2C Initial Read in boot mode :Success with buffer value %x:",*BackBuf);
   } 


   /* lets unlock the device by doing a write operation with 2 bytes as mentioned below */

   BackBuf[0] = 0xDC;
   BackBuf[1] = 0xAA;
   I2cStatus = EFI_QCOM_I2C_ERROR_OPERATION_FAILED;
   for(i=0;i<4;i++)
   {
         gBS->Stall(25000); // 25 ms .
         I2cStatus = I2cIo->Write(I2cHandle, 0, BackBuf, 2,&BytesWritten);
         AsciiPrint(" \n I2C Write  with buffer value & I2cStatus = %d:",I2cStatus);
         if(I2cStatus == EFI_QCOM_I2C_OPERATION_SUCCESSFUL)
         break; // break out of the while loop .
   }


   /* try to assert the CHG line */

   /* Configure the CHG line & enable it */
 
    config =  EFI_GPIO_CFG( 11, 0, GPIO_INPUT, GPIO_NO_PULL, GPIO_2MA );

    Status = TLMMProtocol->ConfigGpio((UINT32)config,TLMM_GPIO_ENABLE);
    if (Status != EFI_SUCCESS)
    {
        AsciiPrint ("TsFwI2CTest: Unable to Configure MSM GPIO 11 ENABLE !!\n");
    }

    /* Check whether the line is driven LOW or not , so that we can start the read operation .
        Only when it is driven LOW we are allowed to do read operation otherwise we get
        unexpected result */
     
    while(value != GPIO_LOW_VALUE)
    {
       Status = TLMMProtocol->GpioIn(config, &value);
       counter++;
       if(value== GPIO_LOW_VALUE)
       {
            AsciiPrint("\n CHG line is HIGH : counter = %d:",counter);
            break;
       }
    }



    /*  Once the CHG line is asserted , then its an indication i moved to a different
         state in bootloader mode , lets read the byte and see if bootloader is waiting
         for frame data, by reading the bit 7 */

   I2cStatus = EFI_QCOM_I2C_ERROR_OPERATION_FAILED;
   for(i=0;i<4;i++)
   {
       gBS->Stall(25000); // 25 ms .
       I2cStatus = I2cIo->Read(I2cHandle, 0, ReadBuf, 1 , &BytesRead);
       AsciiPrint(" \n I2C Read  with buffer value & I2cStatus = %d:",I2cStatus);
       if(I2cStatus == EFI_QCOM_I2C_OPERATION_SUCCESSFUL)
       break;
   }
   
   if (EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
   {
      TestSts= E_BACKUP_FAILED;
      gST->ConOut->OutputString(gST->ConOut, L"TsFwI2CTest Backup Failed \n\r");
      I2cErrorPrint(I2cStatus);
   }
   else
   {
            AsciiPrint(" \n I2C Read  Sucess with buffer value %d:",*ReadBuf);
            AsciiPrint(" \n I2C Read  Sucess with buffer value %x:",*ReadBuf);
   }


  Status = I2cIo->DeInit(I2cHandle);
  if (EFI_SUCCESS != Status)
  {
    TestStatus("TsFwI2CTest",Status);
    TEST_STOP("TsFwI2CTest");
    return Status;
  }
  TestStatus("TsFwI2CTest",EFI_SUCCESS);
  TEST_STOP("TsFwI2CTest");
  return EFI_SUCCESS;
}
