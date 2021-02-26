/*============================================================================
  FILE:         TsensLoader.c

  OVERVIEW:     Implementation of TSENS supporting XBL loader.

  DEPENDENCIES: None

                Copyright (c) 2014-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2017-01-13  jjo  Mask watchdog interrupts.
  2016-12-14  jjo  Add watchdog support.
  2016-04-12  jjo  Add support for truncated offsets.
  2016-03-08  jjo  Memory enhancements.
  2016-03-03  jjo  Add TBCB mapping.
  2015-06-18  jjo  Add PWM enable.
  2014-07-21  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "TsensLib.h"
#include "HALtsens.h"
#include "TsensBsp.h"
#include "DALSys.h"
#include "DDIChipInfo.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/
typedef struct
{
   DALBOOL bChannelIsDead;
   uint32 uCodeAtZero;
   uint32 uShiftedSlope;
} TsensChannelType;

typedef struct
{
   TsensChannelType aChannels[TSENS_MAX_NUM_CHANNELS_PER_CONTROLLER];
   uint8 *pucTsensSROTAddr;
   uint8 *pucTsensTMAddr;
   uint32 uVersion;
   uint32 uNumChannelsEnabled;
   uint32 uChannelEnableMask;
   uint32 uLowerEnableMask;
   uint32 uUpperEnableMask;
   uint32 uCriticalEnableMask;
   DALBOOL bBroadcastEnabled;
   int32 nBase1;
   int32 nBase2;
   int32 nShiftedSlope;
} TsensControllerType;

typedef struct
{
   const TsensBspType *pTsensBsp;
   TsensControllerType *paControllers;
   int32 nLastTempDeciDegC;
   uint32 uLastSensor;
   int32 nThresholdMax;
   int32 nThresholdMin;
} TsensDevCtxtType;

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
TsensDevCtxtType gTsensDevCtxt = {0};
DALBOOL gbTsensInitialized = FALSE;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static uint8* Tsens_MapHWIORegion(uint8 *puPhysical, uint32 uSize)
{
   return puPhysical;
}

static int32 Tsens_DivideWithRounding(int32 nNum, int32 nDen)
{
   int32 nOffset = nDen / 2;

   if (nNum < 0)
   {
      nOffset *= -1;
   }

   return (nNum + nOffset) / nDen;
}

static int32 Tsens_CheckThreshold(int32 nThreshold)
{
   if (nThreshold > gTsensDevCtxt.nThresholdMax)
   {
      return gTsensDevCtxt.nThresholdMax;
   }
   else if (nThreshold < gTsensDevCtxt.nThresholdMin)
   {
      return gTsensDevCtxt.nThresholdMin;
   }
   else
   {
      return nThreshold;
   }
}

static TsensResultType Tsens_WaitForFirstRead(void)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   const TsensControllerType *paControllers = gTsensDevCtxt.paControllers;
   uint32 uMaxNumChannelsEnabled = 0;
   uint32 uNumChannelsEnabled;
   uint32 uController;
   uint32 uWaitTime_us;

   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      uNumChannelsEnabled = paControllers[uController].uNumChannelsEnabled;

      if (uNumChannelsEnabled > uMaxNumChannelsEnabled)
      {
         uMaxNumChannelsEnabled = uNumChannelsEnabled;
      }
   }

   uWaitTime_us = pBsp->uSensorConvTime_us * uMaxNumChannelsEnabled;

   DALSYS_BusyWait(uWaitTime_us);

   return TSENS_SUCCESS;
}

static TsensResultType Tsens_GetCalibration(void)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   const int32 nCalPoint1DeciDegC = pBsp->nCalPoint1DeciDegC;
   const int32 nCalPoint2DeciDegC = pBsp->nCalPoint2DeciDegC;
   const int32 nDeltaDeciDegC = nCalPoint2DeciDegC - nCalPoint1DeciDegC;
   const int32 nShiftedSlopeNum = nDeltaDeciDegC << pBsp->uShift;
   const TsensSensorCfgType *pSensorCfg;
   HAL_tsens_Calibration eSensorCalType = HAL_TSENS_CAL_NOT_CALIBRATED;
   TsensControllerType *paControllers = gTsensDevCtxt.paControllers;
   TsensControllerType *pController;
   HAL_tsens_Calibration eCalType;
   boolean bUseRedundant;
   boolean bPartialGoods;
   TsensChannelType *pChannel;
   uint32 uSensor;
   uint32 uController;
   uint32 uChannel;
   int32 nCode1;
   int32 nNum;
   int32 nDen;

   bUseRedundant = HAL_tsens_UseRedundant();

   eCalType = HAL_tsens_CalSelect(bUseRedundant);
   bPartialGoods = HAL_tsens_PartialGoodsEnabled(bUseRedundant);

   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      pController = &paControllers[uController];
      pController->uChannelEnableMask = 0;

      /* Get the base points as applicable */
      switch (eCalType)
      {
         case HAL_TSENS_CAL_ONE_POINT:
            pController->nBase1 = (int32)HAL_tsens_GetBase1(bUseRedundant, uController);
            break;

         case HAL_TSENS_CAL_TWO_POINT:
         case HAL_TSENS_CAL_TWO_POINT_1_TRUNCATED:
         case HAL_TSENS_CAL_TWO_POINT_3_TRUNCATED:
            pController->nBase1 = (int32)HAL_tsens_GetBase1(bUseRedundant, uController);
            pController->nBase2 = (int32)HAL_tsens_GetBase2(bUseRedundant, uController);

            // uShiftedSlope = ((T2 - T1) << N) / (C2 - C1)
            nDen = pController->nBase2 - pController->nBase1;
            pController->nShiftedSlope = Tsens_DivideWithRounding(nShiftedSlopeNum, nDen);
            break;

         default:
            break;
      }
   }

   /* Get the cal data per sensor */
   for (uSensor = 0; uSensor < pBsp->uNumSensors; uSensor++)
   {
      pSensorCfg = &pBsp->paSensorCfgs[uSensor];
      uController = pSensorCfg->ucController;
      uChannel = pSensorCfg->ucChannel;
      pController = &paControllers[uController];
      pChannel = &pController->aChannels[uChannel];
      pChannel->bChannelIsDead = FALSE;

      if (bPartialGoods)
      {
         if (0xf == HAL_tsens_GetOffset1(bUseRedundant, pSensorCfg->ucFuseIdx))
         {
            pChannel->bChannelIsDead = TRUE;
            continue;
         }
      }

      /* Check to see if default calibration is being forced */
      switch (pSensorCfg->eCal)
      {
         case TSENS_BSP_SENSOR_CAL_NORMAL:
            /* Use the normal method */
            eSensorCalType = eCalType;
            break;

         case TSENS_BSP_SENSOR_CAL_IGNORE_DEVICE_CAL:
            /* Force default char data */
            eSensorCalType = HAL_TSENS_CAL_NOT_CALIBRATED;
            break;

         default:
            return TSENS_ERROR;
      }

      switch (eSensorCalType)
      {
         case HAL_TSENS_CAL_ONE_POINT:
            nCode1 = pController->nBase1 + (int32)HAL_tsens_GetOffset1(bUseRedundant, pSensorCfg->ucFuseIdx);

            // uCodeAtZero = C1 - T1 * slope
            nNum = nCalPoint1DeciDegC * (pSensorCfg->nsCalPoint2CodeDefault - pSensorCfg->nsCalPoint1CodeDefault);
            pChannel->uCodeAtZero = (uint32)(nCode1 - Tsens_DivideWithRounding(nNum, nDeltaDeciDegC));

            // uShiftedSlope = ((T2 - T1) << N) / (C2 - C1)
            nDen = pSensorCfg->nsCalPoint2CodeDefault - pSensorCfg->nsCalPoint1CodeDefault;
            pChannel->uShiftedSlope = (uint32)Tsens_DivideWithRounding(nShiftedSlopeNum, nDen);
            break;

         case HAL_TSENS_CAL_TWO_POINT:
         case HAL_TSENS_CAL_TWO_POINT_1_TRUNCATED:
         case HAL_TSENS_CAL_TWO_POINT_3_TRUNCATED:
            nCode1 = (int32)HAL_tsens_GetOffset1(bUseRedundant, pSensorCfg->ucFuseIdx);
            if (eSensorCalType == HAL_TSENS_CAL_TWO_POINT_1_TRUNCATED)
            {
               nCode1 *= 2;
            }
            else if (eSensorCalType == HAL_TSENS_CAL_TWO_POINT_3_TRUNCATED)
            {
               nCode1 *= 8;
            }
            nCode1 += pController->nBase1;

            // uCodeAtZero = C1 - T1 * slope
            nNum = nCalPoint1DeciDegC * (pController->nBase2 - pController->nBase1);
            pChannel->uCodeAtZero = (uint32)(nCode1 - Tsens_DivideWithRounding(nNum, nDeltaDeciDegC));

            // uShiftedSlope
            pChannel->uShiftedSlope = pController->nShiftedSlope;
            break;

         default:
            // uCodeAtZero = ((T1 * (C1 - C2)) / (T2 - T1)) + C1
            nNum = (pSensorCfg->nsCalPoint1CodeDefault - pSensorCfg->nsCalPoint2CodeDefault) * nCalPoint1DeciDegC;
            pChannel->uCodeAtZero = (uint32)(Tsens_DivideWithRounding(nNum, nDeltaDeciDegC) + pSensorCfg->nsCalPoint1CodeDefault);

            // uShiftedSlope = ((T2 - T1) << N) / (C2 - C1)
            nDen = pSensorCfg->nsCalPoint2CodeDefault - pSensorCfg->nsCalPoint1CodeDefault;
            pChannel->uShiftedSlope = (uint32)Tsens_DivideWithRounding(nShiftedSlopeNum, nDen);
            break;
      }

      pController->uChannelEnableMask |= 1 << uChannel;
      pController->uNumChannelsEnabled++;
   }

   return TSENS_SUCCESS;
}

static TsensResultType Tsens_EnableControllers(void)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   const TsensControllerType *paControllers = gTsensDevCtxt.paControllers;
   uint32 uController;

   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      if (paControllers[uController].uChannelEnableMask != 0)
      {
         HAL_tsens_srot_SetState(paControllers[uController].pucTsensSROTAddr, HAL_TSENS_ENABLE);
      }
   }

   return TSENS_SUCCESS;
}

static TsensResultType Tsens_DisableControllers(void)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   const TsensControllerType *paControllers = gTsensDevCtxt.paControllers;
   uint32 uController;

   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      HAL_tsens_srot_SetState(paControllers[uController].pucTsensSROTAddr, HAL_TSENS_DISABLE);
   }

   return TSENS_SUCCESS;
}

static TsensResultType Tsens_ConfigureControllers(void)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   const TsensControllerCfgType *pControllerCfg;
   TsensControllerType *pController;
   uint8 *pucTsensSROTAddr;
   uint8 *pucTsensTMAddr;
   uint32 uController;
   uint32 uSidebandEnMask;
   uint32 uTbcbClient;

   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      pControllerCfg = &pBsp->paControllerCfgs[uController];
      pController = &gTsensDevCtxt.paControllers[uController];
      pucTsensSROTAddr = pController->pucTsensSROTAddr;
      pucTsensTMAddr = pController->pucTsensTMAddr;

      pController->bBroadcastEnabled = FALSE;

      if (pController->uChannelEnableMask != 0)
      {
         HAL_tsens_srot_Reset(pucTsensSROTAddr);
         HAL_tsens_srot_SetPSHoldResetEn(pucTsensSROTAddr, pControllerCfg->bPSHoldResetEn);
         HAL_tsens_srot_SetResultFormat(pucTsensSROTAddr, HAL_TSENS_RESULT_TYPE_DECI_DEG_C);
         HAL_tsens_srot_SetPeriodActive(pucTsensSROTAddr, pControllerCfg->uPeriodActive);
         HAL_tsens_srot_SetPeriodSleep(pucTsensSROTAddr, pControllerCfg->uPeriodSleep);
         HAL_tsens_srot_SetAutoAdjustPeriodEn(pucTsensSROTAddr, pControllerCfg->bAutoAdjustPeriod);
         HAL_tsens_srot_SelectADCClkSrc(pucTsensSROTAddr, HAL_TSENS_INTERNAL);
         HAL_tsens_srot_SetChannelsEnabled(pucTsensSROTAddr, pController->uChannelEnableMask);
         HAL_tsens_srot_SetAnalogParameters(pucTsensSROTAddr, pControllerCfg->uTSControl, pControllerCfg->uTSConfig);
         HAL_tsens_srot_SetPWMEn(pucTsensSROTAddr, pControllerCfg->bPWMEn);

         uSidebandEnMask = pControllerCfg->uSidebandEnMask & pController->uChannelEnableMask;
         if (pController->uVersion == 0x20000000)
         {
            HAL_tsens_srot_SetSidebandChannelsEnabled(pController->pucTsensTMAddr, uSidebandEnMask);
         }
         else
         {
            HAL_tsens_srot_SetSidebandChannelsEnabled(pucTsensSROTAddr, uSidebandEnMask);
         }

         if (pControllerCfg->bStandAlone)
         {
            HAL_tsens_srot_SelectADCClkSrc(pucTsensSROTAddr, HAL_TSENS_EXTERNAL);
         }
         else
         {
            HAL_tsens_srot_SelectADCClkSrc(pucTsensSROTAddr, HAL_TSENS_INTERNAL);
         }

         if (pControllerCfg->pauTbcbReqs != NULL)
         {
            for (uTbcbClient = 0; uTbcbClient < pControllerCfg->uNumTbcbClients; uTbcbClient++)
            {
               HAL_tsens_srot_SetTempBroadcastClientEnabled(pucTsensSROTAddr,
                                                            uTbcbClient,
                                                            pControllerCfg->pauTbcbReqs[uTbcbClient]);
            }
         }

         HAL_tsens_tm_SetCycleMonitorInterruptEn(pucTsensTMAddr, FALSE);
         HAL_tsens_tm_SetWatchdogBarkInterruptEn(pucTsensTMAddr, FALSE);
         HAL_tsens_srot_SetWatchdogParameters(pucTsensSROTAddr,
                                              pControllerCfg->bWatchdogEn,
                                              pControllerCfg->bWatchdogResetEn,
                                              pControllerCfg->uWatchdogLoadVal);
      }
   }

   return TSENS_SUCCESS;
}

static TsensResultType Tsens_ConfigureSensors(void)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   const TsensSensorCfgType *pSensorCfg;
   const TsensChannelType *pChannel;
   const int32 nThresholdMin = gTsensDevCtxt.nThresholdMin;
   const int32 nThresholdMax = gTsensDevCtxt.nThresholdMax;
   TsensControllerType *pController;
   uint8 *pucTsensSROTAddr;
   uint8 *pucTsensTMAddr;
   uint32 uSensor;
   uint32 uController;
   uint32 uChannel;
   int32 nThreshold;

   for (uSensor = 0; uSensor < pBsp->uNumSensors; uSensor++)
   {
      pSensorCfg = &pBsp->paSensorCfgs[uSensor];
      uController = pSensorCfg->ucController;
      uChannel = pSensorCfg->ucChannel;
      pController = &gTsensDevCtxt.paControllers[uController];
      pucTsensSROTAddr = pController->pucTsensSROTAddr;
      pucTsensTMAddr = pController->pucTsensTMAddr;
      pChannel = &pController->aChannels[uChannel];

      if (!pChannel->bChannelIsDead)
      {
         /* Set up sensor ID */
         HAL_tsens_srot_SetSensorID(pucTsensSROTAddr, uChannel, pSensorCfg->ucSensorID);

         /* Set up scaling parameters */
         HAL_tsens_srot_SetConversionFactors(pucTsensSROTAddr,
                                             uChannel,
                                             gTsensDevCtxt.pTsensBsp->uShift,
                                             pChannel->uShiftedSlope,
                                             pChannel->uCodeAtZero);

         /* Min threshold configuration */
         if (pSensorCfg->nsThresholdMinDeciDegC != TSENS_THRESHOLD_DISABLED)
         {
            nThreshold = Tsens_CheckThreshold(pSensorCfg->nsThresholdMinDeciDegC);
            HAL_tsens_srot_SetThreshold(pucTsensSROTAddr, uChannel, HAL_TSENS_TH_MIN, nThreshold);
            HAL_tsens_srot_EnableInterrupt(pucTsensSROTAddr, uChannel, HAL_TSENS_TH_MIN);
         }
         else
         {
            HAL_tsens_srot_SetThreshold(pucTsensSROTAddr, uChannel, HAL_TSENS_TH_MIN, nThresholdMin);
            HAL_tsens_srot_DisableInterrupt(pucTsensSROTAddr, uChannel, HAL_TSENS_TH_MIN);
         }

         /* Lower threshold configuration */
         if (pSensorCfg->nsThresholdLowerDeciDegC != TSENS_THRESHOLD_DISABLED)
         {
            nThreshold = Tsens_CheckThreshold(pSensorCfg->nsThresholdLowerDeciDegC);
            HAL_tsens_tm_SetThreshold(pucTsensTMAddr, uChannel, HAL_TSENS_TH_LOWER, nThreshold);
            pController->uLowerEnableMask |= 1 << uChannel;
         }
         else
         {
            HAL_tsens_tm_SetThreshold(pucTsensTMAddr, uChannel, HAL_TSENS_TH_LOWER, nThresholdMin);
         }

         /* Upper threshold configuration */
         if (pSensorCfg->nsThresholdUpperDeciDegC != TSENS_THRESHOLD_DISABLED)
         {
            nThreshold = Tsens_CheckThreshold(pSensorCfg->nsThresholdUpperDeciDegC);
            HAL_tsens_tm_SetThreshold(pucTsensTMAddr, uChannel, HAL_TSENS_TH_UPPER, nThreshold);
            pController->uUpperEnableMask |= 1 << uChannel;
         }
         else
         {
            HAL_tsens_tm_SetThreshold(pucTsensTMAddr, uChannel, HAL_TSENS_TH_UPPER, nThresholdMax);
         }

         /* Critical threshold configuration */
         if (pSensorCfg->nsThresholdCriticalDeciDegC != TSENS_THRESHOLD_DISABLED)
         {
            nThreshold = Tsens_CheckThreshold(pSensorCfg->nsThresholdCriticalDeciDegC);
            HAL_tsens_tm_SetThreshold(pucTsensTMAddr, uChannel, HAL_TSENS_TH_CRITICAL, nThreshold);
            pController->uCriticalEnableMask |= 1 << uChannel;
         }
         else
         {
            HAL_tsens_tm_SetThreshold(pucTsensTMAddr, uChannel, HAL_TSENS_TH_CRITICAL, nThresholdMax);
         }

         /* Max threshold configuration */
         if (pSensorCfg->nsThresholdMaxDeciDegC != TSENS_THRESHOLD_DISABLED)
         {
            nThreshold = Tsens_CheckThreshold(pSensorCfg->nsThresholdMaxDeciDegC);
            HAL_tsens_srot_SetThreshold(pucTsensSROTAddr, uChannel, HAL_TSENS_TH_MAX, nThreshold);
            HAL_tsens_srot_EnableInterrupt(pucTsensSROTAddr, uChannel, HAL_TSENS_TH_MAX);
         }
         else
         {
            HAL_tsens_srot_SetThreshold(pucTsensSROTAddr, uChannel, HAL_TSENS_TH_MAX, nThresholdMax);
            HAL_tsens_srot_DisableInterrupt(pucTsensSROTAddr, uChannel, HAL_TSENS_TH_MAX);
         }
      }
   }

   /* Enable / Disable TM thresholds */
   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      pController = &gTsensDevCtxt.paControllers[uController];
      pucTsensTMAddr = pController->pucTsensTMAddr;

      HAL_tsens_tm_SetInterruptEn(pucTsensTMAddr, pController->uLowerEnableMask, HAL_TSENS_TH_LOWER);
      HAL_tsens_tm_SetInterruptEn(pucTsensTMAddr, pController->uUpperEnableMask, HAL_TSENS_TH_UPPER);
      HAL_tsens_tm_SetInterruptEn(pucTsensTMAddr, pController->uCriticalEnableMask, HAL_TSENS_TH_CRITICAL);

      if ((pController->uLowerEnableMask | pController->uUpperEnableMask) != 0)
      {
         HAL_tsens_tm_EnableUpperLowerInterrupt(pucTsensTMAddr);
      }
      else
      {
         HAL_tsens_tm_DisableUpperLowerInterrupt(pucTsensTMAddr);
      }

      if (pController->uCriticalEnableMask != 0)
      {
         HAL_tsens_tm_EnableCriticalInterrupt(pucTsensTMAddr);
      }
      else
      {
         HAL_tsens_tm_DisableCriticalInterrupt(pucTsensTMAddr);
      }
   }

   return TSENS_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
TsensResultType Tsens_Init(void)
{
   DALSYS_PROPERTY_HANDLE_DECLARE(hTsensBootProperties);
   const char *pszTsensBspPropName = "TSENS_BSP";
   const TsensControllerCfgType *pControllerCfg;
   const TsensBspType *pBsp;
   TsensControllerType *pController;
   DALSYSPropertyVar propertyVar;
   DALResult status;
   TsensResultType result;
   uint32 uController;
   DalChipInfoFamilyType eChipFamily;

   if (gbTsensInitialized)
   {
      return TSENS_SUCCESS;
   }

   DALSYS_InitMod(NULL);

   /* Get the BSP */
   eChipFamily = DalChipInfo_ChipFamily();

   if (eChipFamily == DALCHIPINFO_FAMILY_MSM8997)
   {
      pszTsensBspPropName = "TSENS_BSP_8997";
   }

   status = DALSYS_GetDALPropertyHandleStr("/core/hwengines/tsens", hTsensBootProperties);
   if (status != DAL_SUCCESS)
   {
      return TSENS_ERROR;
   }

   status = DALSYS_GetPropertyValue(hTsensBootProperties,
                                    pszTsensBspPropName,
                                    0,
                                    &propertyVar);
   if (status != DAL_SUCCESS)
   {
      return TSENS_ERROR;
   }

   pBsp = (TsensBspType *)propertyVar.Val.pStruct;
   gTsensDevCtxt.pTsensBsp = pBsp;

   /* Allocate memory for controller context */
   status = DALSYS_Malloc((pBsp->uNumControllers * sizeof(TsensControllerType)),
                          (void **)&gTsensDevCtxt.paControllers);
   if (status != DAL_SUCCESS || gTsensDevCtxt.paControllers == NULL)
   {
      return TSENS_ERROR;
   }

   /* Map physical to virtual addresses */
   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      pControllerCfg = &pBsp->paControllerCfgs[uController];
      pController = &gTsensDevCtxt.paControllers[uController];

      pController->pucTsensSROTAddr = Tsens_MapHWIORegion(pControllerCfg->pucTsensSROTPhys,
                                                          pControllerCfg->uTsensSROTSize);
      if (pController->pucTsensSROTAddr == NULL)
      {
         return TSENS_ERROR;
      }

      pController->pucTsensTMAddr = Tsens_MapHWIORegion(pControllerCfg->pucTsensTMPhys,
                                                        pControllerCfg->uTsensTMSize);
      if (pController->pucTsensSROTAddr == NULL)
      {
         return TSENS_ERROR;
      }
   }

   /* Get the controller version */
   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      pController = &gTsensDevCtxt.paControllers[uController];
      pController->uVersion = HAL_tsens_srot_GetControllerVersion(pController->pucTsensSROTAddr);
   }

   /* Obtain the max and min thresholds */
   gTsensDevCtxt.nThresholdMax = HAL_tsens_GetMaxTemp();
   gTsensDevCtxt.nThresholdMin = HAL_tsens_GetMinTemp();

   result = Tsens_GetCalibration();
   if (result != TSENS_SUCCESS)
   {
      return result;
   }

   result = Tsens_DisableControllers();
   if (result != TSENS_SUCCESS)
   {
      return result;
   }

   result = Tsens_ConfigureControllers();
   if (result != TSENS_SUCCESS)
   {
      return result;
   }

   result = Tsens_ConfigureSensors();
   if (result != TSENS_SUCCESS)
   {
      return result;
   }

   result = Tsens_EnableControllers();
   if (result != TSENS_SUCCESS)
   {
      return result;
   }

   result = Tsens_WaitForFirstRead();
   if (result != TSENS_SUCCESS)
   {
      return result;
   }

   gbTsensInitialized = TRUE;

   return TSENS_SUCCESS;
}

TsensResultType Tsens_GetNumSensors(uint32 *puNumSensors)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;

   if (puNumSensors == NULL)
   {
      return TSENS_ERROR;
   }

   if (!gbTsensInitialized)
   {
      return TSENS_ERROR_NOT_INITIALIZED;
   }

   *puNumSensors = pBsp->uNumSensors;

   return TSENS_SUCCESS;
}

TsensResultType Tsens_GetTemp(uint32 uSensor, int32 *pnDeciDegC)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   const TsensSensorCfgType *pSensorCfg;
   const TsensChannelType *pChannel;
   uint8 *pucTsensTMAddr;
   int32 nDeciDegC = 0;
   int32 nDeciDegCTry1;
   int32 nDeciDegCTry2;
   int32 nDeciDegCTry3;
   DALBOOL bValid;
   uint32 uController;
   uint32 uChannel;

   if (pnDeciDegC == NULL)
   {
      return TSENS_ERROR;
   }

   if (!gbTsensInitialized)
   {
      return TSENS_ERROR_NOT_INITIALIZED;
   }

   if (uSensor >= pBsp->uNumSensors)
   {
      return TSENS_ERROR;
   }

   pSensorCfg = &pBsp->paSensorCfgs[uSensor];
   uController = pSensorCfg->ucController;
   uChannel = pSensorCfg->ucChannel;
   pChannel = &gTsensDevCtxt.paControllers[uController].aChannels[uChannel];
   pucTsensTMAddr = gTsensDevCtxt.paControllers[uController].pucTsensTMAddr;

   if (pChannel->bChannelIsDead)
   {
      return TSENS_ERROR_SENSOR_NOT_AVAILABLE;
   }

   /* Read the ADC code */
   bValid = HAL_tsens_tm_GetChannelPrevTemp(pucTsensTMAddr, uChannel, &nDeciDegCTry1);
   if (bValid)
   {
      nDeciDegC = nDeciDegCTry1;
   }
   else
   {
      bValid = HAL_tsens_tm_GetChannelPrevTemp(pucTsensTMAddr, uChannel, &nDeciDegCTry2);
      if (bValid)
      {
         nDeciDegC = nDeciDegCTry2;
      }
      else
      {
         bValid = HAL_tsens_tm_GetChannelPrevTemp(pucTsensTMAddr, uChannel, &nDeciDegCTry3);
         if (bValid)
         {
            nDeciDegC = nDeciDegCTry3;
         }
         else if (nDeciDegCTry1 == nDeciDegCTry2)
         {
            nDeciDegC = nDeciDegCTry1;
         }
         else if (nDeciDegCTry2 == nDeciDegCTry3)
         {
            nDeciDegC = nDeciDegCTry2;
         }
         else
         {
            nDeciDegC = nDeciDegCTry1;
         }
      }
   }

   *pnDeciDegC = nDeciDegC;

   gTsensDevCtxt.nLastTempDeciDegC = nDeciDegC;
   gTsensDevCtxt.uLastSensor = uSensor;

   return TSENS_SUCCESS;
}

TsensResultType Tsens_GetMaxTemp(int32 *pnMaxDeciDegC)
{
   const TsensBspType *pBsp = gTsensDevCtxt.pTsensBsp;
   int32 nMaxDeciDegC = 0;
   DALBOOL bFirst = TRUE;
   uint8 *pucTsensTMAddr;
   uint32 uController;
   int32 nDeciDegC;
   uint32 uSensorID;

   if (pnMaxDeciDegC == NULL)
   {
      return TSENS_ERROR;
   }

   if (!gbTsensInitialized)
   {
      return TSENS_ERROR_NOT_INITIALIZED;
   }

   for (uController = 0; uController < pBsp->uNumControllers; uController++)
   {
      pucTsensTMAddr = gTsensDevCtxt.paControllers[uController].pucTsensTMAddr;

      HAL_tsens_tm_GetMaxTemp(pucTsensTMAddr, &uSensorID, &nDeciDegC);

      if (bFirst)
      {
         nMaxDeciDegC = nDeciDegC;
         bFirst = FALSE;
      }
      else
      {
         if (nDeciDegC > nMaxDeciDegC)
         {
            nMaxDeciDegC = nDeciDegC;
         }
      }
   }

   *pnMaxDeciDegC = nMaxDeciDegC;

   return TSENS_SUCCESS;
}

