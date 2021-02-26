/*===========================================================================
                           PMIC-BQ30Z55 DRIVER
   DESCRIPTION
   This file contains functions prototypes and variable/type/constant
   declarations for supporting BATTERY Gauge BQ30Z55 module for the Qualcomm 
   PMIC chip set.


  Copyright (c) 2013              , 2018 by Qualcomm Technologies, Inc. All Rights Reserved

===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


 when         who     what, where, why
 ----------   ---     ----------------------------------------------------------
 03/05/13     sm      Created file
===========================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BQ30Z55Lib.h>
#include <Library/pm_uefi_lib_err.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Guid/EventGroup.h>

#include <Protocol/EFII2C.h>
#include <Protocol/EFIPmicBms.h>
/*===========================================================================*/
/*                  LOCAL DEFINITIONS                                        */
/*===========================================================================*/
#define BQ30Z55_TEMPERATURE          0x08
#define BQ30Z55_CHARGING_CURRENT     0x0A
#define BQ30Z55_BATTERY_VOLTAGE      0x09
#define BQ30Z55_RELATIVE_SOC_ADDR    0x0D
#define BQ30Z55_RATED_CAPACITY       0x10
#define BQ30Z55_SLAVE_ADDR           0x0B
#define BQ30Z55_I2C_FREQUENCY         100
#define LENGTH1                         1
#define LENGTH2                         2

static EFI_QCOM_I2C_PROTOCOL  *pI2CProtocol = NULL;
static void                   *pI2cHandle   = NULL;
static EFI_PM_BMS_CFGDATA_TYPE BatteryDefaults;

/*==========================================================================
                          LOCAL FUNCTION PROTOTYPES
===========================================================================*/
static EFI_STATUS I2CInit( UINT32 SlaveAddr, UINT32 BQ27541I2cFreq );

/*==========================================================================
                          EXTERNAL FUNCTIONS DEFINITION
===========================================================================*/
/**
 * Initializes the BQ30Z55.
 *
 * @param  void
 *
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
**/
EFI_STATUS BQ30Z55Init(
  void
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    EFI_QCOM_PMIC_BMS_PROTOCOL           *PmicBmsProtocol = NULL;

    I2CInit(BQ30Z55_SLAVE_ADDR, BQ30Z55_I2C_FREQUENCY);
    
    Status = gBS->LocateProtocol( &gQcomPmicBmsProtocolGuid,
                                  NULL,
                                  (VOID**) &PmicBmsProtocol );

    PmicBmsProtocol->GetBatteryConfigData(&BatteryDefaults);

	return Status;
}

/**
 * Commands the BQ30Z55 Hardware to get the battery percentage.
 *
 * @param[in] None
 * @retval    UINT8 battery percentage 0-100%
**/
EFI_STATUS
BQ30Z55GetSoC
(
  OUT UINT32 *pBatteryCapacity
)
{
    EFI_STATUS                Status    = EFI_SUCCESS;
	EFI_QCOM_I2C_STATUS_CODES I2cStatus = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;

	UINT8  BatteryCapacity = 0;
    UINT32 bRead = 0;

    /* Read from the hardware */
    I2cStatus = pI2CProtocol->Read(pI2cHandle, BQ30Z55_RELATIVE_SOC_ADDR, &BatteryCapacity, LENGTH1, &bRead);
    if(EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
    {
        DEBUG((EFI_D_ERROR, "ERROR: BQ30Z55GetSoC - I2cRead Status: 0x%08X\r\n", I2cStatus));
    }

	*pBatteryCapacity = BatteryCapacity;
    if(BatteryDefaults.PrintDebugMsg)
    {
      DEBUG((EFI_D_ERROR, "BQ30Z55GetSoC SOC: %d \n\r", *pBatteryCapacity));
    }

    return Status;
} /* BQ30Z55GetSoC */

EFI_STATUS
BQ30Z55GetRatedCapacity
(
  OUT UINT32 *pRatedCapacity
)
{
  EFI_STATUS                Status          = EFI_SUCCESS;
  EFI_QCOM_I2C_STATUS_CODES I2cStatus = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;

  UINT8  FullChgCapacity[2] = {0, 0};
  UINT16 RatedCapacity = 0;
  UINT32 bRead = 0;

  /* Read from the hardware */
  I2cStatus = pI2CProtocol->Read(pI2cHandle, BQ30Z55_RATED_CAPACITY, FullChgCapacity, LENGTH2, &bRead);
  if(EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
  {
    DEBUG((EFI_D_ERROR, "ERROR: BQ30Z55GetRatedCapacity - I2cRead Status: 0x%08X\r\n", I2cStatus));
  }
  
  RatedCapacity = (FullChgCapacity[1] << 8) + FullChgCapacity[0];

  *pRatedCapacity = RatedCapacity;
  if(BatteryDefaults.PrintDebugMsg)
  {
    DEBUG((EFI_D_ERROR, "BQ30Z55GetRatedCapacity RatedCapacity: %d \n\r", *pRatedCapacity));
  }

  return Status;
} /* BQ30Z55GetRatedCapacity */

EFI_STATUS
BQ30Z55GetChargeCurrent
(
  OUT INT32  *pChargeCurrent
)
{
  EFI_STATUS                Status        = EFI_SUCCESS;
  EFI_QCOM_I2C_STATUS_CODES I2cStatus     = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;

  UINT8   Current[2]    = {0, 0};
  INT16   ChargeCurrent = 0;
  UINT32  bRead         = 0;

  /* Read from the hardware */
  I2cStatus = pI2CProtocol->Read(pI2cHandle, BQ30Z55_CHARGING_CURRENT, Current, LENGTH2, &bRead);
  if(EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
  {
      DEBUG((EFI_D_ERROR, "ERROR: BQ30Z55GetChargeCurrent - I2cRead Status: 0x%08X\r\n", I2cStatus));
  }
  
  ChargeCurrent = (( Current[1] << 8 ) + Current[0] );

  *pChargeCurrent = ChargeCurrent;

  if(BatteryDefaults.PrintDebugMsg)
  {
    DEBUG((EFI_D_ERROR, "BQ30Z55GetChargeCurrent ChargeCurrent: %d \n\r", *pChargeCurrent));
  }

  return Status;
}/* BQ30Z55GetChargeCurrent */

EFI_STATUS
BQ30Z55GetBatteryVoltage
(
OUT INT32  *pBattVoltage
)
{
  EFI_STATUS                Status        = EFI_SUCCESS;
  EFI_QCOM_I2C_STATUS_CODES I2cStatus     = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;

  UINT8   Voltage[2]  = {0, 0};
  INT16   BattVoltage = 0;
  UINT32  bRead       = 0;

  /* Read from the hardware */
  I2cStatus = pI2CProtocol->Read(pI2cHandle, BQ30Z55_BATTERY_VOLTAGE, Voltage, LENGTH2, &bRead);
  if(EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
  {
      DEBUG((EFI_D_ERROR, "ERROR: BQ30Z55GetChargeCurrent - I2cRead Status: 0x%08X\r\n", I2cStatus));
  }

  BattVoltage = (( Voltage[1] << 8 ) + Voltage[0] );

  *pBattVoltage = BattVoltage;

  if(BatteryDefaults.PrintDebugMsg)
  {
    DEBUG((EFI_D_ERROR, "BQ30Z55GetBatteryVoltage BattVoltage: %d \n\r", *pBattVoltage));
  }
  
  return Status;
}/* BQ30Z55GetBatteryVoltage */


/**
 * @brief  Commands the BQ30Z55 Hardware to get the battery temperature.
 *
 * @param  None
 * @retval UINT16 - Battery temperature in Kelvin
**/
EFI_STATUS
BQ30Z55GetTemp
(
  INT32 *pBattTemp
)
{
    EFI_STATUS                Status      = EFI_SUCCESS;
    EFI_QCOM_I2C_STATUS_CODES I2cStatus   = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;

    UINT8   Temp[2]     = {0, 0};
    INT16   BatteryTemp = 0;
    UINT32  bRead       = 0;

    /* Read from the hardware */
    I2cStatus = pI2CProtocol->Read(pI2cHandle, BQ30Z55_TEMPERATURE, Temp, LENGTH2, &bRead);
    if(EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
    {
        DEBUG((EFI_D_ERROR, "BQ30Z55GetTemp - I2cRead Status: %d\r\n", I2cStatus));
        return EFI_INVALID_PARAMETER;
    }

    //Returns the temperature in Celsius
	BatteryTemp = ((( Temp[1] << 8 ) + Temp[0]) / 10) - 273;  //Unit of temperature is 0.1Kelvin

    *pBattTemp = BatteryTemp;

    if(BatteryDefaults.PrintDebugMsg)
    {
      DEBUG((EFI_D_ERROR, "BQ30Z55GetTemp BattTemp: %d \n\r", *pBattTemp));
    }

    return Status;
} /* BQ30Z55GetTemp */

/**
 * Initializes the I2C to be used for BQ30Z55
 *
 * @param  SlaveAddr - Port Address for BQ30Z55
 * @param  BQ30Z55I2cFreq - operating frequency for BQ30Z55
 *
 * @retval EFI_SUCCESS       Initialized successfully
 *         non EFI_SUCCESS   Initialization failed
**/
EFI_STATUS I2CInit(
  UINT32 SlaveAddr,
  UINT32 BQ30Z55I2cFreq
)
{
    EFI_QCOM_I2C_STATUS_CODES I2cStatus = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;
    EFI_STATUS                Status    = EFI_INVALID_PARAMETER;
    EFI_QCOM_I2C_BUS_CONFIG   I2cBusConfig;
    EFI_QCOM_I2C_SLAVE_CONFIG I2cSlaveConfig;
    EFI_QCOM_I2C_CONFIG       I2cConfig;

    // Locate the QCOM I2C Protocol
    Status = gBS->LocateProtocol(&gQcomI2CProtocolGuid, NULL, (VOID **) &pI2CProtocol);

    if(pI2CProtocol == NULL)  
	{ 
      DEBUG((EFI_D_ERROR, "I2CInit - pI2CProtocol is NULL\r\n"));
	  return EFI_DEVICE_ERROR;  
	}

    if(EFI_SUCCESS != Status)  
	{
      DEBUG((EFI_D_ERROR, "I2CInit - could not locate pI2CProtocol Protocol\r\n"));
	  return Status;  
	}

    /* Bus Config */
    I2cBusConfig.Bus_Frequency_KHz = BQ30Z55I2cFreq;

    /* Slave Config */
    I2cSlaveConfig.Slave_Address = SlaveAddr;
    I2cSlaveConfig.Slave_Address_Length = EFI_QCOM_I2C_7_BIT_SLAVE_ADDRESS;
    I2cSlaveConfig.Slave_Device_Type = EFI_QCOM_I2C_REGISTER_ADDRESS_DEVICE;
    I2cSlaveConfig.Byte_Transfer_Wait_Time_Usec = -1;
    I2cSlaveConfig.Read_Options = EFI_QCOM_I2C_GEN_START_BEFORE_READ;

    I2cConfig.Bus_Config = &I2cBusConfig;
    I2cConfig.Slave_Config = &I2cSlaveConfig;

    /* Initialize the I2C Core for PMIC */
    Status = pI2CProtocol->Init((EFI_QCOM_I2C_CORE) EfiQcomI2CCore011, &pI2cHandle);

    if (EFI_SUCCESS != Status)
    {
        return Status;
    }

    /* Configure bus and slave parameters */
    I2cStatus = pI2CProtocol->Configure(pI2cHandle, &I2cConfig);

    if (EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
    {
        DEBUG((EFI_D_ERROR, "I2C Configure - I2cStatus: 0x%08X\r\n", I2cStatus));
        Status = EFI_INVALID_PARAMETER;
    }

    return Status;
} /* I2CInit */
