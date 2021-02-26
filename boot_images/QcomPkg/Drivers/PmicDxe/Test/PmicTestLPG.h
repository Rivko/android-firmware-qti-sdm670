#ifndef __PMICTESTLPG_H__
#define __PMICTESTLPG_H__

/*! @file PMICTESTLPG.h
 *
 *  PMIC Test LPG
 *
 *  Copyright (c) 2015-2016 Qualcomm Technologies, Inc.  All Rights Reserved. 
 *  Qualcomm Technologies Proprietary and Confidential.
 */

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ---------------------------------------------------------- 
10/15/15   RM      Added Pmic Test LPG Header file

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include <Protocol/EFIPmicLpg.h>

/*===========================================================================
                     MACRO DEFINATIONS
===========================================================================*/


/*===========================================================================
                     TYPE DECLARATIONS
===========================================================================*/

typedef enum
{
    EFI_PM_TEST_LPG_PATTERN_CONFIG,
    EFI_PM_TEST_LPG_PWN_CLK_SELC,
    EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_ENABLE, 
    EFI_PM_TEST_LPG_PWM_LUT_INDEX_SET,
    EFI_PM_TEST_LPG_SET_PWM_BIT_SIZE,
    EFI_PM_TEST_LPG_PWM_SRC_SELECT,
    EFI_PM_TEST_LPG_LUT_CONFIG_SET_ARRAY,
    EFI_PM_TEST_LPG_LUT_CONFIG_GET_ARRAY,
    EFI_PM_TEST_LPG_LUT_CONFIG_SET,
    EFI_PM_TEST_LPG_LUT_CONFIG_GET,
    EFI_PM_TEST_LPG_CONFIG_PAUSE_TIME,
    EFI_PM_TEST_LPG_PWM_OUTPUT_ENABLE,
    EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_START
}EFI_PM_TEST_DRIVER_LPG_TYPE_FUNCTION;

typedef struct _EFI_PM_TEST_LPG_PATTERN_CONFIG_PARAM
{
    UINT32                  PmicChip;
    EFI_PM_LPG_CHAN_TYPE    ChanType;
    boolean                 RampDir;
    boolean                 PatternRepeat;
    boolean                 RampToggle;
    boolean                 EnPauseHigh;
    boolean                 EnPauseLow;
} EFI_PM_TEST_LPG_PATTERN_CONFIG_PARAM;

typedef struct _EFI_PM_TEST_LPG_PWN_CLK_SELC_PARAM
{
    UINT32                  PmicChip;
    EFI_PM_LPG_CHAN_TYPE    ChanType;
    EFI_PM_LPG_PWM_CLOCK_TYPE ClkType;
} EFI_PM_TEST_LPG_PWN_CLK_SELC_PARAM;

typedef struct _EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_ENABLE_PARAM
{
    UINT32                  PmicChip;
    EFI_PM_LPG_CHAN_TYPE    ChanType;
    boolean                 Enable;
} EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_ENABLE_PARAM;

typedef struct _EFI_PM_TEST_LPG_PWM_LUT_INDEX_SET_PARAM
{
    UINT32                  PmicChip;
    EFI_PM_LPG_CHAN_TYPE    ChanType;
    UINT32                  LowIndex;
    UINT32                  HighIndex;
} EFI_PM_TEST_LPG_PWM_LUT_INDEX_SET_PARAM;

typedef struct _EFI_PM_TEST_LPG_SET_PWM_BIT_SIZE_PARAM
{
    UINT32                  PmicChip;
    EFI_PM_LPG_CHAN_TYPE    ChanType;
    EFI_PM_LPG_PWM_BIT_SIZE_TYPE PwmSizeType;
} EFI_PM_TEST_LPG_SET_PWM_BIT_SIZE_PARAM;

typedef struct _EFI_PM_TEST_LPG_PWM_SRC_SELECT_PARAM
{
    UINT32                  PmicChip;
    EFI_PM_LPG_CHAN_TYPE    ChanType;
    EFI_PM_LPG_SRC_TYPE     SrcType;
} EFI_PM_TEST_LPG_PWM_SRC_SELECT_PARAM;

typedef struct _EFI_PM_TEST_LPG_LUT_CONFIG_SET_ARRAY_PARAM
{
    UINT32      PmicChip;
    UINT32      StartIndex;
    UINT32      *Value;
    UINT32      Count;
} EFI_PM_TEST_LPG_LUT_CONFIG_SET_ARRAY_PARAM, EFI_PM_TEST_LPG_LUT_CONFIG_GET_ARRAY_PARAM;

typedef struct _EFI_PM_TEST_LPG_LUT_CONFIG_SET_PARAM
{
    UINT32      PmicChip;
    UINT32      LutIndex;
    UINT32      PwmValue;
} EFI_PM_TEST_LPG_LUT_CONFIG_PARAM;

typedef struct _EFI_PM_TEST_LPG_CONFIG_PAUSE_TIME_PARAM
{
    UINT32                  PmicChip;
    EFI_PM_LPG_CHAN_TYPE    ChanType;
    UINT32                  Ramp_Step_Duration;
    UINT32                  Hi_Multiplier;
    UINT32                  Low_Multiplier;
} EFI_PM_TEST_LPG_CONFIG_PAUSE_TIME_PARAM;

typedef struct _EFI_PM_TEST_LPG_PWM_OUTPUT_ENABLE_PARAM
{
   UINT32                PmicChip;
   EFI_PM_LPG_CHAN_TYPE  ChanType;
   BOOLEAN               OutputEnable;
} EFI_PM_TEST_LPG_PWM_OUTPUT_ENABLE_PARAM;

typedef struct _EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_START_PARAM
{
   UINT32                PmicChip;
   EFI_PM_LPG_CHAN_TYPE  ChanType;
} EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_START_PARAM;


typedef union _EFI_PM_TEST_LPG_FUNCTION_TYPE
{
    EFI_PM_TEST_LPG_PATTERN_CONFIG_PARAM                LpgPatternConfig;
    EFI_PM_TEST_LPG_PWN_CLK_SELC_PARAM                  LpgPwmClkSel; 
    EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_ENABLE_PARAM     LpgPwmRampGeneratorEnableParam;
    EFI_PM_TEST_LPG_PWM_LUT_INDEX_SET_PARAM             LpgPwmLutIndexSet;
    EFI_PM_TEST_LPG_SET_PWM_BIT_SIZE_PARAM              LpgSetPwmBitsize;
    EFI_PM_TEST_LPG_PWM_SRC_SELECT_PARAM                LpgPwmSrcSelect;
    EFI_PM_TEST_LPG_LUT_CONFIG_SET_ARRAY_PARAM          LpgLutConfigSetArray;
    EFI_PM_TEST_LPG_LUT_CONFIG_GET_ARRAY_PARAM          LpgLutConfigGetArray;
    EFI_PM_TEST_LPG_LUT_CONFIG_PARAM                    LpgLutConfig;
    EFI_PM_TEST_LPG_CONFIG_PAUSE_TIME_PARAM             LpgConfigPauseTime;
    EFI_PM_TEST_LPG_PWM_OUTPUT_ENABLE_PARAM				LpgPwmOutputEnable;
    EFI_PM_TEST_LPG_PWM_RAMP_GENERATOR_START_PARAM		LpgPwmRampGeneratorStart;

}EFI_PM_TEST_LPG_FUNCTION_TYPE;

/*===========================================================================
                       FUNCTION PROTOTYPES
===========================================================================*/



#endif // __PMICTESTLPG_H__


