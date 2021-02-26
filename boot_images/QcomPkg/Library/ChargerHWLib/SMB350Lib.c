/** @file
 *                             SMB350Lib.c
 *
 * SMB350 Charger driver is responsible for /// Write the description.
 *
 * This file is platform dependent, it supports SMB350 based on the platform.
 *
 *  &copy; Copyright 2012 - 2013 Qualcomm Technologies, Inc.  All Rights Reserved.
 *         Qualcomm Technologies Proprietary and Confidential.
 *
**/
/*=============================================================================
                              EDIT HISTORY

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


 when       who      what, where, why
 --------   ---      ----------------------------------------------------------
 04/26/13   sm       Replaced SMB350_STATUS_B_REG with SMB350_POWER_OK_STATUS_REG
 03/06/13   dy       Added APIs to get Battery Presence and Charger Connected
 11/06/12   dy       Created File
=============================================================================*/

#include "Uefi.h"
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

#include "SMB350Lib.h"

#include <Protocol/EFII2C.h>

#include <Guid/EventGroup.h>

/*===========================================================================*/
/*                  LOCAL DEFINITIONS                                        */
/*===========================================================================*/

#define SMB350_TERMINATION_CURRENT_500mA               0x0C
#define SMB350_TERMINATION_CURRENT_REG                 0x01
#define SMB350_TERMINATION_CURRENT_MASK                0x1C

#define SMB350_CHARGE_CONTROL_REG                      0x04
#define SMB350_AUTOMATIC_RECHARGE_MASK                 0x80

#define SMB350_INTERRUPT_STATUS_B_REG                  0x36
#define SMB350_MISSING_BATT_MASK                       0x10

#define SMB350_POWER_OK_STATUS_REG                     0x3A
#define SMB350_POWER_OK_STATUS_MASK                    0x07

#define SMB350_SLAVE_ADDR           0x2B
#define SMB350_I2C_FREQUENCY         400

#define DATA_SIZE 1

static void                  *pI2cHandle      = NULL;
static EFI_QCOM_I2C_PROTOCOL *pI2CProtocol    = NULL;

/*=========================================================================
                          LOCAL FUNCTION PROTOTYPES
===========================================================================*/
static EFI_STATUS I2CInit( UINT32 SlaveAddr, UINT32 I2cFreq);

static EFI_STATUS Smb350DisableChgWorkaround(void);

/*===========================================================================
                          EXTERNAL FUNCTIONS DEFINITION
===========================================================================*/
/**
 * @brief  Initialise the SMB Charger.
 *
 * @param  I2cPortAddr       I2C port address
 * @param  I2cFreq           I2C frequency
 *
 * @retval EFI_SUCCESS       Initialized successfully
 * @retval non EFI_SUCCESS   Initialization failed
**/
EFI_STATUS EFI_SMB350Init(
  void
)
{
    EFI_STATUS  Status = EFI_INVALID_PARAMETER;

    /* Initialize I2C */
    if(EFI_SUCCESS != I2CInit(SMB350_SLAVE_ADDR, SMB350_I2C_FREQUENCY))
    {
        return Status;
    }

    Smb350DisableChgWorkaround();
    
    return (Status); 
}


/*=========================================================================
                          LOCAL FUNCTION DEFINITIONS
===========================================================================*/ 


EFI_STATUS Smb350DisableChgWorkaround(void)
{
  EFI_QCOM_I2C_STATUS_CODES I2cStatus = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;
  EFI_STATUS                Status     = EFI_SUCCESS;
  UINT8  Data[DATA_SIZE];
  UINT32 bRead = 0;
  UINT32 bWrote;
  
  if (pI2CProtocol == NULL)  
    return EFI_DEVICE_ERROR;
      
  I2cStatus = pI2CProtocol->Read(pI2cHandle, 0x30, Data, DATA_SIZE, &bRead);
  Data[0] = Data[0] | 0x80;
  I2cStatus = pI2CProtocol->Write(pI2cHandle, 0x30, Data, DATA_SIZE, &bWrote);
  
  gBS->Stall(100);
  
  /* Set Termination Current to 500 mA */
  /* Set Reg 01 [4:2] = 0b011 */
  I2cStatus = pI2CProtocol->Read(pI2cHandle, SMB350_TERMINATION_CURRENT_REG, Data, DATA_SIZE, &bRead);
  //DEBUG((EFI_D_ERROR, " 1. Read Register 01 = 0x%x\r\n", Data[0]));
  Data[0] = (Data[0] & ~SMB350_TERMINATION_CURRENT_MASK) | SMB350_TERMINATION_CURRENT_500mA;
  //DEBUG((EFI_D_ERROR, " 2. Modify data in register 01 = 0x%x\r\n", Data[0]));
  I2cStatus = pI2CProtocol->Write(pI2cHandle, SMB350_TERMINATION_CURRENT_REG, Data, DATA_SIZE, &bWrote);
  gBS->Stall(100);
  
  /* Verify */
  Data[0] = 0;
  I2cStatus = pI2CProtocol->Read(pI2cHandle, SMB350_TERMINATION_CURRENT_REG, Data, DATA_SIZE, &bRead);
  gBS->Stall(100);
  //DEBUG((EFI_D_ERROR, " 3. Verify data in register 01 = 0x%x\r\n", Data[0]));
  
  /* Disable Automatic Recharge */
  /* Set Reg 04 [7] = 0b1 */
  I2cStatus = pI2CProtocol->Read(pI2cHandle, SMB350_CHARGE_CONTROL_REG, Data, DATA_SIZE, &bRead);
  //DEBUG((EFI_D_ERROR, " 1. Read Register 04 = 0x%x\r\n", Data[0]));
  Data[0] = Data[0] | SMB350_AUTOMATIC_RECHARGE_MASK;
  //DEBUG((EFI_D_ERROR, " 2. Modify data in register 04 = 0x%x\r\n", Data[0]));
  I2cStatus = pI2CProtocol->Write(pI2cHandle, SMB350_CHARGE_CONTROL_REG, Data, DATA_SIZE, &bWrote);
  gBS->Stall(100);
  
  /* Verify */
  Data[0] = 0;
  I2cStatus = pI2CProtocol->Read(pI2cHandle, SMB350_CHARGE_CONTROL_REG, Data, DATA_SIZE, &bRead);
  //DEBUG((EFI_D_ERROR, " 3. Verify data in register 04 = 0x%x\r\n", Data[0]));
  gBS->Stall(100);
  
  if(EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
  {
    Status = EFI_INVALID_PARAMETER;
  }

  return Status;
}

EFI_STATUS Smb350IsChargerConnected(BOOLEAN *ChgConnected)
{
  EFI_QCOM_I2C_STATUS_CODES I2cStatus = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;
  UINT8  Data[DATA_SIZE];
  UINT32 bRead = 0;
  
  if (pI2CProtocol == NULL)  
      return EFI_DEVICE_ERROR;
      
  I2cStatus = pI2CProtocol->Read(pI2cHandle, SMB350_POWER_OK_STATUS_REG, Data, DATA_SIZE, &bRead);
  
  if(EFI_QCOM_I2C_OPERATION_SUCCESSFUL == I2cStatus)
  {
    *ChgConnected = (BOOLEAN) (Data[0] & SMB350_POWER_OK_STATUS_MASK);
    return EFI_SUCCESS;
  }
  else
  {
    return EFI_DEVICE_ERROR;
  }
}

EFI_STATUS Smb350IsBatteryPresent(BOOLEAN *BattPresent)
{
  EFI_QCOM_I2C_STATUS_CODES I2cStatus = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;
  UINT8  Data[DATA_SIZE];
  UINT32 bRead = 0;
  
  if (pI2CProtocol == NULL)  
      return EFI_DEVICE_ERROR;
      
  I2cStatus = pI2CProtocol->Read(pI2cHandle, SMB350_INTERRUPT_STATUS_B_REG, Data, DATA_SIZE, &bRead);
  
  if(EFI_QCOM_I2C_OPERATION_SUCCESSFUL == I2cStatus)
  {
    *BattPresent = ((Data[0] & SMB350_MISSING_BATT_MASK) == 0);
    return EFI_SUCCESS;
  }
  else
  {
    return EFI_DEVICE_ERROR;
  }
}


/**
 * @brief Initialize the I2C Drivers for communication
 *
 * @param  SlaveAddr       Address of the I2C Slave
 *         I2cFreq         Frequency of I2C
 *
 * @retval EFI_SUCCESS            Initialized successfully.
 * @retval EFI_INVALID_PARAMETER  Initialization failed.
**/
EFI_STATUS I2CInit(
  UINT32 SlaveAddr,
  UINT32 I2cFreq
)
{
    EFI_QCOM_I2C_STATUS_CODES I2cStatus = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;
    EFI_STATUS Status    = EFI_INVALID_PARAMETER;
    EFI_QCOM_I2C_BUS_CONFIG I2cBusConfig;
    EFI_QCOM_I2C_SLAVE_CONFIG I2cSlaveConfig;
    EFI_QCOM_I2C_CONFIG I2cConfig;

    // Locate the QCOM I2C Protocol
    Status = gBS->LocateProtocol(&gQcomI2CProtocolGuid, NULL, (VOID **) &pI2CProtocol);

    if (pI2CProtocol == NULL)  {  return EFI_DEVICE_ERROR;  }

    if (EFI_SUCCESS != Status)  {  return Status;  }

    /* Bus Config */
    I2cBusConfig.Bus_Frequency_KHz = I2cFreq;

    /* Slave Config */
    I2cSlaveConfig.Slave_Address = SlaveAddr;
    I2cSlaveConfig.Slave_Address_Length = EFI_QCOM_I2C_7_BIT_SLAVE_ADDRESS;
    I2cSlaveConfig.Slave_Device_Type = EFI_QCOM_I2C_REGISTER_ADDRESS_DEVICE;
    I2cSlaveConfig.Byte_Transfer_Wait_Time_Usec = -1;
    I2cSlaveConfig.Read_Options = EFI_QCOM_I2C_GEN_STOP_START_BEFORE_READ;

    I2cConfig.Bus_Config = &I2cBusConfig;
    I2cConfig.Slave_Config = &I2cSlaveConfig;

    /* Initialize the I2C Core for PMIC */
    Status = pI2CProtocol->Init((EFI_QCOM_I2C_CORE) EfiQcomI2CCore011, &pI2cHandle);

    if (EFI_SUCCESS != Status)
    {
        DEBUG((EFI_D_ERROR, " I2CInit() Failed!! to Configure I2C. Status =%d\r\n",I2cStatus));
    }

    /* Configure bus and slave parameters */
    I2cStatus = pI2CProtocol->Configure(pI2cHandle, &I2cConfig);

    if (EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
    {
        DEBUG((EFI_D_ERROR, " I2CInit() Failed!! to Configure I2C. Status =%d\r\n",I2cStatus));
        Status = EFI_INVALID_PARAMETER;
    }

    return Status;
}
