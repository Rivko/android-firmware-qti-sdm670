#ifndef __HAL_TSENS_H__
#define __HAL_TSENS_H__
/*============================================================================
  @file HALtsens.h

  This is the internal hardware abstraction layer for Tsens

                Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Confidential and Proprietary.
============================================================================*/
/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "com_dtypes.h"     /* Definitions for byte, word, etc.     */

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
typedef enum
{
   HAL_TSENS_TH_LOWER,
   HAL_TSENS_TH_UPPER,
   HAL_TSENS_TH_CRITICAL,
} HAL_tsens_tm_Threshold;

typedef enum
{
   HAL_TSENS_TH_MIN,
   HAL_TSENS_TH_MAX,
} HAL_tsens_srot_Threshold;

typedef enum
{
   HAL_TSENS_INTERNAL,
   HAL_TSENS_EXTERNAL,
} HAL_tsens_srot_ADCClkSrc;

typedef enum
{
   HAL_TSENS_DISABLE,
   HAL_TSENS_ENABLE
} HAL_tsens_srot_State;

typedef enum
{
   HAL_TSENS_RESULT_TYPE_ADC_CODE,
   HAL_TSENS_RESULT_TYPE_DECI_DEG_C,
} HAL_tsens_srot_ResultType;

typedef enum
{
   HAL_TSENS_CAL_NOT_CALIBRATED = 0,
   HAL_TSENS_CAL_ONE_POINT      = 2,
   HAL_TSENS_CAL_TWO_POINT      = 3,
   HAL_TSENS_CAL_TWO_POINT_3_TRUNCATED = 4,
   HAL_TSENS_CAL_TWO_POINT_1_TRUNCATED = 6,
} HAL_tsens_Calibration;

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Get temperature measurement - TM
 * -------------------------------------------------------------------------*/
boolean HAL_tsens_tm_GetChannelPrevTemp(uint8 *pucTsensTMAddr, uint32 uChannel, int32 *pnDeciDegC);
boolean HAL_tsens_tm_TempMeasurementIsComplete(uint8 *pucTsensTMAddr);
void HAL_tsens_tm_GetMaxTemp(uint8 *pucTsensTMAddr, uint32 *puSensorID, int32 *pnMaxDeciDegC);

/*----------------------------------------------------------------------------
 * Interrupts - TM
 * -------------------------------------------------------------------------*/
void HAL_tsens_tm_EnableUpperLowerInterrupt(uint8 *pucTsensTMAddr);
void HAL_tsens_tm_DisableUpperLowerInterrupt(uint8 *pucTsensTMAddr);
void HAL_tsens_tm_EnableCriticalInterrupt(uint8 *pucTsensTMAddr);
void HAL_tsens_tm_DisableCriticalInterrupt(uint8 *pucTsensTMAddr);
void HAL_tsens_tm_SetInterruptEn(uint8 *pucTsensTMAddr, uint32 uChannelEnMask, HAL_tsens_tm_Threshold eThreshold);
void HAL_tsens_tm_ClearInterrupt(uint8 *pucTsensTMAddr, uint32 uChannelMask, HAL_tsens_tm_Threshold eThreshold);
void HAL_tsens_tm_SetWatchdogBarkInterruptEn(uint8 *pucTsensTMAddr, boolean bEnable);
void HAL_tsens_tm_ClearWatchdogBarkInterrupt(uint8 *pucTsensTMAddr);
void HAL_tsens_tm_SetCycleMonitorInterruptEn(uint8 *pucTsensTMAddr, boolean bEnable);
void HAL_tsens_tm_ClearCycleMonitorInterrupt(uint8 *pucTsensTMAddr);

/*----------------------------------------------------------------------------
 * Interrupts - SROT
 * -------------------------------------------------------------------------*/
void HAL_tsens_srot_EnableInterrupt(uint8 *pucTsensSROTAddr, uint32 uChannel, HAL_tsens_srot_Threshold eThreshold);
void HAL_tsens_srot_DisableInterrupt(uint8 *pucTsensSROTAddr, uint32 uChannel, HAL_tsens_srot_Threshold eThreshold);

/*----------------------------------------------------------------------------
 * Thresholds
 * -------------------------------------------------------------------------*/
int32 HAL_tsens_GetMaxTemp(void);
int32 HAL_tsens_GetMinTemp(void);

/*----------------------------------------------------------------------------
 * Thresholds - TM
 * -------------------------------------------------------------------------*/
void HAL_tsens_tm_SetThreshold(uint8 *pucTsensTMAddr, uint32 uChannel, HAL_tsens_tm_Threshold eThreshold, int32 nThreshold);

/*----------------------------------------------------------------------------
 * Thresholds - SROT
 * -------------------------------------------------------------------------*/
void HAL_tsens_srot_SetThreshold(uint8 *pucTsensSROTAddr, uint32 uChannel, HAL_tsens_srot_Threshold eThreshold, int32 nThreshold);

/*----------------------------------------------------------------------------
 * Measurement period - SROT
 * -------------------------------------------------------------------------*/
void HAL_tsens_srot_SetPeriodActive(uint8 *pucTsensSROTAddr, uint32 uTime);
void HAL_tsens_srot_SetPeriodSleep(uint8 *pucTsensSROTAddr, uint32 uTime);
void HAL_tsens_srot_SetAutoAdjustPeriodEn(uint8 *pucTsensSROTAddr, boolean bEnable);

/*----------------------------------------------------------------------------
 * Channel enable / disable - SROT
 * -------------------------------------------------------------------------*/
void HAL_tsens_srot_SetChannelsEnabled(uint8 *pucTsensSROTAddr, uint32 uChannelEnMask);
uint32 HAL_tsens_srot_GetEnabledChannels(uint8 *pucTsensSROTAddr);

/*----------------------------------------------------------------------------
 * Main enable / disable - SROT
 * -------------------------------------------------------------------------*/
void HAL_tsens_srot_SetState(uint8 *pucTsensSROTAddr, HAL_tsens_srot_State eTsensState);

/*----------------------------------------------------------------------------
 * Software reset - SROT
 * -------------------------------------------------------------------------*/
void HAL_tsens_srot_Reset(uint8 *pucTsensSROTAddr);

/*----------------------------------------------------------------------------
 * Config - SROT
 * -------------------------------------------------------------------------*/
uint32 HAL_tsens_srot_GetControllerVersion(uint8 *pucTsensSROTAddr);
void HAL_tsens_srot_SetValidBitDelay(uint8 *pucTsensSROTAddr, uint32 uNumClockCycles);
void HAL_tsens_srot_SetPSHoldResetEn(uint8 *pucTsensSROTAddr, boolean bEnable);
void HAL_tsens_srot_SetResultFormat(uint8 *pucTsensSROTAddr, HAL_tsens_srot_ResultType eResultType);
void HAL_tsens_srot_SelectADCClkSrc(uint8 *pucTsensSROTAddr, HAL_tsens_srot_ADCClkSrc eADCClkSrc);
void HAL_tsens_srot_SetConversionFactors(uint8 *pucTsensSROTAddr, uint32 uChannel, uint32 uShift, uint32 uShiftedSlope, uint32 uCodeAtZero);
void HAL_tsens_srot_SetSensorID(uint8 *pucTsensSROTAddr, uint32 uChannel, uint32 uSensorID);
void HAL_tsens_srot_SetAnalogParameters(uint8 *pucTsensSROTAddr, uint32 uControl, uint32 uConfig);
void HAL_tsens_srot_SetPWMEn(uint8 *pucTsensSROTAddr, boolean bEnable);
void HAL_tsens_srot_SetWatchdogParameters(uint8 *pucTsensSROTAddr, boolean bEnable, boolean bResetEn, uint32 uLoadVal);

/*----------------------------------------------------------------------------
 * LMh - SROT
 * -------------------------------------------------------------------------*/
void HAL_tsens_srot_SetTempBroadcastClientEnabled(uint8 *pucTsensSROTAddr, uint32 uClient, uint32 uChannelEnMask);
void HAL_tsens_srot_SetTempBroadcastEn(uint8 *pucTsensSROTAddr, boolean bEnable);
void HAL_tsens_srot_SetSidebandChannelsEnabled(uint8 *pucTsensSROTAddr, uint32 uChannelEnMask);

/*----------------------------------------------------------------------------
 * Char data
 * -------------------------------------------------------------------------*/
boolean HAL_tsens_UseRedundant(void);
HAL_tsens_Calibration HAL_tsens_CalSelect(boolean bUseRedundant);
boolean HAL_tsens_PartialGoodsEnabled(boolean bUseRedundant);
uint32 HAL_tsens_GetBase1(boolean bUseRedundant, uint32 uController);
uint32 HAL_tsens_GetBase2(boolean bUseRedundant, uint32 uController);
uint32 HAL_tsens_GetOffset1(boolean bUseRedundant, uint32 uFuseIdx);
uint32 HAL_tsens_GetOffset2(boolean bUseRedundant, uint32 uFuseIdx);

#endif /* #ifndef __HAL_TSENS_H__ */

