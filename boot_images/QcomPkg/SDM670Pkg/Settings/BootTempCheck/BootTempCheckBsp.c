/*============================================================================
  FILE:         BootTempCheckBsp.c

  OVERVIEW:     Thresholds for the boot temp check.

  DEPENDENCIES: None

                Copyright (c) 2012, 2014-2015 Qualcomm Technologies, Inc.
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
  2015-05-05  jjo  Add looping parameters.
  2014-09-25  jjo  Updated include file.
  2012-07-30  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "BootTempCheckBsp.h"

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
const BootTempCheckBspType BootTempCheckBsp[] =
{
   {
      .nUpperThresholdDegC       = 150,
      .nLowerThresholdDegC       = -150,
      .uMaxNumIterations         = 40,
      .uWaitPerIterationMicroSec = 500000,
      .uSensor                   = 0
   }
};

