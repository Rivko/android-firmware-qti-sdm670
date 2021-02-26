/*====================================================================================
File: aba_foss.h

This file contains the declaration for types that are specific to FOSS.
These types are FOSS configuration parameters.

Copyright (c) 2014-2018 Qualcomm Technologies, Inc.
All Rights Reserved.
Confidential and Proprietary - Qualcomm Technologies, Inc.
=====================================================================================*/

#ifndef ABA_FOSS_H
#define ABA_FOSS_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/*=================================================================================================
Defines
=================================================================================================*/

#ifndef TRUE
#define TRUE   1   /* Boolean true value. */
#endif

#ifndef FALSE
#define FALSE  0   /* Boolean false value. */
#endif

#ifndef _BOOL32_DEFINED
typedef  unsigned long int  bool32;        /* Boolean value type. */
#define _BOOL32_DEFINED
#endif

#define ABA_FOSS_VERSION_MAJOR           0x00000001
#define ABA_FOSS_VERSION_MINOR           0x00000001
#define ABA_FOSS_VERSION_REVISION        0x00000003
#define ABA_FOSS_VERSION                 (ABA_FOSS_VERSION_MAJOR << 24 | ABA_FOSS_VERSION_MINOR << 16 | ABA_FOSS_VERSION_REVISION)

#define FOSS_POW_PROFILE_TABLE_LENGTH                     8
#define FOSS_LUT_UPDATE_TABLE_LENGTH                      4
#define FOSS_POWER_SAVING_TABLE_LENGTH                    4
/*=================================================================================================
Types
=================================================================================================*/

typedef enum
{
  FOSS_OPMODE_LINEAR = 0,
  FOSS_OPMODE_POWER_FIDELITY,
  FOSS_OPMODE_CONTRAST_ENHANCE_ONLY,
  FOSS_OPMODE_MAX
} FOSSOpModeType;

/* FOSSConfigParametersType
Defines generic FOSS parameters. Use when get/set-ing default/OEM parameters.
*/
typedef struct
{
  uint32_t       uBrightnessThresholdLevel;
  uint32_t       uMaxBrightnessLevel;
  uint32_t       uLuxEndPoint;
  FOSSOpModeType eOpMode;
  uint32_t       uContrastStrength;
  uint32_t       uLutUpdateDelayCount;
  bool32         bGradientDetection;
  uint32_t       uGradientThreshold;
  uint32_t       uStaticSceneThreshold;

  uint32_t*      pPowerProfileTableInput;
  uint32_t*      pPowerProfileTableOutput;
  uint32_t       uPowerProfileTableLength;
  uint32_t*      pPowerSavingTableInput;
  uint32_t*      pPowerSavingTableOutput;
  uint32_t       uPowerSavingTableLength;
  uint32_t*      pLutUpdateSpeedTableInput;
  uint32_t*      pLutUpdateSpeedTableOutput;
  uint32_t       uLutUpdateSpeedTableLength;
} FOSSConfigParametersType;

/* FOSSDynamicConfigType
Defines parameters that are dynamically modified by FOSS. Use when updating parameters.
*/
typedef struct
{
  FOSSOpModeType  eOpMode;
  uint32_t        uContrastStrength;
  uint32_t        uLutUpdateDelayCount;
  uint32_t        uStaticSceneThreshold;
  uint32_t        uPowerSaveRatioTableLength;
  uint32_t*       uPowerSaveRatioInput;
  uint32_t*       uPowerSaveRatioOutput;
  bool32          bGradientDetection;
  uint32_t        uGradientThreshold;
} FOSSDynamicConfigType;

/* FOSSProcessingInfoType
Current Foss processing info.
*/
typedef union
{
  struct
  {
    // 1st 64-bit data
    unsigned int  uLuxPoint         : 16;
    unsigned int  uAlsValue         : 16;
    unsigned int  uHistDelta        : 8;
    unsigned int  uTemporalSpeed    : 8;
    unsigned int  uReserved1        : 15;
    unsigned int  uState            : 1;
    // 2nd 64-bit data
    unsigned int  uCurPowerLevel    : 16;
    unsigned int  uOrigPowerLevel   : 16;
    unsigned int  uFrameCount       : 8;
    unsigned int  uContrastStrength : 8;
    unsigned int  uPowerSavingRatio : 8;
    unsigned int  uReserved2        : 6;
    unsigned int  uOpMode           : 2;
  } u;

  uint64_t value[2];

} FOSSProcessingInfoType;

#ifdef __cplusplus
}
#endif

#endif /* ABA_FOSS_H */
