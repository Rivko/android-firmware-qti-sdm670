/*============================================================================
  FILE:         AdcDxeWrapper.c

  OVERVIEW:     Wrapper around ADC DXE.

  DEPENDENCIES: None

                Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
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
  2016-05-16  jjo  Status check.
  2015-01-23  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/EFIAdc.h>
#include "DDIAdc.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

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
static EFI_ADC_PROTOCOL *gpAdc = NULL;

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static AdcResultStatusType DalAdc_ConvertStatus(EfiAdcResultStatusType eStatus)
{
   switch (eStatus)
   {
      case EFI_ADC_RESULT_VALID:
         return ADC_RESULT_VALID;
      case EFI_ADC_RESULT_TIMEOUT:
         return ADC_RESULT_TIMEOUT;
      case EFI_ADC_RESULT_STALE:
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
   EfiAdcDeviceChannelType channel;
   EFI_STATUS status;

   if (pszChannelName == NULL || pChannel == NULL)
   {
      return ADC_ERROR_NULL_POINTER;
   }

   status = gpAdc->GetChannel(pszChannelName, nInputNameSize, &channel);
   if (status != EFI_SUCCESS)
   {
      return DAL_ERROR;
   }

   pChannel->nDeviceIdx = channel.nDeviceIdx;
   pChannel->nChannelIdx = channel.nChannelIdx;

   return DAL_SUCCESS;
}

DALResult DalAdc_Read(DalDeviceHandle *_h, const AdcInputPropertiesType *pChannel, AdcResultType *pAdcResult)
{
   EfiAdcDeviceChannelType channel;
   EfiAdcResultType result;
   EFI_STATUS status;

   if (pChannel == NULL || pAdcResult == NULL)
   {
      return ADC_ERROR_NULL_POINTER;
   }

   channel.nDeviceIdx = pChannel->nDeviceIdx;
   channel.nChannelIdx = pChannel->nChannelIdx;

   status = gpAdc->AdcRead(&channel, &result);
   if (status != EFI_SUCCESS)
   {
      return DAL_ERROR;
   }

   pAdcResult->eStatus = DalAdc_ConvertStatus(result.eStatus);
   pAdcResult->nDeviceIdx = pChannel->nDeviceIdx;
   pAdcResult->nChannelIdx = pChannel->nChannelIdx;
   pAdcResult->nPhysical = result.nPhysical;
   pAdcResult->nPercent = result.nPercent;
   pAdcResult->nMicrovolts = result.nMicrovolts;
   pAdcResult->nCode = result.nCode;

   return DAL_SUCCESS;
}

DALResult DalAdc_Recalibrate(DalDeviceHandle *_h, const AdcInputPropertiesType *pChannel, AdcRecalibrationResultType *pAdcRecalResult)
{
   EfiAdcRecalibrateChannelResultType result;
   EfiAdcDeviceChannelType channel;
   EFI_STATUS status;

   if (pChannel == NULL || pAdcRecalResult == NULL)
   {
      return ADC_ERROR_NULL_POINTER;
   }

   channel.nDeviceIdx = pChannel->nDeviceIdx;
   channel.nChannelIdx = pChannel->nChannelIdx;

   status = gpAdc->AdcRecalibrateChannel(&channel, &result);
   if (status != EFI_SUCCESS)
   {
      return DAL_ERROR;
   }

   pAdcRecalResult->eStatus = DalAdc_ConvertStatus(result.eStatus);
   pAdcRecalResult->uDeviceIdx = pChannel->nDeviceIdx;
   pAdcRecalResult->uChannelIdx = pChannel->nChannelIdx;
   pAdcRecalResult->nPhysical1_uV = result.nPhysical1_uV;
   pAdcRecalResult->nPhysical2_uV = result.nPhysical2_uV;
   pAdcRecalResult->uCode1 = result.uCode1;
   pAdcRecalResult->uCode2 = result.uCode2;

   return DAL_SUCCESS;
}

DALResult DAL_AdcDeviceAttach(DALDEVICEID DevId, DalDeviceHandle **phAdcDev)
{
   EFI_STATUS status;

   if (DevId != DALDEVICEID_ADC)
   {
      return DAL_ERROR;
   }

   *phAdcDev = NULL;

   if (gbAdcInitialized == TRUE)
   {
      return DAL_SUCCESS;
   }

   status = gBS->LocateProtocol(&gEfiAdcProtocolGuid, NULL, (VOID**)&gpAdc);
   if (status != EFI_SUCCESS)
   {
      return status;
   }

   gbAdcInitialized = TRUE;

   return DAL_SUCCESS;
}

