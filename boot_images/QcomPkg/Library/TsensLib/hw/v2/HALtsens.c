/*============================================================================
  FILE:         HALTsens.c

  OVERVIEW:     Implementation of the TSENS HAL

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
  2017-01-13  jjo  Add watchdog interrupt support.
  2016-12-14  jjo  Add watchdog support.
  2015-12-04  jjo  Add new cal modes.
  2015-06-18  jjo  Add PWM enable.
  2014-07-21  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "HALhwio.h"
#include "HALhwioTsens.h"
#include "HALtsens.h"

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
   uint32 uTsensSnStatus;
   uint32 uValidBit;
   int32 nDeciDegC;

   uTsensSnStatus = HWIO_INXI(pucTsensTMAddr, TSENS_Sn_STATUS, uChannel);

   nDeciDegC = (int32)((uTsensSnStatus & HWIO_FMSK(TSENS_Sn_STATUS, LAST_TEMP)) >> HWIO_SHFT(TSENS_Sn_STATUS, LAST_TEMP));

   uValidBit = (uTsensSnStatus & HWIO_FMSK(TSENS_Sn_STATUS, VALID)) >> HWIO_SHFT(TSENS_Sn_STATUS, VALID);

   /* Temperature is a 12-bit number -- sign extend to 32 bits */
   if (nDeciDegC & 0x800)
   {
      nDeciDegC |= 0xFFFFF000;
   }

   *pnDeciDegC = nDeciDegC;

   if (uValidBit == 1)
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
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
   uint32 uTempReady;

   uTempReady = HWIO_INXF(pucTsensTMAddr, TSENS_TRDY, TRDY);

   if (uTempReady == 0)
   {
      return FALSE;
   }
   else
   {
      return TRUE;
   }
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
   uint32 uReg;
   int32 nDeciDegC;

   uReg = HWIO_INX(pucTsensTMAddr, TSENS_MAX_TEMP);

   nDeciDegC = (int32)((uReg & HWIO_FMSK(TSENS_MAX_TEMP, MAX_TEMP)) >> HWIO_SHFT(TSENS_MAX_TEMP, MAX_TEMP));

   *puSensorID = (uReg & HWIO_FMSK(TSENS_MAX_TEMP, MAX_TEMP_SENSOR_ID)) >> HWIO_SHFT(TSENS_MAX_TEMP, MAX_TEMP_SENSOR_ID);

   /* Temperature is a 12-bit number -- sign extend to 32 bits */
   if (nDeciDegC & 0x800)
   {
      nDeciDegC |= 0xFFFFF000;
   }

   *pnMaxDeciDegC = nDeciDegC;

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
   uint32 uReg;

   uReg = HWIO_INX(pucTsensTMAddr, TSENS_TM_INT_EN);

   uReg &= ~HWIO_FMSK(TSENS_TM_INT_EN, LOWER_INT_EN);
   uReg &= ~HWIO_FMSK(TSENS_TM_INT_EN, UPPER_INT_EN);

   uReg |= HWIO_FVALV(TSENS_TM_INT_EN, LOWER_INT_EN, ENABLED);
   uReg |= HWIO_FVALV(TSENS_TM_INT_EN, UPPER_INT_EN, ENABLED);

   HWIO_OUTX(pucTsensTMAddr, TSENS_TM_INT_EN, uReg);
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
   uint32 uReg;

   uReg = HWIO_INX(pucTsensTMAddr, TSENS_TM_INT_EN);

   uReg &= ~HWIO_FMSK(TSENS_TM_INT_EN, LOWER_INT_EN);
   uReg &= ~HWIO_FMSK(TSENS_TM_INT_EN, UPPER_INT_EN);

   uReg |= HWIO_FVALV(TSENS_TM_INT_EN, LOWER_INT_EN, DISABLED);
   uReg |= HWIO_FVALV(TSENS_TM_INT_EN, UPPER_INT_EN, DISABLED);

   HWIO_OUTX(pucTsensTMAddr, TSENS_TM_INT_EN, uReg);
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
   HWIO_OUTXV(pucTsensTMAddr, TSENS_TM_INT_EN, CRITICAL_INT_EN, ENABLED);
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
   HWIO_OUTXV(pucTsensTMAddr, TSENS_TM_INT_EN, CRITICAL_INT_EN, DISABLED);
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
   switch (eThreshold)
   {
      case HAL_TSENS_TH_LOWER:
         HWIO_OUTXF(pucTsensTMAddr, TSENS_UPPER_LOWER_INT_MASK, LOWER_INT_MASK, ~uChannelEnMask);
         break;
      case HAL_TSENS_TH_UPPER:
         HWIO_OUTXF(pucTsensTMAddr, TSENS_UPPER_LOWER_INT_MASK, UPPER_INT_MASK, ~uChannelEnMask);
         break;
      case HAL_TSENS_TH_CRITICAL:
         HWIO_OUTXF(pucTsensTMAddr, TSENS_CRITICAL_INT_MASK, CRITICAL_INT_MASK, ~uChannelEnMask);
         break;
      default:
         break;
   }
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
   uint32 uReg;   /* Note: clear register is write only */

   switch (eThreshold)
   {
      case HAL_TSENS_TH_LOWER:
         uReg = HWIO_FVAL(TSENS_UPPER_LOWER_INT_CLEAR, LOWER_INT_CLEAR, uChannelMask);
         HWIO_OUTX(pucTsensTMAddr, TSENS_UPPER_LOWER_INT_CLEAR, uReg);
         HWIO_OUTX(pucTsensTMAddr, TSENS_UPPER_LOWER_INT_CLEAR, 0);
         break;
      case HAL_TSENS_TH_UPPER:
         uReg = HWIO_FVAL(TSENS_UPPER_LOWER_INT_CLEAR, UPPER_INT_CLEAR, uChannelMask);
         HWIO_OUTX(pucTsensTMAddr, TSENS_UPPER_LOWER_INT_CLEAR, uReg);
         HWIO_OUTX(pucTsensTMAddr, TSENS_UPPER_LOWER_INT_CLEAR, 0);
         break;
      case HAL_TSENS_TH_CRITICAL:
         uReg = HWIO_FVAL(TSENS_CRITICAL_INT_CLEAR, CRITICAL_INT_CLEAR, uChannelMask);
         HWIO_OUTX(pucTsensTMAddr, TSENS_CRITICAL_INT_CLEAR, uReg);
         HWIO_OUTX(pucTsensTMAddr, TSENS_CRITICAL_INT_CLEAR, 0);
         break;
      default:
         break;
   }
}

/* ============================================================================
**
**  HAL_tsens_tm_SetWatchdogBarkInterruptEn
**
**  Description:
**    Enable / disable for watchdog interrupt
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**    bEnable - TRUE / FALSE
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_tm_SetWatchdogBarkInterruptEn(uint8 *pucTsensTMAddr, boolean bEnable)
{
   if (bEnable)
   {
      HWIO_OUTXF(pucTsensTMAddr, TSENS_CRITICAL_INT_MASK, WATCHDOG_BARK, 0);
   }
   else
   {
      HWIO_OUTXF(pucTsensTMAddr, TSENS_CRITICAL_INT_MASK, WATCHDOG_BARK, 1);
   }
}

/* ============================================================================
**
**  HAL_tsens_tm_ClearWatchdogBarkInterrupt
**
**  Description:
**    Clear watchdog bark interrupt
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_tm_ClearWatchdogBarkInterrupt(uint8 *pucTsensTMAddr)
{
   uint32 uReg;   /* Note: clear register is write only */

   uReg = HWIO_FVAL(TSENS_CRITICAL_INT_CLEAR, WATCHDOG_BARK, 1);
   HWIO_OUTX(pucTsensTMAddr, TSENS_CRITICAL_INT_CLEAR, uReg);
   HWIO_OUTX(pucTsensTMAddr, TSENS_CRITICAL_INT_CLEAR, 0);
}

/* ============================================================================
**
**  HAL_tsens_tm_SetCycleMonitorInterruptEn
**
**  Description:
**    Enable / disable for cycle completion interrupt
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**    bEnable - TRUE / FALSE
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_tm_SetCycleMonitorInterruptEn(uint8 *pucTsensTMAddr, boolean bEnable)
{
   if (bEnable)
   {
      HWIO_OUTXF(pucTsensTMAddr, TSENS_CRITICAL_INT_MASK, CYCLE_COMPLETION_MONITOR, 0);
   }
   else
   {
      HWIO_OUTXF(pucTsensTMAddr, TSENS_CRITICAL_INT_MASK, CYCLE_COMPLETION_MONITOR, 1);
   }
}

/* ============================================================================
**
**  HAL_tsens_tm_ClearCycleMonitorInterrupt
**
**  Description:
**    Clear cycle completion interrupt
**
**  Parameters:
**    pucTsensTMAddr - address of controller's TM region
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_tm_ClearCycleMonitorInterrupt(uint8 *pucTsensTMAddr)
{
   uint32 uReg;   /* Note: clear register is write only */

   uReg = HWIO_FVAL(TSENS_CRITICAL_INT_CLEAR, CYCLE_COMPLETION_MONITOR, 1);
   HWIO_OUTX(pucTsensTMAddr, TSENS_CRITICAL_INT_CLEAR, uReg);
   HWIO_OUTX(pucTsensTMAddr, TSENS_CRITICAL_INT_CLEAR, 0);
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
   switch (eThreshold)
   {
      case HAL_TSENS_TH_MIN:
         HWIO_OUTXVI(pucTsensSROTAddr, TSENS_Sn_MAX_MIN_STATUS_CTRL, uChannel, MIN_STATUS_MASK, NORMAL_OPERATION);
         break;
      case HAL_TSENS_TH_MAX:
         HWIO_OUTXVI(pucTsensSROTAddr, TSENS_Sn_MAX_MIN_STATUS_CTRL, uChannel, MAX_STATUS_MASK, NORMAL_OPERATION);
         break;
      default:
         break;
   }
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
   switch (eThreshold)
   {
      case HAL_TSENS_TH_MIN:
         HWIO_OUTXVI(pucTsensSROTAddr, TSENS_Sn_MAX_MIN_STATUS_CTRL, uChannel, MIN_STATUS_MASK, MASK_OFF_MIN_STATUS);
         break;
      case HAL_TSENS_TH_MAX:
         HWIO_OUTXVI(pucTsensSROTAddr, TSENS_Sn_MAX_MIN_STATUS_CTRL, uChannel, MAX_STATUS_MASK, MASK_OFF_MAX_STATUS);
         break;
      default:
         break;
   }
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
   switch (eThreshold)
   {
      case HAL_TSENS_TH_LOWER:
         HWIO_OUTXFI(pucTsensTMAddr, TSENS_Sn_UPPER_LOWER_THRESHOLD, uChannel, LOWER_THRESHOLD, nThreshold);
         break;
      case HAL_TSENS_TH_UPPER:
         HWIO_OUTXFI(pucTsensTMAddr, TSENS_Sn_UPPER_LOWER_THRESHOLD, uChannel, UPPER_THRESHOLD, nThreshold);
         break;
      case HAL_TSENS_TH_CRITICAL:
         HWIO_OUTXFI(pucTsensTMAddr, TSENS_Sn_CRITICAL_THRESHOLD, uChannel, CRITICAL_THRESHOLD, nThreshold);
         break;
      default:
         break;
   }
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
   switch (eThreshold)
   {
      case HAL_TSENS_TH_MIN:
         HWIO_OUTXFI(pucTsensSROTAddr, TSENS_Sn_MAX_MIN_STATUS_CTRL, uChannel, MIN_THRESHOLD, nThreshold);
         break;
      case HAL_TSENS_TH_MAX:
         HWIO_OUTXFI(pucTsensSROTAddr, TSENS_Sn_MAX_MIN_STATUS_CTRL, uChannel, MAX_THRESHOLD, nThreshold);
         break;
      default:
         break;
   }
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
   HWIO_OUTXF(pucTsensSROTAddr, TSENS_MEASURE_PERIOD, MAIN_MEASURE_PERIOD, uTime);
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
   HWIO_OUTXF(pucTsensSROTAddr, TSENS_MEASURE_PERIOD, POWERDOWN_MEASURE_PERIOD, uTime);
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
   if (bEnable == TRUE)
   {
      HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, AUTO_ADJUST_PERIOD_EN, ENABLED);
   }
   else
   {
      HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, AUTO_ADJUST_PERIOD_EN, DISABLED);
   }
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
   uint32 uReg;

   uReg = HWIO_INX(pucTsensSROTAddr, TSENS_CTRL);

   uReg &= ~(HWIO_FMSK(TSENS_CTRL, SENSOR0_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR1_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR2_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR3_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR4_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR5_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR6_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR7_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR8_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR9_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR10_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR11_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR12_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR13_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR14_EN) |
             HWIO_FMSK(TSENS_CTRL, SENSOR15_EN));

   uReg |= uChannelEnMask << HWIO_SHFT(TSENS_CTRL, SENSOR0_EN) &
           (HWIO_FMSK(TSENS_CTRL, SENSOR0_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR1_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR2_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR3_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR4_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR5_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR6_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR7_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR8_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR9_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR10_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR11_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR12_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR13_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR14_EN) |
            HWIO_FMSK(TSENS_CTRL, SENSOR15_EN));

   HWIO_OUTX(pucTsensSROTAddr, TSENS_CTRL , uReg);
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
   uint32 uChannelEnMask;

   uChannelEnMask = HWIO_INX(pucTsensSROTAddr, TSENS_CTRL);

   uChannelEnMask &= (HWIO_FMSK(TSENS_CTRL, SENSOR0_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR1_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR2_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR3_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR4_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR5_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR6_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR7_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR8_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR9_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR10_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR11_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR12_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR13_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR14_EN) |
                      HWIO_FMSK(TSENS_CTRL, SENSOR15_EN));

   uChannelEnMask = uChannelEnMask >> HWIO_SHFT(TSENS_CTRL, SENSOR0_EN);

   return uChannelEnMask;
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
   switch (eTsensState)
   {
      case HAL_TSENS_ENABLE:
         HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, TSENS_EN, ENABLED);
         break;
      case HAL_TSENS_DISABLE:
         HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, TSENS_EN, DISABLED);
         break;
      default:
         break;
   }
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
   HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, TSENS_SW_RST, RESET_ASSERTED);

   HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, TSENS_SW_RST, RESET_DEASSERTED);
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
   return HWIO_INX(pucTsensSROTAddr, TSENS_HW_VER);
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
   HWIO_OUTXF(pucTsensSROTAddr, TSENS_CTRL, VALID_DELAY, uNumClockCycles);
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
   if (bEnable == TRUE)
   {
      HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, PSHOLD_ARES_EN, ENABLED);
   }
   else
   {
      HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, PSHOLD_ARES_EN, DISABLED);
   }
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
   switch (eResultType)
   {
      case HAL_TSENS_RESULT_TYPE_ADC_CODE:
         HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, RESULT_FORMAT_CODE_OR_TEMP, ADC_CODE);
         break;
      case HAL_TSENS_RESULT_TYPE_DECI_DEG_C:
         HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, RESULT_FORMAT_CODE_OR_TEMP, REAL_TEMPERATURE);
         break;
      default:
         break;
   }
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
   switch (eADCClkSrc)
   {
      case HAL_TSENS_INTERNAL:
         HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, TSENS_ADC_CLK_SEL, INTERNAL_OSCILLATOR);
         break;
      case HAL_TSENS_EXTERNAL:
         HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, TSENS_ADC_CLK_SEL, EXTERNAL_CLOCK_SOURCE);
         break;
      default:
         break;
   }
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
   uint32 uReg;

   switch (uShift)
   {
      case 7:
         uShift = 0;
         break;
      case 8:
         uShift = 1;
         break;
      case 9:
         uShift = 2;
         break;
      case 10:
         uShift = 3;
         break;
      default:
         break;
   }

   uReg = HWIO_INXI(pucTsensSROTAddr, TSENS_Sn_CONVERSION, uChannel);

   uReg &= ~HWIO_FMSK(TSENS_Sn_CONVERSION, SHIFT);
   uReg &= ~HWIO_FMSK(TSENS_Sn_CONVERSION, SLOPE);
   uReg &= ~HWIO_FMSK(TSENS_Sn_CONVERSION, CZERO);

   uReg |= HWIO_FVAL(TSENS_Sn_CONVERSION, SHIFT, uShift);
   uReg |= HWIO_FVAL(TSENS_Sn_CONVERSION, SLOPE, uShiftedSlope);
   uReg |= HWIO_FVAL(TSENS_Sn_CONVERSION, CZERO, uCodeAtZero);

   HWIO_OUTXI(pucTsensSROTAddr, TSENS_Sn_CONVERSION, uChannel, uReg);
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
   HWIO_OUTXFI(pucTsensSROTAddr, TSENS_Sn_ID_ASSIGNMENT, uChannel, SENSOR_ID, uSensorID);
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
   HWIO_OUTX(pucTsensSROTAddr, TS_CONTROL, uControl);

   HWIO_OUTX(pucTsensSROTAddr, TS_CONFIG, uConfig);
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
   if (bEnable == TRUE)
   {
      HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, MAX_TEMP_PWM_EN, ENABLED);
   }
   else
   {
      HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, MAX_TEMP_PWM_EN, DISABLED);
   }
}

/* ============================================================================
**
**  HAL_tsens_srot_SetWatchdogParameters
**
**  Description:
**    Configures the watchdog
**
**  Parameters:
**    pucTsensSROTAddr - address of controller's SROT region
**    bEnable - TRUE / FALSE
**    bResetEn - TRUE / FALSE
**    uLoadVal - watchdog load value in sleep clock ticks
**
**  Return: None
**
**  Dependencies: None
**
** ========================================================================= */
void HAL_tsens_srot_SetWatchdogParameters(uint8 *pucTsensSROTAddr, boolean bEnable, boolean bResetEn, uint32 uLoadVal)
{
   uint32 uReg = HWIO_INX(pucTsensSROTAddr, TSENS_WATCHDOG_CTRL);

   uReg &= ~(HWIO_FMSK(TSENS_WATCHDOG_CTRL, WATCHDOG_EN) |
             HWIO_FMSK(TSENS_WATCHDOG_CTRL, WATCHDOG_LOAD_VAL) |
             HWIO_FMSK(TSENS_WATCHDOG_CTRL, WATCHDOG_SELF_RESET));

   if (bEnable)
   {
      uReg |= HWIO_FVAL(TSENS_WATCHDOG_CTRL, WATCHDOG_EN, 0x1);
      uReg |= HWIO_FVAL(TSENS_WATCHDOG_CTRL, WATCHDOG_LOAD_VAL, uLoadVal);
      if (bResetEn)
      {
         uReg |= HWIO_FVAL(TSENS_WATCHDOG_CTRL, WATCHDOG_SELF_RESET, 0x1);
      }
   }

   HWIO_OUTX(pucTsensSROTAddr, TSENS_WATCHDOG_CTRL , uReg);
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
   uint32 uClientEnMask;

   HWIO_OUTXFI(pucTsensSROTAddr, TSENS_TBCB_CLIENT_n_REQ, uClient, TBCB_CLIENT_REQ_SENSOR, uChannelEnMask);

   uClientEnMask = HWIO_INXF(pucTsensSROTAddr, TSENS_TBCB_CONTROL, TBCB_CLIENT_EN);

   if (uChannelEnMask == 0)
   {
      uClientEnMask &= ~(1 << uClient);
   }
   else
   {
      uClientEnMask |= 1 << uClient;
   }

   HWIO_OUTXF(pucTsensSROTAddr, TSENS_TBCB_CONTROL, TBCB_CLIENT_EN, uClientEnMask);
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
   if (bEnable == TRUE)
   {
      HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, TEMP_BROADCAST_EN, ENABLED);
   }
   else
   {
      HWIO_OUTXV(pucTsensSROTAddr, TSENS_CTRL, TEMP_BROADCAST_EN, DISABLED);
   }
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
   HWIO_OUTXF(pucTsensSROTAddr, TSENS_SIDEBAND_EN, SENSOR_EN, uChannelEnMask);
}

