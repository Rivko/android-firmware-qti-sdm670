#ifndef ADC_LOG_DISABLED
/*============================================================================
  FILE:         VAdcLog.c

  OVERVIEW:     Logging for the VAdc

  DEPENDENCIES: None

                Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/*============================================================================
  EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.  Please
  use ISO format for dates.

  $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/vadc/src/VAdcLog.c#1 $$DateTime: 2017/09/18 09:26:13 $$Author: pwbldsvc $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2015-01-14  jjo  Log channel name.
  2014-07-07  jjo  Use DDITimetick.
  2013-06-19  jjo  Support for multiple VADCs.
  2013-02-07  jjo  Added F3 logging.
  2012-07-09  jjo  Initial version.

============================================================================*/
/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/
#include "VAdcLog.h"
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
VAdc_GetTimetick(const VAdcDebugInfoType *pVAdcDebugInfo)
{
   DALResult result;
   DalTimetickTime64Type u64Tick;

   result = DalTimetick_GetTimetick64(pVAdcDebugInfo->phTimetickDev, &u64Tick);
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

  FUNCTION        VAdc_LogConversionRequest

  DESCRIPTION     This function logs a conversion request

  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure
                  uChannelIdx    [in] the channel index

  RETURN VALUE    None


  SIDE EFFECTS    None

======================================================================*/
void
VAdc_LogConversionRequest(
   VAdcDebugInfoType *pVAdcDebugInfo,
   uint32 uChannelIdx
   )
{
   uint32 uCurIdx;

   DALSYS_SyncEnter(pVAdcDebugInfo->hSync);

   uCurIdx = pVAdcDebugInfo->uCurIdx;
   if (uCurIdx < VADC_LOG_MAX - 1)
   {
      pVAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uCurIdx = 0;
   }

   DALSYS_SyncLeave(pVAdcDebugInfo->hSync);

   pVAdcDebugInfo->aLog[uCurIdx].uTimestamp = VAdc_GetTimetick(pVAdcDebugInfo);
   pVAdcDebugInfo->aLog[uCurIdx].eEvent = VADC_LOG_EVENT_CONVERSION_REQUEST;
   pVAdcDebugInfo->aLog[uCurIdx].uChannelIdx = uChannelIdx;
   pVAdcDebugInfo->aLog[uCurIdx].pszErrorMsg = NULL;

   MSG_1(MSG_SSID_ADC, MSG_LEGACY_MED, "VADC conv req: index %u", uChannelIdx);
}

/*======================================================================

  FUNCTION        VAdc_LogEOC

  DESCRIPTION     This function logs the internal conversion complete callback

  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
void
VAdc_LogEOC(VAdcDebugInfoType *pVAdcDebugInfo)
{
   uint32 uCurIdx;

   DALSYS_SyncEnter(pVAdcDebugInfo->hSync);

   uCurIdx = pVAdcDebugInfo->uCurIdx;
   if (uCurIdx < VADC_LOG_MAX - 1)
   {
      pVAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uCurIdx = 0;
   }

   DALSYS_SyncLeave(pVAdcDebugInfo->hSync);

   pVAdcDebugInfo->aLog[uCurIdx].uTimestamp = VAdc_GetTimetick(pVAdcDebugInfo);
   pVAdcDebugInfo->aLog[uCurIdx].eEvent = VADC_LOG_EVENT_EOC;
   pVAdcDebugInfo->aLog[uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[uCurIdx].pszErrorMsg = NULL;

   MSG(MSG_SSID_ADC, MSG_LEGACY_MED, "VADC EOC");
}

/*======================================================================

  FUNCTION        VAdc_LogConversionResult

  DESCRIPTION     This function logs the conversion result

  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure
                  pszChanName    [in] channel name
                  pResult        [in] the AdcDeviceResultType result data

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
void
VAdc_LogConversionResult(
   VAdcDebugInfoType *pVAdcDebugInfo,
   const char *pszChanName,
   const AdcDeviceResultType *pResult
   )
{
   uint32 uCurIdx;

   DALSYS_SyncEnter(pVAdcDebugInfo->hSync);

   uCurIdx = pVAdcDebugInfo->uCurIdx;
   if (uCurIdx < VADC_LOG_MAX - 1)
   {
      pVAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uCurIdx = 0;
   }

   DALSYS_SyncLeave(pVAdcDebugInfo->hSync);

   pVAdcDebugInfo->aLog[uCurIdx].result = *pResult;
   pVAdcDebugInfo->aLog[uCurIdx].uTimestamp = VAdc_GetTimetick(pVAdcDebugInfo);
   pVAdcDebugInfo->aLog[uCurIdx].eEvent = VADC_LOG_EVENT_RESULT;
   pVAdcDebugInfo->aLog[uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[uCurIdx].pszErrorMsg = NULL;

   MSG_SPRINTF_6(MSG_SSID_ADC,
                 MSG_LEGACY_HIGH,
                 "VADC conv result for %s: eStatus %i, nPhysical %i, uPercent %u, uMicrovolts %u, uCode %u",
                 pszChanName,
                 pResult->eStatus,
                 pResult->nPhysical,
                 pResult->uPercent,
                 pResult->uMicrovolts,
                 pResult->uCode);
}

/*======================================================================

  FUNCTION        VAdc_LogError

  DESCRIPTION     This function logs an error

  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure
                  pszErrorMsg [in]
                  bFatalError [in]

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
void
VAdc_LogError(
   VAdcDebugInfoType *pVAdcDebugInfo,
   const char *pszErrorMsg,
   DALBOOL bFatalError
   )
{
   uint32 uCurIdx;

   DALSYS_SyncEnter(pVAdcDebugInfo->hSync);

   uCurIdx = pVAdcDebugInfo->uCurIdx;
   if (uCurIdx < VADC_LOG_MAX - 1)
   {
      pVAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uCurIdx = 0;
   }

   DALSYS_SyncLeave(pVAdcDebugInfo->hSync);

   pVAdcDebugInfo->aLog[uCurIdx].uTimestamp = VAdc_GetTimetick(pVAdcDebugInfo);
   pVAdcDebugInfo->aLog[uCurIdx].eEvent = VADC_LOG_EVENT_ERROR;
   pVAdcDebugInfo->aLog[uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[uCurIdx].pszErrorMsg = pszErrorMsg;

   if (bFatalError == TRUE)
   {
      MSG_SPRINTF_1(MSG_SSID_ADC, MSG_LEGACY_FATAL, "%s", pszErrorMsg);

      DALSYS_LogEvent(pVAdcDebugInfo->deviceId,
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      pszErrorMsg);
   }
   else
   {
      MSG_SPRINTF_1(MSG_SSID_ADC, MSG_LEGACY_ERROR, "%s", pszErrorMsg);

      DALSYS_LogEvent(pVAdcDebugInfo->deviceId,
                      DALSYS_LOGEVENT_ERROR,
                      pszErrorMsg);
   }
}

/*======================================================================

  FUNCTION        VAdc_LogInterrupt

  DESCRIPTION     This function logs an interrupt

  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure

  RETURN VALUE    None

  SIDE EFFECTS    None

======================================================================*/
void
VAdc_LogInterrupt(VAdcDebugInfoType *pVAdcDebugInfo)
{
   uint32 uCurIdx;

   DALSYS_SyncEnter(pVAdcDebugInfo->hSync);

   uCurIdx = pVAdcDebugInfo->uCurIdx;
   if (uCurIdx < VADC_LOG_MAX - 1)
   {
      pVAdcDebugInfo->uCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uCurIdx = 0;
   }

   DALSYS_SyncLeave(pVAdcDebugInfo->hSync);

   pVAdcDebugInfo->aLog[uCurIdx].uTimestamp = VAdc_GetTimetick(pVAdcDebugInfo);
   pVAdcDebugInfo->aLog[uCurIdx].eEvent = VADC_LOG_EVENT_INTERRUPT;
   pVAdcDebugInfo->aLog[uCurIdx].uChannelIdx = 0xFFFFFFFF;
   pVAdcDebugInfo->aLog[uCurIdx].pszErrorMsg = NULL;

   MSG(MSG_SSID_ADC, MSG_LEGACY_MED, "VADC EOC Interrupt");
}

/*======================================================================

  FUNCTION        VAdc_LogDumpRegisters

  DESCRIPTION     This function dumps the VADC registers

  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure
                  piVAdcHal      [in]

  RETURN VALUE    None

  SIDE EFFECTS    This function causes SPMI transfers to occur

======================================================================*/
void
VAdc_LogDumpRegisters(
   VAdcDebugInfoType *pVAdcDebugInfo,
   VAdcHalInterfaceType *piVAdcHal
   )
{
   VAdcHalRegDumpType *pVAdcRegDump;
   uint32 uReg;

   pVAdcRegDump = &pVAdcDebugInfo->aRegDump[pVAdcDebugInfo->uRegDumpCurIdx];

   (void)VAdcHalDumpRegisters(piVAdcHal, pVAdcRegDump);

   MSG(MSG_SSID_ADC, MSG_LEGACY_ERROR, "VADC register dump");

   for (uReg = 0; uReg < VADC_HAL_NUM_REGISTERS_TO_DUMP; uReg++)
   {
      MSG_2(MSG_SSID_ADC,
            MSG_LEGACY_ERROR,
            "Offset = %u, Contents = %u",
            pVAdcRegDump->aVAdcReg[uReg].ucOffset,
            pVAdcRegDump->aVAdcReg[uReg].ucVal);
   }

#ifdef VADC_DUMP_REGISTERS_AFTER_READS
   if (pVAdcDebugInfo->uRegDumpCurIdx < VADC_REG_DUMP_MAX - 1)
   {
      pVAdcDebugInfo->uRegDumpCurIdx++;
   }
   else
   {
      pVAdcDebugInfo->uRegDumpCurIdx = 0;
   }
#endif
}

/*======================================================================

  FUNCTION        VAdc_LogInit

  DESCRIPTION     This function initializes logging

  DEPENDENCIES    None

  PARAMETERS      pVAdcDebugInfo [in] debug structure
                  deviceId       [in] DAL device ID

  RETURN VALUE    DAL_SUCCESS or an error

  SIDE EFFECTS    None

======================================================================*/
DALResult
VAdc_LogInit(
   VAdcDebugInfoType *pVAdcDebugInfo,
   DALDEVICEID deviceId
   )
{
   DALResult result;
   pVAdcDebugInfo->uCurIdx = 0;
   pVAdcDebugInfo->uRegDumpCurIdx = 0;
   pVAdcDebugInfo->deviceId = deviceId;

   result = DalTimetick_Attach("SystemTimer", &pVAdcDebugInfo->phTimetickDev);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(deviceId,
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      "VADC: Failed to attach to Timetick device");
      return result;
   }

   /* Interrupts can happen asynchronously so a lock is required */
   result = DALSYS_SyncCreate(DALSYS_SYNC_ATTR_RESOURCE_INTERRUPT,
                              &pVAdcDebugInfo->hSync,
                              &pVAdcDebugInfo->syncObject);
   if (result != DAL_SUCCESS)
   {
      DALSYS_LogEvent(deviceId,
                      DALSYS_LOGEVENT_FATAL_ERROR,
                      "VADC: Failed to create log sync");
      return result;
   }

   return DAL_SUCCESS;
}

#endif /* #ifndef ADC_LOG_DISABLED */

