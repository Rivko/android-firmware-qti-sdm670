/*============================================================================
  FILE:         AdcScalingUtil.c

  OVERVIEW:     This file provides utility functions for the Analog-to-Digital
                Converter driver Board Support Package.

  DEPENDENCIES: None

                Copyright (c) 2008-2013, 2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/common/src/AdcScalingUtil.c#1 $$DateTime: 2017/09/18 09:26:13 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-04-23  jjo  Add thermistor scaling.
  2015-01-14  jjo  Added divide with rounding.
  2013-02-19  jjo  Added inverse scaling functions.
  2012-06-11  jjo  Added BSP scaling functions.
  2008-03-16  jdt  Initial revision.

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
/*===========================================================================

  FUNCTION        AdcLinearInterpolate

  DESCRIPTION     Performs a linear interpolation.

  DEPENDENCIES    None

  PARAMETERS      nNum [in] Numerator
                  nDen [in] Denominator

  RETURN VALUE    Result

  SIDE EFFECTS    None

===========================================================================*/
static int32
AdcLinearInterpolate(
   int32 x,
   int32 x0,
   int32 x1,
   int32 y0,
   int32 y1
   )
{
   int32 y;
   int32 num;
   int32 den;

   num = (y1 - y0) * (x - x0);
   den = x1 - x0;

   y = y0 + AdcDivideWithRounding(num, den);

   return y;
}

/*===========================================================================

  FUNCTION        VAdc_WeightedAverage

  DESCRIPTION     Calculates a weighted average

  DEPENDENCIES    None

  PARAMETERS      nW1   [in] weight 1
                  nVal1 [in] value 1
                  nW2   [in] weight 2
                  nVal2 [in] value 2

  RETURN VALUE    Result

  SIDE EFFECTS    None

===========================================================================*/
static int32
VAdc_WeightedAverage(
   int32 nW1,
   int32 nVal1,
   int32 nW2,
   int32 nVal2
   )
{
   int32 nNum;
   int32 nDen;

   nNum = (nW1 * nVal1) + (nW2 * nVal2);
   nDen = nW1 + nW2;

   return AdcDivideWithRounding(nNum, nDen);
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*===========================================================================

  FUNCTION        AdcMapLinearInt32toInt32

  DESCRIPTION     This function uses linear interpolation to calculate an
                  output value given an input value and a map table that maps
                  input values to output values.

                  *pnOutput = (y2 - y1) * (nInput - x1) / (x2 - x1) + y1

  DEPENDENCIES    Requires the ADC DAL

  PARAMETERS      paPts      [in] Mapping between sensor output voltage and the
                                  actual temp (must be ascending or descending)
                  uTableSize [in] Number of entries in paPts
                  nInput     [in] Input value (x)
                  pnOutput   [out] Result (y) interpolated using input value and table

  RETURN VALUE    ADC_DEVICE_RESULT_INVALID if there is an error with
                  the parameters or in performing the calculation.

                  ADC_DEVICE_RESULT_VALID if successful.

  SIDE EFFECTS    None

===========================================================================*/
AdcDeviceResultStatusType
AdcMapLinearInt32toInt32(
   const AdcMapPtInt32toInt32Type *paPts,
   uint32 uTableSize,
   int32 nInput,
   int32 *pnOutput
   )
{
   DALBOOL bDescending = TRUE;
   uint32 uSearchIdx = 0;
   int32 nX1, nX2, nY1, nY2;

   if ((paPts == NULL) || (pnOutput == NULL))
   {
      return ADC_DEVICE_RESULT_INVALID;
   }

   /* Check if table is descending or ascending */
   if (uTableSize > 1)
   {
      if (paPts[0].x < paPts[1].x)
      {
         bDescending = FALSE;
      }
   }

   while (uSearchIdx < uTableSize)
   {
      if ( (bDescending == TRUE) && (paPts[uSearchIdx].x < nInput) )
      {
        /* table entry is less than measured value and table is descending, stop */
        break;
      }
      else if ( (bDescending == FALSE) && (paPts[uSearchIdx].x > nInput) )
      {
        /* table entry is greater than measured value and table is ascending, stop */
        break;
      }
      else
      {
        uSearchIdx++;
      }
   }

   if (uSearchIdx == 0)
   {
      *pnOutput = paPts[0].y;
   }
   else if (uSearchIdx == uTableSize)
   {
      *pnOutput = paPts[uTableSize-1].y;
   }
   else
   {
      /* result is between search_index and search_index-1 */
      /* interpolate linearly */
      nX1 = paPts[uSearchIdx - 1].x;
      nX2 = paPts[uSearchIdx].x;
      nY1 = paPts[uSearchIdx-1].y;
      nY2 = paPts[uSearchIdx].y;

      *pnOutput = AdcLinearInterpolate(nInput,
                                       nX1,
                                       nX2,
                                       nY1,
                                       nY2);
   }
   return ADC_DEVICE_RESULT_VALID;
}

/*===========================================================================

  FUNCTION        AdcMapLinearInt32toInt32Inverse

  DESCRIPTION     Inverse of AdcMapLinearInt32toInt32:

                  *pnOutput = (x2 - x1) * (nInput - y1) / (y2 - y1) + x1

  DEPENDENCIES    Requires the ADC DAL

  PARAMETERS      paPts      [in] Mapping between sensor output voltage and the
                                  actual temp (must be ascending or descending)
                  uTableSize [in] Number of entries in paPts
                  nInput     [in] Input value (y)
                  pnOutput   [out] Result (x) interpolated using input value and table

  RETURN VALUE    ADC_DEVICE_RESULT_INVALID if there is an error with
                  the parameters or in performing the calculation.

                  ADC_DEVICE_RESULT_VALID if successful.

  SIDE EFFECTS    None

===========================================================================*/
AdcDeviceResultStatusType
AdcMapLinearInt32toInt32Inverse(
   const AdcMapPtInt32toInt32Type *paPts,
   uint32 uTableSize,
   int32 nInput,
   int32 *pnOutput
   )
{
   DALBOOL bDescending = TRUE;
   uint32 uSearchIdx = 0;
   int32 nX1, nX2, nY1, nY2;

   if (paPts == NULL || pnOutput == NULL)
   {
      return ADC_DEVICE_RESULT_INVALID;
   }

   /* Check if table is descending or ascending */
   if (uTableSize > 1)
   {
      if (paPts[0].y < paPts[1].y)
      {
         bDescending = FALSE;
      }
   }

   while (uSearchIdx < uTableSize)
   {
      if ( (bDescending == TRUE) && (paPts[uSearchIdx].y < nInput) )
      {
        /* table entry is less than measured value and table is descending, stop */
        break;
      }
      else if ( (bDescending == FALSE) && (paPts[uSearchIdx].y > nInput) )
      {
        /* table entry is greater than measured value and table is ascending, stop */
        break;
      }
      else
      {
        uSearchIdx++;
      }
   }

   if (uSearchIdx == 0)
   {
      *pnOutput = paPts[0].x;
   }
   else if (uSearchIdx == uTableSize)
   {
      *pnOutput = paPts[uTableSize-1].x;
   }
   else
   {
      /* result is between search_index and search_index - 1 */
      /* interpolate linearly */
      nX1 = paPts[uSearchIdx - 1].x;
      nX2 = paPts[uSearchIdx].x;
      nY1 = paPts[uSearchIdx-1].y;
      nY2 = paPts[uSearchIdx].y;

      *pnOutput = AdcLinearInterpolate(nInput,
                                       nY1,
                                       nY2,
                                       nX1,
                                       nX2);
   }
   return ADC_DEVICE_RESULT_VALID;
}

/*===========================================================================

  FUNCTION        VAdcScalePmicTherm

  DESCRIPTION     Scales the ADC result from millivolts to 0.001 degrees
                  Celsius using the PMIC thermistor conversion equation.

  DEPENDENCIES    None

  PARAMETERS      uMicrovolts [in]
                  pnPhyiscal [out]

  RETURN VALUE    AdcDeviceResultStatusType

  SIDE EFFECTS    None

===========================================================================*/
AdcDeviceResultStatusType
VAdcScalePmicTherm(
   uint32 uMicrovolts,
   int32 *pnPhysical
   )
{
   /*
    * Divide by two to convert from microvolt reading to micro-Kelvin.
    *
    * Subtract 273160 to convert the temperature from Kelvin to
    * 0.001 degrees Celsius.
    */
   *pnPhysical = AdcDivideWithRounding((int32)uMicrovolts, 2) - 273160;

   return ADC_DEVICE_RESULT_VALID;
}

/*===========================================================================

  FUNCTION        VAdcScalePmicThermInverse

  DESCRIPTION     Scales the ADC result from physical units of 0.001 deg C to
                  microvolts.

  DEPENDENCIES    None

  PARAMETERS      nPhysical     [in]
                  puMicrovolts [out]

  RETURN VALUE    AdcDeviceResultStatusType

  SIDE EFFECTS    None

===========================================================================*/
AdcDeviceResultStatusType
VAdcScalePmicThermInverse(
   int32 nPhysical,
   uint32 *puMicrovolts
   )
{
   /*
    * Add 273160 to convert temp from Kelvin to mDegC.
    *
    * Multiply by 2 to convert from  micro-Kelvin to microvolt.
    */
   *puMicrovolts = (uint32)((nPhysical + 273160) * 2);

   return ADC_DEVICE_RESULT_VALID;
}

/*===========================================================================

  FUNCTION        VAdcScaleThermistor

  DESCRIPTION     Scaled an ADC code to physical units for a thermistor.

  DEPENDENCIES    None

  PARAMETERS      uCode [in] ADC reading
                  uVrefN [in] ADC code of GND
                  uVrefP [in] ADC code of VREF
                  uPullUp [in] thermistor pull-up
                  paMap [in] interpolation table
                  uTableSize [in] interpolation table size
                  pnPhysical [out] calibrated physical value

  RETURN VALUE    ADC_DEVICE_RESULT_VALID or an error

  SIDE EFFECTS    None

===========================================================================*/
AdcDeviceResultStatusType
VAdcScaleThermistor(
   uint32 uCode,
   uint32 uVrefN,
   uint32 uVrefP,
   uint32 uPullUp,
   const AdcMapPtInt32toInt32Type *paMap,
   uint32 uTableSize,
   int32 *pnPhysical
   )
{
   int64 llCode = uCode;
   int64 llVrefN = uVrefN;
   int64 llVrefP = uVrefP;
   int64 llRp = uPullUp;
   int64 llNum;
   int64 llDenom;
   int32 nRt;
   AdcDeviceResultStatusType resultStatus;

   /*
    *
    *                    C2 ___  (Vdd)
    *                          |
    *                          |
    *                          >
    *                      Rp  < (pull up)
    *                          >
    *                          |
    *                          |
    *                          |- - - Cin
    *                          |
    *                          |
    *                          >
    *                      Rt  < (thermistor)
    *                          >
    *                          |
    *                          |
    *                    C1 ---  (gnd)
    *
    * Voltage divider equation:
    *     Cin - C1 = (C2 - C1) * Rt / (Rp + Rt)
    *
    * Solving for Rt:
    *     Rt = (Cin - C1) * Rp / (C2 - Cin)
    *
    */
   if (llCode < llVrefN)
   {
      llCode = llVrefN;
   }
   else if (llCode > llVrefP)
   {
      llCode = llVrefP;
   }

   llNum = (llCode - llVrefN) * llRp;
   llDenom = llVrefP - llCode;

   if (llDenom == 0)
   {
      nRt = 0x7fffffff;
   }
   else
   {
      nRt = (int32)(llNum / llDenom);
   }

   resultStatus = AdcMapLinearInt32toInt32(paMap,
                                           uTableSize,
                                           nRt,
                                           pnPhysical);

   return resultStatus;
}

/*===========================================================================

  FUNCTION        VAdcScaleThermistorInverse

  DESCRIPTION     Performs the reverse scaling of a thermistor channel.

  DEPENDENCIES    None

  PARAMETERS      nPhysical [in] thermistor temperature
                  uVrefN [in] ADC code of GND
                  uVrefP [in] ADC code of VREF
                  uPullUp [in] thermistor pull-up
                  paMap [in] interpolation table
                  uTableSize [in] interpolation table size
                  puCode [out] ADC code

  RETURN VALUE    ADC_DEVICE_RESULT_VALID or an error

  SIDE EFFECTS    None

===========================================================================*/
AdcDeviceResultStatusType
VAdcScaleThermistorInverse(
   int32 nPhysical,
   uint32 uVrefN,
   uint32 uVrefP,
   uint32 uPullUp,
   const AdcMapPtInt32toInt32Type *paMap,
   uint32 uTableSize,
   uint32 *puCode
   )
{
   int32 nRt;
   int64 llCode;
   int64 llRt;
   int64 llRp = uPullUp;
   int64 llVrefN = uVrefN;
   int64 llVrefP = uVrefP;
   AdcDeviceResultStatusType resultStatus;

   resultStatus = AdcMapLinearInt32toInt32Inverse(paMap,
                                                  uTableSize,
                                                  nPhysical,
                                                  &nRt);

   llRt = nRt;

   llCode = ((llVrefP - llVrefN) * llRt) / (llRp + llRt) + llVrefN;

   if (llCode < llVrefN)
   {
      llCode = llVrefN;
   }
   else if (llCode > llVrefP)
   {
      llCode = llVrefP;
   }

   *puCode = (uint32)llCode;

   return resultStatus;
}

/*===========================================================================

  FUNCTION        AdcDivideWithRounding

  DESCRIPTION     Performs a division with rounding.

  DEPENDENCIES    None

  PARAMETERS      nNum [in] Numerator
                  nDen [in] Denominator

  RETURN VALUE    Result

  SIDE EFFECTS    None

===========================================================================*/
int32
AdcDivideWithRounding(
   int32 nNum,
   int32 nDen
   )
{
   int32 nOffset = nDen / 2;

   if (nNum < 0)
   {
      nOffset *= -1;
   }

   return (nNum + nOffset) / nDen;
}

/*===========================================================================

  FUNCTION        VAdcBlendResult

  DESCRIPTION     Blends results for two channels. The physical value is
                  blended using a weighted average. The remaining values
                  are blended using averaging.

  DEPENDENCIES    None

  PARAMETERS      nPhysical   [in] neutral reading
                  pLowResult  [in] result accurate at low physical values
                  nLowLimit   [in] anything below is 100% low result
                  pHighResult [in] result accurate at high physical values
                  nHighLimit  [in] anything above is 100% high result
                  pBlendedResult [out] blended result

  RETURN VALUE    None

  SIDE EFFECTS    None

===========================================================================*/
void
VAdcBlendResult(
   int32 nPhysical,
   AdcDeviceResultType *pLowResult,
   int32 nLowLimit,
   AdcDeviceResultType *pHighResult,
   int32 nHighLimit,
   AdcDeviceResultType *pBlendedResult
   )
{
   int32 nLowWeight;
   int32 nHighWeight;
   int32 nNum;
   int32 nDen;

   pBlendedResult->eStatus = ADC_DEVICE_RESULT_VALID;

   if (nPhysical <= nLowLimit)
   {
      pBlendedResult->nPhysical = pLowResult->nPhysical;
      pBlendedResult->uMicrovolts = pLowResult->uMicrovolts;
      pBlendedResult->uPercent = pLowResult->uPercent;
      pBlendedResult->uCode = pLowResult->uCode;
      return;
   }
   else if (nPhysical >= nHighLimit)
   {
      pBlendedResult->nPhysical = pHighResult->nPhysical;
      pBlendedResult->uMicrovolts = pHighResult->uMicrovolts;
      pBlendedResult->uPercent = pHighResult->uPercent;
      pBlendedResult->uCode = pHighResult->uCode;
      return;
   }

   nNum = 1000 * (nPhysical - nLowLimit);
   nDen = nHighLimit - nLowLimit;
   nHighWeight = AdcDivideWithRounding(nNum, nDen);
   nLowWeight = 1000 - nHighWeight;

   pBlendedResult->nPhysical = VAdc_WeightedAverage(nLowWeight,
                                                    pLowResult->nPhysical,
                                                    nHighWeight,
                                                    pHighResult->nPhysical);

   /* To avoid overflows, just average these results */
   pBlendedResult->uMicrovolts = (pLowResult->uMicrovolts + pHighResult->uMicrovolts) / 2;
   pBlendedResult->uPercent = (pLowResult->uPercent + pHighResult->uPercent) / 2;
   pBlendedResult->uCode = (pLowResult->uCode + pHighResult->uCode) / 2;

   return;
}

