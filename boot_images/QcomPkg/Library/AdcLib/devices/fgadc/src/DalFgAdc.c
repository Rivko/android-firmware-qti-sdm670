/*============================================================================
  FILE:         DalFgAdc.c

  OVERVIEW:     Implementation of a Physical ADC device DAL for the
                PMIC FGADC peripheral.

  DEPENDENCIES: None

                Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/fgadc/src/DalFgAdc.c#2 $$DateTime: 2018/07/16 04:57:13 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2018-06-29  KS   compensated PM660 slope/offset coefficients based on FAB_ID.
  2017-03-13  jjo  Forced batt id workaround.
  2016-12-07  jjo  Initiate SW req on fresh BATT_ID reads.
  2016-04-22  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "DalFgAdc.h"
#include "AdcInputs.h"
#include "DDIPlatformInfo.h"

/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/
#define ARRAY_LENGTH(a) (sizeof(a) / sizeof((a)[0]))

#define FGADC_SPMI_READ_NUM_RETRIES 5

#define FGADC_POLLING_TIME_US 100

#define FGADC_BATT_ID_TRIGGER_EVERY_READ 1
#define FGADC_BATT_ID_TRIGGER_FORCED_RESTORE 0

#define FGADC_MILLIVOLTS_PER_VOLT       1000
#define FGADC_MICROVOLTS_PER_MILLIVOLT  1000
#define FGADC_MILLIDEGC_PER_DEGC        1000
#define FGADC_KELVIN_TO_DEGC_OFFSET     273

#define FGADC_FAB_ADDR    0x1f2
#define FGADC_FAB_ID_SMIC 0x11
#define FGADC_FAB_ID_GF   0x30

#define FGADC_FAB_ID_660_GF    0x0
#define FGADC_FAB_ID_660_TSMC  0x2
#define FGADC_FAB_ID_660_MX    0x3

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Static Function Declarations
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Global Data Definitions
 * -------------------------------------------------------------------------*/
boolean gFgAdcOverrideRumiCheck = FALSE;

/*----------------------------------------------------------------------------
 * Static Variable Definitions
 * -------------------------------------------------------------------------*/
/*
 * Charger temperature sensor scaling table - PMI8998 GF
 *
 * The first column in the table is sensor voltage in millivolts and the
 * second column is the temperature in milli degrees C.
 *
 * Scaling equation:
 *
 *    milliDegC = (1303168 - uV) / 3.784 + 25000
 *
 */
static const AdcMapPtInt32toInt32Type gFgAdcMapChgTempSensorGF[] =
{
   { 1587, -50000 },
   {  792, 160000 }
};

/*
 * Charger temperature sensor scaling table - PMI8998 SMIC
 *
 * The first column in the table is sensor voltage in millivolts and the
 * second column is the temperature in milli degrees C.
 *
 * Scaling equation:
 *
 *    milliDegC = (1338433 - uV) / 3.655 + 25000
 *
 */
static const AdcMapPtInt32toInt32Type gFgAdcMapChgTempSensorSMIC[] =
{
   { 1613, -50000 },
   {  845, 160000 }
};

/*
 * Charger temperature sensor scaling table - PM660 GF
 *
 * The first column in the table is sensor voltage in millivolts and the
 * second column is the temperature in milli degrees C.
 *
 * Scaling equation:
 *
 *     milliDegC = (1309001 - uV) / 3.403 + 25000
 *
 */
static const AdcMapPtInt32toInt32Type gFgAdcMapChgTempSensorGF_660[] =
{
   { 1564, -50000 },
   {  850, 160000 }
};

/*
 * Charger temperature sensor scaling table - PM660 TSMC
 *
 * The first column in the table is sensor voltage in millivolts and the
 * second column is the temperature in milli degrees C.
 *
 * Scaling equation:
 *
 *     milliDegC = (1295898 - uV) / 3.596 + 25000
 *
 */
static const AdcMapPtInt32toInt32Type gFgAdcMapChgTempSensorTSMC_660[] =
{
   { 1566, -50000 },
   {  810, 160000 }
};

/*
 * Charger temperature sensor scaling table - PM660 MagnaChip
 *
 * The first column in the table is sensor voltage in millivolts and the
 * second column is the temperature in milli degrees C.
 *
 * Scaling equation:
 *
 *     milliDegC = (1314779 - uV) / 3.496 + 25000
 *
 */
static const AdcMapPtInt32toInt32Type gFgAdcMapChgTempSensorMX_660[] =
{
   { 1577, -50000 },
   {  843, 160000 }
};

/*----------------------------------------------------------------------------
 * Static Function Declarations and Definitions
 * -------------------------------------------------------------------------*/
static FgAdcHalResultType
FgAdc_GetFabID(
   FgAdcDevCtxt *pDevCtxt,
   uint32 *puFabID
   )
{
   uint32 uBytesRead;
   DALResult result;
   uint8 ucData;
   uint32 i;

   for (i = 0; i < FGADC_SPMI_READ_NUM_RETRIES; i++)
   {
      result = AdcSpmiReadAddr(&pDevCtxt->spmiCtxt,
                               FGADC_FAB_ADDR,
                               &ucData,
                               1,
                               &uBytesRead);
      if (result != DAL_SUCCESS || uBytesRead != 1)
      {
         FgAdc_LogError(&pDevCtxt->debug, "FgAdc - SPMI read failed, try again", FALSE);
         continue;
      }

      *puFabID = ucData;

      return FGADC_HAL_SUCCESS;
   }

   FgAdc_LogError(&pDevCtxt->debug, "FgAdc - SPMI read failed repeatedly", TRUE);
   return FGADC_HAL_ERROR;
}

static void
FgAdc_GetChgTempScalingWorkAround(
   FgAdcDevCtxt *pDevCtxt,
   const AdcMapPtInt32toInt32Type **ppaPts,
   uint32 *puTableSize
   )
{
   if (pDevCtxt->pmicDeviceInfo.ePmicModel == PMIC_IS_PMI8998)
   {
      if (pDevCtxt->uFabID == FGADC_FAB_ID_SMIC)
      {
         *ppaPts = gFgAdcMapChgTempSensorSMIC;
         *puTableSize = ARRAY_LENGTH(gFgAdcMapChgTempSensorSMIC);
      }
      else if (pDevCtxt->uFabID == FGADC_FAB_ID_GF)
      {
         *ppaPts = gFgAdcMapChgTempSensorGF;
         *puTableSize = ARRAY_LENGTH(gFgAdcMapChgTempSensorGF);
      }
      else
      {
         const FgAdcChannelConfigType *pChannel;

         pChannel = &pDevCtxt->pBsp->paChannels[pDevCtxt->uChargerTempIdx];
         *ppaPts = pChannel->pInterpolationTable;
         *puTableSize = pChannel->uInterpolationTableLength;
      }
   }
   
   else if (pDevCtxt->pmicDeviceInfo.ePmicModel == PMIC_IS_PM660)
   {
	  if (pDevCtxt->uFabID == FGADC_FAB_ID_660_GF)
      {
         *ppaPts = gFgAdcMapChgTempSensorGF_660;
         *puTableSize = ARRAY_LENGTH(gFgAdcMapChgTempSensorGF_660);
      }
      else if (pDevCtxt->uFabID == FGADC_FAB_ID_660_TSMC)
      {
         *ppaPts = gFgAdcMapChgTempSensorTSMC_660;
         *puTableSize = ARRAY_LENGTH(gFgAdcMapChgTempSensorTSMC_660);
      }
	  else if (pDevCtxt->uFabID == FGADC_FAB_ID_660_MX)
      {
         *ppaPts = gFgAdcMapChgTempSensorMX_660;
         *puTableSize = ARRAY_LENGTH(gFgAdcMapChgTempSensorMX_660);
      }
      else
      {
         const FgAdcChannelConfigType *pChannel;

         pChannel = &pDevCtxt->pBsp->paChannels[pDevCtxt->uChargerTempIdx];
         *ppaPts = pChannel->pInterpolationTable;
         *puTableSize = pChannel->uInterpolationTableLength;
      }
   }
	
   else
   {
      const FgAdcChannelConfigType *pChannel;

      pChannel = &pDevCtxt->pBsp->paChannels[pDevCtxt->uChargerTempIdx];
      *ppaPts = pChannel->pInterpolationTable;
      *puTableSize = pChannel->uInterpolationTableLength;
   }

   return;
}

/*======================================================================

  FUNCTION        FgAdc_ReadBytes

  DESCRIPTION     This function reads from the FgAdc peripheral

  DEPENDENCIES    None

  PARAMETERS
      pCtxt            [in]  pointer to the HAL interface context
      uOffset          [in]  the starting address
      pucData          [out] the bytes read
      uLen             [in]  the number of bytes to read

  RETURN VALUE    FGADC_HAL_SUCCESS or an error code

  SIDE EFFECTS    None

======================================================================*/
static FgAdcHalResultType
FgAdc_ReadBytes(
   void *pCtxt,
   uint32 uOffset,
   uint8 *pucData,
   uint32 uLen
   )
{
   FgAdcDevCtxt *pDevCtxt = (FgAdcDevCtxt *)pCtxt;
   uint32 uBytesRead;
   DALResult result;
   uint32 i;

   for (i = 0; i < FGADC_SPMI_READ_NUM_RETRIES; i++)
   {
      result = AdcSpmiRead(&pDevCtxt->spmiCtxt,
                           uOffset,
                           pucData,
                           uLen,
                           &uBytesRead);
      if (result != DAL_SUCCESS || uBytesRead != uLen)
      {
         FgAdc_LogError(&pDevCtxt->debug, "FgAdc - SPMI read failed, try again", FALSE);
         continue;
      }

      return FGADC_HAL_SUCCESS;
   }

   FgAdc_LogError(&pDevCtxt->debug, "FgAdc - SPMI read failed repeatedly", TRUE);
   return FGADC_HAL_ERROR;
}

/*======================================================================

  FUNCTION        FgAdc_WriteBytes

  DESCRIPTION     This function writes to the FgAdc peripheral

  DEPENDENCIES    None

  PARAMETERS
      pCtxt            [in] pointer to the HAL interface context
      uOffset          [in] the starting address
      pucData          [in] the bytes to write
      uLen             [in] the number of bytes to write

  RETURN VALUE    FGADC_HAL_SUCCESS or an error code

  SIDE EFFECTS    None

======================================================================*/
static FgAdcHalResultType
FgAdc_WriteBytes(
   void *pCtxt,
   uint32 uOffset,
   uint8 *pucData,
   uint32 uLen
   )
{
   FgAdcDevCtxt *pDevCtxt = (FgAdcDevCtxt *)pCtxt;
   DALResult result;

   result = AdcSpmiWrite(&pDevCtxt->spmiCtxt,
                         uOffset,
                         pucData,
                         uLen);
   if (result != DAL_SUCCESS)
   {
      FgAdc_LogError(&pDevCtxt->debug, "FgAdc - SPMI write failed", TRUE);
      return FGADC_HAL_ERROR;
   }

   return FGADC_HAL_SUCCESS;
}

/*======================================================================

  FUNCTION        FgAdc_ScaleCodeToMicrovolts

  DESCRIPTION
      Scales code to microvolts.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt         [in]
      uCode            [in]

  RETURN VALUE
      Microvolts

  SIDE EFFECTS
      None

======================================================================*/
static uint32
FgAdc_ScaleCodeToMicrovolts(
   FgAdcDevCtxt *pDevCtxt,
   uint32 uCode
   )
{
   return (uCode * pDevCtxt->pBsp->uFullScale_uV) / pDevCtxt->pBsp->uFullScale_code;
}

/*======================================================================

  FUNCTION        FgAdc_ScaleMicrovoltsToCode

  DESCRIPTION
      Scales microvolts to code.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt         [in]
      uMicrovolts      [in]

  RETURN VALUE
      Code

  SIDE EFFECTS
      None

======================================================================*/
static uint32
FgAdc_ScaleMicrovoltsToCode(
   FgAdcDevCtxt *pDevCtxt,
   uint32 uMicrovolts
   )
{
   return (uMicrovolts * pDevCtxt->pBsp->uFullScale_code) / pDevCtxt->pBsp->uFullScale_uV;
}

/*======================================================================

  FUNCTION        FgAdc_ProcessConversionResult

  DESCRIPTION
      Scales the conversion result.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt         [in]
      pChannel         [in]
      uCode            [in]
      pAdcDeviceResult [out]

  RETURN VALUE
      None

  SIDE EFFECTS
      None

======================================================================*/
static void
FgAdc_ProcessConversionResult(
   FgAdcDevCtxt *pDevCtxt,
   const FgAdcChannelConfigType *pChannel,
   uint32 uCode,
   AdcDeviceResultType *pResult
   )
{
   uint32 uMicrovolts;

   pResult->eStatus = ADC_DEVICE_RESULT_VALID;

   pResult->uCode = uCode;

   /* Percent result is not useful for FG ADC */
   pResult->uPercent = 0;

   if (pChannel->eScaling == FGADC_SCALE_THERMISTOR)
   {
      /* Microvolts is unknown for hardware-scaled thermistors */
      pResult->uMicrovolts = 0;
   }
   else
   {
      uMicrovolts = FgAdc_ScaleCodeToMicrovolts(pDevCtxt,
                                                uCode);

      uMicrovolts = (uMicrovolts * pChannel->scalingFactor.uDenominator) /
                    pChannel->scalingFactor.uNumerator;

      pResult->uMicrovolts = uMicrovolts;
   }

   switch (pChannel->eScaling)
   {
      case FGADC_SCALE_TO_MILLIVOLTS:
         pResult->nPhysical = (int32)(pResult->uMicrovolts / FGADC_MILLIVOLTS_PER_VOLT);
         break;

      case FGADC_SCALE_INTERPOLATE_FROM_MILLIVOLTS:
         if (pChannel->eChannel == FGADC_CHAN_CHARGER_TEMP)
         {
            const AdcMapPtInt32toInt32Type *paPts;
            uint32 uTableSize;

            FgAdc_GetChgTempScalingWorkAround(pDevCtxt,
                                              &paPts,
                                              &uTableSize);

            pResult->eStatus = AdcMapLinearInt32toInt32(paPts,
                                                        uTableSize,
                                                        pResult->uMicrovolts / FGADC_MILLIVOLTS_PER_VOLT,
                                                        &pResult->nPhysical);
         }
         else
         {
            pResult->eStatus = AdcMapLinearInt32toInt32(pChannel->pInterpolationTable,
                                                        pChannel->uInterpolationTableLength,
                                                        pResult->uMicrovolts / FGADC_MILLIVOLTS_PER_VOLT,
                                                        &pResult->nPhysical);
         }
         break;

      case FGADC_SCALE_THERMISTOR:
         pResult->nPhysical = (int32)(uCode / pDevCtxt->pBsp->uCodePerKelvin);
         pResult->nPhysical -= FGADC_KELVIN_TO_DEGC_OFFSET;
         break;

      case FGADC_SCALE_CURRENT_TO_MILLIAMPS:
         pResult->nPhysical = (int32)(pResult->uMicrovolts /
                              pDevCtxt->pBsp->uMicroVoltsPerMilliAmps);
         break;

      default:
         pResult->eStatus = ADC_DEVICE_RESULT_INVALID;
   }
}

/*======================================================================

  FUNCTION        FgAdc_ScaleThreshold

  DESCRIPTION
      Scales a threshold to ADC code

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt    [in]
      eThresh     [in]
      nThreshold  [in]

  RETURN VALUE
      ADC code

  SIDE EFFECTS
      None

======================================================================*/
static uint32
FgAdc_ScaleThreshold(
   FgAdcDevCtxt *pDevCtxt,
   FgAdcThresholdType eThresh,
   int32 nThreshold
   )
{
   const FgAdcChannelConfigType *pChannel;
   uint32 uMicrovolts;
   int32 nMillivolts;
   uint32 uCode;

   if (eThresh == FGADC_THRESH_SKIN_HOT ||
       eThresh == FGADC_THRESH_SKIN_TOO_HOT)
   {
      /* Skin temp threshold */
      uCode = (uint32)((nThreshold + 30) * 2);
   }
   else
   {
      /* Charger temp threshold */
      const AdcMapPtInt32toInt32Type *paPts;
      uint32 uTableSize;

      pChannel = &pDevCtxt->pBsp->paChannels[pDevCtxt->uChargerTempIdx];

      FgAdc_GetChgTempScalingWorkAround(pDevCtxt,
                                        &paPts,
                                        &uTableSize);

      AdcMapLinearInt32toInt32Inverse(paPts,
                                      uTableSize,
                                      nThreshold * FGADC_MILLIDEGC_PER_DEGC,
                                      &nMillivolts);

      uMicrovolts = (uint32)(nMillivolts * FGADC_MICROVOLTS_PER_MILLIVOLT);
      uMicrovolts = (uMicrovolts * pChannel->scalingFactor.uNumerator) /
                    pChannel->scalingFactor.uDenominator;

      uCode = FgAdc_ScaleMicrovoltsToCode(pDevCtxt,
                                          uMicrovolts);

      /* Truncate the lower two bits */
      uCode = uCode >> 2;
   }

   return uCode;
}

/*======================================================================

  FUNCTION        FgAdc_ReadBattId

  DESCRIPTION
      This function reads BATT_ID and scales the result.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt         [in]
      pChannel         [in]
      bFreshReading    [in]
      pAdcDeviceResult [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None

======================================================================*/
static DALResult
FgAdc_ReadBattId(
   FgAdcDevCtxt *pDevCtxt,
   const FgAdcChannelConfigType *pChannel,
   boolean bFreshReading,
   AdcDeviceResultType *pAdcDeviceResult
   )
{
   FgAdcChanStatusType chanStatus;
   FgAdcHalResultType result;
   uint32 uCurrent;
   uint32 uCode;
   uint32 uTimeWaitedUs = 0;
   boolean bTimedOut = TRUE;
   uint8 ucOrigTriggers;

   if (bFreshReading)
   {
      result = FgAdcHalGetChanTrigs(&pDevCtxt->iFgAdcHalInterface,
                                    pChannel->eChannel,
                                    &ucOrigTriggers);
      if (result != FGADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }

      /* Force a reading by setting the trigger to always */
      result = FgAdcHalSetChanTrigs(&pDevCtxt->iFgAdcHalInterface,
                                    pChannel->eChannel,
                                    FGADC_BATT_ID_TRIGGER_EVERY_READ);
      if (result != FGADC_HAL_SUCCESS)
      {
         (void)FgAdcHalSetChanTrigs(&pDevCtxt->iFgAdcHalInterface,
                                    pChannel->eChannel,
                                    ucOrigTriggers);
         return DAL_ERROR;
      }

      do
      {
         result = FgAdcHalMakeSwReq(&pDevCtxt->iFgAdcHalInterface);
         if (result != FGADC_HAL_SUCCESS)
         {
            (void)FgAdcHalSetChanTrigs(&pDevCtxt->iFgAdcHalInterface,
                                       pChannel->eChannel,
                                       ucOrigTriggers);
            return DAL_ERROR;
         }

         result = FgAdcHalGetChanStatus(&pDevCtxt->iFgAdcHalInterface,
                                        pChannel->eChannel,
                                        &chanStatus);
         if (result != FGADC_HAL_SUCCESS)
         {
            (void)FgAdcHalSetChanTrigs(&pDevCtxt->iFgAdcHalInterface,
                                       pChannel->eChannel,
                                       ucOrigTriggers);
            return DAL_ERROR;
         }

         if (chanStatus.bReadingAvailable && chanStatus.bReadingFresh)
         {
            bTimedOut = FALSE;
            break;
         }

         DALSYS_BusyWait(FGADC_POLLING_TIME_US);
         uTimeWaitedUs += FGADC_POLLING_TIME_US;
      } while (uTimeWaitedUs < pDevCtxt->pBsp->uMaxWaitTimeUs);

      if (bTimedOut)
      {
         FgAdc_LogError(&pDevCtxt->debug, "FgAdc - Fresh BATT_ID reading timed out", FALSE);
         FgAdc_LogDumpRegisters(&pDevCtxt->debug, &pDevCtxt->iFgAdcHalInterface);
      }

      result = FgAdcHalSetChanTrigs(&pDevCtxt->iFgAdcHalInterface,
                                    pChannel->eChannel,
                                    ucOrigTriggers);
      if (result != FGADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }
   }

   result = FgAdcHalGetConversionCode(&pDevCtxt->iFgAdcHalInterface,
                                      FGADC_CHAN_BATT_ID_150,
                                      &uCode);
   if (result != FGADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   uCurrent = 150;

   if (uCode > pDevCtxt->pBsp->uBattIdClipThresh)
   {
      result = FgAdcHalGetConversionCode(&pDevCtxt->iFgAdcHalInterface,
                                         FGADC_CHAN_BATT_ID_15,
                                         &uCode);
      if (result != FGADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }

      uCurrent = 15;

      if (uCode > pDevCtxt->pBsp->uBattIdClipThresh)
      {
         result = FgAdcHalGetConversionCode(&pDevCtxt->iFgAdcHalInterface,
                                            FGADC_CHAN_BATT_ID_5,
                                            &uCode);
         if (result != FGADC_HAL_SUCCESS)
         {
            return DAL_ERROR;
         }

         uCurrent = 5;
      }
   }

   pAdcDeviceResult->uCode = uCode;

   pAdcDeviceResult->uMicrovolts = FgAdc_ScaleCodeToMicrovolts(pDevCtxt,
                                                               uCode);

   pAdcDeviceResult->nPhysical = (int32)(pAdcDeviceResult->uMicrovolts / uCurrent);

   /* Percent result is not useful for FG ADC so overload with
      which channel was used */
   pAdcDeviceResult->uPercent = uCurrent;

   if (bFreshReading && bTimedOut)
   {
      pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_TIMEOUT;
   }
   else
   {
      pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_VALID;
   }

   return DAL_SUCCESS;
}

/*======================================================================

  FUNCTION        FgAdc_ReadChannelInternal

  DESCRIPTION
      This function reads an ADC channel and returns the scaled result
      and status of the read.

  DEPENDENCIES
      None

  PARAMETERS
      pDevCtxt         [in]
      uChannel         [in]
      pAdcDeviceResult [out]

  RETURN VALUE
      DAL_SUCCESS or an error

  SIDE EFFECTS
      None

======================================================================*/
static DALResult
FgAdc_ReadChannelInternal(
   FgAdcDevCtxt *pDevCtxt,
   uint32 uChannel,
   AdcDeviceResultType *pAdcDeviceResult
   )
{
   const FgAdcChannelConfigType *pChannel = &pDevCtxt->pBsp->paChannels[uChannel];
   FgAdcChanStatusType chanStatus;
   FgAdcHalResultType result;
   boolean bFreshReading = FALSE;
   boolean bTimedOut = FALSE;
   DALResult ret;
#ifndef ADC_LOADER_DRIVER
   uint32 uCode;
#endif

   result = FgAdcHalGetChanStatus(&pDevCtxt->iFgAdcHalInterface,
                                  pChannel->eChannel,
                                  &chanStatus);
   if (result != FGADC_HAL_SUCCESS || !chanStatus.bReadingAvailable)
   {
      /* The reading timed out - proceed with getting the ADC codes and mark
         the result as invalid due to time out */
      FgAdc_LogError(&pDevCtxt->debug, "FgAdc - Reading not available", FALSE);
      FgAdc_LogDumpRegisters(&pDevCtxt->debug, &pDevCtxt->iFgAdcHalInterface);
      bTimedOut = TRUE;
   }

   if (result == FGADC_HAL_SUCCESS && chanStatus.bReadingFresh)
   {
      bFreshReading = TRUE;
   }

   if (pChannel->eChannel == FGADC_CHAN_BATT_ID ||
       pChannel->eChannel == FGADC_CHAN_BATT_ID_FRESH)
   {
      if (pChannel->eChannel == FGADC_CHAN_BATT_ID_FRESH)
      {
         bFreshReading = TRUE;
      }

      ret = FgAdc_ReadBattId(pDevCtxt,
                             pChannel,
                             bFreshReading,
                             pAdcDeviceResult);
      if (ret != DAL_SUCCESS)
      {
         return ret;
      }
   }
   else
   {
#ifndef ADC_LOADER_DRIVER
      result = FgAdcHalGetConversionCode(&pDevCtxt->iFgAdcHalInterface,
                                         pChannel->eChannel,
                                         &uCode);
      if (result != FGADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }

      FgAdc_ProcessConversionResult(pDevCtxt,
                                    pChannel,
                                    uCode,
                                    pAdcDeviceResult);
#else
      return DAL_ERROR;
#endif
   }

   FgAdc_LogConversionResult(&pDevCtxt->debug,
                             pChannel->pszName,
                             pAdcDeviceResult);

   if (!bFreshReading && bTimedOut)
   {
      pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_TIMEOUT;
   }
   else if (!bFreshReading)
   {
      pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_STALE;
   }

   return DAL_SUCCESS;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * The following functions are for DALDriver specific functionality
 * -------------------------------------------------------------------------*/
DALResult
FgAdc_DriverInit(FgAdcDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

DALResult
FgAdc_DriverDeInit(FgAdcDrvCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

/*----------------------------------------------------------------------------
 * The following functions are declared in DalDevice Interface.
 * -------------------------------------------------------------------------*/
DALResult
FgAdc_DeviceInit(FgAdcClientCtxt *pCtxt)
{
   FgAdcDevCtxt *pDevCtxt = pCtxt->pFgAdcDevCtxt;
   const FgAdcChannelConfigType *pChannel;
   DALSYSPropertyVar propVar;
   FgAdcHalResultType result;
   uint32 uChannel;
   DALResult ret;
#ifndef FGADC_UNIT_TEST
   pm_err_flag_type pmResult;
   uint32 uGpioIdx;
#endif

   ret = FgAdc_LogInit(&pDevCtxt->debug, pDevCtxt->DevId);
   if (ret != DAL_SUCCESS)
   {
      return ret;
   }

   if (DALPLATFORMINFO_TYPE_RUMI == DalPlatformInfo_Platform() &&
       !gFgAdcOverrideRumiCheck)
   {
      /* PMIC is usually not supported on RUMI */
      pDevCtxt->bHardwareSupported = FALSE;
      return DAL_SUCCESS;
   }
   else
   {
      pDevCtxt->bHardwareSupported = TRUE;
   }

   /* Read the DAL properties - hProp populated in device attach */
   ret = DALSYS_GetPropertyValue(pDevCtxt->hProp, "FGADC_BSP", 0, &propVar);
   if (ret != DAL_SUCCESS)
   {
      FgAdc_LogError(&pDevCtxt->debug, "FgAdc - failed to get BSP", TRUE);
      return ret;
   }

   pDevCtxt->pBsp = (FgAdcBspType *)propVar.Val.pStruct;

#ifndef FGADC_UNIT_TEST
   pmResult = pm_get_pmic_info(pDevCtxt->pBsp->ucPmicDevice, &pDevCtxt->pmicDeviceInfo);
   if (pmResult != PM_ERR_FLAG__SUCCESS)
   {
      FgAdc_LogError(&pDevCtxt->debug, "FgAdc - failed to get PMIC device info", TRUE);
      return DAL_ERROR;
   }
#endif

   /* Attach to to the SPMI driver */
   ret = AdcSpmiInit(&pDevCtxt->spmiCtxt,
                     pDevCtxt->pBsp->eAccessPriority,
                     pDevCtxt->pBsp->uSlaveId,
                     pDevCtxt->pBsp->uPeripheralId);
   if (ret != DAL_SUCCESS)
   {
      FgAdc_LogError(&pDevCtxt->debug, "FgAdc - failed to attach to SPMI", TRUE);
      return ret;
   }

   /* Fab ID is needed for CHG_TEMP scaling on PMI8998 */
   if (pDevCtxt->pmicDeviceInfo.ePmicModel == PMIC_IS_PMI8998 || pDevCtxt->pmicDeviceInfo.ePmicModel == PMIC_IS_PM660)
   {
      result = FgAdc_GetFabID(pDevCtxt, &pDevCtxt->uFabID);
      if (result != FGADC_HAL_SUCCESS)
      {
         FgAdc_LogError(&pDevCtxt->debug, "FgAdc - failed to get fab ID", TRUE);
         return DAL_ERROR;
      }
   }

   /* Initialize the HAL interface */
   pDevCtxt->iFgAdcHalInterface.pCtxt = (void *)pDevCtxt;
   pDevCtxt->iFgAdcHalInterface.pfnWriteBytes = FgAdc_WriteBytes;
   pDevCtxt->iFgAdcHalInterface.pfnReadBytes = FgAdc_ReadBytes;

   /* Verify the hardware meets the minimum version requirements */
   result = FgAdcHalGetRevisionInfo(&pDevCtxt->iFgAdcHalInterface, &pDevCtxt->revisionInfo);
   if (result != FGADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   if (pDevCtxt->pBsp->ucPerphType != pDevCtxt->revisionInfo.ucType)
   {
      FgAdc_LogError(&pDevCtxt->debug, "FgAdc - Invalid FGADC peripheral type", FALSE);
      pDevCtxt->bHardwareSupported = FALSE;
      return DAL_SUCCESS;
   }

   /* Charger temperature index is needed for threshold calculations */
   ret = FgAdc_GetChannel(pCtxt,
                          ADC_INPUT_CHG_TEMP,
                          &pDevCtxt->uChargerTempIdx);
   if (ret != DAL_SUCCESS)
   {
      FgAdc_LogError(&pDevCtxt->debug, "FgAdc - failed to look up charger channel", TRUE);
      return ret;
   }

   /* Set up the RR ADC and start it. At this point, not all RR
    * ADC configuration will be done. PMIC driver will come in
    * and set up the thermistors and thresholds.
    */
   for (uChannel = 0; uChannel < pDevCtxt->pBsp->uNumChannels; uChannel++)
   {
      pChannel = &pDevCtxt->pBsp->paChannels[uChannel];

      result = FgAdcHalSetChanEn(&pDevCtxt->iFgAdcHalInterface,
                                 pChannel->eChannel,
                                 pChannel->eEnable);
      if (result != FGADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }

      if (pChannel->eEnable == FGADC_ENABLE &&
          pChannel->ucTriggers != FGADC_TRIGGER_DEFAULT)
      {
         result = FgAdcHalSetChanTrigs(&pDevCtxt->iFgAdcHalInterface,
                                       pChannel->eChannel,
                                       pChannel->ucTriggers);
         if (result != FGADC_HAL_SUCCESS)
         {
            return DAL_ERROR;
         }
      }
   }

#ifndef FGADC_UNIT_TEST
   /* Map static GPIOs */
   for (uGpioIdx = 0; uGpioIdx < pDevCtxt->pBsp->uNumGpios; uGpioIdx++)
   {
      pmResult = pm_gpio_set_ext_pin_config(pDevCtxt->pBsp->ucPmicDevice,
                                            pDevCtxt->pBsp->paeGpios[uGpioIdx],
                                            PM_GPIO_EXT_PIN_DISABLE);
      if (pmResult != PM_ERR_FLAG__SUCCESS)
      {
         FgAdc_LogError(&pDevCtxt->debug, "FgAdc - failed to configure GPIO", TRUE);
         return DAL_ERROR;
      }
   }
#endif

   result = FgAdcHalSetEnable(&pDevCtxt->iFgAdcHalInterface,
                              FGADC_ENABLE);
   if (result != FGADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   result = FgAdcHalSetShutdown(&pDevCtxt->iFgAdcHalInterface,
                                FGADC_SHDN_NOT_REQUESTED);
   if (result != FGADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

DALResult
FgAdc_DeviceDeInit(FgAdcClientCtxt *pCtxt)
{
   /* ADC is marked as static - deinit is never called */
   return DAL_SUCCESS;
}

DALResult
FgAdc_Open(
   FgAdcClientCtxt *pCtxt,
   uint32 dwaccessMode
   )
{
   return DAL_SUCCESS;
}

DALResult
FgAdc_Close(FgAdcClientCtxt *pCtxt)
{
   return DAL_SUCCESS;
}

/*----------------------------------------------------------------------------
 * The following functions are extended in DalAdcDevice Interface.
 *
 * These functions are documented in DDIAdcDevice.h
 * -------------------------------------------------------------------------*/
DALResult
FgAdc_SetDeviceIndex(
   FgAdcClientCtxt *pCtxt,
   uint32 uDeviceIndex
   )
{
   FgAdcDevCtxt *pDevCtxt = pCtxt->pFgAdcDevCtxt;

   pDevCtxt->uDeviceIndex = uDeviceIndex;

   return DAL_SUCCESS;
}

DALResult
FgAdc_GetDeviceProperties(
   FgAdcClientCtxt *pCtxt,
   AdcDeviceDevicePropertiesType *pAdcDeviceProp
   )
{
   FgAdcDevCtxt *pDevCtxt = pCtxt->pFgAdcDevCtxt;

   if (pDevCtxt->bHardwareSupported)
   {
      pAdcDeviceProp->uNumChannels = pDevCtxt->pBsp->uNumChannels;
   }
   else
   {
      pAdcDeviceProp->uNumChannels = 0;
   }

   pAdcDeviceProp->uNumMeas = 0;

   return DAL_SUCCESS;
}

DALResult
FgAdc_GetChannel(
   FgAdcClientCtxt *pCtxt,
   const char *pszChannelName,
   uint32 *puChannelIdx
   )
{
   FgAdcDevCtxt *pDevCtxt = pCtxt->pFgAdcDevCtxt;
   const FgAdcChannelConfigType *pChannel;
   uint32 uNumChannels;
   uint32 uChannel;

   if (!pDevCtxt->bHardwareSupported)
   {
      return DAL_ERROR;
   }

   uNumChannels = pDevCtxt->pBsp->uNumChannels;

   for (uChannel = 0; uChannel < uNumChannels; uChannel++)
   {
      pChannel = &pDevCtxt->pBsp->paChannels[uChannel];

      if (pChannel->eEnable == FGADC_ENABLE)
      {
         if (strcmp(pszChannelName, pChannel->pszName) == 0)
         {
            *puChannelIdx = uChannel;
            return DAL_SUCCESS;
         }
      }
   }

   return DAL_ERROR;
}

DALResult
FgAdc_ReadChannel(
   FgAdcClientCtxt *pCtxt,
   uint32 uChannelIdx,
   AdcDeviceResultType *pAdcDeviceResult
   )
{
   FgAdcDevCtxt *pDevCtxt = pCtxt->pFgAdcDevCtxt;

   pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_INVALID;

   if (!pDevCtxt->bHardwareSupported)
   {
      return DAL_ERROR;
   }

   if (uChannelIdx >= pDevCtxt->pBsp->uNumChannels ||
       pDevCtxt->pBsp->paChannels[uChannelIdx].eEnable == FGADC_DISABLE)
   {
      pAdcDeviceResult->eStatus = ADC_DEVICE_RESULT_INVALID_CHANNEL;
      return DAL_ERROR;
   }

   FgAdc_LogConversionRequest(&pDevCtxt->debug, uChannelIdx);

   return FgAdc_ReadChannelInternal(pDevCtxt,
                                    uChannelIdx,
                                    pAdcDeviceResult);
}

DALResult
FgAdc_SetThreshold(
   FgAdcClientCtxt *pCtxt,
   AdcDeviceFGThresholdType eWhichThresh,
   int32 nThreshold
   )
{
   FgAdcDevCtxt *pDevCtxt = pCtxt->pFgAdcDevCtxt;
   const FgAdcThreshRangeType *pRange;
   FgAdcThresholdType eHalThresh;
   FgAdcHalResultType result;
   uint32 uCode;

   if (!pDevCtxt->bHardwareSupported)
   {
      return DAL_ERROR;
   }

   switch (eWhichThresh)
   {
      case ADC_DEVICE_FG_THRESH_SKIN_HOT:
         eHalThresh = FGADC_THRESH_SKIN_HOT;
         pRange = pDevCtxt->pBsp->pSkinTempThreshRange;
         break;
      case ADC_DEVICE_FG_THRESH_SKIN_TOO_HOT:
         eHalThresh = FGADC_THRESH_SKIN_TOO_HOT;
         pRange = pDevCtxt->pBsp->pSkinTempThreshRange;
         break;
      case ADC_DEVICE_FG_THRESH_CHARGER_HOT:
         eHalThresh = FGADC_THRESH_CHARGER_HOT;
         pRange = pDevCtxt->pBsp->pChgTempThreshRange;
         break;
      case ADC_DEVICE_FG_THRESH_CHARGER_TOO_HOT:
         eHalThresh = FGADC_THRESH_CHARGER_TOO_HOT;
         pRange = pDevCtxt->pBsp->pChgTempThreshRange;
         break;
      default:
         return DAL_ERROR;
   }

   if (nThreshold < pRange->nMin || nThreshold > pRange->nMax)
   {
      return DAL_ERROR_INVALID_PARAMETER;
   }

   uCode = FgAdc_ScaleThreshold(pDevCtxt,
                                eHalThresh,
                                nThreshold);

   result = FgAdcHalSetThreshold(&pDevCtxt->iFgAdcHalInterface,
                                 eHalThresh,
                                 uCode);
   if (result != FGADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   result = FgAdcHalClearThresholdComparison(&pDevCtxt->iFgAdcHalInterface,
                                             eHalThresh);
   if (result != FGADC_HAL_SUCCESS)
   {
      return DAL_ERROR;
   }

   return DAL_SUCCESS;
}

DALResult
FgAdc_SetBattIdForced(
   FgAdcClientCtxt *pCtxt,
   DALBOOL bEnable
   )
{
   FgAdcDevCtxt *pDevCtxt = pCtxt->pFgAdcDevCtxt;
   FgAdcHalResultType result;

   if (!pDevCtxt->bHardwareSupported)
   {
      return DAL_ERROR;
   }

   if (bEnable)
   {
      result = FgAdcHalSetChanTrigs(&pDevCtxt->iFgAdcHalInterface,
                                    FGADC_CHAN_BATT_ID,
                                    FGADC_BATT_ID_TRIGGER_EVERY_READ);
      if (result != FGADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }

      result = FgAdcHalSetContModeEn(&pDevCtxt->iFgAdcHalInterface,
                                     TRUE);
      if (result != FGADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }
   }
   else
   {
      result = FgAdcHalSetChanTrigs(&pDevCtxt->iFgAdcHalInterface,
                                    FGADC_CHAN_BATT_ID,
                                    FGADC_BATT_ID_TRIGGER_FORCED_RESTORE);
      if (result != FGADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }

      result = FgAdcHalSetContModeEn(&pDevCtxt->iFgAdcHalInterface,
                                     FALSE);
      if (result != FGADC_HAL_SUCCESS)
      {
         return DAL_ERROR;
      }
   }

   return DAL_SUCCESS;
}

