/*============================================================================
  FILE:         TsensTargetLibNull.c

  OVERVIEW:     Stubbed out HAL

  DEPENDENCIES: None

                Copyright (c) 2014-2016 Qualcomm Technologies, Inc.
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
  2016-09-19  kpa  Added Tsens_Init.
  2015-06-18  jjo  Add PWM enable.
  2014-10-16  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "HALtsens.h"
#include "TsensLib.h"

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

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Get temperature measurement - TM
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_tm_GetChannelPrevTemp
**
**  Description:
**    Gets the previous TSENS temperature in deci deg C.
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**    uChannel - which of the channels
**    pnDeciDegC - TSENS temperature in deci deg C
**
**  Return: true if reading is valid, else false
**
**  Dependencies: adc result must be generated already
**
** ========================================================================= */
boolean HAL_tsens_tm_GetChannelPrevTemp(uint8 *pucTsensTMAddr, uint32 uChannel, int32 *pnDeciDegC)
{
   return FALSE;
}

/* ============================================================================
**
**  HAL_tsens_tm_TempMeasurementIsComplete
**
**  Description:
**    Finds out if the result of a given controller is ready.
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**
**  Return: TRUE: measurement is complete
**          FALSE: measurement is not complete
**
**  Dependencies: tsens is enabled
**
** ========================================================================= */
boolean HAL_tsens_tm_TempMeasurementIsComplete(uint8 *pucTsensTMAddr)
{
   return FALSE;
}

/* ============================================================================
**
**  HAL_tsens_tm_GetMaxTemp
**
**  Description:
**    Gets the maximum TSENS temperature for the last measurement period
**    in deci deg C.
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**    puSensorID - sensor ID of the channel with the maximum temperature
**    pnMaxDeciDegC - max TSENS temperature in deci deg C
**
**  Return: None
**
**  Dependencies: adc result must be generated already
**
** ========================================================================= */
void HAL_tsens_tm_GetMaxTemp(uint8 *pucTsensTMAddr, uint32 *puSensorID, int32 *pnMaxDeciDegC)
{
   return;
}

/*----------------------------------------------------------------------------
 * Interrupts - TM
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_tm_EnableUpperLowerInterrupt
**
**  Description:
**    Main enable for upper / lower thresholds
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_tm_EnableUpperLowerInterrupt(uint8 *pucTsensTMAddr)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_tm_DisableUpperLowerInterrupt
**
**  Description:
**    Main disable for upper / lower thresholds
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_tm_DisableUpperLowerInterrupt(uint8 *pucTsensTMAddr)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_tm_EnableCriticalInterrupt
**
**  Description:
**    Main enable for critical thresholds
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_tm_EnableCriticalInterrupt(uint8 *pucTsensTMAddr)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_tm_DisableCriticalInterrupt
**
**  Description:
**    Main disable for critical thresholds
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_tm_DisableCriticalInterrupt(uint8 *pucTsensTMAddr)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_tm_SetInterruptEn
**
**  Description:
**    Sets an interrupt enabled / disabled corresponding to the channel mask
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**    uChannelEnMask - channel enable mask
**    eThreshold - which threshold
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_tm_SetInterruptEn(uint8 *pucTsensTMAddr, uint32 uChannelEnMask, HAL_tsens_tm_Threshold eThreshold)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_tm_ClearInterrupt
**
**  Description:
**    Clear a threshold for a particular channel mask
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**    uChannelMask - channel mask
**    eThreshold - which threshold
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_tm_ClearInterrupt(uint8 *pucTsensTMAddr, uint32 uChannelMask, HAL_tsens_tm_Threshold eThreshold)
{
   return;
}

/*----------------------------------------------------------------------------
 * Interrupts - SROT
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_srot_EnableInterrupt
**
**  Description:
**    Enable a threshold for a particular channel
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uChannel - which channel
**    eThreshold - which threshold
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_EnableInterrupt(uint8 *pucTsensSROTAddr, uint32 uChannel, HAL_tsens_srot_Threshold eThreshold)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_DisableInterrupt
**
**  Description:
**    Disable a threshold for a particular channel
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uChannel - which channel
**    eThreshold - which threshold
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_DisableInterrupt(uint8 *pucTsensSROTAddr, uint32 uChannel, HAL_tsens_srot_Threshold eThreshold)
{
   return;
}

/*----------------------------------------------------------------------------
 * Thresholds
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_GetMaxTemp
**
**  Description:
**    Gets the maximum temperature in deci deg C
**
**  Parameters:
**    None
**
**  Return: Maximum temperature in deci deg C
**
**  Dependencies: None
**
** ========================================================================= */
int32 HAL_tsens_GetMaxTemp(void)
{
   return 0x7FF;
}

/* ============================================================================
**
**  HAL_tsens_GetMinTemp
**
**  Description:
**    Gets the minimum temperature in deci deg C
**
**  Parameters:
**    None
**
**  Return: Minimum temperature in deci deg C
**
**  Dependencies: None
**
** ========================================================================= */
int32 HAL_tsens_GetMinTemp(void)
{
   return 0xFFFFF800;
}

/*----------------------------------------------------------------------------
 * Thresholds - TM
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_tm_SetThreshold
**
**  Description:
**    Sets a given threshold
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**    uChannel - which channel
**    eThreshold - which threshold
**    nThreshold - threshold in deci deg C
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_tm_SetThreshold(uint8 *pucTsensTMAddr, uint32 uChannel, HAL_tsens_tm_Threshold eThreshold, int32 nThreshold)
{
   return;
}

/*----------------------------------------------------------------------------
 * Thresholds - SROT
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_srot_SetThreshold
**
**  Description:
**    Sets a given threshold
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uChannel - which channel
**    eThreshold - which threshold
**    nThreshold - threshold in deci deg C
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetThreshold(uint8 *pucTsensSROTAddr, uint32 uChannel, HAL_tsens_srot_Threshold eThreshold, int32 nThreshold)
{
   return;
}

/*----------------------------------------------------------------------------
 * Measurement period
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_srot_SetPeriodActive
**
**  Description:
**    Sets the period for the active case
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uTime
**     - 0x00 measures continiously
**     - 0x01-0xFE defines the length of sleep between two measurements
**                 approx equal to (upper 2bits) * 250ms + (lower 6bits) * 1.95ms
**     - 0xFF measures once
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetPeriodActive(uint8 *pucTsensSROTAddr, uint32 uTime)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_SetPeriodSleep
**
**  Description:
**    Sets the period for the sleep case
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uTime
**     - 0x00 measures continiously
**     - 0x01-0xFE defines the length of sleep between two measurements
**                 approx equal to (upper 2bits) * 250ms + (lower 6bits) * 1.95ms
**     - 0xFF measures once
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetPeriodSleep(uint8 *pucTsensSROTAddr, uint32 uTime)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_SetAutoAdjustPeriodEn
**
**  Description:
**    Sets whether or not TSENS will auto-adjust the period going in and
**    out of sleep
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    bEnable - TRUE / FALSE
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetAutoAdjustPeriodEn(uint8 *pucTsensSROTAddr, boolean bEnable)
{
   return;
}

/*----------------------------------------------------------------------------
 * Channel enable / disable
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_srot_SetChannelsEnabled
**
**  Description:
**    Enable a set of channels based on a bitmask
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uChannelEnMask - channel enable mask
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetChannelsEnabled(uint8 *pucTsensSROTAddr, uint32 uChannelEnMask)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_GetEnabledChannels
**
**  Description:
**    Gets a bitmask of all enabled TSENS channels.
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**
**  Return: bitmask of enabled channels
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_srot_GetEnabledChannels(uint8 *pucTsensSROTAddr)
{
   return 0;
}

/*----------------------------------------------------------------------------
 * Main enable / disable
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_srot_SetState
**
**  Description:
**    Enable or disable tsens
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    eTsensState - HAL_TSENS_ENABLE/HAL_TSENS_DISABLE
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetState(uint8 *pucTsensSROTAddr, HAL_tsens_srot_State eTsensState)
{
   return;
}

/*----------------------------------------------------------------------------
 * Software reset
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_srot_Reset
**
**  Description:
**    Resets TSENS.
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_Reset(uint8 *pucTsensSROTAddr)
{
   return;
}

/*----------------------------------------------------------------------------
 * Config
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_srot_GetControllerVersion
**
**  Description:
**    Gets the TSENS controller version
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**
**  Return: TSENS controller version
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_srot_GetControllerVersion(uint8 *pucTsensSROTAddr)
{
   return 0;
}

/* ============================================================================
**
**  HAL_tsens_srot_SetValidBitDelay
**
**  Description:
**    Sets the valid bit delay
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uNumClockCycles - number of clock cycles
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetValidBitDelay(uint8 *pucTsensSROTAddr, uint32 uNumClockCycles)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_SetPSHoldResetEn
**
**  Description:
**    Enables / disables PS_HOLD reset functionality
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    bEnable - TRUE / FALSE
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetPSHoldResetEn(uint8 *pucTsensSROTAddr, boolean bEnable)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_SetResultFormat
**
**  Description:
**    Sets whether the result will be in code or real temperature
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    eResultType - whether the result will be in code or real temperature
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetResultFormat(uint8 *pucTsensSROTAddr, HAL_tsens_srot_ResultType eResultType)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_SelectADCClkSrc
**
**  Description:
**    Sets a given threshold
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    eADCClkSrc - internal clk or external clk
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SelectADCClkSrc(uint8 *pucTsensSROTAddr, HAL_tsens_srot_ADCClkSrc eADCClkSrc)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_SetConversionFactors
**
**  Description:
**    Sets up the parameters needed to convert from code to deci deg C.
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uChannel - which channel
**    uShift - shift value of N = 7, 8, 9 or 10
**    uShiftedSlope - slope << N, i.e. (900 << N) / (C120 - C30)
**    uCodeAtZero - ADC code at zero degrees C
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetConversionFactors(uint8 *pucTsensSROTAddr, uint32 uChannel, uint32 uShift, uint32 uShiftedSlope, uint32 uCodeAtZero)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_SetSensorID
**
**  Description:
**    Sets the sensor ID for each channel
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uSensorID - the sensor ID
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetSensorID(uint8 *pucTsensSROTAddr, uint32 uChannel, uint32 uSensorID)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_SetAnalogParameters
**
**  Description:
**    Sets the analog TSENS parameters
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uControl - control register value
**    uConfig - config register value
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetAnalogParameters(uint8 *pucTsensSROTAddr, uint32 uControl, uint32 uConfig)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_SetPWMEn
**
**  Description:
**    Enables / disables PWM functionality
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    bEnable - TRUE / FALSE
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetPWMEn(uint8 *pucTsensSROTAddr, boolean bEnable)
{
   return;
}

/*----------------------------------------------------------------------------
 * LMh
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_srot_SetTempBroadcastClientEnabled
**
**  Description:
**    Sets up the channels enabled for temperature broadcast bus clients
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uClient - which client
**    uChannelEnMask - bitmask of channels to enable
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetTempBroadcastClientEnabled(uint8 *pucTsensSROTAddr, uint32 uClient, uint32 uChannelEnMask)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_SetTempBroadcastEn
**
**  Description:
**    Sets the temperature broadcast bus enabled
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    bEnable - TRUE / FALSE
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetTempBroadcastEn(uint8 *pucTsensSROTAddr, boolean bEnable)
{
   return;
}

/* ============================================================================
**
**  HAL_tsens_srot_SetSidebandChannelsEnabled
**
**  Description:
**    Sets the sideband channels enabled
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    uChannelEnMask - bitmask of channels to enable
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetSidebandChannelsEnabled(uint8 *pucTsensSROTAddr, uint32 uChannelEnMask)
{
   return;
}

/*----------------------------------------------------------------------------
 * Char data
 * -------------------------------------------------------------------------*/
/* ============================================================================
**
**  HAL_tsens_UseRedundant
**
**  Description:
**    Whether or not to use the redundant region
**
**  Parameters:
**    None
**
**  Return: TRUE: use redundant char data
**
**  Dependencies: None
**
** ========================================================================= */
boolean HAL_tsens_UseRedundant(void)
{
   return FALSE;
}

/* ============================================================================
**
**  HAL_tsens_CalSelect
**
**  Description:
**    Determines whether there is 1) no cal data 2) single point cal data or
**    3) two point cal data
**
**  Parameters:
**    bUseRedundant - whether or not to use the redundant location
**
**  Return: HAL_tsens_Calibration (not calibrated, one point, or two point)
**
**  Dependencies: None
**
** ========================================================================= */
HAL_tsens_Calibration HAL_tsens_CalSelect(boolean bUseRedundant)
{
   return HAL_TSENS_CAL_NOT_CALIBRATED;
}

/* ============================================================================
**
**  HAL_tsens_PartialGoodsEnabled
**
**  Description:
**    Determines whether or not partial goods encoding has been enabled
**
**  Parameters:
**    bUseRedundant - whether or not to use the redundant location
**
**  Return: TRUE / FALSE
**
**  Dependencies: None
**
** ========================================================================= */
boolean HAL_tsens_PartialGoodsEnabled(boolean bUseRedundant)
{
   return FALSE;
}

/* ============================================================================
**
**  HAL_tsens_GetBase1
**
**  Description:
**    Gets the Tsens base for cal point 1
**
**  Parameters:
**    bUseRedundant - whether or not to use the redundant location
**    uController - which controller
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetBase1(boolean bUseRedundant, uint32 uController)
{
   return 0;
}

/* ============================================================================
**
**  HAL_tsens_GetBase2
**
**  Description:
**    Gets the Tsens base for cal point 2
**
**  Parameters:
**    bUseRedundant - whether or not to use the redundant location
**    uController - which controller
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetBase2(boolean bUseRedundant, uint32 uController)
{
   return 0;
}

/* ============================================================================
**
**  HAL_tsens_GetOffset1
**
**  Description:
**    Gets Tsens uSensor offset for cal point 1
**
**  Parameters:
**    bUseRedundant - whether or not to use the redundant location
**    uint32 uFuseIdx
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetOffset1(boolean bUseRedundant, uint32 uFuseIdx)
{
   return 0;
}

/* ============================================================================
**
**  HAL_tsens_GetOffset2
**
**  Description:
**    Gets Tsens uSensor offset for cal point 2
**
**  Parameters:
**    bUseRedundant - whether or not to use the redundant location
**    uint32 uFuseIdx
**
**  Return: adc code
**
**  Dependencies: None
**
** ========================================================================= */
uint32 HAL_tsens_GetOffset2(boolean bUseRedundant, uint32 uFuseIdx)
{
   return 0;
}

/* ============================================================================

   @brief Initializes TSENS library.

   Initialize the TSENS library for loader.

   @return TSENS_SUCCESS, else an error.
** ========================================================================= */

TsensResultType Tsens_Init(void)
{
  return TSENS_SUCCESS;
}   