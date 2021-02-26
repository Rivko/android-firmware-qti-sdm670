/*============================================================================
  FILE:         AdcLoaderTest.c

  OVERVIEW:     Test file for ADC.

  Usage: call AdcTest() after PMIC is initialized. Infinite loops are
  inserted at the start and end of the test: change gbAdcTestResume = 1 to
  resume execution.

  DEPENDENCIES: None

                Copyright (c) 2012-2016 Qualcomm Technologies, Inc.
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
  2015-01-23  jjo  Use DDIAdc.h.
  2014-06-06  jjo  Added USB D+ and D-.
  2013-04-18  jjo  Added XO_THERM_MV.
  2012-12-03  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AdcTest.h"
#include "DDIAdc.h"
#include <Library/BaseLib.h>

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef enum
{
   ADC_ERROR_SUCCESS = 0,
   ADC_ERROR_INIT_FAILED,
   ADC_ERROR_READ_FAILED,
   ADC_ERROR_RECALIBRATE_FAILED
} AdcTestErrorType;

typedef struct
{
   const char *pszChannelName;
   AdcInputPropertiesType channel;
   AdcResultType reading;
} AdcTestReadType;

typedef struct
{
   const char *pszChannelName;
   AdcInputPropertiesType channel;
   AdcRecalibrationResultType recal;
} AdcTestRecalType;

 /*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
volatile boolean gbAdcTestResume = FALSE;

AdcTestErrorType gAdcTestError = ADC_ERROR_SUCCESS;

const char *apszReadChannels[] =
{
   ADC_INPUT_VPH_PWR,
   ADC_INPUT_PMIC_TEMP1,
   ADC_INPUT_PMIC_HARDWARE_ID,
   ADC_INPUT_PLATFORM_ID,
   ADC_INPUT_XO_THERM_MV,
   ADC_INPUT_VBATT,
   ADC_INPUT_BATT_ID,
   ADC_INPUT_BATT_ID_OHMS,
   ADC_INPUT_BATT_ID_OHMS_FRESH,
};

#ifndef ADC_LOADER_DRIVER
const char *apszRecalChannels[] =
{
   /* VADC channels */
   ADC_INPUT_VPH_PWR,
   ADC_INPUT_SYS_THERM1
};
#endif

AdcTestReadType adcTestRead[ARRAY_LENGTH(apszReadChannels)];

#ifndef ADC_LOADER_DRIVER
AdcTestRecalType adcTestRecal[ARRAY_LENGTH(apszRecalChannels)];
#endif

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static DalDeviceHandle *phAdcDev;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
void AdcTest(void)
{
#ifndef ADC_LOADER_DRIVER
   AdcTestRecalType *pAdcTestRecal;
#endif
   AdcTestReadType *pAdcTestRead;
   uint32 uInputNameSize;
   DALResult result;
   uint32 uChannel;

   gbAdcTestResume = FALSE;
   while (gbAdcTestResume == FALSE);

   result = DAL_AdcDeviceAttach(DALDEVICEID_ADC, &phAdcDev);
   if (result != DAL_SUCCESS)
   {
     gAdcTestError = ADC_ERROR_INIT_FAILED;
     goto end;
   }

   /* Read all channels */
   for (uChannel = 0; uChannel < ARRAY_LENGTH(apszReadChannels); uChannel++)
   {
      pAdcTestRead = &adcTestRead[uChannel];
      pAdcTestRead->pszChannelName = apszReadChannels[uChannel];
      uInputNameSize = AsciiStrLen(pAdcTestRead->pszChannelName) + 1;

      result = DalAdc_GetAdcInputProperties(phAdcDev, pAdcTestRead->pszChannelName, uInputNameSize, &pAdcTestRead->channel);
      if (result != DAL_SUCCESS)
      {
         pAdcTestRead->reading.eStatus = ADC_RESULT_INVALID;
         continue;
      }

      result = DalAdc_Read(phAdcDev, &pAdcTestRead->channel, &pAdcTestRead->reading);
      if (result != DAL_SUCCESS)
      {
         gAdcTestError = ADC_ERROR_READ_FAILED;
         goto end;
      }
   }

#ifndef ADC_LOADER_DRIVER
   /* Recalibrate channels */
   for (uChannel = 0; uChannel < ARRAY_LENGTH(apszRecalChannels); uChannel++)
   {
      pAdcTestRecal = &adcTestRecal[uChannel];
      pAdcTestRecal->pszChannelName = apszRecalChannels[uChannel];
      uInputNameSize = AsciiStrLen(pAdcTestRecal->pszChannelName) + 1;

      result = DalAdc_GetAdcInputProperties(phAdcDev, pAdcTestRecal->pszChannelName, uInputNameSize, &pAdcTestRecal->channel);
      if (result != DAL_SUCCESS)
      {
         pAdcTestRecal->recal.eStatus = ADC_RESULT_INVALID;
         continue;
      }

      result = DalAdc_Recalibrate(phAdcDev, &pAdcTestRecal->channel, &pAdcTestRecal->recal);
      if (result != DAL_SUCCESS)
      {
         gAdcTestError = ADC_ERROR_RECALIBRATE_FAILED;
         goto end;
      }
   }
#endif

end:
   gbAdcTestResume = FALSE;
   while (gbAdcTestResume == FALSE);

   return;
}

