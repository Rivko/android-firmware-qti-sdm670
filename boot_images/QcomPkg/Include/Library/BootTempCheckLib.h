#ifndef __BOOT_TEMP_CHECK_LIB_H__
#define __BOOT_TEMP_CHECK_LIB_H__
/*============================================================================
  @file BootTempCheckLib.h

  This file contains data structures and functions used to determine if
  the temperature is within a range that is acceptable to continue booting

                Copyright (c) 2009-2012, 2014 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   BOOT_TEMP_CHECK_TEMP_OK = 0,
   BOOT_TEMP_CHECK_TSENS_TIMEOUT,
   BOOT_TEMP_CHECK_INVALID_TSENS_CAL,
   BOOT_TEMP_CHECK_TEMP_BELOW_THRESHOLD,
   BOOT_TEMP_CHECK_TEMP_ABOVE_THRESHOLD,
   BOOT_TEMP_CHECK_TSENS_ERROR,
   BOOT_TEMP_CHECK_SENSOR_NOT_AVAILABLE
} BootTempCheckStatusType;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/* ============================================================================
**
**  BootTempCheck
**
**  Description:
**    Checks the TSENS boot temperature.
**
**  BOOT_TEMP_CHECK_TEMP_OK indicates that the temperature is in an acceptable range to boot up.
**
**  BOOT_TEMP_CHECK_TSENS_TIMEOUT indicates that the TSENS sensor timed out while polling for a temperature ready status bit.
**
**  BOOT_TEMP_CHECK_INVALID_TSENS_CAL indicates that the TSENS sensor does not have a valid calibration data.
**
**  BOOT_TEMP_CHECK_TEMP_BELOW_THRESHOLD indicates that the temperature is below the acceptable temperature range.
**
**  BOOT_TEMP_CHECK_TEMP_ABOVE_THRESHOLD indicates that the temperature is above the acceptable temperature range.
**
**  BOOT_TEMP_CHECK_TSENS_ERROR indicates that an internal TSENS error occured.
**
**  BOOT_TEMP_CHECK_SENSOR_NOT_AVAILABLE indicates that the sensor is not available to be read.
**
**  Parameters:
**    None
**
**  Return:
**    TSENS
**
**  Dependencies:
**    Tsens_Init must be called once prior to this function to initialize
**    the TSENS hardware
**
** ========================================================================= */
BootTempCheckStatusType BootTempCheck(void);

#endif
