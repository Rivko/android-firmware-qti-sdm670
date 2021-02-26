/*============================================================================
  FILE:         adc.c

  OVERVIEW:     Wrapper for DAL ADC.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/dalboot/src/adc.c#1 $$DateTime: 2017/09/18 09:26:13 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2017-02-08  jjo  Ported to boot.
  2016-04-27  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "adc.h"
#include "DDIAdc.h"
#include "string.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/
static DalDeviceHandle *gphAdcDev = NULL;

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
adc_status_type adc_read(const char *input, adc_result_type *adc_result)
{
   AdcInputPropertiesType adcInputProps;
   AdcResultType adcResult;
   DALResult result;
   adc_status_type status;

   if (input == NULL || adc_result == NULL)
   {
      return ADC_STATUS_INVALID_PARAM;
   }

   if (gphAdcDev == NULL)
   {
      result = DAL_AdcDeviceAttach(DALDEVICEID_ADC, &gphAdcDev);
      if (result != DAL_SUCCESS)
      {
         return ADC_STATUS_ERROR;
      }
   }

   result = DalAdc_GetAdcInputProperties(gphAdcDev,
                                         input,
                                         (uint32)(strlen(input) + 1),
                                         &adcInputProps);
   if (result != DAL_SUCCESS)
   {
      return ADC_STATUS_CHANNEL_NOT_SUPPORTED;
   }

   result = DalAdc_Read(gphAdcDev,
                        &adcInputProps,
                        &adcResult);
   if (result != DAL_SUCCESS)
   {
      return ADC_STATUS_ERROR;
   }

   if (adcResult.eStatus == ADC_RESULT_VALID)
   {
      status = ADC_STATUS_SUCCESS;
   }
   else if (adcResult.eStatus == ADC_RESULT_STALE)
   {
      status = ADC_STATUS_STALE;
   }
   else if (adcResult.eStatus == ADC_RESULT_TIMEOUT)
   {
      status = ADC_STATUS_TIMEOUT;
   }
   else
   {
      return ADC_STATUS_ERROR;
   }

   adc_result->physical = adcResult.nPhysical;
   adc_result->microvolts = adcResult.nMicrovolts;
   adc_result->code = adcResult.nCode;

   return status;
}

