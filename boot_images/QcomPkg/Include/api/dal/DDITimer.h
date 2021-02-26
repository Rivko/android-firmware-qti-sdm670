/*==============================================================================
  Copyright (c) 2013-2014 Qualcomm Technologies, Inc.
           All Rights Reserved.
  Qualcomm Confidential and Proprietary

==============================================================================*/
#ifndef __DALITIMER_H__
#define __DALITIMER_H__

#include "DalDevice.h"

#define DALTIMER_INTERFACE_VERSION DALINTERFACE_VERSION(1,1)

// Use these when configuring timer using SYS_TICKs instead of micro-seconds
// NOTE: the TICK extension is currently not available for all SW platforms
//       please check for availability prior to use.
#define SYS_TICK_TIME_FLAG       0x80000000
#define SYS_TICK_TIME( t ) ( (t) | SYS_TICK_TIME_FLAG )

typedef struct DalTimer DalTimer;
struct DalTimer
{
   struct DalDevice DalDevice;
   DALResult (*Register)(DalDeviceHandle * _h, const DALSYSEventHandle  hEvent, uint32  time);
   DALResult (*UnRegister)(DalDeviceHandle * _h, const DALSYSEventHandle  hEvent);
   DALResult (*GetTimerCount)(DalDeviceHandle * _h, void * timer_count, uint32 size);
};

typedef struct DalTimerHandle DalTimerHandle;
struct DalTimerHandle
{
   uint32 dwDalHandleId;
   const DalTimer * pVtbl;
   void * pClientCtxt;
};

#define DAL_TimerDeviceAttach(DevId,hDalDevice)\
        DAL_DeviceAttachEx(NULL,DevId,DALTIMER_INTERFACE_VERSION,hDalDevice)

static __inline DALResult
DalTimer_Register(DalDeviceHandle * _h, const DALSYSEventHandle  hEvent, uint32  time)
{
   return ((DalTimerHandle *)_h)->pVtbl->Register( _h, hEvent, time);
}
static __inline DALResult
DalTimer_UnRegister(DalDeviceHandle * _h, const DALSYSEventHandle  hEvent)
{
   return ((DalTimerHandle *)_h)->pVtbl->UnRegister( _h, hEvent);
}
static __inline DALResult
DalTimer_GetTimerCount(DalDeviceHandle * _h, void * timer_count, uint32 size)
{
   return ((DalTimerHandle *)_h)->pVtbl->GetTimerCount( _h, timer_count, size);
}
#endif
