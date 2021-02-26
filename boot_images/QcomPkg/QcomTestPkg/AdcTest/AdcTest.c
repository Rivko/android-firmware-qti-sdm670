/*============================================================================
  FILE:         AdcTest.c

  OVERVIEW:     Exercises ADC EFI.

  DEPENDENCIES: None

                Copyright (c) 2011-2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*=============================================================================
                              EDIT HISTORY

 when       who     what, where, why
 --------   ---     -----------------------------------------------------------
 03/03/16   jjo     Update channels.
 01/23/15   jjo     Exercise APIs in DDIAdc.h.
 04/03/14   jjo     Added internal channels.
 03/27/14   jjo     Remove IADC.
 07/09/13   jjo     Check for invalid device / channel index.
 05/10/13   jjo     Added additional recalibration channels
 03/22/13   jjo     Added support for some targets not having IBATT_EXTERNAL
 03/05/13   jjo     Added support for IADC being removed
 01/15/13   jjo     Add test for synchronous V-I reading
 10/03/12   jjo     Added IADC tests
 09/06/12   jjo     Removed channel name length array
 07/03/12   aus     Replaced DEBUG with AsciiPrint
 07/30/11   sy      Adding TestInterface Library
 06/15/11   jdt     New File

=============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DDIAdc.h"
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/EFIAdc.h>
#include <Library/TestInterface.h>
#include <Library/QcomBaseLib.h>

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
static const CHAR8 *vAdcChannels[] =
{
   ADC_INPUT_VPH_PWR,
   ADC_INPUT_PMIC_TEMP1,
   ADC_INPUT_PMIC_HARDWARE_ID,
   ADC_INPUT_PLATFORM_ID,
   ADC_INPUT_VCOIN,
   ADC_INPUT_SYS_THERM1,
   ADC_INPUT_SYS_THERM2,
   ADC_INPUT_SYS_THERM3,
   ADC_INPUT_PA_THERM,
   ADC_INPUT_PA_THERM1,
   ADC_INPUT_XO_THERM,
   ADC_INPUT_VBATT,
   ADC_INPUT_VCHG,
   ADC_INPUT_BATT_THERM,
   ADC_INPUT_BATT_ID,
   ADC_INPUT_BATT_ID_OHMS,
   ADC_INPUT_BATT_ID_OHMS_FRESH,
   ADC_INPUT_SKIN_THERM,
   ADC_INPUT_PMIC_TEMP2,
   ADC_INPUT_CHG_TEMP,
   ADC_INPUT_USB_IN,
   ADC_INPUT_USB_IN_I,
   ADC_INPUT_DC_IN,
   ADC_INPUT_DC_IN_I,
   ADC_INPUT_FG_GPIO,
};

static UINT32 uNumVAdcChannels = sizeof(vAdcChannels) / sizeof(vAdcChannels[0]);

static const CHAR8 *recalAdcChannels[] =
{
   ADC_INPUT_VPH_PWR,
   ADC_INPUT_SYS_THERM1,
};

static UINT32 uNumRecalAdcChannels = sizeof(recalAdcChannels) / sizeof(recalAdcChannels[0]);

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
int AdcTestWrapper(void)
{
   AdcInputPropertiesType adcInputProps;
   AdcRecalibrationResultType recal;
   DalDeviceHandle *phAdcDev;
   uint32 uChannelNameLength;
   uint32 uChannelIdx;
   AdcResultType reading;
   DALResult result;

   AsciiPrint("\n\nAdcTestMain: Next test the ADC DXE wrapper\n");

   /*
    * Attach to ADC
    */
   result = DAL_AdcDeviceAttach(DALDEVICEID_ADC, &phAdcDev);
   if (result != DAL_SUCCESS)
   {
      AsciiPrint("ERROR: AdcTestMain: ADC attach failed\n");
      return -1;
   }

   AsciiPrint("AdcTestMain: attached to ADC\n");

   /*
    * Test reading all the VADC channels
    */
   AsciiPrint("AdcTestMain: Reading all ADC channels\n");

   for (uChannelIdx = 0; uChannelIdx < uNumVAdcChannels; uChannelIdx++)
   {
      UINT32 uStartUs;
      UINT32 uEndUs;
      uChannelNameLength = AsciiStrLen(vAdcChannels[uChannelIdx]) + 1;
      result = DalAdc_GetAdcInputProperties(phAdcDev, vAdcChannels[uChannelIdx], uChannelNameLength, &adcInputProps);
      if (result != DAL_SUCCESS)
      {
         AsciiPrint("AdcTestMain: channel %a is not present\n", vAdcChannels[uChannelIdx]);
         continue;
      }

      uStartUs = GetTimerCountus();
      result = DalAdc_Read(phAdcDev, &adcInputProps, &reading);
      uEndUs = GetTimerCountus();
      if (result != DAL_SUCCESS)
      {
         AsciiPrint("ERROR: AdcTestMain: DalAdc_Read failed\n");
         return -1;
      }

      if (reading.eStatus != ADC_RESULT_VALID &&
          reading.eStatus != ADC_RESULT_TIMEOUT &&
          reading.eStatus != ADC_RESULT_STALE)
      {
         AsciiPrint("ERROR: AdcTestMain: eStatus %d not valid for DalAdc_Read\n",
                    reading.eStatus);
         return -1;
      }

      if (reading.eStatus == ADC_RESULT_TIMEOUT)
      {
         AsciiPrint("Timed out: ");
      }
      else if (reading.eStatus == ADC_RESULT_STALE)
      {
         AsciiPrint("Stale: ");
      }

      AsciiPrint("Channel %a: nPhysical=%d, uMicroVolts=%d, uPercent=%d, uCode=0x%04x, uTimeUs=%d\n",
                 vAdcChannels[uChannelIdx],
                 reading.nPhysical,
                 reading.nMicrovolts,
                 reading.nPercent,
                 reading.nCode,
                 uEndUs - uStartUs);
   }

   /*
    * Test recalibration
    */
   AsciiPrint("AdcTestMain: Test recalibrating\n");

   for (uChannelIdx = 0; uChannelIdx < uNumRecalAdcChannels; uChannelIdx++)
   {
      uChannelNameLength = AsciiStrLen(recalAdcChannels[uChannelIdx]) + 1;
      result = DalAdc_GetAdcInputProperties(phAdcDev, recalAdcChannels[uChannelIdx], uChannelNameLength, &adcInputProps);
      if (result != DAL_SUCCESS)
      {
         AsciiPrint("AdcTestMain: channel %a is not present\n", recalAdcChannels[uChannelIdx]);
         continue;
      }

      result = DalAdc_Recalibrate(phAdcDev, &adcInputProps, &recal);
      if (result != DAL_SUCCESS)
      {
         AsciiPrint("ERROR: AdcTestMain: ADC protocol AdcRecalibrateChannel failed\n");
         return -1;
      }

      if (recal.eStatus != ADC_RESULT_VALID)
      {
         AsciiPrint("ERROR: AdcTestMain: eStatus not valid for AdcRecalibrateChannel\n");
         return -1;
      }

      AsciiPrint("Channel %a: nPhysical1=%d uV, nPhysical2=%d uV, uCode1=0x%x, uCode2=0x%04x\n",
                 recalAdcChannels[uChannelIdx],
                 recal.nPhysical1_uV,
                 recal.nPhysical2_uV,
                 recal.uCode1,
                 recal.uCode2);
   }

   return 0;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
AdcTestMain(
   IN EFI_HANDLE ImageHandle,
   IN EFI_SYSTEM_TABLE *SystemTable
   )
{
   EFI_STATUS status = EFI_SUCCESS;
   EFI_ADC_PROTOCOL *adc = NULL;
   UINT32 uChannelIdx;
   UINT32 uChannelNameLength;
   EfiAdcResultType adcResult;
   EfiAdcDeviceChannelType channel;
   EfiAdcRecalibrateChannelResultType adcRecalChanResult;

   TEST_START("AdcTest");

   AsciiPrint("AdcTestMain: First test the ADC protocl\n");

   /*
    * Load ADC protocol
    */
   status = gBS->LocateProtocol(&gEfiAdcProtocolGuid,
                                NULL,
                                (VOID**)&adc);
   if (status != EFI_SUCCESS)
   {
      AsciiPrint("ERROR: AdcTestMain: ADC protocol load failed\n");
      TestStatus("AdcTest", status);
      TEST_STOP("AdcTest");
      return status;
   }

   AsciiPrint("AdcTestMain: ADC protocol loaded\n");

   /*
    * Test reading all the ADC channels
    */
   AsciiPrint("AdcTestMain: Reading all ADC channels\n");

   for (uChannelIdx = 0; uChannelIdx < uNumVAdcChannels; uChannelIdx++)
   {
      UINT32 uStartUs;
      UINT32 uEndUs;
      uChannelNameLength = AsciiStrLen(vAdcChannels[uChannelIdx]) + 1;
      status = adc->GetChannel( vAdcChannels[uChannelIdx], uChannelNameLength, &channel );
      if (status != EFI_SUCCESS)
      {
         if (status == EFI_UNSUPPORTED)
         {
            AsciiPrint("AdcTestMain: channel %a is not present\n", vAdcChannels[uChannelIdx]);
            continue;
         }
         else
         {
            AsciiPrint("ERROR: AdcTestMain: ADC protocol GetChannel failed\n");
            goto end;
         }
      }

      uStartUs = GetTimerCountus();
      status = adc->AdcRead( &channel, &adcResult );
      uEndUs = GetTimerCountus();
      if (status != EFI_SUCCESS)
      {
         AsciiPrint("ERROR: AdcTestMain: ADC protocol AdcRead failed\n");
         goto end;
      }

      if (adcResult.eStatus != EFI_ADC_RESULT_VALID &&
          adcResult.eStatus != EFI_ADC_RESULT_TIMEOUT &&
          adcResult.eStatus != EFI_ADC_RESULT_STALE)
      {
         status = EFI_INVALID_PARAMETER;
         AsciiPrint("ERROR: AdcTestMain: eStatus %d not valid for AdcRead\n",
                    adcResult.eStatus);
         goto end;
      }

      if (adcResult.eStatus == EFI_ADC_RESULT_TIMEOUT)
      {
         AsciiPrint("Timed out: ");
      }
      else if (adcResult.eStatus == EFI_ADC_RESULT_STALE)
      {
         AsciiPrint("Stale: ");
      }

      AsciiPrint("Channel %a: nPhysical=%d, uMicroVolts=%d, uPercent=%d, uCode=0x%04x, uTimeUs=%d\n",
                 vAdcChannels[uChannelIdx],
                 adcResult.nPhysical,
                 adcResult.nMicrovolts,
                 adcResult.nPercent,
                 adcResult.nCode,
                 uEndUs - uStartUs);
   }

   /*
    * Test recalibration
    */
   AsciiPrint("AdcTestMain: Test recalibrating\n");

   for (uChannelIdx = 0; uChannelIdx < uNumRecalAdcChannels; uChannelIdx++)
   {
      uChannelNameLength = AsciiStrLen(recalAdcChannels[uChannelIdx]) + 1;
      status = adc->GetChannel(recalAdcChannels[uChannelIdx], uChannelNameLength, &channel);
      if (status != EFI_SUCCESS)
      {
         if (status == EFI_UNSUPPORTED)
         {
            AsciiPrint("AdcTestMain: channel %a is not present\n", vAdcChannels[uChannelIdx]);
            continue;
         }
         else
         {
            AsciiPrint("ERROR: AdcTestMain: ADC protocol GetChannel failed\n");
            goto end;
         }
      }

      status = adc->AdcRecalibrateChannel(&channel, &adcRecalChanResult);
      if (status != EFI_SUCCESS)
      {
         AsciiPrint("ERROR: AdcTestMain: ADC protocol AdcRecalibrateChannel failed\n");
         goto end;
      }

      if (adcRecalChanResult.eStatus != EFI_ADC_RESULT_VALID)
      {
         status = EFI_INVALID_PARAMETER;
         AsciiPrint("ERROR: AdcTestMain: eStatus not valid for AdcRecalibrateChannel\n");
         goto end;
      }

      AsciiPrint("Channel %a: nPhysical1=%d uV, nPhysical2=%d uV, uCode1=0x%x, uCode2=0x%04x\n",
                 recalAdcChannels[uChannelIdx],
                 adcRecalChanResult.nPhysical1_uV,
                 adcRecalChanResult.nPhysical2_uV,
                 adcRecalChanResult.uCode1,
                 adcRecalChanResult.uCode2);
   }

   if (0 != AdcTestWrapper())
   {
      status = EFI_UNSUPPORTED;
      goto end;
   }

#if 0
   status = adc->AdcSetFuelGaugeThreshold(EFI_ADC_THRESH_SKIN_HOT, 0);
   if (status != EFI_SUCCESS)
   {
      AsciiPrint("ERROR: AdcTestMain: setting skin hot threshold failed\n");
      goto end;
   }

   status = adc->AdcSetFuelGaugeThreshold(EFI_ADC_THRESH_SKIN_TOO_HOT, 97);
   if (status != EFI_SUCCESS)
   {
      AsciiPrint("ERROR: AdcTestMain: setting skin too hot threshold failed\n");
      goto end;
   }

   status = adc->AdcSetFuelGaugeThreshold(EFI_ADC_THRESH_CHARGER_HOT, 150);
   if (status != EFI_SUCCESS)
   {
      AsciiPrint("ERROR: AdcTestMain: setting charger hot threshold failed\n");
      goto end;
   }

   status = adc->AdcSetFuelGaugeThreshold(EFI_ADC_THRESH_CHARGER_TOO_HOT, 160);
   if (status != EFI_SUCCESS)
   {
      AsciiPrint("ERROR: AdcTestMain: setting charger too hot threshold failed\n");
      goto end;
   }
#endif

end:
   TestStatus("AdcTest", status);
   TEST_STOP("AdcTest");

   return EFI_SUCCESS;
}

