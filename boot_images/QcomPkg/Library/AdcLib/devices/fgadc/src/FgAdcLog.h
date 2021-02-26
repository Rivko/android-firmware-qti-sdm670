#ifndef __FGADCLOG_H__
#define __FGADCLOG_H__
/*============================================================================
  @file FgAdcLog.h

  Logging for the FgAdc


                Copyright (c) 2016 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/fgadc/src/FgAdcLog.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DDIAdcDevice.h"
#include "DALFramework.h"
#include "FgAdcHal.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#ifdef ADC_LOG_DISABLED

#define FgAdc_LogConversionRequest(x,y)
#define FgAdc_LogConversionResult(x,y,z)
#define FgAdc_LogError(x,y,z)
#define FgAdc_LogDumpRegisters(x,y)

#else

#define FGADC_LOG_MAX 30

#ifdef FGADC_DUMP_REGISTERS_AFTER_READS
#define FGADC_REG_DUMP_MAX 5
#else
#define FGADC_REG_DUMP_MAX 1
#endif

#endif

/*-------------------------------------------------------------------------
 * Type Declarations
 * ----------------------------------------------------------------------*/
#ifndef ADC_LOG_DISABLED

typedef struct
{
   enum
   {
      FGADC_LOG_EVENT_CONVERSION_REQUEST = 0,
      FGADC_LOG_EVENT_INTERRUPT,
      FGADC_LOG_EVENT_EOC,
      FGADC_LOG_EVENT_RESULT,
      FGADC_LOG_EVENT_ERROR
   } eEvent;
   uint64 uTimestamp;
   uint32 uChannelIdx;
   AdcDeviceResultType result;
   const char *pszErrorMsg;
} FgAdcRegDataLogEntryType;

typedef struct
{
   FgAdcRegDataLogEntryType aLog[FGADC_LOG_MAX];
   FgAdcHalRegDumpType aRegDump[FGADC_REG_DUMP_MAX];
   DalDeviceHandle *phTimetickDev;
   DALSYSSyncHandle hSync;
   DALSYS_SYNC_OBJECT(syncObject);
   uint32 uCurIdx;
   uint32 uRegDumpCurIdx;
   DALDEVICEID deviceId;
} FgAdcDebugInfoType;

#else

typedef void *FgAdcDebugInfoType;

static __inline DALResult
FgAdc_LogInit(FgAdcDebugInfoType *pFgAdcDebugInfo, DALDEVICEID deviceId)
{
   return DAL_SUCCESS;
}

#endif

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
#ifndef ADC_LOG_DISABLED

DALResult FgAdc_LogInit(FgAdcDebugInfoType *pFgAdcDebugInfo, DALDEVICEID deviceId);
void FgAdc_LogConversionRequest(FgAdcDebugInfoType *pFgAdcDebugInfo, uint32 uChannelIdx);
void FgAdc_LogConversionResult(FgAdcDebugInfoType *pFgAdcDebugInfo, const char *pszChanName, const AdcDeviceResultType *pResult);
void FgAdc_LogError(FgAdcDebugInfoType *pFgAdcDebugInfo, const char *pszErrorMsg, DALBOOL bFatalError);
void FgAdc_LogDumpRegisters(FgAdcDebugInfoType *pFgAdcDebugInfo, FgAdcHalInterfaceType *piFgAdcHal);

#endif

#endif /* #ifndef __FGADCLOG_H__ */

