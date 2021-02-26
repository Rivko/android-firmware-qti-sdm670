#ifndef DDISTMTRACE_H
#define DDISTMTRACE_H
/* ==========================================================================

                   STM Trace Driver Interface

 @file DDISTMTrace.h

   This file contains the definitions of the constants, types, and
   interfaces that comprise the interface to the STM Trace
   driver.

               Copyright (c) 2011-2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
//========================================================================
//
//                             Edit History
//
// $Header: //components/rel/core.qdsp6/2.1.c4/debugtrace/stm/dal/inc/DDISTMTrace.h#1 $
//
// when       who     what, where, why
// --------   ---     ----------------------------------------------------
// 05/26/11   ts     Initial revision.
//
//========================================================================

#include "DalDevice.h"
#include "DALDeviceId.h"

#define DALSTMTRACE_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

typedef uint32 STMTracePortType;

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

typedef enum
{
   TRACE_G_DMTS = 0x0,
   TRACE_G_DM,
   TRACE_G_DTS,
   TRACE_G_D,
   TRACE_G_FLAGTS,
   TRACE_G_FLAG,
   TRACE_G_TRIGTS,
   TRACE_G_TRIG,
   TRACE_I_DMTS,
   TRACE_I_DM,
   TRACE_I_DTS,
   TRACE_I_D,
   TRACE_I_FLAGTS,
   TRACE_I_FLAG,
   TRACE_I_TRIGTS,
   TRACE_I_TRIG,
   _PLACEHOLDER_STMDMATraceType = 0x7fffffff
}  eSTMDMATraceType;

typedef uint32 STMDMATraceType;

typedef struct DalSTMTrace DalSTMTrace;
struct DalSTMTrace
{
   struct DalDevice DalDevice;
   DALResult (*NewPort)(uint32 ClientId, DalDeviceHandle * _h, STMTracePortType * uPort, STMTracePortType *uPortDMA);
   void (*FreePort)(uint32 ClientId, DalDeviceHandle * _h, STMTracePortType  uPort);
};

typedef struct DalSTMTraceHandle DalSTMTraceHandle;
struct DalSTMTraceHandle
{
   uint32 dwDalHandleId;
   const DalSTMTrace * pVtbl;
   void * pClientCtxt;
};

#define DAL_STMTraceDeviceAttach(DevId,hDalDevice)\
        DAL_StringDeviceAttachEx(NULL,DevId,DALSTMTRACE_INTERFACE_VERSION,hDalDevice)


static __inline DALResult  DalSTMTrace_NewPort(DalDeviceHandle * _h, STMTracePortType * uPort)
{
   return ((DalSTMTraceHandle *)_h)->pVtbl->NewPort(DALDEVICE_LOCAL_CLIENTID, _h, uPort, NULL);
}

static __inline DALResult  DalSTMTrace_NewPortDMA(DalDeviceHandle * _h, STMTracePortType * uPort, STMTracePortType *uPortDMA)
{
   return ((DalSTMTraceHandle *)_h)->pVtbl->NewPort(DALDEVICE_LOCAL_CLIENTID, _h, uPort, uPortDMA);
}

static __inline void  DalSTMTrace_FreePort(DalDeviceHandle * _h, STMTracePortType uPort)
{
   ((DalSTMTraceHandle *)_h)->pVtbl->FreePort(DALDEVICE_LOCAL_CLIENTID, _h, uPort);
}

static __inline uint32 STMSP_BAMDMA_OFFSET(STMTracePortType uPortDMA, STMDMATraceType eType)
{
   return ( (uPortDMA & ~(0x0F<<13)) | ((eType&0x0F)<<13) );
}

/* **** For SW workaround usage only**** */
#if (DAL_CONFIG_ARCH & DAL_CONFIG_ARCH_ARM)

__asm static void incr2_word_write(STMTracePortType uPort, uint32 * data)
{
   LDMIA r1!,{r2-r3}
   STMIA r0, {r2-r3}
   BX lr
}



static __inline void DalSTMTrace_DataBlk64(DalDeviceHandle * _h,
   STMTracePortType uPort, STMTraceDataType uType, uint32 * uData)
{
   incr2_word_write(((uPort)|(0x98&(~uType))), uData);
}

__asm static void incr4_word_write(STMTracePortType uPort, uint32 * data)
{
   PUSH {r4-r5}
   LDMIA r1!,{r2-r5}
   STMIA r0, {r2-r5}
   POP {r4-r5}
   BX lr
}

static __inline void DalSTMTrace_DataBlk128(DalDeviceHandle * _h,
   STMTracePortType uPort, STMTraceDataType uType, uint32 * uData)
{
   incr4_word_write(((uPort)|(0x98&(~uType))), uData);
}

#endif //(DAL_CONFIG_ARCH & DAL_CONFIG_ARCH_ARM)

static __inline void DalSTMTrace_Data32(DalDeviceHandle * _h,
   STMTracePortType uPort, STMTraceDataType uType, uint32 uData)
{
   uType |= TRACE_DATA_GUARANTEED;
   *(volatile uint32 *)((uPort)|(0x98&(~uType)))=uData;
}

static __inline void DalSTMTrace_Data16(DalDeviceHandle * _h,
   STMTracePortType uPort, STMTraceDataType uType, uint16 uData)
{
   uType |= TRACE_DATA_GUARANTEED;
   *(volatile uint16 *)((uPort)|(0x98&(~uType)))=uData;
}

static __inline void DalSTMTrace_Data8(DalDeviceHandle * _h,
   STMTracePortType uPort, STMTraceDataType uType, uint8 uData)
{
   uType |= TRACE_DATA_GUARANTEED;
   *(volatile uint8 *)((uPort)|(0x98&(~uType)))=uData;
}

static __inline void DalSTMTrace_Flag(DalDeviceHandle * _h,
   STMTracePortType uPort, STMTraceFlagType uType)
{
   uType |= TRACE_DATA_GUARANTEED;
   *(volatile uint32 *)((uPort)|(0xE8&(~uType)))=0;
}

static __inline void DalSTMTrace_Trigger(DalDeviceHandle * _h,
   STMTracePortType uPort, STMTraceTriggerType uType)
{
   uType |= TRACE_DATA_GUARANTEED;
   *(volatile uint32 *)((uPort)|(0xF8&(~uType)))=0;
}

#endif /* DDISTMTRACE_H */

