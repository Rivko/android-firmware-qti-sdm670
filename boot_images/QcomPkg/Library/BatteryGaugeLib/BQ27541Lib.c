/*===========================================================================
                           PMIC-BQ27541 DRIVER
   DESCRIPTION
   This file contains functions prototypes and variable/type/constant
   declarations for supporting BATTERY Gauge BQ27541 module for the Qualcomm 
   PMIC chip set.


  Copyright (c) 2011              by Qualcomm Technologies, Inc.  All Rights Reserved.

===========================================================================*/
/*===========================================================================

                        EDIT HISTORY FOR MODULE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.


 when         who     what, where, why
 ----------   ---     ----------------------------------------------------------
 08/23/12      al      Removing Charger and ChargerTask related
 01/31/2012    sm      Added API to determine battery voltage
 08/22/2011    sm      Created file
===========================================================================*/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BQ27541Lib.h>
#include <Library/pm_uefi_lib_err.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Guid/EventGroup.h>

#include <Protocol/EFII2C.h>

/*===========================================================================*/
/*                  LOCAL DEFINITIONS                                        */
/*===========================================================================*/
#define BQ27541_TEMPERATURE_LOWER    0x06
#define BQ27541_TEMPERATURE_UPPER    0x07
#define BQ27541_VOLTAGE_LOWER        0x08
#define BQ27541_VOLTAGE_UPPER        0x09
#define BQ27541_STATE_OF_CHARGE      0x2C
#define BQ27541_SLAVE_ADDR           0x55
#define BQ27541_I2C_FREQUENCY         400
#define LENGTH1                         1

static EFI_QCOM_I2C_PROTOCOL  *pI2CProtocol = NULL;
static void                   *pI2cHandle   = NULL;

/*==========================================================================
                          LOCAL FUNCTION PROTOTYPES
===========================================================================*/
static EFI_STATUS I2CInit(UINT32 SlaveAddr, UINT32 BQ27541I2cFreq);
static UINT8   BQ27541GetSoC(void);
static UINT32  BQ27541GetChargerErrors(void);

/*==========================================================================
                          EXTERNAL FUNCTIONS DEFINITION
===========================================================================*/
/**
 * Initializes the BQ27541.
 *
 * @param  void
 *
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
**/
EFI_STATUS BQ27541Init(
  void
)
{
    EFI_STATUS Status = EFI_SUCCESS;

    I2CInit(BQ27541_SLAVE_ADDR, BQ27541_I2C_FREQUENCY);

    return Status;
}

/**
 * @brief  This function calculates and returns the battery SoC, Rated capacity
 *         and ChargeCurrent
 *
 * @param  I2C port address
 *
 * @retval EFI_STATUS EFI_INVALID_PARAMETER
 *                    EFI_SUCCESS
 *
 */
EFI_STATUS BQ27541GetBatteryStatus(
  OUT UINT32 *StateOfCharge,
  OUT UINT32 *RatedCapacity,
  OUT INT32 *ChargeCurrent
)
{
    EFI_STATUS Status          = EFI_SUCCESS;
    UINT32     LocalErrorState = 0;

    LocalErrorState = BQ27541GetChargerErrors();
    /* bits 0 to 15 (battery charge %), bits 16 to 32 (errors) */
    *StateOfCharge = BQ27541GetSoC() | LocalErrorState;
    *RatedCapacity  = 950;
    *ChargeCurrent  = 0xffffffff;

    LocalErrorState = (LocalErrorState & 0xffff0000) >> 16;

    /* Did we detect an error? */
//    if ((LocalErrorState != Pmic_BatteryChargingStatusSuccess) && 
//        (LocalErrorState != Pmic_BatteryChargingStatusNone))
//    {
//      Status = EFI_DEVICE_ERROR;
//    }

    return Status;
}/* BQ27541GetBatteryStatus */

UINT32 BQ27541GetChargerErrors(void)
{
    UINT32 rtnVal      = 0;
    UINT16 battTemp    = 0;

    /* Get battery temp in "C". */
    battTemp = BQ27541GetTemp();

    /* Check that the battery temperature is within valid charge range. */
    if (battTemp > 45)
    {
        DEBUG((EFI_D_ERROR, "-PMICHW(Chg) - Invalid Temp (%dC) \r\n", battTemp));

        /* Battery too hot to charge. */
        //rtnVal = Pmic_BatteryChargingStatusOverheat << 16;
    }
    else if (battTemp < 1)
    {
        /* Battery too cold to charge. */
        //rtnVal = Pmic_BatteryChargingStatusTooCold << 16;
    }
    else
    {
        //rtnVal = Pmic_BatteryChargingStatusNone << 16; 
    }

    return rtnVal;
}/* BQ27541GetChargerErrors */

/**
 * Commands the BQ27541 Hardware to get the battery percentage.
 *
 * @param[in] None
 * @retval    UINT8 battery percentage 0-100%
**/
UINT8 BQ27541GetSoC(
  void
)
{
    EFI_QCOM_I2C_STATUS_CODES I2cStatus = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;
    UINT8  BatteryCapacity = 0;
    UINT32 bRead = 0;

    /* Read from the hardware */
    I2cStatus = pI2CProtocol->Read(pI2cHandle, BQ27541_STATE_OF_CHARGE, &BatteryCapacity, LENGTH1, &bRead);
    if(EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
    {
        DEBUG((EFI_D_ERROR, "ERROR: BQ27541GetSoc - I2cRead Status: 0x%08X\r\n", I2cStatus));
    }

    return BatteryCapacity;
} /* BQ27541GetSoc */

/**
 * @brief  Commands the BQ27541 Hardware to get the battery temperature.
 *
 * @param  None
 * @retval UINT16 - Battery temperature in Kelvin
**/
UINT16 BQ27541GetTemp(
  void
)
{
    EFI_QCOM_I2C_STATUS_CODES I2cStatus   = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;
    UINT8                     TempL       = 0;
    UINT8                     TempU       = 0;
    UINT16                    BatteryTemp = 0;
    UINT32                    bRead       = 0;

    /* Read from the hardware */
    I2cStatus = pI2CProtocol->Read(pI2cHandle, BQ27541_TEMPERATURE_UPPER, &TempU, LENGTH1, &bRead);
    I2cStatus = pI2CProtocol->Read(pI2cHandle, BQ27541_TEMPERATURE_LOWER, &TempL, LENGTH1, &bRead);
    if(EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
    {
        DEBUG((EFI_D_ERROR, "BQ27541GetTemp - I2cRead Status: 0x%08X\r\n", I2cStatus));
    }

    BatteryTemp = (((TempU<<8)+TempL)*0.1) - 273;  //Unit of temperature is 0.1Kelvin
    //Returns the temperature in Celsius

    return BatteryTemp;
} /* BQ27541GetTemp */

UINT16 BQ27541GetVBatt(
  void
)
{
    EFI_QCOM_I2C_STATUS_CODES I2cStatus   = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;
    UINT8                     VoltL       = 0;
    UINT8                     VoltU       = 0;
    UINT16                    BattVolt    = 0;
    UINT32                    bRead       = 0;

    /* Read from the hardware */
    I2cStatus = pI2CProtocol->Read(pI2cHandle, BQ27541_VOLTAGE_UPPER, &VoltU, LENGTH1, &bRead);
    I2cStatus = pI2CProtocol->Read(pI2cHandle, BQ27541_VOLTAGE_LOWER, &VoltL, LENGTH1, &bRead);
    if(EFI_QCOM_I2C_OPERATION_SUCCESSFUL != I2cStatus)
    {
        DEBUG((EFI_D_ERROR, "BQ27541GetVBatt - I2cRead Status: 0x%08X\r\n", I2cStatus));
    }

    BattVolt = (UINT16)((VoltU << 8) + VoltL);  //Unit of temperature is 0.1Kelvin

    return BattVolt;
} /* BQ27541GetVBatt */


/**
 * Initializes the I2C to be used for BQ27541
 *
 * @param  SlaveAddr - Port Address for BQ27541
 * @param  BQ27541I2cFreq - operating frequency for BQ27541
 *
 * @retval EFI_SUCCESS       Initialized successfully
 *         non EFI_SUCCESS   Initialization failed
**/
EFI_STATUS I2CInit(
  UINT32 SlaveAddr,
  UINT32 BQ27541I2cFreq
)
{
    EFI_QCOM_I2C_STATUS_CODES I2cStatus = EFI_QCOM_I2C_ERROR_INVALID_PARAMETER;
    EFI_STATUS Status    = EFI_INVALID_PARAMETER;
    EFI_QCOM_I2C_BUS_CONFIG I2cBusConfig;
    EFI_QCOM_I2C_SLAVE_CONFIG I2cSlaveConfig;
    EFI_QCOM_I2C_CONFIG I2cConfig;

    // Locate the QCOM I2C Protocol
    Status = gBS->LocateProtocol(&gQcomI2CProtocolGuid, NULL, (VOID **) &pI2CProtocol);

    if(pI2CProtocol == NULL)  {  return EFI_DEVICE_ERROR;  }

    if(EFI_SUCCESS != Status)  {  return Status;  }

    /* Bus Config */
    I2cBusConfig.Bus_Frequency_KHz = BQ27541I2cFreq;

    /* Slave Config */
    I2cSlaveConfig.Slave_Address = SlaveAddr;
    I2cSlaveConfig.Slave_Address_Length = EFI_QCOM_I2C_7_BIT_SLAVE_ADDRESS;
    I2cSlaveConfig.Slave_Device_Type = EFI_QCOM_I2C_REGISTER_ADDRESS_DEVICE;
    I2cSlaveConfig.Byte_Transfer_Wait_Time_Usec = -1;
    I2cSlaveConfig.Read_Options = EFI_QCOM_I2C_GEN_START_BEFORE_READ;

    I2cConfig.Bus_Config = &I2cBusConfig;
    I2cConfig.Slave_Config = &I2cSlaveConfig;

    /* Initialize the I2C Core for PMIC */
    Status = pI2CProtocol->Init((EFI_QCOM_I2C_CORE) EfiQcomI2CCore010, &pI2cHandle);

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

