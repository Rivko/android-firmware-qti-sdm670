/*============================================================================
  FILE:         VAdcInternalSettings.c

  OVERVIEW:     Internal settings for VADC.

  DEPENDENCIES: None

                Copyright (c) 2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/SDM670Pkg/Settings/ADC/internal/VAdcInternalSettings.c#1 $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2017-07-05  KS   Added support for 670.
  2016-10-07  SA   Updated for SDM660.
  2016-08-12  jjo  Split BSP.
  2016-03-04  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "VAdcBsp.h"
#include "AdcInputs.h"

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
extern const VAdcChannelConfigType gVAdcChannels[];
extern const uint32 uNumVAdcChannels;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
/*
 * This table is a mapping of ADC channels to GPIO inputs.
 */
static const uint8 aucGpio2Channels[]  = {0x12, 0x32, 0x52, 0x72};
static const uint8 aucGpio3Channels[]  = {0x13, 0x33, 0x53, 0x73};
static const uint8 aucGpio4Channels[] = {0x14, 0x34, 0x54, 0x74, 0x94};
static const uint8 aucGpio5Channels[] = {0x15, 0x35, 0x55, 0x75};
static const uint8 aucGpio7Channels[] = {0x16, 0x36, 0x56, 0x76};
static const uint8 aucGpio9Channels[] = {0x17, 0x37, 0x57, 0x77, 0x97};
static const uint8 aucGpio10Channels[] = {0x18, 0x38, 0x58, 0x78};
static const uint8 aucGpio12Channels[] = {0x19, 0x39, 0x59, 0x79};

static const VAdcGpioChannelMappingType vAdcGpioChannelMappings[] =
{
   {
      .paucChannels = aucGpio2Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio2Channels),
      .eGpio        = PM_GPIO_2,
   },

   {
      .paucChannels = aucGpio3Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio3Channels),
      .eGpio        = PM_GPIO_3,
   },

   {
      .paucChannels = aucGpio4Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio4Channels),
      .eGpio        = PM_GPIO_4,
   },

   {
      .paucChannels = aucGpio5Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio5Channels),
      .eGpio        = PM_GPIO_5,
   },

   {
      .paucChannels = aucGpio7Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio7Channels),
      .eGpio        = PM_GPIO_7,
   },

   {
      .paucChannels = aucGpio9Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio9Channels),
      .eGpio        = PM_GPIO_9,
   },

   {
      .paucChannels = aucGpio10Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio10Channels),
      .eGpio        = PM_GPIO_10,
   },

   {
      .paucChannels = aucGpio12Channels,
      .uNumChannels = ARRAY_LENGTH(aucGpio12Channels),
      .eGpio        = PM_GPIO_12,
   },
};

const VAdcBspType VAdcBsp[] =
{
   {
      .paChannels              = gVAdcChannels,
      .puNumChannels           = &uNumVAdcChannels,
      .paAveragedChannels      = NULL,
      .puNumAveragedChannels   = NULL,
      .paGpioChannelMappings   = vAdcGpioChannelMappings,
      .uNumGpioChannelMappings = ARRAY_LENGTH(vAdcGpioChannelMappings),
      .bUsesInterrupts         = FALSE,
      .uFullScale_code         = 0x4000,
      .uFullScale_uV           = 1875000,
      .uReadTimeout_us         = 500000,
      .uLDOSettlingTime_us     = 17,
      .uSlaveId                = 0,
      .uPeripheralId           = 0x31,
      .uMasterID               = 0,
      .eAccessPriority         = SPMI_BUS_ACCESS_PRIORITY_LOW,
      .ucPmicDevice            = 0,
      .usMinDigRev             = VADC_REVISION(3, 1),
      .usMinAnaRev             = VADC_REVISION(1, 0),
      .ucPerphType             = 0x8,
   }
};

