#ifndef __VADCLOG_H__
#define __VADCLOG_H__
/*============================================================================
  @file VAdcLog.h

  Logging for the VAdc


                Copyright (c) 2012-2015 Qualcomm Technologies, Inc.
                All Rights Reserved.
                Qualcomm Technologies Proprietary and Confidential.
============================================================================*/
/* $Header: //components/rel/boot.xf/2.1/QcomPkg/Library/AdcLib/devices/vadc/src/VAdcLog.h#1 $ */

/*-------------------------------------------------------------------------
 * Include Files
 * ----------------------------------------------------------------------*/
#include "DDIAdcDevice.h"
#include "DALFramework.h"
#include "VAdcHal.h"

/*-------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * ----------------------------------------------------------------------*/
#ifdef ADC_LOG_DISABLED

#define VAdc_LogConversionRequest(x,y)
#define VAdc_LogEOC(x)
#define VAdc_LogConversionResult(x,y,z)
#define VAdc_LogError(x,y,z)
#define VAdc_LogInterrupt(x)
#define VAdc_LogDumpRegisters(x,y)

#else

#define VADC_LOG_MAX 30

#ifdef VADC_DUMP_REGISTERS_AFTER_READS
#define VADC_REG_DUMP_MAX 5
#else
#define VADC_REG_DUMP_MAX 1
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
      VADC_LOG_EVENT_CONVERSION_REQUEST = 0,
      VADC_LOG_EVENT_INTERRUPT,
      VADC_LOG_EVENT_EOC,
      VADC_LOG_EVENT_RESULT,
      VADC_LOG_EVENT_ERROR
   } eEvent;
   uint64 uTimestamp;
   uint32 uChannelIdx;
   AdcDeviceResultType result;
   const char *pszErrorMsg;
} VAdcRegDataLogEntryType;

typedef struct
{
   VAdcRegDataLogEntryType aLog[VADC_LOG_MAX];
   VAdcHalRegDumpType aRegDump[VADC_REG_DUMP_MAX];
   DalDeviceHandle *phTimetickDev;
   DALSYSSyncHandle hSync;
   DALSYS_SYNC_OBJECT(syncObject);
   uint32 uCurIdx;
   uint32 uRegDumpCurIdx;
   DALDEVICEID deviceId;
} VAdcDebugInfoType;

#else

typedef void *VAdcDebugInfoType;

static __inline DALResult
VAdc_LogInit(VAdcDebugInfoType *pVAdcDebugInfo, DALDEVICEID deviceId)
{
   return DAL_SUCCESS;
}

#endif

/*-------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ----------------------------------------------------------------------*/
#ifndef ADC_LOG_DISABLED

DALResult VAdc_LogInit(VAdcDebugInfoType *pVAdcDebugInfo, DALDEVICEID deviceId);
void VAdc_LogConversionRequest(VAdcDebugInfoType *pVAdcDebugInfo, uint32 uChannelIdx);
void VAdc_LogEOC(VAdcDebugInfoType *pVAdcDebugInfo);
void VAdc_LogConversionResult(VAdcDebugInfoType *pVAdcDebugInfo, const char *pszChanName, const AdcDeviceResultType *pResult);
void VAdc_LogError(VAdcDebugInfoType *pVAdcDebugInfo, const char *pszErrorMsg, DALBOOL bFatalError);
void VAdc_LogInterrupt(VAdcDebugInfoType *pVAdcDebugInfo);
void VAdc_LogDumpRegisters(VAdcDebugInfoType *pVAdcDebugInfo, VAdcHalInterfaceType *piVAdcHal);

#endif

#endif /* #ifndef __VADCLOG_H__ */

