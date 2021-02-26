#ifndef __TSENS_LIB_H__
#define __TSENS_LIB_H__
/*============================================================================
  @file TsensLib.h

  MSM on-die temperature sensor library API.

  This file contains data structures and functions used to configure, control,
  and query temperature sensors.

               Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
               All Rights Reserved.
               Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DALStdDef.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   TSENS_SUCCESS,
   TSENS_ERROR,
   TSENS_ERROR_INVALID_CALIBRATION,
   TSENS_ERROR_TIMEOUT,
   TSENS_ERROR_NOT_INITIALIZED,
   TSENS_ERROR_SENSOR_NOT_AVAILABLE
} TsensResultType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/**
   @brief Initializes TSENS library.

   Initialize the TSENS library for loader.

   @return TSENS_SUCCESS, else an error.
 */
TsensResultType Tsens_Init(void);

/**
   @brief Returns the number of sensors.

   @param puNumSensors: [out] number of sensors.

   @see Tsens_Init

   @return TSENS_SUCCESS, else an error.
 */
TsensResultType Tsens_GetNumSensors(uint32 *puNumSensors);

/**
   @brief Returns the temperature for the given sensor.

   Returns the temperature for a given sensor in tenths of a degree C.
   The number of sensors can be obtained by calling
   Tsens_GetNumSensors.

   @param uSensor: [in] sensor index (zero-based).
   @param pnTenthDegC: [out] temperature in tenths of a degree C.

   @see Tsens_Init

   @return TSENS_SUCCESS, else an error.
 */
TsensResultType Tsens_GetTemp(uint32 uSensor, int32 *pnTenthDegC);

/**
   @brief Returns the maximum temperature.

   Returns the maximum temperature of all the sensors during the last
   measurement in tenths of a degree C.

   @param pnMaxTenthDegC: [out] maximum temperature in tenths of a degree C.

   @see Tsens_Init

   @return TSENS_SUCCESS, else an error.
 */
TsensResultType Tsens_GetMaxTemp(int32 *pnMaxTenthDegC);

#endif /* #ifndef __TSENS_LIB_H__ */

