#ifndef __ADC_SCALING_BSP_H__
#define __ADC_SCALING_BSP_H__
/*============================================================================
  @file AdcScalingBsp.h

  Function and data structure declarations for ADC's scaling


               Copyright (c) 2017 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "com_dtypes.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef struct
{
   int32 x;
   int32 y;
} AdcMapPtInt32toInt32Type;

typedef struct
{
   const AdcMapPtInt32toInt32Type *pTable;
   uint32 uLength;
} AdcIntTableType;

typedef struct
{
   uint32 uNumerator;
   uint32 uDenominator;
} VAdcChannelScalingFactor;

typedef enum
{
   VADC_SCALE_TO_MILLIVOLTS = 0,
   VADC_SCALE_PMIC_SENSOR_TO_MILLIDEGREES,
   VADC_SCALE_INTERPOLATE_FROM_MILLIVOLTS,
   VADC_SCALE_THERMISTOR
} VAdcScalingMethodType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

#endif /* #ifndef __ADC_SCALING_BSP_H__ */

