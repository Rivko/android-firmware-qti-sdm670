/*============================================================================
  FILE:         TsensDxe.c

  OVERVIEW:     Implementation of the TSENS EFI protocol interface

  DEPENDENCIES: None

                Copyright (c) 2012, 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2014-07-21  jjo  Updated for XBL.
  2012-10-08  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/EFITsens.h>
#include <Library/TsensLib.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/
static EFI_STATUS
EFI_Tsens_Get_Num_Sensors(OUT UINT32 *pnNumSensors);

static EFI_STATUS
EFI_Tsens_Get_Temp(IN UINT32 uSensor, OUT INT32 *pnTempDeciDegC);

static EFI_STATUS
EFI_Tsens_Get_Max_Temp(OUT INT32 *pnMaxTempDeciDegC);

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static EFI_TSENS_PROTOCOL TsensProtocol =
{
   EFI_TSENS_PROTOCOL_REVISION,
   EFI_Tsens_Get_Num_Sensors,
   EFI_Tsens_Get_Temp,
   EFI_Tsens_Get_Max_Temp
};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/**
  Gets the number of TSENS sensors.

  @param[out]  pnNumSensors  The number of temperature sensors supported.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context;
                           e.g. the device is not calibrated.
**/
static EFI_STATUS
EFI_Tsens_Get_Num_Sensors(OUT UINT32 *pnNumSensors)
{
   TsensResultType status;

   if (pnNumSensors == NULL)
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Get the number of sensors from TSENS lib */
   status = Tsens_GetNumSensors(pnNumSensors);
   if (status != TSENS_SUCCESS)
   {
      return EFI_UNSUPPORTED;
   }

   return EFI_SUCCESS;
}

/**
  Gets the TSENS temp for the requested sensor in deci degrees Celcius.

  @param[in]  uSensor  Temperature sensor index.
  @param[out]  pnTempDeciDegC  Temperature of the sensor in deci degrees Celsius.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_TIMEOUT           -- A timeout occured when reading the sensor. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context;
                           e.g. the sensor is dead.
**/
static EFI_STATUS
EFI_Tsens_Get_Temp(IN UINT32 uSensor, OUT INT32 *pnTempDeciDegC)
{
   TsensResultType status;

   if (pnTempDeciDegC == NULL)
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Get the temperature from TSENS lib */
   status = Tsens_GetTemp(uSensor, pnTempDeciDegC);
   if (status != TSENS_SUCCESS)
   {
      return EFI_UNSUPPORTED;
   }

   return EFI_SUCCESS;
}

/**
  Gets the maximum TSENS temperature of all the sensors during the last measurement
  period in deci degrees Celcius.

  @param[out]  pnMaxTempDeciDegC  Maximum temperature in deci degrees Celsius.

  @return
  EFI_SUCCESS           -- Function completed successfully. \n
  EFI_INVALID_PARAMETER -- Parameter is invalid. \n
  EFI_TIMEOUT           -- A timeout occured when reading the sensor. \n
  EFI_UNSUPPORTED       -- Function is not supported in this context;
                           e.g. the sensor is dead.
**/
static EFI_STATUS
EFI_Tsens_Get_Max_Temp(OUT INT32 *pnMaxTempDeciDegC)
{
   TsensResultType status;

   if (pnMaxTempDeciDegC == NULL)
   {
      return EFI_INVALID_PARAMETER;
   }

   /* Get the temperature from TSENS lib */
   status = Tsens_GetMaxTemp(pnMaxTempDeciDegC);
   if (status != TSENS_SUCCESS)
   {
      return EFI_UNSUPPORTED;
   }

   return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
TsensEntryPoint(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
   EFI_HANDLE handle = NULL;
   TsensResultType status;
   EFI_STATUS efiStatus;

   status = Tsens_Init();
   if (status != TSENS_SUCCESS)
   {
      return EFI_UNSUPPORTED;
   }

   efiStatus = gBS->InstallMultipleProtocolInterfaces(&handle,
                                                      &gEfiTsensProtocolGuid,
                                                      (void **)&TsensProtocol,
                                                      NULL,
                                                      NULL,
                                                      NULL);

   return efiStatus;
}

