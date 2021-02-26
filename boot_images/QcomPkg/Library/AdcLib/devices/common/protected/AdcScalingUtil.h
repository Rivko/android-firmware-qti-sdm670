#ifndef __ADC_SCALING_UTIL_H__
#define __ADC_SCALING_UTIL_H__
/*============================================================================
  @file AdcScalingUtil.h

  Analog-to-Digital Converter utility functions.

  This file defines the logical ADC channel identifiers that are specific to
  a board design. The customer may add their own channel identifiers
  here if they wish to extend the BSP with additional ADC channels.

                Copyright (c) 2008-2013, 2015-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/common/protected/AdcScalingUtil.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AdcScalingBsp.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/
/**
   @brief Piece-wise linear interpolation function

   This function uses linear interpolation to calculate an output value given
   an input value and a map table that maps input values to output values.

   @param paPts Pointer to an array of points which define the
                piece-wise linear interpolation function
   @param uTableSize The number of points in the array
   @param nInput The input value to the linear interpolation function
   @param pnOutput The output of the linear interpolation function

   @return: ADC_DEVICE_RESULT_VALID on success, else an AdcDeviceResultStatusType error
*/
AdcDeviceResultStatusType
AdcMapLinearInt32toInt32(
   const AdcMapPtInt32toInt32Type *paPts,
   uint32 uTableSize,
   int32 nInput,
   int32 *pnOutput
   );

AdcDeviceResultStatusType
AdcMapLinearInt32toInt32Inverse(
   const AdcMapPtInt32toInt32Type *paPts,
   uint32 uTableSize,
   int32 nInput,
   int32 *pnOutput
   );

AdcDeviceResultStatusType
VAdcScaleTdkNTCGTherm(
   uint32 uCode,
   uint32 uVrefN,
   uint32 uVrefP,
   const AdcMapPtInt32toInt32Type *paMap,
   uint32 uTableSize,
   int32 *pnPhysical
   );

AdcDeviceResultStatusType
VAdcScaleTdkNTCGThermInverse(
   int32 nPhysical,
   uint32 uVrefN,
   uint32 uVrefP,
   const AdcMapPtInt32toInt32Type *paMap,
   uint32 uTableSize,
   uint32 *puCode
   );

AdcDeviceResultStatusType
VAdcScalePmicTherm(
   uint32 uMicrovolts,
   int32 *pnPhysical
   );

AdcDeviceResultStatusType
VAdcScalePmicThermInverse(
   int32 nPhysical,
   uint32 *puMicrovolts
   );

AdcDeviceResultStatusType
VAdcScaleThermistor(
   uint32 uCode,
   uint32 uVrefN,
   uint32 uVrefP,
   uint32 uPullUp,
   const AdcMapPtInt32toInt32Type *paMap,
   uint32 uTableSize,
   int32 *pnPhysical
   );

AdcDeviceResultStatusType
VAdcScaleThermistorInverse(
   int32 nPhysical,
   uint32 uVrefN,
   uint32 uVrefP,
   uint32 uPullUp,
   const AdcMapPtInt32toInt32Type *paMap,
   uint32 uTableSize,
   uint32 *puCode
   );

int32
AdcDivideWithRounding(
   int32 nNum,
   int32 nDen
   );

void
VAdcBlendResult(
   int32 nPhysical,
   AdcDeviceResultType *pLowResult,
   int32 nLowLimit,
   AdcDeviceResultType *pHighResult,
   int32 nHighLimit,
   AdcDeviceResultType *pBlendedResult
   );

#endif /* #ifndef __ADC_SCALING_UTIL_H__ */

