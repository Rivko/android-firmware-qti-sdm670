#ifndef BOOT_TEMP_CHECK_BSP_H
#define BOOT_TEMP_CHECK_BSP_H
/*============================================================================
  @file BootTempCheckBsp.h

  Boot temp check BSP file.

                Copyright (c) 2012, 2014-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALStdDef.h"

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef struct
{
   int32 nUpperThresholdDegC;
   int32 nLowerThresholdDegC;
   uint32 uMaxNumIterations;
   uint32 uWaitPerIterationMicroSec;
   uint32 uSensor;
} BootTempCheckBspType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/

#endif /* #ifndef BOOT_TEMP_CHECK_BSP_H */
