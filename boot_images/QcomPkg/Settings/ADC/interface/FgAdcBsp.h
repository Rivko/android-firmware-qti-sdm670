#ifndef __FGADCBSP_H__
#define __FGADCBSP_H__
/*============================================================================
  @file FgAdcBsp.h

  Function and data structure declarations for FGADC DAL


                Copyright (c) 2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/boot.xf/2.1/QcomPkg/Settings/ADC/interface/FgAdcBsp.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "AdcScalingBsp.h"
#include "SpmiBus.h"
#include "pm_version.h"
#include "pm_gpio.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
/* These triggers should be ORed together */
#define FGADC_TRIGGER_DEFAULT      0
#define FGADC_TRIGGER_SLEEP        (1 << 0)
#define FGADC_TRIGGER_RFU          (1 << 1)
#define FGADC_TRIGGER_DTEST        (1 << 2)
#define FGADC_TRIGGER_BCL_HPM      (1 << 3)
#define FGADC_TRIGGER_OTG          (1 << 4)
#define FGADC_TRIGGER_DCIN         (1 << 5)
#define FGADC_TRIGGER_USB          (1 << 6)
#define FGADC_TRIGGER_EVERY_CYCLE  (1 << 7)

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   FGADC_SCALE_TO_MILLIVOLTS = 0,
   FGADC_SCALE_BATT_ID_TO_OHMS,
   FGADC_SCALE_INTERPOLATE_FROM_MILLIVOLTS,
   FGADC_SCALE_THERMISTOR,
   FGADC_SCALE_CURRENT_TO_MILLIAMPS
} FgAdcScalingMethodType;

typedef struct
{
   uint32 uNumerator;
   uint32 uDenominator;
} FgAdcChannelScalingFactor;

typedef struct
{
   int32 nMin;
   int32 nMax;
} FgAdcThreshRangeType;

typedef enum
{
   FGADC_CHAN_SKIN_TEMP = 0,
   FGADC_CHAN_BATT_ID,
   FGADC_CHAN_BATT_ID_FRESH,
   FGADC_CHAN_BATT_ID_5,
   FGADC_CHAN_BATT_ID_15,
   FGADC_CHAN_BATT_ID_150,
   FGADC_CHAN_BATT_THERM,
   FGADC_CHAN_AUX_THERM,
   FGADC_CHAN_USB_IN_V,
   FGADC_CHAN_USB_IN_I,
   FGADC_CHAN_DC_IN_V,
   FGADC_CHAN_DC_IN_I,
   FGADC_CHAN_DIE_TEMP,
   FGADC_CHAN_CHARGER_TEMP,
   FGADC_CHAN_GPIO
} FcAdcChannelType;

typedef enum
{
   FGADC_DISABLE = 0,
   FGADC_ENABLE
} FgAdcEnableType;

typedef struct
{
   const char *pszName;
   const AdcMapPtInt32toInt32Type *pInterpolationTable;
   FcAdcChannelType eChannel;
   FgAdcEnableType eEnable;
   FgAdcChannelScalingFactor scalingFactor;
   FgAdcScalingMethodType eScaling;
   uint32 uInterpolationTableLength;
   uint8 ucTriggers;
} FgAdcChannelConfigType;

typedef struct
{
   const FgAdcChannelConfigType *paChannels;
   const FgAdcThreshRangeType *pSkinTempThreshRange;
   const FgAdcThreshRangeType *pChgTempThreshRange;
   const pm_gpio_perph_index *paeGpios;
   uint32 uFullScale_code;
   uint32 uFullScale_uV;
   uint32 uMicroVoltsPerMilliAmps;
   uint32 uCodePerKelvin;
   uint32 uBattIdClipThresh;
   uint32 uNumChannels;
   uint32 uSlaveId;
   uint32 uPeripheralId;
   uint32 uNumGpios;
   uint32 uMaxWaitTimeUs;
   SpmiBus_AccessPriorityType eAccessPriority;
   uint8 ucPmicDevice;
   uint8 ucPerphType;
} FgAdcBspType;

#endif /* #ifndef __FGADCBSP_H__ */

