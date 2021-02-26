/*============================================================================
  FILE:         VAdcCommon.c

  OVERVIEW:     Common scaling functions and helper functions for VADC.

  DEPENDENCIES: None

                Copyright (c) 2015-2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/common/src/VAdcCommon.c#1 $$DateTime: 2017/09/18 09:26:13 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-08-12  jjo  Pass pointer to scaling table.
  2015-08-20  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalVAdc.h"
#include "AdcScalingUtil.h"

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

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
/*======================================================================

  FUNCTION        VAdcScaleCodeToPercent

  DESCRIPTION     Scales the ADC result from code to percent (0 to
                  0xffff).

  DEPENDENCIES    None

  PARAMETERS
      uFullScale_code   [in]  full-scale ADC code
      uCode        [in]  the raw ADC code
      puPercent    [out] the value in percent (0 to 0xffff)

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
static void
VAdcScaleCodeToPercent(
   uint32 uFullScale_code,
   uint32 uCode,
   uint32 *puPercent
   )
{
   /* Note: max ADC code before overflow is 0x10000, which is far beyond
      the full scale reading of 0x4000 */
   *puPercent = (uCode * 0xffff) / uFullScale_code;

   return;
}

/*======================================================================

  FUNCTION        VAdcScaleCodeToMicrovolts

  DESCRIPTION     Scales the ADC result from code to microvolts.

  DEPENDENCIES    None

  PARAMETERS
      pScaling         [in] AMUX scaling
      uFullScale_code  [in] full-scale ADC code
      uFullScale_uV    [in] full-scale uV value
      uCode        [in]  the raw ADC code
      puMicrovolts [out] the value in microvolts

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
static void
VAdcScaleCodeToMicrovolts(
   const VAdcChannelScalingFactor *pScaling,
   uint32 uFullScale_code,
   uint32 uFullScale_uV,
   uint32 uCode,
   uint32 *puMicrovolts
   )
{
   uint64 u64Code = uCode;
   uint64 u64Microvolts;

   u64Microvolts = (u64Code * uFullScale_uV) / uFullScale_code;

   *puMicrovolts = (uint32)u64Microvolts;

   /*
    * Since PMIC AMUX channels have a prescalar gain applied to the input,
    * we need to scale input by inverse of the channel prescalar gain to
    * obtain the actual input voltage.
    */
   *puMicrovolts *= pScaling->uDenominator;
   *puMicrovolts /= pScaling->uNumerator;

   return;
}

/*======================================================================

  FUNCTION        VAdcScaleCodeToMicrovoltsInverse

  DESCRIPTION     Scales the ADC result from microvolts to code.

  DEPENDENCIES    None

  PARAMETERS
      pScaling         [in] AMUX scaling
      uFullScale_code  [in] full-scale ADC code
      uFullScale_uV    [in] full-scale uV value
      uMicrovolts      [in] the value in microvolts
      puCode           [out] the raw ADC code

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
static void
VAdcScaleCodeToMicrovoltsInverse(
   const VAdcChannelScalingFactor *pScaling,
   uint32 uFullScale_code,
   uint32 uFullScale_uV,
   uint32 uMicrovolts,
   uint32 *puCode
   )
{
   uint64 u64Code;
   uint64 u64Microvolts;

   /* First, remove the prescalar gain */
   uMicrovolts *= pScaling->uNumerator;
   uMicrovolts /= pScaling->uDenominator;

   u64Microvolts = uMicrovolts;
   u64Code = (u64Microvolts * uFullScale_code) / uFullScale_uV;
   *puCode = (uint32)u64Code;

   return;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*======================================================================

  FUNCTION        VAdcProcessConversionResult

  DESCRIPTION     This function is called at the end of conversion to process
                  the result. The raw result is converted into a scaled 16-bit
                  result and then and the scaled result is converted into
                  physical units in millivolts. Further scaling may be
                  performed by the scaling function defined in the BSP.

  DEPENDENCIES    None

  PARAMETERS
      pScaling         [in] AMUX scaling
      eScalingMethod   [in] scaling method
      pIntTable        [in] scaling table
      uPullUp          [in] pull up resistor value
      uFullScale_code  [in] full-scale ADC code
      uFullScale_uV    [in] full-scale uV value
      uCode        [in]  the raw ADC code
      puMicrovolts [out] the value in microvolts

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
void
VAdcProcessConversionResult(
   const VAdcChannelScalingFactor *pScaling,
   VAdcScalingMethodType eScalingMethod,
   const AdcIntTableType *pIntTable,
   uint32 uPullUp,
   uint32 uFullScale_code,
   uint32 uFullScale_uV,
   uint32 uCode,
   AdcDeviceResultType *pResult
   )
{
   pResult->eStatus = ADC_DEVICE_RESULT_VALID;
   pResult->uCode = uCode;

   VAdcScaleCodeToPercent(uFullScale_code,
                          uCode,
                          &pResult->uPercent);

   VAdcScaleCodeToMicrovolts(pScaling,
                             uFullScale_code,
                             uFullScale_uV,
                             uCode,
                             &pResult->uMicrovolts);

   /* By default, scale the physical result in units of millivolts */
   pResult->nPhysical = AdcDivideWithRounding((int32)pResult->uMicrovolts, 1000);

   /* Call the BSP scaling function (if present) */
   switch (eScalingMethod)
   {
      case VADC_SCALE_TO_MILLIVOLTS:
         /* by default, nPhysical is in millivolts */
         break;
      case VADC_SCALE_PMIC_SENSOR_TO_MILLIDEGREES:
         pResult->eStatus = VAdcScalePmicTherm(pResult->uMicrovolts,
                                               &pResult->nPhysical);
         break;
      case VADC_SCALE_THERMISTOR:
         pResult->eStatus = VAdcScaleThermistor(pResult->uCode,
                                                0,
                                                uFullScale_code,
                                                uPullUp,
                                                pIntTable->pTable,
                                                pIntTable->uLength,
                                                &pResult->nPhysical);
         break;
      case VADC_SCALE_INTERPOLATE_FROM_MILLIVOLTS:
         pResult->eStatus = AdcMapLinearInt32toInt32(pIntTable->pTable,
                                                     pIntTable->uLength,
                                                     pResult->nPhysical,
                                                     &pResult->nPhysical);
         break;
      default:
         /* By default, nPhysical is in millivolts */
         break;
   }

   return;
}

/*======================================================================

  FUNCTION        VAdcProcessConversionResultInverse

  DESCRIPTION     Inverse of VAdc_ProcessConversionResultInverse.

  DEPENDENCIES    None

  PARAMETERS
      pScaling         [in] AMUX scaling
      eScalingMethod   [in] scaling method
      pIntTable        [in] scaling table
      uPullUp          [in] pull up resistor value
      uFullScale_code  [in] full-scale ADC code
      uFullScale_uV    [in] full-scale uV value
      nPhysical        [in]  the physical value
      puMicrovolts [out] the value in microvolts

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
void
VAdcProcessConversionResultInverse(
   const VAdcChannelScalingFactor *pScaling,
   VAdcScalingMethodType eScalingMethod,
   const AdcIntTableType *pIntTable,
   uint32 uPullUp,
   uint32 uFullScale_code,
   uint32 uFullScale_uV,
   int32 nPhysical,
   AdcDeviceResultType *pResult
   )
{
   boolean bHaveCode = FALSE;
   int32 nMillivolts;

   pResult->eStatus = ADC_DEVICE_RESULT_VALID;
   pResult->nPhysical = nPhysical;

   /* Remaining: eStatus, uMicrovolts, uCode, uPercent */

   /* Reverse the BSP scaling */
   switch (eScalingMethod)
   {
      case VADC_SCALE_PMIC_SENSOR_TO_MILLIDEGREES:
         pResult->eStatus = VAdcScalePmicThermInverse(pResult->nPhysical,
                                                      &pResult->uMicrovolts);
         if (pResult->eStatus != ADC_DEVICE_RESULT_VALID)
         {
            return;
         }
         /* Remaining: uCode, uPercent */

         break;
      case VADC_SCALE_THERMISTOR:
         pResult->eStatus = VAdcScaleThermistorInverse(pResult->nPhysical,
                                                       0,
                                                       uFullScale_code,
                                                       uPullUp,
                                                       pIntTable->pTable,
                                                       pIntTable->uLength,
                                                       &pResult->uCode);
         if (pResult->eStatus != ADC_DEVICE_RESULT_VALID)
         {
            return;
         }

         VAdcScaleCodeToMicrovolts(pScaling,
                                   uFullScale_code,
                                   uFullScale_uV,
                                   pResult->uCode,
                                   &pResult->uMicrovolts);

         bHaveCode = TRUE;
         /* Remaining: uPercent */

         break;
      case VADC_SCALE_INTERPOLATE_FROM_MILLIVOLTS:
         /* Gets mV */
         pResult->eStatus = AdcMapLinearInt32toInt32Inverse(pIntTable->pTable,
                                                            pIntTable->uLength,
                                                            pResult->nPhysical,
                                                            &nMillivolts);
         if (pResult->eStatus != ADC_DEVICE_RESULT_VALID)
         {
            return;
         }

         pResult->uMicrovolts = (uint32)(nMillivolts * 1000);
         /* Remaining: uCode, uPercent */

         break;
      default:
         /* By default, nPhysical is in millivolts */
         pResult->uMicrovolts = (uint32)(nPhysical * 1000);
         /* Remaining: uCode, uPercent */

         break;
   }

   if (bHaveCode == FALSE)
   {
      VAdcScaleCodeToMicrovoltsInverse(pScaling,
                                       uFullScale_code,
                                       uFullScale_uV,
                                       pResult->uMicrovolts,
                                       &pResult->uCode);
      /* Remaining: uPercent */
   }

   /* Calculate the value in percent (0 to 0xffff) */
   VAdcScaleCodeToPercent(uFullScale_code,
                          pResult->uCode,
                          &pResult->uPercent);

   return;
}

