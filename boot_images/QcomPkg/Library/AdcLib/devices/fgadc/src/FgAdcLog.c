#ifndef ADC_LOG_DISABLED
/*============================================================================
  FILE:         FgAdcLog.c

  OVERVIEW:     Logging for the FgAdc

  DEPENDENCIES: None

                Copyright (c) 2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/fgadc/src/FgAdcLog.c#1 $$DateTime: 2017/09/18 09:26:13 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2016-04-22  jjo  Initial revision.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "FgAdcLog.h"
#include "DDITimetick.h"
#include "msg.h"

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
static uint64
FgAdc_GetTimetick(const FgAdcDebugInfoType *pFgAdcDebugInfo)
{
   DALResult result;
   DalTimetickTime64Type u64Tick;

   result = DalTimetick_GetTimetick64(pFgAdcDebugInfo->phTimetickDev, &u64Tick);
   if (result != DAL_SUCCESS)
   {
      return 0;
   }

   return (uint64)u64Tick;
}

/*----------------------------------------------------------------------------
 * Externalized Function Definitions
 * -------------------------------------------------------------------------*/
/*======================================================================

  FUNCTION        FgAdc_LogConversionRequest

  DESCRIPTION     This function logs a conversion request

  DEPENDENCIES    None

  PARAMETERS      pFgAdcDebugInfo [in] debug structure
                  uChannelIdx    [in] the channel index

  RETURN VALUE    None


  SIDE EFFECTS    None

======================================================================*/
void
FgAdc_LogConversionRequest(
   FgAdcDebugInfoType *pFgAdcDebugInfo,
   uint32 uChannelIdx
   )
{
   uint32 uCurIdx;

   DALSYS_SyncEnter(pFgAdcDebugInfo->hSync);

   uCurIdx = pFgAdcDebugInfo->uCurIdx;
   if (uCurIdx < FGADC_LOG_MAX - 1)
   {
      pFgAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pFgAdcDebugInfo->uCurIdx = 0;
   }

   DALSYS_SyncLeave(pFgAdcDebugInfo->hSync);

   pFgAdcDebugInfo->aLog[uCurIdx].uTimestamp = FgAdc_GetTimetick(pFgAdcDebugInfo);
   pFgAdcDebugInfo->aLog[uCurIdx].eEvent = FGADC_LOG_EVENT_CONVERSION_REQUEST;
   pFgAdcDebugInfo->aLog[uCurIdx].uChannelIdx = uChannelIdx;
   pFgAdcDebugInfo->aLog[uCurIdx].pszErrorMsg = NULL;

   MSG_1(MSG_SSID_ADC, MSG_LEGACY_MED, "FGADC conv req: index %u", uChannelIdx);
}

/*======================================================================

  FUNCTION        FgAdc_LogConversionResult

  DESCRIPTION     This function logs the conversion result

  DEPENDENCIES    None

  PARAMETERS      pFgAdcDebugInfo [in] debug structure
                  pszChanName    [in] channel name
                  pResult        [in] the AdcDeviceResultType result data

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
void
FgAdc_LogConversionResult(
   FgAdcDebugInfoType *pFgAdcDebugInfo,
   const char *pszChanName,
   const AdcDeviceResultType *pResult
   )
{
   uint32 uCurIdx;

   DALSYS_SyncEnter(pFgAdcDebugInfo->hSync);

   uCurIdx = pFgAdcDebugInfo->uCurIdx;
   if (uCurIdx < FGADC_LOG_MAX - 1)
   {
      pFgAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pFgAdcDebugInfo->uCurIdx = 0;
   }

   DALSYS_SyncLeave(pFgAdcDebugInfo->hSync);

   pFgAdcDebugInfo->aLog[uCurIdx].result = *pResult;
   pFgAdcDebugInfo->aLog[uCurIdx].uTimestamp = FgAdc_GetTimetick(pFgAdcDebugInfo);
   pFgAdcDebugInfo->aLog[uCurIdx].eEvent = FGADC_LOG_EVENT_RESULT;
   pFgAdcDebugInfo->aLog[uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pFgAdcDebugInfo->aLog[uCurIdx].pszErrorMsg = NULL;

   MSG_SPRINTF_6(MSG_SSID_ADC,
                 MSG_LEGACY_HIGH,
                 "FGADC conv result for %s: eStatus %i, nPhysical %i, uPercent %u, uMicrovolts %u, uCode %u",
                 pszChanName,
                 pResult->eStatus,
                 pResult->nPhysical,
                 pResult->uPercent,
                 pResult->uMicrovolts,
                 pResult->uCode);
}

/*======================================================================

  FUNCTION        FgAdc_LogError

  DESCRIPTION     This function logs an error

  DEPENDENCIES    None

  PARAMETERS      pFgAdcDebugInfo [in] debug structure
                  pszErrorMsg [in]
                  bFatalError [in]

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
void
FgAdc_LogError(
   FgAdcDebugInfoType *pFgAdcDebugInfo,
   const char *pszErrorMsg,
   DALBOOL bFatalError
   )
{
   uint32 uCurIdx;

   DALSYS_SyncEnter(pFgAdcDebugInfo->hSync);

   uCurIdx = pFgAdcDebugInfo->uCurIdx;
   if (uCurIdx < FGADC_LOG_MAX - 1)
   {
      pFgAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pFgAdcDebugInfo->uCurIdx = 0;
   }

   DALSYS_SyncLeave(pFgAdcDebugInfo->hSync);

   pFgAdcDebugInfo->aLog[uCurIdx].uTimestamp = FgAdc_GetTimetick(pFgAdcDebugInfo);
   pFgAdcDebugInfo->aLog[uCurIdx].eEvent = FGADC_LOG_EVENT_ERROR;
   pFgAdcDebugInfo->aLog[uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pFgAdcDebugInfo->aLog[uCurIdx].pszErrorMsg = pszErrorMsg;

   if (bFatalError == TRUE)
   {
      MSG_SPRINTF_1(MSG_SSID_ADC, MSG_LEGACY_FATAL, "%s", pszErrorMsg);

      DALSYS_LogEvent(pFgAdcDebugInfo->deviceId,
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      pszErrorMsg);
   }
   else
   {
      MSG_SPRINTF_1(MSG_SSID_ADC, MSG_LEGACY_ERROR, "%s", pszErrorMsg);

      DALSYS_LogEvent(pFgAdcDebugInfo->deviceId,
                      DALSYS_LOGEVENT_ERROR,
                      pszErrorMsg);
   }
}

/*======================================================================

  FUNCTION        FgAdc_LogDumpRegisters

  DESCRIPTION     This function dumps the FGADC registers

  DEPENDENCIES    None

  PARAMETERS      pFgAdcDebugInfo [in] debug structure
                  piFgAdcHal      [in]

  RETURN VALUE    None

  SIDE EFFECTS    This function causes SPMI transfers to occur

======================================================================*/
void
FgAdc_LogDumpRegisters(
   FgAdcDebugInfoType *pFgAdcDebugInfo,
   FgAdcHalInterfaceType *piFgAdcHal
   )
{
   FgAdcHalRegDumpType *pFgAdcRegDump;
   uint32 uReg;

   pFgAdcRegDump = &pFgAdcDebugInfo->aRegDump[pFgAdcDebugInfo->uRegDumpCurIdx];

   (void)FgAdcHalDumpRegisters(piFgAdcHal, pFgAdcRegDump);

   MSG(MSG_SSID_ADC, MSG_LEGACY_ERROR, "FGADC register dump");

   for (uReg = 0; uReg < FGADC_HAL_NUM_REGISTERS_TO_DUMP; uReg++)
   {
      MSG_2(MSG_SSID_ADC,
            MSG_LEGACY_ERROR,
            "Offset = %u, Contents = %u",
            pFgAdcRegDump->aFgAdcReg[uReg].ucOffset,
            pFgAdcRegDump->aFgAdcReg[uReg].ucVal);
   }

#ifdef FGADC_DUMP_REGISTERS_AFTER_READS
   if (pFgAdcDebugInfo->uRegDumpCurIdx < FGADC_REG_DUMP_MAX - 1)
   {
      pFgAdcDebugInfo->uRegDumpCurIdx++;
   }
   else
   {
      pFgAdcDebugInfo->uRegDumpCurIdx = 0;
   }
#endif
}

/*======================================================================

  FUNCTION        FgAdc_LogInit

  DESCRIPTION     This function initializes logging

  DEPENDENCIES    None

  PARAMETERS      pFgAdcDebugInfo [in] debug structure
                  deviceId       [in] DAL device ID

  RETURN VALUE    DAL_SUCCESS or an error

  SIDE EFFECTS    None

======================================================================*/
DALResult
FgAdc_LogInit(
   FgAdcDebugInfoType *pFgAdcDebugInfo,
   DALDEVICEID deviceId
   )
{
   DALResult result;
   pFgAdcDebugInfo->uCurIdx = 0;
   pFgAdcDebugInfo->uRegDumpCurIdx = 0;
   pFgAdcDebugInfo->deviceId = deviceId;

   result = DalTimetick_Attach("SystemTimer", &pFgAdcDebugInfo->phTimetickDev);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(deviceId,
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      "FGADC: Failed to attach to Timetick device");
      return result;
   }

   /* Interrupts can happen asynchronously so a lock is required */
   result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
                              &pFgAdcDebugInfo->hSync,
                              &pFgAdcDebugInfo->syncObject);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(deviceId,
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      "FGADC: Failed to create log sync");
      return result;
   }

   return DAL_SUCCESS;
}

#endif /* #ifndef ADC_LOG_DISABLED */

