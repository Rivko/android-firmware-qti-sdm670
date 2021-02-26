/*============================================================================
  FILE:         AdcLib.c

  OVERVIEW:     Implementation of the ADC device wrapper layer

  DEPENDENCIES: None

                Copyright (c) 2012-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2017-03-13  jjo  Forced batt id workaround.
  2016-05-16  jjo  Status check.
  2015-01-23  jjo  Use DDIAdc.h.
  2014-04-02  jjo  Platform subtype check for 8994.
  2013-06-19  jjo  Platform subtype check for 8974.
  2012-12-03  jjo  Added IADC.
  2012-07-01  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DDIAdc.h"
#include "AdcBsp.h"
#include "DDIAdcDevice.h"
#include "pm_version.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ADC_MAGIC_NUM ((DalDeviceHandle *)0x43444156)

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct
{
   const char *pszDeviceString;
   DalDeviceHandle *phDevice;
} AdcDeviceType;

typedef struct
{
   AdcBspType *pAdcBsp;
   AdcDeviceType aAdcDevice[ADC_MAX_NUM_DEVICES];
} AdcBootContextType;

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
static boolean gbAdcInitialized = FALSE;
static AdcBootContextType gAdcCtxt;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static AdcResultStatusType DalAdc_ConvertStatus(AdcDeviceResultStatusType eDeviceStatus)
{
   switch (eDeviceStatus)
   {
      case ADC_DEVICE_RESULT_VALID:
         return ADC_RESULT_VALID;
      case ADC_DEVICE_RESULT_TIMEOUT:
         return ADC_RESULT_TIMEOUT;
      case ADC_DEVICE_RESULT_STALE:
         return ADC_RESULT_STALE;
      default:
         return ADC_RESULT_INVALID;
   }
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
DALResult DalAdc_GetAdcInputProperties(DalDeviceHandle *_h, const char *pszChannelName, uint32 nInputNameSize, AdcInputPropertiesType *pChannel)
{
   DALResult result;
   uint32 uDevice;
   uint32 uChannel;
   DalDeviceHandle *phDevice;

   if (_h != ADC_MAGIC_NUM || pszChannelName == NULL || pChannel == NULL)
   {
      return DAL_ERROR;
   }

   for (uDevice = 0; uDevice < gAdcCtxt.pAdcBsp->uNumDevices; uDevice++)
   {
      phDevice = gAdcCtxt.aAdcDevice[uDevice].phDevice;

      result = DalAdcDevice_GetInputProperties(phDevice, pszChannelName, &uChannel);
      if (result == DAL_SUCCESS)
      {
         pChannel->nDeviceIdx = uDevice;
         pChannel->nChannelIdx = uChannel;

         return DAL_SUCCESS;
      }
   }

   return DAL_ERROR;
}

DALResult DalAdc_Read(DalDeviceHandle *_h, const AdcInputPropertiesType *pChannel, AdcResultType *pAdcResult)
{
   DALResult result;
   uint32 uDevice;
   uint32 uChannel;
   DalDeviceHandle *phDevice;
   AdcDeviceResultType adcDeviceResult;

   if (_h != ADC_MAGIC_NUM || pChannel == NULL || pAdcResult == NULL)
   {
      return DAL_ERROR;
   }

   uDevice = pChannel->nDeviceIdx;

   if (uDevice >= gAdcCtxt.pAdcBsp->uNumDevices || uDevice >= ADC_MAX_NUM_DEVICES)
   {
      return DAL_ERROR;
   }

   uChannel = pChannel->nChannelIdx;
   phDevice = gAdcCtxt.aAdcDevice[uDevice].phDevice;

   adcDeviceResult.eStatus = ADC_DEVICE_RESULT_INVALID;
   adcDeviceResult.nPhysical = 0;
   adcDeviceResult.uPercent = 0;
   adcDeviceResult.uMicrovolts = 0;
   adcDeviceResult.uCode = 0;

   result = DalAdcDevice_ReadChannel(phDevice, uChannel, &adcDeviceResult);
   if (result != DAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   pAdcResult->eStatus = DalAdc_ConvertStatus(adcDeviceResult.eStatus);
   pAdcResult->nDeviceIdx = uDevice;
   pAdcResult->nChannelIdx = uChannel;
   pAdcResult->nPhysical = adcDeviceResult.nPhysical;
   pAdcResult->nPercent = adcDeviceResult.uPercent;
   pAdcResult->nMicrovolts = adcDeviceResult.uMicrovolts;
   pAdcResult->nCode = adcDeviceResult.uCode;

   return DAL_SUCCESS;
}

#ifndef ADC_LOADER_DRIVER
DALResult DalAdc_Recalibrate(DalDeviceHandle *_h, const AdcInputPropertiesType *pChannel, AdcRecalibrationResultType *pAdcRecalResult)
{
   DALResult result;
   uint32 uDevice;
   uint32 uChannel;
   DalDeviceHandle *phDevice;
   AdcDeviceRecalibrateResultType adcDeviceRecalibrateResult;

   if (_h != ADC_MAGIC_NUM || pChannel == NULL || pAdcRecalResult == NULL)
   {
      return DAL_ERROR;
   }

   uDevice = pChannel->nDeviceIdx;

   if (uDevice >= gAdcCtxt.pAdcBsp->uNumDevices || uDevice >= ADC_MAX_NUM_DEVICES)
   {
      return DAL_ERROR;
   }

   uChannel = pChannel->nChannelIdx;
   phDevice = gAdcCtxt.aAdcDevice[uDevice].phDevice;

   adcDeviceRecalibrateResult.eStatus = ADC_DEVICE_RESULT_INVALID;
   adcDeviceRecalibrateResult.nPhysical1_uV = 0;
   adcDeviceRecalibrateResult.nPhysical2_uV = 0;
   adcDeviceRecalibrateResult.uCode1 = 0;
   adcDeviceRecalibrateResult.uCode2 = 0;

   result = DalAdcDevice_RecalibrateChannel(phDevice, uChannel, &adcDeviceRecalibrateResult);
   if (result != DAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   pAdcRecalResult->eStatus = DalAdc_ConvertStatus(adcDeviceRecalibrateResult.eStatus);
   pAdcRecalResult->uDeviceIdx = uDevice;
   pAdcRecalResult->uChannelIdx = uChannel;
   pAdcRecalResult->nPhysical1_uV = adcDeviceRecalibrateResult.nPhysical1_uV;
   pAdcRecalResult->nPhysical2_uV = adcDeviceRecalibrateResult.nPhysical2_uV;
   pAdcRecalResult->uCode1 = adcDeviceRecalibrateResult.uCode1;
   pAdcRecalResult->uCode2 = adcDeviceRecalibrateResult.uCode2;

   return DAL_SUCCESS;
}
#endif

DALResult DalAdc_FGSetThreshold(DalDeviceHandle *_h, AdcFGThresholdType eWhichThresh, int32 nThreshold)
{
   AdcDeviceFGThresholdType eDeviceThresh;
   DALResult result;
   uint32 uDevice;
   DalDeviceHandle *phDevice;

   if (_h != ADC_MAGIC_NUM)
   {
      return DAL_ERROR;
   }

   switch (eWhichThresh)
   {
      case ADC_FG_THRESH_SKIN_HOT:
         eDeviceThresh = ADC_DEVICE_FG_THRESH_SKIN_HOT;
         break;
      case ADC_FG_THRESH_SKIN_TOO_HOT:
         eDeviceThresh = ADC_DEVICE_FG_THRESH_SKIN_TOO_HOT;
         break;
      case ADC_FG_THRESH_CHARGER_HOT:
         eDeviceThresh = ADC_DEVICE_FG_THRESH_CHARGER_HOT;
         break;
      case ADC_FG_THRESH_CHARGER_TOO_HOT:
         eDeviceThresh = ADC_DEVICE_FG_THRESH_CHARGER_TOO_HOT;
         break;
      default:
         return ADC_ERROR_INVALID_PARAMETER;
   }

   for (uDevice = 0; uDevice < gAdcCtxt.pAdcBsp->uNumDevices; uDevice++)
   {
      phDevice = gAdcCtxt.aAdcDevice[uDevice].phDevice;

      result = DalAdcDevice_FGSetThreshold(phDevice, eDeviceThresh, nThreshold);
      if (result == DAL_SUCCESS)
      {
         return DAL_SUCCESS;
      }
   }

   return DAL_ERROR;
}

DALResult DalAdc_FGSetBattIdForced(DalDeviceHandle *_h, DALBOOL bEnable)
{
   DALResult result;
   uint32 uDevice;
   DalDeviceHandle *phDevice;

   if (_h != ADC_MAGIC_NUM)
   {
      return DAL_ERROR;
   }

   for (uDevice = 0; uDevice < gAdcCtxt.pAdcBsp->uNumDevices; uDevice++)
   {
      phDevice = gAdcCtxt.aAdcDevice[uDevice].phDevice;

      result = DalAdcDevice_FGSetBattIdForced(phDevice, bEnable);
      if (result == DAL_SUCCESS)
      {
         return DAL_SUCCESS;
      }
   }

   return DAL_ERROR;
}

DALResult DAL_AdcDeviceAttach(DALDEVICEID DevId, DalDeviceHandle **phAdcDev)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hAdcProperties);
   const char *pszAdcBspPropName = "ADC_BSP";
   DALSYSPropertyVar propertyVar;
   const AdcBspType *pBsp;
   AdcDeviceType *pAdcDevice;
   DALResult result;
   uint32 uDevice;

   if (DevId != DALDEVICEID_ADC || phAdcDev == NULL)
   {
      return DAL_ERROR;
   }

   if (gbAdcInitialized)
   {
      *phAdcDev = ADC_MAGIC_NUM;
      return DAL_SUCCESS;
   }

   if (pm_get_pmic_model(0) == PMIC_IS_PM8998 && pm_get_pmic_model(1) == PMIC_IS_PMI8998)
   {
      /* 8998_PM8998_PMI8998 */
      pszAdcBspPropName = "ADC_BSP_8998_PM8998_PMI8998";
   }

   /* Get the properties */
   result = DALSYS_GetDALPropertyHandleStr("/core/hwengines/adc", hAdcProperties);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

   result = DALSYS_GetPropertyValue(hAdcProperties,
                                    pszAdcBspPropName,
                                    0,
                                    &propertyVar);
   if (result != DAL_SUCCESS)
   {
      return result;
   }

   gAdcCtxt.pAdcBsp = (AdcBspType *)propertyVar.Val.pStruct;
   pBsp = gAdcCtxt.pAdcBsp;

   if (pBsp->uNumDevices > ADC_MAX_NUM_DEVICES)
   {
      return DAL_ERROR;
   }

   for (uDevice = 0; uDevice < pBsp->uNumDevices; uDevice++)
   {
      pAdcDevice = &gAdcCtxt.aAdcDevice[uDevice];
      pAdcDevice->pszDeviceString = pBsp->paAdcPhysicalDevices[uDevice].pszDevName;

      result = DAL_AdcDeviceDeviceAttach(pAdcDevice->pszDeviceString,
                                         &pAdcDevice->phDevice);
      if (result != DAL_SUCCESS)
      {
         return DAL_ERROR;
      }
   }

   gbAdcInitialized = TRUE;
   *phAdcDev = ADC_MAGIC_NUM;

   return DAL_SUCCESS;
}

