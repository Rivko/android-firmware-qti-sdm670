/*============================================================================
  FILE:         AdcInternalSettings.c

  OVERVIEW:     Internal settings for ADC.

  DEPENDENCIES: None

                Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
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
  2017-07-05  KS   Added support for 670.
  2016-05-05  jjo  Add FG ADC.
  2015-05-05  jjo  Use designated initializers.
  2014-04-02  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "AdcBsp.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))

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
/*
 * 660_PM660
 */
static const AdcPhysicalDeviceType adcPhysicalDevices_660_PM660[] =
{
   {
      .pszDevName = "/core/hwengines/adc/pmic_0/vadc",
   },
   /* FGADC PM660 */
   {
      .pszDevName = "/core/hwengines/adc/pmic_1/fgadc",
   },
};

const AdcBspType AdcBsp_660_PM660[] =
{
   {
      .paAdcPhysicalDevices = adcPhysicalDevices_660_PM660,
      .uNumDevices          = ARRAY_LENGTH(adcPhysicalDevices_660_PM660)
   }
};


