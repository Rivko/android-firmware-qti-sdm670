/*=============================================================================

FILE:         QdssStmTrace.h

DESCRIPTION:  

================================================================================
            Copyright (c) 2015 Qualcomm Technologies, Inc.
                         All Rights Reserved.
          Qualcomm Technologies Proprietary and Confidential
==============================================================================*/

#ifndef QDSSSTMTRACE_H_
#define QDSSSTMTRACE_H_



#include "HALcomdef.h"


typedef UINTN STMPortAddrType;

typedef enum
{
   TRACE_DATA_NONETYPE = 0x00,    /* Trace packet not gauranteed, without ts & mark */
   TRACE_DATA_GUARANTEED = 0x80,  /* Trace packet gauranteed */
   TRACE_DATA_MARKED = 0x10,      /* Trace packet marked */
   TRACE_DATA_TIMESTAMPED = 0x08, /* Trace packet timestamped */
   _PLACEHOLDER_STMTraceDataType = 0x7f
} eSTMTraceDataType;

typedef uint8 STMTraceDataType;

typedef enum
{
   TRACE_FLAG_NONETYPE = 0x00,    /* Trace packet not gauranteed, without ts & mark */
   TRACE_FLAG_GUARANTEED = 0x80,  /* Trace packet gauranteed */
   TRACE_FLAG_TIMESTAMPED = 0x08, /* Trace packet timestamped */
   _PLACEHOLDER_STMTraceFlagType = 0x7f
} eSTMTraceFlagType;

typedef uint8 STMTraceFlagType;

typedef enum
{
   TRACE_TRIG_NONETYPE = 0x00,    /* Trace packet not gauranteed, without ts & mark */
   TRACE_TRIG_GUARANTEED = 0x80,  /* Trace packet gauranteed */
   TRACE_TRIG_TIMESTAMPED = 0x08, /* Trace packet timestamped */
   _PLACEHOLDER_STMTraceTriggerType = 0x7f
} eSTMTraceTriggerType;

typedef uint8 STMTraceTriggerType;


int  QdssStmTrace_NewPort(STMPortAddrType *uPort);
void QdssStmTrace_FreePort(STMPortAddrType uPort);
void QdssStmTrace_Data32(STMPortAddrType uPort, STMTraceDataType uType, uint32 uData);
void QdssStmTrace_Data16(STMPortAddrType uPort, STMTraceDataType uType, uint16 uData);
void QdssStmTrace_Data8(STMPortAddrType uPort, STMTraceDataType uType, uint8 uData);
void QdssStmTrace_Flag(STMPortAddrType uPort, STMTraceFlagType uType);
void QdssStmTrace_Trigger(STMPortAddrType uPort, STMTraceTriggerType uType);

#endif  /* QDSSSTMTRACE_H_ */
