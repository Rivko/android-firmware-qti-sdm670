/*==============================================================================
@file  DALSysLogEvent.c

DALSYS Event logging and error handling

        Copyright © 2013 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
==============================================================================*/
#include "DALSys.h"
#include "DALStdDef.h"
#include "DALSysCmn.h"
#include "DALFramework.h"
#include <stdarg.h>
#include "qurt_atomic_ops.h"
#include "qurt.h"

#ifdef DALSYS_LOG_BUFFER_SIZE_POWER

#ifdef DAL_ERR_LOG
#include "err.h"
#endif
#include "assert.h"

#define INVALID_ENTRY_IDX  0xffffffff
#define LOG_ENTRY_SIZE     32
#define MAX_ARG_CNT        5
#define LOG_ENTRY_CNT      (1 << DALSYS_LOG_BUFFER_SIZE_POWER)
#define LOG_ENTRY_MASK     (LOG_ENTRY_CNT - 1)
#define MAX_MSG_FORMAT_LEN 1024
#define ENTRY_RESERVED     1
#define ENTRY_FREE         0

// DAL log entry has 32-bytes and is 32-byte aligned to reduce cache misses
typedef struct {
   uint8 eventType;
   uint8 argCnt;
   uint16 timestampHi; // timestamps are in micro-seconds
   uint32 timestampLo;
   const char *pszFmt;
   uint32 argList[MAX_ARG_CNT];
} LogEntry __attribute__((aligned(32)));

// the variables provide information about the DAL log implementation
// used by the DALLog.cmm script
unsigned int PD_VAR(DALSYSLogBufferLen) = LOG_ENTRY_CNT;
unsigned int PD_VAR(DALSYSLogMaxArgCnt) = MAX_ARG_CNT;
LogEntry PD_VAR(DALSYSLogBuffer)[LOG_ENTRY_CNT];

// below needs to be masked against log buffer size to obtain 
// the index of the last log output
// this is left unmask as it cannot be simultaneousl incremented 
// and masked atomically
unsigned int PD_VAR(DALSYSLogUnmaskIdx) = ~0;

// indicate if an entry is reserved and another thread is still writing
// to this
static unsigned int logEntryBusyStatus[LOG_ENTRY_CNT];

// reserve a log entry atomically
static unsigned int logEntryReserve(void)
{
   unsigned int entryIdx;
   unsigned int isFree;
   int retryCount = LOG_ENTRY_CNT;

   do
   {
      entryIdx = (qurt_atomic_inc_return(&PD_VAR(DALSYSLogUnmaskIdx)) & LOG_ENTRY_MASK);
      isFree = qurt_atomic_compare_and_set(&logEntryBusyStatus[entryIdx], ENTRY_FREE, ENTRY_RESERVED);
      retryCount--;
   }
   while (!isFree && retryCount);

   if (isFree)
      return entryIdx;
   else
      return INVALID_ENTRY_IDX;
}

// release a log entry atomically
static void logEntryRelease(unsigned int entryIdx)
{
   qurt_atomic_set(&logEntryBusyStatus[entryIdx], ENTRY_FREE);
}

// get time in micro-seconds from DAL timetick driver
static uint64 getTimeInUS(void)
{
   return (uint64)qurt_timer_timetick_to_us(qurt_sysclock_get_hw_ticks());
}

// encode log to DAL log format
static void logEntryEncode(unsigned int entryIdx,
      uint32 eventType, uint64 timestamp, const char * pszFmt, 
      uint32 argCnt, va_list argList)
{
   int i;
   PD_VAR(DALSYSLogBuffer)[entryIdx].eventType = (uint8)eventType;
   PD_VAR(DALSYSLogBuffer)[entryIdx].argCnt = (uint8)argCnt;
   PD_VAR(DALSYSLogBuffer)[entryIdx].timestampLo = (uint32)timestamp;
   PD_VAR(DALSYSLogBuffer)[entryIdx].timestampHi = (uint16)(timestamp >> 32);
   PD_VAR(DALSYSLogBuffer)[entryIdx].pszFmt = pszFmt;
   for (i = 0; i < argCnt; i++)
   {
      PD_VAR(DALSYSLogBuffer)[entryIdx].argList[i] = va_arg(argList, uint32);
   }
}

// count the number of arguments in log message
// For simplicity, all arguments are assumed to be 32-bits,
// all arguments are in the first MAX_MSG_FORMAT_LEN characters 
// and there are only MAX_ARG_CNT arguments
static unsigned int getArgCnt(const char * cp)
{
   unsigned int strCnt = 0;
   unsigned int argCnt = 0;

   while ((*cp) != '\0' && strCnt < MAX_MSG_FORMAT_LEN && 
         argCnt < MAX_ARG_CNT)
   {
      if ((*(cp++)) == '%')
      {
         // % is a placeholder for an argument
         // however %% is used for a literal % output
         if ((*cp) != '%') argCnt++;
      }
      strCnt++;
   }
   return argCnt;
}

// create DAL log event
void
DALSYS_LogEvent(DALDEVICEID DeviceId, uint32 dwLogEventType, 
      const char * pszFmt, ... )
{
   if (dwLogEventType <= DALSYS_LOG_LEVEL)
   {
      unsigned int entryIdx;
      unsigned int argLen = getArgCnt(pszFmt);
      uint64 timestamp = getTimeInUS();
      va_list argList;
      va_start(argList, pszFmt);

      entryIdx = logEntryReserve();
      if (INVALID_ENTRY_IDX != entryIdx)
      {
         logEntryEncode(entryIdx, dwLogEventType, timestamp, pszFmt,
            argLen, argList);
         logEntryRelease(entryIdx);
      }

      va_end(argList);
   }

   if (dwLogEventType == DALSYS_LOGEVENT_FATAL_ERROR)
   {
      if ((PD_VAR(DALSYSLogBufferLen) != LOG_ENTRY_CNT) ||
          (PD_VAR(DALSYSLogMaxArgCnt) != MAX_ARG_CNT))
      {
         // the useless comparisions and assignments
         // ensure above variables would not be stripped by linker so 
         // they are accessible by the DAL log CMM script
          PD_VAR(DALSYSLogMaxArgCnt) = MAX_ARG_CNT;
      }

#ifdef DAL_ERR_LOG
      static const char * logEvTbl[5] = {"NONE", "FATAL-ERROR", "ERROR", 
         "WARNING", "INFO"};
      ERR_FATAL("DALLOG Device [0x%0x] Type [%s] Log [%s]", DeviceId,
                logEvTbl[dwLogEventType], pszFmt);
#endif
      ASSERT(0);
   }
}

DALSYS_LogEventFncPtr
DALSYS_SetLogCfg(uint32 dwMaxLogLevel, DALSYS_LogEventFncPtr DALSysLogFcn) 
{
   // With the introduction of DALSYS native logging, it no longer makes
   // sense for users to set their own logging function.  The user logging
   // function may only collect logs relevant to the user.  As well the user
   // logging function may have latency or deadlock issues.
   // Furthermore the log verbosity level can no longer be set
   // at run-time so that DAL team can control the proper log level. 
   // The log level is configured with DALSYS_LOG_LEVEL constant
   // in core\dal\sys\build\SConscript
   DALSYS_LOG_ERROR_EVENT("unsupported");
   return NULL;
}
#else 

// DALSYS_LOG_BUFFER_SIZE_POWER not defined

void
DALSYS_LogEvent(DALDEVICEID DeviceId, uint32 dwLogEventType, 
      const char * pszFmt, ... )
{
   return;
}

DALSYS_LogEventFncPtr
DALSYS_SetLogCfg(uint32 dwMaxLogLevel, DALSYS_LogEventFncPtr DALSysLogFcn) 
{
   return NULL;
}

#endif
